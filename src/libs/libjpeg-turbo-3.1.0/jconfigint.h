/* $Id: jconfigint.h $ */
/** @file
 * libjpeg-turbo - This contains only the stuff we can't do in Makefile.kmk.
 * The rest of the preprocessor / constant stuff is done in Makefile.kmk.
 */

/*
 * Copyright (C) 2010-2025 Oracle and/or its affiliates.
 *
 * This file is part of VirtualBox base platform packages, as
 * available from https://www.virtualbox.org.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, in version 3 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef VBOX_INCLUDED_SRC_libjpeg_turbo_3_1_0_jconfigint_h
#define VBOX_INCLUDED_SRC_libjpeg_turbo_3_1_0_jconfigint_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#if defined(__clang__) || !defined _MSC_VER
#define HIDDEN                 __attribute__((visibility("hidden")))
#endif

#ifdef _MSC_VER

# define HAVE_INTRIN_H          1
# define HAVE_BITSCANFORWARD    1
# if (SIZEOF_SIZE_T) == 8
#  define HAVE_BITSCANFORWARD64 1
# endif

# define THREAD_LOCAL           __declspec(thread)

# define INLINE                 __forceinline

#else

# if defined(RT_ARCH_ARM64)
#  define HAVE_BUILTIN_CTZL     1
# endif

# define THREAD_LOCAL           __thread

# define INLINE                 __inline__ __attribute__((always_inline))

# ifdef __has_attribute
#  if __has_attribute(fallthrough)
#   define FALLTHROUGH          __attribute__((fallthrough));
#  endif
# endif

#endif

#ifndef FALLTHROUGH
# define FALLTHROUGH
#endif

#ifndef HIDDEN
# define HIDDEN
#endif

/*
 * Define BITS_IN_JSAMPLE as either
 *   8   for 8-bit sample values (the usual setting)
 *   12  for 12-bit sample values
 * Only 8 and 12 are legal data precisions for lossy JPEG according to the
 * JPEG standard, and the IJG code does not support anything else!
 */

#ifndef BITS_IN_JSAMPLE
# define BITS_IN_JSAMPLE  8      /* use 8 or 12 */
#endif

#undef C_ARITH_CODING_SUPPORTED
#undef D_ARITH_CODING_SUPPORTED

#if BITS_IN_JSAMPLE == 8

/* Support arithmetic encoding */
# define C_ARITH_CODING_SUPPORTED 1

/* Support arithmetic decoding */
# define D_ARITH_CODING_SUPPORTED 1


#endif

#endif /* !VBOX_INCLUDED_SRC_libjpeg_turbo_3_1_0_jconfigint_h */
