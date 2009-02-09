/* $Id: kHlpCRTString.cpp 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpString - String And Memory Routines, CRT based implementation.
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
#include <string.h>


#ifndef kHlpMemChr
void   *kHlpMemChr(const void *pv, int ch, KSIZE cb)
{
    return (void *)memchr(pv, ch, cb);
}
#endif


#ifndef kHlpMemComp
int     kHlpMemComp(const void *pv1, const void *pv2, KSIZE cb)
{
    return memcmp(pv1, pv2, cb);
}
#endif


#ifndef kHlpMemCopy
void   *kHlpMemCopy(void *pv1, const void *pv2, KSIZE cb)
{
    return memcpy(pv1, pv2, cb);
}
#endif


#ifndef kHlpMemPCopy
void   *kHlpMemPCopy(void *pv1, const void *pv2, KSIZE cb)
{
    return (KU8 *)memcpy(pv1, pv2, cb) + cb;
}
#endif


#ifndef kHlpMemMove
void   *kHlpMemMove(void *pv1, const void *pv2, KSIZE cb)
{
    return memmove(pv1, pv2, cb);
}
#endif


#ifndef kHlpMemPMove
void   *kHlpMemPMove(void *pv1, const void *pv2, KSIZE cb)
{
    return (KU8 *)memmove(pv1, pv2, cb) + cb;
}
#endif


#ifndef kHlpMemSet
void   *kHlpMemSet(void *pv1, int ch, KSIZE cb)
{
    return memset(pv1, ch, cb);
}
#endif


#ifndef kHlpMemPSet
void   *kHlpMemPSet(void *pv1, int ch, KSIZE cb)
{
    return (KU8 *)memset(pv1, ch, cb) + cb;
}
#endif


#ifndef kHlpStrCat
char   *kHlpStrCat(char *psz1, const char *psz2)
{
    return strcat(psz1, psz2);
}
#endif


#ifndef kHlpStrNCat
char   *kHlpStrNCat(char *psz1, const char *psz2, KSIZE cb)
{
    return strncat(psz1, psz2, cb);
}
#endif


#ifndef kHlpStrChr
char   *kHlpStrChr(const char *psz, int ch)
{
    return (char *)strchr(psz, ch);
}
#endif


#ifndef kHlpStrRChr
char   *kHlpStrRChr(const char *psz, int ch)
{
    return (char *)strrchr(psz, ch);
}
#endif


#ifndef kHlpStrComp
int     kHlpStrComp(const char *psz1, const char *psz2)
{
    return strcmp(psz1, psz2);
}
#endif


#ifndef kHlpStrNComp
int     kHlpStrNComp(const char *psz1, const char *psz2, KSIZE cch)
{
    return strncmp(psz1, psz2, cch);
}
#endif


#ifndef kHlpStrCopy
char   *kHlpStrCopy(char *psz1, const char *psz2)
{
    return strcpy(psz1, psz2);
}
#endif


#ifndef kHlpStrLen
KSIZE   kHlpStrLen(const char *psz1)
{
    return strlen(psz1);
}
#endif

