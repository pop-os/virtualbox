/* $Id: kPrf2WinApiWrapperHlp.h 13 2008-04-20 10:13:43Z bird $ */
/** @file
 * Helpers for the Windows API wrapper DLL.
 */

/*
 * Copyright (c) 2008 knut st. osmundsen <bird-src-spam@anduin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

typedef struct KPRF2WRAPDLL
{
    HMODULE hmod;
    char szName[32];
} KPRF2WRAPDLL;
typedef KPRF2WRAPDLL *PKPRF2WRAPDLL;
typedef KPRF2WRAPDLL const *PCKPRF2WRAPDLL;

FARPROC kPrf2WrapResolve(void **ppfn, const char *pszName, PKPRF2WRAPDLL pDll);


