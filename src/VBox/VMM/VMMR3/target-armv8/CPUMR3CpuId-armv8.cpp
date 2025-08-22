/* $Id: CPUMR3CpuId-armv8.cpp $ */
/** @file
 * CPUM - CPU ID part for ARMv8 hypervisor.
 */

/*
 * Copyright (C) 2023-2025 Oracle and/or its affiliates.
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
#define CPUM_WITH_NONCONST_HOST_FEATURES /* required for initializing parts of the g_CpumHostFeatures structure here. */
#include <VBox/vmm/cpum.h>
#include <VBox/vmm/dbgf.h>
#include <VBox/vmm/nem.h>
#include <VBox/vmm/ssm.h>
#include "CPUMInternal-armv8.h"
#include <VBox/vmm/vmcc.h>
#include <VBox/gic.h>

#include <VBox/err.h>
#include <iprt/ctype.h>
#include <iprt/mem.h>
#include <iprt/sort.h>
#include <iprt/string.h>




/**
 * Looks up @a idReg in @a paIdRegs.
 *
 * @returns Pointer to matching @a paIdRegs entry if found, NULL if not.
 * @param   paIdRegs    Sorted array of ID register values.
 * @param   cIdRegs     Number of ID register values.
 * @param   idReg       The ID register to lookup.
 */
static PSUPARMSYSREGVAL cpumCpuIdLookupIdReg(PSUPARMSYSREGVAL paIdRegs, uint32_t cIdRegs, uint32_t idReg)
{
    if (cIdRegs)
    {
        /*
         * Binary lookup. The input should be sorted.
         */
        uint32_t iLow = 0;
        uint32_t iEnd = cIdRegs;
        for (;;)
        {
            uint32_t i = iLow + (iEnd - iLow) / 2;
            if (paIdRegs[i].idReg > idReg)
            {
                if (i > iLow)
                    iEnd = i;
                else
                    break;
            }
            else if (paIdRegs[i].idReg < idReg)
            {
                i += 1;
                if (i < iEnd)
                    iLow = i;
                else
                    break;
            }
            else
                return &paIdRegs[i];
        }

#ifdef VBOX_STRICT
        /* Do linear search to make sure it's sorted and the above code works. */
        for (uint32_t i = 0, idPrev = 0 /*ASSUMES no zero ID reg*/; i < cIdRegs; i++)
        {
            uint32_t const idRegThis = paIdRegs[i].idReg;
            Assert(idRegThis != idReg);
            Assert(idRegThis > idPrev);
            idPrev = idRegThis;
        }
#endif
    }

    return NULL;
}


/**
 * Looks up @a idReg in the guest ID registers.
 *
 * @returns Pointer to matching ID register entry if found, NULL if not.
 * @param   idReg       The ID register to lookup.
 */
static PSUPARMSYSREGVAL cpumR3CpuIdLookupGuestIdReg(PVM pVM, uint32_t idReg)
{
    return cpumCpuIdLookupIdReg(pVM->cpum.s.GuestInfo.paIdRegsR3, pVM->cpum.s.GuestInfo.cIdRegs, idReg);
}


/*
 *
 * Init related code.
 * Init related code.
 * Init related code.
 *
 *
 */

/** @name Instruction Set Extension Options
 * @{  */
/** Configuration option type (extended boolean, really). */
typedef uint8_t CPUMISAEXTCFG;
/** Always disable the extension. */
#define CPUMISAEXTCFG_DISABLED              false
/** Enable the extension if it's supported by the host CPU. */
#define CPUMISAEXTCFG_ENABLED_SUPPORTED     true
/** Enable the extension if it's supported by the host CPU, but don't let
 * the portable CPUID feature disable it. */
#define CPUMISAEXTCFG_ENABLED_PORTABLE      UINT8_C(127)
/** Always enable the extension. */
#define CPUMISAEXTCFG_ENABLED_ALWAYS        UINT8_C(255)
/** @} */

/**
 * CPUID Configuration (from CFGM).
 *
 * @remarks  The members aren't document since we would only be duplicating the
 *           \@cfgm entries in cpumR3CpuIdReadConfig.
 */
typedef struct CPUMCPUIDCONFIG
{
    CPUMISAEXTCFG   enmAes;
    CPUMISAEXTCFG   enmPmull;
    CPUMISAEXTCFG   enmSha1;
    CPUMISAEXTCFG   enmSha256;
    CPUMISAEXTCFG   enmSha512;
    CPUMISAEXTCFG   enmCrc32;
    CPUMISAEXTCFG   enmSha3;

    char            szCpuName[128];
} CPUMCPUIDCONFIG;
/** Pointer to CPUID config (from CFGM). */
typedef CPUMCPUIDCONFIG *PCPUMCPUIDCONFIG;


/**
 * Sanitizes and adjust the CPUID leaves.
 *
 * Drop features that aren't virtualized (or virtualizable).  Adjust information
 * and capabilities to fit the virtualized hardware.  Remove information the
 * guest shouldn't have (because it's wrong in the virtual world or because it
 * gives away host details) or that we don't have documentation for and no idea
 * what means.
 *
 * @returns VBox status code.
 * @param   pVM         The cross context VM structure (for cCpus).
 * @param   pCpum       The CPUM instance data.
 * @param   pConfig     The CPUID configuration we've read from CFGM.
 * @param   pCpumCfg    The CPUM CFGM config.
 */
static int cpumR3CpuIdSanitize(PVM pVM, PCPUM pCpum, PCPUMCPUIDCONFIG pConfig, PCFGMNODE pCpumCfg)
{
#define PORTABLE_CLEAR_BITS_WHEN(Lvl, a_pLeafReg, FeatNm, fMask, uValue) \
        if ( pCpum->u8PortableCpuIdLevel >= (Lvl) && ((a_pLeafReg) & (fMask)) == (uValue) ) \
        { \
            LogRel(("PortableCpuId: " #a_pLeafReg "[" #FeatNm "]: %#x -> 0\n", (a_pLeafReg) & (fMask))); \
            (a_pLeafReg) &= ~(uint32_t)(fMask); \
        }
#define PORTABLE_DISABLE_FEATURE_BIT(Lvl, a_pLeafReg, FeatNm, fBitMask) \
        if ( pCpum->u8PortableCpuIdLevel >= (Lvl) && ((a_pLeafReg) & (fBitMask)) ) \
        { \
            LogRel(("PortableCpuId: " #a_pLeafReg "[" #FeatNm "]: 1 -> 0\n")); \
            (a_pLeafReg) &= ~(uint32_t)(fBitMask); \
        }
#define PORTABLE_DISABLE_FEATURE_BIT_CFG(Lvl, a_IdReg, a_FeatNm, a_IdRegValCheck, enmConfig, a_IdRegValNotSup) \
        if (   pCpum->u8PortableCpuIdLevel >= (Lvl) \
            && (RT_BF_GET(a_IdReg, a_FeatNm) >= a_IdRegValCheck) \
            && (enmConfig) != CPUMISAEXTCFG_ENABLED_PORTABLE ) \
        { \
            LogRel(("PortableCpuId: [" #a_FeatNm "]: 1 -> 0\n")); \
            (a_IdReg) = RT_BF_SET(a_IdReg, a_FeatNm, a_IdRegValNotSup); \
        }
    //Assert(pCpum->GuestFeatures.enmCpuVendor != CPUMCPUVENDOR_INVALID);

    /* The CPUID entries we start with here isn't necessarily the ones of the host, so we
       must consult HostFeatures when processing CPUMISAEXTCFG variables. */
#ifdef RT_ARCH_ARM64
    PCCPUMFEATURES pHstFeat = &pCpum->HostFeatures.s;
# define PASSTHRU_FEATURE(a_IdReg, enmConfig, fHostFeature, a_IdRegNm, a_IdRegValSup, a_IdRegValNotSup) do { \
            (a_IdReg) = (enmConfig) && ((enmConfig) == CPUMISAEXTCFG_ENABLED_ALWAYS || (fHostFeature)) \
                      ? RT_BF_SET(a_IdReg, a_IdRegNm, a_IdRegValSup) \
                      : RT_BF_SET(a_IdReg, a_IdRegNm, a_IdRegValNotSup); \
        } while (0)
#else
# define PASSTHRU_FEATURE(a_IdReg, enmConfig, fHostFeature, a_IdRegNm, a_IdRegValSup, a_IdRegValNotSup) do { \
            (a_IdReg) = (enmConfig) != CPUMISAEXTCFG_DISABLED \
                      ? RT_BF_SET(a_IdReg, a_IdRegNm, a_IdRegValSup) \
                      : RT_BF_SET(a_IdReg, a_IdRegNm, a_IdRegValNotSup); \
        } while (0)
#endif

    /*
     * ID_AA64ISAR0_EL1
     */
    PSUPARMSYSREGVAL pIdReg = cpumR3CpuIdLookupGuestIdReg(pVM, ARMV8_AARCH64_SYSREG_ID_AA64ISAR0_EL1);
    if (pIdReg)
    {
        uint64_t uVal = pIdReg->uValue;
        PASSTHRU_FEATURE(uVal, pConfig->enmAes,     pHstFeat->fAes,     ARMV8_ID_AA64ISAR0_EL1_AES,     ARMV8_ID_AA64ISAR0_EL1_AES_SUPPORTED,                   ARMV8_ID_AA64ISAR0_EL1_AES_NOT_IMPL);
        uint64_t uTmp = RT_BF_GET(uVal, ARMV8_ID_AA64ISAR0_EL1_AES) == ARMV8_ID_AA64ISAR0_EL1_AES_SUPPORTED ? ARMV8_ID_AA64ISAR0_EL1_AES_SUPPORTED : ARMV8_ID_AA64ISAR0_EL1_AES_NOT_IMPL;
        PASSTHRU_FEATURE(uVal, pConfig->enmPmull,   pHstFeat->fPmull,   ARMV8_ID_AA64ISAR0_EL1_AES,     ARMV8_ID_AA64ISAR0_EL1_AES_SUPPORTED_PMULL,             uTmp);
        PASSTHRU_FEATURE(uVal, pConfig->enmSha1,    pHstFeat->fSha1,    ARMV8_ID_AA64ISAR0_EL1_SHA1,    ARMV8_ID_AA64ISAR0_EL1_SHA1_SUPPORTED,                  ARMV8_ID_AA64ISAR0_EL1_SHA1_NOT_IMPL);
        PASSTHRU_FEATURE(uVal, pConfig->enmSha256,  pHstFeat->fSha256,  ARMV8_ID_AA64ISAR0_EL1_SHA2,    ARMV8_ID_AA64ISAR0_EL1_SHA2_SUPPORTED_SHA256,           ARMV8_ID_AA64ISAR0_EL1_SHA2_NOT_IMPL);
        uTmp = RT_BF_GET(uVal, ARMV8_ID_AA64ISAR0_EL1_SHA2) == ARMV8_ID_AA64ISAR0_EL1_SHA2_SUPPORTED_SHA256 ? ARMV8_ID_AA64ISAR0_EL1_SHA2_SUPPORTED_SHA256 : ARMV8_ID_AA64ISAR0_EL1_SHA2_NOT_IMPL;
        PASSTHRU_FEATURE(uVal, pConfig->enmSha512,  pHstFeat->fSha512,  ARMV8_ID_AA64ISAR0_EL1_SHA2,    ARMV8_ID_AA64ISAR0_EL1_SHA2_SUPPORTED_SHA256_SHA512,    uTmp);
        PASSTHRU_FEATURE(uVal, pConfig->enmCrc32,   pHstFeat->fCrc32,   ARMV8_ID_AA64ISAR0_EL1_CRC32,   ARMV8_ID_AA64ISAR0_EL1_CRC32_SUPPORTED,                 ARMV8_ID_AA64ISAR0_EL1_CRC32_NOT_IMPL);
        PASSTHRU_FEATURE(uVal, pConfig->enmSha3,    pHstFeat->fSha3,    ARMV8_ID_AA64ISAR0_EL1_SHA3,    ARMV8_ID_AA64ISAR0_EL1_SHA3_SUPPORTED,                  ARMV8_ID_AA64ISAR0_EL1_SHA3_NOT_IMPL);

        if (pCpum->u8PortableCpuIdLevel > 0)
        {
            PORTABLE_DISABLE_FEATURE_BIT_CFG(1, uVal, ARMV8_ID_AA64ISAR0_EL1_AES,   ARMV8_ID_AA64ISAR0_EL1_AES_SUPPORTED,                   pConfig->enmAes,    ARMV8_ID_AA64ISAR0_EL1_AES_NOT_IMPL);
            PORTABLE_DISABLE_FEATURE_BIT_CFG(1, uVal, ARMV8_ID_AA64ISAR0_EL1_AES,   ARMV8_ID_AA64ISAR0_EL1_AES_SUPPORTED_PMULL,             pConfig->enmPmull,  ARMV8_ID_AA64ISAR0_EL1_AES_NOT_IMPL);
            PORTABLE_DISABLE_FEATURE_BIT_CFG(1, uVal, ARMV8_ID_AA64ISAR0_EL1_SHA1,  ARMV8_ID_AA64ISAR0_EL1_SHA1_SUPPORTED,                  pConfig->enmSha1,   ARMV8_ID_AA64ISAR0_EL1_SHA1_NOT_IMPL);
            PORTABLE_DISABLE_FEATURE_BIT_CFG(1, uVal, ARMV8_ID_AA64ISAR0_EL1_SHA2,  ARMV8_ID_AA64ISAR0_EL1_SHA2_SUPPORTED_SHA256,           pConfig->enmSha256, ARMV8_ID_AA64ISAR0_EL1_SHA2_NOT_IMPL);
            PORTABLE_DISABLE_FEATURE_BIT_CFG(1, uVal, ARMV8_ID_AA64ISAR0_EL1_SHA2,  ARMV8_ID_AA64ISAR0_EL1_SHA2_SUPPORTED_SHA256_SHA512,    pConfig->enmSha512, ARMV8_ID_AA64ISAR0_EL1_SHA2_NOT_IMPL);
            PORTABLE_DISABLE_FEATURE_BIT_CFG(1, uVal, ARMV8_ID_AA64ISAR0_EL1_CRC32, ARMV8_ID_AA64ISAR0_EL1_CRC32_SUPPORTED,                 pConfig->enmCrc32,  ARMV8_ID_AA64ISAR0_EL1_CRC32_NOT_IMPL);
            PORTABLE_DISABLE_FEATURE_BIT_CFG(1, uVal, ARMV8_ID_AA64ISAR0_EL1_SHA3,  ARMV8_ID_AA64ISAR0_EL1_SHA3_SUPPORTED,                  pConfig->enmSha3,   ARMV8_ID_AA64ISAR0_EL1_SHA3_NOT_IMPL);
        }

        pIdReg->uValue = uVal; /* write it back */
    }

    /*
     * ID_AA64PFR0_EL1
     */
    pIdReg = cpumR3CpuIdLookupGuestIdReg(pVM, ARMV8_AARCH64_SYSREG_ID_AA64PFR0_EL1);
    if (pIdReg)
    {
        uint64_t uVal = pIdReg->uValue;

        uint8_t uArchRev;
        int rc = CFGMR3QueryU8(pCpumCfg, "GicArchRev", &uArchRev);
        AssertRCReturn(rc, rc);
        if (uArchRev == GIC_DIST_REG_PIDR2_ARCHREV_GICV3)
            uVal = RT_BF_SET(uVal, ARMV8_ID_AA64PFR0_EL1_GIC, ARMV8_ID_AA64PFR0_EL1_GIC_V3_V4); /* 3.0 */
        else if (uArchRev == GIC_DIST_REG_PIDR2_ARCHREV_GICV4)
        {
            uint8_t uArchRevMinor = 0;
            rc = CFGMR3QueryU8Def(pCpumCfg, "GicArchRevMinor", &uArchRevMinor, 0);
            AssertRCReturn(rc, rc);
            uVal = uArchRevMinor == 0
                 ? RT_BF_SET(uVal, ARMV8_ID_AA64PFR0_EL1_GIC, ARMV8_ID_AA64PFR0_EL1_GIC_V3_V4)  /* 4.0 */
                 : RT_BF_SET(uVal, ARMV8_ID_AA64PFR0_EL1_GIC, ARMV8_ID_AA64PFR0_EL1_GIC_V4_1);  /* 4.1 */
        }
        else
            Assert(RT_BF_GET(uVal, ARMV8_ID_AA64PFR0_EL1_GIC) == ARMV8_ID_AA64PFR0_EL1_GIC_NOT_IMPL);

        pIdReg->uValue = uVal;  /* write it back */
    }

    /** @todo Other ID and feature registers. */

#undef PORTABLE_DISABLE_FEATURE_BIT_CFG
#undef PORTABLE_DISABLE_FEATURE_BIT
#undef PORTABLE_CLEAR_BITS_WHEN
#undef PASSTHRU_FEATURE

    return VINF_SUCCESS;
}


/**
 * Reads a value in /CPUM/IsaExts/ node.
 *
 * @returns VBox status code (error message raised).
 * @param   pVM             The cross context VM structure. (For errors.)
 * @param   pIsaExts        The /CPUM/IsaExts node (can be NULL).
 * @param   pszValueName    The value / extension name.
 * @param   penmValue       Where to return the choice.
 * @param   enmDefault      The default choice.
 */
static int cpumR3CpuIdReadIsaExtCfg(PVM pVM, PCFGMNODE pIsaExts, const char *pszValueName,
                                    CPUMISAEXTCFG *penmValue, CPUMISAEXTCFG enmDefault)
{
    /*
     * Try integer encoding first.
     */
    uint64_t uValue;
    int rc = CFGMR3QueryInteger(pIsaExts, pszValueName, &uValue);
    if (RT_SUCCESS(rc))
        switch (uValue)
        {
            case 0: *penmValue = CPUMISAEXTCFG_DISABLED; break;
            case 1: *penmValue = CPUMISAEXTCFG_ENABLED_SUPPORTED; break;
            case 2: *penmValue = CPUMISAEXTCFG_ENABLED_ALWAYS; break;
            case 9: *penmValue = CPUMISAEXTCFG_ENABLED_PORTABLE; break;
            default:
                return VMSetError(pVM, VERR_CPUM_INVALID_CONFIG_VALUE, RT_SRC_POS,
                                  "Invalid config value for '/CPUM/IsaExts/%s': %llu (expected 0/'disabled', 1/'enabled', 2/'portable', or 9/'forced')",
                                  pszValueName, uValue);
        }
    /*
     * If missing, use default.
     */
    else if (rc == VERR_CFGM_VALUE_NOT_FOUND || rc == VERR_CFGM_NO_PARENT)
        *penmValue = enmDefault;
    else
    {
        if (rc == VERR_CFGM_NOT_INTEGER)
        {
            /*
             * Not an integer, try read it as a string.
             */
            char szValue[32];
            rc = CFGMR3QueryString(pIsaExts, pszValueName, szValue, sizeof(szValue));
            if (RT_SUCCESS(rc))
            {
                RTStrToLower(szValue);
                size_t cchValue = strlen(szValue);
#define EQ(a_str) (cchValue == sizeof(a_str) - 1U && memcmp(szValue, a_str, sizeof(a_str) - 1))
                if (     EQ("disabled") || EQ("disable") || EQ("off") || EQ("no"))
                    *penmValue = CPUMISAEXTCFG_DISABLED;
                else if (EQ("enabled")  || EQ("enable")  || EQ("on")  || EQ("yes"))
                    *penmValue = CPUMISAEXTCFG_ENABLED_SUPPORTED;
                else if (EQ("forced")   || EQ("force")   || EQ("always"))
                    *penmValue = CPUMISAEXTCFG_ENABLED_ALWAYS;
                else if (EQ("portable"))
                    *penmValue = CPUMISAEXTCFG_ENABLED_PORTABLE;
                else if (EQ("default")  || EQ("def"))
                    *penmValue = enmDefault;
                else
                    return VMSetError(pVM, VERR_CPUM_INVALID_CONFIG_VALUE, RT_SRC_POS,
                                      "Invalid config value for '/CPUM/IsaExts/%s': '%s' (expected 0/'disabled', 1/'enabled', 2/'portable', or 9/'forced')",
                                      pszValueName, uValue);
#undef EQ
            }
        }
        if (RT_FAILURE(rc))
            return VMSetError(pVM, rc, RT_SRC_POS, "Error reading config value '/CPUM/IsaExts/%s': %Rrc", pszValueName, rc);
    }
    return VINF_SUCCESS;
}


#if 0
/**
 * Reads a value in /CPUM/IsaExts/ node, forcing it to DISABLED if wanted.
 *
 * @returns VBox status code (error message raised).
 * @param   pVM             The cross context VM structure. (For errors.)
 * @param   pIsaExts        The /CPUM/IsaExts node (can be NULL).
 * @param   pszValueName    The value / extension name.
 * @param   penmValue       Where to return the choice.
 * @param   enmDefault      The default choice.
 * @param   fAllowed        Allowed choice.  Applied both to the result and to
 *                          the default value.
 */
static int cpumR3CpuIdReadIsaExtCfgEx(PVM pVM, PCFGMNODE pIsaExts, const char *pszValueName,
                                      CPUMISAEXTCFG *penmValue, CPUMISAEXTCFG enmDefault, bool fAllowed)
{
    int rc;
    if (fAllowed)
        rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, pszValueName, penmValue, enmDefault);
    else
    {
        rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, pszValueName, penmValue, false /*enmDefault*/);
        if (RT_SUCCESS(rc) && *penmValue == CPUMISAEXTCFG_ENABLED_ALWAYS)
            LogRel(("CPUM: Ignoring forced '%s'\n", pszValueName));
        *penmValue = CPUMISAEXTCFG_DISABLED;
    }
    return rc;
}
#endif

static int cpumR3CpuIdReadConfig(PVM pVM, PCPUMCPUIDCONFIG pConfig, PCFGMNODE pCpumCfg)
{
    /** @cfgm{/CPUM/PortableCpuIdLevel, 8-bit, 0, 3, 0}
     * When non-zero CPUID features that could cause portability issues will be
     * stripped.  The higher the value the more features gets stripped.  Higher
     * values should only be used when older CPUs are involved since it may
     * harm performance and maybe also cause problems with specific guests. */
    int rc = CFGMR3QueryU8Def(pCpumCfg, "PortableCpuIdLevel", &pVM->cpum.s.u8PortableCpuIdLevel, 0);
    AssertLogRelRCReturn(rc, rc);

    /** @cfgm{/CPUM/GuestCpuName, string}
     * The name of the CPU we're to emulate.  The default is the host CPU.
     * Note! CPUs other than "host" one is currently unsupported. */
    rc = CFGMR3QueryStringDef(pCpumCfg, "GuestCpuName", pConfig->szCpuName, sizeof(pConfig->szCpuName), "host");
    AssertLogRelRCReturn(rc, rc);

    /*
     * Instruction Set Architecture (ISA) Extensions.
     */
    PCFGMNODE pIsaExts = CFGMR3GetChild(pCpumCfg, "IsaExts");
    if (pIsaExts)
    {
        rc = CFGMR3ValidateConfig(pIsaExts, "/CPUM/IsaExts/",
                                   "AES"
                                   "|PMULL"
                                   "|SHA1"
                                   "|SHA256"
                                   "|SHA512"
                                   "|CRC32"
                                   "|SHA3"
                                  , "" /*pszValidNodes*/, "CPUM" /*pszWho*/, 0 /*uInstance*/);
        if (RT_FAILURE(rc))
            return rc;
    }

    /** @cfgm{/CPUM/IsaExts/AES, boolean, depends}
     * Expose FEAT_AES instruction set extension to the guest.
     */
    rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, "AES", &pConfig->enmAes, true /*enmDefault*/);
    AssertLogRelRCReturn(rc, rc);

    /** @cfgm{/CPUM/IsaExts/AES, boolean, depends}
     * Expose FEAT_AES and FEAT_PMULL instruction set extension to the guest.
     */
    rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, "PMULL", &pConfig->enmPmull, true /*enmDefault*/);
    AssertLogRelRCReturn(rc, rc);

    /** @cfgm{/CPUM/IsaExts/SHA1, boolean, depends}
     * Expose FEAT_SHA1 instruction set extension to the guest.
     */
    rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, "SHA1", &pConfig->enmSha1, true /*enmDefault*/);
    AssertLogRelRCReturn(rc, rc);

    /** @cfgm{/CPUM/IsaExts/SHA256, boolean, depends}
     * Expose FEAT_SHA256 instruction set extension to the guest.
     */
    rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, "SHA256", &pConfig->enmSha256, true /*enmDefault*/);
    AssertLogRelRCReturn(rc, rc);

    /** @cfgm{/CPUM/IsaExts/SHA512, boolean, depends}
     * Expose FEAT_SHA256 and FEAT_SHA512 instruction set extension to the guest.
     */
    rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, "SHA512", &pConfig->enmSha512, true /*enmDefault*/);
    AssertLogRelRCReturn(rc, rc);

    /** @cfgm{/CPUM/IsaExts/CRC32, boolean, depends}
     * Expose FEAT_CRC32 instruction set extension to the guest.
     */
    rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, "CRC32", &pConfig->enmCrc32, true /*enmDefault*/);
    AssertLogRelRCReturn(rc, rc);

    /** @cfgm{/CPUM/IsaExts/SHA3, boolean, depends}
     * Expose FEAT_SHA3 instruction set extension to the guest.
     */
    rc = cpumR3CpuIdReadIsaExtCfg(pVM, pIsaExts, "SHA3", &pConfig->enmSha3, true /*enmDefault*/);
    AssertLogRelRCReturn(rc, rc);

    /** @todo Add more options for other extensions. */

    return VINF_SUCCESS;
}


/**
 * System ID registers to query and consider for sanitizing.
 */
static struct
{
    uint32_t    idReg;
    uint32_t    fSet : 1;
    uint32_t    fAssert : 1;
    const char *pszName;
} const g_aSysIdRegs[] =
{
    /* This is pretty much the same list as in SUPDrv.cpp (supdrvIOCtl_ArmGetSysRegsOnCpu). */
#define READ_SYS_REG_NAMED_EX(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2, a_SysRegName, a_fSet) \
        {   ARMV8_AARCH64_SYSREG_ID_CREATE(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2), \
            (a_fSet), \
               ARMV8_AARCH64_SYSREG_ID_CREATE(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2) \
            == RT_CONCAT(ARMV8_AARCH64_SYSREG_, a_SysRegName) ? 0 : -2, \
            #a_SysRegName }
#define READ_SYS_REG_NOSET(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2, a_SysRegName) \
    READ_SYS_REG_NAMED_EX(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2, a_SysRegName, false)
#define READ_SYS_REG_NAMED(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2, a_SysRegName) \
    READ_SYS_REG_NAMED_EX(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2, a_SysRegName, true)
#define READ_SYS_REG_UNDEF(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2) \
    {   ARMV8_AARCH64_SYSREG_ID_CREATE(a_Op0, a_Op1, a_CRn, a_CRm, a_Op2), true, 0, \
        "s" #a_Op0 "_" #a_Op1 "_c" #a_CRn "_c" #a_CRm "_" #a_Op2 /* gnu assembly compatible format */ }

    /*
     * Standard ID registers.
     *
     * DDI0487L.a section D23.3.1, 3rd item in note states that the registers in
     * the range 3,0,0,2,0 thru 3,0,0,7,7 are defined to be accessible and if not
     * defined will read-as-zero.
     */

    /* The first three seems to be in a sparse block. Haven't found any docs on
       what the Op2 values 1-4 and 7 may do if read. */
    READ_SYS_REG_NAMED(3, 0, 0, 0, 0, MIDR_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 0, 5, MPIDR_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 0, 6, REVIDR_EL1),

    /* AArch64 feature registers
       The CRm values 4 thru 7 are RAZ when undefined as per the D23.3.1 note. */
    READ_SYS_REG_NAMED(3, 0, 0, 4, 0, ID_AA64PFR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 4, 1, ID_AA64PFR1_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 4, 2, ID_AA64PFR2_EL1),
    READ_SYS_REG_UNDEF(3, 0, 0, 4, 3),
    READ_SYS_REG_NAMED(3, 0, 0, 4, 4, ID_AA64ZFR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 4, 5, ID_AA64SMFR0_EL1),
    READ_SYS_REG_UNDEF(3, 0, 0, 4, 6),
    READ_SYS_REG_NAMED(3, 0, 0, 4, 7, ID_AA64FPFR0_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 5, 0, ID_AA64DFR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 5, 1, ID_AA64DFR1_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 5, 2, ID_AA64DFR2_EL1),
    READ_SYS_REG_UNDEF(3, 0, 0, 5, 3),
    READ_SYS_REG_NAMED(3, 0, 0, 5, 4, ID_AA64AFR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 5, 5, ID_AA64AFR1_EL1),
    READ_SYS_REG_UNDEF(3, 0, 0, 5, 6),
    READ_SYS_REG_UNDEF(3, 0, 0, 5, 7),

    READ_SYS_REG_NAMED(3, 0, 0, 6, 0, ID_AA64ISAR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 6, 1, ID_AA64ISAR1_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 6, 2, ID_AA64ISAR2_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 6, 3, ID_AA64ISAR3_EL1),
    READ_SYS_REG_UNDEF(3, 0, 0, 6, 4),
    READ_SYS_REG_UNDEF(3, 0, 0, 6, 5),
    READ_SYS_REG_UNDEF(3, 0, 0, 6, 6),
    READ_SYS_REG_UNDEF(3, 0, 0, 6, 7),

    READ_SYS_REG_NAMED(3, 0, 0, 7, 0, ID_AA64MMFR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 7, 1, ID_AA64MMFR1_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 7, 2, ID_AA64MMFR2_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 7, 3, ID_AA64MMFR3_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 7, 4, ID_AA64MMFR4_EL1),
    READ_SYS_REG_UNDEF(3, 0, 0, 7, 5),
    READ_SYS_REG_UNDEF(3, 0, 0, 7, 6),
    READ_SYS_REG_UNDEF(3, 0, 0, 7, 7),

    /* AArch32 feature registers (covered by the D23.3.1 note). */
    READ_SYS_REG_NAMED(3, 0, 0, 1, 0, ID_PFR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 1, 1, ID_PFR1_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 1, 2, ID_DFR0_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 1, 3, ID_AFR0_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 1, 4, ID_MMFR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 1, 5, ID_MMFR1_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 1, 6, ID_MMFR2_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 1, 7, ID_MMFR3_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 2, 0, ID_ISAR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 2, 1, ID_ISAR1_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 2, 2, ID_ISAR2_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 2, 3, ID_ISAR3_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 2, 4, ID_ISAR4_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 2, 5, ID_ISAR5_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 2, 6, ID_MMFR4_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 2, 7, ID_ISAR6_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 3, 0, MVFR0_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 3, 1, MVFR1_EL1),
    READ_SYS_REG_NAMED(3, 0, 0, 3, 2, MVFR2_EL1),

    READ_SYS_REG_UNDEF(3, 0, 0, 3, 3),

    READ_SYS_REG_NAMED(3, 0, 0, 3, 4, ID_PFR2_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 3, 5, ID_DFR1_EL1),

    READ_SYS_REG_NAMED(3, 0, 0, 3, 6, ID_MMFR5_EL1),

    READ_SYS_REG_UNDEF(3, 0, 0, 3, 7),

    /*
     * Feature dependent registers outside the ID block:
     */
    READ_SYS_REG_NAMED(3, 0, 5, 3, 0, ERRIDR_EL1),      /* FEAT_RAS */

    READ_SYS_REG_NAMED(3, 0, 9,  9, 7, PMSIDR_EL1),     /* FEAT_SPS */
    READ_SYS_REG_NAMED(3, 0, 9, 10, 7, PMBIDR_EL1),     /* FEAT_SPS*/

    READ_SYS_REG_NAMED(3, 0, 9, 11, 7, TRBIDR_EL1),     /* FEAT_TRBE */

    READ_SYS_REG_NAMED(3, 0, 9, 14, 6, PMMIR_EL1),      /* FEAT_PMUv3p4 */

    READ_SYS_REG_NAMED(3, 0, 10, 4, 4, MPAMIDR_EL1),    /* FEAT_MPAM */
    READ_SYS_REG_NAMED(3, 0, 10, 4, 5, MPAMBWIDR_EL1),  /* FEAT_MPAM_PE_BW_CTRL (&& FEAT_MPAM) */

    /// @todo LORID_EL1 3,0,10,4,7  - FEAT_LOR
    /// @todo PMCEID0_EL0 ?
    /// @todo PMCEID1_EL0 ?
    /// @todo AMCFGR_EL0 ?
    /// @todo AMCGCR_EL0 ?
    /// @todo AMCG1IDR_EL0 ?
    /// @todo AMEVTYPER0<n>_EL0 ?

    READ_SYS_REG_NAMED(3, 1, 0, 0, 4, GMID_EL1),        /* FEAT_MTE2 */

    READ_SYS_REG_NAMED(3, 1, 0, 0, 6, SMIDR_EL1),       /* FEAT_SME */

    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0,  8, 7, TRCIDR0),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0,  9, 7, TRCIDR1),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0, 10, 7, TRCIDR2),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0, 11, 7, TRCIDR3),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0, 12, 7, TRCIDR4),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0, 13, 7, TRCIDR5),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0, 14, 7, TRCIDR6),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0, 15, 7, TRCIDR7),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0,  0, 6, TRCIDR8),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0,  1, 6, TRCIDR10),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0,  2, 6, TRCIDR11),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0,  4, 6, TRCIDR12),  */
    /** @todo FEAT_ETE: READ_SYS_REG_NAMED(2, 1, 0,  5, 6, TRCIDR13),  */

    READ_SYS_REG_NAMED(2, 1, 7, 15, 6, TRCDEVARCH),     /* FEAT_ETE */

    /*
     * Collections of other read-only registers.
     */
    READ_SYS_REG_NAMED(3, 1, 0, 0, 1, CLIDR_EL1),   /* cache level id register */
    READ_SYS_REG_NAMED(3, 1, 0, 0, 7, AIDR_EL1),
    READ_SYS_REG_NAMED(3, 3, 0, 0, 1, CTR_EL0),     /* cache type register */
    READ_SYS_REG_NAMED(3, 3, 0, 0, 7, DCZID_EL0),
    READ_SYS_REG_NOSET(3, 3,14, 0, 0, CNTFRQ_EL0),

#undef READ_SYS_REG_NAMED_EX
#undef READ_SYS_REG_NAMED
#undef READ_SYS_REG_NOSET
#undef READ_SYS_REG_UNDEF
};


/**
 * Gets the name of the ID register for logging.
 */
VMMR3_INT_DECL(const char *) CPUMR3CpuIdGetIdRegName(uint32_t idReg, char pszFallback[32])
{
    for (uint32_t i = 0; i < RT_ELEMENTS(g_aSysIdRegs); i++)
        if (g_aSysIdRegs[i].idReg == idReg)
            return g_aSysIdRegs[i].pszName;
    RTStrPrintf(pszFallback, 32, "s%u_%u_c%u_c%u_%u", /* gnu assembly compatible format */
                ARMV8_AARCH64_SYSREG_ID_GET_OP0(idReg),
                ARMV8_AARCH64_SYSREG_ID_GET_OP1(idReg),
                ARMV8_AARCH64_SYSREG_ID_GET_CRN(idReg),
                ARMV8_AARCH64_SYSREG_ID_GET_CRM(idReg),
                ARMV8_AARCH64_SYSREG_ID_GET_OP2(idReg));
    return pszFallback;
}


static PSUPARMSYSREGVAL cpumCpuIdLookupOrInsertIdReg(PSUPARMSYSREGVAL *ppaIdRegs, uint32_t *pcIdRegs,
                                                     uint32_t idReg, uint64_t uInsertValue)
{
    /*
     * Find the location we should insert the register.
     * Doing a linear search here because binary is too much bother.
     */
    PSUPARMSYSREGVAL paIdRegs = *ppaIdRegs;
    uint32_t         cIdRegs  = *pcIdRegs;
    uint32_t         idx      = 0;
    while (idx < cIdRegs && paIdRegs[idx].idReg < idReg)
        idx++;
    if (idx < cIdRegs && paIdRegs[idx].idReg == idReg)
        return &paIdRegs[idx];
    Assert(idx >= cIdRegs || paIdRegs[idx].idReg > idReg);

    /*
     * Reallocate the array and make space for the new entry.
     */
    paIdRegs = (PSUPARMSYSREGVAL)RTMemRealloc(paIdRegs, sizeof(paIdRegs[0]) * (cIdRegs + 1));
    AssertReturn(paIdRegs, NULL);
    *ppaIdRegs = paIdRegs;

    if (idx < cIdRegs)
        memmove(&paIdRegs[idx + 1], &paIdRegs[idx], sizeof(paIdRegs[0]) * (cIdRegs - idx));

    paIdRegs[idx].idReg  = idReg;
    paIdRegs[idx].fFlags = SUP_ARM_SYS_REG_VAL_F_FROM_CPUM;
    paIdRegs[idx].uValue = uInsertValue;

    *pcIdRegs = cIdRegs + 1;

    return &paIdRegs[idx];
}


/**
 * Sets up the multiprocessor affinity register (MPIDR_EL1) for all vCPUs.
 *
 * @returns VBox status code.
 * @param   pVM         The cross context VM structure.
 */
static int cpumCpuIdSetupMpIdr(PVM pVM)
{
    /* This code only deals with 256 cores at the moment. */
    AssertLogRelReturn(pVM->cCpus <= 256, VERR_TOO_MANY_CPUS);

    /*
     * Set it up for vCPU #0.
     */
    PSUPARMSYSREGVAL pMpIdr = cpumCpuIdLookupOrInsertIdReg(&pVM->cpum.s.GuestInfo.paIdRegsR3, &pVM->cpum.s.GuestInfo.cIdRegs,
                                                           ARMV8_AARCH64_SYSREG_MPIDR_EL1, 0 /*uInsertValue*/);
    AssertReturn(pMpIdr, VERR_NO_MEMORY);
    if (pVM->cCpus == 1)
        pMpIdr->uValue = RT_BIT_64(31) /* RES1 */
                       | RT_BIT_64(30) /* uniprocessor system */;
    else
    {
        uint64_t const fMT = pMpIdr->uValue & RT_BIT_64(24); /* Preserve the MT bit. */
        pMpIdr->uValue = RT_BIT_64(31) /* RES1 */ | fMT;

        /*
         * Then for any other vCPUs.
         */
        for (uint32_t idCpu = 1; idCpu < pVM->cCpus; idCpu++)
        {
            PVMCPU const pVCpu = pVM->apCpusR3[idCpu];
            pMpIdr = cpumCpuIdLookupOrInsertIdReg(&pVCpu->cpum.s.paIdRegsR3, &pVCpu->cpum.s.cIdRegs,
                                                  ARMV8_AARCH64_SYSREG_MPIDR_EL1, 0 /*uInsertValue*/);
            AssertReturn(pMpIdr, VERR_NO_MEMORY);

            pMpIdr->uValue = RT_BIT_64(31) /* RES1 */
                           | fMT
                           | idCpu /* Aff0 = idCpu */;
        }
    }

    return VINF_SUCCESS;
}


/**
 * Populate guest feature ID registers.
 *
 * This operates in two modes:
 *      -# pfnQuery != NULL: Determin the guest feature register values and sets
 *         them in the execution manager calling us.
 *      -# pfnQuery == NULL: Enumerate the guest feature registers and set them
 *         in the execution manager calling us.
 *
 * @returns VBox status code.
 * @param   pVM         The cross context VM structure.
 * @param   pVCpu       For passing along to the callbacks.
 * @param   pfnQuery    Callback for query register values in order to establish
 *                      the guest feature ID register values. Specify NULL to
 *                      just get called back to set the value.
 * @param   pfnUpdate   Callback for update (setting) register values.
 * @param   pvUser      User specific callback argument.
 */
VMMR3_INT_DECL(int) CPUMR3PopulateGuestFeaturesViaCallbacks(PVM pVM, PVMCPU pVCpu, PFNCPUMARMCPUIDREGQUERY pfnQuery,
                                                            PFNCPUMARMCPUIDREGUPDATE pfnUpdate, void *pvUser)
{
    Assert(pfnUpdate);

    /*
     * If pfnQuery is given, we must determin the guest feature register values first.
     */
    if (pfnQuery)
    {
        Assert(pVCpu->idCpu == 0);

        /*
         * Read the configuration.
         */
        PCFGMNODE const pCpumCfg = CFGMR3GetChild(CFGMR3GetRoot(pVM), "CPUM");
        CPUMCPUIDCONFIG Config;
        RT_ZERO(Config);

        int rc = cpumR3CpuIdReadConfig(pVM, &Config, pCpumCfg);
        AssertRCReturn(rc, rc);

        /*
         * Query all the registers we might find interesting...
         */
        uint32_t         cIdRegsAlloc = RT_ELEMENTS(g_aSysIdRegs);
        PSUPARMSYSREGVAL paIdRegs     = (PSUPARMSYSREGVAL)RTMemAlloc(cIdRegsAlloc * sizeof(paIdRegs[0]));
        AssertReturn(paIdRegs, VERR_NO_MEMORY);
        uint32_t         cIdRegs = 0;
        for (uint32_t i = 0; i < RT_ELEMENTS(g_aSysIdRegs); i++)
        {
            paIdRegs[cIdRegs].uValue = 0;
            rc = pfnQuery(pVM, pVCpu, g_aSysIdRegs[i].idReg, pvUser, &paIdRegs[cIdRegs].uValue);
            if (RT_SUCCESS(rc))
            {
                paIdRegs[cIdRegs].idReg  = g_aSysIdRegs[i].idReg;
                paIdRegs[cIdRegs].fFlags = SUP_ARM_SYS_REG_VAL_F_FROM_EXEC_ENGINE;
                if (!g_aSysIdRegs[i].fSet)
                    paIdRegs[cIdRegs].fFlags |= SUP_ARM_SYS_REG_VAL_F_NOSET;
                cIdRegs++;
            }
            else
                AssertLogRelMsgReturn(rc == VERR_CPUM_UNSUPPORTED_ID_REGISTER,
                                      ("idReg=%#x %s - %Rrc\n", g_aSysIdRegs[i].idReg, g_aSysIdRegs[i].pszName, rc),
                                      rc);
        }

        /* Without counting too closely, we must at least get some 6 register
           values here or something is seriously busted. */
        AssertLogRelMsgReturnStmt(cIdRegs >= 6, ("cIdRegsAlloc=%d\n", cIdRegsAlloc),
                                  RTMemFree(paIdRegs), VERR_INTERNAL_ERROR_4);

        /* Sort the register values to facilitate binary lookup and such. */
        RTSortShell(paIdRegs, cIdRegs, sizeof(paIdRegs[0]), cpumCpuIdSysRegValSortCmp, NULL);

#if defined(RT_ARCH_ARM64)
        /* If the MIDR_EL1 register is zero, use the host implementor value and
           indicate that the architectural features are identifies via the ID
           registers.  This is a HACK to get older macOS hosts working. */
        PSUPARMSYSREGVAL const pMainIdReg = cpumCpuIdLookupOrInsertIdReg(&paIdRegs, &cIdRegs, ARMV8_AARCH64_SYSREG_MIDR_EL1, 0);
        AssertLogRelReturn(pMainIdReg, VERR_NO_MEMORY);
        if (pMainIdReg->uValue == 0)
        {
            pMainIdReg->uValue = ((uint32_t)g_CpumHostFeatures.s.uImplementeter << 24) | UINT32_C(0x000f0000);
            LogRel(("CPUM: Setting MIDR_EL1 to %#RX64 (hack)\n", pMainIdReg->uValue));
        }
#endif

        /*
         * Install the raw array.
         */
        RTMemFree(pVM->cpum.s.GuestInfo.paIdRegsR3);
        pVM->cpum.s.GuestInfo.paIdRegsR3 = paIdRegs;
        pVM->cpum.s.GuestInfo.cIdRegs    = cIdRegs;
        LogRel(("CPUM: %u guest ID registers\n", cIdRegs));

#if 0
        /*
         * Get the guest CPU data from the database and/or the host.
         *
         * The CPUID and MSRs are currently living on the regular heap to avoid
         * fragmenting the hyper heap (and because there isn't/wasn't any realloc
         * API for the hyper heap).  This means special cleanup considerations.
         */
        rc = cpumR3DbGetCpuInfo(Config.szCpuName, &pCpum->GuestInfo);
        if (RT_FAILURE(rc))
            return rc == VERR_CPUM_DB_CPU_NOT_FOUND
                 ? VMSetError(pVM, rc, RT_SRC_POS,
                              "Info on guest CPU '%s' could not be found. Please, select a different CPU.", Config.szCpuName)
                 : rc;
#endif

        /*
         * The multiprocessor affinity register, MPIDR_EL1, may need setup and
         * extra per-CPU copies.
         */
        rc = cpumCpuIdSetupMpIdr(pVM);
        AssertRCReturn(rc, rc);

        /*
         * Pre-explode the CPU ID register info (ignore errors).
         */
        CPUMCpuIdExplodeFeaturesArmV8(paIdRegs, cIdRegs, &pVM->cpum.s.GuestFeatures);

        /*
         * Sanitize the cpuid information passed on to the guest.
         */
        rc = cpumR3CpuIdSanitize(pVM, &pVM->cpum.s, &Config, pCpumCfg);
        AssertLogRelRCReturn(rc, rc);

        /*
         * Explode the sanitized CPU ID register info.
         */
        rc = CPUMCpuIdExplodeFeaturesArmV8(pVM->cpum.s.GuestInfo.paIdRegsR3, pVM->cpum.s.GuestInfo.cIdRegs,
                                           &pVM->cpum.s.GuestFeatures);
        AssertLogRelRCReturn(rc, rc);
    }

    /*
     * Set the values.
     *
     * We traverse pVM->cpum.s.GuestInfo.paIdRegsR3 in parallel to the sparse
     * override table pVCpu->cpum.s.paIdRegsR3.  This ASSUMES that the override
     * table is correctly sorted and does NOT contain anything that isn't in
     * the main table!
     */
    int                     rcRet        = VINF_SUCCESS;
    PSUPARMSYSREGVAL  const paIdRegsOvrd = pVCpu->cpum.s.paIdRegsR3;
    uint32_t const          cIdRegsOvrd  = pVCpu->cpum.s.cIdRegs;
    PSUPARMSYSREGVAL  const paIdRegs     = pVM->cpum.s.GuestInfo.paIdRegsR3;
    uint32_t const          cIdRegs      = pVM->cpum.s.GuestInfo.cIdRegs;
    for (uint32_t i = 0, iOvrd = 0; i < cIdRegs; i++)
    {
        uint32_t const idReg = paIdRegs[i].idReg;
        AssertLogRel(iOvrd >= cIdRegsOvrd || paIdRegsOvrd[iOvrd].idReg >= idReg);

        if (!(paIdRegs[i].fFlags & SUP_ARM_SYS_REG_VAL_F_NOSET))
        {
            char           szName[32];
            uint64_t const uNewValue     = iOvrd >= cIdRegsOvrd || paIdRegsOvrd[iOvrd].idReg != idReg
                                         ? paIdRegs[i].uValue : paIdRegsOvrd[iOvrd].uValue;
            uint64_t       uUpdatedValue = uNewValue;
            int const      rc2           = pfnUpdate(pVM, pVCpu, idReg, uNewValue, pvUser, &uUpdatedValue);
            if (RT_SUCCESS(rc2))
            {
                if (uUpdatedValue != uNewValue && pfnQuery)
                {
                    LogRel(("CPUM: idReg=%#x (%s) pfnUpdate adjusted %#RX64 -> %#RX64\n",
                            idReg, CPUMR3CpuIdGetIdRegName(idReg, szName), uNewValue, uUpdatedValue));
                    Assert(cIdRegsOvrd == 0);
                    paIdRegs[i].uValue = uUpdatedValue;
                }
                else
                    AssertLogRelMsg(uUpdatedValue == uNewValue,
                                    ("idCpu=%u idReg=%#x (%s) value: %#RX64 -> %#RX64\n", pVCpu->idCpu, idReg,
                                     CPUMR3CpuIdGetIdRegName(idReg, szName), uNewValue, uUpdatedValue));
            }
            else
            {
                LogRel(("CPUM: Error: pfnUpdate(idCpu=%u idReg=%#x (%s) value=%#RX64) failed: %Rrc\n", pVCpu->idCpu,
                        idReg, CPUMR3CpuIdGetIdRegName(idReg, szName), uNewValue, rc2));
                if (RT_SUCCESS(rcRet))
                    rcRet = rc2;
            }
        }

        if (iOvrd < cIdRegsOvrd && paIdRegsOvrd[iOvrd].idReg == idReg)
            iOvrd += 1;
    }
    return rcRet;
}


/**
 * Query an ARM system ID register value.
 *
 * @retval  VINF_SUCCESS
 * @retval  VERR_CPUM_UNSUPPORTED_ID_REGISTER
 * @param   pVM         The cross context VM structure.
 * @param   idReg       The ID register to query.
 * @param   puValue     Where to return the value.
 */
VMMR3_INT_DECL(int) CPUMR3QueryGuestIdReg(PVM pVM, uint32_t idReg, uint64_t *puValue)
{
    PCSUPARMSYSREGVAL pIdReg = cpumR3CpuIdLookupGuestIdReg(pVM, idReg);
    if (pIdReg)
    {
        *puValue = pIdReg->uValue;
        return VINF_SUCCESS;
    }
    *puValue = 0;
    return VERR_CPUM_UNSUPPORTED_ID_REGISTER;
}


/*
 *
 *
 * Saved state related code.
 * Saved state related code.
 * Saved state related code.
 *
 *
 */

/**
 * Old ARMv8 CPU ID registers structure - version 1 & 2 saved states.
 */
typedef struct CPUMARMV8OLDIDREGS
{
    /** Content of the ID_AA64PFR0_EL1 register. */
    uint64_t        u64RegIdAa64Pfr0El1;
    /** Content of the ID_AA64PFR1_EL1 register. */
    uint64_t        u64RegIdAa64Pfr1El1;
    /** Content of the ID_AA64DFR0_EL1 register. */
    uint64_t        u64RegIdAa64Dfr0El1;
    /** Content of the ID_AA64DFR1_EL1 register. */
    uint64_t        u64RegIdAa64Dfr1El1;
    /** Content of the ID_AA64AFR0_EL1 register. */
    uint64_t        u64RegIdAa64Afr0El1;
    /** Content of the ID_AA64AFR1_EL1 register. */
    uint64_t        u64RegIdAa64Afr1El1;
    /** Content of the ID_AA64ISAR0_EL1 register. */
    uint64_t        u64RegIdAa64Isar0El1;
    /** Content of the ID_AA64ISAR1_EL1 register. */
    uint64_t        u64RegIdAa64Isar1El1;
    /** Content of the ID_AA64ISAR2_EL1 register. */
    uint64_t        u64RegIdAa64Isar2El1;
    /** Content of the ID_AA64MMFR0_EL1 register. */
    uint64_t        u64RegIdAa64Mmfr0El1;
    /** Content of the ID_AA64MMFR1_EL1 register. */
    uint64_t        u64RegIdAa64Mmfr1El1;
    /** Content of the ID_AA64MMFR2_EL1 register. */
    uint64_t        u64RegIdAa64Mmfr2El1;
    /** Content of the CLIDR_EL1 register. */
    uint64_t        u64RegClidrEl1;
    /** Content of the CTR_EL0 register. */
    uint64_t        u64RegCtrEl0;
    /** Content of the DCZID_EL0 register. */
    uint64_t        u64RegDczidEl0;
} CPUMARMV8OLDIDREGS;


/** Saved state field descriptors for CPUMARMV8OLDIDREGS. */
static const SSMFIELD g_aCpumArmV8OldIdRegsFields[] =
{
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Pfr0El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Pfr1El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Dfr0El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Dfr1El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Afr0El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Afr1El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Isar0El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Isar1El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Isar2El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Mmfr0El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Mmfr1El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegIdAa64Mmfr2El1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegClidrEl1),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegCtrEl0),
    SSMFIELD_ENTRY(CPUMARMV8OLDIDREGS, u64RegDczidEl0),
    SSMFIELD_ENTRY_TERM()
};


/** Translation table between CPUMARMV8OLDIDREGS and register numbers. */
static struct
{
    uint32_t idReg;
    uint32_t off;
} const g_aArmV8OldIdRegsOffsetsAndIds[] =
{
    {  ARMV8_AARCH64_SYSREG_ID_AA64PFR0_EL1,    RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Pfr0El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64PFR1_EL1,    RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Pfr1El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64DFR0_EL1,    RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Dfr0El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64DFR1_EL1,    RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Dfr1El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64AFR0_EL1,    RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Afr0El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64AFR1_EL1,    RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Afr1El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64ISAR0_EL1,   RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Isar0El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64ISAR1_EL1,   RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Isar1El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64ISAR2_EL1,   RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Isar2El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64MMFR0_EL1,   RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Mmfr0El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64MMFR1_EL1,   RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Mmfr1El1) },
    {  ARMV8_AARCH64_SYSREG_ID_AA64MMFR2_EL1,   RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegIdAa64Mmfr2El1) },
    {  ARMV8_AARCH64_SYSREG_CLIDR_EL1,          RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegClidrEl1) },
    {  ARMV8_AARCH64_SYSREG_CTR_EL0,            RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegCtrEl0) },
    {  ARMV8_AARCH64_SYSREG_DCZID_EL0,          RT_UOFFSETOF(CPUMARMV8OLDIDREGS, u64RegDczidEl0) },
};


/**
 * Called both in pass 0 and the final pass.
 *
 * @param   pVM                 The cross context VM structure.
 * @param   pSSM                The saved state handle.
 */
void cpumR3SaveCpuId(PVM pVM, PSSMHANDLE pSSM)
{
    /*
     * Save all the main CPU ID register values.
     */
    PCSUPARMSYSREGVAL const paIdRegs = pVM->cpum.s.GuestInfo.paIdRegsR3;
    uint32_t const          cIdRegs  = pVM->cpum.s.GuestInfo.cIdRegs;
    SSMR3PutU32(pSSM, cIdRegs);
    for (uint32_t i = 0; i < cIdRegs; i++)
    {
        SSMR3PutU32(pSSM, paIdRegs[i].idReg);
        SSMR3PutU64(pSSM, paIdRegs[i].uValue);
    }
    SSMR3PutU32(pSSM, UINT32_MAX);

    /*
     * Save the per-vCPU overrides.
     * Since vCPU #0 doesn't have any overrides, we start with #1.
     */
    for (VMCPUID idCpu = 1; idCpu < pVM->cCpus; idCpu++)
    {
        PVMCPU const            pVCpu        = pVM->apCpusR3[idCpu];
        PCSUPARMSYSREGVAL const paIdRegsOvrd = pVCpu->cpum.s.paIdRegsR3;
        uint32_t const          cIdRegsOvrd  = pVCpu->cpum.s.cIdRegs;
        SSMR3PutU32(pSSM, cIdRegsOvrd);
        for (uint32_t i = 0; i < cIdRegsOvrd; i++)
        {
            SSMR3PutU32(pSSM, paIdRegsOvrd[i].idReg);
            SSMR3PutU64(pSSM, paIdRegsOvrd[i].uValue);
        }
        SSMR3PutU32(pSSM, RT_MAKE_U32(idCpu, 0xfeed));
    }
}


/**
 * Verifies and sanitizes CPU ID register values saved by pass 0.
 *
 * @returns VBox status code.
 * @param   pVM         The cross context VM structure.
 * @param   pVCpu       The VMCPU structure to apply @a paIdRegs as overrides
 *                      to, NULL for the main structure.
 * @param   pSSM        The saved state handle.
 * @param   paIdRegs    The ID register values being loaded and installed.
 *                      On success ownership is taken over this array.
 *                      On failure the caller must free it.
 * @param   cIdRegs     The number of ID register values in @a paIdRegs.
 * @param   fNewVersion Set if we're loading a new state version.  Clear when
 *                      loading one based on CPUMARMV8OLDIDREGS.
 */
static int cpumR3LoadCpuIdInner(PVM pVM, PVMCPU pVCpu, PSSMHANDLE pSSM,
                                PSUPARMSYSREGVAL paIdRegs, uint32_t cIdRegs, bool fNewVersion)
{
    /*
     * This can be skipped.
     */
    bool fStrictCpuIdChecks;
    int rc = CFGMR3QueryBoolDef(CFGMR3GetChild(CFGMR3GetRoot(pVM), "CPUM"), "StrictCpuIdChecks", &fStrictCpuIdChecks, true);
    AssertRCReturn(rc, rc);

    /*
     * Define a bunch of macros for simplifying the santizing/checking code below.
     */
    /* For checking guest features. */
#define CPUID_GST_FEATURE_RET(a_uLoad, a_uCfg, a_Field) \
    do { \
        if (RT_BF_GET(a_uLoad, a_Field) > RT_BF_GET(a_uCfg, a_Field)) \
        { \
            if (fStrictCpuIdChecks) \
                return SSMR3SetLoadError(pSSM, VERR_SSM_LOAD_CPUID_MISMATCH, RT_SRC_POS, \
                                         N_(#a_Field " is not supported by the guest config / host (%#x) but has already exposed to the guest (%#x)"),\
                                         RT_BF_GET(a_uCfg, a_Field), RT_BF_GET(a_uLoad, a_Field) ); \
            LogRel(("CPUM: " #a_Field " is not supported by the guest config / host (%#x) but has already been exposed to the guest (%#x)\n", \
                    RT_BF_GET(a_uCfg, a_Field), RT_BF_GET(a_uLoad, a_Field) )); \
        } \
    } while (0)
#define CPUID_GST_FEATURE_RET_NOT_IMPL(a_uLoad, a_uCfg, a_Field, a_NotImpl) \
    do { \
        if (   (   RT_BF_GET(a_uLoad, a_Field) != (a_NotImpl) \
                && RT_BF_GET(a_uCfg,  a_Field) == (a_NotImpl)) \
            || RT_BF_GET(a_uLoad, a_Field) > RT_BF_GET(a_uCfg, a_Field)) \
        { \
            if (fStrictCpuIdChecks) \
                return SSMR3SetLoadError(pSSM, VERR_SSM_LOAD_CPUID_MISMATCH, RT_SRC_POS, \
                                         N_(#a_Field " is not supported by the guest config / host (%#x) but has already exposed to the guest (%#x)"),\
                                         RT_BF_GET(a_uCfg, a_Field), RT_BF_GET(a_uLoad, a_Field) ); \
            LogRel(("CPUM: " #a_Field " is not supported by the guest config / host (%#x) but has already been exposed to the guest (%#x)\n", \
                    RT_BF_GET(a_uCfg, a_Field), RT_BF_GET(a_uLoad, a_Field) )); \
        } \
    } while (0)
#define CPUID_GST_FEATURE_WRN(a_uLoad, a_uCfg, a_Field) \
    do { \
        if (RT_BF_GET(a_uLoad, a_Field) > RT_BF_GET(a_uCfg, a_Field)) \
            LogRel(("CPUM: " #a_Field " is not supported by the guest config / host (%#x) but has already been exposed to the guest (%#x)\n", \
                    RT_BF_GET(a_uCfg, a_Field), RT_BF_GET(a_uLoad, a_Field) )); \
    } while (0)
#if 0 /** @todo emulatate how/what? */
# define CPUID_GST_FEATURE_EMU(a_uLoad, a_uCfg, a_Field)  \
    do { \
        if (RT_BF_GET(a_uLoad, a_Field) > RT_BF_GET(a_uCfg, a_Field)) \
            LogRel(("CPUM: Warning - " #a_Field " is not supported by the guest config / host (%#x) but has already been exposed to the guest (%#x). This may impact performance.\n", \
                    RT_BF_GET(a_uCfg, a_Field), RT_BF_GET(a_uLoad, a_Field) )); \
    } while (0)
#endif
#define CPUID_GST_FEATURE_IGN(a_uLoad, a_uCfg, a_Field) do { } while (0)

    PSUPARMSYSREGVAL * const  ppaIdRegsDst = pVCpu ? &pVCpu->cpum.s.paIdRegsR3 : &pVM->cpum.s.GuestInfo.paIdRegsR3;
    uint32_t * const          pcIdRegsDst  = pVCpu ? &pVCpu->cpum.s.cIdRegs    : &pVM->cpum.s.GuestInfo.cIdRegs;

    PSUPARMSYSREGVAL const    paIdRegsCfg  = *ppaIdRegsDst;
    uint32_t   const          cIdRegsCfg   = *pcIdRegsDst;
    PCSUPARMSYSREGVAL         pCfg;
    PSUPARMSYSREGVAL          pLoad;
    uint64_t                  uCfg;
    uint64_t                  uLoad;
#define CPUID_GET_VALUES_FOR_ID_REG(a_idReg) \
    do { \
        pCfg  = cpumCpuIdLookupIdReg(paIdRegsCfg, cIdRegsCfg, (a_idReg)); \
        uCfg  = pCfg  ? pCfg->uValue  : 0; \
        pLoad = cpumCpuIdLookupIdReg(paIdRegs, cIdRegs, (a_idReg)); \
        uLoad = pLoad ? pLoad->uValue : 0; \
    } while (0)


    /*
     * Verify that we can support the features already exposed to the guest on
     * this host.
     *
     * Most of the features we're emulating requires intercepting instruction
     * and doing it the slow way, so there is no need to warn when they aren't
     * present in the host CPU.  Thus we use IGN instead of EMU on these.
     *
     * Trailing comments:
     *      "EMU"  - Possible to emulate, could be lots of work and very slow.
     *      "EMU?" - Can this be emulated?
     */
    /* ID_AA64ISAR0_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64ISAR0_EL1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_AES);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_SHA1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_SHA2);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_CRC32);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_ATOMIC);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_TME);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_RDM);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_SHA3);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_SM3);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_SM4);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_DP);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_FHM);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_TS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_TLB);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR0_EL1_RNDR);

    /* ID_AA64ISAR1_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64ISAR1_EL1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_DPB);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_APA);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_API);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_FJCVTZS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_LRCPC);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_GPA);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_GPI);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_FRINTTS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_SB);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_SPECRES);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_BF16);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_DGH);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_I8MM);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_XS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR1_EL1_LS64);

    /* ID_AA64ISAR2_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64ISAR2_EL1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR2_EL1_WFXT);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR2_EL1_RPRES);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR2_EL1_GPA3);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR2_EL1_APA3);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR2_EL1_MOPS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR2_EL1_BC);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64ISAR2_EL1_PACFRAC);

    /* ID_AA64PFR0_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64PFR0_EL1);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_EL0);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_EL1);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_EL2);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_EL3);
    CPUID_GST_FEATURE_RET_NOT_IMPL(uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_FP,      ARMV8_ID_AA64PFR0_EL1_FP_NOT_IMPL);      /* Special not implemented value. */
    CPUID_GST_FEATURE_RET_NOT_IMPL(uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_ADVSIMD, ARMV8_ID_AA64PFR0_EL1_ADVSIMD_NOT_IMPL); /* Special not implemented value. */
    CPUID_GST_FEATURE_IGN(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_GIC);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_RAS);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_SVE);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_SEL2);
    /** @todo MPAM */
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_AMU);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_DIT);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_RME);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_CSV2);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64PFR0_EL1_CSV3);

    /* ID_AA64PFR1_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64PFR1_EL1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64PFR1_EL1_BT);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64PFR1_EL1_SSBS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64PFR1_EL1_MTE);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64PFR1_EL1_RASFRAC);
    /** @todo MPAM. */
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64PFR1_EL1_SME);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64PFR1_EL1_RNDRTRAP);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64PFR1_EL1_CSV2FRAC);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64PFR1_EL1_NMI);

    /* ID_AA64MMFR0_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64MMFR0_EL1);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_PARANGE);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_ASIDBITS);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_BIGEND);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_SNSMEM);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_BIGENDEL0);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_TGRAN16);
    CPUID_GST_FEATURE_RET_NOT_IMPL(uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_TGRAN64, ARMV8_ID_AA64MMFR0_EL1_TGRAN64_NOT_IMPL);
    CPUID_GST_FEATURE_RET_NOT_IMPL(uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_TGRAN4,  ARMV8_ID_AA64MMFR0_EL1_TGRAN4_NOT_IMPL);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_TGRAN16_2);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_TGRAN64_2);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_TGRAN4_2);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_EXS);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_FGT);
    CPUID_GST_FEATURE_RET(         uLoad, uCfg, ARMV8_ID_AA64MMFR0_EL1_ECV);

    /* ID_AA64MMFR1_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64MMFR1_EL1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_HAFDBS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_VMIDBITS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_VHE);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_HPDS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_LO);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_PAN);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_SPECSEI);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_XNX);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_TWED);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_ETS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_HCX);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_AFP);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_NTLBPA);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_TIDCP1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR1_EL1_CMOW);

    /* ID_AA64MMFR2_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64MMFR2_EL1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_CNP);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_UAO);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_LSM);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_IESB);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_VARANGE);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_CCIDX);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_CNP);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_NV);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_ST);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_AT);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_IDS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_FWB);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_TTL);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_BBM);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_EVT);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64MMFR2_EL1_E0PD);

    /* ID_AA64DFR0_EL1 */
    CPUID_GET_VALUES_FOR_ID_REG(ARMV8_AARCH64_SYSREG_ID_AA64DFR0_EL1);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_DEBUGVER);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_TRACEVER);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_PMUVER);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_BRPS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_WRPS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_CTXCMPS);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_PMSVER);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_DOUBLELOCK);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_TRACEFILT);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_TRACEBUFFER);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_MTPMU);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_BRBE);
    CPUID_GST_FEATURE_RET(uLoad, uCfg, ARMV8_ID_AA64DFR0_EL1_HPMN0);

#undef CPUID_GST_FEATURE_RET
#undef CPUID_GST_FEATURE_RET_NOT_IMPL
#undef CPUID_GST_FEATURE_WRN
#undef CPUID_GST_FEATURE_EMU
#undef CPUID_GST_FEATURE_IGN

    /*
     * Any ID registers missing in the loaded state should be zeroed if this
     * is a new state we're loading.  This is not necessary for the overrides.
     *
     * For the old structure based state, we'll keep the values as-is and just
     * add them to the array to keep existing load behaviour.
     *
     * Note the code ASSUMES that both arrays are sorted!
     */
    if (!pVCpu && pVM->cpum.s.GuestInfo.cIdRegs)
    {
#ifdef VBOX_STRICT
        uint32_t                idCfgPrev   = 0;
        uint32_t                idLoadPrev  = 0;
#endif
        uint32_t const          cCfgIdRegs  = pVM->cpum.s.GuestInfo.cIdRegs;
        PCSUPARMSYSREGVAL const paCfgIdRegs = pVM->cpum.s.GuestInfo.paIdRegsR3;
        for (uint32_t idxCfg = 0, idxLoad = 0; idxCfg < cCfgIdRegs; idxCfg++)
        {
            uint32_t const idCfgReg = paCfgIdRegs[idxCfg].idReg;
            Assert(idCfgPrev < idCfgReg);

            /* Skip load registers not in the config list.  */
            while (idxLoad < cIdRegs && paIdRegs[idxLoad].idReg < idCfgReg)
            {
#ifdef VBOX_STRICT
                Assert(idLoadPrev < paIdRegs[idxLoad].idReg);
                idLoadPrev = paIdRegs[idxLoad].idReg;
#endif
                idxLoad++;
            }

            if (idxLoad >= cIdRegs || paIdRegs[idxLoad].idReg != idCfgReg)
            {
                paIdRegs = (PSUPARMSYSREGVAL)RTMemRealloc(paIdRegs, sizeof(paIdRegs[0]) * (cIdRegs + 1));
                AssertLogRelReturn(paIdRegs, VERR_NO_MEMORY);

                if (idxLoad < cIdRegs)
                {
                    Assert(paIdRegs[idxLoad].idReg > idCfgReg);
                    memmove(&paIdRegs[idxLoad + 1], &paIdRegs[idxLoad], sizeof(paIdRegs[0]) * (cIdRegs - idxLoad));
                }

                paIdRegs[idxLoad].idReg = idCfgReg;
                if (fNewVersion)
                {
                    paIdRegs[idxLoad].fFlags = paCfgIdRegs[idxCfg].fFlags | SUP_ARM_SYS_REG_VAL_F_LOAD_ZERO;
                    paIdRegs[idxLoad].uValue = 0;
                }
                else
                {
                    paIdRegs[idxLoad].fFlags = paCfgIdRegs[idxCfg].fFlags;
                    paIdRegs[idxLoad].uValue = paCfgIdRegs[idxCfg].uValue;
                }
                idxLoad++;
#ifdef VBOX_STRICT
                idLoadPrev = idCfgReg;
#endif
            }
#ifdef VBOX_STRICT
            idCfgPrev = idCfgReg;
#endif
        }
    }

    /*
     * Seems we're good, commit the CPU ID registers.
     */
    RTMemFree(*ppaIdRegsDst);
    *ppaIdRegsDst = paIdRegs;
    *pcIdRegsDst  = cIdRegs;
    return VINF_SUCCESS;
}


/**
 * Loads the CPU ID leaves saved by pass 0, ARMv8 targets.
 *
 * @returns VBox status code.
 * @param   pVM                 The cross context VM structure.
 * @param   pSSM                The saved state handle.
 * @param   uVersion            The format version.
 */
int cpumR3LoadCpuIdArmV8(PVM pVM, PSSMHANDLE pSSM, uint32_t uVersion)
{
    /*
     * Load the main ID register values.
     */
    uint32_t            cIdRegs;
    PSUPARMSYSREGVAL    paIdRegs;
    if (uVersion >= CPUM_SAVED_STATE_VERSION_ARMV8_IDREGS)
    {
        int rc = SSMR3GetU32(pSSM, &cIdRegs);
        AssertRCReturn(rc, rc);
        if (cIdRegs > 256)
            return SSMR3SetLoadError(pSSM, VERR_SSM_DATA_UNIT_FORMAT_CHANGED, RT_SRC_POS,
                                     N_("Too many ID registers: %u (%#x), max 256"), cIdRegs, cIdRegs);
        if (cIdRegs < 2)
            return SSMR3SetLoadError(pSSM, VERR_SSM_DATA_UNIT_FORMAT_CHANGED, RT_SRC_POS,
                                     N_("Too free ID registers: %u (%#x), min 2"), cIdRegs, cIdRegs);

        /* Load the values first without doing any validation. */
        paIdRegs = (PSUPARMSYSREGVAL)RTMemAllocZ(sizeof(paIdRegs[0]) * cIdRegs);
        AssertReturn(paIdRegs, VERR_NO_MEMORY);
        for (uint32_t i = 0; i < cIdRegs; i++)
        {
            SSMR3GetU32(pSSM, &paIdRegs[i].idReg);
            SSMR3GetU64(pSSM, &paIdRegs[i].uValue);
            paIdRegs[i].fFlags = SUP_ARM_SYS_REG_VAL_F_FROM_SAVED_STATE;
        }
        uint32_t uTerm = 0;
        rc = SSMR3GetU32(pSSM, &uTerm);
        AssertLogRelMsgStmt(RT_FAILURE(rc) || uTerm == UINT32_MAX, ("uTerm=%#x\n", uTerm),
                            rc = VERR_SSM_DATA_UNIT_FORMAT_CHANGED);
        if (RT_SUCCESS(rc))
        {
            /* The array shall be sorted and the values within the system register ID range. */
            for (uint32_t i = 0, idPrev = 0 /* ASSUMES no zero ID register */; i < cIdRegs; i++)
            {
                uint32_t const idReg = paIdRegs[i].idReg;
                AssertLogRelMsgStmt(idReg > idPrev, ("#%u: idReg=%#x vs idPrev=%#x\n", i, idReg, idPrev),
                                    rc = VERR_SSM_DATA_UNIT_FORMAT_CHANGED);
                AssertLogRelMsgStmt(idReg <= ARMV8_AARCH64_SYSREG_ID_CREATE(3, 7, 15, 15, 7), ("#%u: idReg=%#x\n", i, idReg),
                                    rc = VERR_SSM_DATA_UNIT_FORMAT_CHANGED);
                idPrev = idReg;
            }
        }
        if (RT_FAILURE(rc))
        {
            RTMemFree(paIdRegs);
            return rc;
        }
    }
    else
    {
        /* Old structure-based format. */
        CPUMARMV8OLDIDREGS OldIdRegs;
        int rc = SSMR3GetStructEx(pSSM, &OldIdRegs, sizeof(OldIdRegs), 0, g_aCpumArmV8OldIdRegsFields, NULL);
        AssertRCReturn(rc, rc);

        /* Convert the structure to the new array format. */
        paIdRegs = (PSUPARMSYSREGVAL)RTMemAllocZ(sizeof(paIdRegs[0]) * RT_ELEMENTS(g_aArmV8OldIdRegsOffsetsAndIds));
        AssertReturn(paIdRegs, VERR_NO_MEMORY);
        for (size_t i = 0; i < RT_ELEMENTS(g_aArmV8OldIdRegsOffsetsAndIds); i++)
        {
            paIdRegs[i].idReg  = g_aArmV8OldIdRegsOffsetsAndIds[i].idReg;
            paIdRegs[i].fFlags = SUP_ARM_SYS_REG_VAL_F_FROM_SAVED_STATE;
            paIdRegs[i].uValue = *(uint64_t *)((uintptr_t)&OldIdRegs + g_aArmV8OldIdRegsOffsetsAndIds[i].off);
        }

        RTSortShell(paIdRegs, RT_ELEMENTS(g_aArmV8OldIdRegsOffsetsAndIds), sizeof(paIdRegs[0]), cpumCpuIdSysRegValSortCmp, NULL);
        cIdRegs = RT_ELEMENTS(g_aArmV8OldIdRegsOffsetsAndIds);
    }

    /*
     * Go over the IDs an mark those that shouldn't be set as such.
     */
    uint32_t cFound = 0;
    for (uint32_t i = 0; i < cIdRegs; i++)
    {
        uint32_t const idReg = paIdRegs[i].idReg;
        uint32_t       j     = 0;
        while (j < RT_ELEMENTS(g_aSysIdRegs) && g_aSysIdRegs[j].idReg != idReg)
            j++;
        if (j < RT_ELEMENTS(g_aSysIdRegs))
        {
            if (!g_aSysIdRegs[j].fSet)
                paIdRegs[i].fFlags |= SUP_ARM_SYS_REG_VAL_F_NOSET;
            cFound++;
        }
    }
    /* not quite sure about this heuristic... */
    int rc;
    if (cIdRegs < 2 || cFound >= RT_MIN(cIdRegs / 2, RT_ELEMENTS(g_aSysIdRegs) / 2))
    {
        /*
         * Sanitize the loaded ID registers and apply them.
         */
        rc = cpumR3LoadCpuIdInner(pVM, NULL, pSSM, paIdRegs, cIdRegs, uVersion >= CPUM_SAVED_STATE_VERSION_ARMV8_IDREGS);

        /*
         * Load per-cpu override values.
         */
        if (RT_SUCCESS(rc) && uVersion >= CPUM_SAVED_STATE_VERSION_ARMV8_IDREGS2)
        {
            /* There are no override values for vCPU #0, so we start the loading with #1. */
            AssertLogRel(pVM->apCpusR3[0]->cpum.s.cIdRegs == 0);
            for (uint32_t idCpu = 1; idCpu < pVM->cCpus; idCpu++)
            {
                PVMCPU const pVCpu = pVM->apCpusR3[idCpu];

                /* The the register count and validate it: */
                uint32_t cIdRegsOvrd = 0;
                rc = SSMR3GetU32(pSSM, &cIdRegsOvrd);
                AssertRCReturn(rc, rc);
                if (cIdRegsOvrd > cIdRegs) /* Using the saved state value here, as we may have extra zero'ing entries. */
                    return SSMR3SetLoadError(pSSM, VERR_SSM_DATA_UNIT_FORMAT_CHANGED, RT_SRC_POS,
                                             N_("Too many override ID registers for vCPU #%u: %u (%#x), only %u main regs"),
                                             idCpu, cIdRegsOvrd, cIdRegsOvrd, cIdRegs);

                /* Load the values first without doing any validation. */
                paIdRegs = (PSUPARMSYSREGVAL)RTMemAllocZ(sizeof(paIdRegs[0]) * cIdRegsOvrd);
                AssertReturn(paIdRegs, VERR_NO_MEMORY);
                for (uint32_t i = 0; i < cIdRegsOvrd; i++)
                {
                    SSMR3GetU32(pSSM, &paIdRegs[i].idReg);
                    SSMR3GetU64(pSSM, &paIdRegs[i].uValue);
                    paIdRegs[i].fFlags = SUP_ARM_SYS_REG_VAL_F_FROM_SAVED_STATE;
                }
                uint32_t uTerm = 0;
                rc = SSMR3GetU32(pSSM, &uTerm);
                AssertRCBreak(rc);
                AssertLogRelMsgBreakStmt(uTerm == RT_MAKE_U32(idCpu, 0xfeed),
                                         ("uTerm=%#x\n", uTerm), rc = VERR_SSM_DATA_UNIT_FORMAT_CHANGED);

                /* The array shall be sorted and the values must be in the main table. */
                for (uint32_t i = 0, idPrev = 0 /* ASSUMES no zero ID register */; i < cIdRegsOvrd; i++)
                {
                    uint32_t const idReg = paIdRegs[i].idReg;
                    AssertLogRelMsgStmt(idReg > idPrev, ("#%u: idReg=%#x vs idPrev=%#x\n", i, idReg, idPrev),
                                        rc = VERR_SSM_DATA_UNIT_FORMAT_CHANGED);
                    AssertLogRelMsgStmt(cpumR3CpuIdLookupGuestIdReg(pVM, idReg) != NULL, ("#%u: idReg=%#x\n", i, idReg),
                                        rc = VERR_SSM_DATA_UNIT_FORMAT_CHANGED);
                    idPrev = idReg;
                }
                AssertRCBreak(rc);

                /* Sanitize and install. */
                rc = cpumR3LoadCpuIdInner(pVM, pVCpu, pSSM, paIdRegs, cIdRegsOvrd, true);
            }
        }
    }
    else
        rc = SSMR3SetLoadError(pSSM, VERR_SSM_DATA_UNIT_FORMAT_CHANGED, RT_SRC_POS,
                               N_("Loaded too many unknown ID registers: cSysRegs=%u cFound=%u RT_ELEMENTS(g_aSysIdRegs)=%u\n"),
                               cIdRegs, cFound, RT_ELEMENTS(g_aSysIdRegs));
    if (RT_FAILURE(rc))
        RTMemFree(paIdRegs);
    return rc;
}

