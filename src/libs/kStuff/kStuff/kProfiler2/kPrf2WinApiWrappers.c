/* $Id: kPrf2WinApiWrappers.c 13 2008-04-20 10:13:43Z bird $ */
/** @file
 * Wrappers for a number of common Windows APIs.
 */

/*
 * Copyright (c) 2008 knut st. osmundsen <bird-src-spam@anduin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define _ADVAPI32_
#define _KERNEL32_
#define _WIN32_WINNT 0x0600
#define UNICODE
#include <Windows.h>
#include <TLHelp32.h>
#include <k/kDefs.h>
#include "kPrf2WinApiWrapperHlp.h"

#if K_ARCH == K_ARCH_X86_32
typedef PVOID PRUNTIME_FUNCTION;
typedef FARPROC PGET_RUNTIME_FUNCTION_CALLBACK;
#endif

/* RtlUnwindEx is used by msvcrt on amd64, but winnt.h only defines it for IA64... */
typedef struct _FRAME_POINTERS {
    ULONGLONG MemoryStackFp;
    ULONGLONG BackingStoreFp;
} FRAME_POINTERS, *PFRAME_POINTERS;
typedef PVOID PUNWIND_HISTORY_TABLE;
typedef PVOID PKNONVOLATILE_CONTEXT_POINTERS;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
KPRF2WRAPDLL g_Kernel32 =
{
    INVALID_HANDLE_VALUE, "KERNEL32"
};


/*
 * Include the generated code.
 */
#include "kPrf2WinApiWrappers-kernel32.h"

/* TODO (amd64):

AddLocalAlternateComputerNameA
AddLocalAlternateComputerNameW
EnumerateLocalComputerNamesA
EnumerateLocalComputerNamesW
RemoveLocalAlternateComputerNameA
RemoveLocalAlternateComputerNameW

RtlLookupFunctionEntry
RtlPcToFileHeader
RtlRaiseException
RtlVirtualUnwind

SetConsoleCursor
SetLocalPrimaryComputerNameA
SetLocalPrimaryComputerNameW
__C_specific_handler
__misaligned_access
_local_unwind

*/


/**
 * The DLL Main for the Windows API wrapper DLL.
 *
 * @returns Success indicator.
 * @param   hInstDll        The instance handle of the DLL. (i.e. the module handle)
 * @param   fdwReason       The reason why we're here. This is a 'flag' for reasons of
 *                          tradition, it's really a kind of enum.
 * @param   pReserved       Reserved / undocumented something.
 */
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, PVOID pReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}

