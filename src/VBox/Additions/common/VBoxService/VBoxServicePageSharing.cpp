/* $Id: VBoxServicePageSharing.cpp $ */
/** @file
 * VBoxService - Guest page sharing.
 */

/*
 * Copyright (C) 2006-2013 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#ifdef RT_OS_WINDOWS
 #include <Ntstatus.h>
#endif

#include <iprt/assert.h>
#include <iprt/avl.h>
#include <iprt/asm.h>
#include <iprt/env.h>
#include <iprt/file.h>
#include <iprt/getopt.h>
#include <iprt/ldr.h>
#include <iprt/mem.h>
#include <iprt/message.h>
#include <iprt/path.h>
#include <iprt/process.h>
#include <iprt/stream.h>
#include <iprt/string.h>
#include <iprt/semaphore.h>
#include <iprt/system.h>
#include <iprt/thread.h>
#include <iprt/time.h>
#include <VBox/VBoxGuestLib.h>
#include "VBoxServiceInternal.h"
#include "VBoxServiceUtils.h"


/*******************************************************************************
*   Externals                                                                  *
*******************************************************************************/
extern int                  VBoxServiceLogCreate(const char *pszLogFile);
extern void                 VBoxServiceLogDestroy(void);


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/

/** The semaphore we're blocking on. */
static RTSEMEVENTMULTI  g_PageSharingEvent = NIL_RTSEMEVENTMULTI;

/** Generic option indices for page sharing fork arguments. */
enum
{
    VBOXSERVICEPAGESHARINGOPT_FORK = 1000,
    VBOXSERVICEPAGESHARINGOPT_LOG_FILE
};

#if defined(RT_OS_WINDOWS) && !defined(TARGET_NT4)
#include <tlhelp32.h>
#include <psapi.h>
#include <winternl.h>

typedef struct
{
    AVLPVNODECORE   Core;
    HMODULE         hModule;
    char            szFileVersion[16];
    MODULEENTRY32   Info;
} KNOWN_MODULE, *PKNOWN_MODULE;

#define SystemModuleInformation     11

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
    ULONG Section;
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    CHAR FullPathName[RTPATH_MAX];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

typedef NTSTATUS (WINAPI *PFNZWQUERYSYSTEMINFORMATION)(ULONG, PVOID, ULONG, PULONG);
static PFNZWQUERYSYSTEMINFORMATION g_pfnZwQuerySystemInformation = NULL;

static DECLCALLBACK(int) VBoxServicePageSharingEmptyTreeCallback(PAVLPVNODECORE pNode, void *pvUser);

static PAVLPVNODECORE   g_pKnownModuleTree = NULL;
static uint64_t         g_idSession = 0;

static int vboxServicePageSharingRetrieveFileVersion(PKNOWN_MODULE pModule)
{
    DWORD dwHandleIgnored;
    DWORD cbVersionSize = GetFileVersionInfoSize(pModule->Info.szExePath,
                                                 &dwHandleIgnored);
    if (!cbVersionSize)
    {
        DWORD dwErr = GetLastError();
        VBoxServiceError("VBoxServicePageSharingRegisterModule: GetFileVersionInfoSize for \"%s\" failed with %ld\n",
                         pModule->Info.szExePath, dwErr);
        return RTErrConvertFromWin32(dwErr);
    }

    BYTE *pVersionInfo = (BYTE *)RTMemAlloc(cbVersionSize);
    if (!pVersionInfo)
        return VERR_NO_MEMORY;

    int rc = VINF_SUCCESS;

    do
    {
        if (!GetFileVersionInfo(pModule->Info.szExePath, 0,
                                cbVersionSize, pVersionInfo))
        {
            DWORD dwErr = GetLastError();
            VBoxServiceError("VBoxServicePageSharingRegisterModule: GetFileVersionInfo for \"%s\" failed with %ld\n",
                             pModule->Info.szExePath, dwErr);

            rc = RTErrConvertFromWin32(dwErr);
            break;
        }

        /* Fetch default code page. */
        struct LANGANDCODEPAGE {
            WORD wLanguage;
            WORD wCodePage;
        } *lpTranslate;

        UINT cbTranslate = 0;
        BOOL fRet = VerQueryValue(pVersionInfo, TEXT("\\VarFileInfo\\Translation"),
                                 (LPVOID *)&lpTranslate, &cbTranslate);
        if (    !fRet
            ||  cbTranslate < 4)
        {
            DWORD dwErr = GetLastError();
            VBoxServiceError("VBoxServicePageSharingRegisterModule: VerQueryValue for \"%s\" failed with %ld (cbTranslate=%ld)\n",
                             pModule->Info.szExePath, dwErr, cbTranslate);

            rc = RTErrConvertFromWin32(dwErr);
            break;
        }

        UINT     cbFileVersion;
        char    *lpszFileVersion;
        unsigned cTranslationBlocks = cbTranslate / sizeof(struct LANGANDCODEPAGE);

        unsigned i = 0;
        for(i; i < cTranslationBlocks; i++)
        {
            /* Fetch file version string. */
            char szFileVersionLocation[RTPATH_MAX];

            if (!RTStrPrintf(szFileVersionLocation, sizeof(szFileVersionLocation),
                             TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage))
            {
                VBoxServiceError("VBoxServicePageSharingRegisterModule: Getting file version for \"%s\" failed\n",
                                 pModule->Info.szExePath);
                rc = VERR_NO_MEMORY;
                break;
            }
            fRet = VerQueryValue(pVersionInfo, szFileVersionLocation, (LPVOID *)&lpszFileVersion, &cbFileVersion);
            if (fRet) /* Value found? */
                break;
        }

        if (RT_FAILURE(rc))
            break;

        if (i == cTranslationBlocks)
        {
            VBoxServiceVerbose(3, "VBoxServicePageSharingRegisterModule: No file version found!\n");
            break;
        }

        if (!RTStrPrintf(pModule->szFileVersion, sizeof(pModule->szFileVersion), "%s", lpszFileVersion))
        {
            VBoxServiceError("VBoxServicePageSharingRegisterModule: Storing file version for \"%s\" failed\n",
                             pModule->Info.szExePath);
            rc = VERR_NO_MEMORY;
            break;
        }
        pModule->szFileVersion[RT_ELEMENTS(pModule->szFileVersion) - 1] = 0;

    } while (0);

    RTMemFree(pVersionInfo);

    return rc;
}

/**
 * Registers a new module with the VMM
 * @param   pModule         Module ptr
 * @param   fValidateMemory Validate/touch memory pages or not
 */
static int vboxServicePageSharingRegisterModule(PKNOWN_MODULE pModule, bool fValidateMemory)
{
    AssertPtrReturn(pModule, VERR_INVALID_POINTER);

    VMMDEVSHAREDREGIONDESC   aRegions[VMMDEVSHAREDREGIONDESC_MAX];
    DWORD                    dwModuleSize = pModule->Info.modBaseSize;
    BYTE                    *pBaseAddress = pModule->Info.modBaseAddr;

    int rc = vboxServicePageSharingRetrieveFileVersion(pModule);
    if (RT_FAILURE(rc))
        return rc;

    unsigned idxRegion = 0;

    if (fValidateMemory)
    {
        do
        {
            MEMORY_BASIC_INFORMATION MemInfo;
            RT_ZERO(MemInfo);
            SIZE_T cbRet = VirtualQuery(pBaseAddress, &MemInfo, sizeof(MemInfo));
            Assert(cbRet);
            if (!cbRet)
            {
                DWORD dwErr = GetLastError();
                VBoxServiceError("VBoxServicePageSharingRegisterModule: VirtualQueryEx failed with error %ld\n",
                                 dwErr);
                rc = RTErrConvertFromWin32(dwErr);
                break;
            }

            if (    MemInfo.State == MEM_COMMIT
                &&  MemInfo.Type  == MEM_IMAGE)
            {
                switch (MemInfo.Protect)
                {

                case PAGE_EXECUTE:
                case PAGE_EXECUTE_READ:
                case PAGE_READONLY:
                {
                    char *pRegion = (char *)MemInfo.BaseAddress;
                    AssertPtr(pRegion);

                    /* Skip the first region as it only contains the image file header. */
                    if (pRegion != (char *)pModule->Info.modBaseAddr)
                    {
                        /* Touch all pages. */
                        while (pRegion < (char *)MemInfo.BaseAddress + MemInfo.RegionSize)
                        {
                            /* Try to trick the optimizer to leave the page touching code in place. */
                            ASMProbeReadByte(pRegion);
                            pRegion += PAGE_SIZE;
                        }
                    }
#ifdef RT_ARCH_X86
                    aRegions[idxRegion].GCRegionAddr = (RTGCPTR32)MemInfo.BaseAddress;
#else
                    aRegions[idxRegion].GCRegionAddr = (RTGCPTR64)MemInfo.BaseAddress;
#endif
                    aRegions[idxRegion].cbRegion     = MemInfo.RegionSize;
                    idxRegion++;

                    break;
                }

                default:
                    break; /* ignore */
                }
            }

            pBaseAddress = (BYTE *)MemInfo.BaseAddress + MemInfo.RegionSize;
            if (dwModuleSize > MemInfo.RegionSize)
            {
                dwModuleSize -= MemInfo.RegionSize;
            }
            else
            {
                dwModuleSize = 0;
                break;
            }

            if (idxRegion >= RT_ELEMENTS(aRegions))
            {
                rc = VERR_BUFFER_OVERFLOW;
                break;  /* Out of room. */
            }
        }
        while (dwModuleSize);
    }
    else
    {
        /* We can't probe kernel memory ranges, so pretend it's one big region. */
#ifdef RT_ARCH_X86
        aRegions[idxRegion].GCRegionAddr = (RTGCPTR32)pBaseAddress;
#else
        aRegions[idxRegion].GCRegionAddr = (RTGCPTR64)pBaseAddress;
#endif
        aRegions[idxRegion].cbRegion     = dwModuleSize;
        idxRegion++;
    }

    VBoxServiceVerbose(3, "VBoxServicePageSharingRegisterModule: VbglR3RegisterSharedModule \"%s\" v%s pBase=0x%p cbSize=%x cntRegions=%u\n",
                       pModule->Info.szModule, pModule->szFileVersion, pModule->Info.modBaseAddr, pModule->Info.modBaseSize, idxRegion);
    /*int rc2 = VbglR3RegisterSharedModule(pModule->Info.szModule, pModule->szFileVersion, (uintptr_t)pModule->Info.modBaseAddr,
                                         pModule->Info.modBaseSize, idxRegion, aRegions);*/
    int rc2 = VINF_SUCCESS;
    if (RT_FAILURE(rc2))
    {
        VBoxServiceVerbose(3, "VBoxServicePageSharingRegisterModule: VbglR3RegisterSharedModule failed with rc=%Rrc\n", rc2);
        if (RT_SUCCESS(rc))
            rc = rc2;
    }

    return rc;
}

/**
 * Inspect all loaded modules for the specified process
 * @param   dwProcessID     Process ID.
 * @param   ppNewTree       Tree where to put new module information into.
 *
 */
int VBoxServicePageSharingInspectModules(DWORD dwProcessID, PAVLPVNODECORE *ppNewTree)
{
    /* Get a list of all the modules in this process. */
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
                                  FALSE /* no child process handle inheritance */, dwProcessID);
    if (hProcess == NULL)
    {
        DWORD dwErr = GetLastError();
        VBoxServiceError("VBoxServicePageSharingInspectModules: OpenProcess %ld failed with %ld\n",
                         dwProcessID, dwErr);
        return RTErrConvertFromWin32(dwErr);
    }

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        DWORD dwErr = GetLastError();
        VBoxServiceError("VBoxServicePageSharingInspectModules: CreateToolhelp32Snapshot failed with %ld\n", dwErr);
        CloseHandle(hProcess);
        return RTErrConvertFromWin32(dwErr);
    }

    int rc = VINF_SUCCESS;

    MODULEENTRY32 ModuleInfo;
    RT_ZERO(ModuleInfo);
    ModuleInfo.dwSize = sizeof(ModuleInfo);

    BOOL fRet = Module32First(hSnapshot, &ModuleInfo);
    if (fRet)
    {
        do
        {
            bool fSkip = false; /* Skip current module? */

            char *pszExt = RTPathExt(ModuleInfo.szModule);
            if (pszExt)
            {
                /** @todo When changing this make sure VBoxService.exe is excluded! */
                if (   !RTStrICmp(pszExt, ".exe")
                    || !RTStrICmp(pszExt, ".com"))
                {
                    fSkip = true; /* Ignore executables for now. */
                }
            }

            VBoxServiceVerbose(4, "VBoxServicePageSharingInspectModules: Module: %s, pszExt=%s, fSkip=%RTbool\n",
                               ModuleInfo.szModule, pszExt ? pszExt : "<None>", fSkip);

            if (fSkip)
                continue;

            /* Found it before? */
            PAVLPVNODECORE pRec = RTAvlPVGet(ppNewTree, ModuleInfo.modBaseAddr);
            if (!pRec)
            {
                pRec = RTAvlPVRemove(&g_pKnownModuleTree, ModuleInfo.modBaseAddr);
                if (!pRec)
                {
                    /* New module; register it. */
                    PKNOWN_MODULE pModule = (PKNOWN_MODULE)RTMemAllocZ(sizeof(KNOWN_MODULE));
                    AssertPtr(pModule);
                    if (!pModule)
                    {
                        rc = VERR_NO_MEMORY;
                        break;
                    }

                    VBoxServiceVerbose(3, "\n\n     MODULE NAME:     %s",           ModuleInfo.szModule );
                    VBoxServiceVerbose(3, "\n     executable     = %s",             ModuleInfo.szExePath );
                    VBoxServiceVerbose(3, "\n     process ID     = 0x%08X",         ModuleInfo.th32ProcessID );
                    VBoxServiceVerbose(3, "\n     base address   = 0x%08X", (DWORD) ModuleInfo.modBaseAddr );
                    VBoxServiceVerbose(3, "\n     base size      = %ld",            ModuleInfo.modBaseSize );

                    pModule->Info     = ModuleInfo;
                    pModule->Core.Key = ModuleInfo.modBaseAddr;
                    pModule->hModule  = LoadLibraryEx(ModuleInfo.szExePath, 0 /* hFile, reserved */,
                                                      DONT_RESOLVE_DLL_REFERENCES);
                    if (pModule->hModule)
                    {
                        rc = vboxServicePageSharingRegisterModule(pModule, true /* Validate pages */);
                        if (RT_FAILURE(rc))
                            VBoxServiceError("VBoxServicePageSharingInspectModules: Failed to register module \"%s\" (Path: %s)",
                                             ModuleInfo.szModule, ModuleInfo.szExePath);
                    }

                    if (RT_SUCCESS(rc))
                        pRec = &pModule->Core;
                    else
                        RTMemFree(pModule);
                }

                if (RT_SUCCESS(rc))
                {
                    fRet = RTAvlPVInsert(ppNewTree, pRec);
                    Assert(fRet); NOREF(fRet);
                }
            }
        }
        while (Module32Next(hSnapshot, &ModuleInfo));
    }

    CloseHandle(hSnapshot);
    CloseHandle(hProcess);

    return rc;
}

/**
 * Inspect all running processes for executables and dlls that might be worth sharing
 * with other VMs.
 *
 */
int VBoxServicePageSharingInspectGuest(void)
{
    PAVLPVNODECORE pNewTree = NULL;
    DWORD dwProcessID = GetCurrentProcessId();

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        /* Check loaded modules for all running processes. */
        PROCESSENTRY32 ProcessInfo;
        RT_ZERO(ProcessInfo);
        ProcessInfo.dwSize = sizeof(PROCESSENTRY32);
        BOOL fRc = Process32First(hSnapshot, &ProcessInfo);
        if (fRc)
        {
            do
            {
                /* Skip our own process. */
                if (ProcessInfo.th32ProcessID != dwProcessID)
                    VBoxServicePageSharingInspectModules(ProcessInfo.th32ProcessID, &pNewTree);
            }
            while (Process32Next(hSnapshot, &ProcessInfo));
        }

        CloseHandle(hSnapshot);
    }
    else
    {
        static int s_iBitchedAboutCreateToolhelp32Snapshot = 0;
        if (s_iBitchedAboutCreateToolhelp32Snapshot++ < 10)
            VBoxServiceError("VBoxServicePageSharingInspectGuest: CreateToolhelp32Snapshot failed with error %ld\n",
                             GetLastError());
        /* Continue getting kernel modules information. */
    }

    int rc = VINF_SUCCESS;

    /* Check all loaded kernel modules. */
    if (g_pfnZwQuerySystemInformation)
    {
        ULONG cbBuffer = 0;
        PVOID pBuffer = NULL;

        do
        {
            NTSTATUS ntRc = g_pfnZwQuerySystemInformation(SystemModuleInformation, (PVOID)&cbBuffer, 0, &cbBuffer);
            if (NT_ERROR(ntRc))
            {
                switch (ntRc)
                {
                    case STATUS_INFO_LENGTH_MISMATCH:
                        if (!cbBuffer)
                        {
                            VBoxServiceError("VBoxServicePageSharingInspectGuest: ZwQuerySystemInformation returned length 0\n");
                            return VERR_NO_MEMORY;
                        }
                        break;

                    default:
                        VBoxServiceError("VBoxServicePageSharingInspectGuest: ZwQuerySystemInformation returned length %u, error %x\n",
                                         cbBuffer, ntRc);
                        rc = RTErrConvertFromNtStatus(ntRc);
                        break;
                }
            }

            pBuffer = RTMemAllocZ(cbBuffer);
            if (!pBuffer)
            {
                rc = VERR_NO_MEMORY;
                break;
            }

            ntRc = g_pfnZwQuerySystemInformation(SystemModuleInformation, pBuffer,
                                                 cbBuffer, &cbBuffer);
            if (NT_ERROR(ntRc))
            {
                VBoxServiceError("VBoxServicePageSharingInspectGuest: ZwQuerySystemInformation returned %x (1)\n", ntRc);
                rc = RTErrConvertFromNtStatus(ntRc);
                break;
            }

            char szSystemDir[RTPATH_MAX];
            if (!GetSystemDirectoryA(szSystemDir, sizeof(szSystemDir)))
            {
                DWORD dwErr = GetLastError();
                VBoxServiceError("VBoxServicePageSharingInspectGuest: Unable to retrieve system directory, error %ld\n", dwErr);
                rc = RTErrConvertFromWin32(dwErr);
                break;
            }

            PRTL_PROCESS_MODULES pSystemModules = (PRTL_PROCESS_MODULES)pBuffer;
            AssertPtr(pSystemModules);
            for (unsigned i = 0; i < pSystemModules->NumberOfModules; i++)
            {
                VBoxServiceVerbose(4, "\n\n   KERNEL  MODULE NAME:     %s",     pSystemModules->Modules[i].FullPathName[pSystemModules->Modules[i].OffsetToFileName] );
                VBoxServiceVerbose(4, "\n     executable     = %s",             pSystemModules->Modules[i].FullPathName );
                VBoxServiceVerbose(4, "\n     flags          = 0x%08X\n",       pSystemModules->Modules[i].Flags);

                /* User-mode modules seem to have no flags set; skip them as we detected them above. */
                if (pSystemModules->Modules[i].Flags == 0)
                    continue;

                /* Found it before? */
                PAVLPVNODECORE pRec = RTAvlPVGet(&pNewTree, pSystemModules->Modules[i].ImageBase);
                if (!pRec)
                {
                    pRec = RTAvlPVRemove(&g_pKnownModuleTree, pSystemModules->Modules[i].ImageBase);
                    if (!pRec)
                    {
                        /* New module; register it. */
                        PKNOWN_MODULE pModule = (PKNOWN_MODULE)RTMemAllocZ(sizeof(KNOWN_MODULE));
                        AssertPtr(pModule);
                        if (!pModule)
                        {
                            rc = VERR_NO_MEMORY;
                            break;
                        }

                        do
                        {
                            const char *pszModule = &pSystemModules->Modules[i].FullPathName[pSystemModules->Modules[i].OffsetToFileName];
                            AssertPtr(pszModule);
                            if (!RTStrPrintf(pModule->Info.szModule, sizeof(pModule->Info.szModule),
                                             "%s", pszModule))
                            {
                                pszModule = pSystemModules->Modules[i].FullPathName;
                                if (!RTStrPrintf(pModule->Info.szModule, sizeof(pModule->Info.szModule),
                                             "%s", pszModule))
                                {
                                    VBoxServiceError("VBoxServicePageSharingInspectGuest: Unable to copy module name of \"%s\" into module info\n",
                                                     pszModule);
                                    rc = VERR_NOT_FOUND;
                                    break;
                                }
                            }

                            /* Skip \SystemRoot\system32 if applicable. */
                            const char *pszCurrentFile = RTStrIStr(pszModule, "\SystemRoot\system32");
                            if (!pszCurrentFile)
                                pszCurrentFile = pszModule; /* ... else use the original file name. */
#ifdef DEBUG
                            VBoxServiceVerbose(3, "VBoxServicePageSharingInspectGuest: pszModule=%s, pszCurrentFile=%s\n",
                                               pszModule, pszCurrentFile ? pszCurrentFile : "<None>");
#endif
                            char szFullFilePath[RTPATH_MAX];
                            if (!RTPathHasPath(pszCurrentFile))
                            {
                                if (!RTStrPrintf(szFullFilePath, sizeof(szFullFilePath), "%s", szSystemDir))
                                {
                                    rc = VERR_NO_MEMORY;
                                    break;
                                }

                                /* Seen just file names in XP; try to locate the file in the system32 and system32\drivers directories. */
                                rc = RTPathAppend(szFullFilePath, sizeof(szFullFilePath), pszCurrentFile);
                                if (RT_FAILURE(rc))
                                    break;

                                VBoxServiceVerbose(3, "Unexpected kernel module name, now trying: %s\n", szFullFilePath);
                                if (!RTFileExists(szFullFilePath)) /* Not in system32, try system32\drivers. */
                                {
                                    if (!RTStrPrintf(szFullFilePath, sizeof(szFullFilePath), "%s", szSystemDir))
                                    {
                                        rc = VERR_NO_MEMORY;
                                        break;
                                    }

                                    rc = RTPathAppend(szFullFilePath, sizeof(szFullFilePath), "drivers");
                                    if (RT_FAILURE(rc))
                                        break;

                                    rc = RTPathAppend(szFullFilePath, sizeof(szFullFilePath), pszCurrentFile);
                                    if (RT_FAILURE(rc))
                                        break;

                                    VBoxServiceVerbose(3, "Unexpected kernel module name, trying: %s\n", szFullFilePath);
                                    if (!RTFileExists(szFullFilePath))
                                    {
                                        VBoxServiceError("Unexpected kernel module name: %s\n",
                                                         pSystemModules->Modules[i].FullPathName);
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                pszCurrentFile = strlen(pszCurrentFile) ? (pszCurrentFile[1], "\\") : NULL;
                                if (!pszCurrentFile)
                                {
                                    VBoxServiceError("Unexpected kernel module name %s (2)\n",
                                                     pSystemModules->Modules[i].FullPathName);
                                    rc = VERR_FILE_NOT_FOUND;
                                    break;
                                }

                                if (!RTStrPrintf(szFullFilePath, sizeof(szFullFilePath), "%s", szSystemDir))
                                {
                                    rc = VERR_NO_MEMORY;
                                    break;
                                }

                                rc = RTPathAppend(szFullFilePath, sizeof(szFullFilePath), pszCurrentFile);
                                if (RT_FAILURE(rc))
                                    break;
                            }

                            if (RTStrPrintf(pModule->Info.szExePath, sizeof(pModule->Info.szExePath),
                                            szFullFilePath))
                            {
                                pModule->Info.modBaseAddr = (BYTE *)pSystemModules->Modules[i].ImageBase;
                                pModule->Info.modBaseSize = pSystemModules->Modules[i].ImageSize;

                                pModule->Core.Key = pSystemModules->Modules[i].ImageBase;

                                VBoxServiceVerbose(3, "\n\n   KERNEL  MODULE NAME:     %s",     pModule->Info.szModule);
                                VBoxServiceVerbose(3, "\n     executable     = %s",             pModule->Info.szExePath);
                                VBoxServiceVerbose(3, "\n     base address   = 0x%08X", (DWORD) pModule->Info.modBaseAddr);
                                VBoxServiceVerbose(3, "\n     flags          = 0x%08X",         pSystemModules->Modules[i].Flags);
                                VBoxServiceVerbose(3, "\n     base size      = %d",             pModule->Info.modBaseSize);

                                rc = vboxServicePageSharingRegisterModule(pModule, false /* Don't check memory pages */);
                            }
                            else
                                rc = VERR_NO_MEMORY;

                        } while (0);

                        if (RT_SUCCESS(rc))
                        {
                            pRec = &pModule->Core;
                        }
                        else if (pModule)
                            RTMemFree(pModule);
                    }

                    if (RT_SUCCESS(rc))
                    {
                        bool fRet= RTAvlPVInsert(&pNewTree, pRec);
                        Assert(fRet); NOREF(fRet);
                    }
                }
            }

        } while (0);

        if (pBuffer)
            RTMemFree(pBuffer);
    }

    /* Delete leftover modules in the old tree. */
    RTAvlPVDestroy(&g_pKnownModuleTree, VBoxServicePageSharingEmptyTreeCallback, NULL);

    /* Check all registered modules. */
    VbglR3CheckSharedModules();

    /* Activate new module tree. */
    g_pKnownModuleTree = pNewTree;

    return VINF_SUCCESS;
}

/**
 * RTAvlPVDestroy callback.
 */
static DECLCALLBACK(int) VBoxServicePageSharingEmptyTreeCallback(PAVLPVNODECORE pNode, void *pvUser)
{
    PKNOWN_MODULE pModule = (PKNOWN_MODULE)pNode;
    bool *pfUnregister = (bool *)pvUser;

    VBoxServiceVerbose(3, "VBoxServicePageSharingEmptyTreeCallback %s %s\n", pModule->Info.szModule, pModule->szFileVersion);

    /* Dereference module in the hypervisor. */
    if (   !pfUnregister
        || *pfUnregister)
    {
        int rc = VbglR3UnregisterSharedModule(pModule->Info.szModule, pModule->szFileVersion,
                                              (uintptr_t)pModule->Info.modBaseAddr, pModule->Info.modBaseSize);
        AssertRC(rc);
    }

    if (pModule->hModule)
        FreeLibrary(pModule->hModule);
    RTMemFree(pNode);
    return 0;
}


#elif TARGET_NT4
int VBoxServicePageSharingInspectGuest(void)
{
    return VERR_NOT_IMPLEMENTED;
}
#else
int VBoxServicePageSharingInspectGuest(void)
{
    return VERR_NOT_IMPLEMENTED;
}
#endif

/** @copydoc VBOXSERVICE::pfnPreInit */
static DECLCALLBACK(int) VBoxServicePageSharingPreInit(void)
{
    return VINF_SUCCESS;
}


/** @copydoc VBOXSERVICE::pfnOption */
static DECLCALLBACK(int) VBoxServicePageSharingOption(const char **ppszShort, int argc, char **argv, int *pi)
{
    NOREF(ppszShort);
    NOREF(argc);
    NOREF(argv);
    NOREF(pi);

    return -1;
}


/** @copydoc VBOXSERVICE::pfnInit */
static DECLCALLBACK(int) VBoxServicePageSharingInit(void)
{
    VBoxServiceVerbose(3, "VBoxServicePageSharingInit\n");

    int rc = RTSemEventMultiCreate(&g_PageSharingEvent);
    AssertRCReturn(rc, rc);

#if defined(RT_OS_WINDOWS) && !defined(TARGET_NT4)
    g_pfnZwQuerySystemInformation = (PFNZWQUERYSYSTEMINFORMATION)RTLdrGetSystemSymbol("ntdll.dll", "ZwQuerySystemInformation");

    rc = VbglR3GetSessionId(&g_idSession);
    if (RT_FAILURE(rc))
    {
        if (rc == VERR_IO_GEN_FAILURE)
            VBoxServiceVerbose(0, "PageSharing: Page sharing support is not available by the host\n");
        else
            VBoxServiceError("VBoxServicePageSharingInit: Failed with rc=%Rrc\n", rc);

        rc = VERR_SERVICE_DISABLED;

        RTSemEventMultiDestroy(g_PageSharingEvent);
        g_PageSharingEvent = NIL_RTSEMEVENTMULTI;

    }
#endif

    return rc;
}

/** @copydoc VBOXSERVICE::pfnWorker */
DECLCALLBACK(int) VBoxServicePageSharingWorker(bool volatile *pfShutdown)
{
    /*
     * Tell the control thread that it can continue
     * spawning services.
     */
    RTThreadUserSignal(RTThreadSelf());

    /*
     * Now enter the loop retrieving runtime data continuously.
     */
    for (;;)
    {
        bool fEnabled = VbglR3PageSharingIsEnabled();
        VBoxServiceVerbose(3, "VBoxServicePageSharingWorker: Enabled=%RTbool\n", fEnabled);

        if (fEnabled)
            VBoxServicePageSharingInspectGuest();

        /*
         * Block for a minute.
         *
         * The event semaphore takes care of ignoring interruptions and it
         * allows us to implement service wakeup later.
         */
        if (*pfShutdown)
            break;
        int rc = RTSemEventMultiWait(g_PageSharingEvent, 60000);
        if (*pfShutdown)
            break;
        if (rc != VERR_TIMEOUT && RT_FAILURE(rc))
        {
            VBoxServiceError("VBoxServicePageSharingWorker: RTSemEventMultiWait failed; rc=%Rrc\n", rc);
            break;
        }
#if defined(RT_OS_WINDOWS) && !defined(TARGET_NT4)
        uint64_t idNewSession = g_idSession;
        rc =  VbglR3GetSessionId(&idNewSession);
        AssertRC(rc);

        if (idNewSession != g_idSession)
        {
            bool fUnregister = false;

            VBoxServiceVerbose(3, "VBoxServicePageSharingWorker: VM was restored!!\n");
            /* The VM was restored, so reregister all modules the next time. */
            RTAvlPVDestroy(&g_pKnownModuleTree, VBoxServicePageSharingEmptyTreeCallback, &fUnregister);
            g_pKnownModuleTree = NULL;

            g_idSession = idNewSession;
        }
#endif
    }

    RTSemEventMultiDestroy(g_PageSharingEvent);
    g_PageSharingEvent = NIL_RTSEMEVENTMULTI;

    VBoxServiceVerbose(3, "VBoxServicePageSharingWorker: finished thread\n");
    return 0;
}

#ifdef RT_OS_WINDOWS

/**
 * This gets control when VBoxService is launched with -pagefusionfork by
 * VBoxServicePageSharingWorkerProcess().
 *
 * @returns RTEXITCODE_SUCCESS.
 *
 * @remarks It won't normally return since the parent drops the shutdown hint
 *          via RTProcTerminate().
 */
RTEXITCODE VBoxServicePageSharingInitFork(int argc, char **argv)
{
    static const RTGETOPTDEF s_aOptions[] =
    {
        { "--pagefusionfork",  VBOXSERVICEPAGESHARINGOPT_FORK,        RTGETOPT_REQ_NOTHING },
        { "--logfile",         VBOXSERVICEPAGESHARINGOPT_LOG_FILE,    RTGETOPT_REQ_STRING },
        { "--verbose",         'v',                                   RTGETOPT_REQ_NOTHING }
    };

    int ch;
    RTGETOPTUNION ValueUnion;
    RTGETOPTSTATE GetState;
    RTGetOptInit(&GetState, argc, argv,
                 s_aOptions, RT_ELEMENTS(s_aOptions),
                 1 /*iFirst*/, RTGETOPTINIT_FLAGS_OPTS_FIRST);

    int rc = VINF_SUCCESS;

    while (   (ch = RTGetOpt(&GetState, &ValueUnion))
           && RT_SUCCESS(rc))
    {
        /* For options that require an argument, ValueUnion has received the value. */
        switch (ch)
        {
            case VBOXSERVICEPAGESHARINGOPT_FORK:
                break;

            case VBOXSERVICEPAGESHARINGOPT_LOG_FILE:
                if (!RTStrPrintf(g_szLogFile, sizeof(g_szLogFile), "%s", ValueUnion.psz))
                    return RTMsgErrorExit(RTEXITCODE_FAILURE, "Unable to set logfile name to '%s'",
                                          ValueUnion.psz);
                break;

            case 'v':
                g_cVerbosity++;
                break;

            default:
                return RTMsgErrorExit(RTEXITCODE_SYNTAX, "Unknown command '%s'", ValueUnion.psz);
                break; /* Never reached. */
        }
    }

    if (RT_FAILURE(rc))
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Initialization failed with rc=%Rrc", rc);

    rc = VBoxServiceLogCreate(strlen(g_szLogFile) ? g_szLogFile : NULL);
    if (RT_FAILURE(rc))
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create release log (%s, %Rrc)",
                              strlen(g_szLogFile) ? g_szLogFile : "<None>", rc);

    bool fShutdown = false;
    VBoxServicePageSharingInit();
    VBoxServicePageSharingWorker(&fShutdown);

    VBoxServiceLogDestroy();
    return RTEXITCODE_SUCCESS;
}

/** @copydoc VBOXSERVICE::pfnWorker */
DECLCALLBACK(int) VBoxServicePageSharingWorkerProcess(bool volatile *pfShutdown)
{
    RTPROCESS hProcess = NIL_RTPROCESS;
    int rc;

    /*
     * Tell the control thread that it can continue
     * spawning services.
     */
    RTThreadUserSignal(RTThreadSelf());

    /*
     * Now enter the loop retrieving runtime data continuously.
     */
    for (;;)
    {
        bool fEnabled = VbglR3PageSharingIsEnabled();
        VBoxServiceVerbose(3, "VBoxServicePageSharingWorkerProcess: Enabled=%RTbool\n", fEnabled);

        /*
         * Start a 2nd VBoxService process to deal with page fusion as we do
         * not wish to dummy load dlls into this process.  (First load with
         * DONT_RESOLVE_DLL_REFERENCES, 2nd normal -> dll init routines not called!)
         */
        if (    fEnabled
            &&  hProcess == NIL_RTPROCESS)
        {
            char szExeName[256];
            char *pszExeName = RTProcGetExecutablePath(szExeName, sizeof(szExeName));
            if (pszExeName)
            {
                int iOptIdx = 0;
                char const *papszArgs[8];
                papszArgs[iOptIdx++] = pszExeName;
                papszArgs[iOptIdx++] = "--pagefusionfork";

                /* Add same verbose flags as parent process. */
                int rc2 = VINF_SUCCESS;
                char szParmVerbose[32] = { 0 };
                for (int i = 0; i < g_cVerbosity && RT_SUCCESS(rc2); i++)
                {
                    if (i == 0)
                        rc2 = RTStrCat(szParmVerbose, sizeof(szParmVerbose), "-");
                    if (RT_FAILURE(rc2))
                        break;
                    rc2 = RTStrCat(szParmVerbose, sizeof(szParmVerbose), "v");
                }
                if (RT_SUCCESS(rc2))
                    papszArgs[iOptIdx++] = szParmVerbose;

                /* Add log file handling. */
                char szParmLogFile[RTPATH_MAX];
                if (   RT_SUCCESS(rc2)
                    && strlen(g_szLogFile))
                {
                    char *pszLogFile = RTStrDup(g_szLogFile);
                    if (pszLogFile)
                    {
                        char *pszLogExt = NULL;
                        if (RTPathHasExt(pszLogFile))
                            pszLogExt = RTStrDup(RTPathExt(pszLogFile));
                        RTPathStripExt(pszLogFile);
                        char *pszLogSuffix;
                        if (RTStrAPrintf(&pszLogSuffix, "-pagesharing") < 0)
                        {
                            rc2 = VERR_NO_MEMORY;
                        }
                        else
                        {
                            rc2 = RTStrAAppend(&pszLogFile, pszLogSuffix);
                            if (RT_SUCCESS(rc2) && pszLogExt)
                                rc2 = RTStrAAppend(&pszLogFile, pszLogExt);
                            if (RT_SUCCESS(rc2))
                            {
                                if (!RTStrPrintf(szParmLogFile, sizeof(szParmLogFile),
                                                 "--logfile=%s", pszLogFile))
                                {
                                    rc2 = VERR_BUFFER_OVERFLOW;
                                }
                            }
                            RTStrFree(pszLogSuffix);
                        }
                        if (RT_FAILURE(rc2))
                            VBoxServiceError("Error building logfile string for page sharing fork, rc=%Rrc\n", rc2);
                        if (pszLogExt)
                            RTStrFree(pszLogExt);
                        RTStrFree(pszLogFile);
                    }
                    if (RT_SUCCESS(rc2))
                        papszArgs[iOptIdx++] = szParmLogFile;
                    papszArgs[iOptIdx++] = NULL;
                }
                else
                    papszArgs[iOptIdx++] = NULL;

                rc = RTProcCreate(pszExeName, papszArgs, RTENV_DEFAULT, 0 /* normal child */, &hProcess);
                if (RT_FAILURE(rc))
                    VBoxServiceError("VBoxServicePageSharingWorkerProcess: RTProcCreate %s failed; rc=%Rrc\n", pszExeName, rc);
            }
        }

        /*
         * Block for a minute.
         *
         * The event semaphore takes care of ignoring interruptions and it
         * allows us to implement service wakeup later.
         */
        if (*pfShutdown)
            break;
        rc = RTSemEventMultiWait(g_PageSharingEvent, 60000);
        if (*pfShutdown)
            break;
        if (rc != VERR_TIMEOUT && RT_FAILURE(rc))
        {
            VBoxServiceError("VBoxServicePageSharingWorkerProcess: RTSemEventMultiWait failed; rc=%Rrc\n", rc);
            break;
        }
    }

    if (hProcess != NIL_RTPROCESS)
        RTProcTerminate(hProcess);

    RTSemEventMultiDestroy(g_PageSharingEvent);
    g_PageSharingEvent = NIL_RTSEMEVENTMULTI;

    VBoxServiceVerbose(3, "VBoxServicePageSharingWorkerProcess: finished thread\n");
    return 0;
}

#endif /* RT_OS_WINDOWS */

/** @copydoc VBOXSERVICE::pfnTerm */
static DECLCALLBACK(void) VBoxServicePageSharingTerm(void)
{
    VBoxServiceVerbose(3, "VBoxServicePageSharingTerm\n");
}


/** @copydoc VBOXSERVICE::pfnStop */
static DECLCALLBACK(void) VBoxServicePageSharingStop(void)
{
    RTSemEventMultiSignal(g_PageSharingEvent);
}


/**
 * The 'pagesharing' service description.
 */
VBOXSERVICE g_PageSharing =
{
    /* pszName. */
    "pagesharing",
    /* pszDescription. */
    "Page Sharing",
    /* pszUsage. */
    NULL,
    /* pszOptions. */
    NULL,
    /* methods */
    VBoxServicePageSharingPreInit,
    VBoxServicePageSharingOption,
    VBoxServicePageSharingInit,
#ifdef RT_OS_WINDOWS
    VBoxServicePageSharingWorkerProcess,
#else
    VBoxServicePageSharingWorker,
#endif
    VBoxServicePageSharingStop,
    VBoxServicePageSharingTerm
};
