/* $Id: VUSBUrbPool.cpp $ */
/** @file
 * Virtual USB - URB pool.
 */

/*
 * Copyright (C) 2016-2025 Oracle and/or its affiliates.
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


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#define LOG_GROUP LOG_GROUP_DRV_VUSB
#include <VBox/log.h>
#include <iprt/errcore.h>
#include <iprt/mem.h>
#include <iprt/critsect.h>

#include "VUSBInternal.h"


/*********************************************************************************************************************************
*   Defined Constants And Macros                                                                                                 *
*********************************************************************************************************************************/

/** Maximum age for one URB buffer. */
#define VUSBURB_AGE_MAX 10

/** Convert from a URB to the URB buffer header. */
#define VUSBURBPOOL_URB_2_URBHDR(a_pUrb) RT_FROM_MEMBER(a_pUrb->pbData, VUSBURBHDR, abHdrData);


/*********************************************************************************************************************************
*   Structures and Typedefs                                                                                                      *
*********************************************************************************************************************************/

typedef enum {
    VUSBHDRSTATE_INVALID,   /* 0 */
    VUSBHDRSTATE_FREE,      /* 1 - buffer is free */
    VUSBHDRSTATE_USED       /* 2 - buffer is in use */
} VUSBHDRSTATE;

/**
 * URB header not visible to the caller allocating an URB
 * and only for internal tracking.
 */
typedef struct VUSBURBHDR
{
    /** List node for keeping the URB in the free list. */
    RTLISTNODE      NdFree;
    /** Size of the data allocated for the URB (Only the variable part including the
     * HCI and TDs). */
    size_t          cbAllocated;
    /** Size of the data that's assumed to have been used. */
    uint32_t        cbData;
    /** Age of the URB waiting on the list, if it is waiting for too long without being used
     * again it will be freed. */
    uint32_t        cAge;
    /** State of the associated buffer. */
    VUSBHDRSTATE    enmHdrState;
#if HC_ARCH_BITS == 64
    uint32_t        u32Alignment0;
#endif
    /** The data immediately following this header. */
    RT_FLEXIBLE_ARRAY_EXTENSION
    uint8_t         abHdrData[RT_FLEXIBLE_ARRAY];
} VUSBURBHDR;
/** Pointer to a URB header. */
typedef VUSBURBHDR *PVUSBURBHDR;

AssertCompileSizeAlignment(VUSBURBHDR, 8);



DECLHIDDEN(int) vusbUrbPoolInit(PVUSBURBPOOL pUrbPool)
{
    int rc = RTCritSectInit(&pUrbPool->CritSectPool);
    if (RT_SUCCESS(rc))
    {
        pUrbPool->cUrbsInPool = 0;
        for (unsigned i = 0; i < RT_ELEMENTS(pUrbPool->aLstFreeUrbs); i++)
            RTListInit(&pUrbPool->aLstFreeUrbs[i]);
    }

    return rc;
}


DECLHIDDEN(void) vusbUrbPoolDestroy(PVUSBURBPOOL pUrbPool)
{
    RTCritSectEnter(&pUrbPool->CritSectPool);
    for (unsigned i = 0; i < RT_ELEMENTS(pUrbPool->aLstFreeUrbs); i++)
    {
        PVUSBURBHDR pHdr, pHdrNext;
        RTListForEachSafe(&pUrbPool->aLstFreeUrbs[i], pHdr, pHdrNext, VUSBURBHDR, NdFree)
        {
            RTListNodeRemove(&pHdr->NdFree);

            pHdr->cbAllocated  = 0;
            pHdr->enmHdrState = VUSBHDRSTATE_INVALID;
            RTMemFree(pHdr);
        }
    }
    RTCritSectLeave(&pUrbPool->CritSectPool);
    RTCritSectDelete(&pUrbPool->CritSectPool);
}


DECLHIDDEN(PVUSBURB) vusbUrbPoolAlloc(PVUSBURBPOOL pUrbPool, VUSBXFERTYPE enmType,
                                      VUSBDIRECTION enmDir, size_t cbData, size_t cbHci,
                                      size_t cbHciTd, unsigned cTds)
{
    Assert((uint32_t)cbData == cbData);
    Assert((uint32_t)cbHci == cbHci);

    /*
     * Reuse or allocate a new URB.
     */
    /** @todo The allocations should be done by the device, at least as an option, since the devices
     * frequently wish to associate their own stuff with the in-flight URB or need special buffering
     * (isochronous on Darwin for instance). */
    /* Get the required amount of additional memory to allocate the whole state. */
    size_t cbCtl = sizeof(VUSBURB) + sizeof(VUSBURBVUSBINT) + cbHci + cTds * cbHciTd;

    AssertReturn((size_t)enmType < RT_ELEMENTS(pUrbPool->aLstFreeUrbs), NULL);


    /* The URB consists of two separate memory allocations, the user-visible
     * buffer and internal control data.
     *
     * The control data is stored in a single contiguous block of memory,
     * but consists of three separate parts:
     *
     * - The URB proper (VUSBURB)
     * - Internal VUSB data (VUSBURBVUSBINT)
     * - HC-specific data opaque to VUSB
     *
     * The HC-specific data has size cbHci + cTds * cbHciTd.
     */

    /* Start by allocating the control section. */
    PVUSBURB pUrb = (PVUSBURB)RTMemAllocZ(cbCtl);
    if (RT_UNLIKELY(!pUrb))
        AssertLogRelFailedReturn(NULL);


    /* Now look for an appropriately sized buffer in the pool. */
    RTCritSectEnter(&pUrbPool->CritSectPool);
    PVUSBURBHDR pHdr = NULL;
    PVUSBURBHDR pIt, pItNext;
    RTListForEachSafe(&pUrbPool->aLstFreeUrbs[enmType], pIt, pItNext, VUSBURBHDR, NdFree)
    {
        if (pIt->cbAllocated >= cbData)
        {
            RTListNodeRemove(&pIt->NdFree);
            Assert(pIt->enmHdrState == VUSBHDRSTATE_FREE);
            /*
             * If the allocation is far too big we increase the age counter too
             * so we don't waste memory for a lot of small transfers
             */
            if (pIt->cbAllocated >= 2 * cbData)
                pIt->cAge++;
            else
                pIt->cAge = 0;
            pIt->enmHdrState = VUSBHDRSTATE_USED;
            pHdr = pIt;
            break;
        }
        else
        {
            /* Increase age and free if it reached a threshold. */
            pIt->cAge++;
            if (pIt->cAge == VUSBURB_AGE_MAX)
            {
                RTListNodeRemove(&pIt->NdFree);
                ASMAtomicDecU32(&pUrbPool->cUrbsInPool);
                pIt->cbAllocated  = 0;
                pIt->enmHdrState = VUSBHDRSTATE_INVALID;
                RTMemFree(pIt);
            }
        }
    }

    if (!pHdr)
    {
        /* Nothing in the pool, allocate a new buffer. */
        size_t cbDataAllocated = cbData <= _4K  ? RT_ALIGN_32(cbData, _1K)
                               : cbData <= _32K ? RT_ALIGN_32(cbData, _4K)
                                                : RT_ALIGN_32(cbData, 16*_1K);

        pHdr = (PVUSBURBHDR)RTMemAllocZ(RT_UOFFSETOF_DYN(VUSBURBHDR, abHdrData[cbDataAllocated]));
        if (RT_UNLIKELY(!pHdr))
        {
            RTCritSectLeave(&pUrbPool->CritSectPool);
            AssertLogRelFailedReturn(NULL);
        }

        pHdr->cbAllocated = cbDataAllocated;
        pHdr->cAge        = 0;
        ASMAtomicIncU32(&pUrbPool->cUrbsInPool);
    }
    else
    {
        /* Paranoia: Clear memory that's part of the guest data buffer now
         * but wasn't before. See @bugref{10410}.
         */
        if (cbData > pHdr->cbData)
        {
            memset(&pHdr->abHdrData[pHdr->cbData], 0, cbData - pHdr->cbData);
        }
    }
    RTCritSectLeave(&pUrbPool->CritSectPool);

    Assert(pHdr->cbAllocated >= cbData);

    /*
     * (Re)init the URB
     */
    pUrb->u32Magic               = VUSBURB_MAGIC;
    pUrb->enmState               = VUSBURBSTATE_ALLOCATED;
    pUrb->fCompleting            = false;
    pUrb->pszDesc                = NULL;
    pUrb->pVUsb                  = (PVUSBURBVUSB)(pUrb + 1);
    pUrb->pVUsb->pUrb            = pUrb;
    pUrb->pVUsb->pvFreeCtx       = NULL;
    pUrb->pVUsb->pfnFree         = NULL;
    pUrb->pVUsb->pCtrlUrb        = NULL;
    pUrb->pVUsb->u64SubmitTS     = 0;
    pUrb->Dev.pvPrivate          = NULL;
    pUrb->Dev.pNext              = NULL;
    pUrb->EndPt                  = UINT8_MAX;
    pUrb->enmType                = enmType;
    pUrb->enmDir                 = enmDir;
    pUrb->fShortNotOk            = false;
    pUrb->enmStatus              = VUSBSTATUS_INVALID;
    pUrb->cbData                 = (uint32_t)cbData;
    pUrb->pbData                 = pHdr->abHdrData;
    /* Any of cbHci, cbHciTd, and cTds can be zero. We have to be careful. */
    pUrb->pHci                   = cbHci ? (PVUSBURBHCI)(pUrb->pVUsb + 1) : NULL;
    pUrb->paTds                  = (cbHciTd && cTds) ? (PVUSBURBHCITD)((uint8_t *)(pUrb->pVUsb + 1) + cbHci) : NULL;

    if (pUrb->paTds)
        Assert(((uint8_t *)pUrb + cbCtl) == ((uint8_t *)pUrb->paTds + cTds * cbHciTd));
    else if (pUrb->pHci)
        Assert(((uint8_t *)pUrb + cbCtl) == ((uint8_t *)pUrb->pHci + cbHci));

    return pUrb;
}


DECLHIDDEN(void) vusbUrbPoolFree(PVUSBURBPOOL pUrbPool, PVUSBURB pUrb)
{
    PVUSBURBHDR pHdr = VUSBURBPOOL_URB_2_URBHDR(pUrb);

    /* Buffers which aged too much because they are too big are freed. */
    if (pHdr->cAge == VUSBURB_AGE_MAX)
    {
        ASMAtomicDecU32(&pUrbPool->cUrbsInPool);
        pHdr->cbAllocated  = 0;
        pHdr->enmHdrState = VUSBHDRSTATE_INVALID;
        RTMemFree(pHdr);
    }
    else
    {
        /* Put it into the list of free buffers. */
        VUSBXFERTYPE enmType = pUrb->enmType;
        AssertReturnVoid((size_t)enmType < RT_ELEMENTS(pUrbPool->aLstFreeUrbs));
        RTCritSectEnter(&pUrbPool->CritSectPool);
        pHdr->cbData = pUrb->cbData;
        pHdr->enmHdrState = VUSBHDRSTATE_FREE;
        RTListAppend(&pUrbPool->aLstFreeUrbs[enmType], &pHdr->NdFree);
        RTCritSectLeave(&pUrbPool->CritSectPool);
    }

    /* Free the control section of the URB. */
    pUrb->cbData = 0;
    pUrb->pbData = NULL;
    pUrb->enmState = VUSBURBSTATE_INVALID;
    RTMemFree(pUrb);
}

