/* $Id: IEMInlineDecode-armv8.h $ */
/** @file
 * IEM - Interpreted Execution Manager - Inlined Decoding related Functions, x86 target.
 */

/*
 * Copyright (C) 2011-2025 Oracle and/or its affiliates.
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

#ifndef VMM_INCLUDED_SRC_VMMAll_target_armv8_IEMInlineDecode_armv8_h
#define VMM_INCLUDED_SRC_VMMAll_target_armv8_IEMInlineDecode_armv8_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#include <VBox/err.h>




/**
 * Fetches a 32-bit opcode word, longjmp on error.
 *
 * @returns The opcode word.
 * @param   pVCpu               The cross context virtual CPU structure of the calling thread.
 */
DECL_INLINE_THROW(uint32_t) iemOpcodeGetU32Jmp(PVMCPUCC pVCpu) IEM_NOEXCEPT_MAY_LONGJMP
{
#if 0 /** @todo */
    /*
     * Check for hardware instruction breakpoints.
     * Note! Guest breakpoints are only checked after POP SS or MOV SS on AMD CPUs.
     */
    if (RT_LIKELY(!(pVCpu->iem.s.fExec & IEM_F_PENDING_BRK_INSTR)))
    { /* likely */ }
    else
    {
        VBOXSTRICTRC rcStrict = DBGFBpCheckInstruction(pVCpu->CTX_SUFF(pVM), pVCpu,
                                                       pVCpu->cpum.GstCtx.rip + pVCpu->cpum.GstCtx.cs.u64Base,
                                                          !(pVCpu->cpum.GstCtx.rflags.uBoth & CPUMCTX_INHIBIT_SHADOW_SS)
                                                       || IEM_IS_GUEST_CPU_AMD(pVCpu));
        if (RT_LIKELY(rcStrict == VINF_SUCCESS))
        { /* likely */ }
        else
        {
            if (rcStrict == VINF_EM_RAW_GUEST_TRAP)
                rcStrict = iemRaiseDebugException(pVCpu);
            IEM_DO_LONGJMP(pVCpu, VBOXSTRICTRC_VAL(rcStrict));
        }
    }
#endif

    /*
     * Fetch the first opcode byte.
     */
# ifdef IEM_WITH_CODE_TLB
    uint8_t const *pbBuf = pVCpu->iem.s.pbInstrBuf;
    if (RT_LIKELY(pbBuf != NULL))
    {
        uintptr_t const offBuf  = pVCpu->iem.s.offInstrNextByte;
        uintptr_t const offNext = offBuf + sizeof(uint32_t);
        if (RT_LIKELY(offNext <= pVCpu->iem.s.cbInstrBufTotal))
        {
            uint32_t const u32Ret = *(uint32_t const *)&pbBuf[offBuf];
            pVCpu->iem.s.offInstrNextByte = offNext;
#  ifdef IEM_WITH_CODE_TLB_AND_OPCODE_BUF
            pVCpu->iem.s.au32Opcode[0] = u32Ret; /** @todo we have no opcode buffer offset on arm... */
#  endif
            return u32Ret;
        }
    }

# else /* !IEM_WITH_CODE_TLB */
    uintptr_t offOpcode = pVCpu->iem.s.offOpcode; /** @todo no offOpcode on arm, so this doesn't compile... */
    Assert(offOpcode & 3);
    if (RT_LIKELY(offOpcode + sizeof(uint32_t) <= (uintptr_t)pVCpu->iem.s.cbOpcode))
    {
        pVCpu->iem.s.offOpcode = (uint8_t)offOpcode + sizeof(uint32_t);
        return pVCpu->iem.s.au32Opcode[offOpcode / sizeof(uint32_t)];
    }
# endif

    return iemOpcodeGetU32SlowJmp(pVCpu);
}


#endif /* !VMM_INCLUDED_SRC_VMMAll_target_armv8_IEMInlineDecode_armv8_h */
