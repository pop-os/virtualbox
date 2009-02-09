/* $Id: tst.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kLdr testcase header.
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

#ifndef ___tst_h___
#define ___tst_h___

#include <k/kLdr.h>
#include <k/kHlp.h>

#if K_OS == K_OS_OS2 \
 || K_OS == K_OS_WINDOWS
# define MY_EXPORT(type) __declspec(dllexport) type
/*# define MY_IMPORT(type) extern __declspec(dllimport) type*/
# define MY_IMPORT(type) extern type
#else
# define MY_EXPORT(type) type
# define MY_IMPORT(type) extern type
#endif

#if K_OS == K_OS_OS2 \
 || K_OS == K_OS_DARWIN
# define MY_NAME(a) "_" a
#else
# define MY_NAME(a) a
#endif

extern const char *g_pszName;

#endif

