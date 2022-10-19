/* $Id: except-vcc.h $ */
/** @file
 * IPRT - Visual C++ Compiler - Exception Management.
 */

/*
 * Copyright (C) 2022 Oracle and/or its affiliates.
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
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL), a copy of it is provided in the "COPYING.CDDL" file included
 * in the VirtualBox distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 *
 * SPDX-License-Identifier: GPL-3.0-only OR CDDL-1.0
 */

#ifndef IPRT_INCLUDED_SRC_common_compiler_vcc_except_vcc_h
#define IPRT_INCLUDED_SRC_common_compiler_vcc_except_vcc_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#define __C_specific_handler their___C_specific_handler
#include <iprt/win/windows.h>
#undef __C_specific_handler

#include <iprt/types.h>
#include <iprt/assertcompile.h>


RT_C_DECLS_BEGIN

#if 0
/** This is part of the AMD64 and ARM (?) exception interface, but appear to
 * live in the runtime headers for some weird reason. */
typedef enum
{
    ExceptionContinueExecution = 0,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind
} EXCEPTION_DISPOSITION;
#endif

/* The following two are borrowed from pecoff.h, as we typically want to include
   winnt.h with this header and the two header cannot co-exist. */

/**
 * An unwind code for AMD64 and ARM64.
 *
 * @note Also known as UNWIND_CODE or _UNWIND_CODE.
 */
typedef union IMAGE_UNWIND_CODE
{
    struct
    {
        /** The prolog offset where the change takes effect.
         * This means the instruction following the one being described.  */
        uint8_t CodeOffset;
        /** Unwind opcode.
         * For AMD64 see IMAGE_AMD64_UNWIND_OP_CODES. */
        RT_GCC_EXTENSION uint8_t UnwindOp : 4;
        /** Opcode specific. */
        RT_GCC_EXTENSION uint8_t OpInfo   : 4;
    } u;
    uint16_t    FrameOffset;
} IMAGE_UNWIND_CODE;
AssertCompileSize(IMAGE_UNWIND_CODE, 2);

/**
 * Unwind information for AMD64 and ARM64.
 *
 * Pointed to by IMAGE_RUNTIME_FUNCTION_ENTRY::UnwindInfoAddress,
 *
 * @note Also known as UNWIND_INFO or _UNWIND_INFO.
 */
typedef struct IMAGE_UNWIND_INFO
{
    /** Version, currently 1 or 2.  The latter if IMAGE_AMD64_UWOP_EPILOG is used. */
    RT_GCC_EXTENSION uint8_t    Version : 3;
    /** IMAGE_UNW_FLAG_XXX */
    RT_GCC_EXTENSION uint8_t    Flags : 5;
    /** Size of function prolog. */
    uint8_t                     SizeOfProlog;
    /** Number of opcodes in aOpcodes. */
    uint8_t                     CountOfCodes;
    /** Initial frame register. */
    RT_GCC_EXTENSION uint8_t    FrameRegister : 4;
    /** Scaled frame register offset. */
    RT_GCC_EXTENSION uint8_t    FrameOffset : 4;
    /** Unwind opcodes. */
    RT_FLEXIBLE_ARRAY_EXTENSION
    IMAGE_UNWIND_CODE           aOpcodes[RT_FLEXIBLE_ARRAY];
} IMAGE_UNWIND_INFO;
AssertCompileMemberOffset(IMAGE_UNWIND_INFO, aOpcodes, 4);
typedef IMAGE_UNWIND_INFO *PIMAGE_UNWIND_INFO;
typedef IMAGE_UNWIND_INFO const *PCIMAGE_UNWIND_INFO;



#ifdef RT_ARCH_AMD64

/**
 * The Visual C++ 2019 layout of the GS_HANDLER_DATA data type for AMD64.
 *
 * This is pointed to by DISPATCHER_CONTEXT::HandlerData when dispatching
 * exceptions.  The data resides after the unwind info for the function.
 */
typedef struct _GS_HANDLER_DATA
{
    union
    {
        struct
        {
            uint32_t    fEHandler : 1;
#define GS_HANDLER_OFF_COOKIE_IS_EHANDLER   RT_BIT(0) /**< Handles exceptions. */
            uint32_t    fUHandler : 1;
#define GS_HANDLER_OFF_COOKIE_IS_UHANDLER   RT_BIT(1) /**< Handles unwind. */
            uint32_t    fHasAlignment : 1;
#define GS_HANDLER_OFF_COOKIE_HAS_ALIGNMENT RT_BIT(2) /**< Has the uAlignmentMask member. */
        } Bits;
#define GS_HANDLER_OFF_COOKIE_MASK          UINT32_C(0xfffffff8) /**< Mask to apply to offCookie to the the value. */
        uint32_t        offCookie;
    } u;
    int32_t             offAlignedBase;
    /** This field is only there when GS_HANDLER_OFF_COOKIE_HAS_ALIGNMENT is set.
     * it seems. */
    uint32_t            uAlignmentMask;
} GS_HANDLER_DATA;
typedef GS_HANDLER_DATA *PGS_HANDLER_DATA;
typedef GS_HANDLER_DATA const *PCGS_HANDLER_DATA;
#endif

#ifdef RT_ARCH_X86
void __fastcall __security_check_cookie(uintptr_t uCookieToCheck);
#else
void __cdecl    __security_check_cookie(uintptr_t uCookieToCheck);
#endif

#if defined(RT_ARCH_AMD64)
EXCEPTION_DISPOSITION __C_specific_handler(PEXCEPTION_RECORD pXcptRec, PEXCEPTION_REGISTRATION_RECORD pXcptRegRec,
                                           PCONTEXT pCpuCtx, PDISPATCHER_CONTEXT pDispCtx);
#endif

RT_C_DECLS_END

#endif /* !IPRT_INCLUDED_SRC_common_compiler_vcc_except_vcc_h */

