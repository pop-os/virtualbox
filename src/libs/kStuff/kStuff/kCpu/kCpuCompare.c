/* $Id: kCpuCompare.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kCpu - kCpuCompare.
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
#include <k/kCpu.h>
#include <k/kErrors.h>


/**
 * Compares arch+cpu some code was generated for with a arch+cpu for executing it
 * to see if it'll work out fine or not.
 *
 * @returns 0 if the code is compatible with the cpu.
 * @returns KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE if the arch+cpu isn't compatible with the code.
 *
 * @param   enmCodeArch The architecture the code was generated for.
 * @param   enmCodeCpu  The cpu the code was generated for.
 * @param   enmArch     The architecture to run it on.
 * @param   enmCpu      The cpu to run it on.
 */
KCPU_DECL(int) kCpuCompare(KCPUARCH enmCodeArch, KCPU enmCodeCpu, KCPUARCH enmArch, KCPU enmCpu)
{
    /*
     * Compare arch and cpu.
     */
    if (enmCodeArch != enmArch)
        return KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;

    /* exact match is nice. */
    if (enmCodeCpu == enmCpu)
        return 0;

    switch (enmArch)
    {
        case K_ARCH_X86_16:
            if (enmCpu < KCPU_FIRST_X86_16 || enmCpu > KCPU_LAST_X86_16)
                return KERR_INVALID_PARAMETER;

            /* intel? */
            if (enmCodeCpu <= KCPU_CORE2_16)
            {
                /* also intel? */
                if (enmCpu <= KCPU_CORE2_16)
                    return enmCodeCpu <= enmCpu ? 0 : KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;
                switch (enmCpu)
                {
                    case KCPU_K6_16:
                        return enmCodeCpu <= KCPU_I586 ? 0 : KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;
                    case KCPU_K7_16:
                    case KCPU_K8_16:
                    default:
                        return enmCodeCpu <= KCPU_I686 ? 0 : KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;
                }
            }
            /* amd */
            return enmCpu >= KCPU_K6_16 && enmCpu <= KCPU_K8_16
                    ? 0 : KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;

        case K_ARCH_X86_32:
            if (enmCpu < KCPU_FIRST_X86_32 || enmCpu > KCPU_LAST_X86_32)
                return KERR_INVALID_PARAMETER;

            /* blend? */
            if (enmCodeCpu == KCPU_X86_32_BLEND)
                return 0;

            /* intel? */
            if (enmCodeCpu <= KCPU_CORE2_32)
            {
                /* also intel? */
                if (enmCpu <= KCPU_CORE2_32)
                    return enmCodeCpu <= enmCpu ? 0 : KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;
                switch (enmCpu)
                {
                    case KCPU_K6:
                        return enmCodeCpu <= KCPU_I586 ? 0 : KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;
                    case KCPU_K7:
                    case KCPU_K8_32:
                    default:
                        return enmCodeCpu <= KCPU_I686 ? 0 : KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;
                }
            }
            /* amd */
            return enmCpu >= KCPU_K6 && enmCpu <= KCPU_K8_32
                    ? 0 : KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;

        case K_ARCH_AMD64:
            if (enmCpu < KCPU_FIRST_AMD64 || enmCpu > KCPU_LAST_AMD64)
                return KERR_INVALID_PARAMETER;

            /* blend? */
            if (enmCodeCpu == KCPU_AMD64_BLEND)
                return 0;
            /* this is simple for now. */
            return KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;

        default:
            break;
    }
    return KCPU_ERR_ARCH_CPU_NOT_COMPATIBLE;
}

