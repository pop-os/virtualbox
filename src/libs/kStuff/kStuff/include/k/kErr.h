/* $Id: kErr.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kErr - Status Code API.
 */

/*
 * Copyright (c) 2007 knut st. osmundsen <bird-src-spam@anduin.net>
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

#ifndef ___k_kErr_h___
#define ___k_kErr_h___

/** @defgroup   grp_kErr        kErr - Status Code API
 * @{
 */

/** @def KERR_DECL
 * Declares a kRdr function according to build context.
 * @param type          The return type.
 */
#if defined(KERR_BUILDING_DYNAMIC)
# define KERR_DECL(type)    K_DECL_EXPORT(type)
#elif defined(KRDR_BUILT_DYNAMIC)
# define KERR_DECL(type)    K_DECL_IMPORT(type)
#else
# define KERR_DECL(type)    type
#endif

#ifdef __cplusplus
extern "C" {
#endif

KERR_DECL(const char *) kErrName(int rc);
KERR_DECL(int)  kErrFromErrno(int);
KERR_DECL(int)  kErrFromOS2(unsigned long rcOs2);
KERR_DECL(int)  kErrFromNtStatus(long rcNtStatus);
KERR_DECL(int)  kErrFromMach(int rcMach);
KERR_DECL(int)  kErrFromDarwin(int rcDarwin);

#ifdef __cplusplus
}
#endif

/** @} */

#endif

