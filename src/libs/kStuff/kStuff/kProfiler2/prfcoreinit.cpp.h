/* $Id: prfcoreinit.cpp.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kProfiler Mark 2 - Core Initialization Code Template.
 */

/*
 * Copyright (c) 2006-2007 knut st. osmundsen <bird-src-spam@anduin.net>
 *
 * This file is part of kProfiler.
 *
 * kProfiler is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * kProfiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with kProfiler; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


/**
 * Calculates the size of the profiler data set.
 *
 * @returns The size of the data set in bytes.
 *
 * @param   cMaxFunctions       The max number of functions.
 * @param   cbMaxModSeg         The max bytes for module segments.
 * @param   cMaxThreads         The max number of threads.
 * @param   cMaxStacks          The max number of stacks. (should be less or equal to the max number of threads)
 * @param   cMaxStackFrames     The max number of frames on each of the stacks.
 *
 * @remark  This function does not input checks, it only aligns it. The caller is
 *          responsible for the input to make some sense.
 */
KPRF_DECL_FUNC(KU32, CalcSize)(KU32 cMaxFunctions, KU32 cbMaxModSegs, KU32 cMaxThreads, KU32 cMaxStacks, KU32 cMaxStackFrames)
{
    /*
     * Normalize input.
     */
    KPRF_SETMIN_ALIGN(cMaxFunctions, 16, 16);
    KPRF_SETMIN_ALIGN(cbMaxModSegs, KPRF_SIZEOF(MODSEG), 32);
    KPRF_SETMIN_ALIGN(cMaxThreads, 1, 1);
    KPRF_SETMIN_ALIGN(cMaxStacks, 1, 1);
    KPRF_SETMIN_ALIGN(cMaxStackFrames, 32, 32);

    /*
     * Calc the size from the input.
     * We do not take overflows into account, stupid user means stupid result.
     */
    KU32 cb = KPRF_OFFSETOF(HDR, aiFunctions[cMaxFunctions]);
    KU32 cbTotal = KPRF_ALIGN(cb, 32);

    cb = cMaxFunctions * KPRF_SIZEOF(FUNC);
    cbTotal += KPRF_ALIGN(cb, 32);

    cbTotal += cbMaxModSegs;

    cb = cMaxThreads * KPRF_SIZEOF(THREAD);
    cbTotal += KPRF_ALIGN(cb, 32);

    cb = cMaxStacks * KPRF_SIZEOF(STACK);
    cbTotal += KPRF_ALIGN(cb, 32);

    cb = cMaxStackFrames * cMaxStacks * KPRF_SIZEOF(FRAME);
    cbTotal += KPRF_ALIGN(cb, 32);

    return cbTotal;
}


/**
 * Initializes the profiler data set.
 *
 * @returns Pointer to the initialized profiler header on success.
 * @returns NULL if the input doesn't add up.
 *
 * @param   pvData              Where to initialize the profiler data set.
 * @param   cbData              The size of the available data.
 * @param   cMaxFunctions       The max number of functions.
 * @param   cbMaxModSeg         The max bytes for module segments.
 * @param   cMaxThreads         The max number of threads.
 * @param   cMaxStacks          The max number of stacks. (should be less or equal to the max number of threads)
 * @param   cMaxStackFrames     The max number of frames on each of the stacks.
 *
 */
KPRF_DECL_FUNC(KPRF_TYPE(P,HDR), Init)(void *pvData, KU32 cbData, KU32 cMaxFunctions, KU32 cbMaxModSegs,
                                       KU32 cMaxThreads, KU32 cMaxStacks, KU32 cMaxStackFrames)
{
    /*
     * Normalize the input.
     */
    if (!pvData)
        return NULL;
    KPRF_SETMIN_ALIGN(cMaxFunctions, 16, 16);
    KPRF_SETMIN_ALIGN(cbMaxModSegs, KPRF_SIZEOF(MODSEG), 32);
    KPRF_SETMIN_ALIGN(cMaxThreads, 1, 1);
    KPRF_SETMIN_ALIGN(cMaxStacks, 1, 1);
    KPRF_SETMIN_ALIGN(cMaxStackFrames, 32, 32);

    /*
     * The header.
     */
    KU32 off = 0;
    KU32 cb = KPRF_OFFSETOF(HDR, aiFunctions[cMaxFunctions]);
    cb = KPRF_ALIGN(cb, 32);
    if (cbData < off + cb || off > off + cb)
        return NULL;
    KPRF_TYPE(P,HDR) pHdr = (KPRF_TYPE(P,HDR))pvData;

    /* the core header */
    pHdr->u32Magic          = 0;        /* Set at the very end */
    pHdr->cFormatBits       = KPRF_BITS;
    pHdr->uBasePtr          = 0;        /* Can be set afterwards using SetBasePtr. */
#if KPRF_BITS <= 16
    pHdr->u16Reserved       = 0;
#endif
#if KPRF_BITS <= 32
    pHdr->u32Reserved       = 0;
#endif
    pHdr->cb                = cbData;
    pHdr->cbAllocated       = cbData;

    /* functions */
    off += cb;
    cb = cMaxFunctions * KPRF_SIZEOF(FUNC);
    cb = KPRF_ALIGN(cb, 32);
    if (cbData < off + cb || off > off + cb)
        return NULL;
    pHdr->cMaxFunctions     = cMaxFunctions;
    pHdr->cFunctions        = 0;
    pHdr->offFunctions      = off;
    pHdr->cbFunction        = KPRF_SIZEOF(FUNC);

    /* modsegs */
    off += cb;
    cb = KPRF_ALIGN(cbMaxModSegs, 32);
    if (cbData < off + cb || off > off + cb)
        return NULL;
    pHdr->cbMaxModSegs      = cbMaxModSegs;
    pHdr->cbModSegs         = 0;
    pHdr->offModSegs        = off;

    /* threads */
    off += cb;
    cb = cMaxThreads * KPRF_SIZEOF(THREAD);
    cb = KPRF_ALIGN(cb, 32);
    if (cbData < off + cb || off > off + cb)
        return NULL;
    pHdr->cMaxThreads       = cMaxThreads;
    pHdr->cThreads          = 0;
    pHdr->offThreads        = off;
    pHdr->cbThread          = KPRF_SIZEOF(THREAD);

    /* stacks */
    off += cb;
    cb = cMaxStacks * KPRF_OFFSETOF(STACK, aFrames[cMaxStackFrames]);
    cb = KPRF_ALIGN(cb, 32);
    if (cbData < off + cb || off > off + cb)
        return NULL;
    pHdr->cMaxStacks        = cMaxStacks;
    pHdr->cStacks           = 0;
    pHdr->offStacks         = off;
    pHdr->cbStack           = KPRF_OFFSETOF(STACK, aFrames[cMaxStackFrames]);
    pHdr->cMaxStackFrames   = cMaxStackFrames;

    /* commandline */
    pHdr->offCommandLine    = 0;
    pHdr->cchCommandLine    = 0;

    /* the final size */
    pHdr->cb                = off + cb;


    /*
     * Done.
     */
    pHdr->u32Magic = KPRF_TYPE(,HDR_MAGIC);
    return pHdr;
}

