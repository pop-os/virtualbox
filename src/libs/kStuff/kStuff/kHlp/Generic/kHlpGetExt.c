/* $Id: kHlpGetExt.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpPath - kHlpGetExt and kHlpGetSuff.
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
#include <k/kHlpPath.h>
#include <k/kHlpString.h>


/**
 * Gets the filename suffix.
 *
 * @returns Pointer to where the suffix starts within the string pointed to by pszFilename.
 * @returns Pointer to the terminator char if no suffix.
 * @param   pszFilename     The filename to parse.
 */
KHLP_DECL(char *) kHlpGetSuff(const char *pszFilename)
{
    const char *pszDot = NULL;
    pszFilename = kHlpGetFilename(pszFilename);
    for (;;)
    {
        char ch = *pszFilename;
        if (ch == '.')
        {
            while ((ch = *++pszFilename) == '.')
                /* nothing */;
            if (ch)
                pszDot = pszFilename - 1;
        }
        if (!ch)
            return (char *)(pszDot ? pszDot : pszFilename);
        pszFilename++;
    }
}


/**
 * Gets the filename extention.
 *
 * @returns Pointer to where the extension starts within the string pointed to by pszFilename.
 * @returns Pointer to the terminator char if no extension.
 * @param   pszFilename     The filename to parse.
 */
KHLP_DECL(char *) kHlpGetExt(const char *pszFilename)
{
    char *psz = kHlpGetSuff(pszFilename);
    return *psz ? psz + 1 : psz;
}

