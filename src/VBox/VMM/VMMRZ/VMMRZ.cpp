/* $Id: VMMRZ.cpp 20875 2009-06-24 02:29:17Z vboxsync $ */
/** @file
 * VMM - Virtual Machine Monitor, Raw-mode and ring-0 context code.
 */

/*
 * Copyright (C) 2009 Sun Microsystems, Inc.
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <VBox/vmm.h>
#include "VMMInternal.h"
#include <VBox/vm.h>
#include <VBox/err.h>

#include <iprt/assert.h>
#include <iprt/string.h>


/**
 * Calls the ring-3 host code.
 *
 * @returns VBox status code of the ring-3 call.
 * @retval  VERR_VMM_RING3_CALL_DISABLED if called at the wrong time. This must
 *          be passed up the stack, or if that isn't possible then VMMRZCallRing3
 *          needs to change it into an assertion.
 *
 *
 * @param   pVM             The VM handle.
 * @param   pVCpu           The virtual CPU handle of the calling EMT.
 * @param   enmOperation    The operation.
 * @param   uArg            The argument to the operation.
 */
VMMRZDECL(int) VMMRZCallRing3(PVM pVM, PVMCPU pVCpu, VMMCALLRING3 enmOperation, uint64_t uArg)
{
    VMCPU_ASSERT_EMT(pVCpu);

    /*
     * Check if calling ring-3 has been disabled and only let let fatal calls thru.
     */
    if (RT_UNLIKELY(    pVCpu->vmm.s.cCallRing3Disabled != 0
                    &&  enmOperation != VMMCALLRING3_VM_R0_ASSERTION))
    {
        /*
         * In most cases, it's sufficient to return a status code which
         * will then be propagated up the code usually encountering several
         * AssertRC invocations along the way. Hitting one of those is more
         * helpful than stopping here.
         *
         * However, some doesn't check the status code because they are called
         * from void functions, and for these we'll turn this into a ring-0
         * assertion host call.
         */
        if (enmOperation != VMMCALLRING3_REM_REPLAY_HANDLER_NOTIFICATIONS)
            return VERR_VMM_RING3_CALL_DISABLED;
#ifdef IN_RC
        RTStrPrintf(g_szRTAssertMsg1, sizeof(pVM->vmm.s.szRing0AssertMsg1),
                    "VMMRZCallRing3: enmOperation=%d uArg=%#llx idCpu=%#x\n", enmOperation, uArg, pVCpu->idCpu);
#endif
        RTStrPrintf(pVM->vmm.s.szRing0AssertMsg1, sizeof(pVM->vmm.s.szRing0AssertMsg1),
                    "VMMRZCallRing3: enmOperation=%d uArg=%#llx idCpu=%#x\n", enmOperation, uArg, pVCpu->idCpu);
        enmOperation = VMMCALLRING3_VM_R0_ASSERTION;
    }

    /*
     * The normal path.
     */
/** @todo profile this! */
    pVCpu->vmm.s.enmCallRing3Operation = enmOperation;
    pVCpu->vmm.s.u64CallRing3Arg = uArg;
    pVCpu->vmm.s.rcCallRing3 = VERR_INTERNAL_ERROR;
#ifdef IN_RC
    pVM->vmm.s.pfnGuestToHostRC(VINF_VMM_CALL_HOST);
#else
    int rc = vmmR0CallRing3LongJmp(&pVCpu->vmm.s.CallRing3JmpBufR0, VINF_VMM_CALL_HOST);
    if (RT_FAILURE(rc))
        return rc;
#endif
    return pVCpu->vmm.s.rcCallRing3;
}


/**
 * Simple wrapper that adds the pVCpu argument.
 *
 * @returns VBox status code of the ring-3 call.
 * @retval  VERR_VMM_RING3_CALL_DISABLED if called at the wrong time. This must
 *          be passed up the stack, or if that isn't possible then VMMRZCallRing3
 *          needs to change it into an assertion.
 *
 * @param   pVM             The VM handle.
 * @param   pVCpu           The virtual CPU handle of the calling EMT.
 * @param   enmOperation    The operation.
 * @param   uArg            The argument to the operation.
 */
VMMRZDECL(int) VMMRZCallRing3NoCpu(PVM pVM, VMMCALLRING3 enmOperation, uint64_t uArg)
{
    return VMMRZCallRing3(pVM, VMMGetCpu(pVM), enmOperation, uArg);
}


/**
 * Disables all host calls, except certain fatal ones.
 *
 * @param   pVCpu               The CPU struct for the calling EMT.
 * @thread  EMT.
 */
VMMRZDECL(void) VMMRZCallRing3Disable(PVMCPU pVCpu)
{
    VMCPU_ASSERT_EMT(pVCpu);
    Assert(pVCpu->vmm.s.cCallRing3Disabled < 16);
    if (++pVCpu->vmm.s.cCallRing3Disabled == 1)
    {
        /** @todo it might make more sense to just disable logging here, then we
         * won't flush away important bits... but that goes both ways really. */
#ifdef IN_RC
        pVCpu->pVMRC->vmm.s.fRCLoggerFlushingDisabled = true;
#else
# ifdef LOG_ENABLED
        if (pVCpu->vmm.s.pR0LoggerR0)
            pVCpu->vmm.s.pR0LoggerR0->fFlushingDisabled = true;
# endif
#endif
    }
}


/**
 * Counters VMMRZCallRing3Disable and re-enables host calls.
 *
 * @param   pVCpu               The CPU struct for the calling EMT.
 * @thread  EMT.
 */
VMMRZDECL(void) VMMRZCallRing3Enable(PVMCPU pVCpu)
{
    VMCPU_ASSERT_EMT(pVCpu);
    Assert(pVCpu->vmm.s.cCallRing3Disabled > 0);
    if (--pVCpu->vmm.s.cCallRing3Disabled == 0)
    {
#ifdef IN_RC
        pVCpu->pVMRC->vmm.s.fRCLoggerFlushingDisabled = false;
#else
# ifdef LOG_ENABLED
        if (pVCpu->vmm.s.pR0LoggerR0)
            pVCpu->vmm.s.pR0LoggerR0->fFlushingDisabled = false;
# endif
#endif
    }
}


/**
 * Checks whether its possible to call host context or not.
 *
 * @returns true if it's safe, false if it isn't.
 * @param   pVCpu               The CPU struct for the calling EMT.
 */
VMMRZDECL(bool) VMMRZCallRing3IsEnabled(PVMCPU pVCpu)
{
    VMCPU_ASSERT_EMT(pVCpu);
    Assert(pVCpu->vmm.s.cCallRing3Disabled <= 16);
    return pVCpu->vmm.s.cCallRing3Disabled == 0;
}
