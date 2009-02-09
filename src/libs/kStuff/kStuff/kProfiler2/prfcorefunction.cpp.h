/* $Id: prfcorefunction.cpp.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kProfiler Mark 2 - Core NewFunction Code Template.
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
 * Creates a new function.
 *
 * @returns Pointer to the new function.
 * @returns NULL if we're out of space.
 */
static KPRF_TYPE(P,FUNC) KPRF_NAME(NewFunction)(KPRF_TYPE(P,HDR) pHdr,KPRF_TYPE(,UPTR) uPC)
{
    /*
     * First find the position of the function (it might actually have been inserted by someone else by now too).
     */
    KPRF_FUNCS_WRITE_LOCK();

    KPRF_TYPE(P,FUNC) paFunctions = KPRF_OFF2PTR(P,FUNC, pHdr->offFunctions, pHdr);
    KI32 iStart = 0;
    KI32 iLast  = pHdr->cFunctions - 1;
    KI32 i      = iLast / 2;
    for (;;)
    {
        KU32 iFunction = pHdr->aiFunctions[i];
        KPRF_TYPE(,IPTR) iDiff = uPC - paFunctions[iFunction].uEntryPtr;
        if (!iDiff)
        {
            KPRF_FUNCS_WRITE_UNLOCK();
            return &paFunctions[iFunction];
        }
        if (iLast == iStart)
            break;
        if (iDiff < 0)
            iLast = i - 1;
        else
            iStart = i + 1;
        if (iLast < iStart)
            break;
        i = iStart + (iLast - iStart) / 2;
    }

    /*
     * Adjust the index so we're exactly in the right spot.
     * (I've too much of a headache to figure out if the above loop leaves us where we should be.)
     */
    const KI32 iNew = pHdr->cFunctions;
    if (paFunctions[pHdr->aiFunctions[i]].uEntryPtr > uPC)
    {
        while (     i > 0
               &&   paFunctions[pHdr->aiFunctions[i - 1]].uEntryPtr > uPC)
            i--;
    }
    else
    {
        while (     i < iNew
               &&   paFunctions[pHdr->aiFunctions[i]].uEntryPtr < uPC)
            i++;
    }

    /*
     * Ensure that there still is space for the function.
     */
    if (iNew >= (KI32)pHdr->cMaxFunctions)
    {
        KPRF_FUNCS_WRITE_UNLOCK();
        return NULL;
    }
    pHdr->cFunctions++;
    KPRF_TYPE(P,FUNC) pNew = &paFunctions[iNew];

    /* init the new function entry */
    pNew->uEntryPtr  = uPC;
    pNew->offModSeg  = 0;
    pNew->cOnStack   = 0;
    pNew->cCalls     = 0;
    pNew->OnStack.MinTicks      = ~(KU64)0;
    pNew->OnStack.MaxTicks      = 0;
    pNew->OnStack.SumTicks      = 0;
    pNew->OnTopOfStack.MinTicks = ~(KU64)0;
    pNew->OnTopOfStack.MaxTicks = 0;
    pNew->OnTopOfStack.SumTicks = 0;

    /* shift the function index array and insert the new one. */
    KI32 j = iNew;
    while (j > i)
    {
        pHdr->aiFunctions[j] = pHdr->aiFunctions[j - 1];
        j--;
    }
    pHdr->aiFunctions[i] = iNew;
    KPRF_FUNCS_WRITE_UNLOCK();

    /*
     * Record the module segment (i.e. add it if it's new).
     */
    pNew->offModSeg = KPRF_NAME(RecordModSeg)(pHdr, uPC);

    return pNew;
}

