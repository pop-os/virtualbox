/* $Id: kHlpIsFilenameOnly.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpPath - kHlpIsFilenameOnly.
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
 * Checks if this is only a filename or if it contains any kind
 * of drive, directory, or server specs.
 *
 * @returns 1 if this is a filename only.
 * @returns 0 of it's isn't only a filename.
 * @param   pszFilename     The filename to parse.
 */
KHLP_DECL(int) kHlpIsFilenameOnly(const char *pszFilename)
{
    for (;;)
    {
        const char ch = *pszFilename++;
#if K_OS == K_OS_OS2 || K_OS == K_OS_WINDOWS
        if (ch == '/' || ch == '\\' || ch == ':')
#else
        if (ch == '/')
#endif
            return 0;
        if (!ch)
            return 1;
    }
}

