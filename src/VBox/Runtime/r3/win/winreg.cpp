/* $Id: winreg.cpp $ */
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


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#include "internal/iprt.h"
#include <iprt/win/reg.h>

#include <iprt/win/windows.h>
#include <iprt/assert.h>
#include <iprt/errcore.h>


/** Translates a root specifier to a special HKEY value. */
DECLINLINE(HKEY) rtWinRegRootToKey(RTWINREGROOT enmRoot)
{
    switch (enmRoot)
    {
        case kRTWinRegRoot_ClassesRoot:                 return HKEY_CLASSES_ROOT;
        case kRTWinRegRoot_CurrentUser:                 return HKEY_CURRENT_USER;
        case kRTWinRegRoot_LocalMachine:                return HKEY_LOCAL_MACHINE;
        case kRTWinRegRoot_Users:                       return HKEY_USERS;
        case kRTWinRegRoot_PerforanceData:              return HKEY_PERFORMANCE_DATA;
        case kRTWinRegRoot_PerforanceText:              return HKEY_PERFORMANCE_TEXT;
        case kRTWinRegRoot_PerforanceNlsText:           return HKEY_PERFORMANCE_NLSTEXT;
        case kRTWinRegRoot_CurrentConfig:               return HKEY_CURRENT_CONFIG;
        case kRTWinRegRoot_DynData:                     return HKEY_DYN_DATA;
        case kRTWinRegRoot_CurrentUserLocalSettings:    return HKEY_CURRENT_USER_LOCAL_SETTINGS;

        case kRTWinRegRoot_Invalid:
        case kRTWinRegRoot_End:
        case kRTWinRegRoot_32BitHack:
            break;
    }
    AssertFailedReturn(NULL);
}


/**
 * Internal helper for querying a simple value.
 */
static int rtWinRegQueryValue(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, void *pvValue,
                              DWORD const cbDataExpected, DWORD const dwTypeExpected)
{
    AssertReturn(enmRoot > kRTWinRegRoot_Invalid && enmRoot < kRTWinRegRoot_End, VERR_INVALID_PARAMETER);
    HKEY    hKey = NULL;
    LSTATUS lrc  = RegOpenKeyExW(rtWinRegRootToKey(enmRoot), pwszKeyPath, 0 /*ulOptions*/, KEY_QUERY_VALUE, &hKey);
    if (lrc == ERROR_SUCCESS)
    {
        DWORD cbData = cbDataExpected;
        DWORD dwType = REG_NONE;
        lrc = RegQueryValueExW(hKey, pwszValue, NULL/*pReserved*/, &dwType, (BYTE *)pvValue, &cbData);
        RegCloseKey(hKey);
        if (lrc == ERROR_SUCCESS)
        {
            if (dwType == dwTypeExpected)
            {
                if (cbData == cbDataExpected)
                    return VINF_SUCCESS;
                return VERR_MISMATCH;
            }
            return VERR_WRONG_TYPE;
        }
    }
    return RTErrConvertFromWin32(lrc);
}


RTDECL(int) RTWinRegQueryValueU32(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, uint32_t *puValue)
{
    return rtWinRegQueryValue(enmRoot, pwszKeyPath, pwszValue, puValue, sizeof(*puValue), REG_DWORD);
}


RTDECL(int) RTWinRegQueryValueS32(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, int32_t *piValue)
{
    return rtWinRegQueryValue(enmRoot, pwszKeyPath, pwszValue, piValue, sizeof(*piValue), REG_DWORD);
}


RTDECL(int) RTWinRegQueryValueDWord(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, DWORD *puValue)
{
    return rtWinRegQueryValue(enmRoot, pwszKeyPath, pwszValue, puValue, sizeof(*puValue), REG_DWORD);
}


RTDECL(int) RTWinRegQueryValueU64(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, uint64_t *puValue)
{
    return rtWinRegQueryValue(enmRoot, pwszKeyPath, pwszValue, puValue, sizeof(*puValue), REG_QWORD);
}


RTDECL(int) RTWinRegQueryValueS64(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, int64_t *piValue)
{
    return rtWinRegQueryValue(enmRoot, pwszKeyPath, pwszValue, piValue, sizeof(*piValue), REG_QWORD);
}


RTDECL(int) RTWinRegQueryValueQWord(RTWINREGROOT enmRoot, PCRTUTF16 pwszKeyPath, PCRTUTF16 pwszValue, DWORD64 *puValue)
{
    return rtWinRegQueryValue(enmRoot, pwszKeyPath, pwszValue, puValue, sizeof(*puValue), REG_QWORD);
}

