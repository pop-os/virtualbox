/* $Id: kHlpSys.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpSys - System Call Prototypes.
 */

/*
 * Copyright (c) 2007 knut st. osmundsen <bird-src-spam@anduin.net>
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

#ifndef ___k_kHlpSys_h___
#define ___k_kHlpSys_h___

#include <k/kHlpDefs.h>
#include <k/kTypes.h>

/** @defgroup grp_kHlpSys kHlpSys - System Call Prototypes
 * @addtogroup grp_kHlp
 * @{*/

#ifdef __cplusplus
extern "C" {
#endif

/* common unix stuff. */
#if K_OS == K_OS_DARWIN \
 || K_OS == K_OS_FREEBSD \
 || K_OS == K_OS_LINUX \
 || K_OS == K_OS_NETBSD \
 || K_OS == K_OS_OPENBSD \
 || K_OS == K_OS_SOLARIS
KSSIZE      kHlpSys_readlink(const char *pszPath, char *pszBuf, KSIZE cbBuf);
int         kHlpSys_open(const char *filename, int flags, int mode);
int         kHlpSys_close(int fd);
KFOFF       kHlpSys_lseek(int fd, int whench, KFOFF off);
KSSIZE      kHlpSys_read(int fd, void *pvBuf, KSIZE cbBuf);
KSSIZE      kHlpSys_write(int fd, const void *pvBuf, KSIZE cbBuf);
void       *kHlpSys_mmap(void *addr, KSIZE len, int prot, int flags, int fd, KI64 off);
int         kHlpSys_mprotect(void *addr, KSIZE len, int prot);
int         kHlpSys_munmap(void *addr, KSIZE len);
void        kHlpSys_exit(int rc);
#endif

/* specific */
#if K_OS == K_OS_DARWIN

#elif K_OS == K_OS_LINUX

#endif

#ifdef __cplusplus
}
#endif

/** @} */

#endif


