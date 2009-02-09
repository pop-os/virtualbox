/* $Id: kCpu.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kCpu - The CPU and Architecture API.
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

#ifndef ___k_kCpu_h___
#define ___k_kCpu_h___

#include <k/kDefs.h>
#include <k/kTypes.h>
#include <k/kCpus.h>


/** @defgroup grp_kCpu      kCpu  - The CPU And Architecture API
 * @{
 */

/** @def KCPU_DECL
 * Declares a kCpu function according to build context.
 * @param type          The return type.
 */
#if defined(KCPU_BUILDING_DYNAMIC)
# define KCPU_DECL(type)        K_DECL_EXPORT(type)
#elif defined(KCPU_BUILT_DYNAMIC)
# define KCPU_DECL(type)        K_DECL_IMPORT(type)
#else
# define KCPU_DECL(type)        type
#endif

#ifdef __cplusplus
extern "C" {
#endif

KCPU_DECL(void) kCpuGetArchAndCpu(PKCPUARCH penmArch, PKCPU penmCpu);
KCPU_DECL(int)  kCpuCompare(KCPUARCH enmCodeArch, KCPU enmCodeCpu, KCPUARCH enmArch, KCPU enmCpu);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
