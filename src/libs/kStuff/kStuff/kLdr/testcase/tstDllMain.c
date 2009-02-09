/* $Id: tstDllMain.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kLdr testcase.
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
# include <string.h>

#elif K_OS == K_OS_WINDOWS
# include <windows.h>
# include <string.h>

#elif K_OS == K_OS_DARWIN
# include <unistd.h>
# include <string.h>

#else
# error "port me"
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void tstWrite(const char *psz);



#if K_OS == K_OS_OS2
/**
 * OS/2 DLL 'main'
 */
ULONG _System _DLL_InitTerm(HMODULE hmod, ULONG fFlags)
{
    switch (fFlags)
    {
        case 0:
            tstWrite("init: ");
            tstWrite(g_pszName);
            tstWrite("\n");
            return TRUE;

        case 1:
            tstWrite("term: ");
            tstWrite(g_pszName);
            tstWrite("\n");
            return TRUE;

        default:
            tstWrite("!invalid!: ");
            tstWrite(g_pszName);
            tstWrite("\n");
            return FALSE;
    }
}

#elif K_OS == K_OS_WINDOWS

/**
 * OS/2 DLL 'main'
 */
BOOL __stdcall DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            tstWrite("init: ");
            tstWrite(g_pszName);
            tstWrite("\n");
            return TRUE;

        case DLL_PROCESS_DETACH:
            tstWrite("term: ");
            tstWrite(g_pszName);
            tstWrite("\n");
            return TRUE;

        case DLL_THREAD_ATTACH:
            tstWrite("thread init: ");
            tstWrite(g_pszName);
            tstWrite("\n");
            return TRUE;

        case DLL_THREAD_DETACH:
            tstWrite("thread term: ");
            tstWrite(g_pszName);
            tstWrite("\n");
            return TRUE;

        default:
            tstWrite("!invalid!: ");
            tstWrite(g_pszName);
            tstWrite("\n");
            return FALSE;
    }
}

#elif K_OS == K_OS_DARWIN
/* later */

#else
# error "port me"
#endif


/**
 * Writes a string with unix lineendings.
 *
 * @param   pszMsg  The string.
 */
void tstWrite(const char *pszMsg)
{
#if K_OS == K_OS_OS2 || K_OS == K_OS_WINDOWS
    /*
     * Line by line.
     */
    ULONG       cbWritten;
    const char *pszNl = strchr(pszMsg, '\n');

    while (pszNl)
    {
        cbWritten = pszNl - pszMsg;

#if K_OS == K_OS_OS2
        if (cbWritten)
            DosWrite((HFILE)2, pszMsg, cbWritten, &cbWritten);
        DosWrite((HFILE)2, "\r\n", 2, &cbWritten);
#else
        if (cbWritten)
            WriteFile((HANDLE)STD_ERROR_HANDLE, pszMsg, cbWritten, &cbWritten, NULL);
        WriteFile((HANDLE)STD_ERROR_HANDLE, "\r\n", 2, &cbWritten, NULL);
#endif

        /* next */
        pszMsg = pszNl + 1;
        pszNl = strchr(pszMsg, '\n');
    }

    /*
     * Remaining incomplete line.
     */
    if (*pszMsg)
    {
        cbWritten = strlen(pszMsg);
#if K_OS == K_OS_OS2
        DosWrite((HFILE)2, pszMsg, cbWritten, &cbWritten);
#else
        WriteFile((HANDLE)STD_ERROR_HANDLE, pszMsg, cbWritten, &cbWritten, NULL);
#endif
    }

#elif K_OS == K_OS_DARWIN
    write(STDERR_FILENO, pszMsg, strlen(pszMsg));

#else
# error "port me"
#endif
}


