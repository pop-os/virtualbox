/* $Id: kHlpBareAssert.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpBare - Assert Backend.
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
 * In addition to the permissions in the GNU Lesser General Public
 * License, you are granted unlimited permission to link the compiled
 * version of this file into combinations with other programs, and to
 * distribute those combinations without any restriction coming from
 * the use of this file.
 *
 * kStuff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with kStuff; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <k/kHlpAssert.h>
#include <k/kHlpString.h>

#if K_OS == K_OS_DARWIN \
 || K_OS == K_OS_FREEBSD \
 || K_OS == K_OS_LINUX \
 || K_OS == K_OS_NETBSD \
 || K_OS == K_OS_OPENBSD \
 || K_OS == K_OS_SOLARIS
# include <k/kHlpSys.h>

#elif K_OS == K_OS_OS2
# define INCL_BASE
# define INCL_ERRORS
# include <os2.h>

#elif  K_OS == K_OS_WINDOWS
# include <Windows.h>

#else
# error "port me"
#endif


/**
 * Writes a assert string with unix lineendings.
 *
 * @param   pszMsg  The string.
 */
static void kHlpAssertWrite(const char *pszMsg)
{
#if K_OS == K_OS_DARWIN \
 || K_OS == K_OS_FREEBSD \
 || K_OS == K_OS_LINUX \
 || K_OS == K_OS_NETBSD \
 || K_OS == K_OS_OPENBSD \
 || K_OS == K_OS_SOLARIS
    KSIZE cchMsg = kHlpStrLen(pszMsg);
    kHlpSys_write(2 /* stderr */, pszMsg, cchMsg);

#elif K_OS == K_OS_OS2 ||  K_OS == K_OS_WINDOWS
    /*
     * Line by line.
     */
    ULONG       cbWritten;
    const char *pszNl = kHlpStrChr(pszMsg, '\n');
    while (pszNl)
    {
        cbWritten = pszNl - pszMsg;

#if K_OS == K_OS_OS2
        if (cbWritten)
            DosWrite((HFILE)2, pszMsg, cbWritten, &cbWritten);
        DosWrite((HFILE)2, "\r\n", 2, &cbWritten);
#else /* K_OS == K_OS_WINDOWS */
        if (cbWritten)
            WriteFile((HANDLE)STD_ERROR_HANDLE, pszMsg, cbWritten, &cbWritten, NULL);
        WriteFile((HANDLE)STD_ERROR_HANDLE, "\r\n", 2, &cbWritten, NULL);
#endif

        /* next */
        pszMsg = pszNl + 1;
        pszNl = kHlpStrChr(pszMsg, '\n');
    }

    /*
     * Remaining incomplete line.
     */
    if (*pszMsg)
    {
        cbWritten = kHlpStrLen(pszMsg);
#if K_OS == K_OS_OS2
        DosWrite((HFILE)2, pszMsg, cbWritten, &cbWritten);
#else /* K_OS == K_OS_WINDOWS */
        WriteFile((HANDLE)STD_ERROR_HANDLE, pszMsg, cbWritten, &cbWritten, NULL);
#endif
    }

#else
# error "port me"
#endif
}


KHLP_DECL(void) kHlpAssertMsg1(const char *pszExpr, const char *pszFile, unsigned iLine, const char *pszFunction)
{
    char szLine[16];

    kHlpAssertWrite("\n!!!kLdr Assertion Failed!!!\nExpression: ");
    kHlpAssertWrite(pszExpr);
    kHlpAssertWrite("\nAt: ");
    kHlpAssertWrite(pszFile);
    kHlpAssertWrite("(");
    kHlpAssertWrite(kHlpInt2Ascii(szLine, sizeof(szLine), iLine, 10));
    kHlpAssertWrite(") ");
    kHlpAssertWrite(pszFunction);
    kHlpAssertWrite("\n");
}


KHLP_DECL(void) kHlpAssertMsg2(const char *pszFormat, ...)
{
    kHlpAssertWrite(pszFormat);
}

