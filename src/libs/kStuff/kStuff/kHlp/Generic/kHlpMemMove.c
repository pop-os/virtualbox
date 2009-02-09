/* $Id: kHlpMemMove.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpString - kHlpMemMove.
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


KHLP_DECL(void *) kHlpMemMove(void *pv1, const void *pv2, KSIZE cb)
{
    union
    {
        void *pv;
        KU8 *pb;
        KUPTR *pu;
    } u1;
    union
    {
        const void *pv;
        volatile KU8 *pb;
        volatile KUPTR *pu;
    } u2;

    u1.pv = pv1;
    u2.pv = pv2;

    if ((KUPTR)u1.pb <= (KUPTR)u2.pb)
    {
        /* forward copy */
        if (cb >= 32)
        {
            while (cb > sizeof(KUPTR))
            {
                KUPTR u = *u2.pu++;
                *u1.pu++ = u;
                cb -= sizeof(KUPTR);
            }
        }

        while (cb-- > 0)
        {
            KU8 b = *u2.pb++;
            *u1.pb++ = b;
        }
    }
    else
    {
        /* backwards copy */
        u1.pb += cb;
        u2.pb += cb;

        if (cb >= 32)
        {
            while (cb > sizeof(KUPTR))
            {
                KUPTR u = *--u2.pu;
                *--u1.pu = u;
                cb -= sizeof(KUPTR);
            }
        }

        while (cb-- > 0)
        {
            KU8 b = *--u2.pb;
            *--u1.pb = b;
        }
    }

    return pv1;
}


