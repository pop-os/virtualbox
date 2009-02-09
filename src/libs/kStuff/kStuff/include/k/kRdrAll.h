/* $Id: kRdrAll.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kRdr - The File Provider, All Details and Dependencies Included.
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

#ifndef ___k_kRdrAll_h___
#define ___k_kRdrAll_h___

#include <k/kDefs.h>
#include <k/kLdr.h>
#include <k/kRdr.h>

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup grp_kRdrAll   All
 * @addtogroup grp_kRdr
 * @{
 */

/**
 * File provider instance operations.
 */
typedef struct KRDROPS
{
    /** The name of this file provider. */
    const char *pszName;
    /** Pointer to the next file provider. */
    const struct KRDROPS *pNext;

    /** Try create a new file provider instance.
     *
     * @returns 0 on success, OS specific error code on failure.
     * @param   ppRdr       Where to store the file provider instance.
     * @param   pszFilename The filename to open.
     */
    int     (* pfnCreate)(  PPKRDR ppRdr, const char *pszFilename);
    /** Destroy the file provider instance.
     *
     * @returns 0 on success, OS specific error code on failure.
     *          On failure, the file provider instance will be in an indeterminate state - don't touch it!
     * @param   pRdr        The file provider instance.
     */
    int     (* pfnDestroy)( PKRDR pRdr);
    /** @copydoc kRdrRead */
    int     (* pfnRead)(    PKRDR pRdr, void *pvBuf, KSIZE cb, KFOFF off);
    /** @copydoc kRdrAllMap */
    int     (* pfnAllMap)(  PKRDR pRdr, const void **ppvBits);
    /** @copydoc kRdrAllUnmap */
    int     (* pfnAllUnmap)(PKRDR pRdr, const void *pvBits);
    /** @copydoc kRdrSize */
    KFOFF   (* pfnSize)(    PKRDR pRdr);
    /** @copydoc kRdrTell */
    KFOFF   (* pfnTell)(    PKRDR pRdr);
    /** @copydoc kRdrName */
    const char * (* pfnName)(PKRDR pRdr);
    /** @copydoc kRdrNativeFH */
    KIPTR  (* pfnNativeFH)(PKRDR pRdr);
    /** @copydoc kRdrPageSize */
    KSIZE   (* pfnPageSize)(PKRDR pRdr);
    /** @copydoc kRdrMap */
    int     (* pfnMap)(     PKRDR pRdr, void **ppvBase, KU32 cSegments, PCKLDRSEG paSegments, KBOOL fFixed);
    /** @copydoc kRdrRefresh */
    int     (* pfnRefresh)( PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments);
    /** @copydoc kRdrProtect */
    int     (* pfnProtect)( PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments, KBOOL fUnprotectOrProtect);
    /** @copydoc kRdrUnmap */
    int     (* pfnUnmap)(   PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments);
    /** @copydoc kRdrDone */
    void    (* pfnDone)(    PKRDR pRdr);
    /** The usual non-zero dummy that makes sure we've initialized all members. */
    KU32    u32Dummy;
} KRDROPS;
/** Pointer to file provider operations. */
typedef KRDROPS *PKRDROPS;
/** Pointer to const file provider operations. */
typedef const KRDROPS *PCKRDROPS;


/**
 * File provider instance core.
 */
typedef struct KRDR
{
    /** Magic number (KRDR_MAGIC). */
    KU32        u32Magic;
    /** Pointer to the file provider operations. */
    PCKRDROPS   pOps;
} KRDR;

void    kRdrAddProvider(PKRDROPS pAdd);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

