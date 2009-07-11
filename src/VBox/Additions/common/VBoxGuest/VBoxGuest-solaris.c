/* $Id: VBoxGuest-solaris.c $ */
/** @file
 * VirtualBox Guest Additions Driver for Solaris.
 */

/*
 * Copyright (C) 2007 Sun Microsystems, Inc.
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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <sys/conf.h>
#include <sys/modctl.h>
#include <sys/mutex.h>
#include <sys/pci.h>
#include <sys/stat.h>
#include <sys/ddi.h>
#include <sys/ddi_intr.h>
#include <sys/sunddi.h>
#include <sys/open.h>
#undef u /* /usr/include/sys/user.h:249:1 is where this is defined to (curproc->p_user). very cool. */

#include "VBoxGuestInternal.h"
#include <VBox/log.h>
#include <VBox/version.h>
#include <iprt/assert.h>
#include <iprt/initterm.h>
#include <iprt/process.h>
#include <iprt/mem.h>
#include <iprt/cdefs.h>
#include <iprt/asm.h>


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define VBOXSOLQUOTE2(x)                #x
#define VBOXSOLQUOTE(x)                 VBOXSOLQUOTE2(x)
/** The module name. */
#define DEVICE_NAME              "vboxguest"
/** The module description as seen in 'modinfo'. */
#define DEVICE_DESC              "VirtualBox GstDrv"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static int VBoxGuestSolarisOpen(dev_t *pDev, int fFlag, int fType, cred_t *pCred);
static int VBoxGuestSolarisClose(dev_t Dev, int fFlag, int fType, cred_t *pCred);
static int VBoxGuestSolarisRead(dev_t Dev, struct uio *pUio, cred_t *pCred);
static int VBoxGuestSolarisWrite(dev_t Dev, struct uio *pUio, cred_t *pCred);
static int VBoxGuestSolarisIOCtl(dev_t Dev, int Cmd, intptr_t pArg, int Mode, cred_t *pCred, int *pVal);
static int VBoxGuestSolarisPoll(dev_t Dev, short fEvents, int fAnyYet, short *pReqEvents, struct pollhead **ppPollHead);

static int VBoxGuestSolarisGetInfo(dev_info_t *pDip, ddi_info_cmd_t enmCmd, void *pArg, void **ppResult);
static int VBoxGuestSolarisAttach(dev_info_t *pDip, ddi_attach_cmd_t enmCmd);
static int VBoxGuestSolarisDetach(dev_info_t *pDip, ddi_detach_cmd_t enmCmd);

static int VBoxGuestSolarisAddIRQ(dev_info_t *pDip);
static void VBoxGuestSolarisRemoveIRQ(dev_info_t *pDip);
static uint_t VBoxGuestSolarisISR(caddr_t Arg);

DECLVBGL(int) VBoxGuestSolarisServiceCall(void *pvSession, unsigned iCmd, void *pvData, size_t cbData, size_t *pcbDataReturned);
DECLVBGL(void *) VBoxGuestSolarisServiceOpen(uint32_t *pu32Version);
DECLVBGL(int) VBoxGuestSolarisServiceClose(void *pvSession);


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/**
 * cb_ops: for drivers that support char/block entry points
 */
static struct cb_ops g_VBoxGuestSolarisCbOps =
{
    VBoxGuestSolarisOpen,
    VBoxGuestSolarisClose,
    nodev,                  /* b strategy */
    nodev,                  /* b dump */
    nodev,                  /* b print */
    VBoxGuestSolarisRead,
    VBoxGuestSolarisWrite,
    VBoxGuestSolarisIOCtl,
    nodev,                  /* c devmap */
    nodev,                  /* c mmap */
    nodev,                  /* c segmap */
    VBoxGuestSolarisPoll,
    ddi_prop_op,            /* property ops */
    NULL,                   /* streamtab  */
    D_NEW | D_MP,           /* compat. flag */
    CB_REV                  /* revision */
};

/**
 * dev_ops: for driver device operations
 */
static struct dev_ops g_VBoxGuestSolarisDevOps =
{
    DEVO_REV,               /* driver build revision */
    0,                      /* ref count */
    VBoxGuestSolarisGetInfo,
    nulldev,                /* identify */
    nulldev,                /* probe */
    VBoxGuestSolarisAttach,
    VBoxGuestSolarisDetach,
    nodev,                  /* reset */
    &g_VBoxGuestSolarisCbOps,
    (struct bus_ops *)0,
    nodev                   /* power */
};

/**
 * modldrv: export driver specifics to the kernel
 */
static struct modldrv g_VBoxGuestSolarisModule =
{
    &mod_driverops,         /* extern from kernel */
    DEVICE_DESC " " VBOX_VERSION_STRING "r" VBOXSOLQUOTE(VBOX_SVN_REV),
    &g_VBoxGuestSolarisDevOps
};

/**
 * modlinkage: export install/remove/info to the kernel
 */
static struct modlinkage g_VBoxGuestSolarisModLinkage =
{
    MODREV_1,               /* loadable module system revision */
    &g_VBoxGuestSolarisModule,
    NULL                    /* terminate array of linkage structures */
};

/**
 * State info for each open file handle.
 */
typedef struct
{
    /** Pointer to the session handle. */
    PVBOXGUESTSESSION       pSession;
    /** The process reference for posting signals */
    void                   *pvProcRef;
} vboxguest_state_t;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** Device handle (we support only one instance). */
static dev_info_t          *g_pDip = NULL;
/** Opaque pointer to file-descriptor states */
static void                *g_pVBoxGuestSolarisState = NULL;
/** Device extention & session data association structure. */
static VBOXGUESTDEVEXT      g_DevExt;
/** IO port handle. */
static ddi_acc_handle_t     g_PciIOHandle;
/** MMIO handle. */
static ddi_acc_handle_t     g_PciMMIOHandle;
/** IO Port. */
static uint16_t             g_uIOPortBase;
/** Address of the MMIO region.*/
static caddr_t              g_pMMIOBase;
/** Size of the MMIO region. */
static off_t                g_cbMMIO;
/** VMMDev Version. */
static uint32_t             g_u32Version;
/** Pointer to the interrupt handle vector */
static ddi_intr_handle_t   *g_pIntr;
/** Number of actually allocated interrupt handles */
static size_t               g_cIntrAllocated;
/** The pollhead structure */
static pollhead_t           g_PollHead;
/** The IRQ Mutex */
static kmutex_t             g_IrqMtx;


/**
 * Kernel entry points
 */
int _init(void)
{
    LogFlow((DEVICE_NAME ":_init\n"));
    int rc = ddi_soft_state_init(&g_pVBoxGuestSolarisState, sizeof(vboxguest_state_t), 1);
    if (!rc)
    {
        rc = mod_install(&g_VBoxGuestSolarisModLinkage);
        if (rc)
            ddi_soft_state_fini(&g_pVBoxGuestSolarisState);
    }
    return rc;
}


int _fini(void)
{
    LogFlow((DEVICE_NAME ":_fini\n"));
    int rc = mod_remove(&g_VBoxGuestSolarisModLinkage);
    if (!rc)
        ddi_soft_state_fini(&g_pVBoxGuestSolarisState);
    return rc;
}


int _info(struct modinfo *pModInfo)
{
    LogFlow((DEVICE_NAME ":_info\n"));
    return mod_info(&g_VBoxGuestSolarisModLinkage, pModInfo);
}


/**
 * Attach entry point, to attach a device to the system or resume it.
 *
 * @param   pDip            The module structure instance.
 * @param   enmCmd          Attach type (ddi_attach_cmd_t)
 *
 * @return  corresponding solaris error code.
 */
static int VBoxGuestSolarisAttach(dev_info_t *pDip, ddi_attach_cmd_t enmCmd)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisAttach\n"));
    switch (enmCmd)
    {
        case DDI_ATTACH:
        {
            if (g_pDip)
            {
                LogRel((DEVICE_NAME "::Attach: Only one instance supported.\n"));
                return DDI_FAILURE;
            }

            int rc;
            int instance;

            instance = ddi_get_instance(pDip);

            /*
             * Initialize IPRT R0 driver, which internally calls OS-specific r0 init.
             */
            rc = RTR0Init(0);
            if (RT_FAILURE(rc))
            {
                Log((DEVICE_NAME "::Attach: RTR0Init failed.\n"));
                return DDI_FAILURE;
            }

            /*
             * Enable resources for PCI access.
             */
            ddi_acc_handle_t PciHandle;
            rc = pci_config_setup(pDip, &PciHandle);
            if (rc == DDI_SUCCESS)
            {
                /*
                 * Map the register address space.
                 */
                caddr_t baseAddr;
                ddi_device_acc_attr_t deviceAttr;
                deviceAttr.devacc_attr_version = DDI_DEVICE_ATTR_V0;
                deviceAttr.devacc_attr_endian_flags = DDI_NEVERSWAP_ACC;
                deviceAttr.devacc_attr_dataorder = DDI_STRICTORDER_ACC;
                deviceAttr.devacc_attr_access = DDI_DEFAULT_ACC;
                rc = ddi_regs_map_setup(pDip, 1, &baseAddr, 0, 0, &deviceAttr, &g_PciIOHandle);
                if (rc == DDI_SUCCESS)
                {
                    /*
                     * Read size of the MMIO region.
                     */
                    g_uIOPortBase = (uintptr_t)baseAddr;
                    rc = ddi_dev_regsize(pDip, 2, &g_cbMMIO);
                    if (rc == DDI_SUCCESS)
                    {
                        rc = ddi_regs_map_setup(pDip, 2, &g_pMMIOBase, 0, g_cbMMIO, &deviceAttr,
                                        &g_PciMMIOHandle);
                        if (rc == DDI_SUCCESS)
                        {
                            /*
                             * Add IRQ of VMMDev.
                             */
                            rc = VBoxGuestSolarisAddIRQ(pDip);
                            if (rc == DDI_SUCCESS)
                            {
                                /*
                                 * Call the common device extension initializer.
                                 */
                                rc = VBoxGuestInitDevExt(&g_DevExt, g_uIOPortBase, g_pMMIOBase,
                                                         g_cbMMIO, VBOXOSTYPE_Solaris,
                                                         VMMDEV_EVENT_MOUSE_POSITION_CHANGED);
                                if (RT_SUCCESS(rc))
                                {
                                    rc = ddi_create_minor_node(pDip, DEVICE_NAME, S_IFCHR, instance, DDI_PSEUDO, 0);
                                    if (rc == DDI_SUCCESS)
                                    {
                                        g_pDip = pDip;
                                        pci_config_teardown(&PciHandle);
                                        return DDI_SUCCESS;
                                    }

                                    LogRel((DEVICE_NAME "::Attach: ddi_create_minor_node failed.\n"));
                                    VBoxGuestDeleteDevExt(&g_DevExt);
                                }
                                else
                                    LogRel((DEVICE_NAME "::Attach: VBoxGuestInitDevExt failed.\n"));
                                VBoxGuestSolarisRemoveIRQ(pDip);
                            }
                            else
                                LogRel((DEVICE_NAME "::Attach: VBoxGuestSolarisAddIRQ failed.\n"));
                            ddi_regs_map_free(&g_PciMMIOHandle);
                        }
                        else
                            LogRel((DEVICE_NAME "::Attach: ddi_regs_map_setup for MMIO region failed.\n"));
                    }
                    else
                        LogRel((DEVICE_NAME "::Attach: ddi_dev_regsize for MMIO region failed.\n"));
                    ddi_regs_map_free(&g_PciIOHandle);
                }
                else
                    LogRel((DEVICE_NAME "::Attach: ddi_regs_map_setup for IOport failed.\n"));
                pci_config_teardown(&PciHandle);
            }
            else
                LogRel((DEVICE_NAME "::Attach: pci_config_setup failed rc=%d.\n", rc));

            RTR0Term();
            return DDI_FAILURE;
        }

        case DDI_RESUME:
        {
            /** @todo implement resume for guest driver. */
            return DDI_SUCCESS;
        }

        default:
            return DDI_FAILURE;
    }
}


/**
 * Detach entry point, to detach a device to the system or suspend it.
 *
 * @param   pDip            The module structure instance.
 * @param   enmCmd          Attach type (ddi_attach_cmd_t)
 *
 * @return  corresponding solaris error code.
 */
static int VBoxGuestSolarisDetach(dev_info_t *pDip, ddi_detach_cmd_t enmCmd)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisDetach\n"));
    switch (enmCmd)
    {
        case DDI_DETACH:
        {
            VBoxGuestSolarisRemoveIRQ(pDip);
            ddi_regs_map_free(&g_PciIOHandle);
            ddi_regs_map_free(&g_PciMMIOHandle);
            ddi_remove_minor_node(pDip, NULL);

            RTR0Term();
            return DDI_SUCCESS;
        }

        case DDI_SUSPEND:
        {
            /** @todo implement suspend for guest driver. */
            return DDI_SUCCESS;
        }

        default:
            return DDI_FAILURE;
    }
}


/**
 * Info entry point, called by solaris kernel for obtaining driver info.
 *
 * @param   pDip            The module structure instance (do not use).
 * @param   enmCmd          Information request type.
 * @param   pvArg           Type specific argument.
 * @param   ppvResult       Where to store the requested info.
 *
 * @return  corresponding solaris error code.
 */
static int VBoxGuestSolarisGetInfo(dev_info_t *pDip, ddi_info_cmd_t enmCmd, void *pvArg, void **ppvResult)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisGetInfo\n"));

    int rc = DDI_SUCCESS;
    switch (enmCmd)
    {
        case DDI_INFO_DEVT2DEVINFO:
            *ppvResult = (void *)g_pDip;
            break;

        case DDI_INFO_DEVT2INSTANCE:
            *ppvResult = (void *)(uintptr_t)ddi_get_instance(g_pDip);
            break;

        default:
            rc = DDI_FAILURE;
            break;
    }

    NOREF(pvArg);
    return rc;
}


/**
 * User context entry points
 */
static int VBoxGuestSolarisOpen(dev_t *pDev, int fFlag, int fType, cred_t *pCred)
{
    int                 rc;
    PVBOXGUESTSESSION   pSession;

    LogFlow((DEVICE_NAME ":VBoxGuestSolarisOpen\n"));

    /*
     * Verify we are being opened as a character device.
     */
    if (fType != OTYP_CHR)
        return EINVAL;

    vboxguest_state_t *pState = NULL;
    unsigned iOpenInstance;
    for (iOpenInstance = 0; iOpenInstance < 4096; iOpenInstance++)
    {
        if (    !ddi_get_soft_state(g_pVBoxGuestSolarisState, iOpenInstance) /* faster */
            &&  ddi_soft_state_zalloc(g_pVBoxGuestSolarisState, iOpenInstance) == DDI_SUCCESS)
        {
            pState = ddi_get_soft_state(g_pVBoxGuestSolarisState, iOpenInstance);
            break;
        }
    }
    if (!pState)
    {
        Log((DEVICE_NAME ":VBoxGuestSolarisOpen: too many open instances."));
        return ENXIO;
    }

    /*
     * Create a new session.
     */
    rc = VBoxGuestCreateUserSession(&g_DevExt, &pSession);
    if (RT_SUCCESS(rc))
    {
        pState->pvProcRef = proc_ref();
        pState->pSession = pSession;
        *pDev = makedevice(getmajor(*pDev), iOpenInstance);
        Log((DEVICE_NAME "VBoxGuestSolarisOpen: pSession=%p pState=%p pid=%d\n", pSession, pState, (int)RTProcSelf()));
        return 0;
    }

    /* Failed, clean up. */
    ddi_soft_state_free(g_pVBoxGuestSolarisState, iOpenInstance);

    LogRel((DEVICE_NAME ":VBoxGuestSolarisOpen: VBoxGuestCreateUserSession failed. rc=%d\n", rc));
    return EFAULT;
}


static int VBoxGuestSolarisClose(dev_t Dev, int flag, int fType, cred_t *pCred)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisClose pid=%d\n", (int)RTProcSelf()));

    PVBOXGUESTSESSION pSession;
    vboxguest_state_t *pState = ddi_get_soft_state(g_pVBoxGuestSolarisState, getminor(Dev));
    if (!pState)
    {
        Log((DEVICE_NAME ":VBoxGuestSolarisClose: failed to get pState.\n"));
        return EFAULT;
    }

    proc_unref(pState->pvProcRef);
    pSession = pState->pSession;
    pState->pSession = NULL;
    Log((DEVICE_NAME ":VBoxGuestSolarisClose: pSession=%p pState=%p\n", pSession, pState));
    ddi_soft_state_free(g_pVBoxGuestSolarisState, getminor(Dev));
    if (!pSession)
    {
        Log((DEVICE_NAME ":VBoxGuestSolarisClose: failed to get pSession.\n"));
        return EFAULT;
    }

    /*
     * Close the session.
     */
    VBoxGuestCloseSession(&g_DevExt, pSession);
    return 0;
}


static int VBoxGuestSolarisRead(dev_t Dev, struct uio *pUio, cred_t *pCred)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisRead\n"));

    PVBOXGUESTSESSION pSession;
    vboxguest_state_t *pState = ddi_get_soft_state(g_pVBoxGuestSolarisState, getminor(Dev));
    if (!pState)
    {
        Log((DEVICE_NAME "::Close: failed to get pState.\n"));
        return EFAULT;
    }

    uint32_t u32CurSeq = ASMAtomicUoReadU32(&g_DevExt.u32MousePosChangedSeq);
    if (pSession->u32MousePosChangedSeq != u32CurSeq)
        pSession->u32MousePosChangedSeq = u32CurSeq;

    return 0;
}


static int VBoxGuestSolarisWrite(dev_t Dev, struct uio *pUio, cred_t *pCred)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisWrite\n"));
    return 0;
}


/** @def IOCPARM_LEN
 * Gets the length from the ioctl number.
 * This is normally defined by sys/ioccom.h on BSD systems...
 */
#ifndef IOCPARM_LEN
# define IOCPARM_LEN(Code)                      (((Code) >> 16) & IOCPARM_MASK)
#endif


/**
 * Driver ioctl, an alternate entry point for this character driver.
 *
 * @param   Dev             Device number
 * @param   Cmd             Operation identifier
 * @param   pArg            Arguments from user to driver
 * @param   Mode            Information bitfield (read/write, address space etc.)
 * @param   pCred           User credentials
 * @param   pVal            Return value for calling process.
 *
 * @return  corresponding solaris error code.
 */
static int VBoxGuestSolarisIOCtl(dev_t Dev, int Cmd, intptr_t pArg, int Mode, cred_t *pCred, int *pVal)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisIOCtl\n"));

    /*
     * Get the session from the soft state item.
     */
    vboxguest_state_t *pState = ddi_get_soft_state(g_pVBoxGuestSolarisState, getminor(Dev));
    if (!pState)
    {
        Log((DEVICE_NAME ":VBoxGuestSolarisIOCtl: no state data for %d\n", getminor(Dev)));
        return EINVAL;
    }

    PVBOXGUESTSESSION pSession = pState->pSession;
    if (!pSession)
    {
        Log((DEVICE_NAME ":VBoxGuestSolarisIOCtl: no session data for %d\n", getminor(Dev)));
        return EINVAL;
    }

    /*
     * Read and validate the request wrapper.
     */
    VBGLBIGREQ ReqWrap;
    if (IOCPARM_LEN(Cmd) != sizeof(ReqWrap))
    {
        LogRel((DEVICE_NAME ": VBoxGuestSolarisIOCtl: bad request %#x size=%d expected=%d\n", Cmd, IOCPARM_LEN(Cmd), sizeof(ReqWrap)));
        return ENOTTY;
    }

    int rc = ddi_copyin((void *)pArg, &ReqWrap, sizeof(ReqWrap), Mode);
    if (RT_UNLIKELY(rc))
    {
        LogRel((DEVICE_NAME ": VBoxGuestSolarisIOCtl: ddi_copyin failed to read header pArg=%p Cmd=%d. rc=%d.\n", pArg, Cmd, rc));
        return EINVAL;
    }

    if (ReqWrap.u32Magic != VBGLBIGREQ_MAGIC)
    {
        LogRel((DEVICE_NAME ": VBoxGuestSolarisIOCtl: bad magic %#x; pArg=%p Cmd=%d.\n", ReqWrap.u32Magic, pArg, Cmd));
        return EINVAL;
    }
    if (RT_UNLIKELY(   ReqWrap.cbData == 0
                    || ReqWrap.cbData > _1M*16))
    {
        Log((DEVICE_NAME ": VBoxGuestSolarisIOCtl: bad size %#x; pArg=%p Cmd=%d.\n", ReqWrap.cbData, pArg, Cmd));
        return EINVAL;
    }

    /*
     * Read the request.
     */
    void *pvBuf = RTMemTmpAlloc(ReqWrap.cbData);
    if (RT_UNLIKELY(!pvBuf))
    {
        LogRel((DEVICE_NAME ":VBoxGuestSolarisIOCtl: RTMemTmpAlloc failed to alloc %d bytes.\n", ReqWrap.cbData));
        return ENOMEM;
    }

    rc = ddi_copyin((void *)(uintptr_t)ReqWrap.pvDataR3, pvBuf, ReqWrap.cbData, Mode);
    if (RT_UNLIKELY(rc))
    {
        RTMemTmpFree(pvBuf);
        LogRel((DEVICE_NAME ":VBoxGuestSolarisIOCtl: ddi_copyin failed; pvBuf=%p pArg=%p Cmd=%d. rc=%d\n", pvBuf, pArg, Cmd, rc));
        return EFAULT;
    }
    if (RT_UNLIKELY(   ReqWrap.cbData != 0
                    && !VALID_PTR(pvBuf)))
    {
        RTMemTmpFree(pvBuf);
        LogRel((DEVICE_NAME ":VBoxGuestSolarisIOCtl: pvBuf invalid pointer %p\n", pvBuf));
        return EINVAL;
    }
    Log((DEVICE_NAME ":VBoxGuestSolarisIOCtl: pSession=%p pid=%d.\n", pSession, (int)RTProcSelf()));

    /*
     * Process the IOCtl.
     */
    size_t cbDataReturned;
    rc = VBoxGuestCommonIOCtl(Cmd, &g_DevExt, pSession, pvBuf, ReqWrap.cbData, &cbDataReturned);
    if (RT_SUCCESS(rc))
    {
        rc = 0;
        if (RT_UNLIKELY(cbDataReturned > ReqWrap.cbData))
        {
            LogRel((DEVICE_NAME ":VBoxGuestSolarisIOCtl: too much output data %d expected %d\n", cbDataReturned, ReqWrap.cbData));
            cbDataReturned = ReqWrap.cbData;
        }
        if (cbDataReturned > 0)
        {
            rc = ddi_copyout(pvBuf, (void *)(uintptr_t)ReqWrap.pvDataR3, cbDataReturned, Mode);
            if (RT_UNLIKELY(rc))
            {
                LogRel((DEVICE_NAME ":VBoxGuestSolarisIOCtl: ddi_copyout failed; pvBuf=%p pArg=%p Cmd=%d. rc=%d\n", pvBuf, pArg, Cmd, rc));
                rc = EFAULT;
            }
        }
    }
    else
    {
        LogRel((DEVICE_NAME ":VBoxGuestSolarisIOCtl: VBoxGuestCommonIOCtl failed. rc=%d\n", rc));
        rc = RTErrConvertToErrno(rc);
    }
    *pVal = rc;
    RTMemTmpFree(pvBuf);
    return rc;
}


static int VBoxGuestSolarisPoll(dev_t Dev, short fEvents, int fAnyYet, short *pReqEvents, struct pollhead **ppPollHead)
{
    LogFlow((DEVICE_NAME "::Poll: fEvents=%d fAnyYet=%d\n", fEvents, fAnyYet));

    vboxguest_state_t *pState = ddi_get_soft_state(g_pVBoxGuestSolarisState, getminor(Dev));
    if (RT_LIKELY(pState))
    {
        PVBOXGUESTSESSION pSession  = (PVBOXGUESTSESSION)pState->pSession;
        uint32_t u32CurSeq = ASMAtomicUoReadU32(&g_DevExt.u32MousePosChangedSeq);
        if (pSession->u32MousePosChangedSeq != u32CurSeq)
        {
            *pReqEvents |= (POLLIN | POLLRDNORM);
            pSession->u32MousePosChangedSeq = u32CurSeq;
        }
        else
        {
            *pReqEvents = 0;
            if (!fAnyYet)
                *ppPollHead = &g_PollHead;
        }

        return 0;
    }
    else
    {
        Log((DEVICE_NAME "::Poll: no state data for %d\n", getminor(Dev)));
        return EINVAL;
    }
}


/**
 * Sets IRQ for VMMDev.
 *
 * @returns Solaris error code.
 * @param   pDip     Pointer to the device info structure.
 */
static int VBoxGuestSolarisAddIRQ(dev_info_t *pDip)
{
    LogFlow((DEVICE_NAME "::AddIRQ: pDip=%p\n", pDip));

    int IntrType = 0;
    int rc = ddi_intr_get_supported_types(pDip, &IntrType);
    if (rc == DDI_SUCCESS)
    {
        /* We won't need to bother about MSIs. */
        if (IntrType & DDI_INTR_TYPE_FIXED)
        {
            int IntrCount = 0;
            rc = ddi_intr_get_nintrs(pDip, IntrType, &IntrCount);
            if (   rc == DDI_SUCCESS
                && IntrCount > 0)
            {
                int IntrAvail = 0;
                rc = ddi_intr_get_navail(pDip, IntrType, &IntrAvail);
                if (   rc == DDI_SUCCESS
                    && IntrAvail > 0)
                {
                    /* Allocated kernel memory for the interrupt handles. The allocation size is stored internally. */
                    g_pIntr = RTMemAlloc(IntrCount * sizeof(ddi_intr_handle_t));
                    if (g_pIntr)
                    {
                        int IntrAllocated;
                        rc = ddi_intr_alloc(pDip, g_pIntr, IntrType, 0, IntrCount, &IntrAllocated, DDI_INTR_ALLOC_NORMAL);
                        if (   rc == DDI_SUCCESS
                            && IntrAllocated > 0)
                        {
                            g_cIntrAllocated = IntrAllocated;
                            uint_t uIntrPriority;
                            rc = ddi_intr_get_pri(g_pIntr[0], &uIntrPriority);
                            if (rc == DDI_SUCCESS)
                            {
                                /* Initialize the mutex. */
                                mutex_init(&g_IrqMtx, NULL, MUTEX_DRIVER, DDI_INTR_PRI(uIntrPriority));

                                /* Assign interrupt handler functions and enable interrupts. */
                                for (int i = 0; i < IntrAllocated; i++)
                                {
                                    rc = ddi_intr_add_handler(g_pIntr[i], (ddi_intr_handler_t *)VBoxGuestSolarisISR,
                                                            NULL /* No Private Data */, NULL);
                                    if (rc == DDI_SUCCESS)
                                        rc = ddi_intr_enable(g_pIntr[i]);
                                    if (rc != DDI_SUCCESS)
                                    {
                                        /* Changing local IntrAllocated to hold so-far allocated handles for freeing. */
                                        IntrAllocated = i;
                                        break;
                                    }
                                }
                                if (rc == DDI_SUCCESS)
                                    return rc;

                                /* Remove any assigned handlers */
                                LogRel((DEVICE_NAME ":failed to assign IRQs allocated=%d\n", IntrAllocated));
                                for (int x = 0; x < IntrAllocated; x++)
                                    ddi_intr_remove_handler(g_pIntr[x]);
                            }
                            else
                                LogRel((DEVICE_NAME ":VBoxGuestSolarisAddIRQ: failed to get priority of interrupt. rc=%d\n", rc));

                            /* Remove allocated IRQs, too bad we can free only one handle at a time. */
                            for (int k = 0; k < g_cIntrAllocated; k++)
                                ddi_intr_free(g_pIntr[k]);
                        }
                        else
                            LogRel((DEVICE_NAME ":VBoxGuestSolarisAddIRQ: failed to allocated IRQs. count=%d\n", IntrCount));
                        RTMemFree(g_pIntr);
                    }
                    else
                        LogRel((DEVICE_NAME ":VBoxGuestSolarisAddIRQ: failed to allocated IRQs. count=%d\n", IntrCount));
                }
                else
                    LogRel((DEVICE_NAME ":VBoxGuestSolarisAddIRQ: failed to get or insufficient available IRQs. rc=%d IntrAvail=%d\n", rc, IntrAvail));
            }
            else
                LogRel((DEVICE_NAME ":VBoxGuestSolarisAddIRQ: failed to get or insufficient number of IRQs. rc=%d IntrCount=%d\n", rc, IntrCount));
        }
        else
            LogRel((DEVICE_NAME ":VBoxGuestSolarisAddIRQ: invalid irq type. IntrType=%#x\n", IntrType));
    }
    else
        LogRel((DEVICE_NAME ":VBoxGuestSolarisAddIRQ: failed to get supported interrupt types\n"));
    return rc;
}


/**
 * Removes IRQ for VMMDev.
 *
 * @param   pDip     Pointer to the device info structure.
 */
static void VBoxGuestSolarisRemoveIRQ(dev_info_t *pDip)
{
    LogFlow((DEVICE_NAME "::RemoveIRQ:\n"));

    for (int i = 0; i < g_cIntrAllocated; i++)
    {
        int rc = ddi_intr_disable(g_pIntr[i]);
        if (rc == DDI_SUCCESS)
        {
            rc = ddi_intr_remove_handler(g_pIntr[i]);
            if (rc == DDI_SUCCESS)
                ddi_intr_free(g_pIntr[i]);
        }
    }
    RTMemFree(g_pIntr);
    mutex_destroy(&g_IrqMtx);
}


/**
 * Interrupt Service Routine for VMMDev.
 *
 * @param   Arg     Private data (unused, will be NULL).
 * @returns DDI_INTR_CLAIMED if it's our interrupt, DDI_INTR_UNCLAIMED if it isn't.
 */
static uint_t VBoxGuestSolarisISR(caddr_t Arg)
{
    LogFlow((DEVICE_NAME "::ISR:\n"));

    mutex_enter(&g_IrqMtx);
    bool fOurIRQ = VBoxGuestCommonISR(&g_DevExt);
    mutex_exit(&g_IrqMtx);

    return fOurIRQ ? DDI_INTR_CLAIMED : DDI_INTR_UNCLAIMED;
}


void VBoxGuestNativeISRMousePollEvent(PVBOXGUESTDEVEXT pDevExt)
{
    LogFlow((DEVICE_NAME "::NativeISRMousePollEvent:\n"));

    /*
     * Wake up poll waiters.
     */
    pollwakeup(&g_PollHead, POLLIN | POLLRDNORM);
}


/**
 * VBoxGuest Common ioctl wrapper from VBoxGuestLib.
 *
 * @returns VBox error code.
 * @param   pvSession           Opaque pointer to the session.
 * @param   iCmd                Requested function.
 * @param   pvData              IO data buffer.
 * @param   cbData              Size of the data buffer.
 * @param   pcbDataReturned     Where to store the amount of returned data.
 */
DECLVBGL(int) VBoxGuestSolarisServiceCall(void *pvSession, unsigned iCmd, void *pvData, size_t cbData, size_t *pcbDataReturned)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisServiceCall %pvSesssion=%p Cmd=%u pvData=%p cbData=%d\n", pvSession, iCmd, pvData, cbData));

    PVBOXGUESTSESSION pSession = (PVBOXGUESTSESSION)pvSession;
    AssertPtrReturn(pSession, VERR_INVALID_POINTER);
    AssertMsgReturn(pSession->pDevExt == &g_DevExt,
                    ("SC: %p != %p\n", pSession->pDevExt, &g_DevExt), VERR_INVALID_HANDLE);

    return VBoxGuestCommonIOCtl(iCmd, &g_DevExt, pSession, pvData, cbData, pcbDataReturned);
}


/**
 * Solaris Guest service open.
 *
 * @returns Opaque pointer to session object.
 * @param   pu32Version         Where to store VMMDev version.
 */
DECLVBGL(void *) VBoxGuestSolarisServiceOpen(uint32_t *pu32Version)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisServiceOpen\n"));

    AssertPtrReturn(pu32Version, NULL);
    PVBOXGUESTSESSION pSession;
    int rc = VBoxGuestCreateKernelSession(&g_DevExt, &pSession);
    if (RT_SUCCESS(rc))
    {
        *pu32Version = VMMDEV_VERSION;
        return pSession;
    }
    LogRel((DEVICE_NAME ":VBoxGuestCreateKernelSession failed. rc=%d\n", rc));
    return NULL;
}


/**
 * Solaris Guest service close.
 *
 * @returns VBox error code.
 * @param   pvState             Opaque pointer to the session object.
 */
DECLVBGL(int) VBoxGuestSolarisServiceClose(void *pvSession)
{
    LogFlow((DEVICE_NAME ":VBoxGuestSolarisServiceClose\n"));

    PVBOXGUESTSESSION pSession = (PVBOXGUESTSESSION)pvSession;
    AssertPtrReturn(pSession, VERR_INVALID_POINTER);
    if (pSession)
    {
        VBoxGuestCloseSession(&g_DevExt, pSession);
        return VINF_SUCCESS;
    }
    LogRel((DEVICE_NAME ":Invalid pSession.\n"));
    return VERR_INVALID_HANDLE;
}

