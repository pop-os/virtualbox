/* $Id: kHlpAssert.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpAssert - Assertion Macros.
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

#ifndef ___kHlpAssert_h___
#define ___kHlpAssert_h___

#include <k/kHlpDefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup   grp_kHlpAssert - Assertion Macros
 * @addtogroup grp_kHlp
 * @{ */

/** @def K_STRICT
 * Assertions are enabled when K_STRICT is \#defined. */

/** @def kHlpAssertBreakpoint
 * Emits a breakpoint instruction or somehow triggers a debugger breakpoint.
 */
#ifdef _MSC_VER
# define kHlpAssertBreakpoint() do { __debugbreak(); } while (0)
#elif defined(__GNUC__)
# define kHlpAssertBreakpoint() do { __asm__ __volatile__ ("int3"); } while (0)
#else
# error "Port Me"
#endif

#ifdef K_STRICT

# define kHlpAssert(expr) \
    do { \
        if (!(expr)) \
        { \
            kHlpAssertMsg1(#expr, __FILE__, __LINE__, __FUNCTION__); \
            kHlpAssertBreakpoint(); \
        }
    } while (0)

# define kHlpAssertReturn(expr, rcRet) \
    do { \
        if (!(expr)) \
        { \
            kHlpAssertMsg1(#expr, __FILE__, __LINE__, __FUNCTION__); \
            kHlpAssertBreakpoint(); \
            return (rcRet); \
        }
    } while (0)

# define kHlpAssertReturnVoid(expr) \
    do { \
        if (!(expr)) \
        { \
            kHlpAssertMsg1(#expr, __FILE__, __LINE__, __FUNCTION__); \
            kHlpAssertBreakpoint(); \
            return; \
        }
    } while (0)

# define kHlpAssertMsg(expr, msg) \
    do { \
        if (!(expr)) \
        { \
            kHlpAssertMsg1(#expr, __FILE__, __LINE__, __FUNCTION__); \
            kHlpAssertMsg2 msg; \
            kHlpAssertBreakpoint(); \
        }
    } while (0)

# define kHlpAssertMsgReturn(expr, msg, rcRet) \
    do { \
        if (!(expr)) \
        { \
            kHlpAssertMsg1(#expr, __FILE__, __LINE__, __FUNCTION__); \
            kHlpAssertMsg2 msg; \
            kHlpAssertBreakpoint(); \
            return (rcRet); \
        }
    } while (0)

# define kHlpAssertMsgReturnVoid(expr, msg) \
    do { \
        if (!(expr)) \
        { \
            kHlpAssertMsg1(#expr, __FILE__, __LINE__, __FUNCTION__); \
            kHlpAssertMsg2 msg; \
            kHlpAssertBreakpoint(); \
            return; \
        }
    } while (0)

#else   /* !K_STRICT */
# define kHlpAssert(expr)                       do { } while (0)
# define kHlpAssertReturn(expr, rcRet)          do { if (!(expr)) return (rcRet); } while (0)
# define kHlpAssertReturnVoid(expr)             do { if (!(expr)) return; } while (0)
# define kHlpAssertMsg(expr, msg)               do { } while (0)
# define kHlpAssertMsgReturn(expr, msg, rcRet)  do { if (!(expr)) return (rcRet); } while (0)
# define kHlpAssertMsgReturnVoid(expr, msg)     do { if (!(expr)) return; } while (0)
#endif  /* !K_STRICT */

#define kHlpAssertPtr(ptr)                      kHlpAssertMsg(K_VALID_PTR(ptr), ("%s = %p\n", #ptr, (ptr)))
#define kHlpAssertPtrReturn(ptr, rcRet)         kHlpAssertMsgReturn(K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)), (rcRet))
#define kHlpAssertPtrReturnVoid(ptr)            kHlpAssertMsgReturnVoid(K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)))
#define kHlpAssertPtrNull(ptr)                  kHlpAssertMsg(!(ptr) || K_VALID_PTR(ptr), ("%s = %p\n", #ptr, (ptr)))
#define kHlpAssertPtrNullReturn(ptr, rcRet)     kHlpAssertMsgReturn(!(ptr) || K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)), (rcRet))
#define kHlpAssertPtrNullReturnVoid(ptr)        kHlpAssertMsgReturnVoid(!(ptr) || K_VALID_PTR(ptr), ("%s = %p -> %d\n", #ptr, (ptr), (rcRet)))
#define kHlpAssertRC(rc)                        kHlpAssertMsg((rc) == 0, ("%s = %d\n", #rc, (rc)))
#define kHlpAssertRCReturn(rc, rcRet)           kHlpAssertMsgReturn((rc) == 0, ("%s = %d -> %d\n", #rc, (rc), (rcRet)), (rcRet))
#define kHlpAssertRCReturnVoid(rc)              kHlpAssertMsgReturnVoid((rc) == 0, ("%s = %d -> %d\n", #rc, (rc), (rcRet)))
#define kHlpAssertFailed()                      kHlpAssert(0)
#define kHlpAssertFailedReturn(rcRet)           kHlpAssertReturn(0, (rcRet))
#define kHlpAssertFailedReturnVoid()            kHlpAssertReturnVoid(0)
#define kHlpAssertMsgFailed(msg)                kHlpAssertMsg(0, msg)
#define kHlpAssertMsgFailedReturn(msg, rcRet)   kHlpAssertMsgReturn(0, msg, (rcRet))
#define kHlpAssertMsgFailedReturnVoid(msg)      kHlpAssertMsgReturnVoid(0, msg))

/**
 * Helper function that displays the first part of the assertion message.
 *
 * @param   pszExpr         The expression.
 * @param   pszFile         The file name.
 * @param   iLine           The line number is the file.
 * @param   pszFunction     The function name.
 * @internal
 */
KHLP_DECL(void) kHlpAssertMsg1(const char *pszExpr, const char *pszFile, unsigned iLine, const char *pszFunction);

/**
 * Helper function that displays custom assert message.
 *
 * @param   pszFormat       Format string that get passed to vprintf.
 * @param   ...             Format arguments.
 * @internal
 */
KHLP_DECL(void) kHlpAssertMsg2(const char *pszFormat, ...);


/** @} */

#ifdef __cplusplus
}
#endif

#endif
