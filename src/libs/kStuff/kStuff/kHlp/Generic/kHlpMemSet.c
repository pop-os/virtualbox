/* $Id: kHlpMemSet.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpString - kHlpMemSet.
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


KHLP_DECL(void *) kHlpMemSet(void *pv1, int ch, KSIZE cb)
{
    union
    {
        void *pv;
        KU8 *pb;
        KUPTR *pu;
    } u1;

    u1.pv = pv1;

    if (cb >= 32)
    {
        KUPTR u = ch & 0xff;
#if K_ARCH_BITS > 8
        u |= u << 8;
#endif
#if K_ARCH_BITS > 16
        u |= u << 16;
#endif
#if K_ARCH_BITS > 32
        u |= u << 32;
#endif
#if K_ARCH_BITS > 64
        u |= u << 64;
#endif

        while (cb > sizeof(KUPTR))
        {
            cb -= sizeof(KUPTR);
            *u1.pu++ = u;
        }
    }

    while (cb-- > 0)
        *u1.pb++ = ch;

    return pv1;
}

