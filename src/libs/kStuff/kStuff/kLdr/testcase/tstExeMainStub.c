/* $Id: tstExeMainStub.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kLdr testcase - DLL Stub.
 */

/*
 * Copyright (c) 2006-2007 knut st. osmundsen <bird-kStuff-spam@anduin.net>
 *
 * This file is part of kStuff.
 *
 * kStuff is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * kStuff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with kStuff; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "tst.h"

#if K_OS == K_OS_OS2
# define INCL_BASE
# include <os2.h>

#elif K_OS == K_OS_WINDOWS
/* nothing */

#elif K_OS == K_OS_NT
# include <ddk/ntapi.h> /** @todo fix the nt port. */

#else
# error "port me"
#endif


extern int main();


#if K_OS == K_OS_OS2
/**
 * OS/2 'main'.
 */
ULONG _System OS2Main(HMODULE hmod, ULONG fFlag, ULONG ulReserved, PSZ pszzEnv, PSZ pszzCmdLine)
{
    int rc;
    rc = main();
    return rc;
}

#elif K_OS == K_OS_WINDOWS
/**
 * Windows'main'
 */
int WindowsMain(void)
{
    int rc;
    rc = main();
    return rc;
}

#elif K_OS == K_OS_NT
/**
 * Windows NT 'main'
 */
VOID NtProcess(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
    int rc;
    rc = main();
    /* (no way around this) */
    for (;;)
        ZwTerminateProcess(NtCurrentProcess(), rc);
}

#else
# error "port me"
#endif


