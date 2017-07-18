/* $Id: DevPciIch9.cpp $ */
/** @file
 * DevPCI - ICH9 southbridge PCI bus emulation device.
 *
 * @note    bird: I've cleaned up DevPCI.cpp to some extent, this file has not
 *                be cleaned up and because of pending code merge.
 *
 *          DO NOT use the PDMPciDev* or PCIDev* family of functions in this
 *          file except in the two callbacks for config space access (and the
 *          functions which are used exclusively by that code) and the two
 *          device constructors when setting up the config space for the
 *          bridges.  Everything else need extremely careful review.  Using
 *          them elsewhere (especially in the init code) causes weird failures
 *          with PCI passthrough, as it would only update the array of
 *          (emulated) config space, but not talk to the actual device (needs
 *          invoking the respective callback).
 */

/*
 * Copyright (C) 2010-2017 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#define LOG_GROUP LOG_GROUP_DEV_PCI
#define PCIBus    ICH9PCIBus        /**< HACK ALERT! Real ugly type hack! */
#define PDMPCIDEV_INCLUDE_PRIVATE  /* Hack to get pdmpcidevint.h included at the right point. */
#include <VBox/vmm/pdmpcidev.h>

#include <VBox/msi.h>
#include <VBox/vmm/pdmdev.h>
#include <VBox/vmm/mm.h>
#include <iprt/asm.h>
#include <iprt/assert.h>
#include <iprt/string.h>
#ifdef IN_RING3
# include <iprt/mem.h>
# include <iprt/uuid.h>
#endif

#include "PciInline.h"
#include "VBoxDD.h"
#include "MsiCommon.h"


/*********************************************************************************************************************************
*   Structures and Typedefs                                                                                                      *
*********************************************************************************************************************************/
/**
 * PCI Bus instance.
 */
typedef struct ICH9PCIBus
{
    /** Bus number. */
    int32_t             iBus;
    /** Number of bridges attached to the bus. */
    uint32_t            cBridges;

    /** Array of PCI devices. We assume 32 slots, each with 8 functions. */
    R3PTRTYPE(PPDMPCIDEV)   apDevices[256];
    /** Array of bridges attached to the bus. */
    R3PTRTYPE(PPDMPCIDEV *) papBridgesR3;

    /** R3 pointer to the device instance. */
    PPDMDEVINSR3        pDevInsR3;
    /** Pointer to the PCI R3  helpers. */
    PCPDMPCIHLPR3       pPciHlpR3;

    /** R0 pointer to the device instance. */
    PPDMDEVINSR0        pDevInsR0;
    /** Pointer to the PCI R0 helpers. */
    PCPDMPCIHLPR0       pPciHlpR0;

    /** RC pointer to the device instance. */
    PPDMDEVINSRC        pDevInsRC;
    /** Pointer to the PCI RC helpers. */
    PCPDMPCIHLPRC       pPciHlpRC;

    /** The PCI device for the PCI bridge. */
    PDMPCIDEV           aPciDev;

    /** Start device number - always zero (only for DevPCI source compat). */
    uint32_t            iDevSearch;
    /** Size alignemnt padding. */
    uint32_t            u32Alignment;
} ICH9PCIBUS, *PICH9PCIBUS;


/** @def PCI_APIC_IRQ_PINS
 * Number of pins for interrupts if the APIC is used.
 */
#define PCI_APIC_IRQ_PINS 8

/**
 * PCI Globals - This is the host-to-pci bridge and the root bus.
 */
typedef struct
{
    /** R3 pointer to the device instance. */
    PPDMDEVINSR3        pDevInsR3;
    /** R0 pointer to the device instance. */
    PPDMDEVINSR0        pDevInsR0;
    /** RC pointer to the device instance. */
    PPDMDEVINSRC        pDevInsRC;

    /** Value latched in Configuration Address Port (0CF8h) */
    uint32_t            uConfigReg;

    /** I/O APIC irq levels */
    volatile uint32_t   uaPciApicIrqLevels[PCI_APIC_IRQ_PINS];

#if 1 /* Will be moved into the BIOS soon. */
    /** The next I/O port address which the PCI BIOS will use. */
    uint32_t            uPciBiosIo;
    /** The next MMIO address which the PCI BIOS will use. */
    uint32_t            uPciBiosMmio;
    /** The next 64-bit MMIO address which the PCI BIOS will use. */
    uint64_t            uPciBiosMmio64;
    /** Actual bus number. */
    uint8_t             uBus;
    uint8_t             Alignment0[7];
#endif
    /** Physical address of PCI config space MMIO region. */
    uint64_t            u64PciConfigMMioAddress;
    /** Length of PCI config space MMIO region. */
    uint64_t            u64PciConfigMMioLength;

    /** PCI bus which is attached to the host-to-PCI bridge. */
    ICH9PCIBUS          aPciBus;
} ICH9PCIGLOBALS, *PICH9PCIGLOBALS;


/**
 * PCI configuration space address.
 */
typedef struct
{
    uint8_t  iBus;
    uint8_t  iDeviceFunc;
    uint16_t iRegister;
} PciAddress;

#ifndef VBOX_DEVICE_STRUCT_TESTCASE


/*********************************************************************************************************************************
*   Defined Constants And Macros                                                                                                 *
*********************************************************************************************************************************/

/** @def VBOX_ICH9PCI_SAVED_STATE_VERSION
 * Saved state version of the ICH9 PCI bus device.
 */
#define VBOX_ICH9PCI_SAVED_STATE_VERSION_NOMSI 1
#define VBOX_ICH9PCI_SAVED_STATE_VERSION_MSI   2
#define VBOX_ICH9PCI_SAVED_STATE_VERSION_CURRENT VBOX_ICH9PCI_SAVED_STATE_VERSION_MSI

/** Converts a bus instance pointer to a device instance pointer. */
#define PCIBUS_2_DEVINS(pPciBus)        ((pPciBus)->CTX_SUFF(pDevIns))
/** Converts a device instance pointer to a ICH9PCIGLOBALS pointer. */
#define DEVINS_2_PCIGLOBALS(pDevIns)    ((PICH9PCIGLOBALS)(PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS)))
/** Converts a device instance pointer to a PCIBUS pointer. */
#define DEVINS_2_PCIBUS(pDevIns)        ((PICH9PCIBUS)(&PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS)->aPciBus))
/** Converts a pointer to a PCI root bus instance to a PCIGLOBALS pointer. */
#define PCIROOTBUS_2_PCIGLOBALS(pPciBus)    ( (PICH9PCIGLOBALS)((uintptr_t)(pPciBus) - RT_OFFSETOF(ICH9PCIGLOBALS, aPciBus)) )

/** @def PCI_LOCK
 * Acquires the PDM lock. This is a NOP if locking is disabled. */
/** @def PCI_UNLOCK
 * Releases the PDM lock. This is a NOP if locking is disabled. */
#define PCI_LOCK(pDevIns, rc) \
    do { \
        int rc2 = DEVINS_2_PCIBUS(pDevIns)->CTX_SUFF(pPciHlp)->pfnLock((pDevIns), rc); \
        if (rc2 != VINF_SUCCESS) \
            return rc2; \
    } while (0)
#define PCI_UNLOCK(pDevIns) \
    DEVINS_2_PCIBUS(pDevIns)->CTX_SUFF(pPciHlp)->pfnUnlock(pDevIns)

/* Prototypes */
static void ich9pciSetIrqInternal(PICH9PCIGLOBALS pGlobals, uint8_t uDevFn, PPDMPCIDEV pPciDev,
                                  int iIrq, int iLevel, uint32_t uTagSrc);
#ifdef IN_RING3
static DECLCALLBACK(uint32_t) ich9pciConfigReadDev(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, uint32_t u32Address, unsigned len);
static DECLCALLBACK(void)     ich9pciConfigWriteDev(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, uint32_t u32Address, uint32_t val, unsigned len);
DECLINLINE(PPDMPCIDEV) ich9pciFindBridge(PICH9PCIBUS pBus, uint8_t uBus);
static void ich9pciBiosInitAllDevicesOnBus(PICH9PCIGLOBALS pGlobals, uint8_t uBus);
static bool ich9pciBiosInitAllDevicesPrefetchableOnBus(PICH9PCIGLOBALS pGlobals, uint8_t uBus, bool fUse64Bit, bool fDryrun);
#endif

// See 7.2.2. PCI Express Enhanced Configuration Mechanism for details of address
// mapping, we take n=6 approach
DECLINLINE(void) ich9pciPhysToPciAddr(PICH9PCIGLOBALS pGlobals, RTGCPHYS GCPhysAddr, PciAddress* pPciAddr)
{
    NOREF(pGlobals);
    pPciAddr->iBus          = (GCPhysAddr >> 20) & ((1<<6)       - 1);
    pPciAddr->iDeviceFunc   = (GCPhysAddr >> 12) & ((1<<(5+3))   - 1); // 5 bits - device, 3 bits - function
    pPciAddr->iRegister     = (GCPhysAddr >>  0) & ((1<<(6+4+2)) - 1); // 6 bits - register, 4 bits - extended register, 2 bits -Byte Enable
}

DECLINLINE(void) ich9pciStateToPciAddr(PICH9PCIGLOBALS pGlobals, RTGCPHYS addr, PciAddress* pPciAddr)
{
    pPciAddr->iBus         = (pGlobals->uConfigReg >> 16) & 0xff;
    pPciAddr->iDeviceFunc  = (pGlobals->uConfigReg >> 8) & 0xff;
    pPciAddr->iRegister    = (pGlobals->uConfigReg & 0xfc) | (addr & 3);
}

PDMBOTHCBDECL(void) ich9pciSetIrq(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, int iIrq, int iLevel, uint32_t uTagSrc)
{
    LogFlowFunc(("invoked by %p/%d: iIrq=%d iLevel=%d uTagSrc=%#x\n", pDevIns, pDevIns->iInstance, iIrq, iLevel, uTagSrc));
    ich9pciSetIrqInternal(PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS), pPciDev->uDevFn, pPciDev, iIrq, iLevel, uTagSrc);
}

PDMBOTHCBDECL(void) ich9pcibridgeSetIrq(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, int iIrq, int iLevel, uint32_t uTagSrc)
{
    /*
     * The PCI-to-PCI bridge specification defines how the interrupt pins
     * are routed from the secondary to the primary bus (see chapter 9).
     * iIrq gives the interrupt pin the pci device asserted.
     * We change iIrq here according to the spec and call the SetIrq function
     * of our parent passing the device which asserted the interrupt instead of the device of the bridge.
     */
    PICH9PCIBUS    pBus          = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);
    PPDMPCIDEV     pPciDevBus    = pPciDev;
    int            iIrqPinBridge = iIrq;
    uint8_t        uDevFnBridge  = 0;

    /* Walk the chain until we reach the host bus. */
    do
    {
        uDevFnBridge  = pBus->aPciDev.uDevFn;
        iIrqPinBridge = ((pPciDevBus->uDevFn >> 3) + iIrqPinBridge) & 3;

        /* Get the parent. */
        pBus = pBus->aPciDev.Int.s.CTX_SUFF(pBus);
        pPciDevBus = &pBus->aPciDev;
    } while (pBus->iBus != 0);

    AssertMsgReturnVoid(pBus->iBus == 0, ("This is not the host pci bus iBus=%d\n", pBus->iBus));
    ich9pciSetIrqInternal(PCIROOTBUS_2_PCIGLOBALS(pBus), uDevFnBridge, pPciDev, iIrqPinBridge, iLevel, uTagSrc);
}


/**
 * Port I/O Handler for PCI address OUT operations.
 *
 * Emulates writes to Configuration Address Port at 0CF8h for
 * Configuration Mechanism #1.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     ICH9 device instance.
 * @param   pvUser      User argument - ignored.
 * @param   uPort       Port number used for the OUT operation.
 * @param   u32         The value to output.
 * @param   cb          The value size in bytes.
 */
PDMBOTHCBDECL(int) ich9pciIOPortAddressWrite(PPDMDEVINS pDevIns, void *pvUser, RTIOPORT Port, uint32_t u32, unsigned cb)
{
    LogFlow(("ich9pciIOPortAddressWrite: Port=%#x u32=%#x cb=%d\n", Port, u32, cb));
    RT_NOREF2(Port, pvUser);
    if (cb == 4)
    {
        PICH9PCIGLOBALS pThis = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);

        /*
         * bits [1:0] are hard-wired, read-only and must return zeroes
         * when read.
         */
        u32 &= ~3;

        PCI_LOCK(pDevIns, VINF_IOM_R3_IOPORT_WRITE);
        pThis->uConfigReg = u32;
        PCI_UNLOCK(pDevIns);
    }

    return VINF_SUCCESS;
}


/**
 * Port I/O Handler for PCI address IN operations.
 *
 * Emulates reads from Configuration Address Port at 0CF8h for
 * Configuration Mechanism #1.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     ICH9 device instance.
 * @param   pvUser      User argument - ignored.
 * @param   uPort       Port number used for the IN operation.
 * @param   pu32        Where to store the result.
 * @param   cb          Number of bytes read.
 */
PDMBOTHCBDECL(int) ich9pciIOPortAddressRead(PPDMDEVINS pDevIns, void *pvUser, RTIOPORT Port, uint32_t *pu32, unsigned cb)
{
    RT_NOREF2(Port, pvUser);
    if (cb == 4)
    {
        PICH9PCIGLOBALS pThis = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);

        PCI_LOCK(pDevIns, VINF_IOM_R3_IOPORT_READ);
        *pu32 = pThis->uConfigReg;
        PCI_UNLOCK(pDevIns);

        LogFlow(("ich9pciIOPortAddressRead: Port=%#x cb=%d -> %#x\n", Port, cb, *pu32));
        return VINF_SUCCESS;
    }

    Log(("ich9pciIOPortAddressRead: Port=%#x cb=%d VERR_IOM_IOPORT_UNUSED\n", Port, cb));
    return VERR_IOM_IOPORT_UNUSED;
}


/*
 * Perform configuration space write.
 */
static int ich9pciDataWriteAddr(PICH9PCIGLOBALS pGlobals, PciAddress* pAddr,
                                uint32_t val, int cb, int rcReschedule)
{
    int rc = VINF_SUCCESS;
#ifdef IN_RING3
    NOREF(rcReschedule);
#else
    RT_NOREF2(val, cb);
#endif

    if (pAddr->iBus != 0)       /* forward to subordinate bus */
    {
        if (pGlobals->aPciBus.cBridges)
        {
#ifdef IN_RING3 /** @todo do lookup in R0/RC too! r=klaus don't think that it can work, since the config space access callback only works in R3 */
            PPDMPCIDEV pBridgeDevice = ich9pciFindBridge(&pGlobals->aPciBus, pAddr->iBus);
            if (pBridgeDevice)
            {
                AssertPtr(pBridgeDevice->Int.s.pfnBridgeConfigWrite);
                pBridgeDevice->Int.s.pfnBridgeConfigWrite(pBridgeDevice->Int.s.CTX_SUFF(pDevIns), pAddr->iBus, pAddr->iDeviceFunc,
                                                          pAddr->iRegister, val, cb);
            }
#else
            rc = rcReschedule;
#endif
        }
    }
    else                    /* forward to directly connected device */
    {
        R3PTRTYPE(PDMPCIDEV *) pPciDev = pGlobals->aPciBus.apDevices[pAddr->iDeviceFunc];
        if (pPciDev)
        {
#ifdef IN_RING3
            pPciDev->Int.s.pfnConfigWrite(pPciDev->Int.s.CTX_SUFF(pDevIns), pPciDev, pAddr->iRegister, val, cb);
#else
            rc = rcReschedule;
#endif
        }
    }

    Log2(("ich9pciDataWriteAddr: %02x:%02x.%d reg %x(%d) %x %Rrc\n",
          pAddr->iBus, pAddr->iDeviceFunc >> 3, pAddr->iDeviceFunc & 0x7, pAddr->iRegister,
          cb, val, rc));
    return rc;
}


/*
 * Decode value latched in Configuration Address Port and perform
 * requsted write to the target configuration space register.
 *
 * XXX: This code should be probably moved to its only caller
 * (ich9pciIOPortDataWrite) to avoid prolifiration of confusingly
 * similarly named functions.
 */
static int ich9pciDataWrite(PICH9PCIGLOBALS pGlobals, uint32_t addr, uint32_t val, int len)
{
    LogFlow(("ich9pciDataWrite: config=%08x val=%08x len=%d\n", pGlobals->uConfigReg, val, len));

    /* Configuration space mapping enabled? */
    if (!(pGlobals->uConfigReg & (1 << 31)))
        return VINF_SUCCESS;

    /* Decode target device and configuration space register */
    PciAddress aPciAddr;
    ich9pciStateToPciAddr(pGlobals, addr, &aPciAddr);

    /* Perform configuration space write */
    return ich9pciDataWriteAddr(pGlobals, &aPciAddr, val, len, VINF_IOM_R3_IOPORT_WRITE);
}


/**
 * Port I/O Handler for PCI data OUT operations.
 *
 * Emulates writes to Configuration Data Port at 0CFCh for
 * Configuration Mechanism #1.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     ICH9 device instance.
 * @param   pvUser      User argument - ignored.
 * @param   uPort       Port number used for the OUT operation.
 * @param   u32         The value to output.
 * @param   cb          The value size in bytes.
 */
PDMBOTHCBDECL(int) ich9pciIOPortDataWrite(PPDMDEVINS pDevIns, void *pvUser, RTIOPORT Port, uint32_t u32, unsigned cb)
{
    LogFlow(("ich9pciIOPortDataWrite: Port=%#x u32=%#x cb=%d\n", Port, u32, cb));
    NOREF(pvUser);
    int rc = VINF_SUCCESS;
    if (!(Port % cb))
    {
        PICH9PCIGLOBALS pThis = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);

        PCI_LOCK(pDevIns, VINF_IOM_R3_IOPORT_WRITE);
        rc = ich9pciDataWrite(pThis, Port, u32, cb);
        PCI_UNLOCK(pDevIns);
    }
    else
        AssertMsgFailed(("Unaligned write to port %#x u32=%#x cb=%d\n", Port, u32, cb));
    return rc;
}


static void ich9pciNoMem(void* ptr, int cb)
{
    for (int i = 0; i < cb; i++)
        ((uint8_t*)ptr)[i] = 0xff;
}


/*
 * Perform configuration space read.
 */
static int ich9pciDataReadAddr(PICH9PCIGLOBALS pGlobals, PciAddress* pPciAddr, int cb,
                               uint32_t *pu32, int rcReschedule)
{
    int rc = VINF_SUCCESS;
#ifdef IN_RING3
    NOREF(rcReschedule);
#endif

    if (pPciAddr->iBus != 0)    /* forward to subordinate bus */
    {
        if (pGlobals->aPciBus.cBridges)
        {
#ifdef IN_RING3 /** @todo do lookup in R0/RC too! r=klaus don't think that it can work, since the config space access callback only works in R3 */
            PPDMPCIDEV pBridgeDevice = ich9pciFindBridge(&pGlobals->aPciBus, pPciAddr->iBus);
            if (pBridgeDevice)
            {
                AssertPtr(pBridgeDevice->Int.s.pfnBridgeConfigRead);
                *pu32 = pBridgeDevice->Int.s.pfnBridgeConfigRead(pBridgeDevice->Int.s.CTX_SUFF(pDevIns), pPciAddr->iBus,
                                                                 pPciAddr->iDeviceFunc, pPciAddr->iRegister, cb);
            }
            else
                ich9pciNoMem(pu32, cb);
#else
            rc = rcReschedule;
#endif
        }
        else
            ich9pciNoMem(pu32, cb);
    }
    else                    /* forward to directly connected device */
    {
        R3PTRTYPE(PDMPCIDEV *) pPciDev = pGlobals->aPciBus.apDevices[pPciAddr->iDeviceFunc];
        if (pPciDev)
        {
#ifdef IN_RING3
            *pu32 = pPciDev->Int.s.pfnConfigRead(pPciDev->Int.s.CTX_SUFF(pDevIns), pPciDev, pPciAddr->iRegister, cb);
#else
            rc = rcReschedule;
#endif
        }
        else
            ich9pciNoMem(pu32, cb);
    }

    Log3(("ich9pciDataReadAddr: %02x:%02x.%d reg %x(%d) gave %x %Rrc\n",
          pPciAddr->iBus, pPciAddr->iDeviceFunc >> 3, pPciAddr->iDeviceFunc & 0x7, pPciAddr->iRegister,
          cb, *pu32, rc));
    return rc;
}


/*
 * Decode value latched in Configuration Address Port and perform
 * requsted read from the target configuration space register.
 *
 * XXX: This code should be probably moved to its only caller
 * (ich9pciIOPortDataRead) to avoid prolifiration of confusingly
 * similarly named functions.
 */
static int ich9pciDataRead(PICH9PCIGLOBALS pGlobals, uint32_t addr, int cb, uint32_t *pu32)
{
    LogFlow(("ich9pciDataRead: config=%x cb=%d\n",  pGlobals->uConfigReg, cb));

    *pu32 = 0xffffffff;

    /* Configuration space mapping enabled? */
    if (!(pGlobals->uConfigReg & (1 << 31)))
        return VINF_SUCCESS;

    /* Decode target device and configuration space register */
    PciAddress aPciAddr;
    ich9pciStateToPciAddr(pGlobals, addr, &aPciAddr);

    /* Perform configuration space read */
    return ich9pciDataReadAddr(pGlobals, &aPciAddr, cb, pu32, VINF_IOM_R3_IOPORT_READ);
}


/**
 * Port I/O Handler for PCI data IN operations.
 *
 * Emulates reads from Configuration Data Port at 0CFCh for
 * Configuration Mechanism #1.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     ICH9 device instance.
 * @param   pvUser      User argument - ignored.
 * @param   uPort       Port number used for the IN operation.
 * @param   pu32        Where to store the result.
 * @param   cb          Number of bytes read.
 */
PDMBOTHCBDECL(int) ich9pciIOPortDataRead(PPDMDEVINS pDevIns, void *pvUser, RTIOPORT Port, uint32_t *pu32, unsigned cb)
{
    NOREF(pvUser);
    if (!(Port % cb))
    {
        PICH9PCIGLOBALS pThis = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);

        PCI_LOCK(pDevIns, VINF_IOM_R3_IOPORT_READ);
        int rc = ich9pciDataRead(pThis, Port, cb, pu32);
        PCI_UNLOCK(pDevIns);

        LogFlow(("ich9pciIOPortDataRead: Port=%#x cb=%#x -> %#x (%Rrc)\n", Port, cb, *pu32, rc));
        return rc;
    }
    AssertMsgFailed(("Unaligned read from port %#x cb=%d\n", Port, cb));
    return VERR_IOM_IOPORT_UNUSED;
}


/* Compute mapping of PCI slot and IRQ number to APIC interrupt line */
DECLINLINE(int) ich9pciSlot2ApicIrq(uint8_t uSlot, int irq_num)
{
    return (irq_num + uSlot) & 7;
}

#ifdef IN_RING3

/* return the global irq number corresponding to a given device irq
   pin. We could also use the bus number to have a more precise
   mapping. This is the implementation note described in the PCI spec chapter 2.2.6 */
DECLINLINE(int) ich9pciSlotGetPirq(uint8_t uBus, uint8_t uDevFn, int iIrqNum)
{
    NOREF(uBus);
    int iSlotAddend = (uDevFn >> 3) - 1;
    return (iIrqNum + iSlotAddend) & 3;
}

/* irqs corresponding to PCI irqs A-D, must match pci_irq_list in pcibios.inc */
/** @todo r=klaus inconsistent! ich9 doesn't implement PIRQ yet, so both needs to be addressed and tested thoroughly. */
static const uint8_t aPciIrqs[4] = { 11, 10, 9, 5 };

#endif /* IN_RING3 */

/* Add one more level up request on APIC input line */
DECLINLINE(void) ich9pciApicLevelUp(PICH9PCIGLOBALS pGlobals, int irq_num)
{
    ASMAtomicIncU32(&pGlobals->uaPciApicIrqLevels[irq_num]);
}

/* Remove one level up request on APIC input line */
DECLINLINE(void) ich9pciApicLevelDown(PICH9PCIGLOBALS pGlobals, int irq_num)
{
    ASMAtomicDecU32(&pGlobals->uaPciApicIrqLevels[irq_num]);
}

static void ich9pciApicSetIrq(PICH9PCIBUS pBus, uint8_t uDevFn, PDMPCIDEV *pPciDev, int irq_num1, int iLevel,
                              uint32_t uTagSrc, int iForcedIrq)
{
    /* This is only allowed to be called with a pointer to the root bus. */
    AssertMsg(pBus->iBus == 0, ("iBus=%u\n", pBus->iBus));

    if (iForcedIrq == -1)
    {
        int apic_irq, apic_level;
        PICH9PCIGLOBALS pGlobals = PCIROOTBUS_2_PCIGLOBALS(pBus);
        int irq_num = ich9pciSlot2ApicIrq(uDevFn >> 3, irq_num1);

        if ((iLevel & PDM_IRQ_LEVEL_HIGH) == PDM_IRQ_LEVEL_HIGH)
            ich9pciApicLevelUp(pGlobals, irq_num);
        else if ((iLevel & PDM_IRQ_LEVEL_HIGH) == PDM_IRQ_LEVEL_LOW)
            ich9pciApicLevelDown(pGlobals, irq_num);

        apic_irq = irq_num + 0x10;
        apic_level = pGlobals->uaPciApicIrqLevels[irq_num] != 0;
        Log3(("ich9pciApicSetIrq: %s: irq_num1=%d level=%d apic_irq=%d apic_level=%d irq_num1=%d uTagSrc=%#x\n",
              R3STRING(pPciDev->pszNameR3), irq_num1, iLevel, apic_irq, apic_level, irq_num, uTagSrc));
        pBus->CTX_SUFF(pPciHlp)->pfnIoApicSetIrq(pBus->CTX_SUFF(pDevIns), apic_irq, apic_level, uTagSrc);

        if ((iLevel & PDM_IRQ_LEVEL_FLIP_FLOP) == PDM_IRQ_LEVEL_FLIP_FLOP)
        {
            /*
             *  we raised it few lines above, as PDM_IRQ_LEVEL_FLIP_FLOP has
             * PDM_IRQ_LEVEL_HIGH bit set
             */
            ich9pciApicLevelDown(pGlobals, irq_num);
            pPciDev->Int.s.uIrqPinState = PDM_IRQ_LEVEL_LOW;
            apic_level = pGlobals->uaPciApicIrqLevels[irq_num] != 0;
            Log3(("ich9pciApicSetIrq: %s: irq_num1=%d level=%d apic_irq=%d apic_level=%d irq_num1=%d uTagSrc=%#x (flop)\n",
                  R3STRING(pPciDev->pszNameR3), irq_num1, iLevel, apic_irq, apic_level, irq_num, uTagSrc));
            pBus->CTX_SUFF(pPciHlp)->pfnIoApicSetIrq(pBus->CTX_SUFF(pDevIns), apic_irq, apic_level, uTagSrc);
        }
    } else {
        Log3(("ich9pciApicSetIrq: (forced) %s: irq_num1=%d level=%d acpi_irq=%d uTagSrc=%#x\n",
              R3STRING(pPciDev->pszNameR3), irq_num1, iLevel, iForcedIrq, uTagSrc));
        pBus->CTX_SUFF(pPciHlp)->pfnIoApicSetIrq(pBus->CTX_SUFF(pDevIns), iForcedIrq, iLevel, uTagSrc);
    }
}

static void ich9pciSetIrqInternal(PICH9PCIGLOBALS pGlobals, uint8_t uDevFn, PPDMPCIDEV pPciDev,
                                  int iIrq, int iLevel, uint32_t uTagSrc)
{
    /* If MSI or MSI-X is enabled, PCI INTx# signals are disabled regardless of the PCI command
     * register interrupt bit state.
     * PCI 3.0 (section 6.8) forbids MSI and MSI-X to be enabled at the same time and makes
     * that undefined behavior. We check for MSI first, then MSI-X.
     */
    if (MsiIsEnabled(pPciDev))
    {
        Assert(!MsixIsEnabled(pPciDev));    /* Not allowed -- see note above. */
        LogFlowFunc(("PCI Dev %p : MSI\n", pPciDev));
        PPDMDEVINS pDevIns = pGlobals->aPciBus.CTX_SUFF(pDevIns);
        MsiNotify(pDevIns, pGlobals->aPciBus.CTX_SUFF(pPciHlp), pPciDev, iIrq, iLevel, uTagSrc);
        return;
    }

    if (MsixIsEnabled(pPciDev))
    {
        LogFlowFunc(("PCI Dev %p : MSI-X\n", pPciDev));
        PPDMDEVINS pDevIns = pGlobals->aPciBus.CTX_SUFF(pDevIns);
        MsixNotify(pDevIns, pGlobals->aPciBus.CTX_SUFF(pPciHlp), pPciDev, iIrq, iLevel, uTagSrc);
        return;
    }

    PICH9PCIBUS pBus = &pGlobals->aPciBus;
    /* safe, only needs to go to the config space array */
    const bool fIsAcpiDevice = PDMPciDevGetDeviceId(pPciDev) == 0x7113;

    LogFlowFunc(("PCI Dev %p : IRQ\n", pPciDev));
    /* Check if the state changed. */
    if (pPciDev->Int.s.uIrqPinState != iLevel)
    {
        pPciDev->Int.s.uIrqPinState = (iLevel & PDM_IRQ_LEVEL_HIGH);

        /** @todo r=klaus: implement PIRQ handling (if APIC isn't active). Needed for legacy OSes which don't use the APIC stuff. */

        /* Send interrupt to I/O APIC only now. */
        if (fIsAcpiDevice)
            /*
             * ACPI needs special treatment since SCI is hardwired and
             * should not be affected by PCI IRQ routing tables at the
             * same time SCI IRQ is shared in PCI sense hence this
             * kludge (i.e. we fetch the hardwired value from ACPIs
             * PCI device configuration space).
             */
            /* safe, only needs to go to the config space array */
            ich9pciApicSetIrq(pBus, uDevFn, pPciDev, -1, iLevel, uTagSrc, PDMPciDevGetInterruptLine(pPciDev));
        else
            ich9pciApicSetIrq(pBus, uDevFn, pPciDev, iIrq, iLevel, uTagSrc, -1);
    }
}


/**
 * Memory mapped I/O Handler for write operations.
 *
 * Emulates writes to configuration space.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      User argument.
 * @param   GCPhysAddr  Physical address (in GC) where the read starts.
 * @param   pv          Where to fetch the result.
 * @param   cb          Number of bytes to write.
 * @remarks Caller enters the device critical section.
 */
PDMBOTHCBDECL(int) ich9pciMcfgMMIOWrite(PPDMDEVINS pDevIns, void *pvUser, RTGCPHYS GCPhysAddr, void const *pv, unsigned cb)
{
    PICH9PCIGLOBALS pGlobals = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);
    uint32_t u32 = 0;
    NOREF(pvUser);

    Log2(("ich9pciMcfgMMIOWrite: %RGp(%d) \n", GCPhysAddr, cb));

    PCI_LOCK(pDevIns, VINF_IOM_R3_MMIO_WRITE);

    /* Decode target device and configuration space register */
    PciAddress aDest;
    ich9pciPhysToPciAddr(pGlobals, GCPhysAddr, &aDest);

    switch (cb)
    {
        case 1:
            u32 = *(uint8_t*)pv;
            break;
        case 2:
            u32 = *(uint16_t*)pv;
            break;
        case 4:
            u32 = *(uint32_t*)pv;
            break;
        default:
            Assert(false);
            break;
    }

    /* Perform configuration space write */
    int rc = ich9pciDataWriteAddr(pGlobals, &aDest, u32, cb, VINF_IOM_R3_MMIO_WRITE);
    PCI_UNLOCK(pDevIns);

    return rc;
}


/**
 * Memory mapped I/O Handler for read operations.
 *
 * Emulates reads from configuration space.
 *
 * @returns VBox status code.
 *
 * @param   pDevIns     The device instance.
 * @param   pvUser      User argument.
 * @param   GCPhysAddr  Physical address (in GC) where the read starts.
 * @param   pv          Where to store the result.
 * @param   cb          Number of bytes read.
 * @remarks Caller enters the device critical section.
 */
PDMBOTHCBDECL(int) ich9pciMcfgMMIORead (PPDMDEVINS pDevIns, void *pvUser, RTGCPHYS GCPhysAddr, void *pv, unsigned cb)
{
    PICH9PCIGLOBALS pGlobals = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);
    uint32_t    rv;
    NOREF(pvUser);

    LogFlow(("ich9pciMcfgMMIORead: %RGp(%d) \n", GCPhysAddr, cb));

    PCI_LOCK(pDevIns, VINF_IOM_R3_MMIO_READ);

    /* Decode target device and configuration space register */
    PciAddress aDest;
    ich9pciPhysToPciAddr(pGlobals, GCPhysAddr, &aDest);

    /* Perform configuration space read */
    int rc = ich9pciDataReadAddr(pGlobals, &aDest, cb, &rv, VINF_IOM_R3_MMIO_READ);

    if (RT_SUCCESS(rc))
    {
        switch (cb)
        {
            case 1:
                *(uint8_t*)pv   = (uint8_t)rv;
                break;
            case 2:
                *(uint16_t*)pv  = (uint16_t)rv;
                break;
            case 4:
                *(uint32_t*)pv  = (uint32_t)rv;
                break;
            default:
                Assert(false);
                break;
        }
    }
    PCI_UNLOCK(pDevIns);

    return rc;
}

#ifdef IN_RING3

/*
 * Include code we share with the other PCI bus implementation.
 *
 * Note! No #ifdefs, use instant data booleans/flags/whatever.  Goal is to
 *       completely merge these files!  File #1 contains code we write, where
 *       as a possible file #2 contains external code if there's any left.
 */
typedef PICH9PCIBUS PPCIMERGEDBUS;
# define pciR3UnmergedConfigReadDev  ich9pciConfigReadDev
# define pciR3UnmergedConfigWriteDev ich9pciConfigWriteDev
# include "DevPciMerge1.cpp.h"


DECLINLINE(PPDMPCIDEV) ich9pciFindBridge(PICH9PCIBUS pBus, uint8_t uBus)
{
    /* Search for a fitting bridge. */
    for (uint32_t iBridge = 0; iBridge < pBus->cBridges; iBridge++)
    {
        /*
         * Examine secondary and subordinate bus number.
         * If the target bus is in the range we pass the request on to the bridge.
         */
        PPDMPCIDEV pBridge = pBus->papBridgesR3[iBridge];
        AssertMsg(pBridge && pciDevIsPci2PciBridge(pBridge),
                  ("Device is not a PCI bridge but on the list of PCI bridges\n"));
        /* safe, only needs to go to the config space array */
        uint32_t uSecondary   = PDMPciDevGetByte(pBridge, VBOX_PCI_SECONDARY_BUS);
        /* safe, only needs to go to the config space array */
        uint32_t uSubordinate = PDMPciDevGetByte(pBridge, VBOX_PCI_SUBORDINATE_BUS);
        Log3(("ich9pciFindBridge on bus %p, bridge %d: %d in %d..%d\n", pBus, iBridge, uBus, uSecondary, uSubordinate));
        if (uBus >= uSecondary && uBus <= uSubordinate)
            return pBridge;
    }

    /* Nothing found. */
    return NULL;
}

static uint32_t ich9pciGetCfg(PPDMPCIDEV pPciDev, int32_t iRegister, int cb)
{
    return pPciDev->Int.s.pfnConfigRead(pPciDev->Int.s.CTX_SUFF(pDevIns), pPciDev, iRegister, cb);
}

DECLINLINE(uint8_t) ich9pciGetByte(PPDMPCIDEV pPciDev, int32_t iRegister)
{
    return (uint8_t)ich9pciGetCfg(pPciDev, iRegister, 1);
}

DECLINLINE(uint16_t) ich9pciGetWord(PPDMPCIDEV pPciDev, int32_t iRegister)
{
    return (uint16_t)ich9pciGetCfg(pPciDev, iRegister, 2);
}

DECLINLINE(uint32_t) ich9pciGetDWord(PPDMPCIDEV pPciDev, int32_t iRegister)
{
    return (uint32_t)ich9pciGetCfg(pPciDev, iRegister, 4);
}

DECLINLINE(uint32_t) ich9pciGetRegionReg(int iRegion)
{
    return (iRegion == VBOX_PCI_ROM_SLOT) ?
            VBOX_PCI_ROM_ADDRESS : (VBOX_PCI_BASE_ADDRESS_0 + iRegion * 4);
}

static void ich9pciSetCfg(PPDMPCIDEV pPciDev, int32_t iRegister, uint32_t u32, int cb)
{
    pPciDev->Int.s.pfnConfigWrite(pPciDev->Int.s.CTX_SUFF(pDevIns), pPciDev, iRegister, u32, cb);
}

DECLINLINE(void) ich9pciSetByte(PPDMPCIDEV pPciDev, int32_t iRegister, uint8_t u8)
{
    ich9pciSetCfg(pPciDev, iRegister, u8, 1);
}

DECLINLINE(void) ich9pciSetWord(PPDMPCIDEV pPciDev, int32_t iRegister, uint16_t u16)
{
    ich9pciSetCfg(pPciDev, iRegister, u16, 2);
}

#if 0
DECLINLINE(void) ich9pciSetDWord(PPDMPCIDEV pPciDev, int32_t iRegister, uint32_t u32)
{
    ich9pciSetCfg(pPciDev, iRegister, u32, 4);
}
#endif


#define INVALID_PCI_ADDRESS ~0U

static int  ich9pciUnmapRegion(PPDMPCIDEV pDev, int iRegion)
{
    PCIIORegion* pRegion = &pDev->Int.s.aIORegions[iRegion];
    int rc = VINF_SUCCESS;
    PICH9PCIBUS pBus = pDev->Int.s.CTX_SUFF(pBus);

    Assert (pRegion->size != 0);

    if (pRegion->addr != INVALID_PCI_ADDRESS)
    {
        if (pRegion->type & PCI_ADDRESS_SPACE_IO)
        {
            /* Port IO */
            rc = PDMDevHlpIOPortDeregister(pDev->Int.s.pDevInsR3, pRegion->addr, pRegion->size);
            AssertRC(rc);
        }
        else
        {
            RTGCPHYS GCPhysBase = pRegion->addr;
            if (pBus->pPciHlpR3->pfnIsMMIOExBase(pBus->pDevInsR3, pDev->Int.s.pDevInsR3, GCPhysBase))
            {
                /* unmap it. */
                rc = pRegion->map_func(pDev->Int.s.pDevInsR3, pDev, iRegion,
                                       NIL_RTGCPHYS, pRegion->size, (PCIADDRESSSPACE)(pRegion->type));
                AssertRC(rc);
                rc = PDMDevHlpMMIOExUnmap(pDev->Int.s.pDevInsR3, pDev, iRegion, GCPhysBase);
            }
            else
                rc = PDMDevHlpMMIODeregister(pDev->Int.s.pDevInsR3, GCPhysBase, pRegion->size);
        }

        pRegion->addr = INVALID_PCI_ADDRESS;
    }

    return rc;
}

static void ich9pciUpdateMappings(PDMPCIDEV* pDev, bool fP2PBridge)
{
    uint64_t uLast, uNew;

    /* safe, only needs to go to the config space array */
    int iCmd = PDMPciDevGetWord(pDev, VBOX_PCI_COMMAND);
    for (int iRegion = 0; iRegion < PCI_NUM_REGIONS; iRegion++)
    {
        /* Skip over BAR2..BAR5 for bridges, as they have a different meaning there. */
        if (fP2PBridge && iRegion >= 2 && iRegion <= 5)
            continue;
        PCIIORegion* pRegion = &pDev->Int.s.aIORegions[iRegion];
        uint32_t uConfigReg  = ich9pciGetRegionReg(iRegion);
        int64_t  iRegionSize =  pRegion->size;
        int rc;

        if (iRegionSize == 0)
            continue;

        bool f64Bit =    (pRegion->type & ((uint8_t)(PCI_ADDRESS_SPACE_BAR64 | PCI_ADDRESS_SPACE_IO)))
                      == PCI_ADDRESS_SPACE_BAR64;

        if (pRegion->type & PCI_ADDRESS_SPACE_IO)
        {
            /* port IO region */
            if (iCmd & VBOX_PCI_COMMAND_IO)
            {
                /* safe, only needs to go to the config space array */
                uNew = PDMPciDevGetDWord(pDev, uConfigReg);
                uNew &= ~(iRegionSize - 1);
                uLast = uNew + iRegionSize - 1;
                /* only 64K ioports on PC */
                if (uLast <= uNew || uNew == 0 || uLast >= 0x10000)
                    uNew = INVALID_PCI_ADDRESS;
            } else
                uNew = INVALID_PCI_ADDRESS;
        }
        else
        {
            /* MMIO region */
            if (iCmd & VBOX_PCI_COMMAND_MEMORY)
            {
                /* safe, only needs to go to the config space array */
                uNew = PDMPciDevGetDWord(pDev, uConfigReg);
                if (f64Bit)
                {
                    /* safe, only needs to go to the config space array */
                    uNew |= (uint64_t)PDMPciDevGetDWord(pDev, uConfigReg + 4) << 32;
                }

                /* the ROM slot has a specific enable bit */
                if (iRegion == PCI_ROM_SLOT && !(uNew & 1))
                    uNew = INVALID_PCI_ADDRESS;
                else
                {
                    uNew &= ~(iRegionSize - 1);
                    uLast = uNew + iRegionSize - 1;
                    /* NOTE: we do not support wrapping */
                    /* XXX: as we cannot support really dynamic
                       mappings, we handle specific values as invalid
                       mappings. */
                    /* Unconditionally exclude I/O-APIC/HPET/ROM. Pessimistic, but better than causing a mess. */
                    if (uLast <= uNew || uNew == 0 || (uNew <= UINT32_C(0xffffffff) && uLast >= UINT32_C(0xfec00000)) || uNew >= UINT64_C(0xffffffff00000000))
                        uNew = INVALID_PCI_ADDRESS;
                }
            } else
                uNew = INVALID_PCI_ADDRESS;
        }
        LogRel2(("PCI: config dev %u/%u BAR%i uOld=%#018llx uNew=%#018llx size=%llu\n", pDev->uDevFn >> 3, pDev->uDevFn & 7, iRegion, pRegion->addr, uNew, pRegion->size));
        /* now do the real mapping */
        if (uNew != pRegion->addr)
        {
            if (pRegion->addr != INVALID_PCI_ADDRESS)
                ich9pciUnmapRegion(pDev, iRegion);

            pRegion->addr = uNew;
            if (pRegion->addr != INVALID_PCI_ADDRESS)
            {

                /* finally, map the region */
                rc = pRegion->map_func(pDev->Int.s.pDevInsR3, pDev, iRegion,
                                       pRegion->addr, pRegion->size,
                                       (PCIADDRESSSPACE)(pRegion->type));
                AssertRC(rc);
            }
        }

        if (f64Bit)
            iRegion++;
    }
}


static DECLCALLBACK(int) ich9pciRegisterMsi(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, PPDMMSIREG pMsiReg)
{
    NOREF(pDevIns);
    int rc;

    rc = MsiInit(pPciDev, pMsiReg);
    if (RT_FAILURE(rc))
        return rc;

    rc = MsixInit(pPciDev->Int.s.CTX_SUFF(pBus)->CTX_SUFF(pPciHlp), pPciDev, pMsiReg);
    if (RT_FAILURE(rc))
        return rc;

    return VINF_SUCCESS;
}


static DECLCALLBACK(int) ich9pciIORegionRegister(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, int iRegion, RTGCPHYS cbRegion,
                                                 PCIADDRESSSPACE enmType, PFNPCIIOREGIONMAP pfnCallback)
{
    NOREF(pDevIns);

    /*
     * Validate.
     */
    AssertMsgReturn(   enmType == (PCI_ADDRESS_SPACE_MEM | PCI_ADDRESS_SPACE_BAR32)
                    || enmType == (PCI_ADDRESS_SPACE_MEM_PREFETCH | PCI_ADDRESS_SPACE_BAR32)
                    || enmType == (PCI_ADDRESS_SPACE_MEM | PCI_ADDRESS_SPACE_BAR64)
                    || enmType == (PCI_ADDRESS_SPACE_MEM_PREFETCH | PCI_ADDRESS_SPACE_BAR64)
                    || enmType ==  PCI_ADDRESS_SPACE_IO
                    ,
                    ("Invalid enmType=%#x? Or was this a bitmask after all...\n", enmType),
                    VERR_INVALID_PARAMETER);
    AssertMsgReturn((unsigned)iRegion < PCI_NUM_REGIONS,
                    ("Invalid iRegion=%d PCI_NUM_REGIONS=%d\n", iRegion, PCI_NUM_REGIONS),
                    VERR_INVALID_PARAMETER);
    int iLastSet = ASMBitLastSetU64(cbRegion);
    AssertMsgReturn(    iLastSet != 0
                    &&  RT_BIT_64(iLastSet - 1) == cbRegion,
                    ("Invalid cbRegion=%RGp iLastSet=%#x (not a power of 2 or 0)\n", cbRegion, iLastSet),
                    VERR_INVALID_PARAMETER);

    Log(("ich9pciIORegionRegister: %s region %d size %RGp type %x\n",
         pPciDev->pszNameR3, iRegion, cbRegion, enmType));

    /* Make sure that we haven't marked this region as continuation of 64-bit region. */
    Assert(pPciDev->Int.s.aIORegions[iRegion].type != 0xff);

    /*
     * Register the I/O region.
     */
    PPCIIOREGION pRegion = &pPciDev->Int.s.aIORegions[iRegion];
    pRegion->addr        = INVALID_PCI_ADDRESS;
    pRegion->size        = cbRegion;
    pRegion->type        = enmType;
    pRegion->map_func    = pfnCallback;

    if ((enmType & PCI_ADDRESS_SPACE_BAR64) != 0)
    {
        /* VBOX_PCI_BASE_ADDRESS_5 and VBOX_PCI_ROM_ADDRESS are excluded. */
        AssertMsgReturn(iRegion < PCI_NUM_REGIONS - 2,
                        ("Region %d cannot be 64-bit\n", iRegion),
                        VERR_INVALID_PARAMETER);
        /* Mark next region as continuation of this one. */
        pPciDev->Int.s.aIORegions[iRegion + 1].type = 0xff;
    }

    /* Set type in the PCI config space. */
    uint32_t u32Value   = (uint32_t)enmType & (PCI_ADDRESS_SPACE_IO | PCI_ADDRESS_SPACE_BAR64 | PCI_ADDRESS_SPACE_MEM_PREFETCH);
    /* safe, only needs to go to the config space array */
    PDMPciDevSetDWord(pPciDev, ich9pciGetRegionReg(iRegion), u32Value);

    return VINF_SUCCESS;
}

static DECLCALLBACK(void) ich9pciSetConfigCallbacks(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, PFNPCICONFIGREAD pfnRead, PPFNPCICONFIGREAD ppfnReadOld,
                                                    PFNPCICONFIGWRITE pfnWrite, PPFNPCICONFIGWRITE ppfnWriteOld)
{
    NOREF(pDevIns);

    if (ppfnReadOld)
        *ppfnReadOld = pPciDev->Int.s.pfnConfigRead;
    pPciDev->Int.s.pfnConfigRead  = pfnRead;

    if (ppfnWriteOld)
        *ppfnWriteOld = pPciDev->Int.s.pfnConfigWrite;
    pPciDev->Int.s.pfnConfigWrite = pfnWrite;
}

static int ich9pciR3CommonSaveExec(PICH9PCIBUS pBus, PSSMHANDLE pSSM)
{
    /*
     * Iterate thru all the devices.
     */
    for (uint32_t i = 0; i < RT_ELEMENTS(pBus->apDevices); i++)
    {
        PPDMPCIDEV pDev = pBus->apDevices[i];
        if (pDev)
        {
            /* Device position */
            SSMR3PutU32(pSSM, i);
            /* PCI config registers */
            SSMR3PutMem(pSSM, pDev->abConfig, sizeof(pDev->abConfig));

            /* Device flags */
            int rc = SSMR3PutU32(pSSM, pDev->Int.s.fFlags);
            if (RT_FAILURE(rc))
                return rc;

            /* IRQ pin state */
            rc = SSMR3PutS32(pSSM, pDev->Int.s.uIrqPinState);
            if (RT_FAILURE(rc))
                return rc;

            /* MSI info */
            rc = SSMR3PutU8(pSSM, pDev->Int.s.u8MsiCapOffset);
            if (RT_FAILURE(rc))
                return rc;
            rc = SSMR3PutU8(pSSM, pDev->Int.s.u8MsiCapSize);
            if (RT_FAILURE(rc))
                return rc;

            /* MSI-X info */
            rc = SSMR3PutU8(pSSM, pDev->Int.s.u8MsixCapOffset);
            if (RT_FAILURE(rc))
                return rc;
            rc = SSMR3PutU8(pSSM, pDev->Int.s.u8MsixCapSize);
            if (RT_FAILURE(rc))
                return rc;
            /* Save MSI-X page state */
            if (pDev->Int.s.u8MsixCapOffset != 0)
            {
                Assert(pDev->Int.s.pMsixPageR3 != NULL);
                SSMR3PutMem(pSSM, pDev->Int.s.pMsixPageR3, 0x1000);
                if (RT_FAILURE(rc))
                    return rc;
            }
        }
    }
    return SSMR3PutU32(pSSM, UINT32_MAX); /* terminator */
}

static DECLCALLBACK(int) ich9pciR3SaveExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM)
{
    PICH9PCIGLOBALS pThis = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);

    /*
     * Bus state data.
     */
    SSMR3PutU32(pSSM, pThis->uConfigReg);

    /*
     * Save IRQ states.
     */
    for (int i = 0; i < PCI_APIC_IRQ_PINS; i++)
        SSMR3PutU32(pSSM, pThis->uaPciApicIrqLevels[i]);

    SSMR3PutU32(pSSM, UINT32_MAX);  /* separator */

    return ich9pciR3CommonSaveExec(&pThis->aPciBus, pSSM);
}


static DECLCALLBACK(int) ich9pcibridgeR3SaveExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM)
{
    PICH9PCIBUS pThis = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);
    return ich9pciR3CommonSaveExec(pThis, pSSM);
}


static DECLCALLBACK(void) ich9pcibridgeConfigWrite(PPDMDEVINSR3 pDevIns, uint8_t uBus, uint8_t uDevice, uint32_t u32Address, uint32_t u32Value, unsigned cb)
{
    PICH9PCIBUS pBus = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);

    LogFlowFunc((": pDevIns=%p uBus=%d uDevice=%d u32Address=%u u32Value=%u cb=%d\n", pDevIns, uBus, uDevice, u32Address, u32Value, cb));

    /* If the current bus is not the target bus search for the bus which contains the device. */
    /* safe, only needs to go to the config space array */
    if (uBus != PDMPciDevGetByte(&pBus->aPciDev, VBOX_PCI_SECONDARY_BUS))
    {
        PPDMPCIDEV pBridgeDevice = ich9pciFindBridge(pBus, uBus);
        if (pBridgeDevice)
        {
            AssertPtr(pBridgeDevice->Int.s.pfnBridgeConfigWrite);
            pBridgeDevice->Int.s.pfnBridgeConfigWrite(pBridgeDevice->Int.s.CTX_SUFF(pDevIns), uBus, uDevice,
                                                      u32Address, u32Value, cb);
        }
    }
    else
    {
        /* This is the target bus, pass the write to the device. */
        PPDMPCIDEV pPciDev = pBus->apDevices[uDevice];
        if (pPciDev)
        {
            Log(("%s: %s: addr=%02x val=%08x len=%d\n", __FUNCTION__, pPciDev->pszNameR3, u32Address, u32Value, cb));
            pPciDev->Int.s.pfnConfigWrite(pPciDev->Int.s.CTX_SUFF(pDevIns), pPciDev, u32Address, u32Value, cb);
        }
    }
}

static DECLCALLBACK(uint32_t) ich9pcibridgeConfigRead(PPDMDEVINSR3 pDevIns, uint8_t uBus, uint8_t uDevice, uint32_t u32Address, unsigned cb)
{
    PICH9PCIBUS pBus = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);
    uint32_t u32Value;

    LogFlowFunc((": pDevIns=%p uBus=%d uDevice=%d u32Address=%u cb=%d\n", pDevIns, uBus, uDevice, u32Address, cb));

    /* If the current bus is not the target bus search for the bus which contains the device. */
    /* safe, only needs to go to the config space array */
    if (uBus != PDMPciDevGetByte(&pBus->aPciDev, VBOX_PCI_SECONDARY_BUS))
    {
        PPDMPCIDEV pBridgeDevice = ich9pciFindBridge(pBus, uBus);
        if (pBridgeDevice)
        {
            AssertPtr( pBridgeDevice->Int.s.pfnBridgeConfigRead);
            u32Value = pBridgeDevice->Int.s.pfnBridgeConfigRead(pBridgeDevice->Int.s.CTX_SUFF(pDevIns), uBus, uDevice,
                                                                u32Address, cb);
        }
        else
            ich9pciNoMem(&u32Value, 4);
    }
    else
    {
        /* This is the target bus, pass the read to the device. */
        PPDMPCIDEV pPciDev = pBus->apDevices[uDevice];
        if (pPciDev)
        {
            u32Value = pPciDev->Int.s.pfnConfigRead(pPciDev->Int.s.CTX_SUFF(pDevIns), pPciDev, u32Address, cb);
            Log(("%s: %s: u32Address=%02x u32Value=%08x cb=%d\n", __FUNCTION__, pPciDev->pszNameR3, u32Address, u32Value, cb));
        }
        else
            ich9pciNoMem(&u32Value, 4);
    }

    return u32Value;
}


/**
 * Common routine for restoring the config registers of a PCI device.
 *
 * @param   pDev                The PCI device.
 * @param   pbSrcConfig         The configuration register values to be loaded.
 */
static void pciR3CommonRestoreConfig(PPDMPCIDEV pDev, uint8_t const *pbSrcConfig)
{
    /*
     * This table defines the fields for normal devices and bridge devices, and
     * the order in which they need to be restored.
     */
    static const struct PciField
    {
        uint8_t     off;
        uint8_t     cb;
        uint8_t     fWritable;
        uint8_t     fBridge;
        const char *pszName;
    } s_aFields[] =
    {
        /* off,cb,fW,fB, pszName */
        { 0x00, 2, 0, 3, "VENDOR_ID" },
        { 0x02, 2, 0, 3, "DEVICE_ID" },
        { 0x06, 2, 1, 3, "STATUS" },
        { 0x08, 1, 0, 3, "REVISION_ID" },
        { 0x09, 1, 0, 3, "CLASS_PROG" },
        { 0x0a, 1, 0, 3, "CLASS_SUB" },
        { 0x0b, 1, 0, 3, "CLASS_BASE" },
        { 0x0c, 1, 1, 3, "CACHE_LINE_SIZE" },
        { 0x0d, 1, 1, 3, "LATENCY_TIMER" },
        { 0x0e, 1, 0, 3, "HEADER_TYPE" },
        { 0x0f, 1, 1, 3, "BIST" },
        { 0x10, 4, 1, 3, "BASE_ADDRESS_0" },
        { 0x14, 4, 1, 3, "BASE_ADDRESS_1" },
        { 0x18, 4, 1, 1, "BASE_ADDRESS_2" },
        { 0x18, 1, 1, 2, "PRIMARY_BUS" },
        { 0x19, 1, 1, 2, "SECONDARY_BUS" },
        { 0x1a, 1, 1, 2, "SUBORDINATE_BUS" },
        { 0x1b, 1, 1, 2, "SEC_LATENCY_TIMER" },
        { 0x1c, 4, 1, 1, "BASE_ADDRESS_3" },
        { 0x1c, 1, 1, 2, "IO_BASE" },
        { 0x1d, 1, 1, 2, "IO_LIMIT" },
        { 0x1e, 2, 1, 2, "SEC_STATUS" },
        { 0x20, 4, 1, 1, "BASE_ADDRESS_4" },
        { 0x20, 2, 1, 2, "MEMORY_BASE" },
        { 0x22, 2, 1, 2, "MEMORY_LIMIT" },
        { 0x24, 4, 1, 1, "BASE_ADDRESS_5" },
        { 0x24, 2, 1, 2, "PREF_MEMORY_BASE" },
        { 0x26, 2, 1, 2, "PREF_MEMORY_LIMIT" },
        { 0x28, 4, 0, 1, "CARDBUS_CIS" },
        { 0x28, 4, 1, 2, "PREF_BASE_UPPER32" },
        { 0x2c, 2, 0, 1, "SUBSYSTEM_VENDOR_ID" },
        { 0x2c, 4, 1, 2, "PREF_LIMIT_UPPER32" },
        { 0x2e, 2, 0, 1, "SUBSYSTEM_ID" },
        { 0x30, 4, 1, 1, "ROM_ADDRESS" },
        { 0x30, 2, 1, 2, "IO_BASE_UPPER16" },
        { 0x32, 2, 1, 2, "IO_LIMIT_UPPER16" },
        { 0x34, 4, 0, 3, "CAPABILITY_LIST" },
        { 0x38, 4, 1, 1, "RESERVED_38" },
        { 0x38, 4, 1, 2, "ROM_ADDRESS_BR" },
        { 0x3c, 1, 1, 3, "INTERRUPT_LINE" },
        { 0x3d, 1, 0, 3, "INTERRUPT_PIN" },
        { 0x3e, 1, 0, 1, "MIN_GNT" },
        { 0x3e, 2, 1, 2, "BRIDGE_CONTROL" },
        { 0x3f, 1, 0, 1, "MAX_LAT" },
        /* The COMMAND register must come last as it requires the *ADDRESS*
           registers to be restored before we pretent to change it from 0 to
           whatever value the guest assigned it. */
        { 0x04, 2, 1, 3, "COMMAND" },
    };

#ifdef RT_STRICT
    /* Check that we've got full register coverage. */
    uint32_t bmDevice[0x40 / 32];
    uint32_t bmBridge[0x40 / 32];
    RT_ZERO(bmDevice);
    RT_ZERO(bmBridge);
    for (uint32_t i = 0; i < RT_ELEMENTS(s_aFields); i++)
    {
        uint8_t off = s_aFields[i].off;
        uint8_t cb  = s_aFields[i].cb;
        uint8_t f   = s_aFields[i].fBridge;
        while (cb-- > 0)
        {
            if (f & 1) AssertMsg(!ASMBitTest(bmDevice, off), ("%#x\n", off));
            if (f & 2) AssertMsg(!ASMBitTest(bmBridge, off), ("%#x\n", off));
            if (f & 1) ASMBitSet(bmDevice, off);
            if (f & 2) ASMBitSet(bmBridge, off);
            off++;
        }
    }
    for (uint32_t off = 0; off < 0x40; off++)
    {
        AssertMsg(ASMBitTest(bmDevice, off), ("%#x\n", off));
        AssertMsg(ASMBitTest(bmBridge, off), ("%#x\n", off));
    }
#endif

    /*
     * Loop thru the fields covering the 64 bytes of standard registers.
     */
    uint8_t const fBridge = pciDevIsPci2PciBridge(pDev) ? 2 : 1;
    Assert(!pciDevIsPassthrough(pDev));
    uint8_t *pbDstConfig = &pDev->abConfig[0];

    for (uint32_t i = 0; i < RT_ELEMENTS(s_aFields); i++)
        if (s_aFields[i].fBridge & fBridge)
        {
            uint8_t const   off = s_aFields[i].off;
            uint8_t const   cb  = s_aFields[i].cb;
            uint32_t        u32Src;
            uint32_t        u32Dst;
            switch (cb)
            {
                case 1:
                    u32Src = pbSrcConfig[off];
                    u32Dst = pbDstConfig[off];
                    break;
                case 2:
                    u32Src = *(uint16_t const *)&pbSrcConfig[off];
                    u32Dst = *(uint16_t const *)&pbDstConfig[off];
                    break;
                case 4:
                    u32Src = *(uint32_t const *)&pbSrcConfig[off];
                    u32Dst = *(uint32_t const *)&pbDstConfig[off];
                    break;
                default:
                    AssertFailed();
                    continue;
            }

            if (    u32Src != u32Dst
                ||  off == VBOX_PCI_COMMAND)
            {
                if (u32Src != u32Dst)
                {
                    if (!s_aFields[i].fWritable)
                        LogRel(("PCI: %8s/%u: %2u-bit field %s: %x -> %x - !READ ONLY!\n",
                                pDev->pszNameR3, pDev->Int.s.CTX_SUFF(pDevIns)->iInstance, cb*8, s_aFields[i].pszName, u32Dst, u32Src));
                    else
                        LogRel(("PCI: %8s/%u: %2u-bit field %s: %x -> %x\n",
                                pDev->pszNameR3, pDev->Int.s.CTX_SUFF(pDevIns)->iInstance, cb*8, s_aFields[i].pszName, u32Dst, u32Src));
                }
                if (off == VBOX_PCI_COMMAND)
                    /* safe, only needs to go to the config space array */
                    PDMPciDevSetCommand(pDev, 0); /* For remapping, see ich9pciR3CommonLoadExec. */
                pDev->Int.s.pfnConfigWrite(pDev->Int.s.CTX_SUFF(pDevIns), pDev, off, u32Src, cb);
            }
        }

    /*
     * The device dependent registers.
     *
     * We will not use ConfigWrite here as we have no clue about the size
     * of the registers, so the device is responsible for correctly
     * restoring functionality governed by these registers.
     */
    for (uint32_t off = 0x40; off < sizeof(pDev->abConfig); off++)
        if (pbDstConfig[off] != pbSrcConfig[off])
        {
            LogRel(("PCI: %8s/%u: register %02x: %02x -> %02x\n",
                    pDev->pszNameR3, pDev->Int.s.CTX_SUFF(pDevIns)->iInstance, off, pbDstConfig[off], pbSrcConfig[off])); /** @todo make this Log() later. */
            pbDstConfig[off] = pbSrcConfig[off];
        }
}

/**
 * Common worker for ich9pciR3LoadExec and ich9pcibridgeR3LoadExec.
 *
 * @returns VBox status code.
 * @param   pBus                The bus which data is being loaded.
 * @param   pSSM                The saved state handle.
 * @param   uVersion            The data version.
 * @param   uPass               The pass.
 */
static DECLCALLBACK(int) ich9pciR3CommonLoadExec(PICH9PCIBUS pBus, PSSMHANDLE pSSM, uint32_t uVersion, uint32_t uPass)
{
    uint32_t    u32;
    uint32_t    i;
    int         rc;

    Assert(uPass == SSM_PASS_FINAL); NOREF(uPass);
    if (uVersion != VBOX_ICH9PCI_SAVED_STATE_VERSION_CURRENT)
        return VERR_SSM_UNSUPPORTED_DATA_UNIT_VERSION;

    /*
     * Iterate thru all the devices and write 0 to the COMMAND register so
     * that all the memory is unmapped before we start restoring the saved
     * mapping locations.
     *
     * The register value is restored afterwards so we can do proper
     * LogRels in pciR3CommonRestoreConfig.
     */
    for (i = 0; i < RT_ELEMENTS(pBus->apDevices); i++)
    {
        PPDMPCIDEV pDev = pBus->apDevices[i];
        if (pDev)
        {
            /* safe, only needs to go to the config space array */
            uint16_t u16 = PDMPciDevGetCommand(pDev);
            pDev->Int.s.pfnConfigWrite(pDev->Int.s.CTX_SUFF(pDevIns), pDev, VBOX_PCI_COMMAND, 0, 2);
            /* safe, only needs to go to the config space array */
            PDMPciDevSetCommand(pDev, u16);
            /* safe, only needs to go to the config space array */
            Assert(PDMPciDevGetCommand(pDev) == u16);
        }
    }

    void *pvMsixPage = RTMemTmpAllocZ(0x1000);
    AssertReturn(pvMsixPage, VERR_NO_TMP_MEMORY);

    /*
     * Iterate all the devices.
     */
    for (i = 0;; i++)
    {
        PPDMPCIDEV  pDev;
        PDMPCIDEV   DevTmp;

        /* index / terminator */
        rc = SSMR3GetU32(pSSM, &u32);
        if (RT_FAILURE(rc))
            break;
        if (u32 == (uint32_t)~0)
            break;
        AssertMsgBreak(u32 < RT_ELEMENTS(pBus->apDevices) && u32 >= i, ("u32=%#x i=%#x\n", u32, i));

        /* skip forward to the device checking that no new devices are present. */
        for (; i < u32; i++)
        {
            pDev = pBus->apDevices[i];
            if (pDev)
            {
                /* safe, only needs to go to the config space array */
                LogRel(("PCI: New device in slot %#x, %s (vendor=%#06x device=%#06x)\n", i, pDev->pszNameR3,
                        PDMPciDevGetVendorId(pDev), PDMPciDevGetDeviceId(pDev)));
                if (SSMR3HandleGetAfter(pSSM) != SSMAFTER_DEBUG_IT)
                {
                    /* safe, only needs to go to the config space array */
                    rc = SSMR3SetCfgError(pSSM, RT_SRC_POS, N_("New device in slot %#x, %s (vendor=%#06x device=%#06x)"),
                                          i, pDev->pszNameR3, PDMPciDevGetVendorId(pDev), PDMPciDevGetDeviceId(pDev));
                    break;
                }
            }
        }
        if (RT_FAILURE(rc))
            break;

        /* get the data */
        DevTmp.Int.s.fFlags = 0;
        DevTmp.Int.s.u8MsiCapOffset = 0;
        DevTmp.Int.s.u8MsiCapSize = 0;
        DevTmp.Int.s.u8MsixCapOffset = 0;
        DevTmp.Int.s.u8MsixCapSize = 0;
        DevTmp.Int.s.uIrqPinState = ~0; /* Invalid value in case we have an older saved state to force a state change in pciSetIrq. */
        SSMR3GetMem(pSSM, DevTmp.abConfig, sizeof(DevTmp.abConfig));

        SSMR3GetU32(pSSM, &DevTmp.Int.s.fFlags);
        SSMR3GetS32(pSSM, &DevTmp.Int.s.uIrqPinState);
        SSMR3GetU8(pSSM, &DevTmp.Int.s.u8MsiCapOffset);
        SSMR3GetU8(pSSM, &DevTmp.Int.s.u8MsiCapSize);
        SSMR3GetU8(pSSM, &DevTmp.Int.s.u8MsixCapOffset);
        rc = SSMR3GetU8(pSSM, &DevTmp.Int.s.u8MsixCapSize);
        if (RT_FAILURE(rc))
            break;

        /* Load MSI-X page state */
        if (DevTmp.Int.s.u8MsixCapOffset != 0)
        {
            Assert(pvMsixPage != NULL);
            rc = SSMR3GetMem(pSSM, pvMsixPage, 0x1000);
            if (RT_FAILURE(rc))
                break;
        }

        /* check that it's still around. */
        pDev = pBus->apDevices[i];
        if (!pDev)
        {
            /* safe, only needs to go to the config space array */
            LogRel(("PCI: Device in slot %#x has been removed! vendor=%#06x device=%#06x\n", i,
                    PDMPciDevGetVendorId(&DevTmp), PDMPciDevGetDeviceId(&DevTmp)));
            if (SSMR3HandleGetAfter(pSSM) != SSMAFTER_DEBUG_IT)
            {
                /* safe, only needs to go to the config space array */
                rc = SSMR3SetCfgError(pSSM, RT_SRC_POS, N_("Device in slot %#x has been removed! vendor=%#06x device=%#06x"),
                                      i, PDMPciDevGetVendorId(&DevTmp), PDMPciDevGetDeviceId(&DevTmp));
                break;
            }
            continue;
        }

        /* match the vendor id assuming that this will never be changed. */
        /* safe, only needs to go to the config space array */
        if (PDMPciDevGetVendorId(&DevTmp) != PDMPciDevGetVendorId(pDev))
        {
            /* safe, only needs to go to the config space array */
            rc = SSMR3SetCfgError(pSSM, RT_SRC_POS, N_("Device in slot %#x (%s) vendor id mismatch! saved=%.4Rhxs current=%.4Rhxs"),
                                  i, pDev->pszNameR3, PDMPciDevGetVendorId(&DevTmp), PDMPciDevGetVendorId(pDev));
            break;
        }

        /* commit the loaded device config. */
        Assert(!pciDevIsPassthrough(pDev));
        pciR3CommonRestoreConfig(pDev, &DevTmp.abConfig[0]);

        pDev->Int.s.uIrqPinState = DevTmp.Int.s.uIrqPinState;
        pDev->Int.s.u8MsiCapOffset  = DevTmp.Int.s.u8MsiCapOffset;
        pDev->Int.s.u8MsiCapSize    = DevTmp.Int.s.u8MsiCapSize;
        pDev->Int.s.u8MsixCapOffset = DevTmp.Int.s.u8MsixCapOffset;
        pDev->Int.s.u8MsixCapSize   = DevTmp.Int.s.u8MsixCapSize;
        if (DevTmp.Int.s.u8MsixCapSize != 0)
        {
            Assert(pDev->Int.s.pMsixPageR3 != NULL);
            memcpy(pDev->Int.s.pMsixPageR3, pvMsixPage, 0x1000);
        }
    }

    RTMemTmpFree(pvMsixPage);

    return rc;
}

static DECLCALLBACK(int) ich9pciR3LoadExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM, uint32_t uVersion, uint32_t uPass)
{
    PICH9PCIGLOBALS pThis = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);
    PICH9PCIBUS     pBus  = &pThis->aPciBus;
    uint32_t        u32;
    int             rc;

    /* We ignore this version as there's no saved state with it anyway */
    if (uVersion == VBOX_ICH9PCI_SAVED_STATE_VERSION_NOMSI)
        return VERR_SSM_UNSUPPORTED_DATA_UNIT_VERSION;
    if (uVersion > VBOX_ICH9PCI_SAVED_STATE_VERSION_MSI)
        return VERR_SSM_UNSUPPORTED_DATA_UNIT_VERSION;

    /*
     * Bus state data.
     */
    SSMR3GetU32(pSSM, &pThis->uConfigReg);

    /*
     * Load IRQ states.
     */
    for (int i = 0; i < PCI_APIC_IRQ_PINS; i++)
        SSMR3GetU32(pSSM, (uint32_t*)&pThis->uaPciApicIrqLevels[i]);

    /* separator */
    rc = SSMR3GetU32(pSSM, &u32);
    if (RT_FAILURE(rc))
        return rc;
    if (u32 != (uint32_t)~0)
        AssertMsgFailedReturn(("u32=%#x\n", u32), rc);

    return ich9pciR3CommonLoadExec(pBus, pSSM, uVersion, uPass);
}

static DECLCALLBACK(int) ich9pcibridgeR3LoadExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM, uint32_t uVersion, uint32_t uPass)
{
    PICH9PCIBUS pThis = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);
    if (uVersion > VBOX_ICH9PCI_SAVED_STATE_VERSION_MSI)
        return VERR_SSM_UNSUPPORTED_DATA_UNIT_VERSION;
    return ich9pciR3CommonLoadExec(pThis, pSSM, uVersion, uPass);
}


/*
 * Perform immediate read of configuration space register.
 * Cannot be rescheduled, as already in R3.
 */
static uint32_t ich9pciConfigRead(PICH9PCIGLOBALS pGlobals, uint8_t uBus, uint8_t uDevFn, uint32_t addr, uint32_t len)
{
    PciAddress aPciAddr;
    aPciAddr.iBus = uBus;
    aPciAddr.iDeviceFunc = uDevFn;
    aPciAddr.iRegister = addr;

    uint32_t u32Val;
    int rc = ich9pciDataReadAddr(pGlobals, &aPciAddr, len, &u32Val, VERR_INTERNAL_ERROR);
    AssertRC(rc);

    return u32Val;
}


/*
 * Perform imeediate write to configuration space register.
 * Cannot be rescheduled, as already in R3.
 */
static void ich9pciConfigWrite(PICH9PCIGLOBALS pGlobals, uint8_t uBus, uint8_t uDevFn, uint32_t addr, uint32_t val, uint32_t len)
{
    PciAddress aPciAddr;
    aPciAddr.iBus = uBus;
    aPciAddr.iDeviceFunc = uDevFn;
    aPciAddr.iRegister = addr;

    int rc = ich9pciDataWriteAddr(pGlobals, &aPciAddr, val, len, VERR_INTERNAL_ERROR);
    AssertRC(rc);
}


static void ich9pciSetRegionAddress(PICH9PCIGLOBALS pGlobals, uint8_t uBus, uint8_t uDevFn, int iRegion, uint64_t addr)
{
    uint32_t uReg = ich9pciGetRegionReg(iRegion);

    /* Read memory type first. */
    uint8_t uResourceType = ich9pciConfigRead(pGlobals, uBus, uDevFn, uReg, 1);
    bool    f64Bit =    (uResourceType & ((uint8_t)(PCI_ADDRESS_SPACE_BAR64 | PCI_ADDRESS_SPACE_IO)))
                     == PCI_ADDRESS_SPACE_BAR64;

    Log(("Set region address: %02x:%02x.%d region %d address=%RX64%s\n",
         uBus, uDevFn >> 3, uDevFn & 7, iRegion, addr, f64Bit ? " (64-bit)" : ""));

    /* Write address of the device. */
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, uReg, (uint32_t)addr, 4);
    if (f64Bit)
        ich9pciConfigWrite(pGlobals, uBus, uDevFn, uReg + 4, (uint32_t)(addr >> 32), 4);
}


static void ich9pciBiosInitBridge(PICH9PCIGLOBALS pGlobals, uint8_t uBus, uint8_t uDevFn)
{
    Log(("BIOS init bridge: %02x:%02x.%d\n", uBus, uDevFn >> 3, uDevFn & 7));

    /*
     * The I/O range for the bridge must be aligned to a 4KB boundary.
     * This does not change anything really as the access to the device is not going
     * through the bridge but we want to be compliant to the spec.
     */
    if ((pGlobals->uPciBiosIo % _4K) != 0)
    {
        pGlobals->uPciBiosIo = RT_ALIGN_32(pGlobals->uPciBiosIo, _4K);
        Log(("%s: Aligned I/O start address. New address %#x\n", __FUNCTION__, pGlobals->uPciBiosIo));
    }
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_IO_BASE, (pGlobals->uPciBiosIo >> 8) & 0xf0, 1);

    /* The MMIO range for the bridge must be aligned to a 1MB boundary. */
    if ((pGlobals->uPciBiosMmio % _1M) != 0)
    {
        pGlobals->uPciBiosMmio = RT_ALIGN_32(pGlobals->uPciBiosMmio, _1M);
        Log(("%s: Aligned MMIO start address. New address %#x\n", __FUNCTION__, pGlobals->uPciBiosMmio));
    }
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_MEMORY_BASE, (pGlobals->uPciBiosMmio >> 16) & UINT32_C(0xffff0), 2);

    /* Save values to compare later to. */
    uint32_t u32IoAddressBase = pGlobals->uPciBiosIo;
    uint32_t u32MMIOAddressBase = pGlobals->uPciBiosMmio;
    uint8_t uBridgeBus = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_SECONDARY_BUS, 1);

    /* Init all devices behind the bridge (recursing to further buses). */
    ich9pciBiosInitAllDevicesOnBus(pGlobals, uBridgeBus);

    /*
     * Set I/O limit register. If there is no device with I/O space behind the
     * bridge we set a lower value than in the base register.
     */
    if (u32IoAddressBase != pGlobals->uPciBiosIo)
    {
        /* Need again alignment to a 4KB boundary. */
        pGlobals->uPciBiosIo = RT_ALIGN_32(pGlobals->uPciBiosIo, _4K);
        ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_IO_LIMIT, ((pGlobals->uPciBiosIo -1) >> 8) & 0xf0, 1);
    }
    else
    {
        ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_IO_BASE, 0xf0, 1);
        ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_IO_LIMIT, 0x00, 1);
    }

    /* Same with the MMIO limit register but with 1MB boundary here. */
    if (u32MMIOAddressBase != pGlobals->uPciBiosMmio)
    {
        pGlobals->uPciBiosMmio = RT_ALIGN_32(pGlobals->uPciBiosMmio, _1M);
        ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_MEMORY_LIMIT, ((pGlobals->uPciBiosMmio - 1) >> 16) & UINT32_C(0xfff0), 2);
    }
    else
    {
        ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_MEMORY_BASE, 0xfff0, 2);
        ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_MEMORY_LIMIT, 0x0000, 2);
    }

    /*
     * Set the prefetch base and limit registers. We currently have no device with a prefetchable region
     * which may be behind a bridge. That's why it is unconditionally disabled here atm by writing a higher value into
     * the base register than in the limit register.
     */
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_PREF_MEMORY_BASE, 0xfff0, 2);
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_PREF_MEMORY_LIMIT, 0x0000, 2);
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_PREF_BASE_UPPER32, 0x00000000, 4);
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_PREF_LIMIT_UPPER32, 0x00000000, 4);
}

static int ichpciBiosInitDeviceGetRegions(PICH9PCIGLOBALS pGlobals, uint8_t uBus, uint8_t uDevFn)
{
    uint8_t uHeaderType = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_HEADER_TYPE, 1) & 0x7f;
    if (uHeaderType == 0x00)
        /* Ignore ROM region here, which is included in VBOX_PCI_NUM_REGIONS. */
        return PCI_NUM_REGIONS - 1;
    else if (uHeaderType == 0x01)
        /* PCI bridges have 2 BARs. */
        return 2;
    else
    {
        AssertMsgFailed(("invalid header type %#x\n", uHeaderType));
        return 0;
    }
}

static void ich9pciBiosInitDeviceBARs(PICH9PCIGLOBALS pGlobals, uint8_t uBus, uint8_t uDevFn)
{
    int cRegions = ichpciBiosInitDeviceGetRegions(pGlobals, uBus, uDevFn);
    bool fSuppressMem = false;
    bool fActiveMemRegion = false;
    bool fActiveIORegion = false;
    for (int iRegion = 0; iRegion < cRegions; iRegion++)
    {
        uint32_t u32Address = ich9pciGetRegionReg(iRegion);

        /* Calculate size - we write all 1s into the BAR, and then evaluate which bits
           are cleared. */
        uint8_t u8ResourceType = ich9pciConfigRead(pGlobals, uBus, uDevFn, u32Address, 1);

        bool fPrefetch =    (u8ResourceType & ((uint8_t)(PCI_ADDRESS_SPACE_MEM_PREFETCH | PCI_ADDRESS_SPACE_IO)))
                         == PCI_ADDRESS_SPACE_MEM_PREFETCH;
        bool f64Bit =    (u8ResourceType & ((uint8_t)(PCI_ADDRESS_SPACE_BAR64 | PCI_ADDRESS_SPACE_IO)))
                      == PCI_ADDRESS_SPACE_BAR64;
        bool fIsPio = ((u8ResourceType & PCI_ADDRESS_SPACE_IO) == PCI_ADDRESS_SPACE_IO);
        uint64_t cbRegSize64 = 0;

        /* Hack: since this PCI init code cannot handle prefetchable BARs on
         * anything besides the primary bus, it's for now the best solution
         * to leave such BARs uninitialized and keep memory transactions
         * disabled. The OS will hopefully be clever enough to fix this.
         * Prefetchable BARs are the only ones which can be truly big (and
         * are almost always 64-bit BARs). The non-prefetchable ones will not
         * cause running out of space in the PCI memory hole. */
        if (fPrefetch && uBus != 0)
        {
            fSuppressMem = true;
            if (f64Bit)
                iRegion++; /* skip next region */
            continue;
        }

        if (f64Bit)
        {
            ich9pciConfigWrite(pGlobals, uBus, uDevFn, u32Address,   UINT32_C(0xffffffff), 4);
            ich9pciConfigWrite(pGlobals, uBus, uDevFn, u32Address+4, UINT32_C(0xffffffff), 4);
            cbRegSize64 = RT_MAKE_U64(ich9pciConfigRead(pGlobals, uBus, uDevFn, u32Address,   4),
                                      ich9pciConfigRead(pGlobals, uBus, uDevFn, u32Address+4, 4));
            cbRegSize64 &= ~UINT64_C(0x0f);
            cbRegSize64 = (~cbRegSize64) + 1;

            /* No 64-bit PIO regions possible. */
#ifndef DEBUG_bird /* EFI triggers this for DevAHCI. */
            AssertMsg((u8ResourceType & PCI_ADDRESS_SPACE_IO) == 0, ("type=%#x rgn=%d\n", u8ResourceType, iRegion));
#endif
        }
        else
        {
            uint32_t cbRegSize32;
            ich9pciConfigWrite(pGlobals, uBus, uDevFn, u32Address, UINT32_C(0xffffffff), 4);
            cbRegSize32 = ich9pciConfigRead(pGlobals, uBus, uDevFn, u32Address, 4);

            /* Clear resource information depending on resource type. */
            if (fIsPio) /* PIO */
                cbRegSize32 &= ~UINT32_C(0x01);
            else        /* MMIO */
                cbRegSize32 &= ~UINT32_C(0x0f);

            /*
             * Invert all bits and add 1 to get size of the region.
             * (From PCI implementation note)
             */
            if (fIsPio && (cbRegSize32 & UINT32_C(0xffff0000)) == 0)
                cbRegSize32 = (~(cbRegSize32 | UINT32_C(0xffff0000))) + 1;
            else
                cbRegSize32 = (~cbRegSize32) + 1;

            cbRegSize64 = cbRegSize32;
        }
        Log2(("%s: Size of region %u for device %d on bus %d is %lld\n", __FUNCTION__, iRegion, uDevFn, uBus, cbRegSize64));

        if (cbRegSize64)
        {
            /* Try 32-bit base first. */
            uint32_t* paddr = fIsPio ? &pGlobals->uPciBiosIo : &pGlobals->uPciBiosMmio;
            uint64_t  uNew = *paddr;
            /* Align starting address to region size. */
            uNew = (uNew + cbRegSize64 - 1) & ~(cbRegSize64 - 1);
            if (fIsPio)
                uNew &= UINT32_C(0xffff);
            /* Unconditionally exclude I/O-APIC/HPET/ROM. Pessimistic, but better than causing a mess. */
            if (   !uNew
                || (uNew <= UINT32_C(0xffffffff) && uNew + cbRegSize64 - 1 >= UINT32_C(0xfec00000))
                || uNew >= _4G)
            {
                /* Only prefetchable regions can be placed above 4GB, as the
                 * address decoder for non-prefetchable addresses in bridges
                 * is limited to 32 bits. */
                if (f64Bit && fPrefetch)
                {
                    /* Map a 64-bit region above 4GB. */
                    Assert(!fIsPio);
                    uNew = pGlobals->uPciBiosMmio64;
                    /* Align starting address to region size. */
                    uNew = (uNew + cbRegSize64 - 1) & ~(cbRegSize64 - 1);
                    LogFunc(("Start address of 64-bit MMIO region %u/%u is %#llx\n", iRegion, iRegion + 1, uNew));
                    ich9pciSetRegionAddress(pGlobals, uBus, uDevFn, iRegion, uNew);
                    fActiveMemRegion = true;
                    pGlobals->uPciBiosMmio64 = uNew + cbRegSize64;
                    Log2Func(("New 64-bit address is %#llx\n", pGlobals->uPciBiosMmio64));
                }
                else
                {
                    uint16_t uVendor = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_VENDOR_ID, 2);
                    uint16_t uDevice = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_DEVICE_ID, 2);
                    LogRel(("PCI: no space left for BAR%u of device %u/%u/%u (vendor=%#06x device=%#06x)\n",
                            iRegion, uBus, uDevFn >> 3, uDevFn & 7, uVendor, uDevice)); /** @todo make this a VM start failure later. */
                    /* Undo the mapping mess caused by the size probing. */
                    ich9pciConfigWrite(pGlobals, uBus, uDevFn, u32Address, UINT32_C(0), 4);
                }
            }
            else
            {
                LogFunc(("Start address of %s region %u is %#x\n", (fIsPio ? "I/O" : "MMIO"), iRegion, uNew));
                ich9pciSetRegionAddress(pGlobals, uBus, uDevFn, iRegion, uNew);
                if (fIsPio)
                    fActiveIORegion = true;
                else
                    fActiveMemRegion = true;
                *paddr = uNew + cbRegSize64;
                Log2Func(("New 32-bit address is %#x\n", *paddr));
            }

            if (f64Bit)
                iRegion++; /* skip next region */
        }
    }

    /* Update the command word appropriately. */
    uint8_t uCmd = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_COMMAND, 2);
    if (fActiveMemRegion && !fSuppressMem)
        uCmd |= VBOX_PCI_COMMAND_MEMORY; /* Enable MMIO access. */
    if (fActiveIORegion)
        uCmd |= VBOX_PCI_COMMAND_IO; /* Enable I/O space access. */
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_COMMAND, uCmd, 2);
}

static bool ich9pciBiosInitDevicePrefetchableBARs(PICH9PCIGLOBALS pGlobals, uint8_t uBus, uint8_t uDevFn, bool fUse64Bit, bool fDryrun)
{
    int cRegions = ichpciBiosInitDeviceGetRegions(pGlobals, uBus, uDevFn);
    bool fActiveMemRegion = false;
    for (int iRegion = 0; iRegion < cRegions; iRegion++)
    {
        uint32_t u32Address = ich9pciGetRegionReg(iRegion);
        uint8_t u8ResourceType = ich9pciConfigRead(pGlobals, uBus, uDevFn, u32Address, 1);
        bool fPrefetch =    (u8ResourceType & ((uint8_t)(PCI_ADDRESS_SPACE_MEM_PREFETCH | PCI_ADDRESS_SPACE_IO)))
                      == PCI_ADDRESS_SPACE_MEM_PREFETCH;
        bool f64Bit =    (u8ResourceType & ((uint8_t)(PCI_ADDRESS_SPACE_BAR64 | PCI_ADDRESS_SPACE_IO)))
                      == PCI_ADDRESS_SPACE_BAR64;
        uint64_t cbRegSize64 = 0;

        /* Everything besides prefetchable regions has been set up already. */
        if (!fPrefetch)
            continue;

        if (f64Bit)
        {
            ich9pciConfigWrite(pGlobals, uBus, uDevFn, u32Address,   UINT32_C(0xffffffff), 4);
            ich9pciConfigWrite(pGlobals, uBus, uDevFn, u32Address+4, UINT32_C(0xffffffff), 4);
            cbRegSize64 = RT_MAKE_U64(ich9pciConfigRead(pGlobals, uBus, uDevFn, u32Address,   4),
                                      ich9pciConfigRead(pGlobals, uBus, uDevFn, u32Address+4, 4));
            cbRegSize64 &= ~UINT64_C(0x0f);
            cbRegSize64 = (~cbRegSize64) + 1;
        }
        else
        {
            uint32_t cbRegSize32;
            ich9pciConfigWrite(pGlobals, uBus, uDevFn, u32Address, UINT32_C(0xffffffff), 4);
            cbRegSize32 = ich9pciConfigRead(pGlobals, uBus, uDevFn, u32Address, 4);
            cbRegSize32 &= ~UINT32_C(0x0f);
            cbRegSize32 = (~cbRegSize32) + 1;

            cbRegSize64 = cbRegSize32;
        }
        Log2(("%s: Size of region %u for device %d on bus %d is %lld\n", __FUNCTION__, iRegion, uDevFn, uBus, cbRegSize64));

        if (cbRegSize64)
        {
            uint64_t uNew;
            if (!fUse64Bit)
            {
                uNew = pGlobals->uPciBiosMmio;
                /* Align starting address to region size. */
                uNew = (uNew + cbRegSize64 - 1) & ~(cbRegSize64 - 1);
                /* Unconditionally exclude I/O-APIC/HPET/ROM. Pessimistic, but better than causing a mess. */
                if (   !uNew
                    || (uNew <= UINT32_C(0xffffffff) && uNew + cbRegSize64 - 1 >= UINT32_C(0xfec00000))
                    || uNew >= _4G)
                {
                    Assert(fDryrun);
                    return true;
                }
                if (!fDryrun)
                {
                    LogFunc(("Start address of MMIO region %u is %#x\n", iRegion, uNew));
                    ich9pciSetRegionAddress(pGlobals, uBus, uDevFn, iRegion, uNew);
                    fActiveMemRegion = true;
                }
                pGlobals->uPciBiosMmio = uNew + cbRegSize64;
            }
            else
            {
                /* Can't handle 32-bit BARs when forcing 64-bit allocs. */
                if (!f64Bit)
                {
                    Assert(fDryrun);
                    return true;
                }
                uNew = pGlobals->uPciBiosMmio64;
                /* Align starting address to region size. */
                uNew = (uNew + cbRegSize64 - 1) & ~(cbRegSize64 - 1);
                pGlobals->uPciBiosMmio64 = uNew + cbRegSize64;
                if (!fDryrun)
                {
                    LogFunc(("Start address of 64-bit MMIO region %u/%u is %#llx\n", iRegion, iRegion + 1, uNew));
                    ich9pciSetRegionAddress(pGlobals, uBus, uDevFn, iRegion, uNew);
                    fActiveMemRegion = true;
                }
            }

            if (f64Bit)
                iRegion++; /* skip next region */
        }
    }

    if (!fDryrun)
    {
        /* Update the command word appropriately. */
        uint8_t uCmd = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_COMMAND, 2);
        if (fActiveMemRegion)
            uCmd |= VBOX_PCI_COMMAND_MEMORY; /* Enable MMIO access. */
        ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_COMMAND, uCmd, 2);
    }
    else
        Assert(!fActiveMemRegion);

    return false;
}

static bool ich9pciBiosInitBridgePrefetchable(PICH9PCIGLOBALS pGlobals, uint8_t uBus, uint8_t uDevFn, bool fUse64Bit, bool fDryrun)
{
    Log(("BIOS init bridge (prefetch): %02x:%02x.%d\n", uBus, uDevFn >> 3, uDevFn & 7));

    pGlobals->uPciBiosMmio = RT_ALIGN_32(pGlobals->uPciBiosMmio, _1M);
    pGlobals->uPciBiosMmio64 = RT_ALIGN_64(pGlobals->uPciBiosMmio64, _1M);

    /* Save values to compare later to. */
    uint32_t u32MMIOAddressBase = pGlobals->uPciBiosMmio;
    uint64_t u64MMIOAddressBase = pGlobals->uPciBiosMmio64;

    uint8_t uBridgeBus = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_SECONDARY_BUS, 1);

    /* Init all devices behind the bridge (recursing to further buses). */
    bool fRes = ich9pciBiosInitAllDevicesPrefetchableOnBus(pGlobals, uBridgeBus, fUse64Bit, fDryrun);
    if (fDryrun)
        return fRes;
    Assert(!fRes);

    /* Set prefetchable MMIO limit register with 1MB boundary. */
    uint64_t uBase, uLimit;
    if (fUse64Bit)
    {
        if (u64MMIOAddressBase == pGlobals->uPciBiosMmio64)
            return false;
        uBase = u64MMIOAddressBase;
        uLimit = RT_ALIGN_64(pGlobals->uPciBiosMmio64, _1M) - 1;
    }
    else
    {
        if (u32MMIOAddressBase == pGlobals->uPciBiosMmio)
            return false;
        uBase = u32MMIOAddressBase;
        uLimit = RT_ALIGN_32(pGlobals->uPciBiosMmio, _1M) - 1;
    }
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_PREF_BASE_UPPER32, uBase >> 32, 4);
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_PREF_MEMORY_BASE, (uint32_t)(uBase >> 16) & UINT32_C(0xfff0), 2);
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_PREF_LIMIT_UPPER32, uLimit >> 32, 4);
    ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_PREF_MEMORY_LIMIT, (uint32_t)(uLimit >> 16) & UINT32_C(0xfff0), 2);

    return false;
}

static bool ich9pciBiosInitAllDevicesPrefetchableOnBus(PICH9PCIGLOBALS pGlobals, uint8_t uBus, bool fUse64Bit, bool fDryrun)
{
    /* First pass: assign resources to all devices. */
    for (uint32_t uDevFn = 0; uDevFn < 256; uDevFn++)
    {
        /* check if device is present */
        uint16_t uVendor = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_VENDOR_ID, 2);
        if (uVendor == 0xffff)
            continue;

        Log(("BIOS init device (prefetch): %02x:%02x.%d\n", uBus, uDevFn >> 3, uDevFn & 7));

        /* prefetchable memory mappings */
        bool fRes = ich9pciBiosInitDevicePrefetchableBARs(pGlobals, uBus, uDevFn, fUse64Bit, fDryrun);
        if (fRes)
        {
            Assert(fDryrun);
            return fRes;
        }
    }

    /* Second pass: handle bridges recursively. */
    for (uint32_t uDevFn = 0; uDevFn < 256; uDevFn++)
    {
        /* check if device is present */
        uint16_t uVendor = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_VENDOR_ID, 2);
        if (uVendor == 0xffff)
            continue;

        /* only handle PCI-to-PCI bridges */
        uint16_t uDevClass = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_CLASS_DEVICE, 2);
        if (uDevClass != 0x0604)
            continue;

        Log(("BIOS init bridge (prefetch): %02x:%02x.%d\n", uBus, uDevFn >> 3, uDevFn & 7));
        bool fRes = ich9pciBiosInitBridgePrefetchable(pGlobals, uBus, uDevFn, fUse64Bit, fDryrun);
        if (fRes)
        {
            Assert(fDryrun);
            return fRes;
        }
    }
    return false;
}

static void ich9pciBiosInitAllDevicesOnBus(PICH9PCIGLOBALS pGlobals, uint8_t uBus)
{
    /* First pass: assign resources to all devices and map the interrupt. */
    for (uint32_t uDevFn = 0; uDevFn < 256; uDevFn++)
    {
        /* check if device is present */
        uint16_t uVendor = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_VENDOR_ID, 2);
        if (uVendor == 0xffff)
            continue;

        Log(("BIOS init device: %02x:%02x.%d\n", uBus, uDevFn >> 3, uDevFn & 7));

        /* default memory mappings */
        ich9pciBiosInitDeviceBARs(pGlobals, uBus, uDevFn);
        uint16_t uDevClass = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_CLASS_DEVICE, 2);
        switch (uDevClass)
        {
            case 0x0101:
                /* IDE controller */
                ich9pciConfigWrite(pGlobals, uBus, uDevFn, 0x40, 0x8000, 2); /* enable IDE0 */
                ich9pciConfigWrite(pGlobals, uBus, uDevFn, 0x42, 0x8000, 2); /* enable IDE1 */
                break;
            case 0x0300:
            {
                /* VGA controller */

                /* NB: Default Bochs VGA LFB address is 0xE0000000. Old guest
                 * software may break if the framebuffer isn't mapped there.
                 */

                /*
                 * Legacy VGA I/O ports are implicitly decoded by a VGA class device. But
                 * only the framebuffer (i.e., a memory region) is explicitly registered via
                 * ich9pciSetRegionAddress, so don't forget to enable I/O decoding.
                 */
                uint8_t uCmd = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_COMMAND, 1);
                ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_COMMAND,
                                   uCmd | VBOX_PCI_COMMAND_IO,
                                   1);
                break;
            }
            default:
                break;
        }

        /* map the interrupt */
        uint32_t iPin = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_INTERRUPT_PIN, 1);
        if (iPin != 0)
        {
            iPin--;

            if (uBus != 0)
            {
                /* Find bus this device attached to. */
                PICH9PCIBUS pBus = &pGlobals->aPciBus;
                while (1)
                {
                    PPDMPCIDEV pBridge = ich9pciFindBridge(pBus, uBus);
                    if (!pBridge)
                    {
                        Assert(false);
                        break;
                    }
                    /* safe, only needs to go to the config space array */
                    if (uBus == PDMPciDevGetByte(pBridge, VBOX_PCI_SECONDARY_BUS))
                    {
                        /* OK, found bus this device attached to. */
                        break;
                    }
                    pBus = PDMINS_2_DATA(pBridge->Int.s.CTX_SUFF(pDevIns), PICH9PCIBUS);
                }

                /* We need to go up to the host bus to see which irq pin this
                 * device will use there. See logic in ich9pcibridgeSetIrq().
                 */
                while (pBus->iBus != 0)
                {
                    /* Get the pin the device would assert on the bridge. */
                    iPin = ((pBus->aPciDev.uDevFn >> 3) + iPin) & 3;
                    pBus = pBus->aPciDev.Int.s.pBusR3;
                };
            }

            int iIrq = aPciIrqs[ich9pciSlotGetPirq(uBus, uDevFn, iPin)];
            Log(("Using pin %d and IRQ %d for device %02x:%02x.%d\n",
                 iPin, iIrq, uBus, uDevFn>>3, uDevFn&7));
            ich9pciConfigWrite(pGlobals, uBus, uDevFn, VBOX_PCI_INTERRUPT_LINE, iIrq, 1);
        }
    }

    /* Second pass: handle bridges recursively. */
    for (uint32_t uDevFn = 0; uDevFn < 256; uDevFn++)
    {
        /* check if device is present */
        uint16_t uVendor = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_VENDOR_ID, 2);
        if (uVendor == 0xffff)
            continue;

        /* only handle PCI-to-PCI bridges */
        uint16_t uDevClass = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_CLASS_DEVICE, 2);
        if (uDevClass != 0x0604)
            continue;

        Log(("BIOS init bridge: %02x:%02x.%d\n", uBus, uDevFn >> 3, uDevFn & 7));
        ich9pciBiosInitBridge(pGlobals, uBus, uDevFn);
    }

    /* Third pass (only for bus 0): set up prefetchable bars recursively. */
    if (uBus == 0)
    {
        for (uint32_t uDevFn = 0; uDevFn < 256; uDevFn++)
        {
            /* check if device is present */
            uint16_t uVendor = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_VENDOR_ID, 2);
            if (uVendor == 0xffff)
                continue;

            /* only handle PCI-to-PCI bridges */
            uint16_t uDevClass = ich9pciConfigRead(pGlobals, uBus, uDevFn, VBOX_PCI_CLASS_DEVICE, 2);
            if (uDevClass != 0x0604)
                continue;

            Log(("BIOS init prefetchable memory behind bridge: %02x:%02x.%d\n", uBus, uDevFn >> 3, uDevFn & 7));
            /* Save values for the prefetchable dryruns. */
            uint32_t u32MMIOAddressBase = pGlobals->uPciBiosMmio;
            uint64_t u64MMIOAddressBase = pGlobals->uPciBiosMmio64;

            bool fProbe = ich9pciBiosInitBridgePrefetchable(pGlobals, uBus, uDevFn, false /* fUse64Bit */, true /* fDryrun */);
            pGlobals->uPciBiosMmio = u32MMIOAddressBase;
            pGlobals->uPciBiosMmio64 = u64MMIOAddressBase;
            if (fProbe)
            {
                fProbe = ich9pciBiosInitBridgePrefetchable(pGlobals, uBus, uDevFn, true /* fUse64Bit */, true /* fDryrun */);
                pGlobals->uPciBiosMmio = u32MMIOAddressBase;
                pGlobals->uPciBiosMmio64 = u64MMIOAddressBase;
                if (fProbe)
                    LogRel(("PCI: unresolvable prefetchable memory behind bridge %02x:%02x.%d\n", uBus, uDevFn >> 3, uDevFn & 7));
                else
                    ich9pciBiosInitBridgePrefetchable(pGlobals, uBus, uDevFn, true /* fUse64Bit */, false /* fDryrun */);
            }
            else
                ich9pciBiosInitBridgePrefetchable(pGlobals, uBus, uDevFn, false /* fUse64Bit */, false /* fDryrun */);
        }
    }
}

/**
 * Initializes bridges registers used for routing.
 *
 * @returns Max subordinate bus number.
 * @param   pGlobals         Global device instance data used to generate unique bus numbers.
 * @param   pBus             The PCI bus to initialize.
 * @param   uBusPrimary      The primary bus number the bus is connected to.
 */
static uint8_t ich9pciInitBridgeTopology(PICH9PCIGLOBALS pGlobals, PICH9PCIBUS pBus, unsigned uBusPrimary)
{
    PPDMPCIDEV pBridgeDev = &pBus->aPciDev;

    /* Set only if we are not on the root bus, it has no primary bus attached. */
    if (pBus->iBus != 0)
    {
        ich9pciSetByte(pBridgeDev, VBOX_PCI_PRIMARY_BUS, uBusPrimary);
        ich9pciSetByte(pBridgeDev, VBOX_PCI_SECONDARY_BUS, pBus->iBus);
        /* Since the subordinate bus value can only be finalized once we
         * finished recursing through everything behind the bridge, the only
         * solution is temporarily configuring the subordinate to the maximum
         * possible value. This makes sure that the config space accesses work
         * (for our own sloppy emulation it apparently doesn't matter, but
         * this is vital for real PCI bridges/devices in passthrough mode). */
        ich9pciSetByte(pBridgeDev, VBOX_PCI_SUBORDINATE_BUS, 0xff);
    }

    uint8_t uMaxSubNum = pBus->iBus;
    for (uint32_t iBridge = 0; iBridge < pBus->cBridges; iBridge++)
    {
        PPDMPCIDEV pBridge = pBus->papBridgesR3[iBridge];
        AssertMsg(pBridge && pciDevIsPci2PciBridge(pBridge),
                  ("Device is not a PCI bridge but on the list of PCI bridges\n"));
        PICH9PCIBUS pChildBus = PDMINS_2_DATA(pBridge->Int.s.CTX_SUFF(pDevIns), PICH9PCIBUS);
        uint8_t uMaxChildSubBus = ich9pciInitBridgeTopology(pGlobals, pChildBus, pBus->iBus);
        uMaxSubNum = RT_MAX(uMaxSubNum, uMaxChildSubBus);
    }
    if (pBus->iBus != 0)
        ich9pciSetByte(pBridgeDev, VBOX_PCI_SUBORDINATE_BUS, uMaxSubNum);

    /* Make sure that transactions are able to get through the bridge. Not
     * strictly speaking necessary this early (before any device is set up),
     * but on the other hand it can't hurt either. */
    if (pBus->iBus != 0)
        ich9pciSetWord(pBridgeDev, VBOX_PCI_COMMAND,
                         VBOX_PCI_COMMAND_IO
                       | VBOX_PCI_COMMAND_MEMORY
                       | VBOX_PCI_COMMAND_MASTER);

    /* safe, only needs to go to the config space array */
    Log2(("ich9pciInitBridgeTopology: for bus %p: primary=%d secondary=%d subordinate=%d\n",
          pBus,
          PDMPciDevGetByte(pBridgeDev, VBOX_PCI_PRIMARY_BUS),
          PDMPciDevGetByte(pBridgeDev, VBOX_PCI_SECONDARY_BUS),
          PDMPciDevGetByte(pBridgeDev, VBOX_PCI_SUBORDINATE_BUS)
          ));

    return uMaxSubNum;
}


static DECLCALLBACK(int) ich9pciFakePCIBIOS(PPDMDEVINS pDevIns)
{
    PICH9PCIGLOBALS pGlobals   = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);
    PVM             pVM        = PDMDevHlpGetVM(pDevIns);
    uint32_t const  cbBelow4GB = MMR3PhysGetRamSizeBelow4GB(pVM);
    uint64_t const  cbAbove4GB = MMR3PhysGetRamSizeAbove4GB(pVM);

    /** @todo r=klaus this needs to do the same elcr magic as DevPCI.cpp, as the BIOS can't be trusted to do the right thing. Of course it's more difficult than with the old code, as there are bridges to be handled. The interrupt routing needs to be taken into account. Also I highly suspect that the chipset has 8 interrupt lines which we might be able to use for handling things on the root bus better (by treating them as devices on the mainboard). */

    /*
     * Set the start addresses.
     */
    pGlobals->uPciBiosIo     = 0xd000;
    pGlobals->uPciBiosMmio   = cbBelow4GB;
    pGlobals->uPciBiosMmio64 = cbAbove4GB + _4G;
    pGlobals->uBus = 0;

    /* NB: Assume that if MMIO range is enabled, it is below the beginning of the memory hole. */
    if (pGlobals->u64PciConfigMMioAddress)
    {
        AssertRelease(pGlobals->u64PciConfigMMioAddress >= cbBelow4GB);
        pGlobals->uPciBiosMmio = pGlobals->u64PciConfigMMioAddress + pGlobals->u64PciConfigMMioLength;
    }
    Log(("cbBelow4GB: %lX, uPciBiosMmio: %lX, cbAbove4GB: %llX\n", cbBelow4GB, pGlobals->uPciBiosMmio, cbAbove4GB));

    /*
     * Assign bridge topology, for further routing to work.
     */
    PICH9PCIBUS pBus = &pGlobals->aPciBus;
    ich9pciInitBridgeTopology(pGlobals, pBus, 0);

    /*
     * Init all devices on bus 0 (recursing to further buses).
     */
    ich9pciBiosInitAllDevicesOnBus(pGlobals, 0);

    return VINF_SUCCESS;
}


/*
 * Configuration space read callback (PCIDEVICEINT::pfnConfigRead) for
 * connected devices.
 */
static DECLCALLBACK(uint32_t) ich9pciConfigReadDev(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, uint32_t u32Address, unsigned len)
{
    NOREF(pDevIns);
    if ((u32Address + len) > 256 && (u32Address + len) < _4K)
    {
        LogRel(("PCI: %8s/%u: Read from extended register %d fallen back to generic code\n",
                pPciDev->pszNameR3, pPciDev->Int.s.CTX_SUFF(pDevIns)->iInstance, u32Address));
        return 0;
    }

    AssertMsgReturn(u32Address + len <= 256, ("Read after the end of PCI config space\n"),
                    0);
    if (   pciDevIsMsiCapable(pPciDev)
        && (u32Address >= pPciDev->Int.s.u8MsiCapOffset)
        && (u32Address < (unsigned)(pPciDev->Int.s.u8MsiCapOffset + pPciDev->Int.s.u8MsiCapSize))
       )
    {
        return MsiPciConfigRead(pPciDev->Int.s.CTX_SUFF(pBus)->CTX_SUFF(pDevIns), pPciDev, u32Address, len);
    }

    if (   pciDevIsMsixCapable(pPciDev)
        && (u32Address >= pPciDev->Int.s.u8MsixCapOffset)
        && (u32Address < (unsigned)(pPciDev->Int.s.u8MsixCapOffset + pPciDev->Int.s.u8MsixCapSize))
       )
    {
        return MsixPciConfigRead(pPciDev->Int.s.CTX_SUFF(pBus)->CTX_SUFF(pDevIns), pPciDev, u32Address, len);
    }

    AssertMsgReturn(u32Address + len <= 256, ("Read after end of PCI config space\n"),
                    0);
    switch (len)
    {
        case 1:
            /* safe, only needs to go to the config space array */
            return PDMPciDevGetByte(pPciDev,  u32Address);
        case 2:
            /* safe, only needs to go to the config space array */
            return PDMPciDevGetWord(pPciDev,  u32Address);
        case 4:
            /* safe, only needs to go to the config space array */
            return PDMPciDevGetDWord(pPciDev, u32Address);
        default:
            Assert(false);
            return 0;
    }
}


DECLINLINE(void) ich9pciWriteBarByte(PPDMPCIDEV pPciDev, int iRegion, int iOffset, uint8_t u8Val)
{
    PCIIORegion * pRegion = &pPciDev->Int.s.aIORegions[iRegion];
    int64_t iRegionSize = pRegion->size;

    Log3(("ich9pciWriteBarByte: region=%d off=%d val=%x size=%d\n",
         iRegion, iOffset, u8Val, iRegionSize));

    if (iOffset > 3)
        Assert((pRegion->type & PCI_ADDRESS_SPACE_BAR64) != 0);

    /* Check if we're writing to upper part of 64-bit BAR. */
    if (pRegion->type == 0xff)
    {
        ich9pciWriteBarByte(pPciDev, iRegion-1, iOffset+4, u8Val);
        return;
    }

    /* Region doesn't exist */
    if (iRegionSize == 0)
        return;

    uint32_t uAddr = ich9pciGetRegionReg(iRegion) + iOffset;
    /* Region size must be power of two */
    Assert((iRegionSize & (iRegionSize - 1)) == 0);
    uint8_t uMask = ((iRegionSize - 1) >> (iOffset*8) ) & 0xff;

    if (iOffset == 0)
    {
        uMask |= (pRegion->type & PCI_ADDRESS_SPACE_IO) ?
                (1 << 2) - 1 /* 2 lowest bits for IO region */ :
                (1 << 4) - 1 /* 4 lowest bits for memory region, also ROM enable bit for ROM region */;

    }

    /* safe, only needs to go to the config space array */
    uint8_t u8Old = PDMPciDevGetByte(pPciDev, uAddr) & uMask;
    u8Val = (u8Old & uMask) | (u8Val & ~uMask);

    Log3(("ich9pciWriteBarByte: was %x writing %x\n", u8Old, u8Val));

    /* safe, only needs to go to the config space array */
    PDMPciDevSetByte(pPciDev, uAddr, u8Val);
}


/**
 * Configuration space write callback (PCIDEVICEINT::pfnConfigWrite)
 * for connected devices.
 *
 * See paragraph 7.5 of PCI Express specification (p. 349) for
 * definition of registers and their writability policy.
 */
static DECLCALLBACK(void) ich9pciConfigWriteDev(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev,
                                                uint32_t u32Address, uint32_t val, unsigned len)
{
    NOREF(pDevIns);
    Assert(len <= 4);

    if ((u32Address + len) > 256 && (u32Address + len) < _4K)
    {
        LogRel(("PCI: %8s/%u: Write to extended register %d fallen back to generic code\n",
                pPciDev->pszNameR3, pPciDev->Int.s.CTX_SUFF(pDevIns)->iInstance, u32Address));
        return;
    }

    AssertMsgReturnVoid(u32Address + len <= 256, ("Write after end of PCI config space\n"));

    if (   pciDevIsMsiCapable(pPciDev)
        && (u32Address >= pPciDev->Int.s.u8MsiCapOffset)
        && (u32Address < (unsigned)(pPciDev->Int.s.u8MsiCapOffset + pPciDev->Int.s.u8MsiCapSize))
       )
    {
        MsiPciConfigWrite(pPciDev->Int.s.CTX_SUFF(pBus)->CTX_SUFF(pDevIns),
                          pPciDev->Int.s.CTX_SUFF(pBus)->CTX_SUFF(pPciHlp),
                          pPciDev, u32Address, val, len);
        return;
    }

    if (   pciDevIsMsixCapable(pPciDev)
        && (u32Address >= pPciDev->Int.s.u8MsixCapOffset)
        && (u32Address < (unsigned)(pPciDev->Int.s.u8MsixCapOffset + pPciDev->Int.s.u8MsixCapSize))
       )
    {
        MsixPciConfigWrite(pPciDev->Int.s.CTX_SUFF(pBus)->CTX_SUFF(pDevIns),
                           pPciDev->Int.s.CTX_SUFF(pBus)->CTX_SUFF(pPciHlp),
                           pPciDev, u32Address, val, len);
        return;
    }

    uint32_t addr = u32Address;
    bool     fUpdateMappings = false;
    bool     fP2PBridge = false;
    /*bool     fPassthrough = pciDevIsPassthrough(pPciDev);*/
    uint8_t  u8HeaderType = ich9pciGetByte(pPciDev, VBOX_PCI_HEADER_TYPE);

    for (uint32_t i = 0; i < len; i++)
    {
        bool fWritable = false;
        bool fRom = false;
        switch (u8HeaderType)
        {
            case 0x00: /* normal device */
            case 0x80: /* multi-function device */
                switch (addr)
                {
                    /* Read-only registers  */
                    case VBOX_PCI_VENDOR_ID: case VBOX_PCI_VENDOR_ID+1:
                    case VBOX_PCI_DEVICE_ID: case VBOX_PCI_DEVICE_ID+1:
                    case VBOX_PCI_REVISION_ID:
                    case VBOX_PCI_CLASS_PROG:
                    case VBOX_PCI_CLASS_SUB:
                    case VBOX_PCI_CLASS_BASE:
                    case VBOX_PCI_HEADER_TYPE:
                    case VBOX_PCI_SUBSYSTEM_VENDOR_ID: case VBOX_PCI_SUBSYSTEM_VENDOR_ID+1:
                    case VBOX_PCI_SUBSYSTEM_ID: case VBOX_PCI_SUBSYSTEM_ID+1:
                    case VBOX_PCI_ROM_ADDRESS: case VBOX_PCI_ROM_ADDRESS+1: case VBOX_PCI_ROM_ADDRESS+2: case VBOX_PCI_ROM_ADDRESS+3:
                    case VBOX_PCI_CAPABILITY_LIST:
                    case VBOX_PCI_INTERRUPT_PIN:
                        fWritable = false;
                        break;
                    /* Others can be written */
                    default:
                        fWritable = true;
                        break;
                }
                break;
            case 0x01: /* PCI-PCI bridge */
                fP2PBridge = true;
                switch (addr)
                {
                    /* Read-only registers */
                    case VBOX_PCI_VENDOR_ID: case VBOX_PCI_VENDOR_ID+1:
                    case VBOX_PCI_DEVICE_ID: case VBOX_PCI_DEVICE_ID+1:
                    case VBOX_PCI_REVISION_ID:
                    case VBOX_PCI_CLASS_PROG:
                    case VBOX_PCI_CLASS_SUB:
                    case VBOX_PCI_CLASS_BASE:
                    case VBOX_PCI_HEADER_TYPE:
                    case VBOX_PCI_ROM_ADDRESS_BR: case VBOX_PCI_ROM_ADDRESS_BR+1: case VBOX_PCI_ROM_ADDRESS_BR+2: case VBOX_PCI_ROM_ADDRESS_BR+3:
                    case VBOX_PCI_INTERRUPT_PIN:
                        fWritable = false;
                        break;
                    default:
                        fWritable = true;
                        break;
                }
                break;
            default:
                AssertMsgFailed(("Unknown header type %x\n", PDMPciDevGetHeaderType(pPciDev)));
                fWritable = false;
                break;
        }

        uint8_t u8Val = (uint8_t)val;
        switch (addr)
        {
            case VBOX_PCI_COMMAND: /* Command register, bits 0-7. */
                fUpdateMappings = true;
                goto default_case;
            case VBOX_PCI_COMMAND+1: /* Command register, bits 8-15. */
                /* don't change reserved bits (11-15) */
                u8Val &= ~UINT32_C(0xf8);
                fUpdateMappings = true;
                goto default_case;
            case VBOX_PCI_STATUS:  /* Status register, bits 0-7. */
                /* don't change read-only bits => actually all lower bits are read-only */
                u8Val &= ~UINT32_C(0xff);
                /* status register, low part: clear bits by writing a '1' to the corresponding bit */
                pPciDev->abConfig[addr] &= ~u8Val;
                break;
            case VBOX_PCI_STATUS+1:  /* Status register, bits 8-15. */
                /* don't change read-only bits */
                u8Val &= ~UINT32_C(0x06);
                /* status register, high part: clear bits by writing a '1' to the corresponding bit */
                pPciDev->abConfig[addr] &= ~u8Val;
                break;
            case VBOX_PCI_ROM_ADDRESS:    case VBOX_PCI_ROM_ADDRESS   +1: case VBOX_PCI_ROM_ADDRESS   +2: case VBOX_PCI_ROM_ADDRESS   +3:
                fRom = true;
                /* fall thru */
            case VBOX_PCI_BASE_ADDRESS_0: case VBOX_PCI_BASE_ADDRESS_0+1: case VBOX_PCI_BASE_ADDRESS_0+2: case VBOX_PCI_BASE_ADDRESS_0+3:
            case VBOX_PCI_BASE_ADDRESS_1: case VBOX_PCI_BASE_ADDRESS_1+1: case VBOX_PCI_BASE_ADDRESS_1+2: case VBOX_PCI_BASE_ADDRESS_1+3:
            case VBOX_PCI_BASE_ADDRESS_2: case VBOX_PCI_BASE_ADDRESS_2+1: case VBOX_PCI_BASE_ADDRESS_2+2: case VBOX_PCI_BASE_ADDRESS_2+3:
            case VBOX_PCI_BASE_ADDRESS_3: case VBOX_PCI_BASE_ADDRESS_3+1: case VBOX_PCI_BASE_ADDRESS_3+2: case VBOX_PCI_BASE_ADDRESS_3+3:
            case VBOX_PCI_BASE_ADDRESS_4: case VBOX_PCI_BASE_ADDRESS_4+1: case VBOX_PCI_BASE_ADDRESS_4+2: case VBOX_PCI_BASE_ADDRESS_4+3:
            case VBOX_PCI_BASE_ADDRESS_5: case VBOX_PCI_BASE_ADDRESS_5+1: case VBOX_PCI_BASE_ADDRESS_5+2: case VBOX_PCI_BASE_ADDRESS_5+3:
                /* We check that, as same PCI register numbers as BARs may mean different registers for bridges */
                if (!fP2PBridge)
                {
                    int iRegion = fRom ? VBOX_PCI_ROM_SLOT : (addr - VBOX_PCI_BASE_ADDRESS_0) >> 2;
                    ich9pciWriteBarByte(pPciDev, iRegion, addr & 3, u8Val);
                    fUpdateMappings = true;
                    break;
                }
                else if (addr < VBOX_PCI_BASE_ADDRESS_2 || addr > VBOX_PCI_BASE_ADDRESS_5+3)
                {
                    /* PCI bridges have only BAR0, BAR1 and ROM */
                    int iRegion = fRom ? VBOX_PCI_ROM_SLOT : (addr - VBOX_PCI_BASE_ADDRESS_0) >> 2;
                    ich9pciWriteBarByte(pPciDev, iRegion, addr & 0x3, u8Val);
                    fUpdateMappings = true;
                    break;
                }
                else if (   addr == VBOX_PCI_IO_BASE
                         || addr == VBOX_PCI_IO_LIMIT
                         || addr == VBOX_PCI_MEMORY_BASE
                         || addr == VBOX_PCI_MEMORY_LIMIT
                         || addr == VBOX_PCI_PREF_MEMORY_BASE
                         || addr == VBOX_PCI_PREF_MEMORY_LIMIT)
                {
                    /* All bridge address decoders have the low 4 bits
                     * as readonly, and all but the prefetchable ones
                     * have the low 4 bits as 0 (the prefetchable have
                     * it as 1 to show the 64-bit decoder support. */
                    u8Val &= 0xf0;
                    if (   addr == VBOX_PCI_PREF_MEMORY_BASE
                        || addr == VBOX_PCI_PREF_MEMORY_LIMIT)
                        u8Val |= 0x01;
                }
                /* fall thru (bridge config space which isn't a BAR) */
                /* fall thru */
            default:
            default_case:
                if (fWritable)
                    /* safe, only needs to go to the config space array */
                    PDMPciDevSetByte(pPciDev, addr, u8Val);
        }
        addr++;
        val >>= 8;
    }

    if (fUpdateMappings)
        /* if the command/base address register is modified, we must modify the mappings */
        ich9pciUpdateMappings(pPciDev, fP2PBridge);
}


static void printIndent(PCDBGFINFOHLP pHlp, int iIndent)
{
    for (int i = 0; i < iIndent; i++)
    {
        pHlp->pfnPrintf(pHlp, "    ");
    }
}

static void ich9pciBusInfo(PICH9PCIBUS pBus, PCDBGFINFOHLP pHlp, int iIndent, bool fRegisters)
{
    /* This has to use the callbacks for accuracy reasons. Otherwise it can get
     * confusing in the passthrough case or when the callbacks for some device
     * are doing something non-trivial (like implementing an indirect
     * passthrough approach), because then the abConfig array is an imprecise
     * cache needed for efficiency (so that certain reads can be done from
     * R0/RC), but far from authoritative or what the guest would see. */

    for (uint32_t iDev = 0; iDev < RT_ELEMENTS(pBus->apDevices); iDev++)
    {
        PPDMPCIDEV pPciDev = pBus->apDevices[iDev];
        if (pPciDev != NULL)
        {
            printIndent(pHlp, iIndent);

            /*
             * For passthrough devices MSI/MSI-X mostly reflects the way interrupts delivered to the guest,
             * as host driver handles real devices interrupts.
             */
            pHlp->pfnPrintf(pHlp, "%02x:%02x.%d %s%s: %04x-%04x",
                            pBus->iBus, (iDev >> 3) & 0xff, iDev & 0x7,
                            pPciDev->pszNameR3,
                            pciDevIsPassthrough(pPciDev) ? " (PASSTHROUGH)" : "",
                            ich9pciGetWord(pPciDev, VBOX_PCI_VENDOR_ID), ich9pciGetWord(pPciDev, VBOX_PCI_DEVICE_ID)
                            );
            if (ich9pciGetByte(pPciDev, VBOX_PCI_INTERRUPT_PIN) != 0)
            {
                pHlp->pfnPrintf(pHlp, " IRQ%d", ich9pciGetByte(pPciDev, VBOX_PCI_INTERRUPT_LINE));
                pHlp->pfnPrintf(pHlp, " (INTA#->IRQ%d)", 0x10 + ich9pciSlot2ApicIrq(iDev >> 3, 0));
            }
            pHlp->pfnPrintf(pHlp, "\n");

            if (pciDevIsMsiCapable(pPciDev) || pciDevIsMsixCapable(pPciDev))
            {
                printIndent(pHlp, iIndent + 2);

                if (pciDevIsMsiCapable(pPciDev))
                    pHlp->pfnPrintf(pHlp, "MSI: %s ", MsiIsEnabled(pPciDev) ? "on" : "off");

                if (pciDevIsMsixCapable(pPciDev))
                    pHlp->pfnPrintf(pHlp, "MSI-X: %s ", MsixIsEnabled(pPciDev) ? "on" : "off");

                pHlp->pfnPrintf(pHlp, "\n");
            }

            for (int iRegion = 0; iRegion < PCI_NUM_REGIONS; iRegion++)
            {
                PCIIORegion* pRegion = &pPciDev->Int.s.aIORegions[iRegion];
                uint64_t  iRegionSize = pRegion->size;

                if (iRegionSize == 0)
                    continue;

                uint32_t u32Addr = ich9pciGetDWord(pPciDev, ich9pciGetRegionReg(iRegion));
                const char * pszDesc;
                char szDescBuf[128];

                bool f64Bit =    (pRegion->type & ((uint8_t)(PCI_ADDRESS_SPACE_BAR64 | PCI_ADDRESS_SPACE_IO)))
                              == PCI_ADDRESS_SPACE_BAR64;
                if (pRegion->type & PCI_ADDRESS_SPACE_IO)
                {
                    pszDesc = "IO";
                    u32Addr &= ~0x3;
                }
                else
                {
                    RTStrPrintf(szDescBuf, sizeof(szDescBuf), "MMIO%s%s",
                                f64Bit ? "64" : "32",
                                (pRegion->type & PCI_ADDRESS_SPACE_MEM_PREFETCH) ? " PREFETCH" : "");
                    pszDesc = szDescBuf;
                    u32Addr &= ~0xf;
                }

                printIndent(pHlp, iIndent + 2);
                pHlp->pfnPrintf(pHlp, "%s region #%d: ",pszDesc, iRegion);
                if (f64Bit)
                {
                    uint32_t u32High = ich9pciGetDWord(pPciDev, ich9pciGetRegionReg(iRegion+1));
                    uint64_t u64Addr = RT_MAKE_U64(u32Addr, u32High);
                    pHlp->pfnPrintf(pHlp, "%RX64..%RX64\n", u64Addr, u64Addr+iRegionSize-1);
                    iRegion++;
                }
                else
                    pHlp->pfnPrintf(pHlp, "%x..%x\n", u32Addr, u32Addr+iRegionSize-1);
            }

            printIndent(pHlp, iIndent + 2);
            uint16_t iCmd = ich9pciGetWord(pPciDev, VBOX_PCI_COMMAND);
            uint16_t iStatus = ich9pciGetWord(pPciDev, VBOX_PCI_STATUS);
            pHlp->pfnPrintf(pHlp, "Command: %04x, Status: %04x\n",
                            iCmd, iStatus);
            printIndent(pHlp, iIndent + 2);
            pHlp->pfnPrintf(pHlp, "Bus master: %s\n",
                            iCmd & VBOX_PCI_COMMAND_MASTER ? "Yes" : "No");
            if (iCmd != PDMPciDevGetCommand(pPciDev))
            {
                printIndent(pHlp, iIndent + 2);
                pHlp->pfnPrintf(pHlp, "CACHE INCONSISTENCY: Command: %04x\n", PDMPciDevGetCommand(pPciDev));
            }

            if (fRegisters)
            {
                printIndent(pHlp, iIndent + 2);
                pHlp->pfnPrintf(pHlp, "PCI registers:\n");
                for (int iReg = 0; iReg < 0x100; )
                {
                    int iPerLine = 0x10;
                    Assert (0x100 % iPerLine == 0);
                    printIndent(pHlp, iIndent + 3);

                    while (iPerLine-- > 0)
                    {
                        pHlp->pfnPrintf(pHlp, "%02x ", ich9pciGetByte(pPciDev, iReg++));
                    }
                    pHlp->pfnPrintf(pHlp, "\n");
                }
            }
        }
    }

    if (pBus->cBridges > 0)
    {
        printIndent(pHlp, iIndent);
        pHlp->pfnPrintf(pHlp, "Registered %d bridges, subordinate buses info follows\n", pBus->cBridges);
        for (uint32_t iBridge = 0; iBridge < pBus->cBridges; iBridge++)
        {
            PICH9PCIBUS pBusSub = PDMINS_2_DATA(pBus->papBridgesR3[iBridge]->Int.s.CTX_SUFF(pDevIns), PICH9PCIBUS);
            uint8_t uPrimary = ich9pciGetByte(&pBusSub->aPciDev, VBOX_PCI_PRIMARY_BUS);
            uint8_t uSecondary = ich9pciGetByte(&pBusSub->aPciDev, VBOX_PCI_SECONDARY_BUS);
            uint8_t uSubordinate = ich9pciGetByte(&pBusSub->aPciDev, VBOX_PCI_SUBORDINATE_BUS);
            printIndent(pHlp, iIndent);
            pHlp->pfnPrintf(pHlp, "%02x:%02x.%d: bridge topology: primary=%d secondary=%d subordinate=%d\n",
                            uPrimary, pBusSub->aPciDev.uDevFn >> 3, pBusSub->aPciDev.uDevFn & 7,
                            uPrimary, uSecondary, uSubordinate);
            if (   uPrimary != PDMPciDevGetByte(&pBusSub->aPciDev, VBOX_PCI_PRIMARY_BUS)
                || uSecondary != PDMPciDevGetByte(&pBusSub->aPciDev, VBOX_PCI_SECONDARY_BUS)
                || uSubordinate != PDMPciDevGetByte(&pBusSub->aPciDev, VBOX_PCI_SUBORDINATE_BUS))
            {
                printIndent(pHlp, iIndent);
                pHlp->pfnPrintf(pHlp, "CACHE INCONSISTENCY: primary=%d secondary=%d subordinate=%d\n",
                                PDMPciDevGetByte(&pBusSub->aPciDev, VBOX_PCI_PRIMARY_BUS),
                                PDMPciDevGetByte(&pBusSub->aPciDev, VBOX_PCI_SECONDARY_BUS),
                                PDMPciDevGetByte(&pBusSub->aPciDev, VBOX_PCI_SUBORDINATE_BUS));
            }
            printIndent(pHlp, iIndent);
            pHlp->pfnPrintf(pHlp, "behind bridge: ");
            uint8_t uIoBase  = ich9pciGetByte(&pBusSub->aPciDev, VBOX_PCI_IO_BASE);
            uint8_t uIoLimit = ich9pciGetByte(&pBusSub->aPciDev, VBOX_PCI_IO_LIMIT);
            pHlp->pfnPrintf(pHlp, "I/O %#06x..%#06x",
                            (uIoBase & 0xf0) << 8,
                            (uIoLimit & 0xf0) << 8 | 0xfff);
            if (uIoBase > uIoLimit)
                pHlp->pfnPrintf(pHlp, " (IGNORED)");
            pHlp->pfnPrintf(pHlp, "\n");
            printIndent(pHlp, iIndent);
            pHlp->pfnPrintf(pHlp, "behind bridge: ");
            uint32_t uMemoryBase  = ich9pciGetWord(&pBusSub->aPciDev, VBOX_PCI_MEMORY_BASE);
            uint32_t uMemoryLimit = ich9pciGetWord(&pBusSub->aPciDev, VBOX_PCI_MEMORY_LIMIT);
            pHlp->pfnPrintf(pHlp, "memory %#010x..%#010x",
                            (uMemoryBase & 0xfff0) << 16,
                            (uMemoryLimit & 0xfff0) << 16 | 0xfffff);
            if (uMemoryBase > uMemoryLimit)
                pHlp->pfnPrintf(pHlp, " (IGNORED)");
            pHlp->pfnPrintf(pHlp, "\n");
            printIndent(pHlp, iIndent);
            pHlp->pfnPrintf(pHlp, "behind bridge: ");
            uint32_t uPrefMemoryRegBase  = ich9pciGetWord(&pBusSub->aPciDev, VBOX_PCI_PREF_MEMORY_BASE);
            uint32_t uPrefMemoryRegLimit = ich9pciGetWord(&pBusSub->aPciDev, VBOX_PCI_PREF_MEMORY_LIMIT);
            uint64_t uPrefMemoryBase = (uPrefMemoryRegBase & 0xfff0) << 16;
            uint64_t uPrefMemoryLimit = (uPrefMemoryRegLimit & 0xfff0) << 16 | 0xfffff;
            if (   (uPrefMemoryRegBase & 0xf) == 1
                && (uPrefMemoryRegLimit & 0xf) == 1)
            {
                uPrefMemoryBase |= (uint64_t)ich9pciGetDWord(&pBusSub->aPciDev, VBOX_PCI_PREF_BASE_UPPER32) << 32;
                uPrefMemoryLimit |= (uint64_t)ich9pciGetDWord(&pBusSub->aPciDev, VBOX_PCI_PREF_LIMIT_UPPER32) << 32;
                pHlp->pfnPrintf(pHlp, "64-bit ");
            }
            else
                pHlp->pfnPrintf(pHlp, "32-bit ");
            pHlp->pfnPrintf(pHlp, "prefetch memory %#018llx..%#018llx", uPrefMemoryBase, uPrefMemoryLimit);
            if (uPrefMemoryBase > uPrefMemoryLimit)
                pHlp->pfnPrintf(pHlp, " (IGNORED)");
            pHlp->pfnPrintf(pHlp, "\n");
            ich9pciBusInfo(pBusSub, pHlp, iIndent + 1, fRegisters);
        }
    }
}

/**
 * Info handler, device version.
 *
 * @param   pDevIns     Device instance which registered the info.
 * @param   pHlp        Callback functions for doing output.
 * @param   pszArgs     Argument string. Optional and specific to the handler.
 */
static DECLCALLBACK(void) ich9pciInfo(PPDMDEVINS pDevIns, PCDBGFINFOHLP pHlp, const char *pszArgs)
{
    PICH9PCIBUS pBus = DEVINS_2_PCIBUS(pDevIns);

    if (pszArgs == NULL || !*pszArgs || !strcmp(pszArgs, "basic"))
        ich9pciBusInfo(pBus, pHlp, 0, false /*fRegisters*/);
    else if (!strcmp(pszArgs, "verbose"))
        ich9pciBusInfo(pBus, pHlp, 0, true /*fRegisters*/);
    else
        pHlp->pfnPrintf(pHlp, "Invalid argument. Recognized arguments are 'basic', 'verbose'.\n");
}


static DECLCALLBACK(int) ich9pciConstruct(PPDMDEVINS pDevIns, int iInstance, PCFGMNODE  pCfg)
{
    RT_NOREF1(iInstance);
    Assert(iInstance == 0);
    PDMDEV_CHECK_VERSIONS_RETURN(pDevIns);

    /*
     * Validate and read configuration.
     */
    if (!CFGMR3AreValuesValid(pCfg,
                              "IOAPIC\0"
                              "GCEnabled\0"
                              "R0Enabled\0"
                              "McfgBase\0"
                              "McfgLength\0"
                              ))
        return VERR_PDM_DEVINS_UNKNOWN_CFG_VALUES;

    /* query whether we got an IOAPIC */
    bool fUseIoApic;
    int rc = CFGMR3QueryBoolDef(pCfg, "IOAPIC", &fUseIoApic, false);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to query boolean value \"IOAPIC\""));

    /* check if RC code is enabled. */
    bool fGCEnabled;
    rc = CFGMR3QueryBoolDef(pCfg, "GCEnabled", &fGCEnabled, true);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to query boolean value \"GCEnabled\""));
    /* check if R0 code is enabled. */
    bool fR0Enabled;
    rc = CFGMR3QueryBoolDef(pCfg, "R0Enabled", &fR0Enabled, true);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to query boolean value \"R0Enabled\""));

    Log(("PCI: fUseIoApic=%RTbool fGCEnabled=%RTbool fR0Enabled=%RTbool\n", fUseIoApic, fGCEnabled, fR0Enabled));

    /*
     * Init data.
     */
    PICH9PCIGLOBALS pGlobals = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);
    PICH9PCIBUS     pBus     = &pGlobals->aPciBus;
    /* Zero out everything */
    memset(pGlobals, 0, sizeof(*pGlobals));
    /* And fill values */
    if (!fUseIoApic)
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Must use IO-APIC with ICH9 chipset"));
    rc = CFGMR3QueryU64Def(pCfg, "McfgBase", &pGlobals->u64PciConfigMMioAddress, 0);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to read \"McfgBase\""));
    rc = CFGMR3QueryU64Def(pCfg, "McfgLength", &pGlobals->u64PciConfigMMioLength, 0);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to read \"McfgLength\""));

    pGlobals->pDevInsR3 = pDevIns;
    pGlobals->pDevInsR0 = PDMDEVINS_2_R0PTR(pDevIns);
    pGlobals->pDevInsRC = PDMDEVINS_2_RCPTR(pDevIns);

    pGlobals->aPciBus.pDevInsR3 = pDevIns;
    pGlobals->aPciBus.pDevInsR0 = PDMDEVINS_2_R0PTR(pDevIns);
    pGlobals->aPciBus.pDevInsRC = PDMDEVINS_2_RCPTR(pDevIns);
    pGlobals->aPciBus.papBridgesR3 = (PPDMPCIDEV *)PDMDevHlpMMHeapAllocZ(pDevIns, sizeof(PPDMPCIDEV) * RT_ELEMENTS(pGlobals->aPciBus.apDevices));

    /*
     * Register bus
     */
    PDMPCIBUSREG PciBusReg;
    PciBusReg.u32Version              = PDM_PCIBUSREG_VERSION;
    PciBusReg.pfnRegisterR3           = pciR3MergedRegister;
    PciBusReg.pfnRegisterMsiR3        = ich9pciRegisterMsi;
    PciBusReg.pfnIORegionRegisterR3   = ich9pciIORegionRegister;
    PciBusReg.pfnSetConfigCallbacksR3 = ich9pciSetConfigCallbacks;
    PciBusReg.pfnSetIrqR3             = ich9pciSetIrq;
    PciBusReg.pfnFakePCIBIOSR3        = ich9pciFakePCIBIOS;
    PciBusReg.pszSetIrqRC             = fGCEnabled ? "ich9pciSetIrq" : NULL;
    PciBusReg.pszSetIrqR0             = fR0Enabled ? "ich9pciSetIrq" : NULL;
    rc = PDMDevHlpPCIBusRegister(pDevIns, &PciBusReg, &pBus->pPciHlpR3);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Failed to register ourselves as a PCI Bus"));
    if (pBus->pPciHlpR3->u32Version != PDM_PCIHLPR3_VERSION)
        return PDMDevHlpVMSetError(pDevIns, VERR_VERSION_MISMATCH, RT_SRC_POS,
                                   N_("PCI helper version mismatch; got %#x expected %#x"),
                                   pBus->pPciHlpR3->u32Version, PDM_PCIHLPR3_VERSION);

    pBus->pPciHlpRC = pBus->pPciHlpR3->pfnGetRCHelpers(pDevIns);
    pBus->pPciHlpR0 = pBus->pPciHlpR3->pfnGetR0Helpers(pDevIns);

    /*
     * Fill in PCI configs and add them to the bus.
     */
    /** @todo Disabled for now because this causes error messages with Linux guests.
     *         The guest loads the x38_edac device which tries to map a memory region
     *         using an address given at place 0x48 - 0x4f in the PCI config space.
     *         This fails. because we don't register such a region.
     */
#if 0
    /* Host bridge device */
    PDMPciDevSetVendorId(  &pBus->aPciDev, 0x8086); /* Intel */
    PDMPciDevSetDeviceId(  &pBus->aPciDev, 0x29e0); /* Desktop */
    PDMPciDevSetRevisionId(&pBus->aPciDev,   0x01); /* rev. 01 */
    PDMPciDevSetClassBase( &pBus->aPciDev,   0x06); /* bridge */
    PDMPciDevSetClassSub(  &pBus->aPciDev,   0x00); /* Host/PCI bridge */
    PDMPciDevSetClassProg( &pBus->aPciDev,   0x00); /* Host/PCI bridge */
    PDMPciDevSetHeaderType(&pBus->aPciDev,   0x00); /* bridge */
    PDMPciDevSetWord(&pBus->aPciDev,  VBOX_PCI_SEC_STATUS, 0x0280);  /* secondary status */

    pBus->aPciDev.pDevIns               = pDevIns;
    /* We register Host<->PCI controller on the bus */
    ich9pciRegisterInternal(pBus, 0, &pBus->aPciDev, "dram");
#endif

    /*
     * Register I/O ports and save state.
     */
    rc = PDMDevHlpIOPortRegister(pDevIns, 0x0cf8, 1, NULL, ich9pciIOPortAddressWrite, ich9pciIOPortAddressRead, NULL, NULL, "ICH9 (PCI)");
    if (RT_FAILURE(rc))
        return rc;
    rc = PDMDevHlpIOPortRegister(pDevIns, 0x0cfc, 4, NULL, ich9pciIOPortDataWrite, ich9pciIOPortDataRead, NULL, NULL, "ICH9 (PCI)");
    if (RT_FAILURE(rc))
        return rc;
    if (fGCEnabled)
    {
        rc = PDMDevHlpIOPortRegisterRC(pDevIns, 0x0cf8, 1, NIL_RTGCPTR, "ich9pciIOPortAddressWrite", "ich9pciIOPortAddressRead", NULL, NULL, "ICH9 (PCI)");
        if (RT_FAILURE(rc))
            return rc;
        rc = PDMDevHlpIOPortRegisterRC(pDevIns, 0x0cfc, 4, NIL_RTGCPTR, "ich9pciIOPortDataWrite", "ich9pciIOPortDataRead", NULL, NULL, "ICH9 (PCI)");
        if (RT_FAILURE(rc))
            return rc;
    }
    if (fR0Enabled)
    {
        rc = PDMDevHlpIOPortRegisterR0(pDevIns, 0x0cf8, 1, NIL_RTR0PTR, "ich9pciIOPortAddressWrite", "ich9pciIOPortAddressRead", NULL, NULL, "ICH9 (PCI)");
        if (RT_FAILURE(rc))
            return rc;
        rc = PDMDevHlpIOPortRegisterR0(pDevIns, 0x0cfc, 4, NIL_RTR0PTR, "ich9pciIOPortDataWrite", "ich9pciIOPortDataRead", NULL, NULL, "ICH9 (PCI)");
        if (RT_FAILURE(rc))
            return rc;
    }

    if (pGlobals->u64PciConfigMMioAddress != 0)
    {
        rc = PDMDevHlpMMIORegister(pDevIns, pGlobals->u64PciConfigMMioAddress, pGlobals->u64PciConfigMMioLength, NULL /*pvUser*/,
                                   IOMMMIO_FLAGS_READ_PASSTHRU | IOMMMIO_FLAGS_WRITE_PASSTHRU,
                                   ich9pciMcfgMMIOWrite, ich9pciMcfgMMIORead, "MCFG ranges");
        AssertMsgRCReturn(rc, ("rc=%Rrc %#llx/%#llx\n", rc,  pGlobals->u64PciConfigMMioAddress, pGlobals->u64PciConfigMMioLength), rc);

        if (fGCEnabled)
        {
            rc = PDMDevHlpMMIORegisterRC(pDevIns, pGlobals->u64PciConfigMMioAddress, pGlobals->u64PciConfigMMioLength,
                                         NIL_RTRCPTR /*pvUser*/, "ich9pciMcfgMMIOWrite", "ich9pciMcfgMMIORead");
            AssertRCReturn(rc, rc);
        }


        if (fR0Enabled)
        {
            rc = PDMDevHlpMMIORegisterR0(pDevIns, pGlobals->u64PciConfigMMioAddress, pGlobals->u64PciConfigMMioLength,
                                         NIL_RTR0PTR /*pvUser*/, "ich9pciMcfgMMIOWrite", "ich9pciMcfgMMIORead");
            AssertRCReturn(rc, rc);
        }
    }

    rc = PDMDevHlpSSMRegisterEx(pDevIns, VBOX_ICH9PCI_SAVED_STATE_VERSION_CURRENT,
                                sizeof(*pBus) + 16*128, "pgm",
                                NULL, NULL, NULL,
                                NULL, ich9pciR3SaveExec, NULL,
                                NULL, ich9pciR3LoadExec, NULL);
    if (RT_FAILURE(rc))
        return rc;


    /** @todo other chipset devices shall be registered too */

    PDMDevHlpDBGFInfoRegister(pDevIns, "pci", "Display PCI bus status. Recognizes 'basic' or 'verbose' "
                                              "as arguments, defaults to 'basic'.", ich9pciInfo);

    return VINF_SUCCESS;
}

/**
 * @interface_method_impl{PDMDEVREG,pfnDestruct}
 */
static DECLCALLBACK(int) ich9pciDestruct(PPDMDEVINS pDevIns)
{
    PICH9PCIGLOBALS pGlobals = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);
    PICH9PCIBUS     pBus     = &pGlobals->aPciBus;
    if (pBus->papBridgesR3)
    {
        PDMDevHlpMMHeapFree(pDevIns, pBus->papBridgesR3);
        pBus->papBridgesR3 = NULL;
    }
    return VINF_SUCCESS;
}

static void ich9pciResetDevice(PPDMPCIDEV pDev)
{
    /* Clear regions */
    for (int iRegion = 0; iRegion < PCI_NUM_REGIONS; iRegion++)
    {
        PCIIORegion* pRegion = &pDev->Int.s.aIORegions[iRegion];
        if (pRegion->size == 0)
            continue;
        bool const f64Bit =    (pRegion->type & ((uint8_t)(PCI_ADDRESS_SPACE_BAR64 | PCI_ADDRESS_SPACE_IO)))
                            == PCI_ADDRESS_SPACE_BAR64;

        ich9pciUnmapRegion(pDev, iRegion);

        if (f64Bit)
            iRegion++;
    }

    if (pciDevIsPassthrough(pDev))
    {
        // no reset handler - we can do what we need in PDM reset handler
        /// @todo is it correct?
    }
    else
    {
        ich9pciSetWord(pDev, VBOX_PCI_COMMAND,
                         ich9pciGetWord(pDev, VBOX_PCI_COMMAND)
                       & ~(VBOX_PCI_COMMAND_IO | VBOX_PCI_COMMAND_MEMORY |
                           VBOX_PCI_COMMAND_MASTER | VBOX_PCI_COMMAND_SPECIAL |
                           VBOX_PCI_COMMAND_PARITY | VBOX_PCI_COMMAND_SERR |
                           VBOX_PCI_COMMAND_FAST_BACK | VBOX_PCI_COMMAND_INTX_DISABLE));

        /* Bridge device reset handlers processed later */
        if (!pciDevIsPci2PciBridge(pDev))
        {
            ich9pciSetByte(pDev, VBOX_PCI_CACHE_LINE_SIZE, 0x0);
            ich9pciSetByte(pDev, VBOX_PCI_INTERRUPT_LINE, 0x0);
        }

        /* Reset MSI message control. */
        if (pciDevIsMsiCapable(pDev))
        {
            ich9pciSetWord(pDev, pDev->Int.s.u8MsiCapOffset + VBOX_MSI_CAP_MESSAGE_CONTROL,
                           ich9pciGetWord(pDev, pDev->Int.s.u8MsiCapOffset + VBOX_MSI_CAP_MESSAGE_CONTROL) & 0xff8e);
        }

        /* Reset MSI-X message control. */
        if (pciDevIsMsixCapable(pDev))
        {
            ich9pciSetWord(pDev, pDev->Int.s.u8MsixCapOffset + VBOX_MSIX_CAP_MESSAGE_CONTROL,
                           ich9pciGetWord(pDev, pDev->Int.s.u8MsixCapOffset + VBOX_MSIX_CAP_MESSAGE_CONTROL) & 0x3fff);
        }
    }
}


/**
 * Recursive worker for ich9pciReset.
 *
 * @param   pDevIns     ICH9 bridge (must be PCI-to-PCI, not root) instance.
 */
static void ich9pciResetBridge(PPDMDEVINS pDevIns)
{
    PICH9PCIBUS pBus = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);

    /* PCI-specific reset for each device. */
    for (uint32_t i = 0; i < RT_ELEMENTS(pBus->apDevices); i++)
    {
        if (pBus->apDevices[i])
            ich9pciResetDevice(pBus->apDevices[i]);
    }

    for (uint32_t iBridge = 0; iBridge < pBus->cBridges; iBridge++)
    {
        if (pBus->papBridgesR3[iBridge])
            ich9pciResetBridge(pBus->papBridgesR3[iBridge]->Int.s.CTX_SUFF(pDevIns));
    }

    /* Reset topology config. Last thing to do, otherwise the secondary and
     * subordinate are instantly unreachable. */
    ich9pciSetByte(&pBus->aPciDev, VBOX_PCI_PRIMARY_BUS, 0);
    ich9pciSetByte(&pBus->aPciDev, VBOX_PCI_SECONDARY_BUS, 0);
    ich9pciSetByte(&pBus->aPciDev, VBOX_PCI_SUBORDINATE_BUS, 0);
    /* Not resetting the address decoders of the bridge to 0, since the
     * PCI-to-PCI Bridge spec says that there is no default value. */
}

/**
 * @interface_method_impl{PDMDEVREG,pfnReset}
 */
static DECLCALLBACK(void) ich9pciReset(PPDMDEVINS pDevIns)
{
    PICH9PCIGLOBALS pGlobals = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);
    PICH9PCIBUS     pBus     = &pGlobals->aPciBus;

    /* PCI-specific reset for each device. */
    for (uint32_t i = 0; i < RT_ELEMENTS(pBus->apDevices); i++)
    {
        if (pBus->apDevices[i])
            ich9pciResetDevice(pBus->apDevices[i]);
    }

    for (uint32_t iBridge = 0; iBridge < pBus->cBridges; iBridge++)
    {
        if (pBus->papBridgesR3[iBridge])
            ich9pciResetBridge(pBus->papBridgesR3[iBridge]->Int.s.CTX_SUFF(pDevIns));
    }
}

static void ich9pciRelocateDevice(PPDMPCIDEV pDev, RTGCINTPTR offDelta)
{
    if (pDev)
    {
        pDev->Int.s.pBusRC += offDelta;
        if (pDev->Int.s.pMsixPageRC)
            pDev->Int.s.pMsixPageRC += offDelta;
    }
}

/**
 * @copydoc FNPDMDEVRELOCATE
 */
static DECLCALLBACK(void) ich9pciRelocate(PPDMDEVINS pDevIns, RTGCINTPTR offDelta)
{
    PICH9PCIGLOBALS pGlobals = PDMINS_2_DATA(pDevIns, PICH9PCIGLOBALS);
    PICH9PCIBUS     pBus     = &pGlobals->aPciBus;
    pGlobals->pDevInsRC = PDMDEVINS_2_RCPTR(pDevIns);

    pBus->pPciHlpRC = pBus->pPciHlpR3->pfnGetRCHelpers(pDevIns);
    pBus->pDevInsRC = PDMDEVINS_2_RCPTR(pDevIns);

    /* Relocate RC pointers for the attached pci devices. */
    for (uint32_t i = 0; i < RT_ELEMENTS(pBus->apDevices); i++)
        ich9pciRelocateDevice(pBus->apDevices[i], offDelta);

}

/**
 * @interface_method_impl{PDMIBASE,pfnQueryInterface}
 */
static DECLCALLBACK(void *) ich9pcibridgeQueryInterface(PPDMIBASE pInterface, const char *pszIID)
{
    PPDMDEVINS pDevIns = RT_FROM_MEMBER(pInterface, PDMDEVINS, IBase);
    PDMIBASE_RETURN_INTERFACE(pszIID, PDMIBASE, &pDevIns->IBase);
    /* Special access to the PDMPCIDEV structure of a ich9pcibridge instance. */
    PICH9PCIBUS pBus = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);
    PDMIBASE_RETURN_INTERFACE(pszIID, PDMIICH9BRIDGEPDMPCIDEV, &pBus->aPciDev);
    return NULL;
}


/**
 * @interface_method_impl{PDMDEVREG,pfnConstruct}
 */
static DECLCALLBACK(int)   ich9pcibridgeConstruct(PPDMDEVINS pDevIns,
                                                  int        iInstance,
                                                  PCFGMNODE  pCfg)
{
    PDMDEV_CHECK_VERSIONS_RETURN(pDevIns);

    /*
     * Validate and read configuration.
     */
    if (!CFGMR3AreValuesValid(pCfg, "GCEnabled\0" "R0Enabled\0" "ExpressEnabled\0"))
        return VERR_PDM_DEVINS_UNKNOWN_CFG_VALUES;

    /* check if RC code is enabled. */
    bool fGCEnabled;
    int rc = CFGMR3QueryBoolDef(pCfg, "GCEnabled", &fGCEnabled, true);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to query boolean value \"GCEnabled\""));

    /* check if R0 code is enabled. */
    bool fR0Enabled;
    rc = CFGMR3QueryBoolDef(pCfg, "R0Enabled", &fR0Enabled, true);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to query boolean value \"R0Enabled\""));
    Log(("PCI: fGCEnabled=%RTbool fR0Enabled=%RTbool\n", fGCEnabled, fR0Enabled));

    /* check if we're supposed to implement a PCIe bridge. */
    bool fExpress;
    rc = CFGMR3QueryBoolDef(pCfg, "ExpressEnabled", &fExpress, false);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Configuration error: Failed to query boolean value \"ExpressEnabled\""));

    pDevIns->IBase.pfnQueryInterface = ich9pcibridgeQueryInterface;

    /*
     * Init data and register the PCI bus.
     */
    PICH9PCIBUS pBus = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);
    pBus->pDevInsR3 = pDevIns;
    pBus->pDevInsR0 = PDMDEVINS_2_R0PTR(pDevIns);
    pBus->pDevInsRC = PDMDEVINS_2_RCPTR(pDevIns);
    /** @todo r=klaus figure out how to extend this to allow PCIe config space
     * extension, which increases the config space frorm 256 bytes to 4K. */
    pBus->papBridgesR3 = (PPDMPCIDEV *)PDMDevHlpMMHeapAllocZ(pDevIns, sizeof(PPDMPCIDEV) * RT_ELEMENTS(pBus->apDevices));

    PDMPCIBUSREG PciBusReg;
    PciBusReg.u32Version              = PDM_PCIBUSREG_VERSION;
    PciBusReg.pfnRegisterR3           = pcibridgeR3MergedRegisterDevice;
    PciBusReg.pfnRegisterMsiR3        = ich9pciRegisterMsi;
    PciBusReg.pfnIORegionRegisterR3   = ich9pciIORegionRegister;
    PciBusReg.pfnSetConfigCallbacksR3 = ich9pciSetConfigCallbacks;
    PciBusReg.pfnSetIrqR3             = ich9pcibridgeSetIrq;
    PciBusReg.pfnFakePCIBIOSR3        = NULL; /* Only needed for the first bus. */
    PciBusReg.pszSetIrqRC             = fGCEnabled ? "ich9pcibridgeSetIrq" : NULL;
    PciBusReg.pszSetIrqR0             = fR0Enabled ? "ich9pcibridgeSetIrq" : NULL;
    rc = PDMDevHlpPCIBusRegister(pDevIns, &PciBusReg, &pBus->pPciHlpR3);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("Failed to register ourselves as a PCI Bus"));
    if (pBus->pPciHlpR3->u32Version != PDM_PCIHLPR3_VERSION)
        return PDMDevHlpVMSetError(pDevIns, VERR_VERSION_MISMATCH, RT_SRC_POS,
                                   N_("PCI helper version mismatch; got %#x expected %#x"),
                                   pBus->pPciHlpR3->u32Version, PDM_PCIHLPR3_VERSION);

    pBus->pPciHlpRC = pBus->pPciHlpR3->pfnGetRCHelpers(pDevIns);
    pBus->pPciHlpR0 = pBus->pPciHlpR3->pfnGetR0Helpers(pDevIns);

    /* Disable default device locking. */
    rc = PDMDevHlpSetDeviceCritSect(pDevIns, PDMDevHlpCritSectGetNop(pDevIns));
    AssertRCReturn(rc, rc);

    /*
     * Fill in PCI configs and add them to the bus.
     */
    PDMPciDevSetVendorId(  &pBus->aPciDev, 0x8086); /* Intel */
    if (fExpress)
    {
        PDMPciDevSetDeviceId(&pBus->aPciDev, 0x29e1); /* 82X38/X48 Express Host-Primary PCI Express Bridge. */
        PDMPciDevSetRevisionId(&pBus->aPciDev, 0x01);
    }
    else
    {
        PDMPciDevSetDeviceId(  &pBus->aPciDev, 0x2448); /* 82801 Mobile PCI bridge. */
        PDMPciDevSetRevisionId(&pBus->aPciDev,   0xf2);
    }
    PDMPciDevSetClassSub(  &pBus->aPciDev,   0x04); /* pci2pci */
    PDMPciDevSetClassBase( &pBus->aPciDev,   0x06); /* PCI_bridge */
    if (fExpress)
        PDMPciDevSetClassProg(&pBus->aPciDev, 0x00); /* Normal decoding. */
    else
        PDMPciDevSetClassProg( &pBus->aPciDev,   0x01); /* Supports subtractive decoding. */
    PDMPciDevSetHeaderType(&pBus->aPciDev,   0x01); /* Single function device which adheres to the PCI-to-PCI bridge spec. */
    if (fExpress)
    {
        PDMPciDevSetCommand(&pBus->aPciDev, VBOX_PCI_COMMAND_SERR);
        PDMPciDevSetStatus(&pBus->aPciDev, VBOX_PCI_STATUS_CAP_LIST); /* Has capabilities. */
        PDMPciDevSetByte(&pBus->aPciDev, VBOX_PCI_CACHE_LINE_SIZE, 8); /* 32 bytes */
        /* PCI Express */
        PDMPciDevSetByte(&pBus->aPciDev, 0xa0 + 0, VBOX_PCI_CAP_ID_EXP); /* PCI_Express */
        PDMPciDevSetByte(&pBus->aPciDev, 0xa0 + 1, 0); /* next */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 2,
                        /* version */ 0x2
                      | /* Root Complex Integrated Endpoint */ (VBOX_PCI_EXP_TYPE_ROOT_INT_EP << 4));
        PDMPciDevSetDWord(&pBus->aPciDev, 0xa0 + 4, VBOX_PCI_EXP_DEVCAP_RBE); /* Device capabilities. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 8, 0x0000); /* Device control. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 10, 0x0000); /* Device status. */
        PDMPciDevSetDWord(&pBus->aPciDev, 0xa0 + 12,
                         /* Max Link Speed */ 2
                       | /* Maximum Link Width */ (16 << 4)
                       | /* Active State Power Management (ASPM) Sopport */ (0 << 10)
                       | VBOX_PCI_EXP_LNKCAP_LBNC
                       | /* Port Number */ ((2 + iInstance) << 24)); /* Link capabilities. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 16, VBOX_PCI_EXP_LNKCTL_CLOCK); /* Link control. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 18,
                        /* Current Link Speed */ 2
                      | /* Negotiated Link Width */ (16 << 4)
                      | VBOX_PCI_EXP_LNKSTA_SL_CLK); /* Link status. */
        PDMPciDevSetDWord(&pBus->aPciDev, 0xa0 + 20,
                         /* Slot Power Limit Value */ (75 << 7)
                       | /* Physical Slot Number */ (0 << 19)); /* Slot capabilities. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 24, 0x0000); /* Slot control. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 26, 0x0000); /* Slot status. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 28, 0x0000); /* Root control. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 30, 0x0000); /* Root capabilities. */
        PDMPciDevSetDWord(&pBus->aPciDev, 0xa0 + 32, 0x00000000); /* Root status. */
        PDMPciDevSetDWord(&pBus->aPciDev, 0xa0 + 36, 0x00000000); /* Device capabilities 2. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 40, 0x0000); /* Device control 2. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 42, 0x0000); /* Device status 2. */
        PDMPciDevSetDWord(&pBus->aPciDev, 0xa0 + 44,
                         /* Supported Link Speeds Vector */ (2 << 1)); /* Link capabilities 2. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 48,
                        /* Target Link Speed */ 2); /* Link control 2. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 50, 0x0000); /* Link status 2. */
        PDMPciDevSetDWord(&pBus->aPciDev, 0xa0 + 52, 0x00000000); /* Slot capabilities 2. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 56, 0x0000); /* Slot control 2. */
        PDMPciDevSetWord(&pBus->aPciDev, 0xa0 + 58, 0x0000); /* Slot status 2. */
        PDMPciDevSetCapabilityList(&pBus->aPciDev, 0xa0);
    }
    else
    {
        PDMPciDevSetCommand(&pBus->aPciDev, 0x00);
        PDMPciDevSetStatus(&pBus->aPciDev, 0x20); /* 66MHz Capable. */
    }
    PDMPciDevSetInterruptLine(&pBus->aPciDev, 0x00); /* This device does not assert interrupts. */

    /*
     * This device does not generate interrupts. Interrupt delivery from
     * devices attached to the bus is unaffected.
     */
    PDMPciDevSetInterruptPin (&pBus->aPciDev, 0x00);

    if (fExpress)
    {
        /** @todo r=klaus set up the PCIe config space beyond the old 256 byte
         * limit, containing additional capability descriptors. */
    }

    /*
     * Register this PCI bridge. The called function will take care on which bus we will get registered.
     */
    rc = PDMDevHlpPCIRegisterEx(pDevIns, &pBus->aPciDev, PDMPCIDEVREG_CFG_PRIMARY, PDMPCIDEVREG_F_PCI_BRIDGE,
                                PDMPCIDEVREG_DEV_NO_FIRST_UNUSED, PDMPCIDEVREG_FUN_NO_FIRST_UNUSED, "ich9pcibridge");
    if (RT_FAILURE(rc))
        return rc;
    pBus->aPciDev.Int.s.pfnBridgeConfigRead  = ich9pcibridgeConfigRead;
    pBus->aPciDev.Int.s.pfnBridgeConfigWrite = ich9pcibridgeConfigWrite;

    /*
     * The iBus property doesn't really represent the bus number
     * because the guest and the BIOS can choose different bus numbers
     * for them.
     * The bus number is mainly for the setIrq function to indicate
     * when the host bus is reached which will have iBus = 0.
     * That's why the + 1.
     */
    pBus->iBus = iInstance + 1;

    /*
     * Register SSM handlers. We use the same saved state version as for the host bridge
     * to make changes easier.
     */
    rc = PDMDevHlpSSMRegisterEx(pDevIns, VBOX_ICH9PCI_SAVED_STATE_VERSION_CURRENT,
                                sizeof(*pBus) + 16*128,
                                "pgm" /* before */,
                                NULL, NULL, NULL,
                                NULL, ich9pcibridgeR3SaveExec, NULL,
                                NULL, ich9pcibridgeR3LoadExec, NULL);
    if (RT_FAILURE(rc))
        return rc;


    return VINF_SUCCESS;
}

/**
 * @interface_method_impl{PDMDEVREG,pfnDestruct}
 */
static DECLCALLBACK(int) ich9pcibridgeDestruct(PPDMDEVINS pDevIns)
{
    PICH9PCIBUS pBus = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);
    if (pBus->papBridgesR3)
    {
        PDMDevHlpMMHeapFree(pDevIns, pBus->papBridgesR3);
        pBus->papBridgesR3 = NULL;
    }
    return VINF_SUCCESS;
}

/**
 * @copydoc FNPDMDEVRELOCATE
 */
static DECLCALLBACK(void) ich9pcibridgeRelocate(PPDMDEVINS pDevIns, RTGCINTPTR offDelta)
{
    PICH9PCIBUS pBus = PDMINS_2_DATA(pDevIns, PICH9PCIBUS);
    pBus->pDevInsRC = PDMDEVINS_2_RCPTR(pDevIns);

    /* Relocate RC pointers for the attached pci devices. */
    for (uint32_t i = 0; i < RT_ELEMENTS(pBus->apDevices); i++)
        ich9pciRelocateDevice(pBus->apDevices[i], offDelta);
}

/**
 * The PCI bus device registration structure.
 */
const PDMDEVREG g_DevicePciIch9 =
{
    /* u32Version */
    PDM_DEVREG_VERSION,
    /* szName */
    "ich9pci",
    /* szRCMod */
    "VBoxDDRC.rc",
    /* szR0Mod */
    "VBoxDDR0.r0",
    /* pszDescription */
    "ICH9 PCI bridge",
    /* fFlags */
    PDM_DEVREG_FLAGS_DEFAULT_BITS | PDM_DEVREG_FLAGS_RC | PDM_DEVREG_FLAGS_R0,
    /* fClass */
    PDM_DEVREG_CLASS_BUS_PCI | PDM_DEVREG_CLASS_BUS_ISA,
    /* cMaxInstances */
    1,
    /* cbInstance */
    sizeof(ICH9PCIGLOBALS),
    /* pfnConstruct */
    ich9pciConstruct,
    /* pfnDestruct */
    ich9pciDestruct,
    /* pfnRelocate */
    ich9pciRelocate,
    /* pfnMemSetup */
    NULL,
    /* pfnPowerOn */
    NULL,
    /* pfnReset */
    ich9pciReset,
    /* pfnSuspend */
    NULL,
    /* pfnResume */
    NULL,
    /* pfnAttach */
    NULL,
    /* pfnDetach */
    NULL,
    /* pfnQueryInterface */
    NULL,
    /* pfnInitComplete */
    NULL,
    /* pfnPowerOff */
    NULL,
    /* pfnSoftReset */
    NULL,
    /* u32VersionEnd */
    PDM_DEVREG_VERSION
};

/**
 * The device registration structure
 * for the PCI-to-PCI bridge.
 */
const PDMDEVREG g_DevicePciIch9Bridge =
{
    /* u32Version */
    PDM_DEVREG_VERSION,
    /* szName */
    "ich9pcibridge",
    /* szRCMod */
    "VBoxDDRC.rc",
    /* szR0Mod */
    "VBoxDDR0.r0",
    /* pszDescription */
    "ICH9 PCI to PCI bridge",
    /* fFlags */
    PDM_DEVREG_FLAGS_DEFAULT_BITS | PDM_DEVREG_FLAGS_RC | PDM_DEVREG_FLAGS_R0,
    /* fClass */
    PDM_DEVREG_CLASS_BUS_PCI,
    /* cMaxInstances */
    ~0U,
    /* cbInstance */
    sizeof(ICH9PCIBUS),
    /* pfnConstruct */
    ich9pcibridgeConstruct,
    /* pfnDestruct */
    ich9pcibridgeDestruct,
    /* pfnRelocate */
    ich9pcibridgeRelocate,
    /* pfnMemSetup */
    NULL,
    /* pfnPowerOn */
    NULL,
    /* pfnReset */
    NULL, /* Must be NULL, to make sure only bus driver handles reset */
    /* pfnSuspend */
    NULL,
    /* pfnResume */
    NULL,
    /* pfnAttach */
    NULL,
    /* pfnDetach */
    NULL,
    /* pfnQueryInterface */
    NULL,
    /* pfnInitComplete */
    NULL,
    /* pfnPowerOff */
    NULL,
    /* pfnSoftReset */
    NULL,
    /* u32VersionEnd */
    PDM_DEVREG_VERSION
};

#endif /* IN_RING3 */
#endif /* !VBOX_DEVICE_STRUCT_TESTCASE */
