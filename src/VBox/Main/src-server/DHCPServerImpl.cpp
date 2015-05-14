/* $Id: DHCPServerImpl.cpp $ */

/** @file
 *
 * VirtualBox COM class implementation
 */

/*
 * Copyright (C) 2006-2011 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#include <string>
#include "NetworkServiceRunner.h"
#include "DHCPServerImpl.h"
#include "AutoCaller.h"
#include "Logging.h"

#include <iprt/cpp/utils.h>

#include <VBox/com/array.h>
#include <VBox/settings.h>

#include "VirtualBoxImpl.h"

// constructor / destructor
/////////////////////////////////////////////////////////////////////////////
const std::string DHCPServerRunner::kDsrKeyGateway = "--gateway";
const std::string DHCPServerRunner::kDsrKeyLowerIp = "--lower-ip";
const std::string DHCPServerRunner::kDsrKeyUpperIp = "--upper-ip";


struct DHCPServer::Data
{
    Data()
        : enabled(FALSE)
        , router(false)
    {}

    Bstr IPAddress;
    Bstr lowerIP;
    Bstr upperIP;

    BOOL enabled;
    bool router;
    DHCPServerRunner dhcp;

    DhcpOptionMap GlobalDhcpOptions;
    VmSlot2OptionsMap VmSlot2Options;
};


DHCPServer::DHCPServer()
  : m(NULL)
  , mVirtualBox(NULL)
{
    m = new DHCPServer::Data();
}


DHCPServer::~DHCPServer()
{
    if (m)
    {
        delete m;
        m = NULL;
    }
}


HRESULT DHCPServer::FinalConstruct()
{
    return BaseFinalConstruct();
}


void DHCPServer::FinalRelease()
{
    uninit ();

    BaseFinalRelease();
}


void DHCPServer::uninit()
{
    /* Enclose the state transition Ready->InUninit->NotReady */
    AutoUninitSpan autoUninitSpan(this);
    if (autoUninitSpan.uninitDone())
        return;

    unconst(mVirtualBox) = NULL;
}


HRESULT DHCPServer::init(VirtualBox *aVirtualBox, IN_BSTR aName)
{
    AssertReturn(aName != NULL, E_INVALIDARG);

    AutoInitSpan autoInitSpan(this);
    AssertReturn(autoInitSpan.isOk(), E_FAIL);

    /* share VirtualBox weakly (parent remains NULL so far) */
    unconst(mVirtualBox) = aVirtualBox;

    unconst(mName) = aName;
    m->IPAddress = "0.0.0.0";
    m->GlobalDhcpOptions[DhcpOpt_SubnetMask] = DhcpOptValue("0.0.0.0");
    m->enabled = FALSE;

    m->lowerIP = "0.0.0.0";
    m->upperIP = "0.0.0.0";

    /* Confirm a successful initialization */
    autoInitSpan.setSucceeded();

    return S_OK;
}


HRESULT DHCPServer::init(VirtualBox *aVirtualBox,
                         const settings::DHCPServer &data)
{
    /* Enclose the state transition NotReady->InInit->Ready */
    AutoInitSpan autoInitSpan(this);
    AssertReturn(autoInitSpan.isOk(), E_FAIL);

    /* share VirtualBox weakly (parent remains NULL so far) */
    unconst(mVirtualBox) = aVirtualBox;

    unconst(mName) = data.strNetworkName;
    m->IPAddress = data.strIPAddress;
    m->enabled = data.fEnabled;
    m->lowerIP = data.strIPLower;
    m->upperIP = data.strIPUpper;

    m->GlobalDhcpOptions.clear();
    m->GlobalDhcpOptions.insert(data.GlobalDhcpOptions.begin(),
                               data.GlobalDhcpOptions.end());

    m->VmSlot2Options.clear();
    m->VmSlot2Options.insert(data.VmSlot2OptionsM.begin(),
                            data.VmSlot2OptionsM.end());

    autoInitSpan.setSucceeded();

    return S_OK;
}


HRESULT DHCPServer::saveSettings(settings::DHCPServer &data)
{
    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    data.strNetworkName = mName;
    data.strIPAddress = m->IPAddress;

    data.fEnabled = !!m->enabled;
    data.strIPLower = m->lowerIP;
    data.strIPUpper = m->upperIP;

    data.GlobalDhcpOptions.clear();
    data.GlobalDhcpOptions.insert(m->GlobalDhcpOptions.begin(),
                                  m->GlobalDhcpOptions.end());

    data.VmSlot2OptionsM.clear();
    data.VmSlot2OptionsM.insert(m->VmSlot2Options.begin(),
                                m->VmSlot2Options.end());

    return S_OK;
}


STDMETHODIMP DHCPServer::COMGETTER(NetworkName) (BSTR *aName)
{
    CheckComArgOutPointerValid(aName);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    mName.cloneTo(aName);
    return S_OK;
}


STDMETHODIMP DHCPServer::COMGETTER(Enabled) (BOOL *aEnabled)
{
    CheckComArgOutPointerValid(aEnabled);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    *aEnabled = m->enabled;
    return S_OK;
}


STDMETHODIMP DHCPServer::COMSETTER(Enabled) (BOOL aEnabled)
{
    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoWriteLock alock(this COMMA_LOCKVAL_SRC_POS);
    m->enabled = aEnabled;

    // save the global settings; for that we should hold only the VirtualBox lock
    alock.release();
    AutoWriteLock vboxLock(mVirtualBox COMMA_LOCKVAL_SRC_POS);
    HRESULT rc = mVirtualBox->saveSettings();

    return rc;
}


STDMETHODIMP DHCPServer::COMGETTER(IPAddress) (BSTR *aIPAddress)
{
    CheckComArgOutPointerValid(aIPAddress);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    m->IPAddress.cloneTo(aIPAddress);
    return S_OK;
}


STDMETHODIMP DHCPServer::COMGETTER(NetworkMask) (BSTR *aNetworkMask)
{
    CheckComArgOutPointerValid(aNetworkMask);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    m->GlobalDhcpOptions[DhcpOpt_SubnetMask].text.cloneTo(aNetworkMask);
    return S_OK;
}


STDMETHODIMP DHCPServer::COMGETTER(LowerIP) (BSTR *aIPAddress)
{
    CheckComArgOutPointerValid(aIPAddress);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    m->lowerIP.cloneTo(aIPAddress);
    return S_OK;
}


STDMETHODIMP DHCPServer::COMGETTER(UpperIP) (BSTR *aIPAddress)
{
    CheckComArgOutPointerValid(aIPAddress);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    m->upperIP.cloneTo(aIPAddress);
    return S_OK;
}


STDMETHODIMP DHCPServer::SetConfiguration (IN_BSTR aIPAddress, IN_BSTR aNetworkMask, IN_BSTR aLowerIP, IN_BSTR aUpperIP)
{
    AssertReturn(aIPAddress != NULL, E_INVALIDARG);
    AssertReturn(aNetworkMask != NULL, E_INVALIDARG);
    AssertReturn(aLowerIP != NULL, E_INVALIDARG);
    AssertReturn(aUpperIP != NULL, E_INVALIDARG);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoWriteLock alock(this COMMA_LOCKVAL_SRC_POS);
    m->IPAddress = aIPAddress;
    m->GlobalDhcpOptions[DhcpOpt_SubnetMask] = DhcpOptValue(aNetworkMask);

    m->lowerIP = aLowerIP;
    m->upperIP = aUpperIP;

    // save the global settings; for that we should hold only the VirtualBox lock
    alock.release();
    AutoWriteLock vboxLock(mVirtualBox COMMA_LOCKVAL_SRC_POS);
    return mVirtualBox->saveSettings();
}


HRESULT DHCPServer::encodeOption(com::Utf8Str &aEncoded,
                                 uint32_t aOptCode, const DhcpOptValue &aOptValue)
{
    switch (aOptValue.encoding)
    {
        case DhcpOptValue::LEGACY:
        {
            /*
             * This is original encoding which assumed that for each
             * option we know its format and so we know how option
             * "value" text is to be interpreted.
             *
             *   "2:10800"           # integer 32
             *   "6:1.2.3.4 8.8.8.8" # array of ip-address
             */
            aEncoded = Utf8StrFmt("%d:%s", aOptCode, aOptValue.text.c_str());
            break;
        }

        case DhcpOptValue::HEX:
        {
            /*
             * This is a bypass for any option - preformatted value as
             * hex string with no semantic involved in formatting the
             * value for the DHCP reply.
             *
             *   234=68:65:6c:6c:6f:2c:20:77:6f:72:6c:64
             */
            aEncoded = Utf8StrFmt("%d=%s", aOptCode, aOptValue.text.c_str());
            break;
        }

        default:
        {
            /*
             * Try to be forward compatible.
             *
             *   "254@42=i hope you know what this means"
             */
            aEncoded = Utf8StrFmt("%d@%d=%s", aOptCode, (int)aOptValue.encoding,
                                  aOptValue.text.c_str());
            break;
        }
    }

    return S_OK;
}


int DHCPServer::addOption(DhcpOptionMap &aMap,
                          DhcpOpt_T aOption, const com::Utf8Str &aValue)
{
    DhcpOptValue OptValue;

    if (aOption != 0)
    {
        OptValue = DhcpOptValue(aValue, DhcpOptValue::LEGACY);
    }
    /*
     * This is a kludge to sneak in option encoding information
     * through existing API.  We use option 0 and supply the real
     * option/value in the same format that encodeOption() above
     * produces for getter methods.
     */
    else
    {
        uint8_t u8Code;
        uint32_t u32Enc;
        char *pszNext;
        int rc;

        rc = RTStrToUInt8Ex(aValue.c_str(), &pszNext, 10, &u8Code);
        if (!RT_SUCCESS(rc))
            return VERR_PARSE_ERROR;

        switch (*pszNext)
        {
            case ':':           /* support legacy format too */
            {
                u32Enc = DhcpOptValue::LEGACY;
                break;
            }

            case '=':
            {
                u32Enc = DhcpOptValue::HEX;
                break;
            }

            case '@':
            {
                rc = RTStrToUInt32Ex(pszNext + 1, &pszNext, 10, &u32Enc);
                if (!RT_SUCCESS(rc))
                    return VERR_PARSE_ERROR;
                if (*pszNext != '=')
                    return VERR_PARSE_ERROR;
                break;
            }

            default:
                return VERR_PARSE_ERROR;
        }

        aOption = (DhcpOpt_T)u8Code;
        OptValue = DhcpOptValue(pszNext + 1, (DhcpOptValue::Encoding)u32Enc);
    }

    aMap[aOption] = OptValue;
    return VINF_SUCCESS;
}


STDMETHODIMP DHCPServer::AddGlobalOption(DhcpOpt_T aOption, IN_BSTR aValue)
{
    CheckComArgStr(aValue);
    /* store global option */
    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoWriteLock alock(this COMMA_LOCKVAL_SRC_POS);

    int rc = addOption(m->GlobalDhcpOptions, aOption, aValue);
    if (!RT_SUCCESS(rc))
        return E_INVALIDARG;

    /* Indirect way to understand that we're on NAT network */
    if (aOption == DhcpOpt_Router)
    {
        m->dhcp.setOption(NetworkServiceRunner::kNsrKeyNeedMain, "on");
        m->router = true;
    }

    alock.release();

    AutoWriteLock vboxLock(mVirtualBox COMMA_LOCKVAL_SRC_POS);
    return mVirtualBox->saveSettings();
}


STDMETHODIMP DHCPServer::COMGETTER(GlobalOptions)(ComSafeArrayOut(BSTR, aValues))
{
    CheckComArgOutSafeArrayPointerValid(aValues);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    SafeArray<BSTR> sf(m->GlobalDhcpOptions.size());
    int i = 0;

    for (DhcpOptIterator it = m->GlobalDhcpOptions.begin();
         it != m->GlobalDhcpOptions.end(); ++it)
    {
        uint32_t OptCode = (*it).first;
        const DhcpOptValue &OptValue = (*it).second;

        com::Utf8Str value;
        encodeOption(value, OptCode, OptValue);
        Bstr(value).detachTo(&sf[i]);
        i++;
    }

    sf.detachTo(ComSafeArrayOutArg(aValues));

    return S_OK;
}


STDMETHODIMP DHCPServer::COMGETTER(VmConfigs)(ComSafeArrayOut(BSTR, aValues))
{
    CheckComArgOutSafeArrayPointerValid(aValues);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    SafeArray<BSTR> sf(m->VmSlot2Options.size());
    VmSlot2OptionsIterator it = m->VmSlot2Options.begin();
    int i = 0;
    for (;it != m->VmSlot2Options.end(); ++it)
    {
        Bstr(Utf8StrFmt("[%s]:%d",
                        it->first.VmName.c_str(),
                        it->first.Slot)).detachTo(&sf[i]);
        i++;
    }

    sf.detachTo(ComSafeArrayOutArg(aValues));

    return S_OK;
}


STDMETHODIMP DHCPServer::AddVmSlotOption(IN_BSTR aVmName, LONG aSlot, DhcpOpt_T aOption, IN_BSTR aValue)
{
    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();
    AutoWriteLock alock(this COMMA_LOCKVAL_SRC_POS);

    DhcpOptionMap &map = m->VmSlot2Options[VmNameSlotKey(com::Utf8Str(aVmName), aSlot)];
    int rc = addOption(map, aOption, aValue);
    if (!RT_SUCCESS(rc))
        return E_INVALIDARG;

    alock.release();

    AutoWriteLock vboxLock(mVirtualBox COMMA_LOCKVAL_SRC_POS);
    return mVirtualBox->saveSettings();
}


STDMETHODIMP DHCPServer::RemoveVmSlotOptions(IN_BSTR aVmName, LONG aSlot)
{
    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();
    AutoWriteLock alock(this COMMA_LOCKVAL_SRC_POS);

    DhcpOptionMap& map = findOptMapByVmNameSlot(com::Utf8Str(aVmName), aSlot);

    map.clear();

    alock.release();

    AutoWriteLock vboxLock(mVirtualBox COMMA_LOCKVAL_SRC_POS);
    return mVirtualBox->saveSettings();
}


/**
 * this is mapping (vm, slot)
 */
STDMETHODIMP DHCPServer::GetVmSlotOptions(IN_BSTR aVmName,
                                          LONG aSlot,
                                          ComSafeArrayOut(BSTR, aValues))
{
    CheckComArgOutSafeArrayPointerValid(aValues);
    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);

    DhcpOptionMap& map = findOptMapByVmNameSlot(com::Utf8Str(aVmName), aSlot);

    SafeArray<BSTR> sf(map.size());
    int i = 0;

    for (DhcpOptIterator it = map.begin();
         it != map.end(); ++it)
    {
        uint32_t OptCode = (*it).first;
        const DhcpOptValue &OptValue = (*it).second;

        com::Utf8Str value;
        encodeOption(value, OptCode, OptValue);
        Bstr(value).detachTo(&sf[i]);
        i++;
    }

    sf.detachTo(ComSafeArrayOutArg(aValues));

    return S_OK;
}


STDMETHODIMP DHCPServer::GetMacOptions(IN_BSTR aMAC, ComSafeArrayOut(BSTR, aValues))
{
    CheckComArgOutSafeArrayPointerValid(aValues);

    AutoCaller autoCaller(this);
    if (FAILED(autoCaller.rc())) return autoCaller.rc();

    AutoReadLock alock(this COMMA_LOCKVAL_SRC_POS);
    HRESULT hrc = S_OK;

    ComPtr<IMachine> machine;
    ComPtr<INetworkAdapter> nic;

    VmSlot2OptionsIterator it;
    for(it = m->VmSlot2Options.begin();
        it != m->VmSlot2Options.end();
        ++it)
    {

        alock.release();
        hrc = mVirtualBox->FindMachine(Bstr(it->first.VmName).raw(), machine.asOutParam());
        alock.acquire();

        if (FAILED(hrc))
            continue;

        alock.release();
        hrc = machine->GetNetworkAdapter(it->first.Slot, nic.asOutParam());
        alock.acquire();

        if (FAILED(hrc))
            continue;

        com::Bstr mac;

        alock.release();
        hrc = nic->COMGETTER(MACAddress)(mac.asOutParam());
        alock.acquire();

        if (FAILED(hrc)) /* no MAC address ??? */
            break;

        if (!RTStrICmp(com::Utf8Str(mac).c_str(), com::Utf8Str(aMAC).c_str()))
            return GetVmSlotOptions(Bstr(it->first.VmName).raw(),
                                    it->first.Slot,
                                    ComSafeArrayOutArg(aValues));
    } /* end of for */

    return hrc;
}


STDMETHODIMP DHCPServer::COMGETTER(EventSource)(IEventSource **aEventSource)
{
    NOREF(aEventSource);
    ReturnComNotImplemented();
}


STDMETHODIMP DHCPServer::Start(IN_BSTR aNetworkName, IN_BSTR aTrunkName, IN_BSTR aTrunkType)
{
    /* Silently ignore attempts to run disabled servers. */
    if (!m->enabled)
        return S_OK;

    /* Commmon Network Settings */
    m->dhcp.setOption(NetworkServiceRunner::kNsrKeyNetwork, Utf8Str(aNetworkName).c_str());

    Bstr tmp(aTrunkName);

    if (!tmp.isEmpty())
        m->dhcp.setOption(NetworkServiceRunner::kNsrTrunkName, Utf8Str(tmp).c_str());
    m->dhcp.setOption(NetworkServiceRunner::kNsrKeyTrunkType, Utf8Str(aTrunkType).c_str());

    /* XXX: should this MAC default initialization moved to NetworkServiceRunner? */
    char strMAC[32];
    Guid guid;
    guid.create();
    RTStrPrintf (strMAC, sizeof(strMAC), "08:00:27:%02X:%02X:%02X",
                 guid.raw()->au8[0],
                 guid.raw()->au8[1],
                 guid.raw()->au8[2]);
    m->dhcp.setOption(NetworkServiceRunner::kNsrMacAddress, strMAC);
    m->dhcp.setOption(NetworkServiceRunner::kNsrIpAddress,  Utf8Str(m->IPAddress).c_str());
    m->dhcp.setOption(NetworkServiceRunner::kNsrIpNetmask, Utf8Str(m->GlobalDhcpOptions[DhcpOpt_SubnetMask].text).c_str());
    m->dhcp.setOption(DHCPServerRunner::kDsrKeyLowerIp, Utf8Str(m->lowerIP).c_str());
    m->dhcp.setOption(DHCPServerRunner::kDsrKeyUpperIp, Utf8Str(m->upperIP).c_str());

    /* XXX: This parameters Dhcp Server will fetch via API */
    return RT_FAILURE(m->dhcp.start(!m->router /* KillProcOnExit */)) ? E_FAIL : S_OK;
    //m->dhcp.detachFromServer(); /* need to do this to avoid server shutdown on runner destruction */
}


STDMETHODIMP DHCPServer::Stop (void)
{
    return RT_FAILURE(m->dhcp.stop()) ? E_FAIL : S_OK;
}


DhcpOptionMap& DHCPServer::findOptMapByVmNameSlot(const com::Utf8Str& aVmName,
                                                  LONG aSlot)
{
    return m->VmSlot2Options[settings::VmNameSlotKey(
          com::Utf8Str(aVmName),
          aSlot)];
}
