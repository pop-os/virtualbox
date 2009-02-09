/* $Id: kHlpAlloc.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpAlloc - Memory Allocation.
 */

/*
 * Copyright (c) 2006-2007 knut st. osmundsen <bird-src-spam@anduin.net>
 *
 * This file is part of kStuff.
 *
 * kStuff is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kStuff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with kStuff; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef ___k_kHlpAlloc_h___
#define ___k_kHlpAlloc_h___

#include <k/kHlpDefs.h>
#include <k/kTypes.h>

/** @defgroup grp_kHlpAlloc kHlpAlloc - Memory Allocation
 * @addtogroup grp_kHlp
 * @{*/

/** @def kHlpAllocA
 * The alloca() wrapper. */
#ifdef __GNUC__
# define kHlpAllocA(a)      __builtin_alloca(a)
#elif defined(_MSC_VER)
# include <malloc.h>
# define kHlpAllocA(a)      alloca(a)
#else
# error "Port Me."
#endif

#ifdef __cplusplus
extern "C" {
#endif

KHLP_DECL(void *)   kHlpAlloc(KSIZE cb);
KHLP_DECL(void *)   kHlpAllocZ(KSIZE cb);
KHLP_DECL(void *)   kHlpDup(const void *pv, KSIZE cb);
KHLP_DECL(char *)   kHlpStrDup(const char *psz);
KHLP_DECL(void *)   kHlpRealloc(void *pv, KSIZE cb);
KHLP_DECL(void)     kHlpFree(void *pv);

KHLP_DECL(int)      kHlpPageAlloc(void **ppv, KSIZE cb, KPROT enmProt, KBOOL fFixed);
KHLP_DECL(int)      kHlpPageProtect(void *pv, KSIZE cb, KPROT enmProt);
KHLP_DECL(int)      kHlpPageFree(void *pv, KSIZE cb);

KHLP_DECL(int)      kHlpHeapInit(void);
KHLP_DECL(void)     kHlpHeapTerm(void);
KHLP_DECL(void)     kHlpHeapDonate(void *pv, KSIZE cb);

#ifdef __cplusplus
}
#endif

/** @} */

#endif

