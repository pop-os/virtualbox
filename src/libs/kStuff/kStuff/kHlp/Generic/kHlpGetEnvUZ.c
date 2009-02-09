/* $Id: kHlpGetEnvUZ.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpEnv - kHlpGetEnvUZ.
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
#include <k/kHlpEnv.h>
#include <k/kHlpString.h>


/**
 * Gets an environment variable and converts it to a KSIZE.
 *
 * @returns 0 and *pcb on success.
 * @returns On failure see kHlpGetEnv.
 * @param   pszVar  The name of the variable.
 * @param   pcb     Where to put the value.
 */
KHLP_DECL(int) kHlpGetEnvUZ(const char *pszVar, KSIZE *pcb)
{
    KSIZE       cb;
    unsigned    uBase;
    char        szVal[64];
    KSIZE       cchVal = sizeof(szVal);
    const char *psz;
    int         rc;

    *pcb = 0;
    rc = kHlpGetEnv(pszVar, szVal, cchVal);
    if (rc)
        return rc;

    /* figure out the base. */
    uBase = 10;
    psz = szVal;
    if (    *psz == '0'
        &&  (psz[1] == 'x' || psz[1] == 'X'))
    {
        uBase = 16;
        psz += 2;
    }

    /* convert it up to the first unknown char. */
    cb = 0;
    for(;;)
    {
        const char ch = *psz;
        unsigned uDigit;
        if (!ch)
            break;
        else if (ch >= '0' && ch <= '9')
            uDigit = ch - '0';
        else if (ch >= 'a' && ch <= 'z')
            uDigit = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'Z')
            uDigit = ch - 'A' + 10;
        else
            break;
        if (uDigit >= uBase)
            break;

        /* add the digit */
        cb *= uBase;
        cb += uDigit;

        psz++;
    }

    /* check for unit */
    if (*psz == 'm' || *psz == 'M')
        cb *= 1024*1024;
    else if (*psz == 'k' ||*psz == 'K')
        cb *= 1024;
    else if (*psz == 'g' || *psz == 'G')
        cb *= 1024*1024*1024;

    *pcb = cb;
    return 0;
}


