/* $Id: kHlpInt2Ascii.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpString - kHlpInt2Ascii.
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
#include <k/kHlpString.h>


/**
 * Converts an signed integer to an ascii string.
 *
 * @returns psz.
 * @param   psz         Pointer to the output buffer.
 * @param   cch         The size of the output buffer.
 * @param   lVal        The value.
 * @param   iBase       The base to format it. (2,8,10 or 16)
 */
KHLP_DECL(char *) kHlpInt2Ascii(char *psz, KSIZE cch, long lVal, unsigned iBase)
{
    static const char s_szDigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char *pszRet = psz;

    if (cch >= (lVal < 0 ? 3U : 2U) && psz)
    {
        /* prefix */
        if (lVal < 0)
        {
            *psz++ = '-';
            cch--;
            lVal = -lVal;
        }

        /* the digits */
        do
        {
            *psz++ = s_szDigits[lVal % iBase];
            cch--;
            lVal /= iBase;
        } while (lVal && cch > 1);

        /* overflow indicator */
        if (lVal)
            psz[-1] = '+';
    }
    else if (!pszRet)
        return pszRet;
    else if (cch < 1 || !pszRet)
        return pszRet;
    else
        *psz++ = '+';
    *psz = '\0';

    return pszRet;
}

