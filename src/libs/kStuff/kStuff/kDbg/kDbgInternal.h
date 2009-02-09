/* $Id: kDbgInternal.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kDbg - The Debug Info Reader, Internal Header.
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

#ifndef ___kDbgInternal_h___
#define ___kDbgInternal_h___

#include <k/kHlpAssert.h>
#include <k/kMagics.h>
#include <k/kErrors.h>
#include <k/kDbgAll.h>


/** @defgroup grp_kDbgInternal  Internal
 * @internal
 * @addtogroup grp_kDbg
 * @{
 */

/** @def KDBG_STRICT
 * If defined the kDbg assertions and other runtime checks will be enabled. */
#ifdef K_ALL_STRICT
# undef KDBG_STRICT
# define KDBG_STRICT
#endif

/** @name Our Assert macros
 * @{ */
#ifdef KDBG_STRICT
# define kDbgAssert(expr)                       kHlpAssert(expr)
# define kDbgAssertReturn(expr, rcRet)          kHlpAssertReturn(expr, rcRet)
# define kDbgAssertReturnVoid(expr)             kHlpAssertReturnVoid(expr)
# define kDbgAssertMsg(expr, msg)               kHlpAssertMsg(expr, msg)
# define kDbgAssertMsgReturn(expr, msg, rcRet)  kHlpAssertMsgReturn(expr, msg, rcRet)
# define kDbgAssertMsgReturnVoid(expr, msg)     kHlpAssertMsgReturnVoid(expr, msg)
#else   /* !KDBG_STRICT */
# define kDbgAssert(expr)                       do { } while (0)
# define kDbgAssertReturn(expr, rcRet)          do { if (!(expr)) return (rcRet); } while (0)
# define kDbgAssertMsg(expr, msg)               do { } while (0)
# define kDbgAssertMsgReturn(expr, msg, rcRet)  do { if (!(expr)) return (rcRet); } while (0)
#endif  /* !KDBG_STRICT */

#define kDbgAssertPtr(ptr)                      kDbgAssertMsg(K_VALID_PTR(ptr), ("%s = %p\n", #ptr, (ptr)))
#define kDbgAssertPtrReturn(ptr, rcRet)         kDbgAssertMsgReturn(K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)), (rcRet))
#define kDbgAssertPtrReturnVoid(ptr)            kDbgAssertMsgReturnVoid(K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)))
#define kDbgAssertPtrNull(ptr)                  kDbgAssertMsg(!(ptr) || K_VALID_PTR(ptr), ("%s = %p\n", #ptr, (ptr)))
#define kDbgAssertPtrNullReturn(ptr, rcRet)     kDbgAssertMsgReturn(!(ptr) || K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)), (rcRet))
#define kDbgAssertPtrNullReturnVoid(ptr)        kDbgAssertMsgReturnVoid(!(ptr) || K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)))
#define kDbgAssertRC(rc)                        kDbgAssertMsg((rc) == 0, ("%s = %d\n", #rc, (rc)))
#define kDbgAssertRCReturn(rc, rcRet)           kDbgAssertMsgReturn((rc) == 0, ("%s = %d -> %d\n", #rc, (rc), (rcRet)), (rcRet))
#define kDbgAssertRCReturnVoid(rc)              kDbgAssertMsgReturnVoid((rc) == 0, ("%s = %d -> %d\n", #rc, (rc), (rcRet)))
#define kDbgAssertFailed()                      kDbgAssert(0)
#define kDbgAssertFailedReturn(rcRet)           kDbgAssertReturn(0, (rcRet))
#define kDbgAssertFailedReturnVoid()            kDbgAssertReturnVoid(0)
#define kDbgAssertMsgFailed(msg)                kDbgAssertMsg(0, msg)
#define kDbgAssertMsgFailedReturn(msg, rcRet)   kDbgAssertMsgReturn(0, msg, (rcRet))
#define kDbgAssertMsgFailedReturnVoid(msg)      kDbgAssertMsgReturnVoid(0, msg)
/** @} */

/** Return / crash validation of a reader argument. */
#define KDBGMOD_VALIDATE_EX(pDbgMod, rc) \
    do  { \
        kDbgAssertPtrReturn((pDbgMod), (rc)); \
        kDbgAssertReturn((pDbgMod)->u32Magic == KDBGMOD_MAGIC, (rc)); \
        kDbgAssertReturn((pDbgMod)->pOps != NULL, (rc)); \
    } while (0)

/** Return / crash validation of a reader argument. */
#define KDBGMOD_VALIDATE(pDbgMod) \
    do  { \
        kDbgAssertPtrReturn((pDbgMod), KERR_INVALID_POINTER); \
        kDbgAssertReturn((pDbgMod)->u32Magic == KDBGMOD_MAGIC, KERR_INVALID_HANDLE); \
        kDbgAssertReturn((pDbgMod)->pOps != NULL, KERR_INVALID_HANDLE); \
    } while (0)

/** Return / crash validation of a reader argument. */
#define KDBGMOD_VALIDATE_VOID(pDbgMod) \
    do  { \
        kDbgAssertPtrReturnVoid((pDbgMod)); \
        kDbgAssertReturnVoid((pDbgMod)->u32Magic == KDBGMOD_MAGIC); \
        kDbgAssertReturnVoid((pDbgMod)->pOps != NULL); \
    } while (0)


#ifdef __cplusplus
extern "C" {
#endif

/** @name Built-in Debug Module Readers
 * @{ */
extern KDBGMODOPS const g_kDbgModWinDbgHelpOpen;
extern KDBGMODOPS const g_kDbgModLdr;
extern KDBGMODOPS const g_kDbgModCv8;
extern KDBGMODOPS const g_kDbgModDwarf;
extern KDBGMODOPS const g_kDbgModHll;
extern KDBGMODOPS const g_kDbgModStabs;
extern KDBGMODOPS const g_kDbgModSym;
extern KDBGMODOPS const g_kDbgModMapILink;
extern KDBGMODOPS const g_kDbgModMapMSLink;
extern KDBGMODOPS const g_kDbgModMapNm;
extern KDBGMODOPS const g_kDbgModMapWLink;
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

#endif

