/* $Id: kRdrInternal.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kRdr - Internal Header.
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

#ifndef ___kRdrInternal_h___
#define ___kRdrInternal_h___

#include <k/kHlpAssert.h>
#include <k/kMagics.h>
#include <k/kRdrAll.h>
#include <k/kErrors.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup grp_kRdrInternal - Internals
 * @internal
 * @addtogroup grp_kRdr
 * @{
 */

/** @def KRDR_STRICT
 * If defined the kRdr assertions and other runtime checks will be enabled. */
#ifdef K_ALL_STRICT
# undef KRDR_STRICT
# define KRDR_STRICT
#endif

/** @name Our Assert macros
 * @{ */
#ifdef KRDR_STRICT
# define kRdrAssert(expr)                       kHlpAssert(expr)
# define kRdrAssertReturn(expr, rcRet)          kHlpAssertReturn(expr, rcRet)
# define kRdrAssertMsg(expr, msg)               kHlpAssertMsg(expr, msg)
# define kRdrAssertMsgReturn(expr, msg, rcRet)  kHlpAssertMsgReturn(expr, msg, rcRet)
#else   /* !KRDR_STRICT */
# define kRdrAssert(expr)                       do { } while (0)
# define kRdrAssertReturn(expr, rcRet)          do { if (!(expr)) return (rcRet); } while (0)
# define kRdrAssertMsg(expr, msg)               do { } while (0)
# define kRdrAssertMsgReturn(expr, msg, rcRet)  do { if (!(expr)) return (rcRet); } while (0)
#endif  /* !KRDR_STRICT */

#define kRdrAssertPtr(ptr)                      kRdrAssertMsg(K_VALID_PTR(ptr), ("%s = %p\n", #ptr, (ptr)))
#define kRdrAssertPtrReturn(ptr, rcRet)         kRdrAssertMsgReturn(K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)), (rcRet))
#define kRdrAssertPtrNull(ptr)                  kRdrAssertMsg(!(ptr) || K_VALID_PTR(ptr), ("%s = %p\n", #ptr, (ptr)))
#define kRdrAssertPtrNullReturn(ptr, rcRet)     kRdrAssertMsgReturn(!(ptr) || K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)), (rcRet))
#define kRdrAssertRC(rc)                        kRdrAssertMsg((rc) == 0, ("%s = %d\n", #rc, (rc)))
#define kRdrAssertRCReturn(rc, rcRet)           kRdrAssertMsgReturn((rc) == 0, ("%s = %d -> %d\n", #rc, (rc), (rcRet)), (rcRet))
#define kRdrAssertFailed()                      kRdrAssert(0)
#define kRdrAssertFailedReturn(rcRet)           kRdrAssertReturn(0, (rcRet))
#define kRdrAssertMsgFailed(msg)                kRdrAssertMsg(0, msg)
#define kRdrAssertMsgFailedReturn(msg, rcRet)   kRdrAssertMsgReturn(0, msg, (rcRet))
/** @} */

/** Return / crash validation of a reader argument. */
#define KRDR_VALIDATE_EX(pRdr, rc) \
    do  { \
        if (    (pRdr)->u32Magic != KRDR_MAGIC \
            ||  (pRdr)->pOps == NULL \
           )\
        { \
            return (rc); \
        } \
    } while (0)

/** Return / crash validation of a reader argument. */
#define KRDR_VALIDATE(pRdr) \
    KRDR_VALIDATE_EX(pRdr, KERR_INVALID_PARAMETER)

/** Return / crash validation of a reader argument. */
#define KRDR_VALIDATE_VOID(pRdr) \
    do  { \
        if (    !K_VALID_PTR(pRdr) \
            ||  (pRdr)->u32Magic != KRDR_MAGIC \
            ||  (pRdr)->pOps == NULL \
           )\
        { \
            return; \
        } \
    } while (0)


/** @name Built-in Providers
 * @{ */
extern const KRDROPS g_kRdrFileOps;
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif

