/* $Id: tst-3.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kLdr - Dynamic Loader testcase no. 3, Driver.
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

#include "tst.h"


int g_i1 = 1;
int g_i2 = 2;
int *g_pi1 = &g_i1;

extern int Tst3Sub(int);
int (*g_pfnTst3Sub)(int) = &Tst3Sub;

MY_IMPORT(int) Tst3Ext(int);
int (*g_pfnTst3Ext)(int) = &Tst3Ext;

char g_achBss[256];


MY_EXPORT(int) Tst3(int iFortyTwo)
{
    int rc;

    if (iFortyTwo != 42)
        return 0;
    if (g_i1 != 1)
        return 1;
    if (g_i2 != 2)
        return 2;
    if (g_pi1 != &g_i1)
        return 3;
    if (g_pfnTst3Sub != &Tst3Sub)
        return 4;
    rc = Tst3Sub(iFortyTwo);
    if (rc != g_pfnTst3Sub(iFortyTwo))
        return 5;
    rc = Tst3Ext(iFortyTwo);
    if (rc != 42)
        return 6;
    rc = g_pfnTst3Ext(iFortyTwo);
    if (rc != 42)
        return 7;
    for (rc = 0; rc < sizeof(g_achBss); rc++)
        if (g_achBss[rc])
            return 8;
    if (g_achBss[0] || g_achBss[1] || g_achBss[255])
        return 9;

    return 42;
}

