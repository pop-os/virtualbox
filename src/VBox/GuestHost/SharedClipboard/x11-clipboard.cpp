/** @file
 *
 * Shared Clipboard:
 * X11 backend code.
 */

/*
 * Copyright (C) 2006-2007 Sun Microsystems, Inc.
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 */

/* Note: to automatically run regression tests on the shared clipboard, set
 * the make variable VBOX_RUN_X11_CLIPBOARD_TEST=1 while building.  If you
 * often make changes to the clipboard code, setting this variable in
 * LocalConfig.kmk will cause the tests to be run every time the code is
 * changed. */

#define LOG_GROUP LOG_GROUP_SHARED_CLIPBOARD

#include <errno.h>
#include <vector>

#include <unistd.h>

#ifdef RT_OS_SOLARIS
#include <tsol/label.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xproto.h>
#include <X11/StringDefs.h>

#include <iprt/env.h>
#include <iprt/mem.h>
#include <iprt/semaphore.h>
#include <iprt/thread.h>

#include <VBox/log.h>

#include <VBox/GuestHost/SharedClipboard.h>
#include <VBox/GuestHost/clipboard-helper.h>
#include <VBox/HostServices/VBoxClipboardSvc.h>

/** Do we want to test Utf16 by disabling other text formats? */
static bool g_testUtf16 = false;
/** Do we want to test Utf8 by disabling other text formats? */
static bool g_testUtf8 = false;
/** Do we want to test compount text by disabling other text formats? */
static bool g_testCText = false;
/** Are we currently debugging the clipboard code? */
static bool g_debugClipboard = false;

/** The different clipboard formats which we support. */
enum g_eClipboardFormats
{
    INVALID = 0,
    TARGETS,
    CTEXT,
    UTF8,
    UTF16
};

/** The X11 clipboard uses several names for the same format.  This
 * structure maps an X11 name to a format. */
typedef struct {
    Atom atom;
    g_eClipboardFormats format;
    unsigned guestFormat;
} VBOXCLIPBOARDFORMAT;

/** Global context information used by the X11 clipboard backend */
struct _VBOXCLIPBOARDCONTEXTX11
{
    /** Opaque data structure describing the front-end. */
    VBOXCLIPBOARDCONTEXT *pFrontend;
    /** The X Toolkit application context structure */
    XtAppContext appContext;

    /** We have a separate thread to wait for Window and Clipboard events */
    RTTHREAD thread;
    /** The X Toolkit widget which we use as our clipboard client.  It is never made visible. */
    Widget widget;

    /** X11 atom refering to the clipboard: CLIPBOARD */
    Atom atomClipboard;
    /** X11 atom refering to the selection: PRIMARY */
    Atom atomPrimary;
    /** X11 atom refering to the clipboard targets: TARGETS */
    Atom atomTargets;
    /** X11 atom refering to the clipboard multiple target: MULTIPLE */
    Atom atomMultiple;
    /** X11 atom refering to the clipboard timestamp target: TIMESTAMP */
    Atom atomTimestamp;
    /** X11 atom refering to the clipboard utf16 text format: text/plain;charset=ISO-10646-UCS-2 */
    Atom atomUtf16;
    /** X11 atom refering to the clipboard utf8 text format: UTF8_STRING */
    Atom atomUtf8;
    /** X11 atom refering to the clipboard compound text format: COMPOUND_TEXT */
    Atom atomCText;

    /** A list of the X11 formats which we support, mapped to our identifier for them, in the
        order we prefer to have them in. */
    std::vector<VBOXCLIPBOARDFORMAT> formatList;

    /** Does VBox or X11 currently own the clipboard? */
    volatile enum g_eOwner eOwner;

    /** What is the best text format X11 has to offer?  INVALID for none. */
    g_eClipboardFormats X11TextFormat;
    /** Atom corresponding to the X11 text format */
    Atom atomX11TextFormat;
    /** What is the best bitmap format X11 has to offer?  INVALID for none. */
    g_eClipboardFormats X11BitmapFormat;
    /** Atom corresponding to the X11 Bitmap format */
    Atom atomX11BitmapFormat;
    /** What formats does VBox have on offer? */
    int vboxFormats;
    /** Windows hosts and guests cache the clipboard data they receive.
     * Since we have no way of knowing whether their cache is still valid,
     * we always send a "data changed" message after a successful transfer
     * to invalidate it. */
    bool notifyVBox;

    /** Since the clipboard data moves asynchronously, we use an event
     * semaphore to wait for it.  When a function issues a request for
     * clipboard data it must wait for this semaphore, which is triggered
     * when the data arrives. */
    RTSEMEVENT waitForData;
    /** When we wish the clipboard to exit, we have to wake up the event
     * loop.  We do this by writing into a pipe.  This end of the pipe is
     * the end that another thread can write to. */
    int wakeupPipeWrite;
    /** The reader end of the pipe */
    int wakeupPipeRead;
};

/* Only one client is supported. There seems to be no need for more clients. 
 */
static VBOXCLIPBOARDCONTEXTX11 g_ctxX11;
static VBOXCLIPBOARDCONTEXTX11 *g_pCtx;

/* Are we actually connected to the X server? */
static bool g_fHaveX11;

/** Convert an atom name string to an X11 atom, looking it up in a cache
 * before asking the server */
static Atom clipGetAtom(Widget widget, const char *pszName)
{
    AssertPtrReturn(pszName, None);
    Atom retval = None;
    XrmValue nameVal, atomVal;
    nameVal.addr = (char *) pszName;
    nameVal.size = strlen(pszName);
    atomVal.size = sizeof(Atom);
    atomVal.addr = (char *) &retval;
    XtConvertAndStore(widget, XtRString, &nameVal, XtRAtom, &atomVal);
    return retval;
}

/**
 * Convert the UTF-16 text obtained from the X11 clipboard to UTF-16LE with
 * Windows EOLs, place it in the buffer supplied and signal that data has
 * arrived.
 *
 * @param pValue      Source UTF-16 text
 * @param cwSourceLen Length in 16-bit words of the source text
 * @param pv          Where to store the converted data
 * @param cb          Length in bytes of the buffer pointed to by cb
 * @param pcbActual   Where to store the size of the converted data
 * @param pClient     Pointer to the client context structure
 * @note  X11 backend code, called from the Xt callback when we wish to read
 *        the X11 clipboard.
 */
static void vboxClipboardGetUtf16(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                  XtPointer pValue, unsigned cwSrcLen,
                                  void *pv, unsigned cb,
                                  uint32_t *pcbActual)
{
    size_t cwDestLen;
    PRTUTF16 pu16SrcText = reinterpret_cast<PRTUTF16>(pValue);
    PRTUTF16 pu16DestText = reinterpret_cast<PRTUTF16>(pv);

    LogFlowFunc (("converting Utf-16 to Utf-16LE.  cwSrcLen=%d, cb=%d, pu16SrcText+1=%.*ls\n",
                   cwSrcLen, cb, cwSrcLen - 1, pu16SrcText + 1));
    *pcbActual = 0;  /* Only set this to the right value on success. */
    /* How long will the converted text be? */
    int rc = vboxClipboardUtf16GetWinSize(pu16SrcText, cwSrcLen + 1,
                                          &cwDestLen);
    if (RT_SUCCESS(rc) && (cb < cwDestLen * 2))
    {
        /* Not enough buffer space provided - report the amount needed. */
        LogFlowFunc (("guest buffer too small: size %d bytes, needed %d.  Returning.\n",
                       cb, cwDestLen * 2));
        *pcbActual = cwDestLen * 2;
        rc = VERR_BUFFER_OVERFLOW;
    }
    /* Convert the text. */
    if (RT_SUCCESS(rc))
        rc = vboxClipboardUtf16LinToWin(pu16SrcText, cwSrcLen + 1,
                                        pu16DestText, cb / 2);
    if (RT_SUCCESS(rc))
    {
        LogFlowFunc (("converted string is %.*ls\n", cwDestLen, pu16DestText));
        *pcbActual = cwDestLen * 2;
    }
    /* We need to do this whether we succeed or fail. */
    XtFree(reinterpret_cast<char *>(pValue));
    RTSemEventSignal(pCtx->waitForData);
    LogFlowFunc(("Returning.  Status is %Rrc\n", rc));
}

/**
 * Convert the UTF-8 text obtained from the X11 clipboard to UTF-16LE with
 * Windows EOLs, place it in the buffer supplied and signal that data has
 * arrived.
 *
 * @param pValue      Source UTF-8 text
 * @param cbSourceLen Length in 8-bit bytes of the source text
 * @param pv          Where to store the converted data
 * @param cb          Length in bytes of the buffer pointed to by pv
 * @param pcbActual   Where to store the size of the converted data
 * @param pClient     Pointer to the client context structure
 * @note  X11 backend code, called from the Xt callback when we wish to read
 *        the X11 clipboard.
 */
static void vboxClipboardGetUtf8FromX11(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                        XtPointer pValue, unsigned cbSrcLen,
                                        void *pv, unsigned cb,
                                        uint32_t *pcbActual)
{
    size_t cwSrcLen, cwDestLen;
    char *pu8SrcText = reinterpret_cast<char *>(pValue);
    PRTUTF16 pu16SrcText = NULL;
    PRTUTF16 pu16DestText = reinterpret_cast<PRTUTF16>(pv);

    LogFlowFunc (("converting Utf-8 to Utf-16LE.  cbSrcLen=%d, cb=%d, pu8SrcText=%.*s\n",
                   cbSrcLen, cb, cbSrcLen, pu8SrcText));
    *pcbActual = 0;  /* Only set this to the right value on success. */
    /* First convert the UTF8 to UTF16 */
    int rc = RTStrToUtf16Ex(pu8SrcText, cbSrcLen, &pu16SrcText, 0, &cwSrcLen);
    /* Check how much longer will the converted text will be. */
    if (RT_SUCCESS(rc))
        rc = vboxClipboardUtf16GetWinSize(pu16SrcText, cwSrcLen + 1,
                                          &cwDestLen);
    if (RT_SUCCESS(rc) && (cb < cwDestLen * 2))
    {
        /* Not enough buffer space provided - report the amount needed. */
        LogFlowFunc (("guest buffer too small: size %d bytes, needed %d.  Returning.\n",
                       cb, cwDestLen * 2));
        *pcbActual = cwDestLen * 2;
        rc = VERR_BUFFER_OVERFLOW;
    }
    /* Convert the text. */
    if (RT_SUCCESS(rc))
        rc = vboxClipboardUtf16LinToWin(pu16SrcText, cwSrcLen + 1,
                                        pu16DestText, cb / 2);
    if (RT_SUCCESS(rc))
    {
        LogFlowFunc (("converted string is %.*ls.\n", cwDestLen, pu16DestText));
        *pcbActual = cwDestLen * 2;
    }
    XtFree(reinterpret_cast<char *>(pValue));
    RTUtf16Free(pu16SrcText);
    RTSemEventSignal(pCtx->waitForData);
    LogFlowFunc(("Returning.  Status is %Rrc", rc));
}

/**
 * Convert the COMPOUND_TEXT obtained from the X11 clipboard to UTF-16LE with
 * Windows EOLs, place it in the buffer supplied and signal that data has
 * arrived.
 *
 * @param pValue      Source COMPOUND_TEXT
 * @param cbSourceLen Length in 8-bit bytes of the source text
 * @param pv          Where to store the converted data
 * @param cb          Length in bytes of the buffer pointed to by pv
 * @param pcbActual   Where to store the size of the converted data
 * @param pClient     Pointer to the client context structure
 * @note  X11 backend code, called from the Xt callback when we wish to read
 *        the X11 clipboard.
 */
static void vboxClipboardGetCTextFromX11(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                         XtPointer pValue, unsigned cbSrcLen,
                                         void *pv, unsigned cb,
                                         uint32_t *pcbActual)
{
    size_t cwSrcLen, cwDestLen;
    char **ppu8SrcText = NULL;
    PRTUTF16 pu16SrcText = NULL;
    PRTUTF16 pu16DestText = reinterpret_cast<PRTUTF16>(pv);
    XTextProperty property;
    int rc = VINF_SUCCESS;
    int cProps;

    LogFlowFunc (("converting COMPOUND TEXT to Utf-16LE.  cbSrcLen=%d, cb=%d, pu8SrcText=%.*s\n",
                   cbSrcLen, cb, cbSrcLen, reinterpret_cast<char *>(pValue)));
    *pcbActual = 0;  /* Only set this to the right value on success. */
    /** @todo quick fix for 2.2, do this properly. */
    if (cbSrcLen == 0)
    {
        XtFree(reinterpret_cast<char *>(pValue));
        if (cb < 2)
            return;
        *(PRTUTF16) pv = 0;
        *pcbActual = 2;
        return;
    }
    /* First convert the compound text to Utf8 */
    property.value = reinterpret_cast<unsigned char *>(pValue);
    property.encoding = pCtx->atomCText;
    property.format = 8;
    property.nitems = cbSrcLen;
#ifdef RT_OS_SOLARIS
    int xrc = XmbTextPropertyToTextList(XtDisplay(pCtx->widget), &property,
                                        &ppu8SrcText, &cProps);
#else
    int xrc = Xutf8TextPropertyToTextList(XtDisplay(pCtx->widget),
                                          &property, &ppu8SrcText, &cProps);
#endif
    XtFree(reinterpret_cast<char *>(pValue));
    if (xrc < 0)
        switch(xrc)
        {
        case XNoMemory:
            rc = VERR_NO_MEMORY;
            break;
        case XLocaleNotSupported:
        case XConverterNotFound:
            rc = VERR_NOT_SUPPORTED;
            break;
        default:
            rc = VERR_UNRESOLVED_ERROR;
        }
    /* Now convert the UTF8 to UTF16 */
    if (RT_SUCCESS(rc))
        rc = RTStrToUtf16Ex(*ppu8SrcText, cbSrcLen, &pu16SrcText, 0, &cwSrcLen);
    /* Check how much longer will the converted text will be. */
    if (RT_SUCCESS(rc))
        rc = vboxClipboardUtf16GetWinSize(pu16SrcText, cwSrcLen + 1,
                                          &cwDestLen);
    if (RT_SUCCESS(rc) && (cb < cwDestLen * 2))
    {
        /* Not enough buffer space provided - report the amount needed. */
        LogFlowFunc (("guest buffer too small: size %d bytes, needed %d.  Returning.\n",
                       cb, cwDestLen * 2));
        *pcbActual = cwDestLen * 2;
        rc = VERR_BUFFER_OVERFLOW;
    }
    /* Convert the text. */
    if (RT_SUCCESS(rc))
        rc = vboxClipboardUtf16LinToWin(pu16SrcText, cwSrcLen + 1,
                                        pu16DestText, cb / 2);
    if (RT_SUCCESS(rc))
    {
        LogFlowFunc (("converted string is %.*ls\n", cwDestLen, pu16DestText));
        *pcbActual = cwDestLen * 2;
    }
    if (ppu8SrcText != NULL)
        XFreeStringList(ppu8SrcText);
    RTUtf16Free(pu16SrcText);
    LogFlowFunc(("Returning.  Status is %Rrc\n", rc));
    RTSemEventSignal(pCtx->waitForData);
}

/**
 * Convert the Latin1 text obtained from the X11 clipboard to UTF-16LE with
 * Windows EOLs, place it in the buffer supplied and signal that data has
 * arrived.
 *
 * @param pValue      Source Latin1 text
 * @param cbSourceLen Length in 8-bit bytes of the source text
 * @param pv          Where to store the converted data
 * @param cb          Length in bytes of the buffer pointed to by cb
 * @param pcbActual   Where to store the size of the converted data
 * @param pClient     Pointer to the client context structure
 * @note  X11 backend code, called from the Xt callback when we wish to read
 *        the X11 clipboard.
 */
static void vboxClipboardGetLatin1FromX11(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                          XtPointer pValue,
                                          unsigned cbSourceLen, void *pv,
                                          unsigned cb, uint32_t *pcbActual)
{
    char *pu8SourceText = reinterpret_cast<char *>(pValue);
    PRTUTF16 pu16DestText = reinterpret_cast<PRTUTF16>(pv);
    int rc = VINF_SUCCESS;

    LogFlowFunc (("converting Latin1 to Utf-16LE.  Original is %.*s\n",
                  cbSourceLen, pu8SourceText));
    *pcbActual = 0;  /* Only set this to the right value on success. */
    unsigned cwDestLen = 0;
    for (unsigned i = 0; i < cbSourceLen && pu8SourceText[i] != '\0'; i++)
    {
        ++cwDestLen;
        if (pu8SourceText[i] == LINEFEED)
            ++cwDestLen;
    }
    /* Leave space for the terminator */
    ++cwDestLen;
    if (cb < cwDestLen * 2)
    {
        /* Not enough buffer space provided - report the amount needed. */
        LogFlowFunc (("guest buffer too small: size %d bytes\n", cb));
        *pcbActual = cwDestLen * 2;
        rc = VERR_BUFFER_OVERFLOW;
    }
    if (RT_SUCCESS(rc))
    {
        for (unsigned i = 0, j = 0; i < cbSourceLen; ++i, ++j)
            if (pu8SourceText[i] != LINEFEED)
                pu16DestText[j] = pu8SourceText[i];  /* latin1 < utf-16LE */
            else
            {
                pu16DestText[j] = CARRIAGERETURN;
                ++j;
                pu16DestText[j] = LINEFEED;
            }
        pu16DestText[cwDestLen - 1] = 0;
        *pcbActual = cwDestLen * 2;
        LogFlowFunc (("converted text is %.*ls\n", cwDestLen, pu16DestText));
    }
    XtFree(reinterpret_cast<char *>(pValue));
    RTSemEventSignal(pCtx->waitForData);
    LogFlowFunc(("Returning.  Status is %Rrc\n", rc));
}

/**
 * Convert the text obtained from the X11 clipboard to UTF-16LE with Windows
 * EOLs, place it in the buffer supplied and signal that data has arrived.
 * @note  X11 backend code, callback for XtGetSelectionValue, for use when
 *        the X11 clipboard contains a text format we understand.
 */
static void vboxClipboardGetDataFromX11(Widget, XtPointer pClientData,
                                        Atom * /* selection */,
                                        Atom *atomType,
                                        XtPointer pValue,
                                        long unsigned int *pcLen,
                                        int *piFormat)
{
    VBOXCLIPBOARDREQUEST *pRequest
        = reinterpret_cast<VBOXCLIPBOARDREQUEST *>(pClientData);
    VBOXCLIPBOARDCONTEXTX11 *pCtx = pRequest->pCtx;
    if (pCtx->eOwner == VB)
    {
        /* We don't want to request data from ourselves! */
        RTSemEventSignal(pCtx->waitForData);
        return;
    }
    LogFlowFunc(("pClientData=%p, *pcLen=%lu, *piFormat=%d\n", pClientData,
                 *pcLen, *piFormat));
    LogFlowFunc(("pCtx->X11TextFormat=%d, pRequest->cb=%d\n",
                 pCtx->X11TextFormat, pRequest->cb));
    unsigned cTextLen = (*pcLen) * (*piFormat) / 8;
    /* The X Toolkit may have failed to get the clipboard selection for us. */
    if (*atomType == XT_CONVERT_FAIL) /* timeout */
    {
        RTSemEventSignal(pCtx->waitForData);
        return;
    }
    /* The clipboard selection may have changed before we could get it. */
    if (NULL == pValue)
    {
        RTSemEventSignal(pCtx->waitForData);
        return;
    }
    /* In which format is the clipboard data? */
    switch (pCtx->X11TextFormat)
    {
    case UTF16:
        vboxClipboardGetUtf16(pCtx, pValue, cTextLen / 2, pRequest->pv,
                              pRequest->cb, pRequest->pcbActual);
        break;
    case CTEXT:
        vboxClipboardGetCTextFromX11(pCtx, pValue, cTextLen, pRequest->pv,
                                     pRequest->cb, pRequest->pcbActual);
        break;
    case UTF8:
    {
        /* If we are given broken Utf-8, we treat it as Latin1.  Is this acceptable? */
        size_t cStringLen;
        char *pu8SourceText = reinterpret_cast<char *>(pValue);

        if ((pCtx->X11TextFormat == UTF8)
            && (RTStrUniLenEx(pu8SourceText, *pcLen, &cStringLen) == VINF_SUCCESS))
        {
            vboxClipboardGetUtf8FromX11(pCtx, pValue, cTextLen, pRequest->pv,
                                     pRequest->cb, pRequest->pcbActual);
            break;
        }
        else
        {
            vboxClipboardGetLatin1FromX11(pCtx, pValue, cTextLen,
                                          pRequest->pv, pRequest->cb,
                                          pRequest->pcbActual);
            break;
        }
    }
    default:
        LogFunc (("bad target format\n"));
        XtFree(reinterpret_cast<char *>(pValue));
        RTSemEventSignal(pCtx->waitForData);
        return;
    }
    pCtx->notifyVBox = true;
}

/**
 * Notify the host clipboard about the data formats we support, based on the
 * "targets" (available data formats) information obtained from the X11
 * clipboard.
 * @note  X11 backend code, callback for XtGetSelectionValue, called when we
 *        poll for available targets.
 */
static void vboxClipboardGetTargetsFromX11(Widget,
                                           XtPointer pClientData,
                                           Atom * /* selection */,
                                           Atom *atomType,
                                           XtPointer pValue,
                                           long unsigned int *pcLen,
                                           int *piFormat)
{
    VBOXCLIPBOARDCONTEXTX11 *pCtx =
            reinterpret_cast<VBOXCLIPBOARDCONTEXTX11 *>(pClientData);
    Atom *atomTargets = reinterpret_cast<Atom *>(pValue);
    unsigned cAtoms = *pcLen;
    g_eClipboardFormats eBestTarget = INVALID;
    Atom atomBestTarget = None;

    Log3 (("%s: called\n", __PRETTY_FUNCTION__));
    if (   *atomType == XT_CONVERT_FAIL /* timeout */
        || pCtx->eOwner == VB           /* VBox currently owns the clipb. */)
    {
        pCtx->atomX11TextFormat = None;
        pCtx->X11TextFormat = INVALID;
        return;
    }

    for (unsigned i = 0; i < cAtoms; ++i)
    {
        for (unsigned j = 0; j != pCtx->formatList.size(); ++j)
            if (pCtx->formatList[j].atom == atomTargets[i])
            {
                if (eBestTarget < pCtx->formatList[j].format)
                {
                    eBestTarget = pCtx->formatList[j].format;
                    atomBestTarget = pCtx->formatList[j].atom;
                }
                break;
            }
        if (g_debugClipboard)
        {
            char *szAtomName = XGetAtomName(XtDisplay(pCtx->widget),
                                            atomTargets[i]);
            if (szAtomName != 0)
            {
                Log2 (("%s: the host offers target %s\n", __PRETTY_FUNCTION__,
                       szAtomName));
                XFree(szAtomName);
            }
        }
    }
    pCtx->atomX11TextFormat = atomBestTarget;
    if ((eBestTarget != pCtx->X11TextFormat) || (pCtx->notifyVBox == true))
    {
        uint32_t u32Formats = 0;
        if (g_debugClipboard)
        {
            if (atomBestTarget != None)
            {
                char *szAtomName = XGetAtomName(XtDisplay(pCtx->widget),
                                                atomBestTarget);
                Log2 (("%s: switching to host text target %s.  Available targets are:\n",
                       __PRETTY_FUNCTION__, szAtomName));
                XFree(szAtomName);
            }
            else
                Log2(("%s: no supported host text target found.  Available targets are:\n",
                      __PRETTY_FUNCTION__));
            for (unsigned i = 0; i < cAtoms; ++i)
            {
                char *szAtomName = XGetAtomName(XtDisplay(pCtx->widget),
                                                atomTargets[i]);
                if (szAtomName != 0)
                {
                    Log2 (("%s:     %s\n", __PRETTY_FUNCTION__, szAtomName));
                    XFree(szAtomName);
                }
            }
        }
        pCtx->X11TextFormat = eBestTarget;
        if (eBestTarget != INVALID)
            u32Formats |= VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT;
        VBoxX11ClipboardReportX11Formats(pCtx->pFrontend, u32Formats);
        pCtx->notifyVBox = false;
    }
    XtFree(reinterpret_cast<char *>(pValue));
}

enum { TIMER_FREQ = 200 /* ms */ };

static void vboxClipboardPollX11ForTargets(XtPointer pUserData,
                                           XtIntervalId * /* hTimerId */);
static void clipSchedulePoller(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                               XtTimerCallbackProc proc);

#ifndef TESTCASE
void clipSchedulePoller(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                        XtTimerCallbackProc proc)
{
    XtAppAddTimeOut(pCtx->appContext, TIMER_FREQ, proc, pCtx);
}
#endif

/**
 * This timer callback is called every 200ms to check the contents of the X11
 * clipboard.
 * @note  X11 backend code, callback for XtAppAddTimeOut, recursively
 *        re-armed.
 * @todo  Use the XFIXES extension to check for new clipboard data when
 *        available.
 */
static void vboxClipboardPollX11ForTargets(XtPointer pUserData,
                                           XtIntervalId * /* hTimerId */)
{
    VBOXCLIPBOARDCONTEXTX11 *pCtx =
            reinterpret_cast<VBOXCLIPBOARDCONTEXTX11 *>(pUserData);
    Log3 (("%s: called\n", __PRETTY_FUNCTION__));
    /* Get the current clipboard contents if we don't own it ourselves */
    if (pCtx->eOwner != VB)
    {
        Log3 (("%s: requesting the targets that the host clipboard offers\n",
               __PRETTY_FUNCTION__));
        XtGetSelectionValue(pCtx->widget, pCtx->atomClipboard,
                            pCtx->atomTargets,
                            vboxClipboardGetTargetsFromX11, pCtx,
                            CurrentTime);
    }
    /* Re-arm our timer */
    clipSchedulePoller(pCtx, vboxClipboardPollX11ForTargets);
}

/** We store information about the target formats we can handle in a global
 * vector for internal use.
 * @note  X11 backend code.
 */
static void vboxClipboardAddFormat(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                   const char *pszName,
                                   g_eClipboardFormats eFormat,
                                   unsigned guestFormat)
{
    VBOXCLIPBOARDFORMAT sFormat;
    /* Get an atom from the X server for that target format */
    Atom atomFormat = clipGetAtom(pCtx->widget, pszName);
    sFormat.atom   = atomFormat;
    sFormat.format = eFormat;
    sFormat.guestFormat = guestFormat;
    pCtx->formatList.push_back(sFormat);
    LogFlow (("vboxClipboardAddFormat: added format %s (%d)\n", pszName, eFormat));
}

#ifndef TESTCASE
/**
 * The main loop of our clipboard reader.
 * @note  X11 backend code.
 */
static int vboxClipboardThread(RTTHREAD self, void *pvUser)
{
    LogRel(("Shared clipboard: starting host clipboard thread\n"));

    VBOXCLIPBOARDCONTEXTX11 *pCtx =
            reinterpret_cast<VBOXCLIPBOARDCONTEXTX11 *>(pvUser);
    while (XtAppGetExitFlag(pCtx->appContext) == FALSE)
        XtAppProcessEvent(pCtx->appContext, XtIMAll);
    pCtx->formatList.clear();
    LogRel(("Shared clipboard: host clipboard thread terminated successfully\n"));
    return VINF_SUCCESS;
}
#endif

/** X11 specific uninitialisation for the shared clipboard.
 * @note  X11 backend code.
 */
static void vboxClipboardUninitX11(VBOXCLIPBOARDCONTEXTX11 *pCtx)
{
    AssertPtrReturnVoid(pCtx);
    if (pCtx->widget)
    {
        /* Valid widget + invalid appcontext = bug.  But don't return yet. */
        AssertPtr(pCtx->appContext);
        XtDestroyWidget(pCtx->widget);
    }
    pCtx->widget = NULL;
    if (pCtx->appContext)
        XtDestroyApplicationContext(pCtx->appContext);
    pCtx->appContext = NULL;
    if (pCtx->wakeupPipeRead != 0)
        close(pCtx->wakeupPipeRead);
    if (pCtx->wakeupPipeWrite != 0)
        close(pCtx->wakeupPipeWrite);
    pCtx->wakeupPipeRead = 0;
    pCtx->wakeupPipeWrite = 0;
}

/** Worker function for stopping the clipboard which runs on the event
 * thread. */
static void vboxClipboardStopWorker(XtPointer pUserData, int * /* source */,
                                    XtInputId * /* id */)
{
    
    VBOXCLIPBOARDCONTEXTX11 *pCtx = (VBOXCLIPBOARDCONTEXTX11 *)pUserData;

    /* This might mean that we are getting stopped twice. */
    Assert(pCtx->widget != NULL);

    /* Set the termination flag to tell the Xt event loop to exit.  We
     * reiterate that any outstanding requests from the X11 event loop to
     * the VBox part *must* have returned before we do this. */
    XtAppSetExitFlag(pCtx->appContext);
    pCtx->eOwner = NONE;
    pCtx->X11TextFormat = INVALID;
    pCtx->X11BitmapFormat = INVALID;
}

/** X11 specific initialisation for the shared clipboard.
 * @note  X11 backend code.
 */
static int vboxClipboardInitX11 (VBOXCLIPBOARDCONTEXTX11 *pCtx)
{
    /* Create a window and make it a clipboard viewer. */
    int cArgc = 0;
    char *pcArgv = 0;
    int rc = VINF_SUCCESS;
    // static String szFallbackResources[] = { (char*)"*.width: 1", (char*)"*.height: 1", NULL };
    Display *pDisplay;

    /* Make sure we are thread safe */
    XtToolkitThreadInitialize();
    /* Set up the Clipbard application context and main window.  We call all these functions
       directly instead of calling XtOpenApplication() so that we can fail gracefully if we
       can't get an X11 display. */
    XtToolkitInitialize();
    pCtx->appContext = XtCreateApplicationContext();
    // XtAppSetFallbackResources(pCtx->appContext, szFallbackResources);
    pDisplay = XtOpenDisplay(pCtx->appContext, 0, 0, "VBoxClipboard", 0, 0, &cArgc, &pcArgv);
    if (NULL == pDisplay)
    {
        LogRel(("Shared clipboard: failed to connect to the host clipboard - the window system may not be running.\n"));
        rc = VERR_NOT_SUPPORTED;
    }
    if (RT_SUCCESS(rc))
    {
        pCtx->widget = XtVaAppCreateShell(0, "VBoxClipboard", applicationShellWidgetClass, pDisplay,
                                          XtNwidth, 1, XtNheight, 1, NULL);
        if (NULL == pCtx->widget)
        {
            LogRel(("Shared clipboard: failed to construct the X11 window for the host clipboard manager.\n"));
            rc = VERR_NO_MEMORY;
        }
    }
    if (RT_SUCCESS(rc))
    {
        XtSetMappedWhenManaged(pCtx->widget, false);
        XtRealizeWidget(pCtx->widget);
        /* Set up a timer to poll the host clipboard */
        clipSchedulePoller(pCtx, vboxClipboardPollX11ForTargets);

        /* Get hold of the atoms which we need */
        pCtx->atomClipboard = clipGetAtom(pCtx->widget, "CLIPBOARD");
        pCtx->atomPrimary   = clipGetAtom(pCtx->widget, "PRIMARY");
        pCtx->atomTargets   = clipGetAtom(pCtx->widget, "TARGETS");
        pCtx->atomMultiple  = clipGetAtom(pCtx->widget, "MULTIPLE");
        pCtx->atomTimestamp = clipGetAtom(pCtx->widget, "TIMESTAMP");
        pCtx->atomUtf16     = clipGetAtom(pCtx->widget,
                                          "text/plain;charset=ISO-10646-UCS-2");
        pCtx->atomUtf8      = clipGetAtom(pCtx->widget, "UTF8_STRING");
        /* And build up the vector of supported formats */
        pCtx->atomCText     = clipGetAtom(pCtx->widget, "COMPOUND_TEXT");
        /* And build up the vector of supported formats */
        if (!g_testUtf8 && !g_testCText)
            vboxClipboardAddFormat(pCtx,
                                   "text/plain;charset=ISO-10646-UCS-2",
                                   UTF16,
                                   VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
        if (!g_testUtf16 && !g_testCText)
        {
            vboxClipboardAddFormat(pCtx, "UTF8_STRING", UTF8,
                                   VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
            vboxClipboardAddFormat(pCtx, "text/plain;charset=UTF-8", UTF8,
                                   VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
            vboxClipboardAddFormat(pCtx, "text/plain;charset=utf-8", UTF8,
                                   VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
            vboxClipboardAddFormat(pCtx, "STRING", UTF8,
                                   VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
            vboxClipboardAddFormat(pCtx, "TEXT", UTF8,
                                   VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
            vboxClipboardAddFormat(pCtx, "text/plain", UTF8,
                                   VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
}
        if (!g_testUtf16 && !g_testUtf8)
            vboxClipboardAddFormat(pCtx, "COMPOUND_TEXT", CTEXT,
                                   VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
    }
    /* Create the pipes */
    int pipes[2];
    if (!pipe(pipes))
    {
        pCtx->wakeupPipeRead = pipes[0];
        pCtx->wakeupPipeWrite = pipes[1];
        XtAppAddInput(pCtx->appContext, pCtx->wakeupPipeRead,
                      (XtPointer) XtInputReadMask, vboxClipboardStopWorker,
                      (XtPointer) pCtx);
    }
    else
        rc = RTErrConvertFromErrno(errno);
    if (RT_FAILURE(rc))
        vboxClipboardUninitX11(pCtx);
    return rc;
}

/**
 * Construct the X11 backend of the shared clipboard.
 * @note  X11 backend code
 */
VBOXCLIPBOARDCONTEXTX11 *VBoxX11ClipboardConstructX11
                                 (VBOXCLIPBOARDCONTEXT *pFrontend)
{
    int rc;

    VBOXCLIPBOARDCONTEXTX11 *pCtx = &g_ctxX11;
    /** @todo we still only support one backend at a time, because the X
     * toolkit intrinsics don't support user data in XtOwnSelection.
     * This function should not fail like this. */
    AssertReturn(g_pCtx == NULL, NULL);
    g_pCtx = &g_ctxX11;
    if (!RTEnvGet("DISPLAY"))
    {
        /*
         * If we don't find the DISPLAY environment variable we assume that
         * we are not connected to an X11 server. Don't actually try to do
         * this then, just fail silently and report success on every call.
         * This is important for VBoxHeadless.
         */
        LogRelFunc(("X11 DISPLAY variable not set -- disabling shared clipboard\n"));
        g_fHaveX11 = false;
        return pCtx;
    }

    if (RTEnvGet("VBOX_CBTEST_UTF16"))
    {
        g_testUtf16 = true;
        LogRel(("Host clipboard: testing Utf16\n"));
    }
    else if (RTEnvGet("VBOX_CBTEST_UTF8"))
    {
        g_testUtf8 = true;
        LogRel(("Host clipboard: testing Utf8\n"));
    }
    else if (RTEnvGet("VBOX_CBTEST_CTEXT"))
    {
        g_testCText = true;
        LogRel(("Host clipboard: testing compound text\n"));
    }
    else if (RTEnvGet("VBOX_CBDEBUG"))
    {
        g_debugClipboard = true;
        LogRel(("Host clipboard: enabling additional debugging output\n"));
    }

    g_fHaveX11 = true;

    LogRel(("Initializing X11 clipboard backend\n"));
    pCtx->pFrontend = pFrontend;
    RTSemEventCreate(&pCtx->waitForData);
    return pCtx;
}

/**
 * Destruct the shared clipboard X11 backend.
 * @note  X11 backend code
 */
void VBoxX11ClipboardDestructX11(VBOXCLIPBOARDCONTEXTX11 *pCtx)
{
    /*
     * Immediately return if we are not connected to the host X server.
     */
    if (!g_fHaveX11)
        return;

    /* We set this to NULL when the event thread exits.  It really should
     * have exited at this point, when we are about to unload the code from
     * memory. */
    Assert(pCtx->widget == NULL);
    RTSemEventDestroy(pCtx->waitForData);
}

/**
 * Announce to the X11 backend that we are ready to start.
 * @param  owner who is the initial clipboard owner
 */
int VBoxX11ClipboardStartX11(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                             bool)
{
    int rc = VINF_SUCCESS;
    LogFlowFunc(("\n"));
    /*
     * Immediately return if we are not connected to the host X server.
     */
    if (!g_fHaveX11)
        return VINF_SUCCESS;

    rc = vboxClipboardInitX11(pCtx);
#ifndef TESTCASE
    if (RT_SUCCESS(rc))
    {
        rc = RTThreadCreate(&pCtx->thread, vboxClipboardThread, pCtx, 0,
                            RTTHREADTYPE_IO, RTTHREADFLAGS_WAITABLE, "SHCLIP");
        if (RT_FAILURE(rc))
            LogRel(("Failed to initialise the shared clipboard X11 backend.\n"));
    }
#endif
    if (RT_SUCCESS(rc))
    {
        pCtx->eOwner = NONE;
        pCtx->notifyVBox = true;
    }
    return rc;
}

/**
 * Called when the VBox may have fallen out of sync with the backend.
 * @note  X11 backend code
 */
void VBoxX11ClipboardRequestSyncX11(VBOXCLIPBOARDCONTEXTX11 *pCtx)
{
    /* No longer needed. */
}

/** String written to the wakeup pipe. */
#define WAKE_UP_STRING      "WakeUp!"
/** Length of the string written. */
#define WAKE_UP_STRING_LEN  ( sizeof(WAKE_UP_STRING) - 1 )

/**
 * Shut down the shared clipboard X11 backend.
 * @note  X11 backend code
 * @note  Any requests from this object to get clipboard data from VBox
 *        *must* have completed or aborted before we are called, as
 *        otherwise the X11 event loop will still be waiting for the request
 *        to return and will not be able to terminate.
 */
int VBoxX11ClipboardStopX11(VBOXCLIPBOARDCONTEXTX11 *pCtx)
{
    int rc, rcThread;
    unsigned count = 0;
    /*
     * Immediately return if we are not connected to the host X server.
     */
    if (!g_fHaveX11)
        return VINF_SUCCESS;

    LogRelFunc(("stopping the shared clipboard X11 backend\n"));

    /* Write to the "stop" pipe */
    rc = write(pCtx->wakeupPipeWrite, WAKE_UP_STRING, WAKE_UP_STRING_LEN);
    do
    {
        rc = RTThreadWait(pCtx->thread, 1000, &rcThread);
        ++count;
        Assert(RT_SUCCESS(rc) || ((VERR_TIMEOUT == rc) && (count != 5)));
    } while ((VERR_TIMEOUT == rc) && (count < 300));
    if (RT_SUCCESS(rc))
        AssertRC(rcThread);
    else
        LogRelFunc(("rc=%Rrc\n", rc));
    vboxClipboardUninitX11(pCtx);
    LogFlowFunc(("returning %Rrc.\n", rc));
    return rc;
}

/**
 * Satisfy a request from X11 for clipboard targets supported by VBox.
 *
 * @returns true if we successfully convert the data to the format
 *          requested, false otherwise.
 *
 * @param  atomTypeReturn The type of the data we are returning
 * @param  pValReturn     A pointer to the data we are returning.  This
 *                        should be set to memory allocated by XtMalloc,
 *                        which will be freed later by the Xt toolkit.
 * @param  pcLenReturn    The length of the data we are returning
 * @param  piFormatReturn The format (8bit, 16bit, 32bit) of the data we are
 *                        returning
 * @note  X11 backend code, called by the XtOwnSelection callback.
 */
static Boolean vboxClipboardConvertTargetsForX11(VBOXCLIPBOARDCONTEXTX11
                                                                      *pCtx,
                                                 Atom *atomTypeReturn,
                                                 XtPointer *pValReturn,
                                                 unsigned long *pcLenReturn,
                                                 int *piFormatReturn)
{
    unsigned uListSize = pCtx->formatList.size();
    Atom *atomTargets = reinterpret_cast<Atom *>(XtMalloc((uListSize + 3) * sizeof(Atom)));
    unsigned cTargets = 0;

    LogFlowFunc (("called\n"));
    for (unsigned i = 0; i < uListSize; ++i)
    {
        if (   ((pCtx->vboxFormats & VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT) != 0)
            && (   pCtx->formatList[i].guestFormat
                == VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT))
        {
            atomTargets[cTargets] = pCtx->formatList[i].atom;
            ++cTargets;
        }
    }
    atomTargets[cTargets] = pCtx->atomTargets;
    atomTargets[cTargets + 1] = pCtx->atomMultiple;
    atomTargets[cTargets + 2] = pCtx->atomTimestamp;
    if (g_debugClipboard)
    {
        for (unsigned i = 0; i < cTargets + 3; i++)
        {
            char *szAtomName = XGetAtomName(XtDisplay(pCtx->widget), atomTargets[i]);
            if (szAtomName != 0)
            {
                Log2 (("%s: returning target %s\n", __PRETTY_FUNCTION__,
                       szAtomName));
                XFree(szAtomName);
            }
            else
            {
                Log(("%s: invalid atom %d in the list!\n", __PRETTY_FUNCTION__,
                     atomTargets[i]));
            }
        }
    }
    *atomTypeReturn = XA_ATOM;
    *pValReturn = reinterpret_cast<XtPointer>(atomTargets);
    *pcLenReturn = cTargets + 3;
    *piFormatReturn = 32;
    return true;
}

/**
 * Satisfy a request from X11 to convert the clipboard text to Utf16.  We
 * return non-zero terminated text.
 * @todo that works, but it is bad.  Change it to return zero-terminated
 *       text.
 *
 * @returns true if we successfully convert the data to the format
 * requested, false otherwise.
 *
 * @param  atomTypeReturn  Where to store the atom for the type of the data
 *                         we are returning
 * @param  pValReturn      Where to store the pointer to the data we are
 *                         returning.  This should be to memory allocated by
 *                         XtMalloc, which will be freed by the Xt toolkit
 *                         later.
 * @param  pcLenReturn     Where to store the length of the data we are
 *                         returning
 * @param  piFormatReturn  Where to store the bit width (8, 16, 32) of the
 *                         data we are returning
 * @note  X11 backend code, called by the callback for XtOwnSelection.
 */
static Boolean vboxClipboardConvertUtf16(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                         Atom *atomTypeReturn,
                                         XtPointer *pValReturn,
                                         unsigned long *pcLenReturn,
                                         int *piFormatReturn)
{
    PRTUTF16 pu16SrcText, pu16DestText;
    void *pvVBox = NULL;
    uint32_t cbVBox = 0;
    size_t cwSrcLen, cwDestLen;
    int rc;

    LogFlowFunc (("called\n"));
    rc = VBoxX11ClipboardReadVBoxData(pCtx->pFrontend, VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT, &pvVBox, &cbVBox);
    if ((RT_FAILURE(rc)) || (cbVBox == 0))
    {
        /* If VBoxX11ClipboardReadVBoxData fails then we may be terminating */
        LogRelFunc (("VBoxX11ClipboardReadVBoxData returned %Rrc%s\n", rc,
                    RT_SUCCESS(rc) ? ", cbVBox == 0" :  ""));
        RTMemFree(pvVBox);
        return false;
    }
    pu16SrcText = reinterpret_cast<PRTUTF16>(pvVBox);
    cwSrcLen = cbVBox / 2;
    /* How long will the converted text be? */
    rc = vboxClipboardUtf16GetLinSize(pu16SrcText, cwSrcLen, &cwDestLen);
    if (RT_FAILURE(rc))
    {
        LogRel(("vboxClipboardConvertUtf16: clipboard conversion failed.  vboxClipboardUtf16GetLinSize returned %Rrc.  Abandoning.\n", rc));
        RTMemFree(pvVBox);
        AssertRCReturn(rc, false);
    }
    if (cwDestLen == 0)
    {
        LogFlowFunc(("received empty clipboard data from the guest, returning false.\n"));
        RTMemFree(pvVBox);
        return false;
    }
    pu16DestText = reinterpret_cast<PRTUTF16>(XtMalloc(cwDestLen * 2));
    if (pu16DestText == 0)
    {
        LogRel(("vboxClipboardConvertUtf16: failed to allocate %d bytes\n", cwDestLen * 2));
        RTMemFree(pvVBox);
        return false;
    }
    /* Convert the text. */
    rc = vboxClipboardUtf16WinToLin(pu16SrcText, cwSrcLen, pu16DestText, cwDestLen);
    if (RT_FAILURE(rc))
    {
        LogRel(("vboxClipboardConvertUtf16: clipboard conversion failed.  vboxClipboardUtf16WinToLin returned %Rrc.  Abandoning.\n", rc));
        XtFree(reinterpret_cast<char *>(pu16DestText));
        RTMemFree(pvVBox);
        return false;
    }
    LogFlowFunc (("converted string is %.*ls. Returning.\n", cwDestLen, pu16DestText));
    RTMemFree(pvVBox);
    *atomTypeReturn = pCtx->atomUtf16;
    *pValReturn = reinterpret_cast<XtPointer>(pu16DestText);
    *pcLenReturn = cwDestLen;
    *piFormatReturn = 16;
    return true;
}

/**
 * Satisfy a request from X11 to convert the clipboard text to Utf8.  We
 * return non-zero terminated text.
 * @todo that works, but it is bad.  Change it to return zero-terminated
 *       text.
 *
 * @returns true if we successfully convert the data to the format
 * requested, false otherwise.
 *
 * @param  atomTypeReturn  Where to store the atom for the type of the data
 *                         we are returning
 * @param  pValReturn      Where to store the pointer to the data we are
 *                         returning.  This should be to memory allocated by
 *                         XtMalloc, which will be freed by the Xt toolkit
 *                         later.
 * @param  pcLenReturn     Where to store the length of the data we are
 *                         returning
 * @param  piFormatReturn  Where to store the bit width (8, 16, 32) of the
 *                         data we are returning
 * @note  X11 backend code, called by the callback for XtOwnSelection.
 */
static Boolean vboxClipboardConvertToUtf8ForX11(VBOXCLIPBOARDCONTEXTX11
                                                                      *pCtx,
                                                Atom *atomTarget,
                                                Atom *atomTypeReturn,
                                                XtPointer *pValReturn,
                                                unsigned long *pcLenReturn,
                                                int *piFormatReturn)
{
    PRTUTF16 pu16SrcText, pu16DestText;
    char *pu8DestText;
    void *pvVBox = NULL;
    uint32_t cbVBox = 0;
    size_t cwSrcLen, cwDestLen, cbDestLen;
    int rc;

    LogFlowFunc (("called\n"));
    /* Read the clipboard data from the guest. */
    rc = VBoxX11ClipboardReadVBoxData(pCtx->pFrontend, VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT, &pvVBox, &cbVBox);
    if ((rc != VINF_SUCCESS) || (cbVBox == 0))
    {
        /* If VBoxX11ClipboardReadVBoxData fails then we may be terminating */
        LogRelFunc (("VBoxX11ClipboardReadVBoxData returned %Rrc%s\n", rc,
                     RT_SUCCESS(rc) ? ", cbVBox == 0" :  ""));
        RTMemFree(pvVBox);
        return false;
    }
    pu16SrcText = reinterpret_cast<PRTUTF16>(pvVBox);
    cwSrcLen = cbVBox / 2;
    /* How long will the converted text be? */
    rc = vboxClipboardUtf16GetLinSize(pu16SrcText, cwSrcLen, &cwDestLen);
    if (RT_FAILURE(rc))
    {
        LogRelFunc (("clipboard conversion failed.  vboxClipboardUtf16GetLinSize returned %Rrc.  Abandoning.\n", rc));
        RTMemFree(pvVBox);
        AssertRCReturn(rc, false);
    }
    if (cwDestLen == 0)
    {
        LogFlowFunc(("received empty clipboard data from the guest, returning false.\n"));
        RTMemFree(pvVBox);
        return false;
    }
    pu16DestText = reinterpret_cast<PRTUTF16>(RTMemAlloc(cwDestLen * 2));
    if (pu16DestText == 0)
    {
        LogRelFunc (("failed to allocate %d bytes\n", cwDestLen * 2));
        RTMemFree(pvVBox);
        return false;
    }
    /* Convert the text. */
    rc = vboxClipboardUtf16WinToLin(pu16SrcText, cwSrcLen, pu16DestText, cwDestLen);
    if (RT_FAILURE(rc))
    {
        LogRelFunc (("clipboard conversion failed.  vboxClipboardUtf16WinToLin() returned %Rrc.  Abandoning.\n", rc));
        RTMemFree(reinterpret_cast<void *>(pu16DestText));
        RTMemFree(pvVBox);
        return false;
    }
    /* Allocate enough space, as RTUtf16ToUtf8Ex may fail if the
       space is too tightly calculated. */
    pu8DestText = XtMalloc(cwDestLen * 4);
    if (pu8DestText == 0)
    {
        LogRelFunc (("failed to allocate %d bytes\n", cwDestLen * 4));
        RTMemFree(reinterpret_cast<void *>(pu16DestText));
        RTMemFree(pvVBox);
        return false;
    }
    /* Convert the Utf16 string to Utf8. */
    rc = RTUtf16ToUtf8Ex(pu16DestText + 1, cwDestLen - 1, &pu8DestText, cwDestLen * 4,
                         &cbDestLen);
    RTMemFree(reinterpret_cast<void *>(pu16DestText));
    if (RT_FAILURE(rc))
    {
        LogRelFunc (("clipboard conversion failed.  RTUtf16ToUtf8Ex() returned %Rrc.  Abandoning.\n", rc));
        XtFree(pu8DestText);
        RTMemFree(pvVBox);
        return false;
    }
    LogFlowFunc (("converted string is %.*s. Returning.\n", cbDestLen, pu8DestText));
    RTMemFree(pvVBox);
    *atomTypeReturn = *atomTarget;
    *pValReturn = reinterpret_cast<XtPointer>(pu8DestText);
    *pcLenReturn = cbDestLen + 1;
    *piFormatReturn = 8;
    return true;
}

/**
 * Satisfy a request from X11 to convert the clipboard text to
 * COMPOUND_TEXT.  We return non-zero terminated text.
 * @todo that works, but it is bad.  Change it to return zero-terminated
 *       text.
 *
 * @returns true if we successfully convert the data to the format
 * requested, false otherwise.
 *
 * @param  atomTypeReturn  Where to store the atom for the type of the data
 *                         we are returning
 * @param  pValReturn      Where to store the pointer to the data we are
 *                         returning.  This should be to memory allocated by
 *                         XtMalloc, which will be freed by the Xt toolkit
 *                         later.
 * @param  pcLenReturn     Where to store the length of the data we are
 *                         returning
 * @param  piFormatReturn  Where to store the bit width (8, 16, 32) of the
 *                         data we are returning
 * @note  X11 backend code, called by the callback for XtOwnSelection.
 */
static Boolean vboxClipboardConvertToCTextForX11(VBOXCLIPBOARDCONTEXTX11
                                                                      *pCtx,
                                                 Atom *atomTypeReturn,
                                                 XtPointer *pValReturn,
                                                 unsigned long *pcLenReturn,
                                                 int *piFormatReturn)
{
    PRTUTF16 pu16SrcText, pu16DestText;
    void *pvVBox = NULL;
    uint32_t cbVBox = 0;
    char *pu8DestText = 0;
    size_t cwSrcLen, cwDestLen, cbDestLen;
    XTextProperty property;
    int rc;

    LogFlowFunc (("called\n"));
    /* Read the clipboard data from the guest. */
    rc = VBoxX11ClipboardReadVBoxData(pCtx->pFrontend, VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT, &pvVBox, &cbVBox);
    if ((rc != VINF_SUCCESS) || (cbVBox == 0))
    {
        /* If VBoxX11ClipboardReadVBoxData fails then we may be terminating */
        LogRelFunc (("VBoxX11ClipboardReadVBoxData returned %Rrc%s\n", rc,
                      RT_SUCCESS(rc) ? ", cbVBox == 0" :  ""));
        RTMemFree(pvVBox);
        return false;
    }
    pu16SrcText = reinterpret_cast<PRTUTF16>(pvVBox);
    cwSrcLen = cbVBox / 2;
    /* How long will the converted text be? */
    rc = vboxClipboardUtf16GetLinSize(pu16SrcText, cwSrcLen, &cwDestLen);
    if (RT_FAILURE(rc))
    {
        LogRelFunc (("clipboard conversion failed.  vboxClipboardUtf16GetLinSize returned %Rrc.  Abandoning.\n", rc));
        RTMemFree(pvVBox);
        AssertRCReturn(rc, false);
    }
    if (cwDestLen == 0)
    {
        LogFlowFunc(("received empty clipboard data from the guest, returning false.\n"));
        RTMemFree(pvVBox);
        return false;
    }
    pu16DestText = reinterpret_cast<PRTUTF16>(RTMemAlloc(cwDestLen * 2));
    if (pu16DestText == 0)
    {
        LogRelFunc (("failed to allocate %d bytes\n", cwDestLen * 2));
        RTMemFree(pvVBox);
        return false;
    }
    /* Convert the text. */
    rc = vboxClipboardUtf16WinToLin(pu16SrcText, cwSrcLen, pu16DestText, cwDestLen);
    if (RT_FAILURE(rc))
    {
        LogRelFunc (("clipboard conversion failed.  vboxClipboardUtf16WinToLin() returned %Rrc.  Abandoning.\n", rc));
        RTMemFree(reinterpret_cast<void *>(pu16DestText));
        RTMemFree(pvVBox);
        return false;
    }
    /* Convert the Utf16 string to Utf8. */
    rc = RTUtf16ToUtf8Ex(pu16DestText + 1, cwDestLen - 1, &pu8DestText, 0, &cbDestLen);
    RTMemFree(reinterpret_cast<void *>(pu16DestText));
    if (RT_FAILURE(rc))
    {
        LogRelFunc (("clipboard conversion failed.  RTUtf16ToUtf8Ex() returned %Rrc.  Abandoning.\n", rc));
        RTMemFree(pvVBox);
        return false;
    }
    /* And finally (!) convert the Utf8 text to compound text. */
#ifdef RT_OS_SOLARIS
    rc = XmbTextListToTextProperty(XtDisplay(pCtx->widget), &pu8DestText, 1,
                                     XCompoundTextStyle, &property);
#else
    rc = Xutf8TextListToTextProperty(XtDisplay(pCtx->widget), &pu8DestText, 1,
                                     XCompoundTextStyle, &property);
#endif
    RTMemFree(pu8DestText);
    if (rc < 0)
    {
        const char *pcReason;
        switch(rc)
        {
        case XNoMemory:
            pcReason = "out of memory";
            break;
        case XLocaleNotSupported:
            pcReason = "locale (Utf8) not supported";
            break;
        case XConverterNotFound:
            pcReason = "converter not found";
            break;
        default:
            pcReason = "unknown error";
        }
        LogRelFunc (("Xutf8TextListToTextProperty failed.  Reason: %s\n",
                pcReason));
        RTMemFree(pvVBox);
        return false;
    }
    LogFlowFunc (("converted string is %s. Returning.\n", property.value));
    RTMemFree(pvVBox);
    *atomTypeReturn = property.encoding;
    *pValReturn = reinterpret_cast<XtPointer>(property.value);
    *pcLenReturn = property.nitems + 1;
    *piFormatReturn = property.format;
    return true;
}

/**
 * Return VBox's clipboard data for an X11 client.
 * @note  X11 backend code, callback for XtOwnSelection
 */
static Boolean vboxClipboardConvertForX11(Widget, Atom *atomSelection,
                                          Atom *atomTarget,
                                          Atom *atomTypeReturn,
                                          XtPointer *pValReturn,
                                          unsigned long *pcLenReturn,
                                          int *piFormatReturn)
{
    g_eClipboardFormats eFormat = INVALID;
    /** @todo find a better way around the lack of user data. */
    VBOXCLIPBOARDCONTEXTX11 *pCtx = g_pCtx;

    LogFlowFunc(("\n"));
    /* Drop requests that we receive too late. */
    if (pCtx->eOwner != VB)
        return false;
    if (   (*atomSelection != pCtx->atomClipboard)
        && (*atomSelection != pCtx->atomPrimary)
       )
    {
        LogFlowFunc(("rc = false\n"));
        return false;
    }
    if (g_debugClipboard)
    {
        char *szAtomName = XGetAtomName(XtDisplay(pCtx->widget), *atomTarget);
        if (szAtomName != 0)
        {
            Log2 (("%s: request for format %s\n", __PRETTY_FUNCTION__, szAtomName));
            XFree(szAtomName);
        }
        else
        {
            LogFunc (("request for invalid target atom %d!\n", *atomTarget));
        }
    }
    if (*atomTarget == pCtx->atomTargets)
    {
        eFormat = TARGETS;
    }
    else
    {
        for (unsigned i = 0; i != pCtx->formatList.size(); ++i)
        {
            if (pCtx->formatList[i].atom == *atomTarget)
            {
                eFormat = pCtx->formatList[i].format;
                break;
            }
        }
    }
    switch (eFormat)
    {
    case TARGETS:
        return vboxClipboardConvertTargetsForX11(pCtx, atomTypeReturn,
                                                 pValReturn, pcLenReturn,
                                                 piFormatReturn);
    case UTF16:
        return vboxClipboardConvertUtf16(pCtx, atomTypeReturn, pValReturn,
                                         pcLenReturn, piFormatReturn);
    case UTF8:
        return vboxClipboardConvertToUtf8ForX11(pCtx, atomTarget,
                                                atomTypeReturn,
                                                pValReturn, pcLenReturn,
                                                piFormatReturn);
    case CTEXT:
        return vboxClipboardConvertToCTextForX11(pCtx, atomTypeReturn,
                                                 pValReturn, pcLenReturn,
                                                 piFormatReturn);
    default:
        LogFunc (("bad format\n"));
        return false;
    }
}

/**
 * This is called by the X toolkit intrinsics to let us know that another
 * X11 client has taken the clipboard.  In this case we notify VBox that
 * we want ownership of the clipboard.
 * @note  X11 backend code, callback for XtOwnSelection
 */
static void vboxClipboardReturnToX11(Widget, Atom *)
{
    /** @todo find a better way around the lack of user data */
    VBOXCLIPBOARDCONTEXTX11 *pCtx = g_pCtx;
    LogFlowFunc (("called, giving X11 clipboard ownership\n"));
    /* These should be set to the right values as soon as we start polling */
    pCtx->X11TextFormat = INVALID;
    pCtx->X11BitmapFormat = INVALID;
    pCtx->eOwner = X11;
    pCtx->notifyVBox = true;
}

/**
 * VBox is taking possession of the shared clipboard.
 *
 * @param u32Formats Clipboard formats the guest is offering
 * @note  X11 backend code
 */
void VBoxX11ClipboardAnnounceVBoxFormat(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                        uint32_t u32Formats)
{
    /*
     * Immediately return if we are not connected to the host X server.
     */
    if (!g_fHaveX11)
        return;

    pCtx->vboxFormats = u32Formats;
    LogFlowFunc (("u32Formats=%d\n", u32Formats));
    if (u32Formats == 0)
    {
        /* This is just an automatism, not a genuine anouncement */
        LogFlowFunc(("returning\n"));
        return;
    }
    if (pCtx->eOwner == VB)
    {
        /* We already own the clipboard, so no need to grab it, especially as that can lead
           to races due to the asynchronous nature of the X11 clipboard.  This event may also
           have been sent out by the guest to invalidate the Windows clipboard cache. */
        LogFlowFunc(("returning\n"));
        return;
    }
    Log2 (("%s: giving the guest clipboard ownership\n", __PRETTY_FUNCTION__));
    pCtx->X11TextFormat = INVALID;
    pCtx->X11BitmapFormat = INVALID;
    if (XtOwnSelection(pCtx->widget, pCtx->atomClipboard, CurrentTime,
                       vboxClipboardConvertForX11, vboxClipboardReturnToX11,
                       0) == True)
    {
        pCtx->eOwner = VB;
        XtOwnSelection(pCtx->widget, pCtx->atomPrimary, CurrentTime,
                       vboxClipboardConvertForX11, NULL, 0);
    }
    else
    {
        /* Another X11 client claimed the clipboard just after us, so let it
         * go again. */
        Log2 (("%s: returning clipboard ownership to the host\n", __PRETTY_FUNCTION__));
        /* We set this so that the guest gets notified when we take the clipboard, even if no
          guest formats are found which we understand. */
        /* VBox thinks it currently owns the clipboard, so we must notify it
         * as soon as we know what formats X11 has to offer. */
        pCtx->notifyVBox = true;
        pCtx->eOwner = X11;
    }
    LogFlowFunc(("returning\n"));

}

/**
 * Called when VBox wants to read the X11 clipboard.
 *
 * @param  pClient   Context information about the guest VM
 * @param  u32Format The format that the guest would like to receive the data in
 * @param  pv        Where to write the data to
 * @param  cb        The size of the buffer to write the data to
 * @param  pcbActual Where to write the actual size of the written data
 * @note   X11 backend code
 */
int VBoxX11ClipboardReadX11Data(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                uint32_t u32Format,
                                VBOXCLIPBOARDREQUEST *pRequest)
{
    /*
     * Immediately return if we are not connected to the host X server.
     */
    if (!g_fHaveX11)
    {
        /* no data available */
        *pRequest->pcbActual = 0;
        return VINF_SUCCESS;
    }

    LogFlowFunc (("u32Format = %d, cb = %d\n", u32Format, pRequest->cb));

    /*
     * The guest wants to read data in the given format.
     */
    if (u32Format & VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT)
    {
        if (pCtx->X11TextFormat == INVALID)
        {
            /* No data available. */
            *pRequest->pcbActual = 0;
            return VERR_NO_DATA;  /* The guest thinks we have data and we don't */
        }
        /* Initially set the size of the data read to zero in case we fail
         * somewhere. */
        *pRequest->pcbActual = 0;
        /* Send out a request for the data to the current clipboard owner */
        XtGetSelectionValue(pCtx->widget, pCtx->atomClipboard,
                            pCtx->atomX11TextFormat,
                            vboxClipboardGetDataFromX11,
                            reinterpret_cast<XtPointer>(pRequest),
                            CurrentTime);
        /* When the data arrives, the vboxClipboardGetDataFromX11 callback will be called.  The
           callback will signal the event semaphore when it has processed the data for us. */

        int rc = RTSemEventWait(pCtx->waitForData, RT_INDEFINITE_WAIT);
        if (RT_FAILURE(rc))
            return rc;
    }
    else
    {
        return VERR_NOT_IMPLEMENTED;
    }
    return VINF_SUCCESS;
}

#ifdef TESTCASE

#include <iprt/initterm.h>
#include <iprt/stream.h>
#include <poll.h>

#define TEST_NAME "tstClipboardX11"
#define TEST_WIDGET (Widget)0xffff

/* Our X11 clipboard target poller */
static XtTimerCallbackProc g_pfnPoller = NULL;
/* User data for the poller function. */
static XtPointer g_pPollerData = NULL;

/* For the testcase, we install the poller function in a global variable
 * which is called when the testcase updates the X11 targets. */
void clipSchedulePoller(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                        XtTimerCallbackProc proc)
{
    g_pfnPoller = proc;
    g_pPollerData = (XtPointer)pCtx;
}

static bool clipPollTargets()
{
    if (!g_pfnPoller)
        return false;
    g_pfnPoller(g_pPollerData, NULL);
    return true;
}

/* For the purpose of the test case, we just execute the procedure to be
 * scheduled, as we are running single threaded. */
void clipSchedule(XtAppContext app_context, XtTimerCallbackProc proc,
                  XtPointer client_data)
{
    proc(client_data, NULL);
}

void XtFree(char *ptr)
{ RTMemFree((void *) ptr); }

/* The data in the simulated VBox clipboard */
static int g_vboxDataRC = VINF_SUCCESS;
static void *g_vboxDatapv = NULL;
static uint32_t g_vboxDatacb = 0;

/* Set empty data in the simulated VBox clipboard. */
static void clipEmptyVBox(VBOXCLIPBOARDCONTEXTX11 *pCtx, int retval)
{
    g_vboxDataRC = retval;
    RTMemFree(g_vboxDatapv);
    g_vboxDatapv = NULL;
    g_vboxDatacb = 0;
    VBoxX11ClipboardAnnounceVBoxFormat(pCtx, 0);
}

/* Set the data in the simulated VBox clipboard. */
static int clipSetVBoxUtf16(VBOXCLIPBOARDCONTEXTX11 *pCtx, int retval,
                            const char *pcszData, size_t cb)
{
    PRTUTF16 pwszData = NULL;
    size_t cwData = 0;
    int rc = RTStrToUtf16Ex(pcszData, RTSTR_MAX, &pwszData, 0, &cwData);
    if (RT_FAILURE(rc))
        return rc;
    AssertReturn(cb <= cwData * 2 + 2, VERR_BUFFER_OVERFLOW);
    void *pv = RTMemDup(pwszData, cb);
    RTUtf16Free(pwszData);
    if (pv == NULL)
        return VERR_NO_MEMORY;
    if (g_vboxDatapv)
        RTMemFree(g_vboxDatapv);
    g_vboxDataRC = retval;
    g_vboxDatapv = pv;
    g_vboxDatacb = cb;
    VBoxX11ClipboardAnnounceVBoxFormat(pCtx,
                                       VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT);
    return VINF_SUCCESS;
}

/* Return the data in the simulated VBox clipboard. */
int VBoxX11ClipboardReadVBoxData(VBOXCLIPBOARDCONTEXT *pCtx,
                                 uint32_t u32Format, void **ppv,
                                 uint32_t *pcb)
{
    *pcb = g_vboxDatacb;
    if (g_vboxDatapv != NULL)
    {
        void *pv = RTMemDup(g_vboxDatapv, g_vboxDatacb);
        *ppv = pv;
        return pv != NULL ? g_vboxDataRC : VERR_NO_MEMORY;
    }
    *ppv = NULL;
    return g_vboxDataRC;
}

Display *XtDisplay(Widget w)
{ return (Display *) 0xffff; }

int XmbTextListToTextProperty(Display *display, char **list, int count,
                              XICCEncodingStyle style,
                              XTextProperty *text_prop_return)
{
    /* We don't fully reimplement this API for obvious reasons. */
    AssertReturn(count == 1, XLocaleNotSupported);
    AssertReturn(style == XCompoundTextStyle, XLocaleNotSupported);
    /* We simplify the conversion by only accepting ASCII. */
    for (unsigned i = 0; (*list)[i] != 0; ++i)
        AssertReturn(((*list)[i] & 0x80) == 0, XLocaleNotSupported);
    text_prop_return->value =
            (unsigned char*)RTMemDup(*list, strlen(*list) + 1);
    text_prop_return->encoding = clipGetAtom(NULL, "COMPOUND_TEXT");
    text_prop_return->format = 8;
    text_prop_return->nitems = strlen(*list);
    return 0;
}

int Xutf8TextListToTextProperty(Display *display, char **list, int count,
                                XICCEncodingStyle style,
                                XTextProperty *text_prop_return)
{
    return XmbTextListToTextProperty(display, list, count, style,
                                     text_prop_return);
}

int XmbTextPropertyToTextList(Display *display,
                              const XTextProperty *text_prop,
                              char ***list_return, int *count_return)
{
    int rc = 0;
    if (text_prop->nitems == 0)
    {
        *list_return = NULL;
        *count_return = 0;
        return 0;
    }
    /* Only accept simple ASCII properties */
    for (unsigned i = 0; i < text_prop->nitems; ++i)
        AssertReturn(!(text_prop->value[i] & 0x80), XConverterNotFound);
    char **ppList = (char **)RTMemAlloc(sizeof(char *));
    char *pValue = (char *)RTMemAlloc(text_prop->nitems + 1);
    if (pValue)
    {
        memcpy(pValue, text_prop->value, text_prop->nitems);
        pValue[text_prop->nitems] = 0;
    }
    if (ppList)
        *ppList = pValue;
    if (!ppList || !pValue)
    {
        RTMemFree(ppList);
        RTMemFree(pValue);
        rc = XNoMemory;
    }
    else
    {
        /* NULL-terminate the string */
        pValue[text_prop->nitems] = '\0';
        *count_return = 1;
        *list_return = ppList;
    }
    return rc;
}

int Xutf8TextPropertyToTextList(Display *display,
                                const XTextProperty *text_prop,
                                char ***list_return, int *count_return)
{
    return XmbTextPropertyToTextList(display, text_prop, list_return,
                                     count_return);
}

void XtAppSetExitFlag(XtAppContext app_context) {}

void XtDestroyWidget(Widget w) {}

XtAppContext XtCreateApplicationContext(void) { return (XtAppContext)0xffff; }

void XtDestroyApplicationContext(XtAppContext app_context) {}

void XtToolkitInitialize(void) {}

Boolean XtToolkitThreadInitialize(void) { return True; }

Display *XtOpenDisplay(XtAppContext app_context,
                       _Xconst _XtString display_string,
                       _Xconst _XtString application_name,
                       _Xconst _XtString application_class,
                       XrmOptionDescRec *options, Cardinal num_options,
                       int *argc, char **argv)
{ return (Display *)0xffff; }

Widget XtVaAppCreateShell(_Xconst _XtString application_name,
                          _Xconst _XtString application_class,
                          WidgetClass widget_class, Display *display, ...)
{ return TEST_WIDGET; }

void XtSetMappedWhenManaged(Widget widget, _XtBoolean mapped_when_managed) {}

void XtRealizeWidget(Widget widget) {}

XtInputId XtAppAddInput(XtAppContext app_context, int source,
                        XtPointer condition, XtInputCallbackProc proc,
                        XtPointer closure)
{ return 0xffff; }

/** The table mapping X11 names to data formats and to the corresponding
 * VBox clipboard formats (currently only Unicode) */
static struct _CLIPFORMATTABLE
{
    /** The X11 atom name of the format (several names can match one format)
     */
    const char *pcszAtom;
    /** The format corresponding to the name */
    g_eClipboardFormats enmFormat;
    /** The corresponding VBox clipboard format */
    uint32_t   u32VBoxFormat;
} g_aFormats[] =
{
    { "text/plain;charset=ISO-10646-UCS-2", UTF16,
      VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT },
    { "UTF8_STRING", UTF8, VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT },
    { "text/plain;charset=UTF-8", UTF8,
      VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT },
    { "text/plain;charset=utf-8", UTF8,
      VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT },
    { "STRING", UTF8, VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT },
    { "TEXT", UTF8, VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT },
    { "text/plain", UTF8, VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT },
    { "COMPOUND_TEXT", CTEXT, VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT }
};

/* Atoms we need other than the formats we support. */
static const char *g_apszSupAtoms[] =
{
    "PRIMARY", "CLIPBOARD", "TARGETS", "MULTIPLE", "TIMESTAMP"
};

/* This just looks for the atom names in a couple of tables and returns an
 * index with an offset added. */
Boolean XtConvertAndStore(Widget widget, _Xconst _XtString from_type,
                          XrmValue* from, _Xconst _XtString to_type,
                          XrmValue* to_in_out)
{
    Boolean rc = False;
    /* What we support is: */
    AssertReturn(from_type == XtRString, False);
    AssertReturn(to_type == XtRAtom, False);
    for (unsigned i = 0; i < RT_ELEMENTS(g_aFormats); ++i)
        if (!strcmp(from->addr, g_aFormats[i].pcszAtom))
        {
            *(Atom *)(to_in_out->addr) = (Atom) (i + 0x1000);
            rc = True;
        }
    for (unsigned i = 0; i < RT_ELEMENTS(g_apszSupAtoms); ++i)
        if (!strcmp(from->addr, g_apszSupAtoms[i]))
        {
            *(Atom *)(to_in_out->addr) = (Atom) (i + 0x2000);
            rc = True;
        }
    Assert(rc == True);  /* Have we missed any atoms? */
    return rc;
}

/* The current values of the X selection, which will be returned to the
 * XtGetSelectionValue callback. */
static Atom g_selTarget = 0;
static Atom g_selType = 0;
static const void *g_pSelData = NULL;
static unsigned long g_cSelData = 0;
static int g_selFormat = 0;

void XtGetSelectionValue(Widget widget, Atom selection, Atom target,
                         XtSelectionCallbackProc callback,
                         XtPointer closure, Time time)
{
    unsigned long count = 0;
    int format = 0;
    Atom type = XA_STRING;
    if (   (   selection != clipGetAtom(NULL, "PRIMARY")
            && selection != clipGetAtom(NULL, "CLIPBOARD")
            && selection != clipGetAtom(NULL, "TARGETS"))
        || (   target != g_selTarget
            && target != clipGetAtom(NULL, "TARGETS")))
    {
        /* Otherwise this is probably a caller error. */
        Assert(target != g_selTarget);
        callback(widget, closure, &selection, &type, NULL, &count, &format);
                /* Could not convert to target. */
        return;
    }
    XtPointer pValue = NULL;
    if (target == clipGetAtom(NULL, "TARGETS"))
    {
        pValue = (XtPointer) RTMemDup(&g_selTarget, sizeof(g_selTarget));
        type = XA_ATOM;
        count = 1;
        format = 32;
    }
    else
    {
        pValue = (XtPointer) g_pSelData ? RTMemDup(g_pSelData, g_cSelData)
                                        : NULL;
        type = g_selType;
        count = g_pSelData ? g_cSelData : 0;
        format = g_selFormat;
    }
    if (!pValue)
    {
        count = 0;
        format = 0;
    }
    callback(widget, closure, &selection, &type, pValue,
             &count, &format);
}

/* The formats currently on offer from X11 via the shared clipboard */
static uint32_t g_fX11Formats = 0;

void VBoxX11ClipboardReportX11Formats(VBOXCLIPBOARDCONTEXT* pCtx,
                                      uint32_t u32Formats)
{
    g_fX11Formats = u32Formats;
}

static uint32_t clipQueryFormats()
{
    return g_fX11Formats;
}

/* Does our clipboard code currently own the selection? */
static bool g_ownsSel = false;
/* The procedure that is called when we should convert the selection to a
 * given format. */
static XtConvertSelectionProc g_pfnSelConvert = NULL;
/* The procedure which is called when we lose the selection. */
static XtLoseSelectionProc g_pfnSelLose = NULL;
/* The procedure which is called when the selection transfer has completed. */
static XtSelectionDoneProc g_pfnSelDone = NULL;

Boolean XtOwnSelection(Widget widget, Atom selection, Time time,
                       XtConvertSelectionProc convert,
                       XtLoseSelectionProc lose,
                       XtSelectionDoneProc done)
{
    if (selection != clipGetAtom(NULL, "CLIPBOARD"))
        return True;  /* We don't really care about this. */
    g_ownsSel = true;  /* Always succeed. */
    g_pfnSelConvert = convert;
    g_pfnSelLose = lose;
    g_pfnSelDone = done;
    return True;
}

void XtDisownSelection(Widget widget, Atom selection, Time time)
{
    g_ownsSel = false;
    g_pfnSelConvert = NULL;
    g_pfnSelLose = NULL;
    g_pfnSelDone = NULL;
}

/* Request the shared clipboard to convert its data to a given format. */
static bool clipConvertSelection(const char *pcszTarget, Atom *type,
                                 XtPointer *value, unsigned long *length,
                                 int *format)
{
    Atom target = clipGetAtom(NULL, pcszTarget);
    if (target == 0)
        return false;
    /* Initialise all return values in case we make a quick exit. */
    *type = XA_STRING;
    *value = NULL;
    *length = 0;
    *format = 0;
    if (!g_ownsSel)
        return false;
    if (!g_pfnSelConvert)
        return false;
    Atom clipAtom = clipGetAtom(NULL, "CLIPBOARD");
    if (!g_pfnSelConvert(TEST_WIDGET, &clipAtom, &target, type,
                         value, length, format))
        return false;
    if (g_pfnSelDone)
        g_pfnSelDone(TEST_WIDGET, &clipAtom, &target);
    return true;
}

/* Set the current X selection data */
static void clipSetSelectionValues(const char *pcszTarget, Atom type,
                                   const void *data,
                                   unsigned long count, int format)
{
    Atom clipAtom = clipGetAtom(NULL, "CLIPBOARD");
    g_selTarget = clipGetAtom(NULL, pcszTarget);
    g_selType = type;
    g_pSelData = data;
    g_cSelData = count;
    g_selFormat = format;
    if (g_pfnSelLose)
        g_pfnSelLose(TEST_WIDGET, &clipAtom);
    g_ownsSel = false;
    g_fX11Formats = 0;
}

char *XtMalloc(Cardinal size) { return (char *) RTMemAlloc(size); }

char *XGetAtomName(Display *display, Atom atom)
{
    AssertReturn((unsigned)atom < RT_ELEMENTS(g_aFormats) + 1, NULL);
    const char *pcszName = NULL;
    if (atom < 0x1000)
        return NULL;
    else if (0x1000 <= atom && atom < 0x2000)
    {
        unsigned index = atom - 0x1000;
        AssertReturn(index < RT_ELEMENTS(g_aFormats), NULL);
        pcszName = g_aFormats[index].pcszAtom;
    }
    else
    {
        unsigned index = atom - 0x2000;
        AssertReturn(index < RT_ELEMENTS(g_apszSupAtoms), NULL);
        pcszName = g_apszSupAtoms[index];
    }
    return (char *)RTMemDup(pcszName, sizeof(pcszName) + 1);
}

int XFree(void *data)
{
    RTMemFree(data);
    return 0;
}

void XFreeStringList(char **list)
{
    if (list)
        RTMemFree(*list);
    RTMemFree(list);
}

const char XtStrings [] = "";
_WidgetClassRec* applicationShellWidgetClass;
const char XtShellStrings [] = "";

#define MAX_BUF_SIZE 256

static bool testStringFromX11(VBOXCLIPBOARDCONTEXTX11 *pCtx, uint32_t cbBuf,
                              const char *pcszExp, int rcExp)
{
    bool retval = false;
    AssertReturn(cbBuf <= MAX_BUF_SIZE, false);
    if (!clipPollTargets())
        RTPrintf("Failed to poll for targets\n");
    else if (clipQueryFormats() != VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT)
        RTPrintf("Wrong targets reported: %02X\n", clipQueryFormats());
    else
    {
        char pc[MAX_BUF_SIZE];
        uint32_t cbActual;
        VBOXCLIPBOARDREQUEST req = { pc, cbBuf, &cbActual, pCtx };
        int rc = VBoxX11ClipboardReadX11Data(pCtx,
                                     VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT,
                                     &req);
        if (rc != rcExp)
            RTPrintf("Wrong return code, expected %Rrc, got %Rrc\n", rcExp,
                     rc);
        else if (RT_FAILURE(rcExp))
            retval = true;
        else
        {
            RTUTF16 wcExp[MAX_BUF_SIZE / 2];
            RTUTF16 *pwcExp = wcExp;
            size_t cwc = 0;
            rc = RTStrToUtf16Ex(pcszExp, RTSTR_MAX, &pwcExp,
                                RT_ELEMENTS(wcExp), &cwc);
            size_t cbExp = cwc * 2 + 2;
            AssertRC(rc);
            if (RT_SUCCESS(rc))
            {
                if (cbActual != cbExp)
                {
                    RTPrintf("Returned string is the wrong size, string \"%.*ls\", size %u\n",
                             RT_MIN(MAX_BUF_SIZE, cbActual), pc, cbActual);
                    RTPrintf("Expected \"%s\", size %u\n", pcszExp,
                             cbExp);
                }
                else
                {
                    if (memcmp(pc, wcExp, cbExp) == 0)
                        retval = true;
                    else
                        RTPrintf("Returned string \"%.*ls\" does not match expected string \"%s\"\n",
                                 MAX_BUF_SIZE, pc, pcszExp);
                }
            }
        }
    }
    if (!retval)
        RTPrintf("Expected: string \"%s\", rc %Rrc (buffer size %u)\n",
                 pcszExp, rcExp, cbBuf);
    return retval;
}

static bool testLatin1FromX11(VBOXCLIPBOARDCONTEXTX11 *pCtx, uint32_t cbBuf,
                              const char *pcszExp, int rcExp)
{
    bool retval = false;
    AssertReturn(cbBuf <= MAX_BUF_SIZE, false);
    if (!clipPollTargets())
        RTPrintf("Failed to poll for targets\n");
    else if (clipQueryFormats() != VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT)
        RTPrintf("Wrong targets reported: %02X\n", clipQueryFormats());
    else
    {
        char pc[MAX_BUF_SIZE];
        uint32_t cbActual;
        VBOXCLIPBOARDREQUEST req = { pc, cbBuf, &cbActual, pCtx };
        int rc = VBoxX11ClipboardReadX11Data(pCtx,
                                     VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT,
                                     &req);
        if (rc != rcExp)
            RTPrintf("Wrong return code, expected %Rrc, got %Rrc\n", rcExp,
                     rc);
        else if (RT_FAILURE(rcExp))
            retval = true;
        else
        {
            RTUTF16 wcExp[MAX_BUF_SIZE / 2];
            RTUTF16 *pwcExp = wcExp;
            size_t cwc;
            for (cwc = 0; cwc == 0 || pcszExp[cwc - 1] != '\0'; ++cwc)
                wcExp[cwc] = pcszExp[cwc];
            size_t cbExp = cwc * 2;
            if (cbActual != cbExp)
            {
                RTPrintf("Returned string is the wrong size, string \"%.*ls\", size %u\n",
                         RT_MIN(MAX_BUF_SIZE, cbActual), pc, cbActual);
                RTPrintf("Expected \"%s\", size %u\n", pcszExp,
                         cbExp);
            }
            else
            {
                if (memcmp(pc, wcExp, cbExp) == 0)
                    retval = true;
                else
                    RTPrintf("Returned string \"%.*ls\" does not match expected string \"%s\"\n",
                             MAX_BUF_SIZE, pc, pcszExp);
            }
        }
    }
    if (!retval)
        RTPrintf("Expected: string \"%s\", rc %Rrc (buffer size %u)\n",
                 pcszExp, rcExp, cbBuf);
    return retval;
}

static bool testOverflowFromX11(VBOXCLIPBOARDCONTEXTX11 *pCtx, uint32_t cbBuf,
                                const char *pcsz, uint32_t cbExp)
{
    bool retval = false;
    AssertReturn(cbBuf <= MAX_BUF_SIZE, false);
    if (!clipPollTargets())
        RTPrintf("Failed to poll for targets\n");
    else if (clipQueryFormats() != VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT)
        RTPrintf("Wrong targets reported: %02X\n", clipQueryFormats());
    else
    {
        char pc[MAX_BUF_SIZE];
        uint32_t cbActual;
        VBOXCLIPBOARDREQUEST req = { pc, cbBuf, &cbActual, pCtx };
        int rc = VBoxX11ClipboardReadX11Data(pCtx,
                                     VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT,
                                     &req);
        if (rc != VINF_SUCCESS)
            RTPrintf("Wrong return code, expected %Rrc, got %Rrc\n",
                     VINF_SUCCESS, rc);
        else if (cbActual != cbExp)
            RTPrintf("Wrong actual size, expected %u, got %u\n",
                     cbExp, cbActual);
        else
            retval = true;
    }
    if (!retval)
        RTPrintf("String tested: \"%s\", size expected: %u\n", pcsz, cbExp);
    return retval;
}

static bool testStringFromVBox(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                               const char *pcszTarget, Atom typeExp,
                               const void *valueExp, unsigned long lenExp,
                               int formatExp)
{
    bool retval = false;
    Atom type;
    XtPointer value = NULL;
    unsigned long length;
    int format;
    if (clipConvertSelection(pcszTarget, &type, &value, &length, &format))
    {
        if (   type != typeExp
            || length != lenExp
            || format != formatExp
            || memcmp((const void *) value, (const void *)valueExp,
                      lenExp))
        {
            RTPrintf("Bad data: type %d, (expected %d), length %u, (%u), format %d (%d),\n",
                     type, typeExp, length, lenExp, format, formatExp);
            RTPrintf("value \"%.*s\" (\"%.*s\")", RT_MIN(length, 20), value,
                     RT_MIN(lenExp, 20), valueExp);
        }
        else
            retval = true;
    }
    else
        RTPrintf("Conversion failed\n");
    XtFree((char *)value);
    if (!retval)
        RTPrintf("Conversion to %s, expected \"%s\"\n", pcszTarget, valueExp);
    return retval;
}

static bool testStringFromVBoxFailed(VBOXCLIPBOARDCONTEXTX11 *pCtx,
                                     const char *pcszTarget)
{
    bool retval = false;
    Atom type;
    XtPointer value = NULL;
    unsigned long length;
    int format;
    if (!clipConvertSelection(pcszTarget, &type, &value, &length, &format))
        retval = true;
    XtFree((char *)value);
    if (!retval)
    {
        RTPrintf("Conversion to target %s, should have failed but didn't\n",
                 pcszTarget);
        RTPrintf("Returned type %d, length %u, format %d, value \"%.*s\"\n",
                 type, length, format, RT_MIN(length, 20), value);
    }
    return retval;
}

int main()
{
    RTR3Init();
    VBOXCLIPBOARDCONTEXTX11 *pCtx = VBoxX11ClipboardConstructX11(NULL);
    unsigned cErrs = 0;
    char pc[MAX_BUF_SIZE];
    uint32_t cbActual;
    int rc = VBoxX11ClipboardStartX11(pCtx, true);
    AssertRCReturn(rc, 1);

    /*** Utf-8 from X11 ***/
    RTPrintf(TEST_NAME ": TESTING reading Utf-8 from X11\n");
    /* Simple test */
    clipSetSelectionValues("UTF8_STRING", XA_STRING, "hello world",
                           sizeof("hello world"), 8);
    if (!testStringFromX11(pCtx, 256, "hello world", VINF_SUCCESS))
        ++cErrs;
    /* Receiving buffer of the exact size needed */
    if (!testStringFromX11(pCtx, sizeof("hello world") * 2, "hello world",
                           VINF_SUCCESS))
        ++cErrs;
    /* Buffer one too small */
    if (!testOverflowFromX11(pCtx, sizeof("hello world") * 2 - 1,
                             "hello world", sizeof("hello world") * 2))
        ++cErrs;
    /* Zero-size buffer */
    if (!testOverflowFromX11(pCtx, 0, "hello world",
                             sizeof("hello world") * 2))
        ++cErrs;
    /* With an embedded carriage return */
    clipSetSelectionValues("text/plain;charset=UTF-8", XA_STRING,
                           "hello\nworld", sizeof("hello\nworld"), 8);
    if (!testStringFromX11(pCtx, sizeof("hello\r\nworld") * 2,
                           "hello\r\nworld", VINF_SUCCESS))
        ++cErrs;
    /* An empty string */
    clipSetSelectionValues("text/plain;charset=utf-8", XA_STRING, "",
                           sizeof(""), 8);
    if (!testStringFromX11(pCtx, sizeof("") * 2, "", VINF_SUCCESS))
        ++cErrs;
    /* With an embedded Utf-8 character. */
    clipSetSelectionValues("STRING", XA_STRING,
                           "100\xE2\x82\xAC" /* 100 Euro */,
                           sizeof("100\xE2\x82\xAC"), 8);
    if (!testStringFromX11(pCtx, sizeof("100\xE2\x82\xAC") * 2,
                           "100\xE2\x82\xAC", VINF_SUCCESS))
        ++cErrs;
    /* A non-zero-terminated string */
    clipSetSelectionValues("TEXT", XA_STRING,
                           "hello world", sizeof("hello world") - 2, 8);
    if (!testStringFromX11(pCtx, sizeof("hello world") * 2 - 2,
                           "hello worl", VINF_SUCCESS))
        ++cErrs;

    /*** COMPOUND TEXT from X11 ***/
    RTPrintf(TEST_NAME ": TESTING reading compound text from X11\n");
    /* Simple test */
    clipSetSelectionValues("COMPOUND_TEXT", XA_STRING, "hello world",
                           sizeof("hello world"), 8);
    if (!testStringFromX11(pCtx, 256, "hello world", VINF_SUCCESS))
        ++cErrs;
    /* Receiving buffer of the exact size needed */
    if (!testStringFromX11(pCtx, sizeof("hello world") * 2, "hello world",
                           VINF_SUCCESS))
        ++cErrs;
    /* Buffer one too small */
    if (!testOverflowFromX11(pCtx, sizeof("hello world") * 2 - 1,
                             "hello world", sizeof("hello world") * 2))
        ++cErrs;
    /* Zero-size buffer */
    if (!testOverflowFromX11(pCtx, 0, "hello world",
                             sizeof("hello world") * 2))
        ++cErrs;
    /* With an embedded carriage return */
    clipSetSelectionValues("COMPOUND_TEXT", XA_STRING, "hello\nworld",
                           sizeof("hello\nworld"), 8);
    if (!testStringFromX11(pCtx, sizeof("hello\r\nworld") * 2,
                           "hello\r\nworld", VINF_SUCCESS))
        ++cErrs;
    /* An empty string */
    clipSetSelectionValues("COMPOUND_TEXT", XA_STRING, "",
                           sizeof(""), 8);
    if (!testStringFromX11(pCtx, sizeof("") * 2, "", VINF_SUCCESS))
        ++cErrs;
    /* A non-zero-terminated string */
    clipSetSelectionValues("COMPOUND_TEXT", XA_STRING,
                           "hello world", sizeof("hello world") - 2, 8);
    if (!testStringFromX11(pCtx, sizeof("hello world") * 2 - 2,
                           "hello worl", VINF_SUCCESS))
        ++cErrs;

    /*** Latin1 from X11 ***/
    RTPrintf(TEST_NAME ": TESTING reading Latin1 from X11\n");
    /* Simple test */
    clipSetSelectionValues("STRING", XA_STRING, "Georges Dupr\xEA",
                           sizeof("Georges Dupr\xEA"), 8);
    if (!testLatin1FromX11(pCtx, 256, "Georges Dupr\xEA", VINF_SUCCESS))
        ++cErrs;
    /* Receiving buffer of the exact size needed */
    if (!testLatin1FromX11(pCtx, sizeof("Georges Dupr\xEA") * 2,
                           "Georges Dupr\xEA", VINF_SUCCESS))
        ++cErrs;
    /* Buffer one too small */
    if (!testOverflowFromX11(pCtx, sizeof("Georges Dupr\xEA") * 2 - 1,
                             "hello world", sizeof("Georges Dupr\xEA") * 2))
        ++cErrs;
    /* Zero-size buffer */
    if (!testOverflowFromX11(pCtx, 0, "Georges Dupr\xEA",
                             sizeof("Georges Dupr\xEA") * 2))
        ++cErrs;
    /* With an embedded carriage return */
    clipSetSelectionValues("TEXT", XA_STRING, "Georges\nDupr\xEA",
                           sizeof("Georges\nDupr\xEA"), 8);
    if (!testLatin1FromX11(pCtx, sizeof("Georges\r\nDupr\xEA") * 2,
                           "Georges\r\nDupr\xEA", VINF_SUCCESS))
        ++cErrs;
    /* A non-zero-terminated string */
    clipSetSelectionValues("text/plain", XA_STRING,
                           "Georges Dupr\xEA!",
                           sizeof("Georges Dupr\xEA!") - 2, 8);
    if (!testLatin1FromX11(pCtx, sizeof("Georges Dupr\xEA!") * 2 - 2,
                           "Georges Dupr\xEA", VINF_SUCCESS))
        ++cErrs;


    /*** Timeout from X11 ***/
    RTPrintf(TEST_NAME ": TESTING X11 timeout\n");
    clipSetSelectionValues("UTF8_STRING", XT_CONVERT_FAIL, "hello world",
                           sizeof("hello world"), 8);
    VBOXCLIPBOARDREQUEST req = { pc, sizeof(pc), &cbActual, pCtx };
    rc = VBoxX11ClipboardReadX11Data(pCtx,
                                     VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT,
                                     &req);
    if (rc != VINF_SUCCESS || cbActual != 0)
    {
        RTPrintf("Bad return values: rc=%Rrc (expected %Rrc), cbActual=%u (%u)\n",
                 rc, VINF_SUCCESS, cbActual, 0);
        ++cErrs;
    }

    /*** No data in X11 clipboard ***/
    RTPrintf(TEST_NAME ": TESTING a data request from an empty X11 clipboard\n");
    clipSetSelectionValues("UTF8_STRING", XA_STRING, NULL,
                           0, 8);
    rc = VBoxX11ClipboardReadX11Data(pCtx,
                                     VBOX_SHARED_CLIPBOARD_FMT_UNICODETEXT,
                                     &req);
    if (rc != VINF_SUCCESS || cbActual != 0)
    {
        RTPrintf("Bad return values: rc=%Rrc (expected %Rrc), cbActual=%u (%u)\n",
                 rc, VINF_SUCCESS, cbActual, 0);
        ++cErrs;
    }

    /*** request for an invalid VBox format from X11 ***/
    RTPrintf(TEST_NAME ": TESTING a request for an invalid host format from X11\n");
    rc = VBoxX11ClipboardReadX11Data(pCtx, 0xffff, &req);
    if (rc != VINF_SUCCESS || cbActual != 0)
    {
        RTPrintf("Bad return values: rc=%Rrc (expected %Rrc), cbActual=%u (%u)\n",
                 rc, VINF_SUCCESS, cbActual, 0);
        ++cErrs;
    }

    /*** Utf-8 from VBox ***/
    RTPrintf(TEST_NAME ": TESTING reading Utf-8 from VBox\n");
    /* Simple test */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "hello world",
                     sizeof("hello world") * 2);
    if (!testStringFromVBox(pCtx, "UTF8_STRING",
                            clipGetAtom(NULL, "UTF8_STRING"),
                            "hello world", sizeof("hello world"), 8))
        ++cErrs;
    /* With an embedded carriage return */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "hello\r\nworld",
                     sizeof("hello\r\nworld") * 2);
    if (!testStringFromVBox(pCtx, "text/plain;charset=UTF-8",
                            clipGetAtom(NULL, "text/plain;charset=UTF-8"),
                            "hello\nworld", sizeof("hello\nworld"), 8))
        ++cErrs;
    /* An empty string */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "", 2);
    if (!testStringFromVBox(pCtx, "text/plain;charset=utf-8",
                            clipGetAtom(NULL, "text/plain;charset=utf-8"),
                            "", sizeof(""), 8))
        ++cErrs;
    /* With an embedded Utf-8 character. */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "100\xE2\x82\xAC" /* 100 Euro */,
                     10);
    if (!testStringFromVBox(pCtx, "STRING",
                            clipGetAtom(NULL, "STRING"),
                            "100\xE2\x82\xAC", sizeof("100\xE2\x82\xAC"), 8))
        ++cErrs;
    /* A non-zero-terminated string */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "hello world",
                     sizeof("hello world") * 2 - 4);
    if (!testStringFromVBox(pCtx, "TEXT",
                            clipGetAtom(NULL, "TEXT"),
                            "hello worl", sizeof("hello worl"), 8))
        ++cErrs;

    /*** COMPOUND TEXT from VBox ***/
    RTPrintf(TEST_NAME ": TESTING reading COMPOUND TEXT from VBox\n");
    /* Simple test */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "hello world",
                     sizeof("hello world") * 2);
    if (!testStringFromVBox(pCtx, "COMPOUND_TEXT",
                            clipGetAtom(NULL, "COMPOUND_TEXT"),
                            "hello world", sizeof("hello world"), 8))
        ++cErrs;
    /* With an embedded carriage return */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "hello\r\nworld",
                     sizeof("hello\r\nworld") * 2);
    if (!testStringFromVBox(pCtx, "COMPOUND_TEXT",
                            clipGetAtom(NULL, "COMPOUND_TEXT"),
                            "hello\nworld", sizeof("hello\nworld"), 8))
        ++cErrs;
    /* An empty string */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "", 2);
    if (!testStringFromVBox(pCtx, "COMPOUND_TEXT",
                            clipGetAtom(NULL, "COMPOUND_TEXT"),
                            "", sizeof(""), 8))
        ++cErrs;
    /* A non-zero-terminated string */
    clipSetVBoxUtf16(pCtx, VINF_SUCCESS, "hello world",
                     sizeof("hello world") * 2 - 4);
    if (!testStringFromVBox(pCtx, "COMPOUND_TEXT",
                            clipGetAtom(NULL, "COMPOUND_TEXT"),
                            "hello worl", sizeof("hello worl"), 8))
        ++cErrs;

    /*** Timeout from VBox ***/
    RTPrintf(TEST_NAME ": TESTING reading from VBox with timeout\n");
    clipEmptyVBox(pCtx, VERR_TIMEOUT);
    if (!testStringFromVBoxFailed(pCtx, "UTF8_STRING"))
        ++cErrs;

    /*** No data in VBox clipboard ***/
    RTPrintf(TEST_NAME ": TESTING reading from VBox with no data\n");
    clipEmptyVBox(pCtx, VINF_SUCCESS);
    if (!testStringFromVBoxFailed(pCtx, "UTF8_STRING"))
        ++cErrs;
    if (cErrs > 0)
        RTPrintf("Failed with %u error(s)\n", cErrs);
    return cErrs > 0 ? 1 : 0;
}

#endif

#ifdef SMOKETEST

/* This is a simple test case that just starts a copy of the X11 clipboard
 * backend, checks the X11 clipboard and exits.  If ever needed I will add an
 * interactive mode in which the user can read and copy to the clipboard from
 * the command line. */

#include <iprt/initterm.h>
#include <iprt/stream.h>

#define TEST_NAME "tstClipboardX11Smoke"

int VBoxX11ClipboardReadVBoxData(VBOXCLIPBOARDCONTEXT *pCtx,
                                 uint32_t u32Format, void **ppv,
                                 uint32_t *pcb)
{
    return VERR_NO_DATA;
}

void VBoxX11ClipboardReportX11Formats(VBOXCLIPBOARDCONTEXT *pCtx,
                                      uint32_t u32Formats)
{}

int main()
{
    int rc = VINF_SUCCESS;
    RTR3Init();
    /* We can't test anything without an X session, so just return success
     * in that case. */
    if (!RTEnvGet("DISPLAY"))
    {
        RTPrintf(TEST_NAME ": X11 not available, not running test\n");
        return 0;
    }
    RTPrintf(TEST_NAME ": TESTING\n");
    VBOXCLIPBOARDCONTEXTX11 *pCtx = VBoxX11ClipboardConstructX11(NULL);
    AssertReturn(pCtx, 1);
    rc = VBoxX11ClipboardStartX11(pCtx, true);
    AssertRCReturn(rc, 1);
    /* Give the clipboard time to synchronise. */
    RTThreadSleep(500);
    rc = VBoxX11ClipboardStopX11(pCtx);
    AssertRCReturn(rc, 1);
    VBoxX11ClipboardDestructX11(pCtx);
    return 0;
}

#endif /* SMOKETEST defined */
