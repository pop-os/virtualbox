/* $Id: VBoxWinDrvInst.cpp $ */
/** @file
 * VBoxWinDrvInst - Windows driver installation handling.
 */

/*
 * Copyright (C) 2024-2025 Oracle and/or its affiliates.
 *
 * This file is part of VirtualBox base platform packages, as
 * available from https://www.virtualbox.org.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, in version 3 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#ifdef VBOX_WINDRVINST_USE_NT_APIS
# include <iprt/nt/nt-and-windows.h>
#else
# include <iprt/win/windows.h>
#endif
#include <iprt/win/setupapi.h>
#include <newdev.h> /* For INSTALLFLAG_XXX. */
#include <cfgmgr32.h> /* For MAX_DEVICE_ID_LEN. */
#ifdef RT_ARCH_X86
# include <wintrust.h>
# include <softpub.h>
#endif

#include <iprt/assert.h>
#include <iprt/buildconfig.h>
#include <iprt/cdefs.h>
#include <iprt/dir.h>
#include <iprt/env.h>
#include <iprt/file.h>
#include <iprt/ldr.h>
#include <iprt/list.h>
#include <iprt/mem.h>
#include <iprt/once.h>
#include <iprt/path.h>
#include <iprt/stream.h>
#include <iprt/string.h>
#include <iprt/system.h>
#include <iprt/thread.h> /* For RTThreadSleep(). */
#include <iprt/utf16.h>

#include <package-generated.h>
#include "product-generated.h"

#include <VBox/err.h> /* For VERR_PLATFORM_ARCH_NOT_SUPPORTED.*/
#include <VBox/version.h>

#include <VBox/GuestHost/VBoxWinDrvDefs.h>
#include <VBox/GuestHost/VBoxWinDrvInst.h>
#include <VBox/GuestHost/VBoxWinDrvStore.h>

#include "VBoxWinDrvCommon.h"
#include "VBoxWinDrvInstInternal.h"


/*********************************************************************************************************************************
*   Defines                                                                                                                      *
*********************************************************************************************************************************/

/* Defines from newdev.h (WINVER >= _WIN32_WINNT_VISTA). */
#define DIIRFLAG_INF_ALREADY_COPIED             0x00000001
#define DIIRFLAG_FORCE_INF                      0x00000002
#define DIIRFLAG_HW_USING_THE_INF               0x00000004
#define DIIRFLAG_HOTPATCH                       0x00000008
#define DIIRFLAG_NOBACKUP                       0x00000010


/* SetupUninstallOEMInf Flags values. */
#define SUOI_FORCEDELETE                        0x00000001


/*********************************************************************************************************************************
*   Defined Constants And Macros                                                                                                 *
*********************************************************************************************************************************/
/** The magic value for RTFTPSERVERINTERNAL::u32Magic. */
#define VBOXWINDRVINST_MAGIC               UINT32_C(0x20171201)

/** Validates a handle and returns VERR_INVALID_HANDLE if not valid. */
#define VBOXWINDRVINST_VALID_RETURN_RC(hDrvInst, a_rc) \
    do { \
        AssertPtrReturn((hDrvInst), (a_rc)); \
        AssertReturn((hDrvInst)->u32Magic == VBOXWINDRVINST_MAGIC, (a_rc)); \
    } while (0)

/** Validates a handle and returns VERR_INVALID_HANDLE if not valid. */
#define VBOXWINDRVINST_VALID_RETURN(hDrvInst) VBOXWINDRVINST_VALID_RETURN_RC((hDrvInst), VERR_INVALID_HANDLE)

/** Validates a handle and returns (void) if not valid. */
#define VBOXWINDRVINST_VALID_RETURN_VOID(hDrvInst) \
    do { \
        AssertPtrReturnVoid(hDrvInst); \
        AssertReturnVoid((hDrvInst)->u32Magic == VBOXWINDRVINST_MAGIC); \
    } while (0)


/*********************************************************************************************************************************
*   Structures and Typedefs                                                                                                      *
*********************************************************************************************************************************/
#ifdef VBOX_WINDRVINST_USE_NT_APIS
/* ntdll.dll: Only for > NT4. */
typedef NTSTATUS(WINAPI* PFNNTOPENSYMBOLICLINKOBJECT) (PHANDLE LinkHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);
typedef NTSTATUS(WINAPI* PFNNTQUERYSYMBOLICLINKOBJECT) (HANDLE LinkHandle, PUNICODE_STRING LinkTarget, PULONG ReturnedLength);
#endif /* VBOX_WINDRVINST_USE_NT_APIS */
/* newdev.dll: */
typedef BOOL(WINAPI* PFNDIINSTALLDRIVERW) (HWND hwndParent, LPCWSTR InfPath, DWORD Flags, PBOOL NeedReboot);
typedef BOOL(WINAPI* PFNDIUNINSTALLDRIVERW) (HWND hwndParent, LPCWSTR InfPath, DWORD Flags, PBOOL NeedReboot);
typedef BOOL(WINAPI* PFNUPDATEDRIVERFORPLUGANDPLAYDEVICESW) (HWND hwndParent, LPCWSTR HardwareId, LPCWSTR FullInfPath, DWORD InstallFlags, PBOOL bRebootRequired);
/* setupapi.dll: */
typedef VOID(WINAPI* PFNINSTALLHINFSECTIONW) (HWND Window, HINSTANCE ModuleHandle, PCWSTR CommandLine, INT ShowCommand);
typedef BOOL(WINAPI* PFNSETUPCOPYOEMINFW) (PCWSTR SourceInfFileName, PCWSTR OEMSourceMediaLocation, DWORD OEMSourceMediaType, DWORD CopyStyle, PWSTR DestinationInfFileName, DWORD DestinationInfFileNameSize, PDWORD RequiredSize, PWSTR DestinationInfFileNameComponent);
typedef HINF(WINAPI* PFNSETUPOPENINFFILEW) (PCWSTR FileName, PCWSTR InfClass, DWORD InfStyle, PUINT ErrorLine);
typedef VOID(WINAPI* PFNSETUPCLOSEINFFILE) (HINF InfHandle);
typedef BOOL(WINAPI* PFNSETUPDIGETINFCLASSW) (PCWSTR, LPGUID, PWSTR, DWORD, PDWORD);
typedef BOOL(WINAPI* PFNSETUPUNINSTALLOEMINFW) (PCWSTR InfFileName, DWORD Flags, PVOID Reserved);
typedef BOOL(WINAPI *PFNSETUPSETNONINTERACTIVEMODE) (BOOL NonInteractiveFlag);
/* advapi32.dll: */
typedef BOOL(WINAPI *PFNQUERYSERVICESTATUSEX) (SC_HANDLE, SC_STATUS_TYPE, LPBYTE, DWORD, LPDWORD);

/** Function pointer for a general try INF section callback. */
typedef int (*PFNVBOXWINDRVINST_TRYINFSECTION_CALLBACK)(HINF hInf, PCRTUTF16 pwszSection, void *pvCtx);


/*********************************************************************************************************************************
*   Global Variables                                                                                                             *
*********************************************************************************************************************************/
/** Init once structure for run-as-user functions we need. */
DECL_HIDDEN_DATA(RTONCE)                                 g_vboxWinDrvInstResolveOnce              = RTONCE_INITIALIZER;

#ifdef VBOX_WINDRVINST_USE_NT_APIS
/* ntdll.dll: */
DECL_HIDDEN_DATA(PFNNTOPENSYMBOLICLINKOBJECT)            g_pfnNtOpenSymbolicLinkObject = NULL;
DECL_HIDDEN_DATA(PFNNTQUERYSYMBOLICLINKOBJECT)           g_pfnNtQuerySymbolicLinkObject = NULL;
#endif
/* newdev.dll: */
DECL_HIDDEN_DATA(PFNDIINSTALLDRIVERW)                    g_pfnDiInstallDriverW                    = NULL; /* For Vista+ .*/
DECL_HIDDEN_DATA(PFNDIUNINSTALLDRIVERW)                  g_pfnDiUninstallDriverW                  = NULL; /* Since Win 10 version 1703. */
DECL_HIDDEN_DATA(PFNUPDATEDRIVERFORPLUGANDPLAYDEVICESW)  g_pfnUpdateDriverForPlugAndPlayDevicesW  = NULL; /* For < Vista .*/
/* setupapi.dll: */
DECL_HIDDEN_DATA(PFNINSTALLHINFSECTIONW)                 g_pfnInstallHinfSectionW                 = NULL; /* For W2K+. */
DECL_HIDDEN_DATA(PFNSETUPCOPYOEMINFW)                    g_pfnSetupCopyOEMInfW                    = NULL; /* For W2K+. */
DECL_HIDDEN_DATA(PFNSETUPOPENINFFILEW)                   g_pfnSetupOpenInfFileW                   = NULL; /* For W2K+. */
DECL_HIDDEN_DATA(PFNSETUPCLOSEINFFILE)                   g_pfnSetupCloseInfFile                   = NULL; /* For W2K+. */
DECL_HIDDEN_DATA(PFNSETUPDIGETINFCLASSW)                 g_pfnSetupDiGetINFClassW                 = NULL; /* For W2K+. */
DECL_HIDDEN_DATA(PFNSETUPUNINSTALLOEMINFW)               g_pfnSetupUninstallOEMInfW               = NULL; /* For XP+.  */
DECL_HIDDEN_DATA(PFNSETUPSETNONINTERACTIVEMODE)          g_pfnSetupSetNonInteractiveMode          = NULL; /* For W2K+. */
/* advapi32.dll: */
DECL_HIDDEN_DATA(PFNQUERYSERVICESTATUSEX)                g_pfnQueryServiceStatusEx                = NULL; /* For W2K+. */

/**
 * Structure for keeping a single SetupAPI log section.
 */
typedef struct VBOXWINDRVSETUPAPILOGSECT
{
    /** Section (text) data. */
    char    *pszBuf;
    /** Size (in bytes) of \a pszBuf. */
    size_t   cbBuf;
    /** Used for internal accounting. Don't touch. */
    uint64_t offBuf;
} VBOXWINDRVSETUPAPILOGSECT;
/** Pointer to a structure for keeping a single SetupAPI log section. */
typedef VBOXWINDRVSETUPAPILOGSECT *PVBOXWINDRVSETUPAPILOGSECT;

/**
 * Structure for keeping SetupAPI log instance data.
 */
typedef struct VBOXWINDRVSETUPAPILOG
{
    /** Array of log sections.
     *  Not necessarily all sections. */
    PVBOXWINDRVSETUPAPILOGSECT paSections;
    /** Number of sections in \a paSections. */
    unsigned                   cSections;
} VBOXWINDRVSETUPAPILOG;
/** Pointer to a structure for keeping SetupAPI log instance data. */
typedef VBOXWINDRVSETUPAPILOG *PVBOXWINDRVSETUPAPILOG;

/**
 * Structure for keeping the internal Windows driver context.
 */
typedef struct VBOXWINDRVINSTINTERNAL
{
    /** Magic value. */
    uint32_t               u32Magic;
    /** Callback function for logging output. Optional and can be NULL. */
    PFNVBOXWINDRIVERLOGMSG pfnLog;
    /** User-supplied pointer for \a pfnLog. Optional and can be NULL. */
    void                  *pvUser;
    /** Currently set verbosity level. */
    unsigned               uVerbosity;
    /** Number of (logged) warnings. */
    unsigned               cWarnings;
    /** Number of (logged) errors. */
    unsigned               cErrors;
    /** Whether a reboot is needed in order to perform the current (un)installation. */
    bool                   fReboot;
    /** OS version to use. Detected on creation. RTSYSTEM_NT_VERSION_GET_XXX style.
     *  Can be overwritten via VBoxWinDrvInstSetOsVersion(). */
    uint64_t               uOsVer;
    /** Parameters for (un)installation. */
    VBOXWINDRVINSTPARMS    Parms;
    /** Driver store entry to use. */
    PVBOXWINDRVSTORE       pStore;
} VBOXWINDRVINSTINTERNAL;
/** Pointer to an internal Windows driver installation context. */
typedef VBOXWINDRVINSTINTERNAL *PVBOXWINDRVINSTINTERNAL;

/**
 * Structure for holding a single DLL import symbol.
 *
 * Ordinal currently ignored.
 */
typedef struct VBOXWINDRVINSTIMPORTSYMBOL
{
    /** Symbol name. */
    const char *pszSymbol;
    /** Function pointer. */
    void       **pfnFunc;
} VBOXWINDRVINSTIMPORTSYMBOL;


/*********************************************************************************************************************************
*   Prototypes                                                                                                                   *
*********************************************************************************************************************************/
static int vboxWinDrvParmsDetermine(PVBOXWINDRVINSTINTERNAL pCtx, PVBOXWINDRVINSTPARMS pParms, bool fForce);
static int vboxWinDrvInstSetupAPILog(PVBOXWINDRVINSTINTERNAL pCtx, unsigned cLastSections);


/*********************************************************************************************************************************
*   Import tables                                                                                                                *
*********************************************************************************************************************************/

#ifdef VBOX_WINDRVINST_USE_NT_APIS
/* ntdll.dll: */
static VBOXWINDRVINSTIMPORTSYMBOL s_aNtDllImports[] =
{
    { "NtOpenSymbolicLinkObject",  (void **)&g_pfnNtOpenSymbolicLinkObject },
    { "NtQuerySymbolicLinkObject", (void **)&g_pfnNtQuerySymbolicLinkObject }
};
#endif

/* setupapi.dll: */
static VBOXWINDRVINSTIMPORTSYMBOL s_aSetupApiImports[] =
{
    { "InstallHinfSectionW", (void **)&g_pfnInstallHinfSectionW },
    { "SetupCopyOEMInfW", (void **)&g_pfnSetupCopyOEMInfW },
    { "SetupUninstallOEMInfW", (void **)&g_pfnSetupUninstallOEMInfW },
    { "SetupOpenInfFileW", (void **)&g_pfnSetupOpenInfFileW },
    { "SetupCloseInfFile", (void **)&g_pfnSetupCloseInfFile },
    { "SetupDiGetINFClassW", (void **)&g_pfnSetupDiGetINFClassW },
    { "SetupSetNonInteractiveMode", (void **)&g_pfnSetupSetNonInteractiveMode }
};

/* newdev.dll: */
static VBOXWINDRVINSTIMPORTSYMBOL s_aNewDevImports[] =
{
    /* Only for Vista / 2008 Server and up. */
    { "DiInstallDriverW", (void **)&g_pfnDiInstallDriverW },
    { "DiUninstallDriverW", (void **)&g_pfnDiUninstallDriverW },
    /* Anything older (must support Windows 2000). */
    { "UpdateDriverForPlugAndPlayDevicesW", (void **)&g_pfnUpdateDriverForPlugAndPlayDevicesW }
};

/* newdev.dll: */
static VBOXWINDRVINSTIMPORTSYMBOL s_aAdvApi32Imports[] =
{
    /* Only for Windows 2000 and up. */
    { "QueryServiceStatusEx", (void **)&g_pfnQueryServiceStatusEx }
};


/*********************************************************************************************************************************
*   Implementation                                                                                                               *
*********************************************************************************************************************************/

/**
 * Logs message, va_list version.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   enmType             Log type to use.
 * @param   pszFormat           Format string to log.
 * @param   args                va_list to use.
 */
DECLINLINE(void) vboxWinDrvInstLogExV(PVBOXWINDRVINSTINTERNAL pCtx,
                                      VBOXWINDRIVERLOGTYPE enmType, const char *pszFormat, va_list args)
{
    if (!pCtx->pfnLog)
        return;

    char *psz = NULL;
    RTStrAPrintfV(&psz, pszFormat, args);
    AssertPtrReturnVoid(psz);

    pCtx->pfnLog(enmType, psz, pCtx->pvUser);
    RTStrFree(psz);
}

/**
 * Logs message, extended version.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   enmType             Log type to use.
 * @param   pszFormat           Format string to log.
 */
DECLINLINE(void) vboxWinDrvInstLogEx(PVBOXWINDRVINSTINTERNAL pCtx,
                                     VBOXWINDRIVERLOGTYPE enmType, const char *pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    vboxWinDrvInstLogExV(pCtx, enmType, pszFormat, args);
    va_end(args);
}

/**
 * Logs an error message, extended version.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   fIgnore             Whether to ignore the error in the error count or not.
 * @param   pszFormat           Format string to log.
 * @param   args                va_list for \a pszFormat.
 */
DECLINLINE(void) vboxWinDrvInstLogErrorExV(PVBOXWINDRVINSTINTERNAL pCtx, bool fIgnore, const char *pszFormat, va_list args)
{
    vboxWinDrvInstLogExV(pCtx, VBOXWINDRIVERLOGTYPE_ERROR, pszFormat, args);
    if (!fIgnore)
        pCtx->cErrors++;
}

#ifdef RT_ARCH_X86
/**
 * Logs an error message but ignores (skips) the error count.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pszFormat           Format string to log.
 * @param   ...                 Variable arguments for \a pszFormat.
 */
DECLINLINE(void) vboxWinDrvInstLogErrorIgn(PVBOXWINDRVINSTINTERNAL pCtx, const char *pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    vboxWinDrvInstLogErrorExV(pCtx, true /* fIgnore */, pszFormat, args);
    va_end(args);
}

/**
 * Logs an error message, ignores (skips) the error count and returns the given rc for convenience.
 *
 * @returns VBox status code, given by \a rc.
 * @param   pCtx                Windows driver installer context.
 * @param   rc                  rc to return.
 * @param   pszFormat           Format string to log.
 * @param   ...                 Variable arguments for \a pszFormat.
 */
DECLINLINE(int) vboxWinDrvInstLogErrorRetIgn(PVBOXWINDRVINSTINTERNAL pCtx, int rc, const char *pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    vboxWinDrvInstLogErrorExV(pCtx, true /* fIgnore */, pszFormat, args);
    va_end(args);

    return rc;
}
#endif /* RT_ARCH_X86 */

/**
 * Logs an error message.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pszFormat           Format string to log.
 */
DECLINLINE(void) vboxWinDrvInstLogError(PVBOXWINDRVINSTINTERNAL pCtx, const char *pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    vboxWinDrvInstLogErrorExV(pCtx, false /* fIgnore */, pszFormat, args);
    va_end(args);
}

/**
 * Logs a warning message.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pszFormat           Format string to log.
 */
DECLINLINE(void) vboxWinDrvInstLogWarn(PVBOXWINDRVINSTINTERNAL pCtx, const char *pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    vboxWinDrvInstLogExV(pCtx, VBOXWINDRIVERLOGTYPE_WARN, pszFormat, args);
    va_end(args);

    pCtx->cWarnings++;
}

/**
 * Logs an information message.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pszFormat           Format string to log.
 */
DECLINLINE(void) vboxWinDrvInstLogInfo(PVBOXWINDRVINSTINTERNAL pCtx, const char *pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    vboxWinDrvInstLogExV(pCtx, VBOXWINDRIVERLOGTYPE_INFO, pszFormat, args);
    va_end(args);
}

/**
 * Logs a verbose message.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   uVerbosity          Verbosity level to use for logging.
 * @param   pszFormat           Format string to log.
 */
DECLINLINE(void) vboxWinDrvInstLogVerbose(PVBOXWINDRVINSTINTERNAL pCtx, unsigned uVerbosity, const char *pszFormat, ...)
{
    if (uVerbosity <= pCtx->uVerbosity)
    {
        va_list args;
        va_start(args, pszFormat);
        vboxWinDrvInstLogExV(pCtx, VBOXWINDRIVERLOGTYPE_VERBOSE, pszFormat, args);
        va_end(args);
    }
}

/**
 * Logs (and indicates) that a reboot is needed.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pszFormat           Format string to log.
 */
DECLINLINE(void) vboxWinDrvInstLogRebootNeeded(PVBOXWINDRVINSTINTERNAL pCtx, const char *pszFormat, ...)
{
    va_list args;
    va_start(args, pszFormat);
    vboxWinDrvInstLogExV(pCtx, VBOXWINDRIVERLOGTYPE_REBOOT_NEEDED, pszFormat, args);
    va_end(args);
}

/**
 * Logs the last Windows error given via GetLastError().
 *
 * @returns Last Windows error translated into VBox status code.
 * @retval  VERR_INSTALLATION_FAILED if a translation to a VBox status code wasn't possible.
 * @param   pCtx                Windows driver installer context.
 * @param   pszFormat           Format string to log.
 */
DECLINLINE(int) vboxWinDrvInstLogLastError(PVBOXWINDRVINSTINTERNAL pCtx, const char *pszFormat, ...)
{
    DWORD const dwErr = GetLastError();

    va_list args;
    va_start(args, pszFormat);

    char *psz = NULL;
    RTStrAPrintfV(&psz, pszFormat, args);
    AssertPtrReturn(psz, VERR_NO_MEMORY);

    int rc = VERR_INSTALLATION_FAILED;

#ifdef DEBUG_andy
    bool const fAssertMayPanic = RTAssertMayPanic();
    RTAssertSetMayPanic(false);
#endif

    /* Try resolving Setup API errors first (we don't handle those in IPRT). */
    const char *pszErr = VBoxWinDrvSetupApiErrToStr(dwErr);
    if (!pszErr) /* Also ask for special winerr.h codes we don't handle in IPRT. */
        pszErr = VBoxWinDrvWinErrToStr(dwErr);
    if (!pszErr)
        rc = RTErrConvertFromWin32(dwErr);

#ifdef DEBUG_andy
    RTAssertSetMayPanic(fAssertMayPanic);
#endif

    if (pszErr)
        vboxWinDrvInstLogError(pCtx, "%s: %s (%ld / %#x)", psz, pszErr, dwErr, dwErr);
    else
        vboxWinDrvInstLogError(pCtx, "%s: %Rrc (%ld / %#x)", psz, rc, dwErr, dwErr);

    RTStrFree(psz);

    va_end(args);

    return rc;
}

/**
 * Resolves a single symbol of a module (DLL).
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   hMod                Module handle to use.
 * @param   pszSymbol           Name of symbol to resolve.
 * @param   pfnFunc             Where to return the function pointer for resolved symbol on success.
 */
DECLINLINE(int) vboxWinDrvInstInstModResolveSym(PVBOXWINDRVINSTINTERNAL pCtx, RTLDRMOD hMod, const char *pszSymbol,
                                                void **pfnFunc)
{
    int rc = RTLdrGetSymbol(hMod, pszSymbol, pfnFunc);
    if (RT_FAILURE(rc))
    {
        vboxWinDrvInstLogVerbose(pCtx, 1, "Warning: Symbol \"%s\" not found (%Rrc)", pszSymbol, rc);
        *pfnFunc = NULL;
    }

    return rc;
}

/**
 * Resolves symbols of a specific module (DLL).
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pszFilename         Path of module to resolve symbols for.
 * @param   pSymbols            Table of symbols to resolve.
 * @param   cSymbols            Number of symbols within \a pSymbols to resolve.
 */
static DECLCALLBACK(int) vboxWinDrvInstResolveMod(PVBOXWINDRVINSTINTERNAL pCtx,
                                                  const char *pszFilename, VBOXWINDRVINSTIMPORTSYMBOL *pSymbols, size_t cSymbols)
{
    vboxWinDrvInstLogVerbose(pCtx, 1, "Resolving symbols for module \"%s\"  ...", pszFilename);

    RTLDRMOD hMod;
    int rc = RTLdrLoadSystem(pszFilename, true /*fNoUnload*/, &hMod);
    if (RT_SUCCESS(rc))
    {
        for (size_t i = 0; i < cSymbols; i++)
        {
            void *pfnFunc;
            rc = vboxWinDrvInstInstModResolveSym(pCtx, hMod, pSymbols[i].pszSymbol, &pfnFunc);
            if (RT_SUCCESS(rc))
                *pSymbols[i].pfnFunc = pfnFunc;
        }

        RTLdrClose(hMod);
    }
    else
        vboxWinDrvInstLogError(pCtx, "Unabled to load module \"%s\" (%Rrc)", pszFilename, rc);

    return rc;
}

/**
 * Initialize the import APIs for run-as-user and special environment support.
 *
 * @returns VBox status code.
 * @param   pvUser              Pointer to VBOXWINDRVINSTINTERNAL.
 */
static DECLCALLBACK(int) vboxWinDrvInstResolveOnce(void *pvUser)
{
    PVBOXWINDRVINSTINTERNAL pCtx = (PVBOXWINDRVINSTINTERNAL)pvUser;

    /*
     * Note: Any use of Difx[app|api].dll imports is forbidden (and also marked as being deprecated since Windows 10)!
     */
#ifdef VBOX_WINDRVINST_USE_NT_APIS
    /* rc ignored, keep going */ vboxWinDrvInstResolveMod(pCtx, "ntdll.dll",
                                                          s_aNtDllImports, RT_ELEMENTS(s_aNtDllImports));
#endif
    /* rc ignored, keep going */ vboxWinDrvInstResolveMod(pCtx, "setupapi.dll",
                                                          s_aSetupApiImports, RT_ELEMENTS(s_aSetupApiImports));
    /* rc ignored, keep going */ vboxWinDrvInstResolveMod(pCtx, "newdev.dll",
                                                          s_aNewDevImports, RT_ELEMENTS(s_aNewDevImports));
    /* rc ignored, keep going */ vboxWinDrvInstResolveMod(pCtx, "advapi32.dll",
                                                          s_aAdvApi32Imports, RT_ELEMENTS(s_aAdvApi32Imports));
    return VINF_SUCCESS;
}

/**
 * Initializes a driver installation parameters structure.
 *
 * @param   pParms              Installation parameters structure to initialize.
 */
static void vboxWinDrvInstParmsInit(PVBOXWINDRVINSTPARMS pParms)
{
    RT_BZERO(pParms, sizeof(VBOXWINDRVINSTPARMS));
}

/**
 * Destroys a driver installation parameters structure.
 *
 * @param   pParms              Installation parameters structure to destroy.
 */
static void vboxWinDrvInstParmsDestroy(PVBOXWINDRVINSTPARMS pParms)
{
    switch (pParms->enmMode)
    {
        case VBOXWINDRVINSTMODE_INSTALL:
        case VBOXWINDRVINSTMODE_UNINSTALL:
        {
            RTUtf16Free(pParms->u.UnInstall.pwszModel);
            pParms->u.UnInstall.pwszModel = NULL;
            RTUtf16Free(pParms->u.UnInstall.pwszPnpId);
            pParms->u.UnInstall.pwszPnpId = NULL;
            RTUtf16Free(pParms->u.UnInstall.pwszSection);
            pParms->u.UnInstall.pwszSection = NULL;
            break;
        }

        case VBOXWINDRVINSTMODE_INSTALL_INFSECTION:
        case VBOXWINDRVINSTMODE_UNINSTALL_INFSECTION:
        {
            RTUtf16Free(pParms->u.ExecuteInf.pwszSection);
            pParms->u.ExecuteInf.pwszSection = NULL;
            break;
        }

        case VBOXWINDRVINSTMODE_INVALID:
            break;

        default:
            AssertFailed();
            break;
    }

    RTUtf16Free(pParms->pwszInfFile);
    pParms->pwszInfFile = NULL;
}

/**
 * Structure for keeping callback data for vboxDrvInstExecuteInfFileCallback().
 */
typedef struct VBOXDRVINSTINFCALLBACKCTX
{
    /** Pointer to driver installer instance. */
    PVBOXWINDRVINSTINTERNAL pThis;
    /** Weak pointer to INF section being handled. */
    PCRTUTF16               pwszSection;
    /** User-supplied context pointer. */
    void                   *pvSetupContext;
} VBOXDRVINSTINFCALLBACKCTX;
/** Pointer to structure for keeping callback data for vboxDrvInstExecuteInfFileCallback(). */
typedef VBOXDRVINSTINFCALLBACKCTX *PVBOXDRVINSTINFCALLBACKCTX;

/** Callback for SetupInstallFromInfSectionW(). */
RT_C_DECLS_BEGIN /** @todo r=andy Not sure if we have something else to use. */
static UINT WINAPI vboxDrvInstExecuteInfFileCallback(PVOID    pvCtx,
                                                     UINT     uNotification,
                                                     UINT_PTR Param1,
                                                     UINT_PTR Param2)
{
    RT_NOREF(Param2);

    PVBOXDRVINSTINFCALLBACKCTX pCtx = (PVBOXDRVINSTINFCALLBACKCTX)pvCtx;

    vboxWinDrvInstLogVerbose(pCtx->pThis, 4, "Got installation notification %#x", uNotification);

    switch (uNotification)
    {
        case SPFILENOTIFY_NEEDMEDIA:
        {
            PSOURCE_MEDIA_W pSourceMedia = (PSOURCE_MEDIA_W)Param1;
            vboxWinDrvInstLogInfo(pCtx->pThis, "Requesting installation media \"%ls\\%ls\"...",
                                  pSourceMedia->SourcePath, pSourceMedia->SourceFile);
            break;
        }

        case SPFILENOTIFY_STARTCOPY:
        case SPFILENOTIFY_ENDCOPY:
        {
            PFILEPATHS_W pFilePaths = (PFILEPATHS_W)Param1;
            vboxWinDrvInstLogInfo(pCtx->pThis, "%s copying \"%ls\" -> \"%ls\"",
                                  uNotification == SPFILENOTIFY_STARTCOPY
                                  ? "Started" : "Finished", pFilePaths->Source, pFilePaths->Target);
            break;
        }

        case SPFILENOTIFY_RENAMEERROR:
        case SPFILENOTIFY_DELETEERROR:
        case SPFILENOTIFY_COPYERROR:
        {
            PFILEPATHS_W pFilePaths = (PFILEPATHS_W)Param1;
            vboxWinDrvInstLogError(pCtx->pThis, "Rename/Delete/Copy error \"%ls\" -> \"%s\" (%#x)",
                                   pFilePaths->Source, pFilePaths->Target, pFilePaths->Win32Error);
            break;
        }

        case SPFILENOTIFY_TARGETNEWER:
            vboxWinDrvInstLogInfo(pCtx->pThis, "A newer version of the specified file exists on the target");
            break;

        case SPFILENOTIFY_TARGETEXISTS:
            vboxWinDrvInstLogInfo(pCtx->pThis, "A copy of the specified file already exists on the target");
            break;

        default:
            break;
    }

    return SetupDefaultQueueCallbackW(pCtx->pvSetupContext, uNotification, Param1, Param2);;
}
RT_C_DECLS_END

/**
 * Generic function to for probing a list of well-known sections for [un]installation.
 *
 * Due to the nature of INF files this function tries different combinations of decorations (e.g. SectionName[.NTAMD64|.X86])
 * and invokes the given callback for the first found section.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   hInf                Handle of INF file.
 * @param   pwszSection         Section to invoke for [un]installation.
 *                              If NULL, the "DefaultInstall" / "DefaultUninstall" section will be tried.
 * @param   pfnCallback         Callback to invoke for each found section.
 */
static int vboxWinDrvTryInfSectionEx(PVBOXWINDRVINSTINTERNAL pCtx, HINF hInf, PCRTUTF16 pwszSection,
                                     PFNVBOXWINDRVINST_TRYINFSECTION_CALLBACK pfnCallback)
{
    if (pwszSection)
        vboxWinDrvInstLogVerbose(pCtx, 1, "Trying section \"%ls\"", pwszSection);

    /* Sorted by most likely-ness. */
    PCRTUTF16 apwszTryInstallSections[] =
    {
        /* The more specific (using decorations), the better. Try these first. Might be NULL. */
        pwszSection,
        /* The Default[Un]Install sections apply to primitive (and legacy) drivers. */
           pCtx->Parms.enmMode == VBOXWINDRVINSTMODE_INSTALL
        ?  L"DefaultInstall" : L"DefaultUninstall"
    };

    PCRTUTF16 apwszTryInstallDecorations[] =
    {
        /* No decoration. Try that first. */
        L"",
        /* Native architecture. */
        L"" VBOXWINDRVINF_DOT_NT_NATIVE_ARCH_STR
    };

    int rc = VERR_NOT_FOUND;

    for (size_t i = 0; i < RT_ELEMENTS(apwszTryInstallSections); i++)
    {
        PCRTUTF16 const pwszTrySection = apwszTryInstallSections[i];
        if (!pwszTrySection)
            continue;

        for (size_t d = 0; d < RT_ELEMENTS(apwszTryInstallDecorations); d++)
        {
            RTUTF16 wszTrySection[64];
            rc = RTUtf16Copy(wszTrySection, sizeof(wszTrySection), pwszTrySection);
            AssertRCBreak(rc);
            rc = RTUtf16Cat(wszTrySection, sizeof(wszTrySection), apwszTryInstallDecorations[d]);
            AssertRCBreak(rc);

            rc = pfnCallback(hInf, wszTrySection, pCtx /* pvCtx */);
            if (RT_SUCCESS(rc))
                break;

            if (rc == VERR_FILE_NOT_FOUND) /* File gone already. */
            {
                rc = VINF_SUCCESS;
                break;
            }

            if (rc != VERR_NOT_FOUND)
                vboxWinDrvInstLogError(pCtx, "Trying INF section failed with %Rrc", rc);
        }

        if (RT_SUCCESS(rc)) /* Bail out if callback above succeeded. */
            break;
    }

    if (rc == VERR_NOT_FOUND)
    {
        vboxWinDrvInstLogWarn(pCtx, "No matching sections to try found -- buggy driver?");
        rc = VINF_SUCCESS;
    }

    return rc;
}

/**
 * Generic function to for probing a list of well-known sections for [un]installation.
 *
 * Due to the nature of INF files this function tries different combinations of decorations (e.g. SectionName[.NTAMD64|.X86])
 * and invokes the given callback for the first found section.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pwszInfPathAbs      Absolute path of INF file to use for [un]installation.
 * @param   pwszSection         Section to invoke for [un]installation.
 *                              If NULL, the "DefaultInstall" / "DefaultUninstall" section will be tried.
 * @param   pfnCallback         Callback to invoke for each found section.
 */
static int vboxWinDrvTryInfSection(PVBOXWINDRVINSTINTERNAL pCtx, PCRTUTF16 pwszInfPathAbs, PCRTUTF16 pwszSection,
                                   PFNVBOXWINDRVINST_TRYINFSECTION_CALLBACK pfnCallback)
{
    HINF hInf;
    int rc = VBoxWinDrvInfOpen(pwszInfPathAbs, &hInf);
    if (RT_FAILURE(rc))
    {
        vboxWinDrvInstLogError(pCtx, "Unable to open INF file: %Rrc\n", rc);
        return rc;
    }
    vboxWinDrvInstLogVerbose(pCtx, 1, "INF file \"%ls\" opened", pwszInfPathAbs);

    rc = vboxWinDrvTryInfSectionEx(pCtx, hInf, pwszSection, pfnCallback);

    VBoxWinDrvInfClose(hInf);
    vboxWinDrvInstLogVerbose(pCtx, 1, "INF file \"%ls\" closed", pwszInfPathAbs);

    return rc;
}

/**
 * Uninstalls a section of a given INF file.
 *
 * @returns VBox status code.
 * @retval  VERR_NOT_FOUND if the given section has not been found.
 * @param   pCtx                Windows driver installer context.
 * @param   hInf                Handle of INF file.
 * @param   pwszSection         Section within INF file to uninstall.
 *                              Can have a platform decoration (e.g. "Foobar.NTx86").
 */
static int vboxWinDrvUninstallInfSectionEx(PVBOXWINDRVINSTINTERNAL pCtx, HINF hInf, PCRTUTF16 pwszSection)
{
    AssertPtrReturn(pwszSection, VERR_INVALID_POINTER);

    int rc = VINF_SUCCESS;

    vboxWinDrvInstLogInfo(pCtx, "Uninstalling INF section \"%ls\" ...", pwszSection);

    /*
     * Uninstall services (if any).
     */
    RTUTF16 wszSection[64];
    ssize_t const cwchSection = RTUtf16Printf(wszSection, RT_ELEMENTS(wszSection),
                                              "%ls%s", pwszSection, ".Services");
    if (cwchSection > 0)
    {
        /* We always want to be the first service tag in the group order list (if any). */
        DWORD fFlags = SPSVCINST_TAGTOFRONT;
        BOOL fRc = SetupInstallServicesFromInfSectionW(hInf, wszSection, fFlags);
        if (!fRc)
        {
            DWORD const dwErr = GetLastError();
            if (dwErr == ERROR_SECTION_NOT_FOUND)
            {
                vboxWinDrvInstLogVerbose(pCtx, 1, "INF section \"%ls\" not found", wszSection);
                rc = VERR_NOT_FOUND;
            }
            else
            {
                rc = vboxWinDrvInstLogLastError(pCtx, "Could not uninstall INF services section \"%ls\"", wszSection);
                if (rc == VERR_FILE_NOT_FOUND)
                {
                    /* Hint: Getting VERR_FILE_NOT_FOUND here might mean that an old service entry still is dangling around.
                     *       'sc query <service name> won't show this, however.
                     *       Use 'sc delete <service name>' to delete the leftover. */
                    vboxWinDrvInstLogError(pCtx,
                                           "Hint: An old service (SCM) entry might be dangling around.\n"
                                           "Try removing it via 'sc delete <service name>' and try again.");
                }
            }
        }
        else
            vboxWinDrvInstLogInfo(pCtx, "Uninstalling INF services section \"%ls\" successful", wszSection);

    }
    else
    {
        vboxWinDrvInstLogError(pCtx, "Unable to build uninstallation section string");
        rc = VERR_BUFFER_OVERFLOW;
    }

    return rc;
}

/**
 * Installs a section of a given INF file.
 *
 * @returns VBox status code.
 * @retval  VERR_NOT_FOUND if the given section has not been found.
 * @param   pCtx                Windows driver installer context.
 * @param   hInf                Handle of INF file.
 * @param   pwszSection         Section within INF file to install.
 *                              Can have a platform decoration (e.g. "Foobar.NTx86").
 */
static int vboxWinDrvInstallInfSectionEx(PVBOXWINDRVINSTINTERNAL pCtx, HINF hInf, PCRTUTF16 pwszSection)
{
    AssertPtrReturn(pwszSection, VERR_INVALID_POINTER);

    int rc = VINF_SUCCESS;

    vboxWinDrvInstLogInfo(pCtx, "Installing INF section \"%ls\" ...", pwszSection);

    VBOXDRVINSTINFCALLBACKCTX CallbackCtx;
    RT_ZERO(CallbackCtx);
    CallbackCtx.pThis = pCtx;
    CallbackCtx.pwszSection = pwszSection;
    CallbackCtx.pvSetupContext = SetupInitDefaultQueueCallback(NULL);

    BOOL fRc;
    if (!(pCtx->Parms.fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
    {
        /* Contains entries to install via SetupInstallFromInfSectionW() in a particular order.
         * This gives us more flexibility and control compared to the InstallHinfSectionW() fallback below. */
        struct
        {
            UINT  uFlags;
            UINT  uCopyFlags;
            PVOID pCallback;
            PVOID pContext;
        } s_aFromInfSectionInstall[] =
        {
            /* Install files. */
            { SPINST_FILES, SP_COPY_NOSKIP | SP_COPY_IN_USE_NEEDS_REBOOT,
              vboxDrvInstExecuteInfFileCallback, &CallbackCtx },
            /* Install registry directives. */
            { SPINST_REGISTRY, 0 /* uFlags */,
              NULL /* pCallback */, NULL /* pContext */ }
            /** @todo Add more SPINST_ entries as soon as we need those. */
        };

        for (size_t i = 0; i < RT_ELEMENTS(s_aFromInfSectionInstall); i++)
        {
            fRc = SetupInstallFromInfSectionW(NULL, // hWndOwner
                                              hInf,
                                              pwszSection,
                                              s_aFromInfSectionInstall[i].uFlags,
                                              NULL, // RelativeKeyRoot
                                              NULL, // SourceRootPath
                                              s_aFromInfSectionInstall[i].uCopyFlags,
                                              (PSP_FILE_CALLBACK_W)s_aFromInfSectionInstall[i].pCallback,
                                              s_aFromInfSectionInstall[i].pContext,
                                              NULL,  // DeviceInfoSet
                                              NULL); // DeviceInfoData);
            if (!fRc)
            {
                DWORD const dwErr = GetLastError();
                vboxWinDrvInstLogVerbose(pCtx, 1, "SetupInstallFromInfSectionW(%zu) failed (%ld / %#x)", i, dwErr, dwErr);
                break;
            }
        }

        if (!fRc)
        {
            /* As a last resort, try InstallHinfSectionW() ... we hopefully won't get here ever. */
            RTUTF16       wszCmdLine[RTPATH_MAX + 64 /* To cover section name + flags */];
            int     const fFlags = 128 /* Use standard path */ + 0 /* Don't ask for reboot. */;
            RTUtf16Printf(wszCmdLine, RT_ELEMENTS(wszCmdLine), "%ls %d %ls", pwszSection, fFlags, pCtx->Parms.pwszInfFile);
            vboxWinDrvInstLogVerbose(pCtx, 1, "Executing \"InstallHinfSection(%ls)\" ...", wszCmdLine);
            InstallHinfSectionW(NULL, NULL, wszCmdLine, 0);

            /* InstallHinfSectionW() does not provide any return value, so we ASSUME success.
             * The outcome will be logged via setupapi logs though. */
            fRc = TRUE;
        }
    }
    else
        fRc = TRUE;

    if (fRc)
    {
        vboxWinDrvInstLogInfo(pCtx, "Installing INF section \"%ls\" successful", pwszSection);
    }
    else
    {
        DWORD const dwErr = GetLastError();
        /* Seems like newer Windows OSes (seen on Win10) don't like undecorated sections with SetupInstallFromInfSectionW().
         * So ignore this and continue. */
        if (dwErr == ERROR_BADKEY)
            vboxWinDrvInstLogVerbose(pCtx, 1, "Installing INF section \"%ls\" not possible (ERROR_BADKEY), skipping",
                                     pwszSection);
        /* Seen on WinXP: Will return ERROR_INVALID_FLAGS when try to install an INF section which isn't installable. */
        else if (dwErr == ERROR_INVALID_FLAGS)
            vboxWinDrvInstLogVerbose(pCtx, 1, "Installing INF section \"%ls\" not possible (ERROR_INVALID_FLAGS), skipping",
                                     pwszSection);
        else
            rc = vboxWinDrvInstLogLastError(pCtx, "Installing INF section \"%ls\" failed", pwszSection);
    }

    /*
     * Try install services.
     */
    RTUTF16 wszSection[64];
    ssize_t const cwchSection = RTUtf16Printf(wszSection, RT_ELEMENTS(wszSection),
                                              "%ls%ls%s", pwszSection, VBOXWINDRVINF_DECORATION_SEP_UTF16_STR, "Services");
    if (cwchSection > 0)
    {
        /* We always want to be the first service tag in the group order list (if any). */
        DWORD const fFlags = SPSVCINST_TAGTOFRONT;
        if (!(pCtx->Parms.fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
            fRc = SetupInstallServicesFromInfSectionW(hInf, wszSection, fFlags);
        else
            fRc = TRUE;
        if (!fRc)
        {
            DWORD const dwErr = GetLastError();
            if (dwErr == ERROR_SECTION_NOT_FOUND)
            {
                vboxWinDrvInstLogVerbose(pCtx, 1, "INF section \"%ls\" not found, skipping", wszSection);
                rc = VERR_NOT_FOUND;
            }
            else if (dwErr == ERROR_SERVICE_MARKED_FOR_DELETE)
                vboxWinDrvInstLogWarn(pCtx, "Service in INF section \"%ls\" already marked for deletion, skipping", wszSection);
            else if (dwErr == ERROR_SERVICE_CANNOT_ACCEPT_CTRL)
                vboxWinDrvInstLogWarn(pCtx, "Service in INF section \"%ls\" does not accept any control commands (probably in starting/stopping state), skipping", wszSection);
            else
            {
                rc = vboxWinDrvInstLogLastError(pCtx, "Could not install INF services section \"%ls\"", wszSection);
                if (rc == VERR_FILE_NOT_FOUND)
                {
                    /* Hint: Getting VERR_FILE_NOT_FOUND here might mean that an old service entry still is dangling around.
                     *       'sc query <service name> won't show this, however.
                     *       Use 'sc delete <service name>' to delete the leftover. */
                    vboxWinDrvInstLogError(pCtx, "An old service (SCM) entry might be dangling around.");
                    vboxWinDrvInstLogInfo (pCtx, "Try removing it via 'sc delete <service name>' and try again.");
                }
            }
        }
        else
            vboxWinDrvInstLogInfo(pCtx, "Installing INF services section \"%ls\" successful", wszSection);

    }
    else
    {
        vboxWinDrvInstLogError(pCtx, "Unable to build section string");
        rc = VERR_BUFFER_OVERFLOW;
    }

    if (CallbackCtx.pvSetupContext)
    {
        SetupTermDefaultQueueCallback(CallbackCtx.pvSetupContext);
        CallbackCtx.pvSetupContext = NULL;
    }

    return rc;
}

/**
 * Installs a section of a given INF file.
 *
 * Only supported for the VBOXWINDRVINSTMODE_INSTALL_INFSECTION + VBOXWINDRVINSTMODE_UNINSTALL_INFSECTION modes.
 *
 * @returns VBox status code.
 * @retval  VERR_NOT_FOUND if the given section has not been found.
 * @param   pCtx                Windows driver installer context.
 * @param   pParms              Windows driver installation parameters to use.
 */
static int vboxWinDrvInstallInfSection(PVBOXWINDRVINSTINTERNAL pCtx, PVBOXWINDRVINSTPARMS pParms)
{
    AssertReturn(   pParms->enmMode == VBOXWINDRVINSTMODE_INSTALL_INFSECTION
                 || pParms->enmMode == VBOXWINDRVINSTMODE_UNINSTALL_INFSECTION, VERR_INVALID_PARAMETER);

    HINF hInf;
    int rc = VBoxWinDrvInfOpen(pParms->pwszInfFile, &hInf);
    if (RT_FAILURE(rc))
    {
        vboxWinDrvInstLogError(pCtx, "Unable to open INF file: %Rrc\n", rc);
        return rc;
    }

    vboxWinDrvInstLogVerbose(pCtx, 1, "INF file \"%ls\" opened", pParms->pwszInfFile);

    rc = vboxWinDrvInstallInfSectionEx(pCtx, hInf, pParms->u.ExecuteInf.pwszSection);

    VBoxWinDrvInfClose(hInf);
    vboxWinDrvInstLogVerbose(pCtx, 1, "INF file \"%ls\" closed", pParms->pwszInfFile);

    return rc;
}

/**
 * Callback implementation for invoking a section for installation.
 *
 * @returns VBox status code.
 * @param   hInf                Handle of INF file to use.
 * @param   pwszSection         Section to invoke.
 * @param   pvCtx               User-supplied pointer. Usually PVBOXWINDRVINSTINTERNAL.
 */
DECLCALLBACK(int) vboxWinDrvInstallTryInfSectionCallback(HINF hInf, PCRTUTF16 pwszSection, void *pvCtx)
{
    PVBOXWINDRVINSTINTERNAL pCtx = (PVBOXWINDRVINSTINTERNAL)pvCtx;

    return vboxWinDrvInstallInfSectionEx(pCtx, hInf, pwszSection);
}

#ifdef RT_ARCH_X86
/** Static pointer to the internal driver installer handle, used by vboxWinDrvInterceptedWinVerifyTrust(). */
static PVBOXWINDRVINSTINTERNAL s_vboxWinDrvInterceptedWinVerifyTrustCtx = NULL;

/**
 * Interceptor WinVerifyTrust function for SetupApi.dll on Windows 2000, XP,
 * W2K3 and XP64.
 *
 * This crudely modifies the driver verification request from a WHQL/logo driver
 * check to a simple Authenticode check.
 *
 * s_vboxWinDrvInterceptedWinVerifyTrustCtx must be set.
 */
static LONG WINAPI vboxWinDrvInterceptedWinVerifyTrust(HWND hwnd, GUID *pActionId, void *pvData)
{
    AssertPtrReturn(s_vboxWinDrvInterceptedWinVerifyTrustCtx, TRUST_E_SYSTEM_ERROR);

    /*
     * Resolve the real WinVerifyTrust function.
     */
    static decltype(WinVerifyTrust) * volatile s_pfnRealWinVerifyTrust = NULL;
    decltype(WinVerifyTrust) *pfnRealWinVerifyTrust = s_pfnRealWinVerifyTrust;
    if (!pfnRealWinVerifyTrust)
    {
        HMODULE hmod = GetModuleHandleW(L"WINTRUST.DLL");
        if (!hmod)
            hmod = LoadLibraryW(L"WINTRUST.DLL");
        if (!hmod)
        {
            vboxWinDrvInstLogErrorIgn(s_vboxWinDrvInterceptedWinVerifyTrustCtx, "InterceptedWinVerifyTrust: Failed to load wintrust.dll");
            return TRUST_E_SYSTEM_ERROR;
        }
        pfnRealWinVerifyTrust = (decltype(WinVerifyTrust) *)GetProcAddress(hmod, "WinVerifyTrust");
        if (!pfnRealWinVerifyTrust)
        {
            vboxWinDrvInstLogErrorIgn(s_vboxWinDrvInterceptedWinVerifyTrustCtx, "InterceptedWinVerifyTrust: Failed to locate WinVerifyTrust in wintrust.dll");
            return TRUST_E_SYSTEM_ERROR;
        }
        s_pfnRealWinVerifyTrust = pfnRealWinVerifyTrust;
    }

    /*
     * Modify the ID if appropriate.
     */
    static const GUID s_GuidDriverActionVerify       = DRIVER_ACTION_VERIFY;
    static const GUID s_GuidActionGenericChainVerify = WINTRUST_ACTION_GENERIC_CHAIN_VERIFY;
    static const GUID s_GuidActionGenericVerify2     = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    if (pActionId)
    {
        if (memcmp(pActionId, &s_GuidDriverActionVerify, sizeof(*pActionId)) == 0)
        {
            /** @todo don't apply to obvious NT components... */
            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "DRIVER_ACTION_VERIFY: Changing it to WINTRUST_ACTION_GENERIC_VERIFY_V2");
            pActionId = (GUID *)&s_GuidActionGenericVerify2;
        }
        else if (memcmp(pActionId, &s_GuidActionGenericChainVerify, sizeof(*pActionId)) == 0)
            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "WINTRUST_ACTION_GENERIC_CHAIN_VERIFY");
        else if (memcmp(pActionId, &s_GuidActionGenericVerify2, sizeof(*pActionId)) == 0)
            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "WINTRUST_ACTION_GENERIC_VERIFY_V2");
        else
            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "WINTRUST_ACTION_UNKNOWN");
    }

    /*
     * Log the data.
     */
    if (pvData)
    {
        WINTRUST_DATA *pData = (WINTRUST_DATA *)pvData;
        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "                  cbStruct = %ld", pData->cbStruct);
# ifdef DEBUG
        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "                dwUIChoice = %ld", pData->dwUIChoice);
        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "       fdwRevocationChecks = %ld", pData->fdwRevocationChecks);
        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "             dwStateAction = %ld", pData->dwStateAction);
        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "             hWVTStateData = %p", (uintptr_t)pData->hWVTStateData);
# endif
        if (pData->cbStruct >= 7*sizeof(uint32_t))
        {
            switch (pData->dwUnionChoice)
            {
                case WTD_CHOICE_FILE:
                    vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "                     pFile = %p", (uintptr_t)pData->pFile);
                    if (RT_VALID_PTR(pData->pFile))
                    {
                        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "           pFile->cbStruct = %ld", pData->pFile->cbStruct);
# ifndef DEBUG
                        if (pData->pFile->hFile)
# endif
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "              pFile->hFile = %p", (uintptr_t)pData->pFile->hFile);
                        if (RT_VALID_PTR(pData->pFile->pcwszFilePath))
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "      pFile->pcwszFilePath = %ls", pData->pFile->pcwszFilePath);
# ifdef DEBUG
                        else
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "      pFile->pcwszFilePath = %ls", (uintptr_t)pData->pFile->pcwszFilePath);
                        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "     pFile->pgKnownSubject = %p", (uintptr_t)pData->pFile->pgKnownSubject);
# endif
                    }
                    break;

                case WTD_CHOICE_CATALOG:
                    vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "                  pCatalog = %p", (uintptr_t)pData->pCatalog);
                    if (RT_VALID_PTR(pData->pCatalog))
                    {
                        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "            pCat->cbStruct = %ld", pData->pCatalog->cbStruct);
# ifdef DEBUG
                        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "    pCat->dwCatalogVersion = %ld", pData->pCatalog->dwCatalogVersion);
# endif
                        if (RT_VALID_PTR(pData->pCatalog->pcwszCatalogFilePath))
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "pCat->pcwszCatalogFilePath = %ls", pData->pCatalog->pcwszCatalogFilePath);
# ifdef DEBUG
                        else
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "pCat->pcwszCatalogFilePath =  %ls", (uintptr_t)pData->pCatalog->pcwszCatalogFilePath);
# endif
                        if (RT_VALID_PTR(pData->pCatalog->pcwszMemberTag))
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "      pCat->pcwszMemberTag = %ls", pData->pCatalog->pcwszMemberTag);
# ifdef DEBUG
                        else
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "      pCat->pcwszMemberTag = %ls", (uintptr_t)pData->pCatalog->pcwszMemberTag);
# endif
                        if (RT_VALID_PTR(pData->pCatalog->pcwszMemberFilePath))
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, " pCat->pcwszMemberFilePath = %ls", pData->pCatalog->pcwszMemberFilePath);
# ifdef DEBUG
                        else
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, " pCat->pcwszMemberFilePath = %ls", (uintptr_t)pData->pCatalog->pcwszMemberFilePath);
# else
                        if (pData->pCatalog->hMemberFile)
# endif
                            vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "         pCat->hMemberFile = %p", (uintptr_t)pData->pCatalog->hMemberFile);
# ifdef DEBUG
                        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "pCat->pbCalculatedFileHash = %p", (uintptr_t)pData->pCatalog->pbCalculatedFileHash);
                        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "pCat->cbCalculatedFileHash = %ld", pData->pCatalog->cbCalculatedFileHash);
                        vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "    pCat->pcCatalogContext = %p", (uintptr_t)pData->pCatalog->pcCatalogContext);
# endif
                    }
                    break;

                case WTD_CHOICE_BLOB:
                    vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "                     pBlob = %p\n", (uintptr_t)pData->pBlob);
                    break;

                case WTD_CHOICE_SIGNER:
                    vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "                     pSgnr = %p", (uintptr_t)pData->pSgnr);
                    break;

                case WTD_CHOICE_CERT:
                    vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "                     pCert = %p", (uintptr_t)pData->pCert);
                    break;

                default:
                    vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "             dwUnionChoice = %ld", pData->dwUnionChoice);
                    break;
            }
        }
    }

    /*
     * Make the call.
     */
    vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "Calling WinVerifyTrust ...");
    LONG iRet = pfnRealWinVerifyTrust(hwnd, pActionId, pvData);
    vboxWinDrvInstLogVerbose(s_vboxWinDrvInterceptedWinVerifyTrustCtx, 1, "WinVerifyTrust returns %ld", iRet);

    return iRet;
}

/**
 * Installs an WinVerifyTrust interceptor in setupapi.dll on Windows 2000, XP,
 * W2K3 and XP64.
 *
 * This is a very crude hack to lower the WHQL check to just require a valid
 * Authenticode signature by intercepting the verification call.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 */
static int vboxWinDrvInstallWinVerifyTrustInterceptorInSetupApi(PVBOXWINDRVINSTINTERNAL pCtx)
{
    /* Check the version: */
    OSVERSIONINFOW VerInfo = { sizeof(VerInfo) };
    GetVersionExW(&VerInfo);
    if (VerInfo.dwMajorVersion != 5)
    {
        vboxWinDrvInstLogVerbose(pCtx, 1, "OS not supported to intercept WinVerifyTrust, skipping");
        return VERR_NOT_SUPPORTED;
    }

# define PRINT_ERROR_AND_RETURN(a_Msg, ...) \
    return vboxWinDrvInstLogErrorRetIgn(pCtx, VERR_NOT_SUPPORTED, a_Msg, __VA_ARGS__);

    /* The the target module: */
    HMODULE const hModSetupApi = GetModuleHandleW(L"SETUPAPI.DLL");
    if (!hModSetupApi)
    {
        DWORD const dwLastErr = GetLastError();
        PRINT_ERROR_AND_RETURN("Failed to locate SETUPAPI.DLL in the process: %ld / %#x", dwLastErr, dwLastErr);
    }

    /*
     * Find the delayed import table (at least that's how it's done in the RTM).
     */
    IMAGE_DOS_HEADER const *pDosHdr = (IMAGE_DOS_HEADER const *)hModSetupApi;
    IMAGE_NT_HEADERS const *pNtHdrs = (IMAGE_NT_HEADERS const *)(  (uintptr_t)hModSetupApi
                                                                 + (  pDosHdr->e_magic == IMAGE_DOS_SIGNATURE
                                                                    ? pDosHdr->e_lfanew : 0));

    if (pNtHdrs->Signature != IMAGE_NT_SIGNATURE)
        PRINT_ERROR_AND_RETURN("Failed to parse SETUPAPI.DLL for WinVerifyTrust interception: #1");
    if (pNtHdrs->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
        PRINT_ERROR_AND_RETURN("Failed to parse SETUPAPI.DLL for WinVerifyTrust interception: #2");
    if (pNtHdrs->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT)
        PRINT_ERROR_AND_RETURN("Failed to parse SETUPAPI.DLL for WinVerifyTrust interception: #3");

    uint32_t const cbDir = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size;
    if (cbDir < sizeof(IMAGE_DELAYLOAD_DESCRIPTOR))
        PRINT_ERROR_AND_RETURN("Failed to parse SETUPAPI.DLL for WinVerifyTrust interception: #4");
    uint32_t const cbImages = pNtHdrs->OptionalHeader.SizeOfImage;
    if (cbDir >= cbImages)
        PRINT_ERROR_AND_RETURN("Failed to parse SETUPAPI.DLL for WinVerifyTrust interception: #5");
    uint32_t const offDir = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress;
    if (offDir > cbImages - cbDir)
        PRINT_ERROR_AND_RETURN("Failed to parse SETUPAPI.DLL for WinVerifyTrust interception: #6");

    /*
     * Scan the entries looking for wintrust.dll.
     */
    IMAGE_DELAYLOAD_DESCRIPTOR const * const paEntries = (IMAGE_DELAYLOAD_DESCRIPTOR const *)((uintptr_t)hModSetupApi + offDir);
    uint32_t const                           cEntries  = cbDir / sizeof(paEntries[0]);
    for (uint32_t iImp = 0; iImp < cEntries; iImp++)
    {
        const char * const pchRva2Ptr = paEntries[iImp].Attributes.RvaBased ? (const char *)hModSetupApi : (const char *)0;
        const char * const pszDllName = &pchRva2Ptr[paEntries[iImp].DllNameRVA];
        if (RTStrICmpAscii(pszDllName, "WINTRUST.DLL") == 0)
        {
            /*
             * Scan the symbol names.
             */
            //uint32_t const    cbHdrs      = pNtHdrs->OptionalHeader.SizeOfHeaders;
            uint32_t * const  pauNameRvas = (uint32_t  *)&pchRva2Ptr[paEntries[iImp].ImportNameTableRVA];
            uintptr_t * const paIat       = (uintptr_t *)&pchRva2Ptr[paEntries[iImp].ImportAddressTableRVA];
            for (uint32_t iSym = 0; pauNameRvas[iSym] != NULL; iSym++)
            {
                IMAGE_IMPORT_BY_NAME const * const pName = (IMAGE_IMPORT_BY_NAME const *)&pchRva2Ptr[pauNameRvas[iSym]];
                if (RTStrCmp(pName->Name, "WinVerifyTrust") == 0)
                {
                    vboxWinDrvInstLogVerbose(pCtx, 1, "Intercepting WinVerifyTrust for SETUPAPI.DLL (old: %p)", paIat[iSym]);
                    paIat[iSym] = (uintptr_t)vboxWinDrvInterceptedWinVerifyTrust;
                    return 0;
                }
            }
            PRINT_ERROR_AND_RETURN("Failed to parse SETUPAPI.DLL for WinVerifyTrust interception: #9");
        }
    }
    PRINT_ERROR_AND_RETURN("Failed to parse SETUPAPI.DLL for WinVerifyTrust interception: #10");

# undef PRINT_ERROR_AND_RETURN
}
#endif /* RT_ARCH_X86 */

/**
 * Performs the actual driver installation.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pParms              Windows driver installation parameters to use.
 */
static int vboxWinDrvInstallPerform(PVBOXWINDRVINSTINTERNAL pCtx, PVBOXWINDRVINSTPARMS pParms)
{
    int rc = vboxWinDrvParmsDetermine(pCtx, pParms, false /* fForce */);
    if (RT_FAILURE(rc))
        return rc;

    switch (pParms->enmMode)
    {
        case VBOXWINDRVINSTMODE_INSTALL:
        {
            BOOL fRc = FALSE;
            BOOL fReboot = FALSE;

#ifdef RT_ARCH_X86
            s_vboxWinDrvInterceptedWinVerifyTrustCtx = pCtx; /* Hand over the driver installer context. */
            vboxWinDrvInstallWinVerifyTrustInterceptorInSetupApi(pCtx);
#endif
            /*
             * Pre-install driver.
             */
            DWORD dwInstallFlags = 0;

            /* Note: We don't use the following installation method as the default for
             *       Windows >= Vista anymore, as this might cause unrelated (system) device
             *       removal / re-adding, which makes the installation process a lot longer
             *       and can have other side effects.
             *
             *       As a new default we use the block for Windows 2000 below.
             *
             *       However, we leave this code enabled in case it needs testing / re-evaluation.
             *       Needs the OS version set to >= 99 (i.e. --debug-os-ver "99:0"). Intentionally undocumented.
             */
            if (pCtx->uOsVer >= RTSYSTEM_MAKE_NT_VERSION(99, 0, 0))
            {
                if (g_pfnDiInstallDriverW)
                {
                    if (pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_FORCE)
                        dwInstallFlags |= DIIRFLAG_FORCE_INF;

                    vboxWinDrvInstLogVerbose(pCtx, 1, "Using g_pfnDiInstallDriverW(), dwInstallFlags=%#x", dwInstallFlags);

                    if (!(pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
                        fRc = g_pfnDiInstallDriverW(NULL /* hWndParent */, pParms->pwszInfFile, dwInstallFlags, &fReboot);
                    else
                        fRc = TRUE;
                    if (!fRc)
                    {
                        DWORD const dwErr = GetLastError();

                        /*
                         * Work around an error code wich only appears on old(er) Windows Server editions (e.g. 2012 R2 or 2016)
                         * where SetupAPI tells "unable to mark devices that match new inf", which ultimately results in
                         * ERROR_LINE_NOT_FOUND. This probably is because of primitive drivers which don't have a PnP ID set in
                         * the INF file.
                         *
                         * pnputil.exe also gives the same error in the SetupAPI log when handling the very same INF file.
                         *
                         * So skip this error and pretend everything is fine. */
                        if (dwErr == ERROR_LINE_NOT_FOUND)
                            fRc = true;

                        /*
                         * Work around an error which occurs on Windows Vista, where DiInstallDriverW() can't handle
                         * primitive drivers (i.e. [Manufacturer] section is missing). So try installing the detected
                         * INF section in the next block below.
                         */
                        if (dwErr == ERROR_WRONG_INF_TYPE)
                            fRc = true;

                        /* For anything else we want to get notified that something isn't working. */
                        if (!fRc)
                        {
                            switch (dwErr)
                            {
                                case ERROR_AUTHENTICODE_TRUST_NOT_ESTABLISHED:
                                {
                                    /* For silent installs give a clue why this might have failed. */
                                    if (pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_SILENT)
                                        vboxWinDrvInstLogWarn(pCtx, "Silent installation was selected, but required certificates "
                                                                    "were not pre-installed into the Windows drvier store, so "
                                                                    "the installation will be rejected automatically");
                                    RT_FALL_THROUGH();
                                }

                                default:
                                    rc = vboxWinDrvInstLogLastError(pCtx, "DiInstallDriverW() failed");
                                    break;
                            }
                        }
                    }

                    if (fRc)
                        rc = vboxWinDrvTryInfSection(pCtx,
                                                     pParms->pwszInfFile, pParms->u.UnInstall.pwszSection,
                                                     vboxWinDrvInstallTryInfSectionCallback);
                }
                else
                    vboxWinDrvInstLogError(pCtx, "DiInstallDriverW() not available on this platform");

            }
            else /* For Windows 2000 and below. */
            {
                if (pParms->u.UnInstall.pwszPnpId)
                {
                    if (pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_SILENT)
                    {
                        /* Using INSTALLFLAG_NONINTERACTIVE will trigger an invalid parameter error on Windows 2000. */
                        if (pCtx->uOsVer >= RTSYSTEM_MAKE_NT_VERSION(5, 1, 0))
                            dwInstallFlags |= INSTALLFLAG_NONINTERACTIVE;
                        else
                            vboxWinDrvInstLogWarn(pCtx, "This version of Windows does not support silent installs");
                    }

                    if (pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_FORCE)
                        dwInstallFlags |= INSTALLFLAG_FORCE;

                    vboxWinDrvInstLogVerbose(pCtx, 4, "Using g_pfnUpdateDriverForPlugAndPlayDevicesW(), pwszPnpId=%ls, pwszInfFile=%ls, dwInstallFlags=%#x",
                                             pParms->u.UnInstall.pwszPnpId, pParms->pwszInfFile, dwInstallFlags);

                    if (!(pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
                        fRc = g_pfnUpdateDriverForPlugAndPlayDevicesW(NULL /* hWndParent */,
                                                                      pParms->u.UnInstall.pwszPnpId,
                                                                      pParms->pwszInfFile, dwInstallFlags, &fReboot);
                    else
                        fRc = TRUE;

                    if (!fRc)
                    {
                        DWORD const dwErr = GetLastError();
                        switch (dwErr)
                        {
                            case ERROR_NO_SUCH_DEVINST:
                            {
                                vboxWinDrvInstLogInfo(pCtx, "Device (\"%ls\") not found (yet), pre-installing driver ...",
                                                      pParms->u.UnInstall.pwszPnpId);
                                break;
                            }

                            case ERROR_NO_DRIVER_SELECTED:
                            {
                                vboxWinDrvInstLogWarn(pCtx, "Not able to select a driver from the given INF, using given model");
                                break;
                            }

                            case ERROR_AUTHENTICODE_TRUST_NOT_ESTABLISHED:
                            {
                                /* For silent installs give a clue why this might have failed. */
                                if (pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_SILENT)
                                    vboxWinDrvInstLogWarn(pCtx, "Silent installation was selected, but required certificates "
                                                                "were not pre-installed into the Windows drvier store, so "
                                                                "the installation will be rejected automatically");
                                RT_FALL_THROUGH();
                            }

                            default:
                                rc = vboxWinDrvInstLogLastError(pCtx, "Installation(UpdateDriverForPlugAndPlayDevicesW) failed");
                                break;
                        }
                    }
                }

                if (RT_FAILURE(rc))
                    break;

                /*
                 * Pre-install driver.
                 */
                RTUTF16 wszInfFileAbs[RTPATH_MAX] = { 0 };
                LPWSTR  pwszInfFile = NULL;
                if (   GetFullPathNameW(pParms->pwszInfFile, RT_ELEMENTS(wszInfFileAbs), wszInfFileAbs, &pwszInfFile)
                    && pwszInfFile)
                {
                    RTUTF16 wszSrcPathAbs[RTPATH_MAX] = { 0 };
                    rc = RTUtf16CopyEx(wszSrcPathAbs, RT_ELEMENTS(wszSrcPathAbs), wszInfFileAbs,
                                       RTUtf16Len(wszInfFileAbs) - RTUtf16Len(pwszInfFile));
                    if (RT_SUCCESS(rc))
                    {
                        RTUTF16 wszDstPathAbs[RTPATH_MAX] = { 0 };
                        if (!(pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
                            fRc = g_pfnSetupCopyOEMInfW(wszInfFileAbs, wszSrcPathAbs, SPOST_PATH, 0,
                                                        wszDstPathAbs, RT_ELEMENTS(wszDstPathAbs), NULL, NULL);
                        else
                            fRc = TRUE;

                        vboxWinDrvInstLogVerbose(pCtx, 1, "   INF file: %ls", wszInfFileAbs);
                        vboxWinDrvInstLogVerbose(pCtx, 1, "Source path: %ls", wszSrcPathAbs);
                        vboxWinDrvInstLogVerbose(pCtx, 1, "  Dest path: %ls", wszDstPathAbs);

                        if (fRc)
                            vboxWinDrvInstLogInfo(pCtx, "Copying OEM INF successful");
                        else
                        {
                            DWORD const dwErr = GetLastError();
                            switch (dwErr)
                            {
                                /* Will happen on Windows Vista (for VBoxMouse).
                                 * Harmless and can be skipped. */
                                case ERROR_WRONG_INF_TYPE:
                                    vboxWinDrvInstLogWarn(pCtx, "Copying OEM INF file not possible, reported wrong "
                                                                "INF type. Ignoring.");
                                    break;

                                case CERT_E_UNTRUSTEDROOT:
                                    vboxWinDrvInstLogError(pCtx, "Not able to copy OEM INF into the system, as the "
                                                                 "certificate chain terminated in an untrusted root "
                                                                 "certificate. If this is a custom / debug build, "
                                                                 "the required certificates need to be installed into the "
                                                                 "driver store first.");
                                    break;

                                default:
                                    rc = vboxWinDrvInstLogLastError(pCtx, "Installation(SetupCopyOEMInfW) failed");
                                    break;
                            }
                        }
                    }
                }
                else
                    rc = vboxWinDrvInstLogLastError(pCtx, "GetFullPathNameW() failed");

                if (RT_SUCCESS(rc))
                    rc = vboxWinDrvTryInfSection(pCtx,
                                                 pParms->pwszInfFile, pParms->u.UnInstall.pwszSection,
                                                 vboxWinDrvInstallTryInfSectionCallback);
            }

            if (RT_FAILURE(rc))
                break;

            pCtx->fReboot = RT_BOOL(fReboot);
            break;
        }

        case VBOXWINDRVINSTMODE_INSTALL_INFSECTION:
        {
            rc = vboxWinDrvInstallInfSection(pCtx, pParms);
            break;
        }

        default:
            break;
    }

    if (   pCtx->cErrors
        && !(pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
        /* ignore rc */ vboxWinDrvInstSetupAPILog(pCtx, pCtx->uVerbosity <= 1 ? 1 : 3 /* Last sections */);

    return rc;
}

/**
 * Returns whether the given (in)installation parameters are valid or not.
 *
 * @returns \c true if valid, \c false if not.
 * @param   pCtx                Windows driver installer context.
 * @param   pParms              Windows driver (un)installation parameters to validate.
 */
static bool vboxWinDrvParmsAreValid(PVBOXWINDRVINSTINTERNAL pCtx, PVBOXWINDRVINSTPARMS pParms)
{
    if (pParms->u.UnInstall.pwszPnpId)
    {
        size_t const cchPnpId = RTUtf16Len(pParms->u.UnInstall.pwszPnpId);
        if (   !cchPnpId
            || cchPnpId > MAX_DEVICE_ID_LEN)
        {
            vboxWinDrvInstLogVerbose(pCtx, 1, "PnP ID not specified explicitly or invalid");
            return false;
        }
    }

    return true;
}

/**
 * Determines (un)installation parameters from a given set of parameters, logged.
 *
 * @returns VBox status code.
 * @retval  VERR_INVALID_PARAMETER if no valid parameters could be determined.
 * @param   pCtx                Windows driver installer context.
 * @param   pParms              Windows driver installation parameters to determine for.
 * @param   fForce              Whether to overwrite already set parameters or not.
 *
 * @note    Only can deal with the first model / PnP ID found for now.
 */
static int vboxWinDrvParmsDetermine(PVBOXWINDRVINSTINTERNAL pCtx, PVBOXWINDRVINSTPARMS pParms, bool fForce)
{
    int rc;

    /* INF file given? */
    if (pParms->pwszInfFile)
    {
        HINF hInf;
        rc = VBoxWinDrvInfOpen(pParms->pwszInfFile, &hInf);
        if (RT_SUCCESS(rc))
        {
            /* Get the INF type first. */
            PRTUTF16 pwszMainSection;
            VBOXWINDRVINFTYPE enmType = VBoxWinDrvInfGetTypeEx(hInf, &pwszMainSection);
            if (enmType != VBOXWINDRVINFTYPE_INVALID)
            {
                vboxWinDrvInstLogVerbose(pCtx, 1, "INF type is: %s",
                                           enmType == VBOXWINDRVINFTYPE_NORMAL
                                         ? "Normal" : "Primitive");
                /*
                 * Determine model.
                 */
                if (   !pParms->u.UnInstall.pwszModel
                    || fForce)
                {
                    vboxWinDrvInstLogVerbose(pCtx, 1, "Determining model ...");
                    if (fForce)
                    {
                        RTUtf16Free(pParms->u.UnInstall.pwszModel);
                        pParms->u.UnInstall.pwszModel = NULL;
                    }
                    rc = VBoxWinDrvInfQueryFirstModel(hInf, pwszMainSection, &pParms->u.UnInstall.pwszModel);
                    if (RT_SUCCESS(rc))
                    {
                        RTUtf16Free(pParms->u.UnInstall.pwszSection);
                        pParms->u.UnInstall.pwszSection = NULL;

                        /* Now that we have determined the model, try if there is a section in the INF file for this model. */
                        rc = VBoxWinDrvInfQueryInstallSection(hInf, pParms->u.UnInstall.pwszModel,
                                                              &pParms->u.UnInstall.pwszSection);
                        if (RT_FAILURE(rc))
                        {
                            switch (enmType)
                            {
                                case VBOXWINDRVINFTYPE_NORMAL:
                                {
                                    vboxWinDrvInstLogError(pCtx, "No section to install found, can't continue");
                                    break;
                                }

                                case VBOXWINDRVINFTYPE_PRIMITIVE:
                                {
                                    /* If for the given model there is no install section, set the section to main section
                                     * we got when we determined the INF type.
                                     *
                                     * This will be mostly the case for primitive drivers. */
                                    if (rc == VERR_NOT_FOUND)
                                    {
                                        pParms->u.UnInstall.pwszSection = RTUtf16Dup(pwszMainSection);
                                        if (pParms->u.UnInstall.pwszSection)
                                        {
                                            rc = VINF_SUCCESS;
                                        }
                                        else
                                            rc = VERR_NO_MEMORY;
                                    }
                                    break;
                                }

                                default:
                                    AssertFailedStmt(rc = VERR_NOT_IMPLEMENTED);
                                    break;
                            }
                        }
                    }
                    else
                    {
                        switch (rc)
                        {
                            case VERR_PLATFORM_ARCH_NOT_SUPPORTED:
                            {
                                vboxWinDrvInstLogError(pCtx, "Model found, but platform is not supported");
                                break;
                            }

                            case VERR_NOT_FOUND:
                            {
                                vboxWinDrvInstLogError(pCtx, "No model found to install found -- buggy driver?");
                                break;
                            }

                            default:
                                break;
                        }
                    }
                }

                /*
                 * Determine PnP ID.
                 *
                 * Only available in non-primitive drivers.
                 */
                if (   enmType == VBOXWINDRVINFTYPE_NORMAL
                    && (   !pParms->u.UnInstall.pwszPnpId
                        || fForce))
                {
                    if (pParms->u.UnInstall.pwszModel)
                    {
                        vboxWinDrvInstLogVerbose(pCtx, 1, "Determining PnP ID ...");
                        if (fForce)
                        {
                            RTUtf16Free(pParms->u.UnInstall.pwszPnpId);
                            pParms->u.UnInstall.pwszPnpId = NULL;
                        }
                        /* ignore rc */ VBoxWinDrvInfQueryFirstPnPId(hInf,
                                                                     pParms->u.UnInstall.pwszModel, &pParms->u.UnInstall.pwszPnpId);
                    }
                    else
                        vboxWinDrvInstLogVerbose(pCtx, 1, "No first model found/set, skipping determining PnP ID");
                }

                RTUtf16Free(pwszMainSection);
            }
            else
            {
                vboxWinDrvInstLogError(pCtx, "INF file is invalid");
                rc = VERR_INVALID_PARAMETER;
            }

            VBoxWinDrvInfClose(hInf);
        }
    }
    /* No INF file given but either the model or the PnP ID? */
    else if (   pParms->u.UnInstall.pwszModel
             || pParms->u.UnInstall.pwszPnpId)
    {
        /* Nothing to do for us here. */
        rc = VINF_SUCCESS;
    }
    else
    {
        vboxWinDrvInstLogError(pCtx, "Neither INF file nor model/PnP ID given; can't continue");
        rc = VERR_INVALID_PARAMETER;
    }

    if (RT_SUCCESS(rc))
    {
        vboxWinDrvInstLogVerbose(pCtx, 1, "Determined parameters:");
        vboxWinDrvInstLogVerbose(pCtx, 1, "\tINF File: %ls",
                                 pParms->pwszInfFile ? pParms->pwszInfFile : L"<None>");
        vboxWinDrvInstLogVerbose(pCtx, 1, "\t   Model: %ls",
                                 pParms->u.UnInstall.pwszModel ? pParms->u.UnInstall.pwszModel : L"<None>");
        vboxWinDrvInstLogVerbose(pCtx, 1, "\t  PnP ID: %ls",
                                 pParms->u.UnInstall.pwszPnpId ? pParms->u.UnInstall.pwszPnpId : L"<None>");
        vboxWinDrvInstLogVerbose(pCtx, 1, "\t Section: %ls",
                                 pParms->u.UnInstall.pwszSection ? pParms->u.UnInstall.pwszSection : L"<None>");
    }

    return rc;
}

/**
 * Queries OEM INF files from the driver store.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pParms              Windows driver installation parameters to use.
 * @param   ppList              Where to return the list of found Windows driver store entries on success.
 *                              Needs to be destroyed with VBoxWinDrvStoreListFree().
 */
static int vboxWinDrvQueryFromDriverStore(PVBOXWINDRVINSTINTERNAL pCtx, PVBOXWINDRVINSTPARMS pParms,
                                          PVBOXWINDRVSTORELIST *ppList)
{
    int rc = vboxWinDrvParmsDetermine(pCtx, pParms, false /* fForce */);
    if (RT_SUCCESS(rc))
    {
        PVBOXWINDRVSTORELIST pList     = NULL;
        char                *pszNeedle = NULL;

        if (pParms->u.UnInstall.pwszPnpId)
        {
            rc = RTUtf16ToUtf8(pParms->u.UnInstall.pwszPnpId, &pszNeedle);
            if (RT_SUCCESS(rc))
                rc = VBoxWinDrvStoreQueryByPnpId(pCtx->pStore, pszNeedle, &pList);
        }
        else if (pParms->u.UnInstall.pwszModel)
        {
            rc = RTUtf16ToUtf8(pParms->u.UnInstall.pwszModel, &pszNeedle);
            if (RT_SUCCESS(rc))
                rc = VBoxWinDrvStoreQueryByModelName(pCtx->pStore, pszNeedle, &pList);
        }
        else if (pParms->pwszInfFile)
        {
            rc = VERR_NOT_IMPLEMENTED;
        }

        RTStrFree(pszNeedle);
        pszNeedle = NULL;

        if (RT_SUCCESS(rc))
        {
            *ppList = pList;
        }
        else
            VBoxWinDrvStoreListFree(pList);
    }

    return rc;
}

/**
 * Destroys setupapi section entries.
 *
 * @returns VBox status code.
 * @param   pLog                SetupAPI log entries to destroy.
 */
void vboxWinDrvInstSetupAPILogDestroySections(PVBOXWINDRVSETUPAPILOG pLog)
{
    for (unsigned i = 0; i < pLog->cSections; i++)
    {
        if (pLog->paSections[i].pszBuf)
        {
            RTMemFree(pLog->paSections[i].pszBuf);
            pLog->paSections[i].pszBuf = NULL;
        }
    }

    RTMemFree(pLog->paSections);
    pLog->cSections = 0;
}

/**
 * Queries the last N sections (i.e. tail) from a given setupapi.log file.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pszFile             Absolute path to setupapi.log file.
 * @param   cLastSections       Number of last sections to query.
 * @param   pLog                Where to store the result.
 */
static int vboxWinDrvInstSetupAPILogQuerySections(PVBOXWINDRVINSTINTERNAL pCtx, const char *pszFile,
                                                  unsigned cLastSections, PVBOXWINDRVSETUPAPILOG pLog)
{
    RT_NOREF(pCtx);
    AssertReturn(cLastSections, VERR_INVALID_PARAMETER);

    /* We only keep the last N sections in our buffers. */
    unsigned idxSection = 0;
    AssertReturn(pLog->paSections == NULL && pLog->cSections == 0, VERR_INVALID_PARAMETER);
    pLog->paSections = (PVBOXWINDRVSETUPAPILOGSECT)RTMemAllocZ(  cLastSections
                                                               * sizeof(VBOXWINDRVSETUPAPILOGSECT));
    AssertPtrReturn(pLog->paSections, VERR_NO_TMP_MEMORY);
    pLog->cSections = cLastSections;

    int rc = VINF_SUCCESS;

    if (RT_FAILURE(rc))
        return rc;

    RTFILE hFile;
    rc = RTFileOpen(&hFile, pszFile, RTFILE_O_OPEN | RTFILE_O_READ | RTFILE_O_DENY_WRITE);
    if (RT_SUCCESS(rc))
    {
        char szLine[_1K];
        for (;;)
        {
            size_t cbRead;
            rc = RTFileRead(hFile, szLine, sizeof(szLine), &cbRead);
            if (   !cbRead
                || RT_FAILURE(rc))
                break;
            const char *pszEnd = RTStrStr(szLine, "\n");
            size_t const cbLine = pszEnd - szLine + 1 /* Include '\n' */;
            if (   !pszEnd
                || !cbLine)
                break;
            rc = RTFileSeek(hFile, 0 - cbRead + cbLine + 0 /* Skip EOL terminator */, RTFILE_SEEK_CURRENT, NULL);
            AssertRCBreak(rc);
            szLine[cbLine] = '\0';
            const char *pszSectionStart = RTStrStr(szLine, "Section start");
            if (pszSectionStart)
            {
                if (++idxSection >= cLastSections)
                    idxSection = 0;
#ifdef DEBUG
                vboxWinDrvInstLogVerbose(pCtx, 4, "Section@%08RU64: %.*s -> Stored in #%u (%zu)",
                                         RTFileTell(hFile), cbLine - 1 /* Skip '\n' */, szLine, idxSection, pLog->paSections[idxSection].cbBuf);
#endif
                pLog->paSections[idxSection].offBuf = 0;
            }
            if (pLog->paSections[idxSection].offBuf + cbLine + 0 /* Terminator */ >= pLog->paSections[idxSection].cbBuf)
            {
                size_t const cbGrow = _64K;
                pLog->paSections[idxSection].pszBuf =
                                      pLog->paSections[idxSection].pszBuf == NULL
                                    ? (char *)RTMemAlloc(cbGrow)
                                    : (char *)RTMemRealloc(pLog->paSections[idxSection].pszBuf, pLog->paSections[idxSection].cbBuf + cbGrow);
                pLog->paSections[idxSection].cbBuf += cbGrow;
            }
            AssertBreakStmt(pLog->paSections[idxSection].offBuf + cbLine < pLog->paSections[idxSection].cbBuf, rc = VERR_BUFFER_OVERFLOW);
            memcpy(pLog->paSections[idxSection].pszBuf + pLog->paSections[idxSection].offBuf, szLine, cbLine);
#ifdef DEBUG
            vboxWinDrvInstLogVerbose(pCtx, 4, "Section #%u: %.*s", idxSection, cbLine - 1 /* Skip '\n' */, pLog->paSections[idxSection].pszBuf + pLog->paSections[idxSection].offBuf);
#endif
            pLog->paSections[idxSection].offBuf += cbLine;
            pLog->paSections[idxSection].pszBuf[pLog->paSections[idxSection].offBuf] = '\0';
        }

        RTFileClose(hFile);

        if (RT_FAILURE(rc))
            vboxWinDrvInstSetupAPILogDestroySections(pLog);
    }

    return rc;
}

/**
 * Logs the setupapi(.dev).log file to the installation logging instance.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   cLastSections       Number of installation sections to log (i.e. tail).
 */
static int vboxWinDrvInstSetupAPILog(PVBOXWINDRVINSTINTERNAL pCtx, unsigned cLastSections)
{
    int rc;

    /* Note: Don't call vboxWinDrvInstLogError() here to prevent increasing the errror count. */

    RTUTF16 wszWinDir[RTPATH_MAX];
    if (GetWindowsDirectoryW(wszWinDir, RTPATH_MAX))
    {
        char *pszWinDir = NULL;
        rc = RTUtf16ToUtf8(wszWinDir, &pszWinDir);
        if (RT_SUCCESS(rc))
        {
            static const char *s_asLogLocations[] =
            {
                "INF\\setupapi.dev.log",
                /* Combined log (<= Windows XP, including NT4). */
                "setupapi.log",
            };

            for (size_t i = 0; i < RT_ELEMENTS(s_asLogLocations); i++)
            {
                char szSetupAPILog[RTPATH_MAX];
                rc = RTStrCopy(szSetupAPILog, sizeof(szSetupAPILog), pszWinDir);
                if (RT_SUCCESS(rc))
                {
                    rc = RTPathAppend(szSetupAPILog, sizeof(szSetupAPILog), s_asLogLocations[i]);
                    if (RT_SUCCESS(rc))
                    {
                        VBOXWINDRVSETUPAPILOG Log;
                        RT_ZERO(Log);
                        rc = vboxWinDrvInstSetupAPILogQuerySections(pCtx, szSetupAPILog, cLastSections, &Log);
                        if (RT_SUCCESS(rc))
                        {
                            vboxWinDrvInstLogInfo(pCtx, "Output of '%s' (last %u sections):", szSetupAPILog, cLastSections);

                            for (unsigned s = 0; s < Log.cSections; s++)
                            {
                                if (Log.paSections[s].pszBuf)
                                    vboxWinDrvInstLogInfo(pCtx, "%s", Log.paSections[s].pszBuf);
                            }

                            vboxWinDrvInstSetupAPILogDestroySections(&Log);
                        }
                        else if (rc != VERR_FILE_NOT_FOUND) /* Skip non-existing log files. */
                             vboxWinDrvInstLogEx(pCtx, VBOXWINDRIVERLOGTYPE_ERROR,
                                                 "Error processing '%s', rc=%Rrc", szSetupAPILog, rc);

                        vboxWinDrvInstLogVerbose(pCtx, 1, "Processed '%s' with %Rrc", szSetupAPILog, rc);

                        if (rc == VERR_FILE_NOT_FOUND) /* Ditto. */
                            rc = VINF_SUCCESS;
                    }
                }
            }

            RTStrFree(pszWinDir);
            pszWinDir = NULL;
        }
    }
    else
        rc = vboxWinDrvInstLogLastError(pCtx, "Could not determine Windows installation directory");

    if (RT_FAILURE(rc))
        vboxWinDrvInstLogEx(pCtx, VBOXWINDRIVERLOGTYPE_ERROR, "Error retrieving SetupAPI log, rc=%Rrc", rc);

    return rc;
}

/**
 * Logs the setupapi(.dev).log file to the installation logging instance.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   cLastSections       Number of installation sections to log (i.e. tail).
 */
int VBoxWinDrvInstLogSetupAPI(VBOXWINDRVINST hDrvInst, unsigned cLastSections)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN(pCtx);

    return vboxWinDrvInstSetupAPILog(pCtx, cLastSections);
}

/**
 * Callback implementation for invoking a section for uninstallation.
 *
 * @returns VBox status code.
 * @param   hInf                Handle to INF file.
 * @param   pwszSection         Section to invoke.
 * @param   pvCtx               User-supplied pointer. Usually PVBOXWINDRVINSTINTERNAL.
 */
DECLCALLBACK(int) vboxWinDrvUninstallTryInfSectionCallback(HINF hInf, PCRTUTF16 pwszSection, void *pvCtx)
{
    PVBOXWINDRVINSTINTERNAL pCtx = (PVBOXWINDRVINSTINTERNAL)pvCtx;

    return vboxWinDrvUninstallInfSectionEx(pCtx, hInf, pwszSection);
}

/**
 * Removes OEM INF files from the driver store.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pParms              Windows driver uninstallation parameters to use.
 * @param   pList               Driver store list with OEM INF entries to remove.
 */
static int vboxWinDrvUninstallFromDriverStore(PVBOXWINDRVINSTINTERNAL pCtx,
                                              PVBOXWINDRVINSTPARMS pParms, PVBOXWINDRVSTORELIST pList)
{

    int rc = VINF_SUCCESS;

    const char *pszDrvStorePath = VBoxWinDrvStoreBackendGetLocation(pCtx->pStore);

    vboxWinDrvInstLogInfo(pCtx, "Uninstalling %zu matching entr%s", pList->cEntries, pList->cEntries == 1 ? "y" : "ies");
    PVBOXWINDRVSTOREENTRY pCur;
    RTListForEach(&pList->List, pCur, VBOXWINDRVSTOREENTRY, Node)
    {
        bool fRc = FALSE;

        /*
         * Running the uninstalling section(s) first before removing the driver from the driver store below.
         */
        RTUTF16 wszInfPathAbs[RTPATH_MAX];
        ssize_t const cwchInfPathAbs = RTUtf16Printf(wszInfPathAbs, RT_ELEMENTS(wszInfPathAbs),
                                                     "%s\\%ls", pszDrvStorePath, pCur->wszInfFile);
        AssertBreakStmt(cwchInfPathAbs > 0, rc = VERR_BUFFER_OVERFLOW);

        vboxWinDrvInstLogInfo(pCtx, "Uninstalling %ls (%ls)", pCur->wszModel, wszInfPathAbs);

        /* Only calltry calling the "DefaultUninstall" section here, as there aren't any other section(s)
         * to handle for uninstalling a driver via INF files. */
        /* rc ignored, keep going */ vboxWinDrvTryInfSection(pCtx, wszInfPathAbs, NULL /* DefaultUninstall */,
                                                             vboxWinDrvUninstallTryInfSectionCallback);

        /*
         * Remove the driver from the driver store.
         */
        if (g_pfnDiUninstallDriverW)
        {
            vboxWinDrvInstLogVerbose(pCtx, 1, "Using DiUninstallDriverW()");

            BOOL fReboot = FALSE;
            if (!(pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
            {
                /* Takes the fully qualified path of the INF file to uninstall. */
                fRc = g_pfnDiUninstallDriverW(NULL /* hWndParent */, wszInfPathAbs, 0 /* Flags */, &fReboot);
            }
            else
                fRc = TRUE;
            if (fRc)
                pCtx->fReboot = RT_BOOL(fReboot);
            else
            {
                /* Not fatal, try next block. */
                DWORD const dwErr = GetLastError();
                vboxWinDrvInstLogVerbose(pCtx, 1, "DiUninstallDriverW() failed with %#x (%d)", dwErr, dwErr);
            }
        }
        else
            vboxWinDrvInstLogVerbose(pCtx, 1, "DiUninstallDriverW() not available on this platform");

        /* Not (yet) successful? Try harder using an older API. */
        if (   !fRc
            && g_pfnSetupUninstallOEMInfW)
        {
            vboxWinDrvInstLogVerbose(pCtx, 1, "Using SetupUninstallOEMInfW()");

            if (!(pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
            {
                DWORD dwUninstallFlags = 0;
                if (pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_FORCE)
                    dwUninstallFlags |= SUOI_FORCEDELETE;

                /* Takes the oemXXX.inf file (without a path), as found in the Windows INF directory. */
                fRc = g_pfnSetupUninstallOEMInfW(pCur->wszInfFile, dwUninstallFlags, NULL /* pReserved */);
            }
        }

        int rc2 = VINF_SUCCESS;

        if (fRc)
            vboxWinDrvInstLogInfo(pCtx, "Uninstalling OEM INF \"%ls\" successful", wszInfPathAbs);
        else
        {
            DWORD const dwErr = GetLastError();
            if (dwErr == ERROR_INF_IN_USE_BY_DEVICES)
                vboxWinDrvInstLogError(pCtx, "Unable to uninstall OEM INF \"%ls\": Driver still in use by device", wszInfPathAbs);
            else
                rc2 = vboxWinDrvInstLogLastError(pCtx, "Uninstalling OEM INF \"%ls\" failed", wszInfPathAbs);
        }

        /* If anything failed above, try removing stuff ourselves as good as we can. */
        if (RT_FAILURE(rc2))
            /* rc ignored, keep going */ vboxWinDrvTryInfSection(pCtx, wszInfPathAbs, pCur->wszModel,
                                                                 vboxWinDrvUninstallTryInfSectionCallback);

        if (RT_SUCCESS(rc)) /* Keep first error if already set. */
            rc = rc2;

        /* Keep going. */
    }

    return rc;
}

/**
 * Performs the actual driver uninstallation.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pParms              Windows driver installation parameters to use.
 */
static int vboxWinDrvUninstallPerform(PVBOXWINDRVINSTINTERNAL pCtx, PVBOXWINDRVINSTPARMS pParms)
{
    int rc;
    switch (pParms->enmMode)
    {
        case VBOXWINDRVINSTMODE_UNINSTALL:
        {
            PVBOXWINDRVSTORELIST pList = NULL;
            rc = vboxWinDrvQueryFromDriverStore(pCtx, pParms, &pList);
            if (RT_SUCCESS(rc))
            {
                rc = vboxWinDrvUninstallFromDriverStore(pCtx, pParms, pList);

                VBoxWinDrvStoreListFree(pList);
                pList = NULL;
            }
            break;
        }

        case VBOXWINDRVINSTMODE_UNINSTALL_INFSECTION:
        {
            rc = vboxWinDrvInstallInfSection(pCtx, pParms);
            break;
        }

        default:
            rc = VINF_SUCCESS;
            break;
    }

    if (   pCtx->cErrors
        && !(pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
        /* ignore rc */ vboxWinDrvInstSetupAPILog(pCtx, pCtx->uVerbosity <= 1 ? 1 : 3 /* Last sections */);

    return rc;
}

/**
 * Main function to perform the actual driver [un]installation.
 *
 * @returns VBox status code.
 * @param   pCtx                Windows driver installer context.
 * @param   pParms              Windows driver installation parameters to use.
 */
static int vboxWinDrvInstMain(PVBOXWINDRVINSTINTERNAL pCtx, PVBOXWINDRVINSTPARMS pParms)
{
    /* Note: Other parameters might be optional, depending on the mode. */
    AssertReturn(!(pParms->fFlags & ~VBOX_WIN_DRIVERINSTALL_F_VALID_MASK), VERR_INVALID_PARAMETER);

    /* Make sure we start w/o any last errors set. */
    SetLastError(0);

    bool const fInstall = pParms->enmMode == VBOXWINDRVINSTMODE_INSTALL
                       || pParms->enmMode == VBOXWINDRVINSTMODE_INSTALL_INFSECTION;

    if (pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN)
        vboxWinDrvInstLogWarn(pCtx, "Dry-run mode active -- no installation performed!");

    const char * const pszLogAction = fInstall ? "Installing" : "Uninstalling";
    if (pParms->pwszInfFile)
        vboxWinDrvInstLogInfo(pCtx, "%s driver \"%ls\" ... ", pszLogAction, pParms->pwszInfFile);
    else if (pParms->u.UnInstall.pwszModel)
        vboxWinDrvInstLogInfo(pCtx, "%s driver model \"%ls\" ... ", pszLogAction, pParms->u.UnInstall.pwszModel);
    else if (pParms->u.UnInstall.pwszPnpId)
        vboxWinDrvInstLogInfo(pCtx, "%s PnP ID \"%ls\" ... ", pszLogAction, pParms->u.UnInstall.pwszPnpId);

    if (   pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_SILENT
        && g_pfnSetupSetNonInteractiveMode)
    {
        vboxWinDrvInstLogInfo(pCtx, "Setting non-interactive mode ...");
        if (!(pParms->fFlags & VBOX_WIN_DRIVERINSTALL_F_DRYRUN))
            g_pfnSetupSetNonInteractiveMode(TRUE /* fEnable */);
    }

    if (!vboxWinDrvParmsAreValid(pCtx, pParms))
    {
        vboxWinDrvInstLogError(pCtx, "%s parameters are invalid, can't continue", fInstall ? "Installation" : "Uninstallation");
        return VERR_INVALID_PARAMETER;
    }

    int rc;
    if (fInstall)
        rc = vboxWinDrvInstallPerform(pCtx, pParms);
    else
        rc = vboxWinDrvUninstallPerform(pCtx, pParms);

    if (RT_SUCCESS(rc))
    {
        vboxWinDrvInstLogInfo(pCtx, "Driver was %sinstalled successfully", fInstall ? "" : "un");
        if (pCtx->fReboot)
        {
            vboxWinDrvInstLogRebootNeeded(pCtx, "A reboot is needed in order to complete the driver %sinstallation.",
                                          fInstall ? "" : "un");
            rc = VINF_REBOOT_NEEDED;
        }
    }

    /* Note: Call vboxWinDrvInstLogEx() to not increase the error/warn count here. */
    if (pCtx->cErrors)
        vboxWinDrvInstLogEx(pCtx, VBOXWINDRIVERLOGTYPE_ERROR, "%sstalling driver(s) failed with %u errors, %u warnings (rc=%Rrc)",
                            fInstall ? "In" : "Unin", pCtx->cErrors, pCtx->cWarnings, rc);
    else if (pCtx->cWarnings)
        vboxWinDrvInstLogEx(pCtx, VBOXWINDRIVERLOGTYPE_WARN, "%sstalling driver(s) succeeded with %u warnings",
                            fInstall ? "In" : "Unin", pCtx->cWarnings);
    return rc;
}

/**
 * Creates a Windows driver installer instance, extended version.
 *
 * @returns VBox status code.
 * @param   phDrvInst           where to return the created driver installer handle on success.
 * @param   uVerbosity          Sets the initial verbosity level.
 * @param   pfnLog              Log callback function to set.
 * @param   pvUser              User-supplied pointer to set. Optional and might be NULL.
 */
int VBoxWinDrvInstCreateEx(PVBOXWINDRVINST phDrvInst, unsigned uVerbosity, PFNVBOXWINDRIVERLOGMSG pfnLog, void *pvUser)
{
    int rc;

    PVBOXWINDRVINSTINTERNAL pCtx = (PVBOXWINDRVINSTINTERNAL)RTMemAllocZ(sizeof(VBOXWINDRVINSTINTERNAL));
    if (pCtx)
    {
        pCtx->u32Magic   = VBOXWINDRVINST_MAGIC;
        pCtx->uVerbosity = uVerbosity;
        pCtx->pfnLog     = pfnLog;
        pCtx->pvUser     = pvUser;

        /* 1. Detect the Windows version using API calls. */
        pCtx->uOsVer     = RTSystemGetNtVersion(); /* Might be overwritten later via VBoxWinDrvInstSetOsVersion(). */

        /* 2. Detect the Windows from the registry. */
        HKEY hKey;
        LSTATUS lrc = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey);
        if (lrc == ERROR_SUCCESS)
        {
            DWORD dwMaj;
            rc = VBoxWinDrvRegQueryDWORD(hKey, "CurrentMajorVersionNumber", &dwMaj);
            if (RT_SUCCESS(rc))
            {
                DWORD dwMin;
                rc = VBoxWinDrvRegQueryDWORD(hKey, "CurrentMinorVersionNumber", &dwMin);
                if (RT_SUCCESS(rc))
                {
                    uint64_t const uRegOsVer = RTSYSTEM_MAKE_NT_VERSION(dwMaj, dwMin, 0 /* Build, ignored */);

                    /* When running this code in context of an MSI installer, the MSI engine might shim the Windows
                     * version which is being reported via API calls. So compare the both OS versions
                     * and prefer the one being reported via the registry if they don't match.
                     * Ignore the build number (too specific).
                     *
                     * The OS version to use still can be later tweaked using VBoxWinDrvInstSetOsVersion(). */
                    if (   (   RTSYSTEM_NT_VERSION_GET_MAJOR(pCtx->uOsVer) != RTSYSTEM_NT_VERSION_GET_MAJOR(uRegOsVer)
                            || RTSYSTEM_NT_VERSION_GET_MINOR(pCtx->uOsVer) != RTSYSTEM_NT_VERSION_GET_MINOR(uRegOsVer))
                        && RTSYSTEM_NT_VERSION_GET_MAJOR(uRegOsVer) > 4 /* Only XP+ */)
                    {
                        vboxWinDrvInstLogInfo(pCtx, "Detected Windows version (%u.%u) does not match the one stored in the registry (%u.%u)",
                                              RTSYSTEM_NT_VERSION_GET_MAJOR(pCtx->uOsVer),
                                              RTSYSTEM_NT_VERSION_GET_MINOR(pCtx->uOsVer),
                                              RTSYSTEM_NT_VERSION_GET_MAJOR(uRegOsVer),
                                              RTSYSTEM_NT_VERSION_GET_MINOR(uRegOsVer));
                        vboxWinDrvInstLogInfo(pCtx, "This might be due a compatibility layer or MSI installer engine shimming the Windows version");

                        /* Override the OS version from the API with the one found in the registry. */
                        VBoxWinDrvInstSetOsVersion(pCtx, uRegOsVer);
                    }
                }
            }

            RegCloseKey(hKey);
        }
        /* else not fatal. */

        vboxWinDrvInstLogInfo(pCtx, VBOX_PRODUCT " Version " VBOX_VERSION_STRING " - r%s", RTBldCfgRevisionStr());
        vboxWinDrvInstLogInfo(pCtx, "Using Windows version %d.%d.%d (%s)", RTSYSTEM_NT_VERSION_GET_MAJOR(pCtx->uOsVer),
                                                                           RTSYSTEM_NT_VERSION_GET_MINOR(pCtx->uOsVer),
                                                                           RTSYSTEM_NT_VERSION_GET_BUILD(pCtx->uOsVer),
                                                                           RTBldCfgTargetArch());

        rc = RTOnce(&g_vboxWinDrvInstResolveOnce, vboxWinDrvInstResolveOnce, pCtx);
        if (RT_SUCCESS(rc))
        {
            rc = VBoxWinDrvStoreCreate(&pCtx->pStore);
            if (RT_SUCCESS(rc))
            {
                *phDrvInst = (VBOXWINDRVINST)pCtx;
                return VINF_SUCCESS;
            }
            else
                vboxWinDrvInstLogError(pCtx, "Creating driver store failed with %Rrc", rc);
        }
    }
    else
        rc = VERR_NO_MEMORY;

    VBoxWinDrvStoreDestroy(pCtx->pStore);
    VBoxWinDrvInstDestroy(pCtx);
    return rc;
}

/**
 * Creates a Windows driver installer instance.
 *
 * @returns VBox status code.
 * @param   phDrvInst           where to return the created driver installer handle on success.
 */
int VBoxWinDrvInstCreate(PVBOXWINDRVINST phDrvInst)
{
    return VBoxWinDrvInstCreateEx(phDrvInst, 0 /* uVerbosity */, NULL /* pfnLog */, NULL /* pvUser */);
}

/**
 * Destroys a Windows driver installer instance.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to destroy.
 *                              The handle will be invalid after calling this function.
 */
int VBoxWinDrvInstDestroy(VBOXWINDRVINST hDrvInst)
{
    if (hDrvInst == NIL_VBOXWINDRVINST)
        return VINF_SUCCESS;

    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN(pCtx);

    vboxWinDrvInstParmsDestroy(&pCtx->Parms);

    VBoxWinDrvStoreDestroy(pCtx->pStore);
    pCtx->pStore = NULL;

    RTMemFree(pCtx);

    return VINF_SUCCESS;
}

/**
 * Returns the number of (logged) warnings so far.
 *
 * @returns Number of (logged) warnings so far.
 * @param   hDrvInst            Windows driver installer handle.
 */
unsigned VBoxWinDrvInstGetWarnings(VBOXWINDRVINST hDrvInst)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN_RC(pCtx, UINT8_MAX);

    return pCtx->cWarnings;
}

/**
 * Returns the number of (logged) errors so far.
 *
 * @returns Number of (logged) errors so far..
 * @param   hDrvInst            Windows driver installer handle.
 */
unsigned VBoxWinDrvInstGetErrors(VBOXWINDRVINST hDrvInst)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN_RC(pCtx, UINT8_MAX);

    return pCtx->cErrors;
}

/**
 * Sets (overwrites) the current OS version used for the (un)installation code.
 *
 * @param   hDrvInst            Windows driver installer handle.
 * @param   uOsVer              OS version to set. RTSYSTEM_MAKE_NT_VERSION style.
 */
void VBoxWinDrvInstSetOsVersion(VBOXWINDRVINST hDrvInst, uint64_t uOsVer)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN_VOID(pCtx);

    pCtx->uOsVer = uOsVer;

    vboxWinDrvInstLogInfo(pCtx, "Set OS version to: %u.%u", RTSYSTEM_NT_VERSION_GET_MAJOR(pCtx->uOsVer),
                                                            RTSYSTEM_NT_VERSION_GET_MINOR(pCtx->uOsVer));
}

/**
 * Sets the verbosity of a Windows driver installer instance.
 *
 * @returns The old verbosity level.
 * @param   hDrvInst            Windows driver installer handle to set verbosity for.
 * @param   uVerbosity          Verbosity level to set.
 */
unsigned VBoxWinDrvInstSetVerbosity(VBOXWINDRVINST hDrvInst, unsigned uVerbosity)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN_RC(pCtx, UINT8_MAX);

    unsigned const uOldVerbosity = hDrvInst->uVerbosity;
    hDrvInst->uVerbosity = uVerbosity;
    return uOldVerbosity;
}

/**
 * Sets the log callback of a Windows driver installer instance.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to set log callback for.
 * @param   pfnLog              Log callback function to set.
 * @param   pvUser              User-supplied pointer to set. Optional and might be NULL.
 */
void VBoxWinDrvInstSetLogCallback(VBOXWINDRVINST hDrvInst, PFNVBOXWINDRIVERLOGMSG pfnLog, void *pvUser)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN_VOID(pCtx);

    pCtx->pfnLog = pfnLog;
    pCtx->pvUser = pvUser;
}

/**
 * Installs a driver, extended version.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   pszInfFile          INF file to use.
 * @param   pszModel            Model name to use. Optional and can be NULL.
 * @param   pszPnpId            PnP ID to use. NT-style wildcards supported. Optional and can be NULL.
 * @param   fFlags              Installation flags (of type VBOX_WIN_DRIVERINSTALL_F_XXX) to use.
 */
int VBoxWinDrvInstInstallEx(VBOXWINDRVINST hDrvInst,
                            const char *pszInfFile, const char *pszModel, const char *pszPnpId, uint32_t fFlags)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN(pCtx);

    AssertPtrReturn(pszInfFile, VERR_INVALID_PARAMETER);

    vboxWinDrvInstParmsInit(&pCtx->Parms);

    /* Resolve the INF file's absolute path, as as Setup API functions tend to need this. */
    char szInfPathAbs[RTPATH_MAX];
    int rc = RTPathReal(pszInfFile, szInfPathAbs, sizeof(szInfPathAbs));
    if (RT_SUCCESS(rc))
    {
        rc = RTStrToUtf16(szInfPathAbs, &pCtx->Parms.pwszInfFile);
        if (RT_FAILURE(rc))
            vboxWinDrvInstLogError(pCtx, "Failed to build path for INF file \"%s\", rc=%Rrc", pszInfFile, rc);
    }
    else
        vboxWinDrvInstLogError(pCtx, "Determining real path for INF file \"%s\" failed, rc=%Rrc", pszInfFile, rc);

    if (RT_SUCCESS(rc) && pszModel) /* Model is optional. */
        rc = RTStrToUtf16(pszModel, &pCtx->Parms.u.UnInstall.pwszModel);
    if (RT_SUCCESS(rc) && pszPnpId) /* Ditto. */
        rc = RTStrToUtf16(pszPnpId, &pCtx->Parms.u.UnInstall.pwszPnpId);

    if (RT_SUCCESS(rc))
    {
        pCtx->Parms.enmMode = VBOXWINDRVINSTMODE_INSTALL;
        pCtx->Parms.fFlags  = fFlags;

        rc = vboxWinDrvInstMain(pCtx, &pCtx->Parms);
    }

    if (!(fFlags & VBOX_WIN_DRIVERINSTALL_F_NO_DESTROY))
        vboxWinDrvInstParmsDestroy(&pCtx->Parms);

    if (RT_FAILURE(rc))
        vboxWinDrvInstLogError(pCtx, "Driver installation failed with %Rrc", rc);

    return rc;
}

/**
 * Installs a driver.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   pszInfFile          INF file to use.
 * @param   fFlags              Installation flags (of type VBOX_WIN_DRIVERINSTALL_F_XXX) to use.
 *
 * @note    This function tries determining the model / PnP ID from the given INF file.
 *          To control the behavior exactly, use VBoxWinDrvInstInstallEx().
 */
int VBoxWinDrvInstInstall(VBOXWINDRVINST hDrvInst, const char *pszInfFile, uint32_t fFlags)
{
    return VBoxWinDrvInstInstallEx(hDrvInst, pszInfFile, NULL /* pszModel */, NULL /* pszPnpId */, fFlags);
}

/**
 * Uninstalls a driver.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   pszInfFile          INF file within driver store to uninstall.
 *                              Optional and can be NULL.
 * @param   pszModel            Model to uninstall (e.g. "VBoxUSB.AMD64"). NT-style wildcards supported.
 *                              Optional and can be NULL.
 * @param   pszPnpId            PnP ID to use (e.g. "USB\\VID_80EE&PID_CAFE"). NT-style wildcards supported.
 *                              Optional and can be NULL.
 * @param   fFlags              Installation flags (of type VBOX_WIN_DRIVERINSTALL_F_XXX) to use.
 */
int VBoxWinDrvInstUninstall(VBOXWINDRVINST hDrvInst, const char *pszInfFile, const char *pszModel, const char *pszPnpId,
                            uint32_t fFlags)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN(pCtx);

    int rc = VINF_SUCCESS;

    vboxWinDrvInstParmsInit(&pCtx->Parms);

    /* If given, get the sole INF file name (without path), to make it searchable within the driver store.
     * Note: This only will work with "oemXXX.inf" files for the (legcy) driver store. */
    if (pszInfFile && *pszInfFile)
    {
        char *pszInfFileName = RTPathFilename(pszInfFile);
        if (pszInfFileName)
            rc = RTStrToUtf16(pszInfFileName, &pCtx->Parms.pwszInfFile);
        else
            rc = VERR_FILE_NOT_FOUND;
    }

    if (RT_SUCCESS(rc) && pszModel && *pszModel)
        rc = RTStrToUtf16(pszModel, &pCtx->Parms.u.UnInstall.pwszModel);
    if (RT_SUCCESS(rc) && pszPnpId && *pszPnpId)
        rc = RTStrToUtf16(pszPnpId, &pCtx->Parms.u.UnInstall.pwszPnpId);

    pCtx->Parms.enmMode = VBOXWINDRVINSTMODE_UNINSTALL;
    pCtx->Parms.fFlags  = fFlags;

    if (RT_SUCCESS(rc))
        rc = vboxWinDrvInstMain(pCtx, &pCtx->Parms);

    if (!(fFlags & VBOX_WIN_DRIVERINSTALL_F_NO_DESTROY))
        vboxWinDrvInstParmsDestroy(&pCtx->Parms);

    if (RT_FAILURE(rc))
        vboxWinDrvInstLogError(pCtx, "Driver uninstallation failed with %Rrc", rc);

    return rc;
}

/**
 * Worker function for executing a section of an INF file.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   fInstall            Whether to execute the section to install or uninstall.
 * @param   pszInfFile          INF file to use.
 * @param   pszSection          Section within the INF file to execute.
 * @param   fFlags              Installation flags to use.
 */
int VBoxWinDrvInstExecuteInfWorker(VBOXWINDRVINST hDrvInst,
                                   bool fInstall, const char *pszInfFile, const char *pszSection, uint32_t fFlags)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN(pCtx);

    AssertPtrReturn(pszInfFile, VERR_INVALID_POINTER);

    vboxWinDrvInstParmsInit(&pCtx->Parms);

    int rc = RTStrToUtf16(pszInfFile, &pCtx->Parms.pwszInfFile);
    if (RT_SUCCESS(rc) && pszSection) /* pszSection is optional. */
        rc = RTStrToUtf16(pszSection, &pCtx->Parms.u.ExecuteInf.pwszSection);
    AssertRCReturn(rc, rc);

    pCtx->Parms.enmMode = fInstall ? VBOXWINDRVINSTMODE_INSTALL_INFSECTION : VBOXWINDRVINSTMODE_UNINSTALL_INFSECTION;
    pCtx->Parms.fFlags  = fFlags;

    rc = vboxWinDrvInstMain(pCtx, &pCtx->Parms);

    if (!(fFlags & VBOX_WIN_DRIVERINSTALL_F_NO_DESTROY))
        vboxWinDrvInstParmsDestroy(&pCtx->Parms);

    return rc;
}

/**
 * Executes a section of an INF file for installation.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   pszInfFile          INF file to use.
 * @param   pszSection          Section within the INF file to execute.
 * @param   fFlags              Installation flags to use.
 */
int VBoxWinDrvInstInstallExecuteInf(VBOXWINDRVINST hDrvInst, const char *pszInfFile, const char *pszSection, uint32_t fFlags)
{
    return VBoxWinDrvInstExecuteInfWorker(hDrvInst, true /* fInstall */, pszInfFile, pszSection, fFlags);
}

/**
 * Executes a section of an INF file for uninstallation.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   pszInfFile          INF file to use.
 * @param   pszSection          Section within the INF file to execute.
 * @param   fFlags              Installation flags to use.
 */
int VBoxWinDrvInstUninstallExecuteInf(VBOXWINDRVINST hDrvInst, const char *pszInfFile, const char *pszSection, uint32_t fFlags)
{
    return VBoxWinDrvInstExecuteInfWorker(hDrvInst, false /* fInstall */, pszInfFile, pszSection, fFlags);
}

#ifdef VBOX_WINDRVINST_USE_NT_APIS
/**
 * Queries the target in the NT namespace of the given symbolic link.
 *
 * @returns VBox status code.
 * @param   pwszLinkNt      The symbolic link to query the target for.
 * @param   ppwszLinkTarget Where to store the link target in the NT namespace on success.
 *                          Must be freed with RTUtf16Free().
 */
int VBoxWinDrvInstQueryNtLinkTarget(PCRTUTF16 pwszLinkNt, PRTUTF16 *ppwszLinkTarget)
{
    int                 rc    = VINF_SUCCESS;
    HANDLE              hFile = RTNT_INVALID_HANDLE_VALUE;
    IO_STATUS_BLOCK     Ios   = RTNT_IO_STATUS_BLOCK_INITIALIZER;
    UNICODE_STRING      NtName;

    NtName.Buffer        = (PWSTR)pwszLinkNt;
    NtName.Length        = (USHORT)(RTUtf16Len(pwszLinkNt) * sizeof(RTUTF16));
    NtName.MaximumLength = NtName.Length + sizeof(RTUTF16);

    OBJECT_ATTRIBUTES ObjAttr;
    InitializeObjectAttributes(&ObjAttr, &NtName, OBJ_CASE_INSENSITIVE, NULL /*hRootDir*/, NULL /*pSecDesc*/);

    NTSTATUS rcNt = g_pfnNtOpenSymbolicLinkObject(&hFile, SYMBOLIC_LINK_QUERY, &ObjAttr);
    if (NT_SUCCESS(rcNt))
    {
        UNICODE_STRING UniStr;
        RTUTF16 awszBuf[1024];
        RT_ZERO(awszBuf);
        UniStr.Buffer = awszBuf;
        UniStr.MaximumLength = sizeof(awszBuf);
        rcNt = g_pfnNtQuerySymbolicLinkObject(hFile, &UniStr, NULL);
        if (NT_SUCCESS(rcNt))
        {
            *ppwszLinkTarget = RTUtf16Dup((PRTUTF16)UniStr.Buffer);
            if (!*ppwszLinkTarget)
                rc = VERR_NO_STR_MEMORY;
        }
        else
            rc = RTErrConvertFromNtStatus(rcNt);

        CloseHandle(hFile);
    }
    else
        rc = RTErrConvertFromNtStatus(rcNt);

    return rc;
}
#endif /* VBOX_WINDRVINST_USE_NT_APIS */

/**
 * Controls a Windows service, internal version.
 *
 * @returns VBox status code.
 * @retval  VERR_NOT_FOUND if the given service was not found.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   pszService          Name of service to control.
 * @param   enmFn               Service control function to use.
 *                              VBOXWINDRVSVCFN_RESTART is not implemented and must be composed of
 *                              VBOXWINDRVSVCFN_START + VBOXWINDRVSVCFN_STOP by the caller.
 * @param   fFlags              Service control flags (of type VBOXWINDRVSVCFN_F_XXX) to use.
 * @param   msTimeout           Timeout (in ms) to use. Ignored if VBOXWINDRVSVCFN_F_WAIT is missing in \a fFlags.
 */
static int vboxWinDrvInstServiceControlEx(PVBOXWINDRVINSTINTERNAL pCtx,
                                          const char *pszService, VBOXWINDRVSVCFN enmFn, uint32_t fFlags, RTMSINTERVAL msTimeout)
{
    AssertPtrReturn(pszService, VERR_INVALID_POINTER);
    AssertReturn(!(fFlags & ~VBOXWINDRVSVCFN_F_VALID_MASK), VERR_INVALID_PARAMETER);
    AssertReturn(enmFn > VBOXWINDRVSVCFN_INVALID && enmFn < VBOXWINDRVSVCFN_END, VERR_INVALID_PARAMETER);
    AssertReturn(!(fFlags & VBOXWINDRVSVCFN_F_WAIT) || msTimeout, VERR_INVALID_PARAMETER);

    PRTUTF16 pwszService;
    int rc = RTStrToUtf16(pszService, &pwszService);
    if (RT_FAILURE(rc))
        return rc;

    SC_HANDLE hSvc = NULL;
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM != NULL)
    {
        hSvc = OpenServiceW(hSCM, pwszService, SERVICE_ALL_ACCESS | SERVICE_QUERY_STATUS);
        if (hSvc == NULL)
        {
            rc = RTErrConvertFromWin32(GetLastError());
            if (rc != VERR_NOT_FOUND)
                rc = vboxWinDrvInstLogLastError(pCtx, "Opening service '%s' failed", pszService);
        }
    }
    else
        rc = vboxWinDrvInstLogLastError(pCtx, "Opening Service Control Manager (SCM) failed");

    if (RT_FAILURE(rc))
    {
        RTUtf16Free(pwszService);
        if (hSvc)
            CloseServiceHandle(hSvc);
        if (hSCM)
            CloseServiceHandle(hSCM);
        return rc;
    }

    SERVICE_STATUS_PROCESS enmSvcSts;

    switch (enmFn)
    {
        case VBOXWINDRVSVCFN_START:
        {
            if (!StartService(hSvc, 0, NULL))
            {
                if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
                    break;

                /** @todo Also handle disabled services here? */

                rc = vboxWinDrvInstLogLastError(pCtx, "Starting service '%s' failed", pszService);
            }
            else
                vboxWinDrvInstLogInfo(pCtx, "Starting service '%s' ...", pszService);
            break;
        }

        case VBOXWINDRVSVCFN_STOP:
        {
            if (!ControlService(hSvc, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&enmSvcSts))
            {
                DWORD const dwErr = GetLastError();

                /* A not active or disabled service is not an error, so just skip. */
                if (   dwErr == ERROR_SERVICE_DISABLED
                    || dwErr == ERROR_SERVICE_NOT_ACTIVE)
                    break;

                rc = vboxWinDrvInstLogLastError(pCtx, "Stopping service '%s' failed", pszService);
            }
            else
                vboxWinDrvInstLogInfo(pCtx, "Stopping service '%s' ...", pszService);
            break;
        }

        case VBOXWINDRVSVCFN_DELETE:
        {
            if (!DeleteService(hSvc))
                rc = vboxWinDrvInstLogLastError(pCtx, "Deleting service '%s' failed", pszService);
            else
            {
                vboxWinDrvInstLogInfo(pCtx, "Successfully deleted service '%s'", pszService);
                fFlags &= ~VBOXWINDRVSVCFN_F_WAIT; /* Drop the wait flag, makes no sense here. */
            }
            break;
        }

        default:
            AssertFailedStmt(rc = VERR_NOT_SUPPORTED);
            break;
    }

    if (RT_SUCCESS(rc))
    {
        if (fFlags & VBOXWINDRVSVCFN_F_WAIT)
        {
            uint64_t const msStartTS = RTTimeMilliTS();

            rc = VERR_NO_CHANGE; /* No change yet. */

            if (!g_pfnQueryServiceStatusEx)
            {
                vboxWinDrvInstLogWarn(pCtx, "Waiting for status change of service '%s' not supported on this OS, skipping",
                                      pszService);
                rc = VINF_SUCCESS;
            }
            else
            {
                vboxWinDrvInstLogInfo(pCtx, "Waiting for status change of service '%s' (%ums timeout) ...", pszService, msTimeout);
                for (;;)
                {
                    DWORD dwBytes;
                    if (!g_pfnQueryServiceStatusEx(hSvc,
                                                   SC_STATUS_PROCESS_INFO,
                                                   (LPBYTE)&enmSvcSts,
                                                   sizeof(SERVICE_STATUS_PROCESS),
                                                   &dwBytes))
                    {
                        rc = vboxWinDrvInstLogLastError(pCtx, "Failed to query service status");
                        break;
                    }

                    if ((RTTimeMilliTS() - msStartTS) % RT_MS_1SEC == 0) /* Don't spam. */
                        vboxWinDrvInstLogVerbose(pCtx, 3, "Service '%s' status is %#x: %u",
                                                 pszService, enmSvcSts.dwCurrentState, (RTTimeMilliTS() - msStartTS) % 100 == 0);

                    switch (enmSvcSts.dwCurrentState)
                    {
                        case SERVICE_STOP_PENDING:
                        case SERVICE_START_PENDING:
                            RTThreadSleep(100); /* Wait a bit before retrying. */
                            break;

                        case SERVICE_RUNNING:
                        {
                            if (enmFn == VBOXWINDRVSVCFN_START)
                                rc = VINF_SUCCESS;
                            break;
                        }

                        case SERVICE_STOPPED:
                        {
                            if (enmFn == VBOXWINDRVSVCFN_START)
                            {
                                vboxWinDrvInstLogError(pCtx, "Service '%s' stopped unexpectedly", pszService);
                                rc = VERR_INVALID_STATE;
                            }
                            else
                                rc = VINF_SUCCESS;
                            break;
                        }

                        default:
                        {
                            vboxWinDrvInstLogError(pCtx, "Service '%s' reported an unexpected state (%#x)",
                                                   pszService, enmSvcSts.dwCurrentState);
                            rc = VERR_INVALID_STATE;
                        }
                    }

                    if (   RT_FAILURE(rc)
                        && rc != VERR_NO_CHANGE)
                        break;

                    if (RT_SUCCESS(rc))
                        break;

                    if (RTTimeMilliTS() - msStartTS >= msTimeout)
                    {
                        vboxWinDrvInstLogError(pCtx, "Waiting for service '%s' timed out (%ums)", pszService, msTimeout);
                        rc = VERR_TIMEOUT;
                        break;
                    }
                }
            }

            if (RT_SUCCESS(rc))
                vboxWinDrvInstLogInfo(pCtx, "Service '%s' successfully %s",
                                      pszService, enmFn == VBOXWINDRVSVCFN_START ? "started" : "stopped");
        }
        else
            vboxWinDrvInstLogVerbose(pCtx, 1, "Service '%s' was %s asynchronously",
                                     pszService, enmFn == VBOXWINDRVSVCFN_START ? "started" : "stopped");
    }

    RTUtf16Free(pwszService);
    CloseServiceHandle(hSvc);
    CloseServiceHandle(hSCM);
    return rc;
}

/**
 * Controls a Windows service, extended version.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   pszService          Name of service to control.
 * @param   enmFn               Service control function to use.
 * @param   fFlags              Service control flags (of type VBOXWINDRVSVCFN_F_XXX) to use.
 * @param   msTimeout           Timeout (in ms) to use. Only being used if VBOXWINDRVSVCFN_F_WAIT is specified in \a fFlags.
 */
int VBoxWinDrvInstServiceControlEx(VBOXWINDRVINST hDrvInst,
                                   const char *pszService, VBOXWINDRVSVCFN enmFn, uint32_t fFlags, RTMSINTERVAL msTimeout)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN(pCtx);

#define CONTROL_SERVICE(a_Fn) \
    vboxWinDrvInstServiceControlEx(pCtx, pszService, a_Fn, fFlags, msTimeout);

    int rc;
    if (enmFn == VBOXWINDRVSVCFN_RESTART)
    {
        rc = CONTROL_SERVICE(VBOXWINDRVSVCFN_STOP);
        if (RT_SUCCESS(rc))
            rc = CONTROL_SERVICE(VBOXWINDRVSVCFN_START);
    }
    else
        rc = CONTROL_SERVICE(enmFn);

#undef CONTROL_SERVICE
    return rc;
}

/**
 * Controls a Windows service.
 *
 * @returns VBox status code.
 * @param   hDrvInst            Windows driver installer handle to use.
 * @param   pszService          Name of service to control.
 * @param   enmFn               Service control function to use.
 *
 * @note    Function waits 30s for the service to reach the desired control function.
 *          Use VBooxWinDrvInstControlServiceEx() for more flexibility.
 */
int VBoxWinDrvInstServiceControl(VBOXWINDRVINST hDrvInst, const char *pszService, VBOXWINDRVSVCFN enmFn)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN(pCtx);

    return VBoxWinDrvInstServiceControlEx(pCtx, pszService, enmFn, VBOXWINDRVSVCFN_F_WAIT, RT_MS_30SEC);
}

/**
 * Destroys a service information structure, internal version.
 *
 * @param   pSvcInfo                 Service information structure to destroy.
 */
static void vboxWinDrvInstServiceInfoDestroy(PVBOXWINDRVSVCINFO pSvcInfo)
{
    if (pSvcInfo->pStatus)
    {
        RTMemFree(pSvcInfo->pStatus);
        pSvcInfo->pStatus = NULL;
    }

    if (pSvcInfo->pConfig)
    {
        RTMemFree(pSvcInfo->pConfig);
        pSvcInfo->pConfig = NULL;
    }
}

/**
 * Destroys a service information structure.
 *
 * @param   pSvcInfo                 Service information structure to destroy.
 */
void VBoxWinDrvInstServiceInfoDestroy(PVBOXWINDRVSVCINFO pSvcInfo)
{
    vboxWinDrvInstServiceInfoDestroy(pSvcInfo);
}

/**
 * Queries information about a system service (or driver).
 *
 * @returns VBox status code.
 * @retval  VERR_NOT_SUPPORTED if querying for system services is not supported (NT4).
 * @param   hSCM                SCM handle to use.
 * @param   pszService          Service (or driver) name to query information for.
 * @param   pSvcInfo            Where to store the queried information on success.
 *                              Must be destroyed with VBoxWinDrvInstServiceInfoDestroy().
 */
static int vboxWinDrvInstServiceQueryExInternal(SC_HANDLE hSCM, const char *pszService, PVBOXWINDRVSVCINFO pSvcInfo)
{
    if (!g_pfnQueryServiceStatusEx)
        return VERR_NOT_SUPPORTED;

    PRTUTF16 pwszService = NULL;
    int rc = RTStrToUtf16(pszService, &pwszService);
    AssertRCReturn(rc, rc);

    SC_HANDLE const hSvc = OpenServiceW(hSCM, pwszService, SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);

    RTUtf16Free(pwszService);
    pwszService = NULL;

    if (hSvc == NULL)
        return RTErrConvertFromWin32(GetLastError());

    RT_BZERO(pSvcInfo, sizeof(VBOXWINDRVSVCINFO));

    DWORD dwBytesNeeded;
    g_pfnQueryServiceStatusEx(hSvc, SC_STATUS_PROCESS_INFO, NULL, 0, &dwBytesNeeded);
    DWORD dwLastErr = GetLastError();
    if (dwLastErr == ERROR_INSUFFICIENT_BUFFER)
    {
        pSvcInfo->pStatus = (LPSERVICE_STATUS_PROCESS)RTMemAlloc(dwBytesNeeded);
        if (pSvcInfo->pStatus)
        {
            if (g_pfnQueryServiceStatusEx(hSvc, SC_STATUS_PROCESS_INFO, (LPBYTE)pSvcInfo->pStatus, dwBytesNeeded, &dwBytesNeeded))
            {

            }
            else
                rc = RTErrConvertFromWin32(dwLastErr);
        }
        else
            rc = VERR_NO_MEMORY;
    }
    else
        rc = RTErrConvertFromWin32(dwLastErr);

    if (RT_SUCCESS(rc))
    {
        QueryServiceConfigW(hSvc, pSvcInfo->pConfig, 0, &dwBytesNeeded);
        dwLastErr = GetLastError();
        if (dwLastErr == ERROR_INSUFFICIENT_BUFFER)
        {
            pSvcInfo->pConfig = (LPQUERY_SERVICE_CONFIGW)RTMemAlloc(dwBytesNeeded);
            if (pSvcInfo->pConfig)
            {
                if (QueryServiceConfigW(hSvc, pSvcInfo->pConfig, dwBytesNeeded, &dwBytesNeeded))
                {

                }
                else
                    rc = RTErrConvertFromWin32(dwLastErr);
            }
            else
                rc = VERR_NO_MEMORY;
        }
        else
           rc = RTErrConvertFromWin32(dwLastErr);
    }

    int const rc2 = VBoxWinDrvInstFileQueryVersionUtf16(pSvcInfo->pConfig->lpBinaryPathName,
                                                        pSvcInfo->szVer, sizeof(pSvcInfo->szVer));
    /* If the file was not found we wanted to query the version from, don't return this as
     * an error to the caller. We still potentially have the service information from above. */
    if (rc2 != VERR_FILE_NOT_FOUND)
    {
        if (RT_SUCCESS(rc))
            rc = rc2;
    }

    if (RT_FAILURE(rc))
        vboxWinDrvInstServiceInfoDestroy(pSvcInfo);

    CloseServiceHandle(hSvc);
    return rc;
}

/**
 * Queries information about a system service (or driver).
 *
 * @returns VBox status code.
 * @param   pszService          Service (or driver) name to query information for.
 * @param   pSvcInfo            Where to store the queried information on success.
 *                              Must be destroyed with VBoxWinDrvInstServiceInfoDestroy().
 */
int VBoxWinDrvInstServiceQuery(const char *pszService, PVBOXWINDRVSVCINFO pSvcInfo)
{
    int rc;

    SC_HANDLE const hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSCM != NULL)
    {
        rc = vboxWinDrvInstServiceQueryExInternal(hSCM, pszService, pSvcInfo);
        CloseServiceHandle(hSCM);
    }
    else
        rc = RTErrConvertFromWin32(GetLastError());

    return rc;
}

/**
 * Replaces all occurrences of a given pattern set within a string.
 *
 * @returns VBox status code.
 * @param   pszInput            Input string to replace patterns in.
 * @param   paPatterns          Array of patterns to process.
 * @param   cPatterns           Number of patterns in \a paPatterns.
 * @param   ppszOutput          Where to return the ouptut string on success.
 *                              Must be free'd with RTStrFree().
 */
int VBoxWinDrvPatternReplace(const char *pszInput, const PVBOXWINDRVSTRPATTERN paPatterns, size_t cPatterns, char **ppszOutput)
{
    size_t cchSrc = strlen(pszInput);
    size_t cchDst = cchSrc + 1;

    for (size_t i = 0; i < cPatterns; i++)
    {
        PVBOXWINDRVSTRPATTERN pPatt = &paPatterns[i];

        size_t const cchPattern = strlen(pPatt->psz);
        AssertReturn(cchPattern, VERR_INVALID_PARAMETER);

        size_t cFound = 0;
        const char *psz = pszInput;
        while ((psz = strstr(psz, pPatt->psz)) != NULL)
        {
            cFound++;
            psz += cchPattern;
        }

        if (cFound)
        {
            pPatt->rep = pPatt->pfn(pPatt->psz, pPatt->pvUser);
            size_t const cchPatt = strlen(pPatt->psz);
            size_t const cchRep  = pPatt->rep ? strlen(pPatt->rep) : cchPatt;
            size_t const cchDiff = cchRep > cchPatt ? cchRep - cchPatt : 0;
            cchDst += cFound * cchDiff;
        }
    }

    int rc;

    char *pszOutput = (char *)RTStrAlloc(cchDst);
    if (pszOutput)
    {
        size_t idxDst = 0;
        size_t idxSrc = 0;
        while (idxSrc < cchSrc)
        {
            size_t cReplaced = 0;
            for (size_t p = 0; p < cPatterns; p++)
            {
                size_t const cchPatt = strlen(paPatterns[p].psz);
                if (   cchPatt
                    && strncmp(&pszInput[idxSrc], paPatterns[p].psz, cchPatt) == 0)
                {
                    size_t const cchRep = paPatterns[p].rep ? strlen(paPatterns[p].rep) : 0;
                    if (cchRep)
                    {
                        memcpy(pszOutput + idxDst, paPatterns[p].rep, cchRep);
                        idxDst += cchRep;
                    }
                    idxSrc += cchPatt;
                    cReplaced = 1;
                    break;
                }
            }
            if (!cReplaced)
                pszOutput[idxDst++] = pszInput[idxSrc++];
        }

        pszOutput[idxDst] = '\0';
        rc = VINF_SUCCESS;
    }
    else
        rc = VERR_NO_STR_MEMORY;

    for (size_t i = 0; i < cPatterns; i++)
    {
        RTStrFree(paPatterns[i].rep);
        paPatterns[i].rep = NULL;
    }

    *ppszOutput = pszOutput;
    return rc;
}

/**
 * Callback function for resolving patterns to environment variables.
 *
 * @sa FNVBOXWINDRVSTRPATTERN
 */
DECLCALLBACK(char *) vboxWinDrvInstPatternToEnvCallback(const char *pszPattern, void *pvUser)
{
    RT_NOREF(pvUser);

    const char *pszRessolvedPattern;
    if (RTStrICmp(pszPattern, "\\SystemRoot") == 0)
        pszRessolvedPattern = "SystemRoot";
    else
        pszRessolvedPattern = pszPattern;

    char szVal[RTPATH_MAX];
    int rc = RTEnvGetEx(RTENV_DEFAULT, pszRessolvedPattern, szVal, sizeof(szVal), NULL);
    if (RT_SUCCESS(rc))
        return RTStrDup(szVal);

    return NULL;
}

/**
 * Resolves a registry path.
 *
 * @returns VBox status code.
 * @param   pszPath             Path to resolve.
 * @param   ppszResolved        Where to return the resolved path on success.
 *                              Must be free'd using RTStrFree().
 */
int vboxWinDrvInstRegResolveRegPath(const char *pszPath, char **ppszResolved)
{
    static VBOXWINDRVSTRPATTERN s_aPatterns[] =
    {
        { "\\SystemRoot", vboxWinDrvInstPatternToEnvCallback, NULL, NULL },
        { "\\??\\", vboxWinDrvInstPatternToEnvCallback, NULL, NULL }
    };

    return VBoxWinDrvPatternReplace(pszPath, s_aPatterns, RT_ELEMENTS(s_aPatterns), ppszResolved);
}

/**
 * Worker for vboxDrvInstQueryFileVersion.
 *
 * @returns VBox status code.
 * @param   pszPath             ASCII & ANSI & UTF-8 compliant name.
 * @param   puMajor             Where to return the major version number.
 * @param   puMinor             Where to return the minor version number.
 * @param   puBuildNumber       Where to return the build number.
 * @param   puRevisionNumber    Where to return the revision number.
 *
 * @note    Copied from vgsvcUtilGetFileVersion(). Merge this?
 */
int VBoxWinDrvInstFileQueryVersionEx(const char *pszPath, uint32_t *puMajor, uint32_t *puMinor, uint32_t *puBuildNumber,
                                     uint32_t *puRevisionNumber)
{
    int rc;

    *puMajor = *puMinor = *puBuildNumber = *puRevisionNumber = 0;

    char *pszPathTmp = RTStrDup(pszPath);
    AssertPtrReturn(pszPathTmp, VERR_NO_TMP_MEMORY);

    /* Remove quotes (if any). */
    char *pszSrc = pszPathTmp;
    char *pszDst = pszPathTmp;
    while (*pszSrc)
    {
        if (*pszSrc != '"')
            *pszDst++ = *pszSrc;
        pszSrc++;
    }
    *pszDst = L'\0';

    /*
     * Get the file version info.
     */
    DWORD dwHandleIgnored;
    DWORD cbVerData = GetFileVersionInfoSizeA(pszPathTmp, &dwHandleIgnored);
    if (cbVerData)
    {
        LPTSTR pVerData = (LPTSTR)RTMemTmpAllocZ(cbVerData);
        if (pVerData)
        {
            if (GetFileVersionInfoA(pszPathTmp, dwHandleIgnored, cbVerData, pVerData))
            {
                /* Always use VS_FIXEDFILEINFO, to make comparing file versions of different binaries possible / consistent. */
                UINT                 cbFileInfoIgnored = 0;
                VS_FIXEDFILEINFO    *pFileInfo = NULL;
                if (VerQueryValueW(pVerData, L"\\", (LPVOID *)&pFileInfo, &cbFileInfoIgnored))
                {
                    *puMajor          = HIWORD(pFileInfo->dwFileVersionMS);
                    *puMinor          = LOWORD(pFileInfo->dwFileVersionMS);
                    *puBuildNumber    = HIWORD(pFileInfo->dwFileVersionLS);
                    *puRevisionNumber = LOWORD(pFileInfo->dwFileVersionLS);
                    rc = VINF_SUCCESS;
                }
                else
                    rc = RTErrConvertFromWin32(GetLastError());
            }
            else
                rc = RTErrConvertFromWin32(GetLastError());

            RTMemTmpFree(pVerData);
        }
        else
            rc = VERR_NO_TMP_MEMORY;
    }
    else
        rc = RTErrConvertFromWin32(GetLastError());

    RTStrFree(pszPathTmp);
    return rc;
}

/**
 * Gets a re-formatted version string from the VS_FIXEDFILEINFO table.
 *
 * @returns VBox status code.  The output buffer is always valid and the status
 *          code can safely be ignored.
 *
 * @param   pszPath         Absolute path to the file to query.
 * @param   pszVersion      Where to return the version string.
 * @param   cbVersion       The size of the version string buffer. This MUST be
 *                          at least 2 bytes!
 *
 * @note    On failure the returned version string always will contain '-'.
 * @note    Copied from VGSvcUtilWinGetFileVersionString(). Merge this?
 */
int VBoxWinDrvInstFileQueryVersion(const char *pszPath, char *pszVersion, size_t cbVersion)
{
    /*
     * We will ALWAYS return with a valid output buffer.
     */
    AssertReturn(cbVersion >= 2, VERR_BUFFER_OVERFLOW);
    pszVersion[0] = '-';
    pszVersion[1] = '\0';

    char *pszTarget;
    int rc = vboxWinDrvInstRegResolveRegPath(pszPath, &pszTarget);
    if (RT_SUCCESS(rc))
    {
        uint32_t uMajor, uMinor, uBuild, uRev;
        rc = VBoxWinDrvInstFileQueryVersionEx(pszTarget, &uMajor, &uMinor, &uBuild, &uRev);
        if (RT_SUCCESS(rc))
            RTStrPrintf(pszVersion, cbVersion, "%u.%u.%ur%u", uMajor, uMinor, uBuild, uRev);

        RTStrFree(pszTarget);
    }
    return rc;
}

/**
 * Gets a re-formatted version string from the VS_FIXEDFILEINFO table, UTF-16 version.
 *
 * @returns VBox status code.  The output buffer is always valid and the status
 *          code can safely be ignored.
 * @param   pszwPath        Absolute path to the file to query.
 * @param   pszVersion      Where to return the version string.
 * @param   cbVersion       The size of the version string buffer. This MUST be
 *                          at least 2 bytes!
 *
 * @note    On failure the returned version string always will contain '-'.
 */
int VBoxWinDrvInstFileQueryVersionUtf16(PCRTUTF16 pwszPath, char *pszVersion, size_t cbVersion)
{
    char *pszPath;
    int rc = RTUtf16ToUtf8(pwszPath, &pszPath);
    if (RT_SUCCESS(rc))
    {
        rc = VBoxWinDrvInstFileQueryVersion(pszPath, pszVersion, cbVersion);
        RTStrFree(pszPath);
    }

    return rc;
}

#ifdef TESTCASE
/**
 * Returns the internal parameters of an (un)installation.
 *
 * @returns Internal parameters of an (un)installation.
 * @param   hDrvInst            Windows driver installer handle to use.
 */
PVBOXWINDRVINSTPARMS VBoxWinDrvInstTestGetParms(VBOXWINDRVINST hDrvInst)
{
    PVBOXWINDRVINSTINTERNAL pCtx = hDrvInst;
    VBOXWINDRVINST_VALID_RETURN_RC((hDrvInst), NULL);

    return &pCtx->Parms;
}

/**
 * Detroys internal parameters of an (un)installation.
 *
 * @param   pParms              Internal parameters of an (un)installation to destroy.
 */
void VBoxWinDrvInstTestParmsDestroy(PVBOXWINDRVINSTPARMS pParms)
{
    vboxWinDrvInstParmsDestroy(pParms);
}
#endif /* TESTCASE */
