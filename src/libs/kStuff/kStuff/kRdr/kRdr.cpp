/* $Id: kRdr.cpp 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kRdr - The File Provider.
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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "kRdrInternal.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** The list of file providers. */
static PCKRDROPS g_pRdrHead = &g_kRdrFileOps;


/**
 * Adds a new file provider.
 *
 * @param   pAdd        The new file provider.
 */
KRDR_DECL(void) kRdrAddProvider(PKRDROPS pAdd)
{
    pAdd->pNext = g_pRdrHead;
    g_pRdrHead = pAdd;
}


/**
 * Tries to opens a file.
 *
 * @returns 0 on success, OS status code on failure.
 * @param   ppRdr           Where to store the file provider instance.
 * @param   pszFilename     The filename.
 */
KRDR_DECL(int) kRdrOpen(PPKRDR ppRdr, const char *pszFilename)
{
    int             rc = -1;
    PCKRDROPS    pCur;
    for (pCur = g_pRdrHead; pCur; pCur = pCur->pNext)
    {
        rc = pCur->pfnCreate(ppRdr, pszFilename);
        if (!rc)
            return 0;
    }
    return rc;
}


/**
 * Closes the file.
 *
 * @returns 0 on success, OS specific error code on failure.
 *          On failure, the file provider instance will be in an indeterminate state - don't touch it!
 * @param   pRdr        The file provider instance.
 */
KRDR_DECL(int) kRdrClose(PKRDR pRdr)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnDestroy(pRdr);
}


/** Read bits from the file.
 *
 * @returns 0 on success, OS specific error code on failure.
 * @param   pRdr        The file provider instance.
 * @param   pvBuf       Where to put the bits.
 * @param   cb          The number of bytes to read.
 * @param   off         Where to start reading.
 */
KRDR_DECL(int) kRdrRead(PKRDR pRdr, void *pvBuf, KSIZE cb, KFOFF off)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnRead(pRdr, pvBuf, cb, off);
}


/** Map all the file bits into memory (read only).
 *
 * @returns 0 on success, OS specific error code on failure.
 * @param   pRdr        The file provider instance.
 * @param   ppvBits     Where to store the address of the mapping.
 *                      The size can be obtained using pfnSize.
 */
KRDR_DECL(int) kRdrAllMap(PKRDR pRdr, const void **ppvBits)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnAllMap(pRdr, ppvBits);
}


/** Unmap a file bits mapping obtained by KRDROPS::pfnAllMap.
 *
 * @returns 0 on success, OS specific error code on failure.
 * @param   pRdr        The file provider instance.
 * @param   pvBits      The mapping address.
 */
KRDR_DECL(int) kRdrAllUnmap(PKRDR pRdr, const void *pvBits)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnAllUnmap(pRdr, pvBits);
}


/** Get the file size.
 *
 * @returns The file size. Returns -1 on failure.
 * @param   pRdr        The file provider instance.
 */
KRDR_DECL(KFOFF) kRdrSize(PKRDR pRdr)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnSize(pRdr);
}


/** Get the file pointer offset.
 *
 * @returns The file pointer offset. Returns -1 on failure.
 * @param   pRdr        The file provider instance.
 */
KRDR_DECL(KFOFF) kRdrTell(PKRDR pRdr)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnTell(pRdr);
}


/** Get the file name.
 *
 * @returns The file name. Returns NULL on failure.
 * @param   pRdr        The file provider instance.
 */
KRDR_DECL(const char *) kRdrName(PKRDR pRdr)
{
    KRDR_VALIDATE_EX(pRdr, NULL);
    return pRdr->pOps->pfnName(pRdr);
}


/** Get the native file handle if possible.
 *
 * @returns The native file handle. Returns -1 if not available.
 * @param   pRdr        The file provider instance.
 */
KRDR_DECL(KIPTR) kRdrNativeFH(PKRDR pRdr)
{
    KRDR_VALIDATE_EX(pRdr, -1);
    return pRdr->pOps->pfnNativeFH(pRdr);
}


/**
 * Gets the page size used when mapping sections of the file.
 *
 * @returns The page size.
 * @param   pRdr        The file provider instance.
 */
KRDR_DECL(KSIZE) kRdrPageSize(PKRDR pRdr)
{
    KRDR_VALIDATE_EX(pRdr, 0x10000);
    return pRdr->pOps->pfnPageSize(pRdr);
}


/**
 * Maps the segments of a image into memory.
 *
 * The file reader will be using the RVA member of each segment to figure out where
 * it goes relative to the image base address.
 *
 * @returns 0 on success, OS specific error code on failure.
 * @param   pRdr        The file provider instance.
 * @param   ppvBase     On input when fFixed is set, this contains the base address of the mapping.
 *                      On output this contains the base of the image mapping.
 * @param   cSegments   The number of segments in the array pointed to by paSegments.
 * @param   paSegments  The segments thats going to be mapped.
 * @param   fFixed      If set, the address at *ppvBase should be the base address of the mapping.
 */
KRDR_DECL(int) kRdrMap(PKRDR pRdr, void **ppvBase, KU32 cSegments, PCKLDRSEG paSegments, KBOOL fFixed)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnMap(pRdr, ppvBase, cSegments, paSegments, fFixed);
}


/**
 * Reloads dirty pages in mapped image.
 *
 * @returns 0 on success, OS specific error code on failure.
 * @param   pRdr        The file provider instance.
 * @param   pvBase      The base address of the image mapping.
 * @param   cSegments   The number of segments in the array pointed to by paSegments.
 * @param   paSegments  The segments thats going to be mapped.
 */
KRDR_DECL(int) kRdrRefresh(PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnRefresh(pRdr, pvBase, cSegments, paSegments);
}


/**
 * Protects or unprotects an image mapping.
 *
 * This is typically used for getting write access to read or execute only
 * pages while applying fixups.
 *
 * @returns 0 on success, OS specific error code on failure.
 * @param   pRdr        The file provider instance.
 * @param   pvBase      The base address of the image mapping.
 * @param   cSegments   The number of segments in the array pointed to by paSegments.
 * @param   paSegments  The segments thats going to be mapped.
 * @param   fUnprotectOrProtect     When set the all mapped segments are made writable.
 *                                  When clean the segment protection is restored.
 */
KRDR_DECL(int) kRdrProtect(PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments, KBOOL fUnprotectOrProtect)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnProtect(pRdr, pvBase, cSegments, paSegments, fUnprotectOrProtect);
}


/**
 * Unmaps a image mapping.
 *
 * @returns 0 on success, OS specific error code on failure.
 * @param   pRdr        The file provider instance.
 * @param   pvBase      The base address of the image mapping.
 * @param   cSegments   The number of segments in the array pointed to by paSegments.
 * @param   paSegments  The segments thats going to be mapped.
 */
KRDR_DECL(int) kRdrUnmap(PKRDR pRdr, void *pvBase, KU32 cSegments, PCKLDRSEG paSegments)
{
    KRDR_VALIDATE(pRdr);
    return pRdr->pOps->pfnUnmap(pRdr, pvBase, cSegments, paSegments);
}


/**
 * We're done reading from the file but would like to keep file mappings.
 *
 * If the OS support closing the file handle while the file is mapped,
 * the reader should do so.
 *
 * @param   pRdr        The file provider instance.
 */
KRDR_DECL(void) kRdrDone(PKRDR pRdr)
{
    KRDR_VALIDATE_VOID(pRdr);
    pRdr->pOps->pfnDone(pRdr);
}

