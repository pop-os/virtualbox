/* $Id: DrvAudioCommon.cpp $ */
/** @file
 * Intermedia audio driver, common routines. These are also used
 * in the drivers which are bound to Main, e.g. the VRDE or the
 * video audio recording drivers.
 */

/*
 * Copyright (C) 2006-2015 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 * --------------------------------------------------------------------
 *
 * This code is based on: audio_template.h from QEMU AUDIO subsystem.
 *
 * QEMU Audio subsystem header
 *
 * Copyright (c) 2005 Vassili Karpov (malc)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#define LOG_GROUP LOG_GROUP_DRV_AUDIO
#include <VBox/log.h>
#include <iprt/asm-math.h>
#include <iprt/assert.h>
#include <iprt/uuid.h>
#include <iprt/string.h>
#include <iprt/alloc.h>

#include <VBox/vmm/pdmdev.h>
#include <VBox/vmm/pdm.h>
#include <VBox/err.h>
#include <VBox/vmm/mm.h>

#include <ctype.h>
#include <stdlib.h>

#include "DrvAudio.h"
#include "AudioMixBuffer.h"

bool drvAudioPCMPropsAreEqual(PPDMAUDIOPCMPROPS pProps, PPDMAUDIOSTREAMCFG pCfg);

const char *drvAudioRecSourceToString(PDMAUDIORECSOURCE enmRecSource)
{
    switch (enmRecSource)
    {
        case PDMAUDIORECSOURCE_MIC:     return "Microphone In";
        case PDMAUDIORECSOURCE_CD:      return "CD";
        case PDMAUDIORECSOURCE_VIDEO:   return "Video";
        case PDMAUDIORECSOURCE_AUX:     return "AUX";
        case PDMAUDIORECSOURCE_LINE_IN: return "Line In";
        case PDMAUDIORECSOURCE_PHONE:   return "Phone";
        default:
            break;
    }

    AssertMsgFailed(("Bogus recording source %ld\n", enmRecSource));
    return "Unknown";
}

const char *drvAudioHlpFormatToString(PDMAUDIOFMT enmFormat)
{
    switch (enmFormat)
    {
        case AUD_FMT_U8:
            return "U8";

        case AUD_FMT_U16:
            return "U16";

        case AUD_FMT_U32:
            return "U32";

        case AUD_FMT_S8:
            return "S8";

        case AUD_FMT_S16:
            return "S16";

        case AUD_FMT_S32:
            return "S32";

        default:
            break;
    }

    AssertMsgFailed(("Bogus audio format %ld\n", enmFormat));
    return "Invalid";
}

PDMAUDIOFMT drvAudioHlpStringToFormat(const char *pszFormat)
{
    if (!RTStrICmp(pszFormat, "u8"))
        return AUD_FMT_U8;
    else if (!RTStrICmp(pszFormat, "u16"))
        return AUD_FMT_U16;
    else if (!RTStrICmp(pszFormat, "u32"))
        return AUD_FMT_U32;
    else if (!RTStrICmp(pszFormat, "s8"))
        return AUD_FMT_S8;
    else if (!RTStrICmp(pszFormat, "s16"))
        return AUD_FMT_S16;
    else if (!RTStrICmp(pszFormat, "s32"))
        return AUD_FMT_S32;

    AssertMsgFailed(("Bogus audio format \"%s\"\n", pszFormat));
    return AUD_FMT_INVALID;
}

/*********************************** In Stream Functions **********************************************/

void drvAudioGstInFreeRes(PPDMAUDIOGSTSTRMIN pGstStrmIn)
{
    AssertPtrReturnVoid(pGstStrmIn);

    if (pGstStrmIn->State.pszName)
    {
        RTStrFree(pGstStrmIn->State.pszName);
        pGstStrmIn->State.pszName = NULL;
    }

    AudioMixBufDestroy(&pGstStrmIn->MixBuf);
}

void drvAudioHstInFreeRes(PPDMAUDIOHSTSTRMIN pHstStrmIn)
{
    AssertPtrReturnVoid(pHstStrmIn);
    AudioMixBufDestroy(&pHstStrmIn->MixBuf);
}

void drvAudioGstOutFreeRes(PPDMAUDIOGSTSTRMOUT pGstStrmOut)
{
    if (!pGstStrmOut)
        return;

    if (pGstStrmOut->State.pszName)
    {
        RTStrFree(pGstStrmOut->State.pszName);
        pGstStrmOut->State.pszName = NULL;
    }

    AudioMixBufDestroy(&pGstStrmOut->MixBuf);
}

#if 0

/**
 * Finds the minimum number of not yet captured samples of all
 * attached guest input streams for a certain host input stream.
 *
 * @return  uint32_t            Minimum number of not yet captured samples.
 *                              UINT32_MAX if none found.
 * @param   pHstStrmIn          Host input stream to check for.
 */
inline uint32_t drvAudioHstInFindMinCaptured(PPDMAUDIOHSTSTRMIN pHstStrmIn)
{
    AssertPtrReturn(pHstStrmIn, 0);
    uint32_t cMinSamples = UINT32_MAX;

    PPDMAUDIOGSTSTRMIN pGstStrmIn;
    RTListForEach(&pHstStrmIn->lstGstStrmIn, pGstStrmIn, PDMAUDIOGSTSTRMIN, Node)
    {
        if (pGstStrmIn->State.fActive)
            cMinSamples = RT_MIN(cMinSamples, audioMixBufMixed(&pGstStrmIn->MixBuf));
    }

#ifdef DEBUG_andy
    LogFlowFunc(("cMinSamples=%RU32\n", cMinSamples));
#endif
    return cMinSamples;
}

uint32_t drvAudioHstInGetFree(PPDMAUDIOHSTSTRMIN pHstStrmIn)
{
    AssertPtrReturn(pHstStrmIn, 0);

    return audioMixBufSize(&pHstStrmIn->MixBuf) - drvAudioHstInGetLive(pHstStrmIn);
}

uint32_t drvAudioHstInGetLive(PPDMAUDIOHSTSTRMIN pHstStrmIn)
{
    AssertPtrReturn(pHstStrmIn, 0);

    uint32_t cMinSamplesCaptured = drvAudioHstInFindMinCaptured(pHstStrmIn);
    uint32_t cSamplesCaptured = audioMixBufMixed(&pHstStrmIn->MixBuf);

    Assert(cSamplesCaptured >= cMinSamplesCaptured);
    uint32_t cSamplesLive = cSamplesCaptured - cMinSamplesCaptured;
    Assert(cSamplesLive <= audioMixBufSize(&pHstStrmIn->MixBuf));

#ifdef DEBUG_andy
    LogFlowFunc(("cSamplesLive=%RU32\n", cSamplesLive));
#endif
    return cSamplesLive;
}
#endif

void drvAudioHstOutFreeRes(PPDMAUDIOHSTSTRMOUT pHstStrmOut)
{
    AssertPtrReturnVoid(pHstStrmOut);
    AudioMixBufDestroy(&pHstStrmOut->MixBuf);
}

#if 0
/**
 * Returns the number of live sample data (in bytes) of a certain
 * guest input stream.
 *
 * @return  uint32_t            Live sample data (in bytes), 0 if none.
 * @param   pGstStrmIn          Guest input stream to check for.
 */
uint32_t drvAudioGstInGetLiveBytes(PPDMAUDIOGSTSTRMIN pGstStrmIn)
{
    AssertPtrReturn(pGstStrmIn, 0);
    AssertPtrReturn(pGstStrmIn->pHstStrmIn, 0);

    Assert(pGstStrmIn->pHstStrmIn->cTotalSamplesCaptured >= pGstStrmIn->cTotalHostSamplesRead);
    uint32_t cSamplesLive = pGstStrmIn->pHstStrmIn->cTotalSamplesCaptured - pGstStrmIn->cTotalHostSamplesRead;
    if (!cSamplesLive)
        return 0;
    Assert(cSamplesLive <= pGstStrmIn->pHstStrmIn->cSamples);

    /** @todo Document / refactor this! */
    return (((int64_t) cSamplesLive << 32) / pGstStrmIn->State.uFreqRatio) << pGstStrmIn->Props.cShift;
}


/**
 * Returns the total number of unused sample data (in bytes) of a certain
 * guest output stream.
 *
 * @return  uint32_t            Number of unused sample data (in bytes), 0 if all used up.
 * @param   pGstStrmOut         Guest output stream to check for.
 */
uint32_t drvAudioGstOutGetFreeBytes(PPDMAUDIOGSTSTRMOUT pGstStrmOut)
{
    AssertPtrReturn(pGstStrmOut, 0);

    Assert(pGstStrmOut->cTotalSamplesWritten <= pGstStrmOut->pHstStrmOut->cSamples);
    uint32_t cSamplesFree =   pGstStrmOut->pHstStrmOut->cSamples
                            - pGstStrmOut->cTotalSamplesWritten;
    if (!cSamplesFree)
        return 0;

    /** @todo Document / refactor this! */
    return (((int64_t) cSamplesFree << 32) / pGstStrmOut->State.uFreqRatio) << pGstStrmOut->Props.cShift;
}
#endif

bool drvAudioPCMPropsAreEqual(PPDMAUDIOPCMPROPS pProps, PPDMAUDIOSTREAMCFG pCfg)
{
    int cBits = 8;
    bool fSigned = false;

    switch (pCfg->enmFormat)
    {
        case AUD_FMT_S8:
            fSigned = true;
            /* fall thru */
        case AUD_FMT_U8:
            break;

        case AUD_FMT_S16:
            fSigned = true;
            /* fall thru */
        case AUD_FMT_U16:
            cBits = 16;
            break;

        case AUD_FMT_S32:
            fSigned = true;
            /* fall thru */
        case AUD_FMT_U32:
            cBits = 32;
            break;

        default:
            AssertMsgFailed(("Unknown format %ld\n", pCfg->enmFormat));
            break;
    }

    bool fEqual =    pProps->uHz         == pCfg->uHz
                  && pProps->cChannels   == pCfg->cChannels
                  && pProps->fSigned     == fSigned
                  && pProps->cBits       == cBits
                  && pProps->fSwapEndian == !(pCfg->enmEndianness == PDMAUDIOHOSTENDIANNESS);

    LogFlowFunc(("fEqual=%RTbool\n", fEqual));
    return fEqual;
}

/**
 * Checks whether given PCM properties are valid or not.
 *
 * Returns @c true if properties are valid, @c false if not.
 * @param   pProps              PCM properties to check.
 *
 * @remarks Does *not* support surround (> 2 channels) yet! This is intentional, as
 *          we consider surround support as experimental / not enabled by default for now.
 */
bool DrvAudioHlpPCMPropsAreValid(const PPDMAUDIOPCMPROPS pProps)
{
    AssertPtrReturn(pProps, false);

    bool fValid = (   pProps->cChannels == 1
                   || pProps->cChannels == 2); /* Either stereo (2) or mono (1), per stream. */

    if (fValid)
    {
        switch (pProps->cBits)
        {
            case 8:
            case 16:
            /** @todo Do we need support for 24-bit samples? */
            case 32:
                break;
            default:
                fValid = false;
                break;
        }
    }

    if (!fValid)
        return false;

    fValid &= pProps->uHz > 0;
    fValid &= pProps->cShift == PDMAUDIOPCMPROPS_MAKE_SHIFT_PARMS(pProps->cBits, pProps->cChannels);

    fValid &= pProps->fSwapEndian == false; /** @todo Handling Big Endian audio data is not supported yet. */

    return fValid;
}

/**
 * Converts an audio stream configuration to matching PCM properties.
 *
 * @return  IPRT status code.
 * @param   pCfg                    Audio stream configuration to convert.
 * @param   pProps                  PCM properties to save result to.
 */
int DrvAudioStreamCfgToProps(PPDMAUDIOSTREAMCFG pCfg, PPDMAUDIOPCMPROPS pProps)
{
    AssertPtrReturn(pCfg,   VERR_INVALID_POINTER);
    AssertPtrReturn(pProps, VERR_INVALID_POINTER);

    int rc = VINF_SUCCESS;

    int cBits = 8, cShift = 0;
    bool fSigned = false;

    switch (pCfg->enmFormat)
    {
        case AUD_FMT_S8:
            fSigned = true;
            /* fall thru */
        case AUD_FMT_U8:
            break;

        case AUD_FMT_S16:
            fSigned = true;
            /* fall thru */
        case AUD_FMT_U16:
            cBits = 16;
            cShift = 1;
            break;

        case AUD_FMT_S32:
            fSigned = true;
            /* fall thru */
        case AUD_FMT_U32:
            cBits = 32;
            cShift = 2;
            break;

        default:
            AssertMsgFailed(("Unknown format %ld\n", pCfg->enmFormat));
            rc = VERR_NOT_SUPPORTED;
            break;
    }

    if (RT_SUCCESS(rc))
    {
        pProps->uHz         = pCfg->uHz;
        pProps->cBits       = cBits;
        pProps->fSigned     = fSigned;
        pProps->cChannels   = pCfg->cChannels;
        pProps->cShift      = (pCfg->cChannels == 2) + cShift;
        pProps->uAlign      = (1 << pProps->cShift) - 1;
        pProps->cbPerSec    = pProps->uHz << pProps->cShift;
        pProps->fSwapEndian = pCfg->enmEndianness != PDMAUDIOHOSTENDIANNESS;
    }

    return rc;
}

void drvAudioStreamCfgPrint(PPDMAUDIOSTREAMCFG pCfg)
{
    LogFlowFunc(("uHz=%RU32, cChannels=%RU8, enmFormat=",
                 pCfg->uHz, pCfg->cChannels));

    switch (pCfg->enmFormat)
    {
        case AUD_FMT_S8:
            LogFlow(("S8"));
            break;
        case AUD_FMT_U8:
            LogFlow(("U8"));
            break;
        case AUD_FMT_S16:
            LogFlow(("S16"));
            break;
        case AUD_FMT_U16:
            LogFlow(("U16"));
            break;
        case AUD_FMT_S32:
            LogFlow(("S32"));
            break;
        case AUD_FMT_U32:
            LogFlow(("U32"));
            break;
        default:
            LogFlow(("invalid(%d)", pCfg->enmFormat));
            break;
    }

    LogFlow((", endianness="));
    switch (pCfg->enmEndianness)
    {
        case PDMAUDIOENDIANNESS_LITTLE:
            LogFlow(("little\n"));
            break;
        case PDMAUDIOENDIANNESS_BIG:
            LogFlow(("big\n"));
            break;
        default:
            LogFlow(("invalid\n"));
            break;
    }
}

/**
* Sanitizes the file name component so that unsupported characters
* will be replaced by an underscore ("_").
*
* @return  IPRT status code.
* @param   pszPath             Path to sanitize.
* @param   cbPath              Size (in bytes) of path to sanitize.
*/
int DrvAudioHlpSanitizeFileName(char *pszPath, size_t cbPath)
{
    RT_NOREF(cbPath);
    int rc = VINF_SUCCESS;
#ifdef RT_OS_WINDOWS
    /* Filter out characters not allowed on Windows platforms, put in by
    RTTimeSpecToString(). */
    /** @todo Use something like RTPathSanitize() if available later some time. */
    static RTUNICP const s_uszValidRangePairs[] =
    {
        ' ', ' ',
        '(', ')',
        '-', '.',
        '0', '9',
        'A', 'Z',
        'a', 'z',
        '_', '_',
        0xa0, 0xd7af,
        '\0'
    };
    ssize_t cReplaced = RTStrPurgeComplementSet(pszPath, s_uszValidRangePairs, '_' /* Replacement */);
    if (cReplaced < 0)
        rc = VERR_INVALID_UTF8_ENCODING;
#else
    RT_NOREF(pszPath);
#endif
    return rc;
}

/**
* Constructs an unique file name, based on the given path and the audio file type.
*
* @returns IPRT status code.
* @param   pszFile             Where to store the constructed file name.
* @param   cchFile             Size (in characters) of the file name buffer.
* @param   pszPath             Base path to use.
* @param   pszName             A name for better identifying the file. Optional.
*/
int DrvAudioHlpGetFileName(char *pszFile, size_t cchFile, const char *pszPath, const char *pszName)
{
    AssertPtrReturn(pszFile, VERR_INVALID_POINTER);
    AssertReturn(cchFile, VERR_INVALID_PARAMETER);
    AssertPtrReturn(pszPath, VERR_INVALID_POINTER);
    int rc;

    do
    {
        char szFilePath[RTPATH_MAX];
        RTStrPrintf(szFilePath, sizeof(szFilePath), "%s", pszPath);

        /* Create it when necessary. */
        if (!RTDirExists(szFilePath))
        {
            rc = RTDirCreateFullPath(szFilePath, RTFS_UNIX_IRWXU);
            if (RT_FAILURE(rc))
                break;
        }

        /* The actually drop directory consist of the current time stamp and a
        * unique number when necessary. */
        char pszTime[64];
        RTTIMESPEC time;
        if (!RTTimeSpecToString(RTTimeNow(&time), pszTime, sizeof(pszTime)))
        {
            rc = VERR_BUFFER_OVERFLOW;
            break;
        }

        rc = DrvAudioHlpSanitizeFileName(pszTime, sizeof(pszTime));
        if (RT_FAILURE(rc))
            break;

        rc = RTPathAppend(szFilePath, sizeof(szFilePath), pszTime);
        if (RT_FAILURE(rc))
            break;

        if (pszName) /* Optional name given? */
        {
            rc = RTStrCat(szFilePath, sizeof(szFilePath), "-");
            if (RT_FAILURE(rc))
                break;

            rc = RTStrCat(szFilePath, sizeof(szFilePath), pszName);
            if (RT_FAILURE(rc))
                break;
        }

        rc = RTStrCat(szFilePath, sizeof(szFilePath), ".wav");

        if (RT_FAILURE(rc))
            break;

        RTStrPrintf(pszFile, cchFile, "%s", szFilePath);

    } while (0);

    LogFlowFuncLeaveRC(rc);
    return rc;
}
