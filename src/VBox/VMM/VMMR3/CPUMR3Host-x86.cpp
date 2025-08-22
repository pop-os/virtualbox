/* $Id: CPUMR3Host-x86.cpp $ */
/** @file
 * CPUM - X86 Host Specific code.
 */

/*
 * Copyright (C) 2013-2025 Oracle and/or its affiliates.
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
#define LOG_GROUP LOG_GROUP_CPUM
#include <VBox/vmm/cpum.h>
#include "CPUMInternal.h"
#include <VBox/vmm/vmcc.h>

#include <iprt/asm-amd64-x86.h>
#include <iprt/string.h>



/**
 * Determins the host CPU MXCSR mask.
 *
 * @returns MXCSR mask.
 */
VMMR3DECL(uint32_t) CPUMR3DeterminHostMxCsrMask(void)
{
    if (   ASMHasCpuId()
        && RTX86IsValidStdRange(ASMCpuId_EAX(0))
        && ASMCpuId_EDX(1) & X86_CPUID_FEATURE_EDX_FXSR)
    {
        uint8_t volatile abBuf[sizeof(X86FXSTATE) + 64];
        PX86FXSTATE      pState = (PX86FXSTATE)&abBuf[64 - ((uintptr_t)&abBuf[0] & 63)];
        RT_ZERO(*pState);
        ASMFxSave(pState);
        if (pState->MXCSR_MASK == 0)
            return 0xffbf;
        return pState->MXCSR_MASK;
    }
    return 0;
}

