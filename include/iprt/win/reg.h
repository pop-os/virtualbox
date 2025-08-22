/* $Id: reg.h $ */
/** @file
 * IPRT - Windows Registry Accessors.
 */

/*
 * Copyright (C) 2025 Oracle and/or its affiliates.
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

#ifndef IPRT_INCLUDED_win_reg_h
#define IPRT_INCLUDED_win_reg_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#include <iprt/cdefs.h>
#include <iprt/types.h>


RT_C_DECLS_BEGIN

/** @defgroup grp_rt_win_reg    RTWinReg - Windows Registry Accessors
 * @ingroup grp_rt
 * @{
 */

typedef enum RTWINREGROOT
{
    kRTWinRegRoot_Invalid = 0,
    kRTWinRegRoot_ClassesRoot,
    kRTWinRegRoot_CurrentUser,
    kRTWinRegRoot_LocalMachine,
    kRTWinRegRoot_Users,
    kRTWinRegRoot_PerforanceData,
    kRTWinRegRoot_PerforanceText,
    kRTWinRegRoot_PerforanceNlsText,
    kRTWinRegRoot_CurrentConfig,
    kRTWinRegRoot_DynData,
    kRTWinRegRoot_CurrentUserLocalSettings,
    kRTWinRegRoot_End,
    kRTWinRegRoot_32BitHack = 0x7fffffff
} RTWINREGROOT;

/**
 * Reads an unsigned 32-bit value from the windows registry.
 *
 * @returns IPRT status code.
 * @param   enmRoot             The root for the key path.
 * @param   pwszKeyPath         The path to the key.
 * @param   pwszValue           The value to read. NULL for default.
 * @param   puValue             Where to return the value.
 */
RTDECL(int) RTWinRegQueryValueU32(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, uint32_t *puValue);

/**
 * Reads a signed 32-bit value from the windows registry.
 *
 * @returns IPRT status code.
 * @param   enmRoot             The root for the key path.
 * @param   pwszKeyPath         The path to the key.
 * @param   pwszValue           The value to read. NULL for default.
 * @param   piValue             Where to return the value.
 */
RTDECL(int) RTWinRegQueryValueS32(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, int32_t *piValue);

/**
 * Reads an unsigned 64-bit value from the windows registry.
 *
 * @returns IPRT status code.
 * @param   enmRoot             The root for the key path.
 * @param   pwszKeyPath         The path to the key.
 * @param   pwszValue           The value to read. NULL for default.
 * @param   puValue             Where to return the value.
 */
RTDECL(int) RTWinRegQueryValueU64(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, uint64_t *puValue);

/**
 * Reads a signed 64-bit value from the windows registry.
 *
 * @returns IPRT status code.
 * @param   enmRoot             The root for the key path.
 * @param   pwszKeyPath         The path to the key.
 * @param   pwszValue           The value to read. NULL for default.
 * @param   piValue             Where to return the value.
 */
RTDECL(int) RTWinRegQueryValueS64(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, int64_t *piValue);

#if defined(DOXYGEN_RUNNING) || defined(REG_DWORD)

/**
 * Reads a DWORD (32-bit) value from the windows registry.
 *
 * @returns IPRT status code.
 * @param   enmRoot             The root for the key path.
 * @param   pwszKeyPath         The path to the key.
 * @param   pwszValue           The value to read. NULL for default.
 * @param   puValue             Where to return the value.
 */
RTDECL(int) RTWinRegQueryValueDWord(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, DWORD *puValue);

/**
 * Reads a DWORD64/QWORD (64-bit) value from the windows registry.
 *
 * @returns IPRT status code.
 * @param   enmRoot             The root for the key path.
 * @param   pwszKeyPath         The path to the key.
 * @param   pwszValue           The value to read. NULL for default.
 * @param   puValue             Where to return the value.
 */
RTDECL(int) RTWinRegQueryValueQWord(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, DWORD64 *puValue);

#endif


/** @} */

RT_C_DECLS_END

#endif /* !IPRT_INCLUDED_win_reg_h */

