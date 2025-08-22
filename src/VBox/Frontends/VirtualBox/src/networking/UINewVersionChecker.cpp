/* $Id: UINewVersionChecker.cpp $ */
/** @file
 * VBox Qt GUI - UINewVersionChecker class implementation.
 */

/*
 * Copyright (C) 2006-2025 Oracle and/or its affiliates.
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

/* Qt includes: */
#include <QRegularExpression>
#include <QUrlQuery>

/* GUI includes: */
#include "UIExtraDataManager.h"
#include "UIGlobalSession.h"
#include "UINetworkReply.h"
#include "UINewVersionChecker.h"
#include "UINotificationCenter.h"
#include "UIUpdateDefs.h"
#include "UIVersion.h"
#ifdef Q_OS_LINUX
# include "QIProcess.h"
#endif

/* Other VBox includes: */
#if defined(RT_ARCH_AMD64) || defined(RT_ARCH_X86)
# include <iprt/asm-amd64-x86.h>
#endif
#include <iprt/mp.h>
#ifdef Q_OS_LINUX
# include <iprt/path.h>
#endif
#include <iprt/string.h>
#include <iprt/system.h>

/* COM includes: */
#include "CHost.h"


UINewVersionChecker::UINewVersionChecker(bool fForcedCall)
    : m_fForcedCall(fForcedCall)
    , m_url("https://update.virtualbox.org/query.php")
{
}

void UINewVersionChecker::start()
{
    /* Compose query: */
    QUrlQuery url;
    url.addQueryItem("platform", gpGlobalSession->virtualBox().GetPackageType());
    /* Check if branding is active: */
    if (UIVersionInfo::brandingIsActive())
    {
        /* Branding: Check whether we have a local branding file which tells us our version suffix "FOO"
                     (e.g. 3.06.54321_FOO) to identify this installation: */
        url.addQueryItem("version", QString("%1_%2_%3").arg(gpGlobalSession->virtualBox().GetVersion())
                                                       .arg(gpGlobalSession->virtualBox().GetRevision())
                                                       .arg(UIVersionInfo::brandingGetKey("VerSuffix")));
    }
    else
    {
        /* Use hard coded version set by VBOX_VERSION_STRING: */
        url.addQueryItem("version", QString("%1_%2").arg(gpGlobalSession->virtualBox().GetVersion())
                                                    .arg(gpGlobalSession->virtualBox().GetRevision()));
    }
    url.addQueryItem("count", QString::number(gEDataManager->applicationUpdateCheckCounter()));
    url.addQueryItem("branch", VBoxUpdateData(gEDataManager->applicationUpdateData()).updateChannelName());
    const QString strUserAgent(QString("VirtualBox %1 <%2>").arg(gpGlobalSession->virtualBox().GetVersion()).arg(platformInfo()));
    AssertMsgFailed(("TODO: Check the user agent string (HW: xxx). Code is currently orphaned and can't be tested.\n"
                     "User-Agent: %s\n", strUserAgent.toUtf8().constData()));

    /* Send GET request: */
    UserDictionary headers;
    headers["User-Agent"] = strUserAgent;
    QUrl fullUrl(m_url);
    fullUrl.setQuery(url);
    createNetworkRequest(UINetworkRequestType_GET, QList<QUrl>() << fullUrl, QString(), headers);
}

void UINewVersionChecker::cancel()
{
    cancelNetworkRequest();
}

void UINewVersionChecker::processNetworkReplyProgress(qint64, qint64)
{
}

void UINewVersionChecker::processNetworkReplyFailed(const QString &strError)
{
    emit sigProgressFailed(strError);
}

void UINewVersionChecker::processNetworkReplyCanceled(UINetworkReply *)
{
    emit sigProgressCanceled();
}

void UINewVersionChecker::processNetworkReplyFinished(UINetworkReply *pReply)
{
    /* Deserialize incoming data: */
    const QString strResponseData(pReply->readAll());

#ifdef VBOX_NEW_VERSION_TEST
    strResponseData = VBOX_NEW_VERSION_TEST;
#endif
    /* Newer version of necessary package found: */
    if (strResponseData.indexOf(QRegularExpression("^\\d+\\.\\d+\\.\\d+(_[0-9A-Z]+)? \\S+$")) == 0)
    {
        const QStringList response = strResponseData.split(" ", Qt::SkipEmptyParts);
        UINotificationMessage::showUpdateSuccess(response[0], response[1]);
    }
    /* No newer version of necessary package found: */
    else
    {
        if (isItForcedCall())
            UINotificationMessage::showUpdateNotFound();
    }

    /* Increment update check counter: */
    gEDataManager->incrementApplicationUpdateCheckCounter();

    /* Notify about completion: */
    emit sigProgressFinished();
}

/** A platformInfo() helper that replaces bad characters with spaces. */
static char *sanitizeUserAgentString(char *psz, const char *pszBadChars = "|[]:;<>")
{
    char * const pszRet = psz;
    RTStrPurgeEncoding(psz);
    while ((psz = strpbrk(psz, pszBadChars)) != NULL)
        *psz++ = ' ';
    return pszRet;
}

/**
 * @note This code is duplicated in Main UpdateAgentImpl.cpp.  Changes must be
 *       applied in both places.
 */
/* static */
QString UINewVersionChecker::platformInfo()
{
    char szTmp[256];
    int vrc;

    /* Prepare platform report: */
    QString strPlatform;

    /*
     * The format is <system>.<bitness>:
     */
#if defined(RT_OS_WINDOWS)
    strPlatform = "win";
#elif defined(RT_OS_LINUX)
    strPlatform = "linux";
#elif defined(RT_OS_DARWIN)
    strPlatform = "macosx";
#elif defined(RT_OS_OS2)
    strPlatform = "os2";
#elif defined(RT_OS_FREEBSD)
    strPlatform = "freebsd";
#elif defined(RT_OS_SOLARIS)
    strPlatform = "solaris";
#else
    strPlatform = "unknown";
#endif
    strPlatform.append('.');
#if  defined(RT_ARCH_AMD64)
    strPlatform.append("amd64");
#elif defined(RT_ARCH_ARM64)
    strPlatform.append("arm64");
#elif defined(RT_ARCH_X86)
    strPlatform.append("x86");
#else
# error "Unexpected RT_ARCH_XXX"
#endif

    /*
     * Get some very basic HW info.
     * Do this before the linux stuff, as it may dump too much info on us.
     */
    uint64_t cbRam = 0;
    RTSystemQueryTotalRam(&cbRam);
    strPlatform.append(QString(" [HW: ram=%1M;cpus=%2:").arg(cbRam / _1M).arg(RTMpGetOnlineCount()));

# if defined(RT_ARCH_AMD64) || defined(RT_ARCH_X86)
    /* Get the CPU vendor (shorten it if possible) and basic model info. */
    uint32_t uEax, uEbx, uEcx, uEdx;
    ASMCpuIdExSlow(0, 0, 0, 0, &uEax, &uEbx, &uEcx, &uEdx);
    const char *pszVendor;
    if (RTX86IsAmdCpu(uEbx, uEcx, uEdx))                pszVendor = "AMD";
    else if (RTX86IsIntelCpu(uEbx, uEcx, uEdx))         pszVendor = "Intel";
    else if (RTX86IsViaCentaurCpu(uEbx, uEcx, uEdx))    pszVendor = "Via";
    else if (RTX86IsShanghaiCpu(uEbx, uEcx, uEdx))      pszVendor = "Shanghai";
    else if (RTX86IsHygonCpu(uEbx, uEcx, uEdx))         pszVendor = "Hygon";
    else
    {
        ((uint32_t *)szTmp)[0] = uEbx;
        ((uint32_t *)szTmp)[1] = uEdx;
        ((uint32_t *)szTmp)[2] = uEcx;
        ((uint32_t *)szTmp)[3] = 0;
        pszVendor = RTStrStrip(sanitizeUserAgentString(szTmp));
    }
    ASMCpuIdExSlow(1, 0, 0, 0, &uEax, &uEbx, &uEcx, &uEdx);
    strPlatform.append(QString("%1:0x%2").arg(pszVendor).arg(uEax, 0, 16));

# else
    vrc = RTMpGetDescription(NIL_RTCPUID, szTmp, RT_MIN(sizeof(szTmp), 64));
    if (RT_SUCCESS(vrc) || vrc == VERR_BUFFER_OVERFLOW)
        sanitizeUserAgentString(szTmp);
    else
        szTmp[0] = '\0';
    strPlatform.append(RTStrStrip(szTmp));
# endif

    /* NEM & HM support for the host architecture: */
    bool fIsNativeApiSupported = false;
    bool fIsHwVirtSupported = false;
# if defined(RT_ARCH_ARM64) || defined(RT_ARCH_AMD64) || defined(RT_ARCH_X86)
    CHost comHost = gpGlobalSession->host();
#  if defined(RT_ARCH_AMD64) || defined(RT_ARCH_X86)
    fIsNativeApiSupported = comHost.IsExecutionEngineSupported(KCPUArchitecture_x86, KVMExecutionEngine_NativeApi) != FALSE
                         && comHost.isOk();
    fIsHwVirtSupported    = comHost.IsExecutionEngineSupported(KCPUArchitecture_x86, KVMExecutionEngine_HwVirt) != FALSE
                         && comHost.isOk();
#  else
    fIsNativeApiSupported = comHost.IsExecutionEngineSupported(KCPUArchitecture_ARMv8_64, KVMExecutionEngine_NativeApi) != FALSE
                         && comHost.isOk();
#  endif
# else
#  error "port me"
# endif
    strPlatform.append(fIsNativeApiSupported ? ";NEM" : ";no-NEM");
    strPlatform.append(fIsHwVirtSupported    ? ":HM"  : ":no-HM");


#ifdef Q_OS_LINUX
    /*
     * WORKAROUND: On Linux we try to generate information using script first.
     *
     * It will get more information than what IPRT currently provide, with distro
     * details and such.
     */
    /* Get script path: */
    char szAppPrivPath[RTPATH_MAX];
    vrc = RTPathAppPrivateNoArch(szAppPrivPath, sizeof(szAppPrivPath));
    AssertRC(vrc);
    if (RT_SUCCESS(vrc))
    {
        /* Run script: */
        QByteArray result = QIProcess::singleShot(QString(szAppPrivPath) + "/VBoxSysInfo.sh");
        if (!result.isNull())
        {
            QString strTrimmed = QString(result).trimmed();
            if (!strTrimmed.isEmpty())
                strPlatform += QString(" | %1]").arg(QString(result).trimmed());
            else
                vrc = VERR_TRY_AGAIN; /* (take the fallback path) */
        }
        else
            vrc = VERR_TRY_AGAIN; /* (take the fallback path) */
    }
    if (RT_FAILURE(vrc))
#endif /* Q_OS_LINUX */
    {
        /*
         * Use RTSystemQueryOSInfo:
         */
        vrc = RTSystemQueryOSInfo(RTSYSOSINFO_PRODUCT, szTmp, sizeof(szTmp));
        if ((RT_SUCCESS(vrc) || vrc == VERR_BUFFER_OVERFLOW) && szTmp[0] != '\0')
            strPlatform += QString(" | Product: %1").arg(szTmp);

        vrc = RTSystemQueryOSInfo(RTSYSOSINFO_RELEASE, szTmp, sizeof(szTmp));
        if ((RT_SUCCESS(vrc) || vrc == VERR_BUFFER_OVERFLOW) && szTmp[0] != '\0')
            strPlatform += QString(" | Release: %1").arg(szTmp);

        vrc = RTSystemQueryOSInfo(RTSYSOSINFO_VERSION, szTmp, sizeof(szTmp));
        if ((RT_SUCCESS(vrc) || vrc == VERR_BUFFER_OVERFLOW) && szTmp[0] != '\0')
            strPlatform += QString(" | Version: %1").arg(szTmp);

        vrc = RTSystemQueryOSInfo(RTSYSOSINFO_SERVICE_PACK, szTmp, sizeof(szTmp));
        if ((RT_SUCCESS(vrc) || vrc == VERR_BUFFER_OVERFLOW) && szTmp[0] != '\0')
            strPlatform += QString(" | SP: %1").arg(szTmp);

        strPlatform.append("]");
    }

    return strPlatform;
}
