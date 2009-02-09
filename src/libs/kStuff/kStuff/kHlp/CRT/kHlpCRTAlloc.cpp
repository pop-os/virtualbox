/* $Id: kHlpCRTAlloc.cpp 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpAlloc - Memory Allocation, CRT based implementation.
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
#include <k/kHlpAlloc.h>
#include <stdlib.h>
#include <string.h>


KHLP_DECL(void *) kHlpAlloc(KSIZE cb)
{
    return malloc(cb);
}


KHLP_DECL(void *) kHlpAllocZ(KSIZE cb)
{
    return calloc(1, cb);
}


KHLP_DECL(void *) kHlpDup(const void *pv, KSIZE cb)
{
    void *pvDup = kHlpAlloc(cb);
    if (pvDup)
        return memcpy(pvDup, pv, cb);
    return NULL;
}


KHLP_DECL(char *) kHlpStrDup(const char *psz)
{
    size_t cb = strlen(psz) + 1;
    return (char *)kHlpDup(psz, cb);
}


KHLP_DECL(void *) kHlpRealloc(void *pv, KSIZE cb)
{
    return realloc(pv, cb);
}


KHLP_DECL(void) kHlpFree(void *pv)
{
    if (pv)
        free(pv);
}

