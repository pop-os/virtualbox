/* $Id: kRdr.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kRdr - The File Provider.
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

#ifndef ___kRdr_h___
#define ___kRdr_h___

#include <k/kDefs.h>
#include <k/kTypes.h>

/** @defgroup grp_kRdr      kRdr - The File Provider
 * @{ */

/** @def KRDR_DECL
 * Declares a kRdr function according to build context.
 * @param type          The return type.
 */
#if defined(KRDR_BUILDING_DYNAMIC)
# define KRDR_DECL(type)    K_DECL_EXPORT(type)
#elif defined(KRDR_BUILT_DYNAMIC)
# define KRDR_DECL(type)    K_DECL_IMPORT(type)
#else
# define KRDR_DECL(type)    type
#endif

#ifdef __cplusplus
extern "C" {
#endif

KRDR_DECL(int)      kRdrOpen(   PPKRDR ppRdr, const char *pszFilename);
KRDR_DECL(int)      kRdrClose(    PKRDR pRdr);
KRDR_DECL(int)      kRdrRead(     PKRDR pRdr, void *pvBuf, KSIZE cb, KFOFF off);
KRDR_DECL(int)      kRdrAllMap(   PKRDR pRdr, const void **ppvBits);
KRDR_DECL(int)      kRdrAllUnmap( PKRDR pRdr, const void *pvBits);
KRDR_DECL(KFOFF)    kRdrSize(     PKRDR pRdr);
KRDR_DECL(KFOFF)    kRdrTell(     PKRDR pRdr);
KRDR_DECL(const char *) kRdrName( PKRDR pRdr);
KRDR_DECL(KIPTR)    kRdrNativeFH( PKRDR pRdr);
KRDR_DECL(KSIZE)    kRdrPageSize( PKRDR pRdr);
KRDR_DECL(int)      kRdrMap(      PKRDR pRdr, void **ppvBase, KU32 cSegments, PCKLDRSEG paSegments, KBOOL fFixed);
KRDR_DECL(int)      kRdrRefresh(  PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments);
KRDR_DECL(int)      kRdrProtect(  PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments, KBOOL fUnprotectOrProtect);
KRDR_DECL(int)      kRdrUnmap(    PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments);
KRDR_DECL(void)     kRdrDone(     PKRDR pRdr);

KRDR_DECL(int)      kRdrBufOpen(PPKRDR ppRdr, const char *pszFilename);
KRDR_DECL(int)      kRdrBufWrap(PPKRDR ppRdr, PKRDR pRdr, KBOOL fCloseIt);
KRDR_DECL(KBOOL)    kRdrBufIsBuffered(PKRDR pRdr);
KRDR_DECL(int)      kRdrBufLine(PKRDR pRdr, char *pszLine, KSIZE cbLine);
KRDR_DECL(int)      kRdrBufLineEx(PKRDR pRdr, char *pszLine, KSIZE *pcbLine);
KRDR_DECL(const char *) kRdrBufLineQ(PKRDR pRdr);

#ifdef __cplusplus
}
#endif

/** @} */

#endif

