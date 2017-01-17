/* $Id: DrvHostCoreAudio.cpp $ */
/** @file
 * VBox audio devices: Mac OS X CoreAudio audio driver.
 */

/*
 * Copyright (C) 2010-2016 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */
#define LOG_GROUP LOG_GROUP_DRV_HOST_AUDIO
#include <VBox/log.h>

#include "DrvAudio.h"
#include "AudioMixBuffer.h"

#include "VBoxDD.h"

#include <iprt/asm.h>
#include <iprt/cdefs.h>
#include <iprt/circbuf.h>
#include <iprt/mem.h>

#include <iprt/uuid.h>

#include <CoreAudio/CoreAudio.h>
#include <CoreServices/CoreServices.h>
#include <AudioToolbox/AudioConverter.h>
#include <AudioToolbox/AudioToolbox.h>

/* Audio Queue buffer configuration. */
#define AQ_BUF_COUNT    32      /* Number of buffers. */
#define AQ_BUF_SIZE     512     /* Size of each buffer in bytes. */
#define AQ_BUF_TOTAL    (AQ_BUF_COUNT * AQ_BUF_SIZE)
#define AQ_BUF_SAMPLES  (AQ_BUF_TOTAL / 4)  /* Hardcoded 4 bytes per sample! */

#if 0
# include <iprt/file.h>
# define DEBUG_DUMP_PCM_DATA
# ifdef RT_OS_WINDOWS
#  define DEBUG_DUMP_PCM_DATA_PATH "c:\\temp\\"
# else
#  define DEBUG_DUMP_PCM_DATA_PATH "/tmp/"
# endif
#endif

/* Enables utilizing the Core Audio converter unit for converting
 * input / output from/to our requested formats. That might be more
 * performant than using our own routines later down the road. */
/** @todo Needs more investigation and testing first before enabling. */
//# define VBOX_WITH_AUDIO_CA_CONVERTER

#ifdef DEBUG_andy
# undef  DEBUG_DUMP_PCM_DATA_PATH
# define DEBUG_DUMP_PCM_DATA_PATH "/Users/anloeffl/Documents/"
# undef  VBOX_WITH_AUDIO_CA_CONVERTER
#endif

/* TODO:
 * - Maybe make sure the threads are immediately stopped if playing/recording stops.
 */

/*
 * Most of this is based on:
 * http://developer.apple.com/mac/library/technotes/tn2004/tn2097.html
 * http://developer.apple.com/mac/library/technotes/tn2002/tn2091.html
 * http://developer.apple.com/mac/library/qa/qa2007/qa1533.html
 * http://developer.apple.com/mac/library/qa/qa2001/qa1317.html
 */

/**
 * Host Coreaudio driver instance data.
 * @implements PDMIAUDIOCONNECTOR
 */
typedef struct DRVHOSTCOREAUDIO
{
    /** Pointer to the driver instance structure. */
    PPDMDRVINS    pDrvIns;
    /** Pointer to host audio interface. */
    PDMIHOSTAUDIO IHostAudio;
} DRVHOSTCOREAUDIO, *PDRVHOSTCOREAUDIO;

/*******************************************************************************
 *
 * Helper function section
 *
 ******************************************************************************/

static void drvHostCoreAudioPrintASBD(const char *pszDesc, const AudioStreamBasicDescription *pASBD)
{
    char pszSampleRate[32];
    LogRel2(("CoreAudio: %s description:\n", pszDesc));
    LogRel2(("CoreAudio:\tFormat ID: %RU32 (%c%c%c%c)\n", pASBD->mFormatID,
             RT_BYTE4(pASBD->mFormatID), RT_BYTE3(pASBD->mFormatID),
             RT_BYTE2(pASBD->mFormatID), RT_BYTE1(pASBD->mFormatID)));
    LogRel2(("CoreAudio:\tFlags: %RU32", pASBD->mFormatFlags));
    if (pASBD->mFormatFlags & kAudioFormatFlagIsFloat)
        LogRel2((" Float"));
    if (pASBD->mFormatFlags & kAudioFormatFlagIsBigEndian)
        LogRel2((" BigEndian"));
    if (pASBD->mFormatFlags & kAudioFormatFlagIsSignedInteger)
        LogRel2((" SignedInteger"));
    if (pASBD->mFormatFlags & kAudioFormatFlagIsPacked)
        LogRel2((" Packed"));
    if (pASBD->mFormatFlags & kAudioFormatFlagIsAlignedHigh)
        LogRel2((" AlignedHigh"));
    if (pASBD->mFormatFlags & kAudioFormatFlagIsNonInterleaved)
        LogRel2((" NonInterleaved"));
    if (pASBD->mFormatFlags & kAudioFormatFlagIsNonMixable)
        LogRel2((" NonMixable"));
    if (pASBD->mFormatFlags & kAudioFormatFlagsAreAllClear)
        LogRel2((" AllClear"));
    LogRel2(("\n"));
    snprintf(pszSampleRate, 32, "%.2f", (float)pASBD->mSampleRate); /** @todo r=andy Use RTStrPrint*. */
    LogRel2(("CoreAudio:\tSampleRate      : %s\n", pszSampleRate));
    LogRel2(("CoreAudio:\tChannelsPerFrame: %RU32\n", pASBD->mChannelsPerFrame));
    LogRel2(("CoreAudio:\tFramesPerPacket : %RU32\n", pASBD->mFramesPerPacket));
    LogRel2(("CoreAudio:\tBitsPerChannel  : %RU32\n", pASBD->mBitsPerChannel));
    LogRel2(("CoreAudio:\tBytesPerFrame   : %RU32\n", pASBD->mBytesPerFrame));
    LogRel2(("CoreAudio:\tBytesPerPacket  : %RU32\n", pASBD->mBytesPerPacket));
}

static int drvHostCoreAudioPCMPropsToASBD(PPDMPCMPROPS pPCMProps, AudioStreamBasicDescription *pASBD)
{
    AssertPtrReturn(pPCMProps, VERR_INVALID_PARAMETER);
    AssertPtrReturn(pASBD,     VERR_INVALID_PARAMETER);

    RT_BZERO(pASBD, sizeof(AudioStreamBasicDescription));

    pASBD->mFormatID         = kAudioFormatLinearPCM;
    pASBD->mFormatFlags      = kAudioFormatFlagIsPacked;
    pASBD->mFramesPerPacket  = 1; /* For uncompressed audio, set this to 1. */
    pASBD->mSampleRate       = (Float64)pPCMProps->uHz;
    pASBD->mChannelsPerFrame = pPCMProps->cChannels;
    pASBD->mBitsPerChannel   = pPCMProps->cBits;
    if (pPCMProps->fSigned)
        pASBD->mFormatFlags |= kAudioFormatFlagIsSignedInteger;
    pASBD->mBytesPerFrame    = pASBD->mChannelsPerFrame * (pASBD->mBitsPerChannel / 8);
    pASBD->mBytesPerPacket   = pASBD->mFramesPerPacket * pASBD->mBytesPerFrame;

    return VINF_SUCCESS;
}

static int drvHostCoreAudioStreamCfgToASBD(PPDMAUDIOSTREAMCFG pCfg, AudioStreamBasicDescription *pASBD)
{
    AssertPtrReturn(pCfg,  VERR_INVALID_PARAMETER);
    AssertPtrReturn(pASBD, VERR_INVALID_PARAMETER);

    PDMPCMPROPS Props;
    int rc = DrvAudioStreamCfgToProps(pCfg, &Props);
    if (RT_SUCCESS(rc))
        rc = drvHostCoreAudioPCMPropsToASBD(&Props, pASBD);

    return rc;
}

static int drvHostCoreAudioASBDToStreamCfg(AudioStreamBasicDescription *pASBD, PPDMAUDIOSTREAMCFG pCfg)
{
    AssertPtrReturn(pASBD, VERR_INVALID_PARAMETER);
    AssertPtrReturn(pCfg,  VERR_INVALID_PARAMETER);

    pCfg->cChannels     = pASBD->mChannelsPerFrame;
    pCfg->uHz           = (uint32_t)pASBD->mSampleRate;
    pCfg->enmEndianness = PDMAUDIOENDIANNESS_LITTLE;

    int rc = VINF_SUCCESS;

    if (pASBD->mFormatFlags & kAudioFormatFlagIsSignedInteger)
    {
        switch (pASBD->mBitsPerChannel)
        {
            case 8:  pCfg->enmFormat = AUD_FMT_S8;  break;
            case 16: pCfg->enmFormat = AUD_FMT_S16; break;
            case 32: pCfg->enmFormat = AUD_FMT_S32; break;
            default: rc = VERR_NOT_SUPPORTED;       break;
        }
    }
    else
    {
        switch (pASBD->mBitsPerChannel)
        {
            case 8:  pCfg->enmFormat = AUD_FMT_U8;  break;
            case 16: pCfg->enmFormat = AUD_FMT_U16; break;
            case 32: pCfg->enmFormat = AUD_FMT_U32; break;
            default: rc = VERR_NOT_SUPPORTED;       break;
        }
    }

    AssertRC(rc);
    return rc;
}

#if 0 // unused
static AudioDeviceID drvHostCoreAudioDeviceUIDtoID(const char* pszUID)
{
    /* Create a CFString out of our CString. */
    CFStringRef strUID = CFStringCreateWithCString(NULL, pszUID, kCFStringEncodingMacRoman);

    /* Fill the translation structure. */
    AudioDeviceID deviceID;

    AudioValueTranslation translation;
    translation.mInputData      = &strUID;
    translation.mInputDataSize  = sizeof(CFStringRef);
    translation.mOutputData     = &deviceID;
    translation.mOutputDataSize = sizeof(AudioDeviceID);

    /* Fetch the translation from the UID to the device ID. */
    AudioObjectPropertyAddress propAdr = { kAudioHardwarePropertyDeviceForUID, kAudioObjectPropertyScopeGlobal,
                                           kAudioObjectPropertyElementMaster };

    UInt32 uSize = sizeof(AudioValueTranslation);
    OSStatus err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propAdr, 0, NULL, &uSize, &translation);

    /* Release the temporary CFString */
    CFRelease(strUID);

    if (RT_LIKELY(err == noErr))
        return deviceID;

    /* Return the unknown device on error. */
    return kAudioDeviceUnknown;
}
#endif

/*******************************************************************************
 *
 * Global structures section
 *
 ******************************************************************************/

/* Initialization status indicator used for device (re)initialization. */
#define CA_STATUS_UNINIT    UINT32_C(0) /* The device is uninitialized */
#define CA_STATUS_IN_INIT   UINT32_C(1) /* The device is currently initializing */
#define CA_STATUS_INIT      UINT32_C(2) /* The device is initialized */
#define CA_STATUS_IN_UNINIT UINT32_C(3) /* The device is currently uninitializing */
#define CA_STATUS_REINIT    UINT32_C(4) /* The device has to be reinitialized */

#ifdef VBOX_WITH_AUDIO_CA_CONVERTER
/* Error code which indicates "End of data" */
static const OSStatus caConverterEOFDErr = 0x656F6664; /* 'eofd' */
#endif

/* Prototypes needed for COREAUDIOSTREAMCBCTX. */
struct COREAUDIOSTREAMIN;
typedef struct COREAUDIOSTREAMIN *PCOREAUDIOSTREAMIN;
struct COREAUDIOSTREAMOUT;
typedef struct COREAUDIOSTREAMOUT *PCOREAUDIOSTREAMOUT;

typedef struct COREAUDIOSTREAM
{
    /** The stream's direction. */
    PDMAUDIODIR                 enmDir;
    union
    {
        /** Pointer to self, if it's an input stream. */
        PCOREAUDIOSTREAMIN      pIn;
        /** Pointer to self, if it's an output stream. */
        PCOREAUDIOSTREAMOUT     pOut;
        /** @todo Add attributes here as soon as COREAUDIOSTREAMIN / COREAUDIOSTREAMOUT are unified. */
    };
    /** The stream's thread handle for maintaining the audio queue. */
    RTTHREAD                    hThread;
    /** Flag indicating to start a stream's data processing. */
    bool                        fRun;
    /** Whether the stream is in a running (active) state or not.
     *  For playback streams this means that audio data can be (or is being) played,
     *  for capturing streams this means that audio data is being captured (if available). */
    bool                        fIsRunning;
    /** Thread shutdown indicator. */
    bool                        fShutdown;
    /** Critical section for serializing access between thread + callbacks. */
    RTCRITSECT                  CritSect;
    /** The actual audio queue being used. */
    AudioQueueRef               audioQueue;
    /** The audio buffers which are used with the above audio queue. */
    AudioQueueBufferRef         audioBuffer[AQ_BUF_COUNT];
    /** The acquired (final) audio format for this stream. */
    AudioStreamBasicDescription asbdStream;
    /** The device' UUID. */
    CFStringRef                 UUID;
    /** An internal ring buffer for transferring data from/to the rendering callbacks. */
    PRTCIRCBUF                  pCircBuf;
} COREAUDIOSTREAM, *PCOREAUDIOSTREAM;

/**
 * Simple structure for maintaining a stream's callback context.
 */
typedef struct COREAUDIOSTREAMCBCTX
{
    /** Pointer to driver instance. */
    PDRVHOSTCOREAUDIO pThis;
    /** Pointer to the stream being handled. Can be NULL if not used. */
    PCOREAUDIOSTREAM  pStream;
} COREAUDIOSTREAMCBCTX, *PCOREAUDIOSTREAMCBCTX;

/**
 * Structure for keeping a conversion callback context.
 * This is needed when using an audio converter during input/output processing.
 */
typedef struct COREAUDIOCONVCBCTX
{
    /** Pointer to stream context this converter callback context
     *  is bound to. */
    /** @todo Remove this as soon as we have unified input/output streams in this backend. */
    PCOREAUDIOSTREAMCBCTX        pStreamCtx;
    /** Source stream description. */
    AudioStreamBasicDescription  asbdSrc;
    /** Destination stream description. */
    AudioStreamBasicDescription  asbdDst;
    /** Pointer to native buffer list used for rendering the source audio data into. */
    AudioBufferList             *pBufLstSrc;
    /** Total packet conversion count. */
    UInt32                       uPacketCnt;
    /** Current packet conversion index. */
    UInt32                       uPacketIdx;
    /** Error count, for limiting the logging. */
    UInt32                       cErrors;
} COREAUDIOCONVCBCTX, *PCOREAUDIOCONVCBCTX;

/** @todo Unify COREAUDIOSTREAMOUT / COREAUDIOSTREAMIN. */
typedef struct COREAUDIOSTREAMOUT
{
    /** Host output stream.
     *  Note: Always must come first in this structure! */
    PDMAUDIOHSTSTRMOUT          streamOut;
    /** The audio device ID of the currently used device. */
    AudioDeviceID               deviceID;
    /** A ring buffer for transferring data to the playback thread. */
    PRTCIRCBUF                  pCircBuf;
    /** Initialization status tracker. Used when some of the device parameters
     *  or the device itself is changed during the runtime. */
    volatile uint32_t           status;
    /** Flag whether the "default device changed" listener was registered. */
    bool                        fDefDevChgListReg;
    /** Flag whether the "device state changed" listener was registered. */
    bool                        fDevStateChgListReg;
    /** Unified attribtues. */
    /** @todo Remove this as soon as we have unified input/output streams in this backend. */
    COREAUDIOSTREAM             Stream;
    /** Callback context for this stream for handing this stream in an CoreAudio callback.
     ** @todo Remove this as soon as we have unified input/output streams in this backend. */
    COREAUDIOSTREAMCBCTX        cbCtx;
} COREAUDIOSTREAMOUT, *PCOREAUDIOSTREAMOUT;

typedef struct COREAUDIOSTREAMIN
{
    /** Host input stream.
     *  Note: Always must come first in this structure! */
    PDMAUDIOHSTSTRMIN           streamIn;
    /** The audio device ID of the currently used device. */
    AudioDeviceID               deviceID;
    /** A ring buffer for transferring data from the recording thread. */
    PRTCIRCBUF                  pCircBuf;
#ifdef VBOX_WITH_AUDIO_CA_CONVERTER
    /** The audio converter if necessary. NULL if no converter is being used. */
    AudioConverterRef           ConverterRef;
    /** Callback context for the audio converter. */
    COREAUDIOCONVCBCTX          convCbCtx;
#endif
    /** Initialization status tracker. Used when some of the device parameters
     *  or the device itself is changed during the runtime. */
    volatile uint32_t           status;
    /** Flag whether the "default device changed" listener was registered. */
    bool                        fDefDevChgListReg;
    /** Flag whether the "device state changed" listener was registered. */
    bool                        fDevStateChgListReg;
    /** Unified attribtues. */
    /** @todo Remove this as soon as we have unified input/output streams in this backend. */
    COREAUDIOSTREAM             Stream;
    /** Callback context for this stream for handing this stream in an CoreAudio callback.
     ** @todo Remove this as soon as we have unified input/output streams in this backend. */
    COREAUDIOSTREAMCBCTX        cbCtx;
} COREAUDIOSTREAMIN, *PCOREAUDIOSTREAMIN;

static int drvHostCoreAudioReinitInput(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMIN pHstStrmIn);
static int drvHostCoreAudioReinitOutput(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMOUT pHstStrmOut);

static int drvHostCoreAudioControlIn(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMIN pHstStrmIn, PDMAUDIOSTREAMCMD enmStreamCmd);
static int drvHostCoreAudioControlOut(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMOUT pHstStrmOut, PDMAUDIOSTREAMCMD enmStreamCmd);
static int drvHostCoreAudioFiniIn(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMIN pHstStrmIn);
static int drvHostCoreAudioFiniOut(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMOUT pHstStrmOut);

static OSStatus drvHostCoreAudioDevPropChgCb(AudioObjectID propertyID, UInt32 nAddresses, const AudioObjectPropertyAddress properties[], void *pvUser);

static void coreAudioInputQueueCb(void *pvUser, AudioQueueRef audioQueue, AudioQueueBufferRef audioBuffer, const AudioTimeStamp *pAudioTS, UInt32 cPacketDesc, const AudioStreamPacketDescription *paPacketDesc);
static void coreAudioOutputQueueCb(void *pvUser, AudioQueueRef audioQueue, AudioQueueBufferRef audioBuffer);


/**
 * Does a (Re-)enumeration of the host's playback + recording devices.
 *
 * @return  IPRT status code.
 * @param   pThis               Host audio driver instance.
 * @param   pCfg                Where to store the enumeration results.
 * @param   fEnum               Enumeration flags.
 */
static int drvHostCoreAudioDevicesEnumerate(PDRVHOSTCOREAUDIO pThis, PPDMAUDIOBACKENDCFG pCfg, bool fIn, uint32_t fEnum)
{
    RT_NOREF(fEnum);
    AssertPtrReturn(pThis, VERR_INVALID_POINTER);
    /* pCfg is optional. */

    int rc = VINF_SUCCESS;

    uint8_t cDevs = 0;

    do
    {
        AudioObjectPropertyAddress propAdrDevList = { kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal,
                                                      kAudioObjectPropertyElementMaster };
        UInt32 uSize = 0;
        OSStatus err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propAdrDevList, 0, NULL, &uSize);
        if (err != kAudioHardwareNoError)
            break;

        AudioDeviceID *pDevIDs = (AudioDeviceID *)alloca(uSize);
        if (pDevIDs == NULL)
            break;

        err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propAdrDevList, 0, NULL, &uSize, pDevIDs);
        if (err != kAudioHardwareNoError)
            break;

        UInt32 cDevices = uSize / sizeof (AudioDeviceID);
        for (UInt32 i = 0; i < cDevices; i++)
        {
            AudioDeviceID curDevID = pDevIDs[i];

            /* Check if the device is valid. */
            AudioObjectPropertyAddress propAddrCfg = { kAudioDevicePropertyStreamConfiguration,
                                                       fIn ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput,
                                                       kAudioObjectPropertyElementMaster };

            err = AudioObjectGetPropertyDataSize(curDevID, &propAddrCfg, 0, NULL, &uSize);
            if (err != noErr)
                continue;

            AudioBufferList *pBufList = (AudioBufferList *)RTMemAlloc(uSize);
            if (!pBufList)
                continue;

            bool fIsValid      = false;
            uint16_t cChannels = 0;

            err = AudioObjectGetPropertyData(curDevID, &propAddrCfg, 0, NULL, &uSize, pBufList);
            if (err == noErr)
            {
                for (UInt32 a = 0; a < pBufList->mNumberBuffers; a++)
                    cChannels += pBufList->mBuffers[a].mNumberChannels;

                fIsValid = cChannels > 0;
            }

            if (pBufList)
            {
                RTMemFree(pBufList);
                pBufList = NULL;
            }

            if (!fIsValid)
                continue;

            /* Resolve the device's name. */
            AudioObjectPropertyAddress propAddrName = { kAudioObjectPropertyName,
                                                        fIn ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput,
                                                        kAudioObjectPropertyElementMaster };
            uSize = sizeof(CFStringRef);
            CFStringRef pcfstrName = NULL;

            err = AudioObjectGetPropertyData(curDevID, &propAddrName, 0, NULL, &uSize, &pcfstrName);
            if (err != kAudioHardwareNoError)
                continue;

            CFIndex uMax = CFStringGetMaximumSizeForEncoding(CFStringGetLength(pcfstrName), kCFStringEncodingUTF8) + 1;
            if (uMax)
            {
                char *pszName = (char *)RTStrAlloc(uMax);
                if (   pszName
                    && CFStringGetCString(pcfstrName, pszName, uMax, kCFStringEncodingUTF8))
                {
                    LogRel2(("CoreAudio: Found %s device '%s' (%RU16 channels max)\n",
                             fIn ? "recording" : "playback", pszName, cChannels));
                    cDevs++;
                }

                if (pszName)
                {
                    RTStrFree(pszName);
                    pszName = NULL;
                }
            }

            CFRelease(pcfstrName);
        }

    } while (0);

    if (fIn)
        LogRel2(("CoreAudio: Found %RU8 recording device(s)\n", cDevs));
    else
        LogRel2(("CoreAudio: Found %RU8 playback device(s)\n", cDevs));

    if (pCfg)
    {
        if (fIn)
            pCfg->cMaxHstStrmsIn  = cDevs;
        else
            pCfg->cMaxHstStrmsOut = cDevs;
    }

    LogFlowFuncLeaveRC(rc);
    return rc;
}

/**
 * Updates this host driver's internal status, according to the global, overall input/output
 * state and all connected (native) audio streams.
 *
 * @param   pThis               Host audio driver instance.
 * @param   pCfg                Where to store the backend configuration. Optional.
 * @param   fEnum               Enumeration flags.
 */
int coreAudioUpdateStatusInternalEx(PDRVHOSTCOREAUDIO pThis, PPDMAUDIOBACKENDCFG pCfg, uint32_t fEnum)
{
    RT_NOREF(fEnum);
    AssertPtrReturn(pThis, VERR_INVALID_POINTER);
    /* pCfg is optional. */

    PDMAUDIOBACKENDCFG Cfg;
    RT_ZERO(Cfg);

    Cfg.cbStreamOut = sizeof(COREAUDIOSTREAMOUT);
    Cfg.cbStreamIn  = sizeof(COREAUDIOSTREAMIN);

    int rc = drvHostCoreAudioDevicesEnumerate(pThis, &Cfg, false /* fIn */, 0 /* fEnum */);
    AssertRC(rc);
    rc = drvHostCoreAudioDevicesEnumerate(pThis, &Cfg, true /* fIn */, 0 /* fEnum */);
    AssertRC(rc);

    if (pCfg)
        memcpy(pCfg, &Cfg, sizeof(PDMAUDIOBACKENDCFG));

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(OSStatus) drvHostCoreAudioDeviceStateChangedCb(AudioObjectID propertyID,
                                                                   UInt32 nAddresses,
                                                                   const AudioObjectPropertyAddress properties[],
                                                                   void *pvUser)
{
    RT_NOREF(propertyID, nAddresses, properties);
    LogFlowFunc(("propertyID=%u, nAddresses=%u, pvUser=%p\n", propertyID, nAddresses, pvUser));

    PCOREAUDIOSTREAMCBCTX pCbCtx = (PCOREAUDIOSTREAMCBCTX)pvUser;
    AssertPtr(pCbCtx);
    AssertPtr(pCbCtx->pStream);

    UInt32 uAlive = 1;
    UInt32 uSize  = sizeof(UInt32);

    AudioObjectPropertyAddress propAdr = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal,
                                           kAudioObjectPropertyElementMaster };

    AudioDeviceID deviceID = pCbCtx->pStream->enmDir == PDMAUDIODIR_IN
                           ? pCbCtx->pStream->pIn->deviceID : pCbCtx->pStream->pOut->deviceID;

    OSStatus err = AudioObjectGetPropertyData(deviceID, &propAdr, 0, NULL, &uSize, &uAlive);

    bool fIsDead = false;

    if (err == kAudioHardwareBadDeviceError)
        fIsDead = true; /* Unplugged. */
    else if ((err == kAudioHardwareNoError) && (!RT_BOOL(uAlive)))
        fIsDead = true; /* Something else happened. */

    if (fIsDead)
    {
        switch (pCbCtx->pStream->enmDir)
        {
            case PDMAUDIODIR_IN:
            {
                PCOREAUDIOSTREAMIN pStreamIn = pCbCtx->pStream->pIn;

                /* We move the reinitialization to the next output event.
                 * This make sure this thread isn't blocked and the
                 * reinitialization is done when necessary only. */
                ASMAtomicXchgU32(&pStreamIn->status, CA_STATUS_REINIT);

                LogRel(("CoreAudio: Recording device stopped functioning\n"));
                break;
            }

            case PDMAUDIODIR_OUT:
            {
                PCOREAUDIOSTREAMOUT pStreamOut = pCbCtx->pStream->pOut;

                /* We move the reinitialization to the next output event.
                 * This make sure this thread isn't blocked and the
                 * reinitialization is done when necessary only. */
                ASMAtomicXchgU32(&pStreamOut->status, CA_STATUS_REINIT);

                LogRel(("CoreAudio: Playback device stopped functioning\n"));
                break;
            }

            default:
                AssertMsgFailed(("Not implemented\n"));
                break;
        }
    }

    int rc2 = drvHostCoreAudioDevicesEnumerate(pCbCtx->pThis, NULL /* pCfg */, false /* fIn */, 0 /* fEnum */);
    AssertRC(rc2);
    rc2 = drvHostCoreAudioDevicesEnumerate(pCbCtx->pThis, NULL /* pCfg */, true /* fIn */, 0 /* fEnum */);
    AssertRC(rc2);

    return noErr;
}

/* Callback for getting notified when the default recording/playback device has been changed. */
static DECLCALLBACK(OSStatus) drvHostCoreAudioDefaultDeviceChangedCb(AudioObjectID propertyID,
                                                                     UInt32 nAddresses,
                                                                     const AudioObjectPropertyAddress properties[],
                                                                     void *pvUser)
{
    RT_NOREF(propertyID);
    OSStatus err = noErr;

    LogFlowFunc(("propertyID=%u, nAddresses=%u, pvUser=%p\n", propertyID, nAddresses, pvUser));

    PCOREAUDIOSTREAMCBCTX pCbCtx = (PCOREAUDIOSTREAMCBCTX)pvUser;
    AssertPtr(pCbCtx);
    AssertPtr(pCbCtx->pStream);

    for (UInt32 idxAddress = 0; idxAddress < nAddresses; idxAddress++)
    {
        const AudioObjectPropertyAddress *pProperty = &properties[idxAddress];

        switch (pProperty->mSelector)
        {
            case kAudioHardwarePropertyDefaultInputDevice:
            {
                PCOREAUDIOSTREAMIN pStreamIn = pCbCtx->pStream->pIn;
                AssertPtr(pStreamIn);

                /* This listener is called on every change of the hardware
                 * device. So check if the default device has really changed. */
                UInt32 uSize = sizeof(pStreamIn->deviceID);
                UInt32 uResp;
                err = AudioObjectGetPropertyData(kAudioObjectSystemObject, pProperty, 0, NULL, &uSize, &uResp);

                if (err == noErr)
                {
                    if (pStreamIn->deviceID != uResp)
                    {
                        LogRel(("CoreAudio: Default device for recording has changed\n"));

                        /* We move the reinitialization to the next input event.
                         * This make sure this thread isn't blocked and the
                         * reinitialization is done when necessary only. */
                        ASMAtomicXchgU32(&pStreamIn->status, CA_STATUS_REINIT);
                    }
                }
                break;
            }

            case kAudioHardwarePropertyDefaultOutputDevice:
            {
                PCOREAUDIOSTREAMOUT pStreamOut = pCbCtx->pStream->pOut;
                AssertPtr(pStreamOut);

                /* This listener is called on every change of the hardware
                 * device. So check if the default device has really changed. */
                AudioObjectPropertyAddress propAdr = { kAudioHardwarePropertyDefaultOutputDevice,
                                                       kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };

                UInt32 uSize = sizeof(pStreamOut->deviceID);
                UInt32 uResp;
                err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propAdr, 0, NULL, &uSize, &uResp);

                if (err == noErr)
                {
                    if (pStreamOut->deviceID != uResp)
                    {
                        LogRel(("CoreAudio: Default device for playback has changed\n"));

                        /* We move the reinitialization to the next input event.
                         * This make sure this thread isn't blocked and the
                         * reinitialization is done when necessary only. */
                        ASMAtomicXchgU32(&pStreamOut->status, CA_STATUS_REINIT);
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    int rc2 = drvHostCoreAudioDevicesEnumerate(pCbCtx->pThis, NULL /* pCfg */, false /* fIn */, 0 /* fEnum */);
    AssertRC(rc2);
    rc2 = drvHostCoreAudioDevicesEnumerate(pCbCtx->pThis, NULL /* pCfg */, true /* fIn */, 0 /* fEnum */);
    AssertRC(rc2);

    /** @todo Implement callback notification here to let the audio connector / device emulation
     *        know that something has changed. */

    return noErr;
}

/**
 * Thread for a Core Audio stream's audio queue handling.
 * This thread is required per audio queue to pump data to/from the Core Audio stream and
 * handling its callbacks.
 *
 * @returns IPRT status code.
 * @param   hThreadSelf         Thread handle.
 * @param   pvUser              User argument.
 */
static DECLCALLBACK(int) coreAudioQueueThread(RTTHREAD hThreadSelf, void *pvUser)
{
    NOREF(hThreadSelf);

    PCOREAUDIOSTREAM pCAStream = (PCOREAUDIOSTREAM)pvUser;
    AssertPtr(pCAStream);

    LogFunc(("Starting pCAStream=%p\n", pCAStream));

    /*
     * Create audio queue.
     */
    OSStatus err;
    if (pCAStream->enmDir == PDMAUDIODIR_IN)
        err = AudioQueueNewInput(&pCAStream->asbdStream, coreAudioInputQueueCb, pCAStream /* pvData */,
                                 CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0, &pCAStream->audioQueue);
    else
        err = AudioQueueNewOutput(&pCAStream->asbdStream, coreAudioOutputQueueCb, pCAStream /* pvData */,
                                  CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0, &pCAStream->audioQueue);

    if (err != noErr)
    {
        LogRel(("CoreAudio: Failed to create audio queue (%RI32)\n", err));
        return VERR_GENERAL_FAILURE; /** @todo Fudge! */
    }

    ///@todo: The following code may cause subsequent AudioQueueStart to fail
    // with !dev error (560227702). If we skip it entirely, we end up using the
    // default device, which is what we're doing anyway. 
#if 0
    /*
     * Assign device to queue.
     */
    UInt32 uSize = sizeof(pCAStream->UUID);
    err = AudioQueueSetProperty(pCAStream->audioQueue, kAudioQueueProperty_CurrentDevice, &pCAStream->UUID, uSize);
    if (err != noErr)
    {
        LogRel(("CoreAudio: Failed to set queue device UUID (%d)\n", err));
        return VERR_GENERAL_FAILURE; /** @todo Fudge! */
    }
#endif

    const size_t cbBufSize = AQ_BUF_SIZE; /** @todo Make this configurable! */

    /*
     * Allocate audio buffers.
     */
    for (size_t i = 0; i < RT_ELEMENTS(pCAStream->audioBuffer); i++)
    {
        err = AudioQueueAllocateBuffer(pCAStream->audioQueue, cbBufSize, &pCAStream->audioBuffer[i]);
        if (err != noErr)
            break;
    }

    if (err != noErr)
        return VERR_GENERAL_FAILURE; /** @todo Fudge! */

    /* Signal the main thread before entering the main loop. */
    RTThreadUserSignal(RTThreadSelf());

    /*
     * Enter the main loop.
     */
    const bool fIn = pCAStream->enmDir == PDMAUDIODIR_IN;

    while (!ASMAtomicReadBool(&pCAStream->fShutdown))
    {
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.10, 1);
    }

    /*
     * Cleanup.
     */
    if (fIn)
    {
        AudioQueueStop(pCAStream->audioQueue, 1);
    }
    else
    {
        AudioQueueStop(pCAStream->audioQueue, 0);
    }

    for (size_t i = 0; i < RT_ELEMENTS(pCAStream->audioBuffer); i++)
    {
        if (pCAStream->audioBuffer[i])
            AudioQueueFreeBuffer(pCAStream->audioQueue, pCAStream->audioBuffer[i]);
    }

    AudioQueueDispose(pCAStream->audioQueue, 1);
    pCAStream->audioQueue = NULL;

    LogFunc(("Ended pCAStream=%p\n", pCAStream));
    return VINF_SUCCESS;
}

/**
 * Processes input data of an audio queue buffer and stores it into a Core Audio stream.
 *
 * @returns IPRT status code.
 * @param   pCAStream           Core Audio stream to store input data into.
 * @param   audioBuffer         Audio buffer to process input data from.
 */
int coreAudioInputQueueProcBuffer(PCOREAUDIOSTREAM pCAStream, AudioQueueBufferRef audioBuffer)
{
    PRTCIRCBUF pCircBuf = pCAStream->pCircBuf;
    AssertPtr(pCircBuf);

    UInt8 *pvSrc = (UInt8 *)audioBuffer->mAudioData;
    UInt8 *pvDst = NULL;

    size_t cbWritten = 0;

    size_t cbToWrite = audioBuffer->mAudioDataByteSize;
    size_t cbLeft    = cbToWrite;

    while (cbLeft)
    {
        /* Try to acquire the necessary block from the ring buffer. */
        RTCircBufAcquireWriteBlock(pCircBuf, cbLeft, (void **)&pvDst, &cbToWrite);

        if (!cbToWrite)
            break;

        /* Copy the data from our ring buffer to the core audio buffer. */
        memcpy((UInt8 *)pvDst, pvSrc + cbWritten, cbToWrite);

        /* Release the read buffer, so it could be used for new data. */
        RTCircBufReleaseWriteBlock(pCircBuf, cbToWrite);

        cbWritten += cbToWrite;

        Assert(cbLeft >= cbToWrite);
        cbLeft -= cbToWrite;
    }

    Log3Func(("pCAStream=%p, cbBuffer=%RU32/%zu, cbWritten=%zu\n",
              pCAStream, audioBuffer->mAudioDataByteSize, audioBuffer->mAudioDataBytesCapacity, cbWritten));

    return VINF_SUCCESS;
}

/**
 * Input audio queue callback. Called whenever input data from the audio queue becomes available.
 *
 * @param   pvUser              User argument.
 * @param   audioQueue          Audio queue to process input data from.
 * @param   audioBuffer         Audio buffer to process input data from. Must be part of audio queue.
 * @param   pAudioTS            Audio timestamp.
 * @param   cPacketDesc         Number of packet descriptors.
 * @param   paPacketDesc        Array of packet descriptors.
 */
static DECLCALLBACK(void) coreAudioInputQueueCb(void *pvUser, AudioQueueRef audioQueue, AudioQueueBufferRef audioBuffer,
                                                const AudioTimeStamp *pAudioTS,
                                                UInt32 cPacketDesc, const AudioStreamPacketDescription *paPacketDesc)
{
    NOREF(pAudioTS);
    NOREF(cPacketDesc);
    NOREF(paPacketDesc);

    PCOREAUDIOSTREAM pCAStream = (PCOREAUDIOSTREAM)pvUser;
    AssertPtr(pCAStream);

    int rc = RTCritSectEnter(&pCAStream->CritSect);
    AssertRC(rc);

    rc = coreAudioInputQueueProcBuffer(pCAStream, audioBuffer);
    if (RT_SUCCESS(rc))
        AudioQueueEnqueueBuffer(audioQueue, audioBuffer, 0, NULL);

    rc = RTCritSectLeave(&pCAStream->CritSect);
    AssertRC(rc);
}

/**
 * Processes output data of a Core Audio stream into an audio queue buffer.
 *
 * @returns IPRT status code.
 * @param   pCAStream           Core Audio stream to process output data for.
 * @param   audioBuffer         Audio buffer to store data into.
 */
int coreAudioOutputQueueProcBuffer(PCOREAUDIOSTREAM pCAStream, AudioQueueBufferRef audioBuffer)
{
    PRTCIRCBUF pCircBuf = pCAStream->pCircBuf;
    AssertPtr(pCircBuf);

    size_t cbRead = 0;

    UInt8 *pvSrc = NULL;
    UInt8 *pvDst = (UInt8 *)audioBuffer->mAudioData;

    size_t cbToRead = RT_MIN(RTCircBufUsed(pCircBuf), audioBuffer->mAudioDataBytesCapacity);
    size_t cbLeft   = cbToRead;

    while (cbLeft)
    {
        /* Try to acquire the necessary block from the ring buffer. */
        RTCircBufAcquireReadBlock(pCircBuf, cbLeft, (void **)&pvSrc, &cbToRead);

        if (cbToRead)
        {
            /* Copy the data from our ring buffer to the core audio buffer. */
            memcpy((UInt8 *)pvDst + cbRead, pvSrc, cbToRead);
        }

        /* Release the read buffer, so it could be used for new data. */
        RTCircBufReleaseReadBlock(pCircBuf, cbToRead);

        if (!cbToRead)
            break;

        /* Move offset. */
        cbRead += cbToRead;
        Assert(cbRead <= audioBuffer->mAudioDataBytesCapacity);

        Assert(cbToRead <= cbLeft);
        cbLeft -= cbToRead;
    }

    audioBuffer->mAudioDataByteSize = cbRead;

    if (audioBuffer->mAudioDataByteSize < audioBuffer->mAudioDataBytesCapacity)
    {
        RT_BZERO((UInt8 *)audioBuffer->mAudioData + audioBuffer->mAudioDataByteSize,
                 audioBuffer->mAudioDataBytesCapacity - audioBuffer->mAudioDataByteSize);

        audioBuffer->mAudioDataByteSize = audioBuffer->mAudioDataBytesCapacity;
    }

    Log3Func(("pCAStream=%p, cbCapacity=%RU32, cbRead=%zu\n",
              pCAStream, audioBuffer->mAudioDataBytesCapacity, cbRead));

    return VINF_SUCCESS;
}

/**
 * Output audio queue callback. Called whenever an audio queue is ready to process more output data.
 *
 * @param   pvUser              User argument.
 * @param   audioQueue          Audio queue to process output data for.
 * @param   audioBuffer         Audio buffer to store output data in. Must be part of audio queue.
 */
static DECLCALLBACK(void) coreAudioOutputQueueCb(void *pvUser, AudioQueueRef audioQueue, AudioQueueBufferRef audioBuffer)
{
    PCOREAUDIOSTREAM pCAStream = (PCOREAUDIOSTREAM)pvUser;
    AssertPtr(pCAStream);

    int rc = RTCritSectEnter(&pCAStream->CritSect);
    AssertRC(rc);

    rc = coreAudioOutputQueueProcBuffer(pCAStream, audioBuffer);
    if (RT_SUCCESS(rc))
        AudioQueueEnqueueBuffer(audioQueue, audioBuffer, 0, NULL);

    rc = RTCritSectLeave(&pCAStream->CritSect);
    AssertRC(rc);
}

/**
 * Invalidates a Core Audio stream's audio queue.
 *
 * @returns IPRT status code.
 * @param   pCAStream           Core Audio stream to invalidate its queue for.
 */
static int coreAudioStreamInvalidateQueue(PCOREAUDIOSTREAM pCAStream)
{
    int rc = VINF_SUCCESS;

    for (size_t i = 0; i < RT_ELEMENTS(pCAStream->audioBuffer); i++)
    {
        AudioQueueBufferRef pBuf = pCAStream->audioBuffer[i];

        if (pCAStream->enmDir == PDMAUDIODIR_IN)
        {
            int rc2 = coreAudioInputQueueProcBuffer(pCAStream, pBuf);
            if (RT_SUCCESS(rc2))
            {
                AudioQueueEnqueueBuffer(pCAStream->audioQueue, pBuf, 0, NULL);
            }
        }
        else if (pCAStream->enmDir == PDMAUDIODIR_OUT)
        {
            int rc2 = coreAudioOutputQueueProcBuffer(pCAStream, pBuf);
            if (   RT_SUCCESS(rc2)
                && pBuf->mAudioDataByteSize)
            {
                AudioQueueEnqueueBuffer(pCAStream->audioQueue, pBuf, 0, NULL);
            }

            if (RT_SUCCESS(rc))
                rc = rc2;
        }
        else
            AssertFailed();
    }

    return rc;
}

/**
 * Initializes a Core Audio stream's audio queue.
 *
 * @returns IPRT status code.
 * @param   pCAStream           Core Audio stream to initialize audio queue for.
 * @param   fIn                 Whether this is an input or output queue.
 * @param   pCfgReq             Requested stream configuration.
 * @param   pCfgAcq             Acquired stream configuration on success.
 */
static int coreAudioStreamInitQueue(PCOREAUDIOSTREAM pCAStream, bool fIn, PPDMAUDIOSTREAMCFG pCfgReq, PPDMAUDIOSTREAMCFG pCfgAcq)
{
    RT_NOREF(pCfgAcq);
    LogFunc(("pCAStream=%p, pCfgReq=%p, pCfgAcq=%p\n", pCAStream, pCfgReq, pCfgAcq));

    AudioDeviceID deviceID = kAudioDeviceUnknown;

    /* Fetch the default audio device currently in use. */
    AudioObjectPropertyAddress propAdrDefaultDev = {   fIn
                                                     ? kAudioHardwarePropertyDefaultInputDevice
                                                     : kAudioHardwarePropertyDefaultOutputDevice,
                                                     kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    UInt32 uSize = sizeof(deviceID);
    OSStatus err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propAdrDefaultDev, 0, NULL, &uSize, &deviceID);
    if (err != noErr)
    {
        LogRel(("CoreAudio: Unable to determine default %s device (%RI32)\n",
                fIn ? "capturing" : "playback", err));
        return VERR_NOT_FOUND;
    }

    /* Get the device UUID. */
    AudioObjectPropertyAddress propAdrDevUUID = { kAudioDevicePropertyDeviceUID,
                                                  fIn ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput,
                                                  kAudioObjectPropertyElementMaster };
    uSize = sizeof(pCAStream->UUID);
    err = AudioObjectGetPropertyData(deviceID, &propAdrDevUUID, 0, NULL, &uSize, &pCAStream->UUID);
    if (err != noErr)
    {
        LogRel(("CoreAudio: Failed to retrieve device UUID for device %RU32 (%RI32)\n", deviceID, err));
        return VERR_NOT_FOUND;
    }

    /* Create the recording device's out format based on our required audio settings. */
    int rc = drvHostCoreAudioStreamCfgToASBD(pCfgReq, &pCAStream->asbdStream);
    if (RT_FAILURE(rc))
    {
        LogRel(("CoreAudio: Failed to convert requested %s format to native format (%Rrc)\n",
                fIn ? "input" : "output", rc));
        return rc;
    }

    pCAStream->enmDir = fIn ? PDMAUDIODIR_IN : PDMAUDIODIR_OUT;

    /* Assign device ID. */
    if (fIn)
    {
        AssertPtr(pCAStream->pIn);
        pCAStream->pIn->deviceID = deviceID;
    }
    else
    {
        AssertPtr(pCAStream->pOut);
        pCAStream->pOut->deviceID = deviceID;
    }

    drvHostCoreAudioPrintASBD(  fIn
                              ? "Capturing queue format"
                              : "Playback queue format", &pCAStream->asbdStream);

    rc = RTCircBufCreate(&pCAStream->pCircBuf, 8096 << 1 /*pHstStrmIn->Props.cShift*/); /** @todo FIX THIS !!! */
    if (RT_FAILURE(rc))
        return rc;

    /*
     * Start the thread.
     */
    pCAStream->fShutdown = false;
    rc = RTThreadCreate(&pCAStream->hThread, coreAudioQueueThread,
                        pCAStream /* pvUser */, 0 /* Default stack size */,
                        RTTHREADTYPE_DEFAULT, RTTHREADFLAGS_WAITABLE, "CAQUEUE");
    if (RT_SUCCESS(rc))
        rc = RTThreadUserWait(pCAStream->hThread, 10 * 1000 /* 10s timeout */);

    LogFunc(("Returning %Rrc\n", rc));
    return rc;
}

/**
 * Unitializes a Core Audio stream's audio queue.
 *
 * @returns IPRT status code.
 * @param   pCAStream           Core Audio stream to unitialize audio queue for.
 */
static int coreAudioStreamUninitQueue(PCOREAUDIOSTREAM pCAStream)
{
    LogFunc(("pCAStream=%p\n", pCAStream));

    int rc;

    if (pCAStream->hThread != NIL_RTTHREAD)
    {
        LogFunc(("Waiting for thread ...\n"));

        ASMAtomicXchgBool(&pCAStream->fShutdown, true);

        int rcThread;
        rc = RTThreadWait(pCAStream->hThread, 30 * 1000, &rcThread);
        if (RT_FAILURE(rc))
            return rc;

        NOREF(rcThread);
        LogFunc(("Thread stopped with %Rrc\n", rcThread));

        pCAStream->hThread = NIL_RTTHREAD;
    }

    if (pCAStream->pCircBuf)
    {
        RTCircBufDestroy(pCAStream->pCircBuf);
        pCAStream->pCircBuf = NULL;
    }

    LogFunc(("Returning\n"));
    return VINF_SUCCESS;
}

#if 0 // unused
/**
 * Unitializes a Core Audio stream.
 *
 * @returns IPRT status code.
 * @param   pCAStream           Core Audio stream to uninitialize.
 */
static int coreAudioStreamUninit(PCOREAUDIOSTREAM pCAStream)
{
    LogFunc(("pCAStream=%p\n", pCAStream));

    int rc = coreAudioStreamUninitQueue(pCAStream);
    return rc;
}
#endif

static int drvHostCoreAudioReinitInput(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMIN pHstStrmIn)
{
    int rc = drvHostCoreAudioFiniIn(pInterface, pHstStrmIn);
    if (RT_SUCCESS(rc))
    {
        PCOREAUDIOSTREAMIN pStreamIn = (PCOREAUDIOSTREAMIN)pHstStrmIn;

        PDMAUDIOSTREAMCFG CfgAcq;
        rc = drvHostCoreAudioASBDToStreamCfg(&pStreamIn->cbCtx.pStream->asbdStream, &CfgAcq);
        if (RT_SUCCESS(rc))
        {
            int rc2;
            rc2 = RTCritSectInit(&pStreamIn->cbCtx.pStream->CritSect);
            AssertRC(rc2);
                   
            rc = coreAudioStreamInitQueue(pStreamIn->cbCtx.pStream, true /* fInput */, &CfgAcq /* pCfgReq */, NULL /* pCfgAcq */);
            if (RT_SUCCESS(rc))
            {
                ASMAtomicXchgU32(&pStreamIn->status, CA_STATUS_INIT);
                rc = drvHostCoreAudioControlIn(pInterface, pHstStrmIn, PDMAUDIOSTREAMCMD_ENABLE);
            }

            if (RT_FAILURE(rc))
            {
                rc2 = drvHostCoreAudioFiniIn(pInterface, pHstStrmIn);
                AssertRC(rc2);
            }
        }
    }

    if (RT_FAILURE(rc))
        LogRel(("CoreAudio: Unable to re-init input stream: %Rrc\n", rc));

    return rc;
}

static int drvHostCoreAudioReinitOutput(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMOUT pHstStrmOut)
{
    int rc = drvHostCoreAudioFiniOut(pInterface, pHstStrmOut);
    if (RT_SUCCESS(rc))
    {
        PCOREAUDIOSTREAMOUT pStreamOut = (PCOREAUDIOSTREAMOUT)pHstStrmOut;

        PDMAUDIOSTREAMCFG CfgAcq;
        rc = drvHostCoreAudioASBDToStreamCfg(&pStreamOut->cbCtx.pStream->asbdStream, &CfgAcq);
        if (RT_SUCCESS(rc))
        {
            int rc2;
            rc2 = RTCritSectInit(&pStreamOut->cbCtx.pStream->CritSect);
            AssertRC(rc2);

            rc = coreAudioStreamInitQueue(pStreamOut->cbCtx.pStream, false /* fInput */, &CfgAcq /* pCfgReq */, NULL /* pCfgAcq */);
            if (RT_SUCCESS(rc))
            {
                ASMAtomicXchgU32(&pStreamOut->status, CA_STATUS_INIT);
                rc = drvHostCoreAudioControlOut(pInterface, pHstStrmOut, PDMAUDIOSTREAMCMD_ENABLE);
            }

            if (RT_FAILURE(rc))
            {
                rc2 = drvHostCoreAudioFiniOut(pInterface, pHstStrmOut);
                AssertRC(rc2);
            }
        }
    }

    if (RT_FAILURE(rc))
        LogRel(("CoreAudio: Unable to re-init output stream: %Rrc\n", rc));

    return rc;
}

#ifdef VBOX_WITH_AUDIO_CA_CONVERTER
/* Callback to convert audio input data from one format to another. */
static DECLCALLBACK(OSStatus) drvHostCoreAudioConverterCb(AudioConverterRef              inAudioConverter,
                                                          UInt32                        *ioNumberDataPackets,
                                                          AudioBufferList               *ioData,
                                                          AudioStreamPacketDescription **ppASPD,
                                                          void                          *pvUser)
{
    RT_NOREF(inAudioConverter);
    AssertPtrReturn(ioNumberDataPackets, caConverterEOFDErr);
    AssertPtrReturn(ioData,              caConverterEOFDErr);

    PCOREAUDIOCONVCBCTX pConvCbCtx = (PCOREAUDIOCONVCBCTX)pvUser;
    AssertPtr(pConvCbCtx);

    /* Initialize values. */
    ioData->mBuffers[0].mNumberChannels = 0;
    ioData->mBuffers[0].mDataByteSize   = 0;
    ioData->mBuffers[0].mData           = NULL;

    if (ppASPD)
    {
        Log3Func(("Handling packet description not implemented\n"));
    }
    else
    {
        /** @todo Check converter ID? */

        /** @todo Handled non-interleaved data by going through the full buffer list,
         *        not only through the first buffer like we do now. */
        Log3Func(("ioNumberDataPackets=%RU32\n", *ioNumberDataPackets));

        UInt32 cNumberDataPackets = *ioNumberDataPackets;
        Assert(pConvCbCtx->uPacketIdx + cNumberDataPackets <= pConvCbCtx->uPacketCnt);

        if (cNumberDataPackets)
        {
            AssertPtr(pConvCbCtx->pBufLstSrc);
            Assert(pConvCbCtx->pBufLstSrc->mNumberBuffers == 1); /* Only one buffer for the source supported atm. */

            AudioStreamBasicDescription *pSrcASBD = &pConvCbCtx->asbdSrc;
            AudioBuffer                 *pSrcBuf  = &pConvCbCtx->pBufLstSrc->mBuffers[0];

            size_t cbOff   = pConvCbCtx->uPacketIdx * pSrcASBD->mBytesPerPacket;

            cNumberDataPackets = RT_MIN((pSrcBuf->mDataByteSize - cbOff) / pSrcASBD->mBytesPerPacket,
                                        cNumberDataPackets);

            void  *pvAvail = (uint8_t *)pSrcBuf->mData + cbOff;
            size_t cbAvail = RT_MIN(pSrcBuf->mDataByteSize - cbOff, cNumberDataPackets * pSrcASBD->mBytesPerPacket);

            Log3Func(("cNumberDataPackets=%RU32, cbOff=%zu, cbAvail=%zu\n", cNumberDataPackets, cbOff, cbAvail));

            /* Set input data for the converter to use.
             * Note: For VBR (Variable Bit Rates) or interleaved data handling we need multiple buffers here. */
            ioData->mNumberBuffers = 1;

            ioData->mBuffers[0].mNumberChannels = pSrcBuf->mNumberChannels;
            ioData->mBuffers[0].mDataByteSize   = cbAvail;
            ioData->mBuffers[0].mData           = pvAvail;

#ifdef DEBUG_DUMP_PCM_DATA
            RTFILE fh;
            int rc = RTFileOpen(&fh, DEBUG_DUMP_PCM_DATA_PATH "ca-converter-cb-input.pcm",
                                RTFILE_O_OPEN_CREATE | RTFILE_O_APPEND | RTFILE_O_WRITE | RTFILE_O_DENY_NONE);
            if (RT_SUCCESS(rc))
            {
                RTFileWrite(fh, pvAvail, cbAvail, NULL);
                RTFileClose(fh);
            }
            else
                AssertFailed();
#endif
            pConvCbCtx->uPacketIdx += cNumberDataPackets;
            Assert(pConvCbCtx->uPacketIdx <= pConvCbCtx->uPacketCnt);

            *ioNumberDataPackets = cNumberDataPackets;
        }
    }

    Log3Func(("%RU32 / %RU32 -> ioNumberDataPackets=%RU32\n",
              pConvCbCtx->uPacketIdx, pConvCbCtx->uPacketCnt, *ioNumberDataPackets));

    return noErr;
}
#endif /* VBOX_WITH_AUDIO_CA_CONVERTER */


/* Callback for getting notified when some of the properties of an audio device have changed. */
static DECLCALLBACK(OSStatus) drvHostCoreAudioDevPropChgCb(AudioObjectID                     propertyID,
                                                           UInt32                            cAddresses,
                                                           const AudioObjectPropertyAddress  properties[],
                                                           void                             *pvUser)
{
    RT_NOREF(cAddresses, properties);
    PCOREAUDIOSTREAMCBCTX pCbCtx = (PCOREAUDIOSTREAMCBCTX)pvUser;
    AssertPtr(pCbCtx);
    AssertPtr(pCbCtx->pStream);

    LogFlowFunc(("propertyID=%u, nAddresses=%u, pCbCtx=%p\n", propertyID, cAddresses, pCbCtx));

    if (pCbCtx->pStream->enmDir == PDMAUDIODIR_IN)
    {
        PCOREAUDIOSTREAMIN pStreamIn = pCbCtx->pStream->pIn;
        AssertPtr(pStreamIn);

        switch (propertyID)
        {
#ifdef DEBUG
           case kAudioDeviceProcessorOverload:
            {
                LogFunc(("Processor overload detected!\n"));
                break;
            }
#endif /* DEBUG */
            case kAudioDevicePropertyNominalSampleRate:
            {
                LogRel2(("CoreAudio: Recording sample rate changed\n"));

                /* We move the reinitialization to the next input event.
                 * This make sure this thread isn't blocked and the
                 * reinitialization is done when necessary only. */
                ASMAtomicXchgU32(&pStreamIn->status, CA_STATUS_REINIT);
                break;
            }

            default:
                break;
        }
    }
    else
    {
        PCOREAUDIOSTREAMOUT pStreamOut = pCbCtx->pStream->pOut;
        AssertPtr(pStreamOut);

        switch (propertyID)
        {
            case kAudioDevicePropertyNominalSampleRate:
            {
                LogRel2(("CoreAudio: Playback sample rate changed\n"));

                /* We move the reinitialization to the next input event.
                 * This make sure this thread isn't blocked and the
                 * reinitialization is done when necessary only. */
                ASMAtomicXchgU32(&pStreamOut->status, CA_STATUS_REINIT);
                break;
            }

            default:
                break;
        }
    }

    return noErr;
}

static DECLCALLBACK(int) drvHostCoreAudioInit(PPDMIHOSTAUDIO pInterface)
{
    NOREF(pInterface);

    LogFlowFuncEnter();

    return VINF_SUCCESS;
}

static DECLCALLBACK(int) drvHostCoreAudioCaptureIn(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMIN pHstStrmIn,
                                                   uint32_t *pcSamplesCaptured)
{
    PCOREAUDIOSTREAMIN pStreamIn = (PCOREAUDIOSTREAMIN)pHstStrmIn;

    /* Check if the audio device should be reinitialized. If so do it. */
    if (ASMAtomicReadU32(&pStreamIn->status) == CA_STATUS_REINIT)
        drvHostCoreAudioReinitInput(pInterface, &pStreamIn->streamIn);

    if (ASMAtomicReadU32(&pStreamIn->status) != CA_STATUS_INIT)
    {
        if (pcSamplesCaptured)
            *pcSamplesCaptured = 0;
        return VINF_SUCCESS;
    }

    PRTCIRCBUF pCircBuf = NULL;
    pCircBuf = pStreamIn->cbCtx.pStream->pCircBuf;
    AssertPtr(pCircBuf);

    int rc = VINF_SUCCESS;
    uint32_t cbWrittenTotal = 0;

    do
    {
        size_t cbBuf     = AudioMixBufSizeBytes(&pHstStrmIn->MixBuf);
        size_t cbToWrite = RT_MIN(cbBuf, RTCircBufUsed(pCircBuf));

        uint32_t cWritten, cbWritten;
        uint8_t *puBuf;
        size_t   cbToRead;

        Log3Func(("cbBuf=%zu, cbToWrite=%zu/%zu\n", cbBuf, cbToWrite, RTCircBufSize(pCircBuf)));

        while (cbToWrite)
        {
            /* Try to acquire the necessary block from the ring buffer. */
            RTCircBufAcquireReadBlock(pCircBuf, cbToWrite, (void **)&puBuf, &cbToRead);

            if (cbToRead)
            {
#ifdef DEBUG_DUMP_PCM_DATA
                RTFILE fh;
                rc = RTFileOpen(&fh, DEBUG_DUMP_PCM_DATA_PATH "ca-capture.pcm",
                                RTFILE_O_OPEN_CREATE | RTFILE_O_APPEND | RTFILE_O_WRITE | RTFILE_O_DENY_NONE);
                if (RT_SUCCESS(rc))
                {
                    RTFileWrite(fh, puBuf + cbWrittenTotal, cbToRead, NULL);
                    RTFileClose(fh);
                }
                else
                    AssertFailed();
#endif
                rc = AudioMixBufWriteCirc(&pHstStrmIn->MixBuf, puBuf, cbToRead, &cWritten);
            }

            /* Release the read buffer, so it could be used for new data. */
            RTCircBufReleaseReadBlock(pCircBuf, cbToRead);

            if (   RT_FAILURE(rc)
                || !cWritten)
            {
                break;
            }

            cbWritten = AUDIOMIXBUF_S2B(&pHstStrmIn->MixBuf, cWritten);

            Assert(cbToWrite >= cbWritten);
            cbToWrite      -= cbWritten;
            cbWrittenTotal += cbWritten;
        }

        Log3Func(("cbToWrite=%zu, cbToRead=%zu, cbWrittenTotal=%RU32, rc=%Rrc\n", cbToWrite, cbToRead, cbWrittenTotal, rc));
    }
    while (0);

    if (RT_SUCCESS(rc))
    {
        uint32_t cCaptured     = 0;
        uint32_t cWrittenTotal = AUDIOMIXBUF_B2S(&pHstStrmIn->MixBuf, cbWrittenTotal);
        if (cWrittenTotal)
            rc = AudioMixBufMixToParent(&pHstStrmIn->MixBuf, cWrittenTotal, &cCaptured);

        Log3Func(("cWrittenTotal=%RU32 (%RU32 bytes), cCaptured=%RU32, rc=%Rrc\n", cWrittenTotal, cbWrittenTotal, cCaptured, rc));

        if (cCaptured)
            LogFlowFunc(("%RU32 samples captured\n", cCaptured));

        if (pcSamplesCaptured)
            *pcSamplesCaptured = cCaptured;
    }

    if (RT_FAILURE(rc))
        LogFunc(("Failed with rc=%Rrc\n", rc));

    return rc;
}

static DECLCALLBACK(int) drvHostCoreAudioPlayOut(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMOUT pHstStrmOut,
                                                 uint32_t *pcSamplesPlayed)
{
    PCOREAUDIOSTREAMOUT pStreamOut = (PCOREAUDIOSTREAMOUT)pHstStrmOut;

    int rc = VINF_SUCCESS;

    /* Check if the audio device should be reinitialized. If so do it. */
    if (ASMAtomicReadU32(&pStreamOut->status) == CA_STATUS_REINIT)
    {
        rc = drvHostCoreAudioReinitOutput(pInterface, &pStreamOut->streamOut);
        if (RT_FAILURE(rc))
            return rc;
    }

    uint32_t cLive = AudioMixBufAvail(&pHstStrmOut->MixBuf);
    if (!cLive) /* Not samples to play? Bail out. */
    {
        if (pcSamplesPlayed)
            *pcSamplesPlayed = 0;
        return VINF_SUCCESS;
    }

    PCOREAUDIOSTREAM pCAStream = pStreamOut->cbCtx.pStream;
    AssertPtr(pCAStream);

    PRTCIRCBUF pCircBuf = NULL;
    pCircBuf = pCAStream->pCircBuf;
    AssertPtr(pCircBuf);

    size_t cbLive  = AUDIOMIXBUF_S2B(&pHstStrmOut->MixBuf, cLive);

    uint32_t cbReadTotal = 0;

    size_t cbToRead = RT_MIN(cbLive, RTCircBufFree(pCircBuf));
    Log3Func(("cbLive=%zu, cbToRead=%zu\n", cbLive, cbToRead));

    uint8_t *pvChunk;
    size_t   cbChunk;

    while (cbToRead)
    {
        uint32_t cRead, cbRead;

        /* Try to acquire the necessary space from the ring buffer. */
        RTCircBufAcquireWriteBlock(pCircBuf, cbToRead, (void **)&pvChunk, &cbChunk);
        if (!cbChunk)
        {
            RTCircBufReleaseWriteBlock(pCircBuf, cbChunk);
            break;
        }

        Assert(cbChunk <= cbToRead);

        rc = AudioMixBufReadCirc(&pHstStrmOut->MixBuf, pvChunk, cbChunk, &cRead);

        cbRead = AUDIOMIXBUF_S2B(&pHstStrmOut->MixBuf, cRead);

        /* Release the ring buffer, so the read thread could start reading this data. */
        RTCircBufReleaseWriteBlock(pCircBuf, cbChunk);

        if (RT_FAILURE(rc))
            break;

        Assert(cbToRead >= cbRead);
        cbToRead -= cbRead;
        cbReadTotal += cbRead;
    }

    if (    RT_SUCCESS(rc)
        &&  pCAStream->fRun
        && !pCAStream->fIsRunning)
    {
        rc = coreAudioStreamInvalidateQueue(pCAStream);
        if (RT_SUCCESS(rc))
        {
            AudioQueueStart(pCAStream->audioQueue, NULL);
            pCAStream->fRun       = false;
            pCAStream->fIsRunning = true;
        }
    }

    if (RT_SUCCESS(rc))
    {
        uint32_t cReadTotal = AUDIOMIXBUF_B2S(&pHstStrmOut->MixBuf, cbReadTotal);
        if (cReadTotal)
            AudioMixBufFinish(&pHstStrmOut->MixBuf, cReadTotal);

        Log3Func(("cReadTotal=%RU32 (%RU32 bytes)\n", cReadTotal, cbReadTotal));

        if (pcSamplesPlayed)
            *pcSamplesPlayed = cReadTotal;
    }

    return rc;
}

static DECLCALLBACK(int) drvHostCoreAudioControlOut(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMOUT pHstStrmOut,
                                                    PDMAUDIOSTREAMCMD enmStreamCmd)
{
    RT_NOREF(pInterface);
    PCOREAUDIOSTREAMOUT pStreamOut = (PCOREAUDIOSTREAMOUT)pHstStrmOut;

    LogFlowFunc(("enmStreamCmd=%RU32\n", enmStreamCmd));

    uint32_t uStatus = ASMAtomicReadU32(&pStreamOut->status);
    if (!(   uStatus == CA_STATUS_INIT
          || uStatus == CA_STATUS_REINIT))
    {
        return VINF_SUCCESS;
    }

    int rc = VINF_SUCCESS;

    PCOREAUDIOSTREAM pCAStream = pStreamOut->cbCtx.pStream;
    AssertPtr(pCAStream);

    OSStatus err; RT_NOREF(err);
    switch (enmStreamCmd)
    {
        case PDMAUDIOSTREAMCMD_ENABLE:
        case PDMAUDIOSTREAMCMD_RESUME:
        {
            LogFunc(("Queue enable\n"));
            rc = coreAudioStreamInvalidateQueue(pCAStream);
            if (RT_SUCCESS(rc))
            {
                /* Start the audio queue immediately. */
                AudioQueueStart(pCAStream->audioQueue, NULL);
            }
            break;
        }

        case PDMAUDIOSTREAMCMD_DISABLE:
        {
            LogFunc(("Queue disable\n"));
            AudioQueueStop(pCAStream->audioQueue, 1 /* Immediately */);
            ASMAtomicXchgBool(&pCAStream->fRun,       false);
            ASMAtomicXchgBool(&pCAStream->fIsRunning, false);
            break;
        }

        case PDMAUDIOSTREAMCMD_PAUSE:
        {
            LogFunc(("Queue pause\n"));
            AudioQueuePause(pCAStream->audioQueue);
            ASMAtomicXchgBool(&pCAStream->fIsRunning, false);
            break;
        }

        default:
            rc = VERR_NOT_SUPPORTED;
            break;
    }

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(int) drvHostCoreAudioControlIn(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMIN pHstStrmIn,
                                                   PDMAUDIOSTREAMCMD enmStreamCmd)
{
    RT_NOREF(pInterface);
    PCOREAUDIOSTREAMIN pStreamIn = (PCOREAUDIOSTREAMIN)pHstStrmIn;

    LogFlowFunc(("enmStreamCmd=%RU32\n", enmStreamCmd));

    uint32_t uStatus = ASMAtomicReadU32(&pStreamIn->status);
    if (!(   uStatus == CA_STATUS_INIT
          || uStatus == CA_STATUS_REINIT))
    {
        return VINF_SUCCESS;
    }

    int rc = VINF_SUCCESS;

    PCOREAUDIOSTREAM pCAStream = pStreamIn->cbCtx.pStream;
    AssertPtr(pCAStream);

    OSStatus err; RT_NOREF(err);
    switch (enmStreamCmd)
    {
        case PDMAUDIOSTREAMCMD_ENABLE:
        case PDMAUDIOSTREAMCMD_RESUME:
        {
            LogFunc(("Queue enable\n"));
            rc = coreAudioStreamInvalidateQueue(pCAStream);
            if (RT_SUCCESS(rc))
            {
                /* Start the audio queue immediately. */
                AudioQueueStart(pCAStream->audioQueue, NULL);
            }
            break;
        }

        case PDMAUDIOSTREAMCMD_DISABLE:
        {
            LogFunc(("Queue disable\n"));
            AudioQueueStop(pCAStream->audioQueue, 1 /* Immediately */);
            ASMAtomicXchgBool(&pCAStream->fRun,       false);
            ASMAtomicXchgBool(&pCAStream->fIsRunning, false);
            break;
        }

        case PDMAUDIOSTREAMCMD_PAUSE:
        {
            LogFunc(("Queue pause\n"));
            AudioQueuePause(pCAStream->audioQueue);
            ASMAtomicXchgBool(&pCAStream->fIsRunning, false);
            break;
        }

        default:
            rc = VERR_NOT_SUPPORTED;
            break;
    }

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(int) drvHostCoreAudioFiniIn(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMIN pHstStrmIn)
{
    PCOREAUDIOSTREAMIN pStreamIn = (PCOREAUDIOSTREAMIN) pHstStrmIn;

    LogFlowFuncEnter();

    uint32_t status = ASMAtomicReadU32(&pStreamIn->status);
    if (!(   status == CA_STATUS_INIT
          || status == CA_STATUS_REINIT))
    {
        return VINF_SUCCESS;
    }

    OSStatus err = noErr;

    int rc = drvHostCoreAudioControlIn(pInterface, &pStreamIn->streamIn, PDMAUDIOSTREAMCMD_DISABLE);
    if (RT_SUCCESS(rc))
    {
        /*
         * Unregister recording device callbacks.
         */
        AudioObjectPropertyAddress propAdr = { kAudioDeviceProcessorOverload, kAudioObjectPropertyScopeGlobal,
                                               kAudioObjectPropertyElementMaster };
#ifdef DEBUG
        err = AudioObjectRemovePropertyListener(pStreamIn->deviceID, &propAdr,
                                                drvHostCoreAudioDevPropChgCb, &pStreamIn->cbCtx /* pvUser */);
        if (   err != noErr
            && err != kAudioHardwareBadObjectError)
        {
            LogRel(("CoreAudio: Failed to remove the recording processor overload listener (%RI32)\n", err));
        }
#endif /* DEBUG */

        propAdr.mSelector = kAudioDevicePropertyNominalSampleRate;
        err = AudioObjectRemovePropertyListener(pStreamIn->deviceID, &propAdr,
                                                drvHostCoreAudioDevPropChgCb, &pStreamIn->cbCtx /* pvUser */);
        if (   err != noErr
            && err != kAudioHardwareBadObjectError)
        {
            LogRel(("CoreAudio: Failed to remove the recording sample rate changed listener (%RI32)\n", err));
        }

        if (pStreamIn->fDefDevChgListReg)
        {
            propAdr.mSelector = kAudioHardwarePropertyDefaultInputDevice;
            err = AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &propAdr,
                                                    drvHostCoreAudioDefaultDeviceChangedCb, pStreamIn);
            if (   err != noErr
                && err != kAudioHardwareBadObjectError)
            {
                LogRel(("CoreAudio: Failed to remove the default recording device changed listener (%RI32)\n", err));
            }

            pStreamIn->fDefDevChgListReg = false;
        }

        if (pStreamIn->fDevStateChgListReg)
        {
            Assert(pStreamIn->deviceID != kAudioDeviceUnknown);

            AudioObjectPropertyAddress propAdr2 = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal,
                                                    kAudioObjectPropertyElementMaster };
            err = AudioObjectRemovePropertyListener(pStreamIn->deviceID, &propAdr2,
                                                    drvHostCoreAudioDeviceStateChangedCb, &pStreamIn->cbCtx);
            if (   err != noErr
                && err != kAudioHardwareBadObjectError)
            {
                LogRel(("CoreAudio: Failed to remove the recording device state changed listener (%RI32)\n", err));
            }

            pStreamIn->fDevStateChgListReg = false;
        }

        if (RT_SUCCESS(rc))
        {
            PCOREAUDIOSTREAM pCAStream = pStreamIn->cbCtx.pStream;

            rc = coreAudioStreamUninitQueue(pCAStream);
            if (RT_FAILURE(rc))
            {
                LogRel(("CoreAudio: Failed to uninit stream queue: %Rrc)\n", rc));
                return rc;
            }

            if (RTCritSectIsInitialized(&pCAStream->CritSect))
                RTCritSectDelete(&pCAStream->CritSect);
        }

#ifdef VBOX_WITH_AUDIO_CA_CONVERTER
        if (pStreamIn->ConverterRef)
        {
            AudioConverterDispose(pStreamIn->ConverterRef);
            pStreamIn->ConverterRef = NULL;
        }
#endif

        pStreamIn->deviceID      = kAudioDeviceUnknown;

#ifdef VBOX_WITH_AUDIO_CA_CONVERTER
        drvHostCoreAudioUninitConvCbCtx(&pStreamIn->convCbCtx);
#endif
        if (pStreamIn->pCircBuf)
        {
            RTCircBufDestroy(pStreamIn->pCircBuf);
            pStreamIn->pCircBuf = NULL;
        }

        ASMAtomicXchgU32(&pStreamIn->status, CA_STATUS_UNINIT);
    }
    else
    {
        LogRel(("CoreAudio: Failed to stop recording on uninit (%RI32)\n", err));
        rc = VERR_GENERAL_FAILURE; /** @todo Fudge! */
    }

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(int) drvHostCoreAudioFiniOut(PPDMIHOSTAUDIO pInterface, PPDMAUDIOHSTSTRMOUT pHstStrmOut)
{
    PCOREAUDIOSTREAMOUT pStreamOut = (PCOREAUDIOSTREAMOUT)pHstStrmOut;

    LogFlowFuncEnter();

    uint32_t status = ASMAtomicReadU32(&pStreamOut->status);
    if (!(   status == CA_STATUS_INIT
          || status == CA_STATUS_REINIT))
    {
        return VINF_SUCCESS;
    }

    int rc = drvHostCoreAudioControlOut(pInterface, &pStreamOut->streamOut, PDMAUDIOSTREAMCMD_DISABLE);
    if (RT_SUCCESS(rc))
    {
        OSStatus err;

        /*
         * Unregister playback device callbacks.
         */
        AudioObjectPropertyAddress propAdr = { kAudioDeviceProcessorOverload, kAudioObjectPropertyScopeGlobal,
                                               kAudioObjectPropertyElementMaster };
#ifdef DEBUG
        err = AudioObjectRemovePropertyListener(pStreamOut->deviceID, &propAdr,
                                                drvHostCoreAudioDevPropChgCb, &pStreamOut->cbCtx /* pvUser */);
        if (   err != noErr
            && err != kAudioHardwareBadObjectError)
        {
            LogRel(("CoreAudio: Failed to remove the playback processor overload listener (%RI32)\n", err));
        }
#endif /* DEBUG */

        propAdr.mSelector = kAudioDevicePropertyNominalSampleRate;
        err = AudioObjectRemovePropertyListener(pStreamOut->deviceID, &propAdr,
                                                drvHostCoreAudioDevPropChgCb, &pStreamOut->cbCtx /* pvUser */);
        if (   err != noErr
            && err != kAudioHardwareBadObjectError)
        {
            LogRel(("CoreAudio: Failed to remove the playback sample rate changed listener (%RI32)\n", err));
        }

        if (pStreamOut->fDefDevChgListReg)
        {
            propAdr.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
            propAdr.mScope    = kAudioObjectPropertyScopeGlobal;
            propAdr.mElement  = kAudioObjectPropertyElementMaster;
            err = AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &propAdr,
                                                    drvHostCoreAudioDevPropChgCb, &pStreamOut->cbCtx /* pvUser */);
            if (   err != noErr
                && err != kAudioHardwareBadObjectError)
            {
                LogRel(("CoreAudio: Failed to remove the default playback device changed listener (%RI32)\n", err));
            }

            pStreamOut->fDefDevChgListReg = false;
        }

        if (pStreamOut->fDevStateChgListReg)
        {
            Assert(pStreamOut->deviceID != kAudioDeviceUnknown);

            AudioObjectPropertyAddress propAdr2 = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal,
                                                    kAudioObjectPropertyElementMaster };
            err = AudioObjectRemovePropertyListener(pStreamOut->deviceID, &propAdr2,
                                                    drvHostCoreAudioDeviceStateChangedCb, &pStreamOut->cbCtx);
            if (   err != noErr
                && err != kAudioHardwareBadObjectError)
            {
                LogRel(("CoreAudio: Failed to remove the playback device state changed listener (%RI32)\n", err));
            }

            pStreamOut->fDevStateChgListReg = false;
        }

        if (RT_SUCCESS(rc))
        {
            PCOREAUDIOSTREAM pCAStream = pStreamOut->cbCtx.pStream;

            rc = coreAudioStreamUninitQueue(pCAStream);
            if (RT_FAILURE(rc))
            {
                LogRel(("CoreAudio: Failed to uninit stream queue: %Rrc)\n", rc));
                return rc;
            }

            if (RTCritSectIsInitialized(&pCAStream->CritSect))
                RTCritSectDelete(&pCAStream->CritSect);
        }

        pStreamOut->deviceID  = kAudioDeviceUnknown;
        if (pStreamOut->pCircBuf)
        {
            RTCircBufDestroy(pStreamOut->pCircBuf);
            pStreamOut->pCircBuf = NULL;
        }

        ASMAtomicXchgU32(&pStreamOut->status, CA_STATUS_UNINIT);
    }
    else
        LogRel(("CoreAudio: Failed to stop playback on uninit, rc=%Rrc\n", rc));

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(int) drvHostCoreAudioInitIn(PPDMIHOSTAUDIO pInterface,
                                                PPDMAUDIOHSTSTRMIN pHstStrmIn,
                                                PPDMAUDIOSTREAMCFG pCfgReq, PPDMAUDIOSTREAMCFG pCfgAcq,
                                                PDMAUDIORECSOURCE enmRecSource, uint32_t *pcSamples)
{
    RT_NOREF(enmRecSource);
    PPDMDRVINS        pDrvIns = PDMIBASE_2_PDMDRV(pInterface);
    PDRVHOSTCOREAUDIO pThis   = PDMINS_2_DATA(pDrvIns, PDRVHOSTCOREAUDIO);

    PCOREAUDIOSTREAMIN pStreamIn = (PCOREAUDIOSTREAMIN)pHstStrmIn;

    LogFlowFunc(("enmRecSource=%RU32\n", enmRecSource));

    pStreamIn->deviceID            = kAudioDeviceUnknown;
#ifdef VBOX_WITH_AUDIO_CA_CONVERTER
    pStreamIn->ConverterRef        = NULL;
#endif
    pStreamIn->pCircBuf            = NULL;
    pStreamIn->status              = CA_STATUS_UNINIT;
    pStreamIn->fDefDevChgListReg   = false;
    pStreamIn->fDevStateChgListReg = false;

    /* Set attributes. */
    pStreamIn->Stream.enmDir = PDMAUDIODIR_IN;
    pStreamIn->Stream.pIn    = pStreamIn;

    /* Set callback context. */
    pStreamIn->cbCtx.pThis   = pThis;
    pStreamIn->cbCtx.pStream = &pStreamIn->Stream;

    int rc;

    PCOREAUDIOSTREAM pCAStream = pStreamIn->cbCtx.pStream;

    rc = RTCritSectInit(&pCAStream->CritSect);
    if (RT_FAILURE(rc))
        return rc;

    pCAStream->hThread    = NIL_RTTHREAD;
    pCAStream->fRun       = false;
    pCAStream->fIsRunning = false;

    bool fDeviceByUser = false; /* Do we use a device which was set by the user? */
#if 0
    /* Try to find the audio device set by the user */
    if (DeviceUID.pszInputDeviceUID)
    {
        pStreamIn->deviceID = drvHostCoreAudioDeviceUIDtoID(DeviceUID.pszInputDeviceUID);
        /* Not fatal */
        if (pStreamIn->deviceID == kAudioDeviceUnknown)
            LogRel(("CoreAudio: Unable to find recording device %s. Falling back to the default audio device. \n", DeviceUID.pszInputDeviceUID));
        else
            fDeviceByUser = true;
    }
#endif

    rc = coreAudioStreamInitQueue(pStreamIn->cbCtx.pStream, true /* fIn */, pCfgReq, pCfgAcq);
    if (RT_SUCCESS(rc))
    {
        *pcSamples = AQ_BUF_SAMPLES;
    }
    if (RT_SUCCESS(rc))
    {
        ASMAtomicXchgU32(&pStreamIn->status, CA_STATUS_INIT);

        OSStatus err;

        /* When the devices isn't forced by the user, we want default device change notifications. */
        if (!fDeviceByUser)
        {
            if (!pStreamIn->fDefDevChgListReg)
            {
                AudioObjectPropertyAddress propAdr = { kAudioHardwarePropertyDefaultInputDevice, kAudioObjectPropertyScopeGlobal,
                                                       kAudioObjectPropertyElementMaster };
                err = AudioObjectAddPropertyListener(kAudioObjectSystemObject, &propAdr,
                                                     drvHostCoreAudioDefaultDeviceChangedCb, &pStreamIn->cbCtx);
                if (   err == noErr
                    || err == kAudioHardwareIllegalOperationError)
                {
                    pStreamIn->fDefDevChgListReg = true;
                }
                else
                    LogRel(("CoreAudio: Failed to add the default recording device changed listener (%RI32)\n", err));
            }
        }

        if (   !pStreamIn->fDevStateChgListReg
            && (pStreamIn->deviceID != kAudioDeviceUnknown))
        {
            /* Register callback for being notified if the device stops being alive. */
            AudioObjectPropertyAddress propAdr = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal,
                                                   kAudioObjectPropertyElementMaster };
            err = AudioObjectAddPropertyListener(pStreamIn->deviceID, &propAdr, drvHostCoreAudioDeviceStateChangedCb,
                                                 &pStreamIn->cbCtx);
            if (err == noErr)
            {
                pStreamIn->fDevStateChgListReg = true;
            }
            else
                LogRel(("CoreAudio: Failed to add the recording device state changed listener (%RI32)\n", err));
        }
    }

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(int) drvHostCoreAudioInitOut(PPDMIHOSTAUDIO pInterface,
                                                 PPDMAUDIOHSTSTRMOUT pHstStrmOut,
                                                 PPDMAUDIOSTREAMCFG pCfgReq, PPDMAUDIOSTREAMCFG pCfgAcq,
                                                 uint32_t *pcSamples)
{
    PPDMDRVINS        pDrvIns = PDMIBASE_2_PDMDRV(pInterface);
    PDRVHOSTCOREAUDIO pThis   = PDMINS_2_DATA(pDrvIns, PDRVHOSTCOREAUDIO);

    PCOREAUDIOSTREAMOUT pStreamOut = (PCOREAUDIOSTREAMOUT)pHstStrmOut;

    LogFlowFuncEnter();

    pStreamOut->deviceID            = kAudioDeviceUnknown;
    pStreamOut->pCircBuf            = NULL;
    pStreamOut->status              = CA_STATUS_UNINIT;
    pStreamOut->fDefDevChgListReg   = false;
    pStreamOut->fDevStateChgListReg = false;

    /* Set attributes. */
    pStreamOut->Stream.enmDir = PDMAUDIODIR_OUT;
    pStreamOut->Stream.pOut   = pStreamOut;

    /* Set callback context. */
    pStreamOut->cbCtx.pThis   = pThis;
    pStreamOut->cbCtx.pStream = &pStreamOut->Stream;

    PCOREAUDIOSTREAM pCAStream = pStreamOut->cbCtx.pStream;
    
    int rc = RTCritSectInit(&pCAStream->CritSect);
    if (RT_FAILURE(rc))
        return rc;

    pCAStream->hThread    = NIL_RTTHREAD;
    pCAStream->fRun       = false;
    pCAStream->fIsRunning = false;

    bool fDeviceByUser = false; /* Do we use a device which was set by the user? */

#if 0
    /* Try to find the audio device set by the user. Use
     * export VBOX_COREAUDIO_OUTPUT_DEVICE_UID=AppleHDAEngineOutput:0
     * to set it. */
    if (DeviceUID.pszOutputDeviceUID)
    {
        pStreamOut->audioDeviceId = drvHostCoreAudioDeviceUIDtoID(DeviceUID.pszOutputDeviceUID);
        /* Not fatal */
        if (pStreamOut->audioDeviceId == kAudioDeviceUnknown)
            LogRel(("CoreAudio: Unable to find playback device %s. Falling back to the default audio device. \n", DeviceUID.pszOutputDeviceUID));
        else
            fDeviceByUser = true;
    }
#endif

    rc = coreAudioStreamInitQueue(pStreamOut->cbCtx.pStream, false /* fIn */, pCfgReq, pCfgAcq);
    if (RT_SUCCESS(rc))
    {
        *pcSamples = AQ_BUF_SAMPLES;
    }
    if (RT_SUCCESS(rc))
    {
        ASMAtomicXchgU32(&pStreamOut->status, CA_STATUS_INIT);

        OSStatus err;

        /* When the devices isn't forced by the user, we want default device change notifications. */
        if (!fDeviceByUser)
        {
            AudioObjectPropertyAddress propAdr = { kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal,
                                                   kAudioObjectPropertyElementMaster };
            err = AudioObjectAddPropertyListener(kAudioObjectSystemObject, &propAdr,
                                                 drvHostCoreAudioDefaultDeviceChangedCb, &pStreamOut->cbCtx);
            if (err == noErr)
            {
                pStreamOut->fDefDevChgListReg = true;
            }
            else
                LogRel(("CoreAudio: Failed to add the default playback device changed listener (%RI32)\n", err));
        }

        if (   !pStreamOut->fDevStateChgListReg
            && (pStreamOut->deviceID != kAudioDeviceUnknown))
        {
            /* Register callback for being notified if the device stops being alive. */
            AudioObjectPropertyAddress propAdr = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal,
                                                   kAudioObjectPropertyElementMaster };
            err = AudioObjectAddPropertyListener(pStreamOut->deviceID, &propAdr, drvHostCoreAudioDeviceStateChangedCb,
                                                 (void *)&pStreamOut->cbCtx);
            if (err == noErr)
            {
                pStreamOut->fDevStateChgListReg = true;
            }
            else
                LogRel(("CoreAudio: Failed to add the playback device state changed listener (%RI32)\n", err));
        }
    }

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(bool) drvHostCoreAudioIsEnabled(PPDMIHOSTAUDIO pInterface, PDMAUDIODIR enmDir)
{
    NOREF(pInterface);
    NOREF(enmDir);
    return true; /* Always all enabled. */
}

static DECLCALLBACK(int) drvHostCoreAudioGetConf(PPDMIHOSTAUDIO pInterface, PPDMAUDIOBACKENDCFG pCfg)
{
    PPDMDRVINS        pDrvIns = PDMIBASE_2_PDMDRV(pInterface);
    PDRVHOSTCOREAUDIO pThis   = PDMINS_2_DATA(pDrvIns, PDRVHOSTCOREAUDIO);

    return coreAudioUpdateStatusInternalEx(pThis, pCfg, 0 /* fEnum */);
}

static DECLCALLBACK(void) drvHostCoreAudioShutdown(PPDMIHOSTAUDIO pInterface)
{
    NOREF(pInterface);
}

static DECLCALLBACK(void *) drvHostCoreAudioQueryInterface(PPDMIBASE pInterface, const char *pszIID)
{
    PPDMDRVINS        pDrvIns = PDMIBASE_2_PDMDRV(pInterface);
    PDRVHOSTCOREAUDIO pThis   = PDMINS_2_DATA(pDrvIns, PDRVHOSTCOREAUDIO);

    PDMIBASE_RETURN_INTERFACE(pszIID, PDMIBASE, &pDrvIns->IBase);
    PDMIBASE_RETURN_INTERFACE(pszIID, PDMIHOSTAUDIO, &pThis->IHostAudio);

    return NULL;
}

 /* Construct a DirectSound Audio driver instance.
 *
 * @copydoc FNPDMDRVCONSTRUCT
 */
static DECLCALLBACK(int) drvHostCoreAudioConstruct(PPDMDRVINS pDrvIns, PCFGMNODE pCfg, uint32_t fFlags)
{
    RT_NOREF(pCfg, fFlags);
    PDRVHOSTCOREAUDIO pThis = PDMINS_2_DATA(pDrvIns, PDRVHOSTCOREAUDIO);
    LogRel(("Audio: Initializing Core Audio driver\n"));

    /*
     * Init the static parts.
     */
    pThis->pDrvIns                   = pDrvIns;
    /* IBase */
    pDrvIns->IBase.pfnQueryInterface = drvHostCoreAudioQueryInterface;
    /* IHostAudio */
    PDMAUDIO_IHOSTAUDIO_CALLBACKS(drvHostCoreAudio);

    return VINF_SUCCESS;
}

/**
 * Char driver registration record.
 */
const PDMDRVREG g_DrvHostCoreAudio =
{
    /* u32Version */
    PDM_DRVREG_VERSION,
    /* szName */
    "CoreAudio",
    /* szRCMod */
    "",
    /* szR0Mod */
    "",
    /* pszDescription */
    "Core Audio host driver",
    /* fFlags */
     PDM_DRVREG_FLAGS_HOST_BITS_DEFAULT,
    /* fClass. */
    PDM_DRVREG_CLASS_AUDIO,
    /* cMaxInstances */
    ~0U,
    /* cbInstance */
    sizeof(DRVHOSTCOREAUDIO),
    /* pfnConstruct */
    drvHostCoreAudioConstruct,
    /* pfnDestruct */
    NULL,
    /* pfnRelocate */
    NULL,
    /* pfnIOCtl */
    NULL,
    /* pfnPowerOn */
    NULL,
    /* pfnReset */
    NULL,
    /* pfnSuspend */
    NULL,
    /* pfnResume */
    NULL,
    /* pfnAttach */
    NULL,
    /* pfnDetach */
    NULL,
    /* pfnPowerOff */
    NULL,
    /* pfnSoftReset */
    NULL,
    /* u32EndVersion */
    PDM_DRVREG_VERSION
};

