/* $Id: VBoxCpuProfile.cpp $ */
/** @file
 * VBoxCpuProfile - For testing and poking at the CPU profile DB.
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
 * SPDX-License-Identifier: GPL-3.0-only
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#include <VBox/vmm/cpum.h>
#include <VBox/vmm/vmmr3vtable.h>

#include <iprt/errcore.h>
#include <iprt/getopt.h>
#include <iprt/initterm.h>
#include <iprt/ldr.h>
#include <iprt/message.h>
#include <iprt/mem.h>
#include <iprt/path.h>
#include <iprt/process.h>
#include <iprt/sort.h>
#include <iprt/string.h>
#include <iprt/stream.h>


/*********************************************************************************************************************************
*   Global Variables                                                                                                             *
*********************************************************************************************************************************/
static RTLDRMOD                                 g_hModVMM = NIL_RTLDRMOD;
static PCVMMR3VTABLE                            g_pVMM    = NULL;
static PFNVMMGETVTABLE                          g_pfnVMMR3GetVTable;
static PFNCPUMDBGETENTRIES                      g_pfnCPUMR3DbGetEntries;
static PFNCPUMDBGETENTRYBYINDEX                 g_pfnCPUMR3DbGetEntryByIndex;
static PFNCPUMDBGETENTRYBYNAME                  g_pfnCPUMR3DbGetEntryByName;
static PFNCPUMDBGETBESTENTRYBYNAME              g_pfnCPUMR3DbGetBestEntryByName;
static PFNCPUMDBGETBESTENTRYBYARM64MAINID       g_pfnCPUMR3DbGetBestEntryByArm64MainId;
static PFNCPUMCPUIDPRINTARMV8FEATURES           g_pfnCPUMR3CpuIdPrintArmV8Features;
static PFNCPUMCPUIDDETERMINEARMV8MICROARCHEX    g_pfnCPUMCpuIdDetermineArmV8MicroarchEx;
static PFNCPUMR3CPUIDINFOX86                    g_pfnCPUMR3CpuIdInfoX86;
static PFNCPUMR3CPUIDINFOARMV8                  g_pfnCPUMR3CpuIdInfoArmV8;
static DECLCALLBACKMEMBER(int, g_pfnCPUMCpuIdExplodeFeaturesX86,(PCCPUMCPUIDLEAF paLeaves, uint32_t cLeaves, CPUMFEATURESX86 *pFeatures));
static DECLCALLBACKMEMBER(int, g_pfnCPUMCpuIdExplodeFeaturesArmV8,(PCSUPARMSYSREGVAL paSysRegs, uint32_t cSysRegs, CPUMFEATURESARMV8 *pFeatures));
#if defined(RT_ARCH_AMD64)
static DECLCALLBACKMEMBER(int, g_pfnCPUMCpuIdCollectLeavesFromX86Host,(PCPUMCPUIDLEAF *ppaLeaves, uint32_t *pcLeaves));
typedef CPUMCPUIDINFOSTATEX86       CPUMCPUIDINFOSTATEHOST;
#elif defined(RT_ARCH_ARM64)
static DECLCALLBACKMEMBER(int, g_pfnCPUMCpuIdCollectIdSysRegsFromArmV8Host,(PSUPARMSYSREGVAL *ppaSysRegs, uint32_t *pcSysRegs));
typedef CPUMCPUIDINFOSTATEARMV8     CPUMCPUIDINFOSTATEHOST;
#endif


static const struct
{
    bool        fOptional;
    const char *pszSymbol;
    /** Note! Should've been 'PFNRT *ppfn', but clang 14.1 on macos complaints
     *        that "exception specifications are not allowed beyond a single
     *        level of indirection".  So, we're using uintptr_t here as a HACK. */
    uintptr_t  *ppfn;
} g_aImports[] =
{
    { false, VMMR3VTABLE_GETTER_NAME,                       (uintptr_t *)&g_pfnVMMR3GetVTable                           },
    { false, "CPUMR3DbGetEntries",                          (uintptr_t *)&g_pfnCPUMR3DbGetEntries                       },
    { false, "CPUMR3DbGetEntryByIndex",                     (uintptr_t *)&g_pfnCPUMR3DbGetEntryByIndex                  },
    { false, "CPUMR3DbGetEntryByName",                      (uintptr_t *)&g_pfnCPUMR3DbGetEntryByName                   },
    { false, "CPUMR3DbGetBestEntryByName",                  (uintptr_t *)&g_pfnCPUMR3DbGetBestEntryByName               },
    { true,  "CPUMR3DbGetBestEntryByArm64MainId",           (uintptr_t *)&g_pfnCPUMR3DbGetBestEntryByArm64MainId        },
    { true,  "CPUMR3CpuIdPrintArmV8Features",               (uintptr_t *)&g_pfnCPUMR3CpuIdPrintArmV8Features            },
    { true,  "CPUMCpuIdDetermineArmV8MicroarchEx",          (uintptr_t *)&g_pfnCPUMCpuIdDetermineArmV8MicroarchEx       },
    { true,  "CPUMR3CpuIdInfoX86",                          (uintptr_t *)&g_pfnCPUMR3CpuIdInfoX86                       },
    { true,  "CPUMR3CpuIdInfoArmV8",                        (uintptr_t *)&g_pfnCPUMR3CpuIdInfoArmV8                     },
    { true,  "CPUMCpuIdExplodeFeaturesX86",                 (uintptr_t *)&g_pfnCPUMCpuIdExplodeFeaturesX86              },
    { true,  "CPUMCpuIdExplodeFeaturesArmV8",               (uintptr_t *)&g_pfnCPUMCpuIdExplodeFeaturesArmV8            },
#if defined(RT_ARCH_AMD64)
    { true,  "CPUMCpuIdCollectLeavesFromX86Host",           (uintptr_t *)&g_pfnCPUMCpuIdCollectLeavesFromX86Host        },
#elif defined(RT_ARCH_ARM64)
    { true,  "CPUMCpuIdCollectIdSysRegsFromArmV8Host",      (uintptr_t *)&g_pfnCPUMCpuIdCollectIdSysRegsFromArmV8Host   },
#endif
};

static uint32_t g_cchStdOutWidth   = 80;
static unsigned g_cVerbosity       = 1;
static bool     g_fShowArmFeatures = false;


/** @interface_method_impl{DBGFINFOHLP,pfnPrintfV} */
static DECLCALLBACK(void) vboxCpuProfileHlp_PrintfV(PCDBGFINFOHLP pHlp, const char *pszFormat, va_list va)
{
    RT_NOREF_PV(pHlp);
    RTPrintfV(pszFormat, va);
}


/** @interface_method_impl{DBGFINFOHLP,pfnPrintf} */
static DECLCALLBACK(void) vboxCpuProfileHlp_Printf(PCDBGFINFOHLP pHlp, const char *pszFormat, ...)
{
    va_list va;
    va_start(va, pszFormat);
    vboxCpuProfileHlp_PrintfV(pHlp, pszFormat, va);
    va_end(va);
}


/** @interface_method_impl{DBGFINFOHLP,pfnGetOptError} */
static DECLCALLBACK(void) vboxCpuProfileHlp_GetOptError(PCDBGFINFOHLP pHlp, int rc, union RTGETOPTUNION *pValueUnion,
                                                        struct RTGETOPTSTATE *pState)
{
    RT_NOREF(pHlp, pState);
    RTGetOptPrintError(rc, pValueUnion);
}


/** For making info handler code output to stdout. */
static DBGFINFOHLP g_StdOutInfoHlp =
{
    vboxCpuProfileHlp_Printf,
    vboxCpuProfileHlp_PrintfV,
    vboxCpuProfileHlp_GetOptError
};


/** @callback_method_impl{FNRTSORTCMP} */
DECLCALLBACK(int) armrmRegValSortCmp(void const *pvElement1, void const *pvElement2, void *pvUser)
{
    RT_NOREF(pvUser);
    PCSUPARMSYSREGVAL const pElm1 = (PCSUPARMSYSREGVAL)pvElement1;
    PCSUPARMSYSREGVAL const pElm2 = (PCSUPARMSYSREGVAL)pvElement2;
    return pElm1->idReg < pElm2->idReg ? -1 : pElm1->idReg > pElm2->idReg ? 1 : 0;
}


#define DISPLAY_ENTRY_F_NOTHING     UINT32_C(0x00000000)
#define DISPLAY_ENTRY_F_INDEX       UINT32_C(0x00000001)
#define DISPLAY_ENTRY_F_SCORE       UINT32_C(0x00000002)
void displayEntry(PCCPUMDBENTRY pEntry, uint32_t uInfo, uint32_t fFlags, uint32_t iVerboseSub)
{
    if (fFlags & DISPLAY_ENTRY_F_INDEX)
    {
        if (g_cVerbosity >= 2 && uInfo > 0)
            RTPrintf("\n");
        RTPrintf("#%u: ", uInfo);
    }
    switch (pEntry->enmEntryType)
    {
        case CPUMDBENTRYTYPE_ARM:       RTPrintf("arm"); break;
        case CPUMDBENTRYTYPE_X86:       RTPrintf("x86"); break;
        default:                        RTPrintf("bogus-entry-type=%u", pEntry->enmEntryType); break;
    }
    RTPrintf(" - %s", pEntry->pszName);
    if (fFlags & DISPLAY_ENTRY_F_SCORE)
        RTPrintf(" - score %u%%", uInfo);
    RTPrintf("\n");
    if (g_cVerbosity >= 2)
    {
        RTPrintf("     enmVendor = %s\n", g_pVMM->pfnCPUMCpuVendorName(pEntry->enmVendor));
        RTPrintf("  enmMicroarch = %s\n", g_pVMM->pfnCPUMMicroarchName(pEntry->enmMicroarch));
        if (pEntry->fFlags)
            RTPrintf("        fFlags = %#010x\n", pEntry->fFlags);
        if (pEntry->enmEntryType == CPUMDBENTRYTYPE_X86)
        {
            //PCCPUMDBENTRYX86 const pEntryX86   = (PCCPUMDBENTRYX86)pEntry;
            /** @todo more detailed listing if -v is high enough...   */
        }
        else if (pEntry->enmEntryType == CPUMDBENTRYTYPE_ARM)
        {
            PCCPUMDBENTRYARM const pEntryArm   = (PCCPUMDBENTRYARM)pEntry;
            //RTPrintf("     cVariants = %u\n", pEntryArm->cVariants);
            for (unsigned iVar = 0; iVar < pEntryArm->cVariants; iVar++)
                RTPrintf("   Core Var #%u = %s\n", iVar, pEntryArm->aVariants[iVar].pszName);
        }
        /** @todo more detailed listing if -v is high enough...   */
    }

    /*
     * Display the CPU ID info for the entry.
     */
    if (   g_cVerbosity >= 2 + iVerboseSub
        || (   pEntry->enmEntryType == CPUMDBENTRYTYPE_ARM
            && g_cVerbosity >= 1
            && g_fShowArmFeatures) )
    {
        bool fSameAsHost = false;
        if (   pEntry->enmEntryType == CPUMDBENTRYTYPE_X86
            && g_pfnCPUMCpuIdExplodeFeaturesX86
            && g_pfnCPUMR3CpuIdInfoX86)
        {
            PCCPUMDBENTRYX86 const pEntryX86   = (PCCPUMDBENTRYX86)pEntry;
            CPUMFEATURESX86        FeaturesX86;
            int rc = g_pfnCPUMCpuIdExplodeFeaturesX86(pEntryX86->paCpuIdLeaves, pEntryX86->cCpuIdLeaves, &FeaturesX86);
            if (RT_SUCCESS(rc))
            {
                CPUMCPUIDINFOSTATEX86 InfoState =
                {
                    {
                        /* .pHlp        = */    &g_StdOutInfoHlp,
                        /* .iVerbosity  = */    g_cVerbosity - 2 - iVerboseSub,
                        /* .cchLabelMax = */    5,
                        /* .pszShort    = */    "Gst",
                        /* .pszLabel    = */    "Guest",
                        /* .cchLabel    = */    5,
                        /* .cchLabel2   = */    0,
                        /* .pszShort2   = */    fSameAsHost ? "Hst"  : NULL,
                        /* .pszLabel2   = */    fSameAsHost ? "Host" : NULL,
                    },
                    /* .pFeatures         = */  &FeaturesX86,
                    /* .paLeaves/IdRegs   = */  pEntryX86->paCpuIdLeaves,
                    /* .cLeaves/IdRegs    = */  pEntryX86->cCpuIdLeaves,
                    /* .cLeaves2/IdRegs2  = */  fSameAsHost ? pEntryX86->cCpuIdLeaves  : 0,
                    /* .paLeaves2/IdRegs2 = */  fSameAsHost ? pEntryX86->paCpuIdLeaves : NULL,
                };
                g_pfnCPUMR3CpuIdInfoX86(&InfoState);
            }
            else
                RTMsgError("CPUMCpuIdExplodeFeaturesX86 failed: %Rrc", rc);
        }
        else if (   pEntry->enmEntryType == CPUMDBENTRYTYPE_ARM
                 && g_pfnCPUMCpuIdExplodeFeaturesArmV8)
        {
            PCCPUMDBENTRYARM const pEntryArm   = (PCCPUMDBENTRYARM)pEntry;
            uint32_t               cSysRegs    = pEntryArm->cSysRegCmnVals + pEntryArm->aVariants[0].cSysRegVals;
            PSUPARMSYSREGVAL       paSysRegs   = (PSUPARMSYSREGVAL)RTMemAllocZ(sizeof(paSysRegs[0]) * cSysRegs);
            if (paSysRegs)
            {
                memcpy(paSysRegs,
                       pEntryArm->aVariants[0].paSysRegVals,
                       pEntryArm->aVariants[0].cSysRegVals * sizeof(paSysRegs[0]));
                memcpy(&paSysRegs[pEntryArm->aVariants[0].cSysRegVals],
                       pEntryArm->paSysRegCmnVals,
                       pEntryArm->cSysRegCmnVals * sizeof(paSysRegs[0]));
                RTSortShell(paSysRegs, cSysRegs, sizeof(paSysRegs[0]), armrmRegValSortCmp, NULL);

                CPUMFEATURESARMV8  FeaturesArm;
                int rc = g_pfnCPUMCpuIdExplodeFeaturesArmV8(paSysRegs, cSysRegs, &FeaturesArm);
                if (RT_SUCCESS(rc))
                {
                    if (   g_pfnCPUMR3CpuIdInfoArmV8
                        && g_cVerbosity >= 2 + iVerboseSub)
                    {
                        CPUMCPUIDINFOSTATEARMV8 InfoState =
                        {
                            {
                                /* .pHlp        = */    &g_StdOutInfoHlp,
                                /* .iVerbosity  = */    g_cVerbosity - 2 - iVerboseSub,
                                /* .cchLabelMax = */    5,
                                /* .pszShort    = */    "Gst",
                                /* .pszLabel    = */    "Guest",
                                /* .cchLabel    = */    5,
                                /* .cchLabel2   = */    0,
                                /* .pszShort2   = */    fSameAsHost ? "Hst"  : NULL,
                                /* .pszLabel2   = */    fSameAsHost ? "Host" : NULL,
                            },
                            /* .pFeatures         = */  &FeaturesArm,
                            /* .paLeaves/IdRegs   = */  paSysRegs,
                            /* .cLeaves/IdRegs    = */  cSysRegs,
                            /* .cLeaves2/IdRegs2  = */  fSameAsHost ? cSysRegs  : 0,
                            /* .paLeaves2/IdRegs2 = */  fSameAsHost ? paSysRegs : NULL,
                        };
                        g_pfnCPUMR3CpuIdInfoArmV8(&InfoState);
                    }

                    /* Display the feature list if sufficiently verbose or explicitly requested. */
                    if (   g_pfnCPUMR3CpuIdPrintArmV8Features
                        && (   g_fShowArmFeatures
                            || g_cVerbosity >= 4 + iVerboseSub) )
                        g_pfnCPUMR3CpuIdPrintArmV8Features(&g_StdOutInfoHlp, g_cchStdOutWidth, &FeaturesArm, "Guest", NULL, NULL);
                }
                else
                    RTMsgError("CPUMCpuIdExplodeFeaturesArmV8 failed: %Rrc", rc);
                RTMemFree(paSysRegs);
            }
            else
                RTMsgError("RTMemAllocZ failed");
        }
    }
}


/**
 * Handles the 'host' command.
 */
static RTEXITCODE cmdHost(const char *pszCmd)
{
    /*
     * Collect the CPU ID register values and explode the features.
     */
    uint32_t        cIdValues;

#if defined(RT_ARCH_AMD64)
    if (!g_pfnCPUMCpuIdCollectLeavesFromX86Host)
        return RTMsgErrorExitFailure("%s: CPUMCpuIdCollectLeavesFromX86Host missing from the current VMM", pszCmd);
    if (!g_pfnCPUMCpuIdExplodeFeaturesX86)
        return RTMsgErrorExitFailure("%s: CPUMCpuIdExplodeFeaturesX86 missing from the current VMM", pszCmd);
    if (!g_pfnCPUMR3CpuIdInfoX86)
        return RTMsgErrorExitFailure("%s: CPUMR3CpuIdInfoX86 missing from the current VMM", pszCmd);

    PCPUMCPUIDLEAF paIdValues;
    int rc = g_pfnCPUMCpuIdCollectLeavesFromX86Host(&paIdValues, &cIdValues);
    if (RT_FAILURE(rc))
        return RTMsgErrorExitFailure("%s: CPUMCpuIdCollectLeavesFromX86Host failed: %Rrc", pszCmd, rc);

    CPUMFEATURESX86 Features;
    rc = g_pfnCPUMCpuIdExplodeFeaturesX86(paIdValues, cIdValues, &Features);
    if (RT_FAILURE(rc))
        return RTMsgErrorExitFailure("%s: CPUMCpuIdExplodeFeaturesX86 failed: %Rrc", pszCmd, rc);

#elif defined(RT_ARCH_ARM64)
    if (!g_pfnCPUMCpuIdCollectIdSysRegsFromArmV8Host)
        return RTMsgErrorExitFailure("%s: CPUMCpuIdCollectIdSysRegsFromArmV8Host missing from the current VMM", pszCmd);
    if (!g_pfnCPUMCpuIdExplodeFeaturesArmV8)
        return RTMsgErrorExitFailure("%s: CPUMCpuIdExplodeFeaturesArmV8 missing from the current VMM", pszCmd);
    if (!g_pfnCPUMR3CpuIdInfoArmV8)
        return RTMsgErrorExitFailure("%s: CPUMR3CpuIdInfoArmV8 missing from the current VMM", pszCmd);

    PSUPARMSYSREGVAL paIdValues;
    int rc = g_pfnCPUMCpuIdCollectIdSysRegsFromArmV8Host(&paIdValues, &cIdValues);
    if (RT_FAILURE(rc))
        return RTMsgErrorExitFailure("%s: CPUMCpuIdCollectLeavesFromX86Host failed: %Rrc", pszCmd, rc);

    CPUMFEATURESARMV8 Features;
    rc = g_pfnCPUMCpuIdExplodeFeaturesArmV8(paIdValues, cIdValues, &Features);
    if (RT_FAILURE(rc))
        return RTMsgErrorExitFailure("%s: CPUMCpuIdExplodeFeaturesArmV8 failed: %Rrc", pszCmd, rc);

#else
# error "port me"
#endif

    /*
     * Print the info.
     */
    CPUMCPUIDINFOSTATEHOST InfoState =
    {
        {
            /* .pHlp        = */    &g_StdOutInfoHlp,
            /* .iVerbosity  = */    g_cVerbosity,
            /* .cchLabelMax = */    4,
            /* .pszShort    = */    "Hst",
            /* .pszLabel    = */    "Host",
            /* .cchLabel    = */    4,
            /* .cchLabel2   = */    0,
            /* .pszShort2   = */    NULL,
            /* .pszLabel2   = */    NULL,
        },
        /* .pFeatures         = */  &Features,
        /* .paLeaves/IdRegs   = */  paIdValues,
        /* .cLeaves/IdRegs    = */  cIdValues,
        /* .cLeaves2/IdRegs2  = */  0,
        /* .paLeaves2/IdRegs2 = */  NULL,
    };

#if defined(RT_ARCH_AMD64)
    g_pfnCPUMR3CpuIdInfoX86(&InfoState);
#elif defined(RT_ARCH_ARM64)
    g_pfnCPUMR3CpuIdInfoArmV8(&InfoState);
#else
# error "port me"
#endif

    RTMemFree(paIdValues);
    return RTEXITCODE_SUCCESS;
}


static RTEXITCODE cmdHelp(void)
{
    RTStrmWrappedPrintf(g_pStdOut, RTSTRMWRAPPED_F_HANGING_INDENT | (11 << RTSTRMWRAPPED_F_HANGING_INDENT_SHIFT),
                        "Usage: %s [options] [cmd [args] [options] [cmd2 [args] ...]]\n", RTProcShortName());
    RTStrmWrappedPrintf(g_pStdOut, 0,
                        "\n"
                        "This is a tool for testing CPU ID register/leaves info items and applying these to the CPU profiles.\n"
                        "\n"
                        "Options:\n");
    RTStrmWrappedPrintf(g_pStdOut, RTSTRMWRAPPED_F_HANGING_INDENT | (4 << RTSTRMWRAPPED_F_HANGING_INDENT_SHIFT),
                        "  --vmm[-path]=<name|path>, --path=<name|path>\n"
                        "    Load the given VMM module. Anything from a filename w/o to suffix to a full path can be given.\n"
                        "  --verbose, -v\n"
                        "    Increases the output verbosity.\n"
                        "  --quiet, -q\n"
                        "    Resets the verbosity to the absolute minimal.\n"
                        "\n"
                        "Commands:\n");
    RTStrmWrappedPrintf(g_pStdOut, RTSTRMWRAPPED_F_HANGING_INDENT | (6 << RTSTRMWRAPPED_F_HANGING_INDENT_SHIFT),
                        "  - list\n"
                        "      Lists all the profiles in the current VMM module.\n"
                        "  - best-by-name <name>\n"
                        "  - best-arm-by-name <name>\n"
                        "  - best-x86-by-name <name>\n"
                        "      Finds the best profile match for the given name.  The 'arm' and 'x86' variants limits the "
                        "selection to the given architectures.\n"
                        "  - best-by-midr [hexvalue]\n"
                        "      Finds the best profile match for the given main ID value. Implies 'arm' architecture.\n"
                        "  - host\n"
                        "      Display the CPU ID registers/leaves for the host.\n"
                        "  - help\n"
                        "      Display this help info.\n"
                        );
    return RTEXITCODE_SUCCESS;
}


static void echoCommand(const char *pszFormat, ...)
{
    if (g_cVerbosity > 0)
    {
        va_list va;
        va_start(va, pszFormat);
        RTPrintf("cmd> %N\n", pszFormat, &va);
        va_end(va);
    }
}


int main(int argc, char **argv)
{
    int rc = RTR3InitExe(argc, &argv, 0);
    if (RT_FAILURE(rc))
        return RTMsgInitFailure(rc);


    /*
     * Initialize globals.
     */
    rc = RTStrmQueryTerminalWidth(g_pStdOut, &g_cchStdOutWidth);
    if (RT_FAILURE(rc))
        g_cchStdOutWidth = 80;

    /*
     * Parse parameters.
     */
    static const RTGETOPTDEF s_aOptions[] =
    {
        { "--path",                 'p', RTGETOPT_REQ_STRING },
        { "--vmm",                  'p', RTGETOPT_REQ_STRING },
        { "--vmm-path",             'p', RTGETOPT_REQ_STRING },
        { "--quiet",                'q', RTGETOPT_REQ_NOTHING },
        { "--verbose",              'v', RTGETOPT_REQ_NOTHING },

        { "--arm-feat",             'a', RTGETOPT_REQ_NOTHING },
        { "--arm-feats",            'a', RTGETOPT_REQ_NOTHING },
        { "--arm-features",         'a', RTGETOPT_REQ_NOTHING },
        { "--show-arm-features",    'a', RTGETOPT_REQ_NOTHING },

        { "--no-arm-feat",          'A', RTGETOPT_REQ_NOTHING },
        { "--no-arm-feats",         'A', RTGETOPT_REQ_NOTHING },
        { "--no-arm-features",      'A', RTGETOPT_REQ_NOTHING },
        { "--no-show-arm-features", 'A', RTGETOPT_REQ_NOTHING },
    };

    char            szPath[RTPATH_MAX];
    RTERRINFOSTATIC ErrInfo;

    RTEXITCODE      rcExit = RTEXITCODE_SUCCESS;
    int             chOpt;
    RTGETOPTUNION   ValueUnion;
    RTGETOPTSTATE   GetState;
    rc = RTGetOptInit(&GetState, argc, argv, s_aOptions, RT_ELEMENTS(s_aOptions), 1, 0);
    AssertRC(rc);
    while ((chOpt = RTGetOpt(&GetState, &ValueUnion)))
    {
        switch (chOpt)
        {
            case 'p':
            {
                if (g_hModVMM != NIL_RTLDRMOD)
                    RTLdrClose(g_hModVMM);
                const char *pszPath = ValueUnion.psz;
                if (!*pszPath)
                    pszPath = "VBoxVMM";
                if (!RTPathHasPath(pszPath))
                {
                    rc = RTPathExecDir(szPath, sizeof(szPath));
                    if (RT_FAILURE(rc))
                        return RTMsgErrorExitFailure("RTPathExecDir failed: %Rrc", rc);
                    RTPathStripTrailingSlash(szPath);
                    RTPathStripFilename(szPath);
                    rc = RTPathAppend(szPath, sizeof(szPath), pszPath);
                    if (RT_FAILURE(rc))
                        return RTMsgErrorExitFailure("RTPathAppend failed: %Rrc", rc);
                    pszPath = szPath;
                }
                if (!RTPathHasSuffix(pszPath))
                {
                    if (pszPath != szPath)
                    {
                        rc = RTStrCopy(szPath, sizeof(szPath), pszPath);
                        if (RT_FAILURE(rc))
                            return RTMsgErrorExitFailure("VMM path is too long or smth: %Rrc - %s", rc, pszPath);
                        pszPath = szPath;
                    }
                    rc = RTStrCat(szPath, sizeof(szPath), RTLdrGetSuff());
                    if (RT_FAILURE(rc))
                        return RTMsgErrorExitFailure("VMM path is too long: %Rrc", rc);
                }

                rc = RTLdrLoadEx(pszPath, &g_hModVMM, RTLDRLOAD_FLAGS_LOCAL, RTErrInfoInitStatic(&ErrInfo));
                if (RT_FAILURE(rc))
                    return RTMsgErrorExitFailure("RTLdrLoadEx failed on '%s': %Rrc%-RTeim", pszPath, rc, &ErrInfo.Core);
                for (unsigned i = 0; i < RT_ELEMENTS(g_aImports); i++)
                {
                    void *pv = NULL;
                    rc = RTLdrGetSymbol(g_hModVMM, g_aImports[i].pszSymbol, &pv);
                    if (RT_FAILURE(rc))
                    {
                        if (!g_aImports[i].fOptional)
                            return RTMsgErrorExitFailure("Unable to resolve %s in %s: %Rrc", g_aImports[i].pszSymbol, pszPath, rc);
                        pv = NULL;
                    }
                    *g_aImports[i].ppfn = (uintptr_t)pv;
                }
                g_pVMM = g_pfnVMMR3GetVTable();
                if (!RT_VALID_PTR(g_pVMM))
                    return RTMsgErrorExitFailure("VMMR3GetVTable in %s returns a bogus pointer: %p", pszPath, g_pVMM);
                if (!VMMR3VTABLE_IS_COMPATIBLE(g_pVMM->uMagicVersion))
                    return RTMsgErrorExitFailure("Incompatible VMM '%s': magic+ver is %#RX64, expected something compatible with %#RX64",
                                                 pszPath, g_pVMM->uMagicVersion, VMMR3VTABLE_MAGIC_VERSION);
                RTMsgInfo("Loaded '%s' - vtable v%u.%u, target %u (%s), description '%s'.\n",
                          pszPath,
                          (unsigned)((g_pVMM->uMagicVersion >> 48) & 0xffff), (unsigned)((g_pVMM->uMagicVersion >> 32) & 0xffff),
                          g_pVMM->fFlags & VMMR3VTABLE_F_TARGET_MASK,
                          (g_pVMM->fFlags & VMMR3VTABLE_F_TARGET_MASK) == VMMR3VTABLE_F_TARGET_X86 ? "X86"
                          : (g_pVMM->fFlags & VMMR3VTABLE_F_TARGET_MASK) == VMMR3VTABLE_F_TARGET_ARMV8 ? "ARMv8" : "unknown",
                          g_pVMM->pszDescription);
                break;
            }

            case 'q':
                g_cVerbosity = 0;
                break;

            case 'v':
                g_cVerbosity += 1;
                break;

            case 'a':
                g_fShowArmFeatures = true;
                break;

            case 'A':
                g_fShowArmFeatures = false;
                break;

            case VINF_GETOPT_NOT_OPTION:
            {
                if (g_hModVMM == NIL_RTLDRMOD)
                    return RTMsgErrorExitFailure("No VMM module has been specified!");

                const char * const pszCmd = ValueUnion.psz;
                if (strcmp(pszCmd, "list") == 0)
                {
                    echoCommand("%s", pszCmd);

                    uint32_t const cEntries = g_pfnCPUMR3DbGetEntries();
                    for (uint32_t i = 0; i < cEntries; i++)
                        displayEntry(g_pfnCPUMR3DbGetEntryByIndex(i), i, DISPLAY_ENTRY_F_INDEX, 1);
                }
                else if (   strcmp(pszCmd, "best-by-name") == 0
                         || strcmp(pszCmd, "best-arm-by-name") == 0
                         || strcmp(pszCmd, "best-x86-by-name") == 0)
                {
                    CPUMDBENTRYTYPE const enmEntryType = strstr(pszCmd, "arm") != NULL ? CPUMDBENTRYTYPE_ARM
                                                       : strstr(pszCmd, "x86") != NULL ? CPUMDBENTRYTYPE_X86
                                                       :                                 CPUMDBENTRYTYPE_INVALID;
                    rc = RTGetOptFetchValue(&GetState, &ValueUnion, RTGETOPT_REQ_STRING);
                    if (RT_FAILURE(rc))
                        return RTMsgSyntax("The '%s' command requires a name string.", pszCmd);
                    echoCommand("%s '%s'", pszCmd, ValueUnion.psz);

                    uint32_t      uScore = 0;
                    PCCPUMDBENTRY pEntry = g_pfnCPUMR3DbGetBestEntryByName(ValueUnion.psz, enmEntryType, &uScore);
                    if (pEntry)
                        displayEntry(pEntry, uScore, DISPLAY_ENTRY_F_SCORE, 0);
                    else
                        rcExit = RTMsgErrorExitFailure("%s: No match for '%s'", pszCmd, ValueUnion.psz);
                }
                else if (strcmp(pszCmd, "best-by-midr") == 0)
                {
                    rc = RTGetOptFetchValue(&GetState, &ValueUnion, RTGETOPT_REQ_UINT64 | RTGETOPT_FLAG_HEX);
                    if (RT_FAILURE(rc))
                        return RTMsgSyntax("The '%s' command requires a MIDR_EL1 value (64-bit, defaults to hex).", pszCmd);
                    echoCommand("%s %#RX64", pszCmd, ValueUnion.u64);

                    if (g_pfnCPUMR3DbGetBestEntryByArm64MainId)
                    {
                        uint32_t         uScore = 0;
                        PCCPUMDBENTRYARM pEntry = g_pfnCPUMR3DbGetBestEntryByArm64MainId(ValueUnion.u64, &uScore);
                        if (pEntry)
                            displayEntry(&pEntry->Core, uScore, DISPLAY_ENTRY_F_SCORE, 0);
                        else
                            rcExit = RTMsgErrorExitFailure("%s: No match for midr %#RX64", pszCmd, ValueUnion.u64);
                    }
                    else
                        rcExit = RTMsgErrorExitFailure("%s: CPUMR3DbGetBestEntryByArm64MainId missing from the current VMM",
                                                       pszCmd);
                }
                else if (strcmp(pszCmd, "host") == 0)
                {
                    echoCommand("%s", pszCmd, ValueUnion.u64);
                    rcExit = cmdHost(pszCmd);
                }
                else if (strcmp(pszCmd, "help") == 0)
                    return cmdHelp();
                else
                    return RTMsgSyntax("Unknown command: %s", pszCmd);
                break;
            }

            default:
                return RTGetOptPrintError(chOpt, &ValueUnion);
        }
    }

    return rcExit;
}

