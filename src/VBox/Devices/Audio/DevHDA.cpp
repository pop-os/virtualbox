/* $Id: DevHDA.cpp $ */
/** @file
 * DevHDA.cpp - VBox ICH Intel HD Audio Controller.
 *
 * Implemented against the specifications found in "High Definition Audio
 * Specification", Revision 1.0a June 17, 2010, and  "Intel I/O Controller
 * HUB 6 (ICH6) Family, Datasheet", document number 301473-002.
 */

/*
 * Copyright (C) 2006-2017 Oracle Corporation
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
#define LOG_GROUP LOG_GROUP_DEV_HDA
#include <VBox/log.h>

#include <VBox/vmm/pdmdev.h>
#include <VBox/vmm/pdmaudioifs.h>
#include <VBox/version.h>

#include <iprt/assert.h>
#include <iprt/asm.h>
#include <iprt/asm-math.h>
#include <iprt/circbuf.h>
#include <iprt/critsect.h>
#include <iprt/file.h>
#include <iprt/list.h>
#ifdef IN_RING3
# include <math.h>
# include <iprt/mem.h>
# include <iprt/semaphore.h>
# include <iprt/string.h>
# include <iprt/uuid.h>
#endif

#include "VBoxDD.h"

#include "AudioMixBuffer.h"
#include "AudioMixer.h"
#include "HDACodec.h"
#include "HDAStreamPeriod.h"
#include "DevHDA.h"
#include "DrvAudio.h"


/*********************************************************************************************************************************
*   Defined Constants And Macros                                                                                                 *
*********************************************************************************************************************************/
//#define HDA_AS_PCI_EXPRESS
#define VBOX_WITH_INTEL_HDA

/* Installs a DMA access handler (via PGM callback) to monitor
 * HDA's DMA operations, that is, writing / reading audio stream data.
 *
 * !!! Note: Certain guests are *that* timing sensitive that when enabling  !!!
 * !!!       such a handler will mess up audio completely (e.g. Windows 7). !!! */
//# define HDA_USE_DMA_ACCESS_HANDLER
#ifdef HDA_USE_DMA_ACCESS_HANDLER
# include <VBox/vmm/pgm.h>
#endif

/* Uses the DMA access handler to read the written DMA audio (output) data.
 * Only valid if HDA_USE_DMA_ACCESS_HANDLER is set.
 *
 * Also see the note / warning for HDA_USE_DMA_ACCESS_HANDLER. */
//# define HDA_USE_DMA_ACCESS_HANDLER_WRITING

/* Useful to debug the device' timing. */
//#define HDA_DEBUG_TIMING

/* To debug silence coming from the guest in form of audio gaps.
 * Very crude implementation for now. */
//#define HDA_DEBUG_SILENCE

#if defined(VBOX_WITH_HP_HDA)
/* HP Pavilion dv4t-1300 */
# define HDA_PCI_VENDOR_ID 0x103c
# define HDA_PCI_DEVICE_ID 0x30f7
#elif defined(VBOX_WITH_INTEL_HDA)
/* Intel HDA controller */
# define HDA_PCI_VENDOR_ID 0x8086
# define HDA_PCI_DEVICE_ID 0x2668
#elif defined(VBOX_WITH_NVIDIA_HDA)
/* nVidia HDA controller */
# define HDA_PCI_VENDOR_ID 0x10de
# define HDA_PCI_DEVICE_ID 0x0ac0
#else
# error "Please specify your HDA device vendor/device IDs"
#endif

/** Default timer frequency (in Hz).
 *
 *  Note: Keep in mind that the Hz rate has nothing to do with samples rates
 *        or DMA / interrupt timing -- it's purely needed in order to drive
 *        the data flow at a constant (and sufficient) rate.
 *
 *        Lowering this value can ask for trouble, as backends then can run
 *        into data underruns. */
#define HDA_TIMER_HZ        200

/** HDA's (fixed) audio frame size in bytes.
 *  We only support 16-bit stereo frames at the moment. */
#define HDA_FRAME_SIZE      4

#define HDA_NREGS           114
#define HDA_NREGS_SAVED     112

/**
 *  NB: Register values stored in memory (au32Regs[]) are indexed through
 *  the HDA_RMX_xxx macros (also HDA_MEM_IND_NAME()). On the other hand, the
 *  register descriptors in g_aHdaRegMap[] are indexed through the
 *  HDA_REG_xxx macros (also HDA_REG_IND_NAME()).
 *
 *  The au32Regs[] layout is kept unchanged for saved state
 *  compatibility. */

/* Registers */
#define HDA_REG_IND_NAME(x)                 HDA_REG_##x
#define HDA_MEM_IND_NAME(x)                 HDA_RMX_##x
#define HDA_REG_FIELD_MASK(reg, x)          HDA_##reg##_##x##_MASK
#define HDA_REG_FIELD_FLAG_MASK(reg, x)     RT_BIT(HDA_##reg##_##x##_SHIFT)
#define HDA_REG_FIELD_SHIFT(reg, x)         HDA_##reg##_##x##_SHIFT
#define HDA_REG_IND(pThis, x)               ((pThis)->au32Regs[g_aHdaRegMap[x].mem_idx])
#define HDA_REG(pThis, x)                   (HDA_REG_IND((pThis), HDA_REG_IND_NAME(x)))
#define HDA_REG_FLAG_VALUE(pThis, reg, val) (HDA_REG((pThis),reg) & (((HDA_REG_FIELD_FLAG_MASK(reg, val)))))


#define HDA_REG_GCAP                0 /* range 0x00-0x01*/
#define HDA_RMX_GCAP                0
/* GCAP HDASpec 3.3.2 This macro encodes the following information about HDA in a compact manner:
 * oss (15:12) - number of output streams supported
 * iss (11:8)  - number of input streams supported
 * bss (7:3)   - number of bidirectional streams supported
 * bds (2:1)   - number of serial data out signals supported
 * b64sup (0)  - 64 bit addressing supported.
 */
#define HDA_MAKE_GCAP(oss, iss, bss, bds, b64sup) \
    (  (((oss) & 0xF)  << 12)   \
     | (((iss) & 0xF)  << 8)    \
     | (((bss) & 0x1F) << 3)    \
     | (((bds) & 0x3)  << 2)    \
     | ((b64sup) & 1))

#define HDA_REG_VMIN                1 /* 0x02 */
#define HDA_RMX_VMIN                1

#define HDA_REG_VMAJ                2 /* 0x03 */
#define HDA_RMX_VMAJ                2

#define HDA_REG_OUTPAY              3 /* 0x04-0x05 */
#define HDA_RMX_OUTPAY              3

#define HDA_REG_INPAY               4 /* 0x06-0x07 */
#define HDA_RMX_INPAY               4

#define HDA_REG_GCTL                5 /* 0x08-0x0B */
#define HDA_RMX_GCTL                5
#define HDA_GCTL_RST_SHIFT          0
#define HDA_GCTL_FSH_SHIFT          1
#define HDA_GCTL_UR_SHIFT           8

#define HDA_REG_WAKEEN              6 /* 0x0C */
#define HDA_RMX_WAKEEN              6

#define HDA_REG_STATESTS            7 /* 0x0E */
#define HDA_RMX_STATESTS            7
#define HDA_STATESTS_SCSF_MASK      0x7 /* State Change Status Flags (6.2.8). */

#define HDA_REG_GSTS                8 /* 0x10-0x11*/
#define HDA_RMX_GSTS                8
#define HDA_GSTS_FSH_SHIFT          1

#define HDA_REG_OUTSTRMPAY          9  /* 0x18 */
#define HDA_RMX_OUTSTRMPAY          112

#define HDA_REG_INSTRMPAY           10 /* 0x1a */
#define HDA_RMX_INSTRMPAY           113

#define HDA_REG_INTCTL              11 /* 0x20 */
#define HDA_RMX_INTCTL              9
#define HDA_INTCTL_GIE_SHIFT        31
#define HDA_INTCTL_CIE_SHIFT        30
#define HDA_INTCTL_S0_SHIFT         0
#define HDA_INTCTL_S1_SHIFT         1
#define HDA_INTCTL_S2_SHIFT         2
#define HDA_INTCTL_S3_SHIFT         3
#define HDA_INTCTL_S4_SHIFT         4
#define HDA_INTCTL_S5_SHIFT         5
#define HDA_INTCTL_S6_SHIFT         6
#define HDA_INTCTL_S7_SHIFT         7
#define INTCTL_SX(pThis, X)         (HDA_REG_FLAG_VALUE((pThis), INTCTL, S##X))

#define HDA_REG_INTSTS              12 /* 0x24 */
#define HDA_RMX_INTSTS              10
#define HDA_INTSTS_FLAG_CIS         RT_BIT(30) /* Controller Interrupt Status (CIS). */
#define HDA_INTSTS_FLAG_GIS         RT_BIT(31) /* Global Interrupt Status (GIS). */
#define HDA_INTSTS_MASK_SIS         0xFF       /* Stream Interrupt Status (SIS). */

#define HDA_REG_WALCLK              13 /* 0x24 */
#define HDA_RMX_WALCLK              /* Not defined! */

/* Note: The HDA specification defines a SSYNC register at offset 0x38. The
 * ICH6/ICH9 datahseet defines SSYNC at offset 0x34. The Linux HDA driver matches
 * the datasheet.
 */
#define HDA_REG_SSYNC               14 /* 0x34 */
#define HDA_RMX_SSYNC               12

#define HDA_REG_CORBLBASE           15 /* 0x40 */
#define HDA_RMX_CORBLBASE           13

#define HDA_REG_CORBUBASE           16 /* 0x44 */
#define HDA_RMX_CORBUBASE           14

#define HDA_REG_CORBWP              17 /* 0x48 */
#define HDA_RMX_CORBWP              15

#define HDA_REG_CORBRP              18 /* 0x4A */
#define HDA_RMX_CORBRP              16
#define HDA_CORBRP_RST_SHIFT        15
#define HDA_CORBRP_RST              RT_BIT(15)
#define HDA_CORBRP_WP_SHIFT         0
#define HDA_CORBRP_WP_MASK          0xFF

#define HDA_REG_CORBCTL             19 /* 0x4C */
#define HDA_RMX_CORBCTL             17
#define HDA_CORBCTL_DMA_SHIFT       1
#define HDA_CORBCTL_CMEIE_SHIFT     0

#define HDA_REG_CORBSTS             20 /* 0x4D */
#define HDA_RMX_CORBSTS             18
#define HDA_CORBSTS_CMEI_SHIFT      0

#define HDA_REG_CORBSIZE            21 /* 0x4E */
#define HDA_RMX_CORBSIZE            19
#define HDA_CORBSIZE_SZ_CAP         0xF0
#define HDA_CORBSIZE_SZ             0x3
/* till ich 10 sizes of CORB and RIRB are hardcoded to 256 in real hw */

#define HDA_REG_RIRBLBASE           22 /* 0x50 */
#define HDA_RMX_RIRBLBASE           20

#define HDA_REG_RIRBUBASE           23 /* 0x54 */
#define HDA_RMX_RIRBUBASE           21

#define HDA_REG_RIRBWP              24 /* 0x58 */
#define HDA_RMX_RIRBWP              22
#define HDA_RIRBWP_RST_SHIFT        15
#define HDA_RIRBWP_WP_MASK          0xFF

#define HDA_REG_RINTCNT             25 /* 0x5A */
#define HDA_RMX_RINTCNT             23
#define RINTCNT_N(pThis)            (HDA_REG(pThis, RINTCNT) & 0xff)

#define HDA_REG_RIRBCTL             26 /* 0x5C */
#define HDA_RMX_RIRBCTL             24
#define HDA_RIRBCTL_RIC_SHIFT       0
#define HDA_RIRBCTL_DMA_SHIFT       1
#define HDA_ROI_DMA_SHIFT           2

#define HDA_REG_RIRBSTS             27 /* 0x5D */
#define HDA_RMX_RIRBSTS             25
#define HDA_RIRBSTS_RINTFL_SHIFT    0
#define HDA_RIRBSTS_RIRBOIS_SHIFT   2

#define HDA_REG_RIRBSIZE            28 /* 0x5E */
#define HDA_RMX_RIRBSIZE            26
#define HDA_RIRBSIZE_SZ_CAP         0xF0
#define HDA_RIRBSIZE_SZ             0x3

#define RIRBSIZE_SZ(pThis)          (HDA_REG(pThis, HDA_REG_RIRBSIZE) & HDA_RIRBSIZE_SZ)
#define RIRBSIZE_SZ_CAP(pThis)      (HDA_REG(pThis, HDA_REG_RIRBSIZE) & HDA_RIRBSIZE_SZ_CAP)


#define HDA_REG_IC                  29 /* 0x60 */
#define HDA_RMX_IC                  27

#define HDA_REG_IR                  30 /* 0x64 */
#define HDA_RMX_IR                  28

#define HDA_REG_IRS                 31 /* 0x68 */
#define HDA_RMX_IRS                 29
#define HDA_IRS_ICB_SHIFT           0
#define HDA_IRS_IRV_SHIFT           1

#define HDA_REG_DPLBASE             32 /* 0x70 */
#define HDA_RMX_DPLBASE             30
#define DPLBASE(pThis)              (HDA_REG((pThis), DPLBASE))

#define HDA_REG_DPUBASE             33 /* 0x74 */
#define HDA_RMX_DPUBASE             31
#define DPUBASE(pThis)              (HDA_REG((pThis), DPUBASE))

#define DPBASE_ADDR_MASK            (~(uint64_t)0x7f)

#define HDA_STREAM_REG_DEF(name, num)           (HDA_REG_SD##num##name)
#define HDA_STREAM_RMX_DEF(name, num)           (HDA_RMX_SD##num##name)
/* Note: sdnum here _MUST_ be stream reg number [0,7]. */
#define HDA_STREAM_REG(pThis, name, sdnum)      (HDA_REG_IND((pThis), HDA_REG_SD0##name + (sdnum) * 10))

#define HDA_SD_NUM_FROM_REG(pThis, func, reg)   ((reg - HDA_STREAM_REG_DEF(func, 0)) / 10)

#define HDA_REG_SD0CTL              34 /* 0x80 */
#define HDA_REG_SD1CTL              (HDA_STREAM_REG_DEF(CTL, 0) + 10) /* 0xA0 */
#define HDA_REG_SD2CTL              (HDA_STREAM_REG_DEF(CTL, 0) + 20) /* 0xC0 */
#define HDA_REG_SD3CTL              (HDA_STREAM_REG_DEF(CTL, 0) + 30) /* 0xE0 */
#define HDA_REG_SD4CTL              (HDA_STREAM_REG_DEF(CTL, 0) + 40) /* 0x100 */
#define HDA_REG_SD5CTL              (HDA_STREAM_REG_DEF(CTL, 0) + 50) /* 0x120 */
#define HDA_REG_SD6CTL              (HDA_STREAM_REG_DEF(CTL, 0) + 60) /* 0x140 */
#define HDA_REG_SD7CTL              (HDA_STREAM_REG_DEF(CTL, 0) + 70) /* 0x160 */
#define HDA_RMX_SD0CTL              32
#define HDA_RMX_SD1CTL              (HDA_STREAM_RMX_DEF(CTL, 0) + 10)
#define HDA_RMX_SD2CTL              (HDA_STREAM_RMX_DEF(CTL, 0) + 20)
#define HDA_RMX_SD3CTL              (HDA_STREAM_RMX_DEF(CTL, 0) + 30)
#define HDA_RMX_SD4CTL              (HDA_STREAM_RMX_DEF(CTL, 0) + 40)
#define HDA_RMX_SD5CTL              (HDA_STREAM_RMX_DEF(CTL, 0) + 50)
#define HDA_RMX_SD6CTL              (HDA_STREAM_RMX_DEF(CTL, 0) + 60)
#define HDA_RMX_SD7CTL              (HDA_STREAM_RMX_DEF(CTL, 0) + 70)

#define SD(func, num)               SD##num##func

#define HDA_SDCTL(pThis, num)       HDA_REG((pThis), SD(CTL, num))
#define HDA_SDCTL_NUM(pThis, num)   ((HDA_SDCTL((pThis), num) & HDA_REG_FIELD_MASK(SDCTL,NUM)) >> HDA_REG_FIELD_SHIFT(SDCTL, NUM))
#define HDA_SDCTL_NUM_MASK          0xF
#define HDA_SDCTL_NUM_SHIFT         20
#define HDA_SDCTL_DIR_SHIFT         19
#define HDA_SDCTL_TP_SHIFT          18
#define HDA_SDCTL_STRIPE_MASK       0x3
#define HDA_SDCTL_STRIPE_SHIFT      16
#define HDA_SDCTL_DEIE_SHIFT        4
#define HDA_SDCTL_FEIE_SHIFT        3
#define HDA_SDCTL_ICE_SHIFT         2
#define HDA_SDCTL_RUN_SHIFT         1
#define HDA_SDCTL_SRST_SHIFT        0

#define HDA_REG_SD0STS              35 /* 0x83 */
#define HDA_REG_SD1STS              (HDA_STREAM_REG_DEF(STS, 0) + 10) /* 0xA3 */
#define HDA_REG_SD2STS              (HDA_STREAM_REG_DEF(STS, 0) + 20) /* 0xC3 */
#define HDA_REG_SD3STS              (HDA_STREAM_REG_DEF(STS, 0) + 30) /* 0xE3 */
#define HDA_REG_SD4STS              (HDA_STREAM_REG_DEF(STS, 0) + 40) /* 0x103 */
#define HDA_REG_SD5STS              (HDA_STREAM_REG_DEF(STS, 0) + 50) /* 0x123 */
#define HDA_REG_SD6STS              (HDA_STREAM_REG_DEF(STS, 0) + 60) /* 0x143 */
#define HDA_REG_SD7STS              (HDA_STREAM_REG_DEF(STS, 0) + 70) /* 0x163 */
#define HDA_RMX_SD0STS              33
#define HDA_RMX_SD1STS              (HDA_STREAM_RMX_DEF(STS, 0) + 10)
#define HDA_RMX_SD2STS              (HDA_STREAM_RMX_DEF(STS, 0) + 20)
#define HDA_RMX_SD3STS              (HDA_STREAM_RMX_DEF(STS, 0) + 30)
#define HDA_RMX_SD4STS              (HDA_STREAM_RMX_DEF(STS, 0) + 40)
#define HDA_RMX_SD5STS              (HDA_STREAM_RMX_DEF(STS, 0) + 50)
#define HDA_RMX_SD6STS              (HDA_STREAM_RMX_DEF(STS, 0) + 60)
#define HDA_RMX_SD7STS              (HDA_STREAM_RMX_DEF(STS, 0) + 70)

#define SDSTS(pThis, num)           HDA_REG((pThis), SD(STS, num))
#define HDA_SDSTS_FIFORDY_SHIFT     5
#define HDA_SDSTS_DE_SHIFT          4
#define HDA_SDSTS_FE_SHIFT          3
#define HDA_SDSTS_BCIS_SHIFT        2

#define HDA_REG_SD0LPIB             36 /* 0x84 */
#define HDA_REG_SD1LPIB             (HDA_STREAM_REG_DEF(LPIB, 0) + 10) /* 0xA4 */
#define HDA_REG_SD2LPIB             (HDA_STREAM_REG_DEF(LPIB, 0) + 20) /* 0xC4 */
#define HDA_REG_SD3LPIB             (HDA_STREAM_REG_DEF(LPIB, 0) + 30) /* 0xE4 */
#define HDA_REG_SD4LPIB             (HDA_STREAM_REG_DEF(LPIB, 0) + 40) /* 0x104 */
#define HDA_REG_SD5LPIB             (HDA_STREAM_REG_DEF(LPIB, 0) + 50) /* 0x124 */
#define HDA_REG_SD6LPIB             (HDA_STREAM_REG_DEF(LPIB, 0) + 60) /* 0x144 */
#define HDA_REG_SD7LPIB             (HDA_STREAM_REG_DEF(LPIB, 0) + 70) /* 0x164 */
#define HDA_RMX_SD0LPIB             34
#define HDA_RMX_SD1LPIB             (HDA_STREAM_RMX_DEF(LPIB, 0) + 10)
#define HDA_RMX_SD2LPIB             (HDA_STREAM_RMX_DEF(LPIB, 0) + 20)
#define HDA_RMX_SD3LPIB             (HDA_STREAM_RMX_DEF(LPIB, 0) + 30)
#define HDA_RMX_SD4LPIB             (HDA_STREAM_RMX_DEF(LPIB, 0) + 40)
#define HDA_RMX_SD5LPIB             (HDA_STREAM_RMX_DEF(LPIB, 0) + 50)
#define HDA_RMX_SD6LPIB             (HDA_STREAM_RMX_DEF(LPIB, 0) + 60)
#define HDA_RMX_SD7LPIB             (HDA_STREAM_RMX_DEF(LPIB, 0) + 70)

#define HDA_REG_SD0CBL              37 /* 0x88 */
#define HDA_REG_SD1CBL              (HDA_STREAM_REG_DEF(CBL, 0) + 10) /* 0xA8 */
#define HDA_REG_SD2CBL              (HDA_STREAM_REG_DEF(CBL, 0) + 20) /* 0xC8 */
#define HDA_REG_SD3CBL              (HDA_STREAM_REG_DEF(CBL, 0) + 30) /* 0xE8 */
#define HDA_REG_SD4CBL              (HDA_STREAM_REG_DEF(CBL, 0) + 40) /* 0x108 */
#define HDA_REG_SD5CBL              (HDA_STREAM_REG_DEF(CBL, 0) + 50) /* 0x128 */
#define HDA_REG_SD6CBL              (HDA_STREAM_REG_DEF(CBL, 0) + 60) /* 0x148 */
#define HDA_REG_SD7CBL              (HDA_STREAM_REG_DEF(CBL, 0) + 70) /* 0x168 */
#define HDA_RMX_SD0CBL              35
#define HDA_RMX_SD1CBL              (HDA_STREAM_RMX_DEF(CBL, 0) + 10)
#define HDA_RMX_SD2CBL              (HDA_STREAM_RMX_DEF(CBL, 0) + 20)
#define HDA_RMX_SD3CBL              (HDA_STREAM_RMX_DEF(CBL, 0) + 30)
#define HDA_RMX_SD4CBL              (HDA_STREAM_RMX_DEF(CBL, 0) + 40)
#define HDA_RMX_SD5CBL              (HDA_STREAM_RMX_DEF(CBL, 0) + 50)
#define HDA_RMX_SD6CBL              (HDA_STREAM_RMX_DEF(CBL, 0) + 60)
#define HDA_RMX_SD7CBL              (HDA_STREAM_RMX_DEF(CBL, 0) + 70)

#define HDA_REG_SD0LVI              38 /* 0x8C */
#define HDA_REG_SD1LVI              (HDA_STREAM_REG_DEF(LVI, 0) + 10) /* 0xAC */
#define HDA_REG_SD2LVI              (HDA_STREAM_REG_DEF(LVI, 0) + 20) /* 0xCC */
#define HDA_REG_SD3LVI              (HDA_STREAM_REG_DEF(LVI, 0) + 30) /* 0xEC */
#define HDA_REG_SD4LVI              (HDA_STREAM_REG_DEF(LVI, 0) + 40) /* 0x10C */
#define HDA_REG_SD5LVI              (HDA_STREAM_REG_DEF(LVI, 0) + 50) /* 0x12C */
#define HDA_REG_SD6LVI              (HDA_STREAM_REG_DEF(LVI, 0) + 60) /* 0x14C */
#define HDA_REG_SD7LVI              (HDA_STREAM_REG_DEF(LVI, 0) + 70) /* 0x16C */
#define HDA_RMX_SD0LVI              36
#define HDA_RMX_SD1LVI              (HDA_STREAM_RMX_DEF(LVI, 0) + 10)
#define HDA_RMX_SD2LVI              (HDA_STREAM_RMX_DEF(LVI, 0) + 20)
#define HDA_RMX_SD3LVI              (HDA_STREAM_RMX_DEF(LVI, 0) + 30)
#define HDA_RMX_SD4LVI              (HDA_STREAM_RMX_DEF(LVI, 0) + 40)
#define HDA_RMX_SD5LVI              (HDA_STREAM_RMX_DEF(LVI, 0) + 50)
#define HDA_RMX_SD6LVI              (HDA_STREAM_RMX_DEF(LVI, 0) + 60)
#define HDA_RMX_SD7LVI              (HDA_STREAM_RMX_DEF(LVI, 0) + 70)

#define HDA_REG_SD0FIFOW            39 /* 0x8E */
#define HDA_REG_SD1FIFOW            (HDA_STREAM_REG_DEF(FIFOW, 0) + 10) /* 0xAE */
#define HDA_REG_SD2FIFOW            (HDA_STREAM_REG_DEF(FIFOW, 0) + 20) /* 0xCE */
#define HDA_REG_SD3FIFOW            (HDA_STREAM_REG_DEF(FIFOW, 0) + 30) /* 0xEE */
#define HDA_REG_SD4FIFOW            (HDA_STREAM_REG_DEF(FIFOW, 0) + 40) /* 0x10E */
#define HDA_REG_SD5FIFOW            (HDA_STREAM_REG_DEF(FIFOW, 0) + 50) /* 0x12E */
#define HDA_REG_SD6FIFOW            (HDA_STREAM_REG_DEF(FIFOW, 0) + 60) /* 0x14E */
#define HDA_REG_SD7FIFOW            (HDA_STREAM_REG_DEF(FIFOW, 0) + 70) /* 0x16E */
#define HDA_RMX_SD0FIFOW            37
#define HDA_RMX_SD1FIFOW            (HDA_STREAM_RMX_DEF(FIFOW, 0) + 10)
#define HDA_RMX_SD2FIFOW            (HDA_STREAM_RMX_DEF(FIFOW, 0) + 20)
#define HDA_RMX_SD3FIFOW            (HDA_STREAM_RMX_DEF(FIFOW, 0) + 30)
#define HDA_RMX_SD4FIFOW            (HDA_STREAM_RMX_DEF(FIFOW, 0) + 40)
#define HDA_RMX_SD5FIFOW            (HDA_STREAM_RMX_DEF(FIFOW, 0) + 50)
#define HDA_RMX_SD6FIFOW            (HDA_STREAM_RMX_DEF(FIFOW, 0) + 60)
#define HDA_RMX_SD7FIFOW            (HDA_STREAM_RMX_DEF(FIFOW, 0) + 70)

/*
 * ICH6 datasheet defined limits for FIFOW values (18.2.38).
 */
#define HDA_SDFIFOW_8B              0x2
#define HDA_SDFIFOW_16B             0x3
#define HDA_SDFIFOW_32B             0x4

#define HDA_REG_SD0FIFOS            40 /* 0x90 */
#define HDA_REG_SD1FIFOS            (HDA_STREAM_REG_DEF(FIFOS, 0) + 10) /* 0xB0 */
#define HDA_REG_SD2FIFOS            (HDA_STREAM_REG_DEF(FIFOS, 0) + 20) /* 0xD0 */
#define HDA_REG_SD3FIFOS            (HDA_STREAM_REG_DEF(FIFOS, 0) + 30) /* 0xF0 */
#define HDA_REG_SD4FIFOS            (HDA_STREAM_REG_DEF(FIFOS, 0) + 40) /* 0x110 */
#define HDA_REG_SD5FIFOS            (HDA_STREAM_REG_DEF(FIFOS, 0) + 50) /* 0x130 */
#define HDA_REG_SD6FIFOS            (HDA_STREAM_REG_DEF(FIFOS, 0) + 60) /* 0x150 */
#define HDA_REG_SD7FIFOS            (HDA_STREAM_REG_DEF(FIFOS, 0) + 70) /* 0x170 */
#define HDA_RMX_SD0FIFOS            38
#define HDA_RMX_SD1FIFOS            (HDA_STREAM_RMX_DEF(FIFOS, 0) + 10)
#define HDA_RMX_SD2FIFOS            (HDA_STREAM_RMX_DEF(FIFOS, 0) + 20)
#define HDA_RMX_SD3FIFOS            (HDA_STREAM_RMX_DEF(FIFOS, 0) + 30)
#define HDA_RMX_SD4FIFOS            (HDA_STREAM_RMX_DEF(FIFOS, 0) + 40)
#define HDA_RMX_SD5FIFOS            (HDA_STREAM_RMX_DEF(FIFOS, 0) + 50)
#define HDA_RMX_SD6FIFOS            (HDA_STREAM_RMX_DEF(FIFOS, 0) + 60)
#define HDA_RMX_SD7FIFOS            (HDA_STREAM_RMX_DEF(FIFOS, 0) + 70)

/*
 * ICH6 datasheet defines limits for FIFOS registers (18.2.39)
 * formula: size - 1
 * Other values not listed are not supported.
 */
/** Maximum FIFO size (in bytes). */
#define HDA_FIFO_MAX                256

#define HDA_SDINFIFO_120B           0x77 /* 8-, 16-, 20-, 24-, 32-bit Input Streams */
#define HDA_SDINFIFO_160B           0x9F /* 20-, 24-bit Input Streams Streams */

#define HDA_SDONFIFO_16B            0x0F /* 8-, 16-, 20-, 24-, 32-bit Output Streams */
#define HDA_SDONFIFO_32B            0x1F /* 8-, 16-, 20-, 24-, 32-bit Output Streams */
#define HDA_SDONFIFO_64B            0x3F /* 8-, 16-, 20-, 24-, 32-bit Output Streams */
#define HDA_SDONFIFO_128B           0x7F /* 8-, 16-, 20-, 24-, 32-bit Output Streams */
#define HDA_SDONFIFO_192B           0xBF /* 8-, 16-, 20-, 24-, 32-bit Output Streams */
#define HDA_SDONFIFO_256B           0xFF /* 20-, 24-bit Output Streams */
#define SDFIFOS(pThis, num)         HDA_REG((pThis), SD(FIFOS, num))

#define HDA_REG_SD0FMT              41 /* 0x92 */
#define HDA_REG_SD1FMT              (HDA_STREAM_REG_DEF(FMT, 0) + 10) /* 0xB2 */
#define HDA_REG_SD2FMT              (HDA_STREAM_REG_DEF(FMT, 0) + 20) /* 0xD2 */
#define HDA_REG_SD3FMT              (HDA_STREAM_REG_DEF(FMT, 0) + 30) /* 0xF2 */
#define HDA_REG_SD4FMT              (HDA_STREAM_REG_DEF(FMT, 0) + 40) /* 0x112 */
#define HDA_REG_SD5FMT              (HDA_STREAM_REG_DEF(FMT, 0) + 50) /* 0x132 */
#define HDA_REG_SD6FMT              (HDA_STREAM_REG_DEF(FMT, 0) + 60) /* 0x152 */
#define HDA_REG_SD7FMT              (HDA_STREAM_REG_DEF(FMT, 0) + 70) /* 0x172 */
#define HDA_RMX_SD0FMT              39
#define HDA_RMX_SD1FMT              (HDA_STREAM_RMX_DEF(FMT, 0) + 10)
#define HDA_RMX_SD2FMT              (HDA_STREAM_RMX_DEF(FMT, 0) + 20)
#define HDA_RMX_SD3FMT              (HDA_STREAM_RMX_DEF(FMT, 0) + 30)
#define HDA_RMX_SD4FMT              (HDA_STREAM_RMX_DEF(FMT, 0) + 40)
#define HDA_RMX_SD5FMT              (HDA_STREAM_RMX_DEF(FMT, 0) + 50)
#define HDA_RMX_SD6FMT              (HDA_STREAM_RMX_DEF(FMT, 0) + 60)
#define HDA_RMX_SD7FMT              (HDA_STREAM_RMX_DEF(FMT, 0) + 70)

#define SDFMT(pThis, num)           (HDA_REG((pThis), SD(FMT, num)))
#define HDA_SDFMT_BASE_RATE_SHIFT   14
#define HDA_SDFMT_BASE_RATE_MASK    RT_BIT(14)
#define HDA_SDFMT_MULT_SHIFT        11
#define HDA_SDFMT_MULT_MASK         0x7
#define HDA_SDFMT_DIV_SHIFT         8
#define HDA_SDFMT_DIV_MASK          0x7
#define HDA_SDFMT_BITS_SHIFT        4
#define HDA_SDFMT_BITS_MASK         0x7
#define SDFMT_BASE_RATE(pThis, num) ((SDFMT(pThis, num) & HDA_REG_FIELD_FLAG_MASK(SDFMT, BASE_RATE)) >> HDA_REG_FIELD_SHIFT(SDFMT, BASE_RATE))
#define SDFMT_MULT(pThis, num)      ((SDFMT((pThis), num) & HDA_REG_FIELD_MASK(SDFMT,MULT)) >> HDA_REG_FIELD_SHIFT(SDFMT, MULT))
#define SDFMT_DIV(pThis, num)       ((SDFMT((pThis), num) & HDA_REG_FIELD_MASK(SDFMT,DIV)) >> HDA_REG_FIELD_SHIFT(SDFMT, DIV))

#define HDA_REG_SD0BDPL             42 /* 0x98 */
#define HDA_REG_SD1BDPL             (HDA_STREAM_REG_DEF(BDPL, 0) + 10) /* 0xB8 */
#define HDA_REG_SD2BDPL             (HDA_STREAM_REG_DEF(BDPL, 0) + 20) /* 0xD8 */
#define HDA_REG_SD3BDPL             (HDA_STREAM_REG_DEF(BDPL, 0) + 30) /* 0xF8 */
#define HDA_REG_SD4BDPL             (HDA_STREAM_REG_DEF(BDPL, 0) + 40) /* 0x118 */
#define HDA_REG_SD5BDPL             (HDA_STREAM_REG_DEF(BDPL, 0) + 50) /* 0x138 */
#define HDA_REG_SD6BDPL             (HDA_STREAM_REG_DEF(BDPL, 0) + 60) /* 0x158 */
#define HDA_REG_SD7BDPL             (HDA_STREAM_REG_DEF(BDPL, 0) + 70) /* 0x178 */
#define HDA_RMX_SD0BDPL             40
#define HDA_RMX_SD1BDPL             (HDA_STREAM_RMX_DEF(BDPL, 0) + 10)
#define HDA_RMX_SD2BDPL             (HDA_STREAM_RMX_DEF(BDPL, 0) + 20)
#define HDA_RMX_SD3BDPL             (HDA_STREAM_RMX_DEF(BDPL, 0) + 30)
#define HDA_RMX_SD4BDPL             (HDA_STREAM_RMX_DEF(BDPL, 0) + 40)
#define HDA_RMX_SD5BDPL             (HDA_STREAM_RMX_DEF(BDPL, 0) + 50)
#define HDA_RMX_SD6BDPL             (HDA_STREAM_RMX_DEF(BDPL, 0) + 60)
#define HDA_RMX_SD7BDPL             (HDA_STREAM_RMX_DEF(BDPL, 0) + 70)

#define HDA_REG_SD0BDPU             43 /* 0x9C */
#define HDA_REG_SD1BDPU             (HDA_STREAM_REG_DEF(BDPU, 0) + 10) /* 0xBC */
#define HDA_REG_SD2BDPU             (HDA_STREAM_REG_DEF(BDPU, 0) + 20) /* 0xDC */
#define HDA_REG_SD3BDPU             (HDA_STREAM_REG_DEF(BDPU, 0) + 30) /* 0xFC */
#define HDA_REG_SD4BDPU             (HDA_STREAM_REG_DEF(BDPU, 0) + 40) /* 0x11C */
#define HDA_REG_SD5BDPU             (HDA_STREAM_REG_DEF(BDPU, 0) + 50) /* 0x13C */
#define HDA_REG_SD6BDPU             (HDA_STREAM_REG_DEF(BDPU, 0) + 60) /* 0x15C */
#define HDA_REG_SD7BDPU             (HDA_STREAM_REG_DEF(BDPU, 0) + 70) /* 0x17C */
#define HDA_RMX_SD0BDPU             41
#define HDA_RMX_SD1BDPU             (HDA_STREAM_RMX_DEF(BDPU, 0) + 10)
#define HDA_RMX_SD2BDPU             (HDA_STREAM_RMX_DEF(BDPU, 0) + 20)
#define HDA_RMX_SD3BDPU             (HDA_STREAM_RMX_DEF(BDPU, 0) + 30)
#define HDA_RMX_SD4BDPU             (HDA_STREAM_RMX_DEF(BDPU, 0) + 40)
#define HDA_RMX_SD5BDPU             (HDA_STREAM_RMX_DEF(BDPU, 0) + 50)
#define HDA_RMX_SD6BDPU             (HDA_STREAM_RMX_DEF(BDPU, 0) + 60)
#define HDA_RMX_SD7BDPU             (HDA_STREAM_RMX_DEF(BDPU, 0) + 70)

#define HDA_BDLE_FLAG_IOC           RT_BIT(0) /* Interrupt on completion (IOC). */

#define HDA_CODEC_CAD_SHIFT         28
/* Encodes the (required) LUN into a codec command. */
#define HDA_CODEC_CMD(cmd, lun)     ((cmd) | (lun << HDA_CODEC_CAD_SHIFT))



/*********************************************************************************************************************************
*   Structures and Typedefs                                                                                                      *
*********************************************************************************************************************************/

#ifdef HDA_USE_DMA_ACCESS_HANDLER
typedef struct HDADMAACCESSHANDLER
{
    /** Node for storing this handler in our list in HDASTREAMSTATE. */
    RTLISTNODER3            Node;
    /** Pointer to stream to which this access handler is assigned to. */
    R3PTRTYPE(PHDASTREAM)   pStream;
    /** Access handler type handle. */
    PGMPHYSHANDLERTYPE      hAccessHandlerType;
    /** First address this handler uses. */
    RTGCPHYS                GCPhysFirst;
    /** Last address this handler uses. */
    RTGCPHYS                GCPhysLast;
    /** Actual BDLE address to handle. */
    RTGCPHYS                BDLEAddr;
    /** Actual BDLE buffer size to handle. */
    RTGCPHYS                BDLESize;
    /** Whether the access handler has been registered or not. */
    bool                    fRegistered;
    uint8_t                 Padding[3];
} HDADMAACCESSHANDLER, *PHDADMAACCESSHANDLER;
#endif

typedef struct HDAINPUTSTREAM
{
    /** PCM line input stream. */
    R3PTRTYPE(PPDMAUDIOGSTSTRMIN)      pStrmIn;
    /** Mixer handle for line input stream. */
    R3PTRTYPE(PAUDMIXSTREAM)           phStrmIn;
} HDAINPUTSTREAM, *PHDAINPUTSTREAM;

typedef struct HDAOUTPUTSTREAM
{
    /** PCM output stream. */
    R3PTRTYPE(PPDMAUDIOGSTSTRMOUT)     pStrmOut;
    /** Mixer handle for line output stream. */
    R3PTRTYPE(PAUDMIXSTREAM)           phStrmOut;
} HDAOUTPUTSTREAM, *PHDAOUTPUTSTREAM;

/**
 * Struct for maintaining a host backend driver.
 * This driver must be associated to one, and only one,
 * HDA codec. The HDA controller does the actual multiplexing
 * of HDA codec data to various host backend drivers then.
 *
 * This HDA device uses a timer in order to synchronize all
 * read/write accesses across all attached LUNs / backends.
 */
typedef struct HDADRIVER
{
    /** Node for storing this driver in our device driver list of HDASTATE. */
    RTLISTNODER3                       Node;
    /** Pointer to HDA controller (state). */
    R3PTRTYPE(PHDASTATE)               pHDAState;
    /** Driver flags. */
    PDMAUDIODRVFLAGS                   fFlags;
    uint8_t                            u32Padding0[2];
    /** LUN to which this driver has been assigned. */
    uint8_t                            uLUN;
    /** Whether this driver is in an attached state or not. */
    bool                               fAttached;
    /** Pointer to attached driver base interface. */
    R3PTRTYPE(PPDMIBASE)               pDrvBase;
    /** Audio connector interface to the underlying host backend. */
    R3PTRTYPE(PPDMIAUDIOCONNECTOR)     pConnector;
    /** Stream for line input. */
    HDAINPUTSTREAM                     LineIn;
    /** Stream for mic input. */
    HDAINPUTSTREAM                     MicIn;
    /** Stream for output. */
    HDAOUTPUTSTREAM                    Out;
} HDADRIVER;

#ifdef DEBUG
/** @todo Make STAM values out of this? */
typedef struct HDASTATEDBGINFO
{
    /** Timestamp (in ns) of the last timer callback (hdaTimer).
     * Used to calculate the time actually elapsed between two timer callbacks. */
    uint64_t                           tsTimerLastCalledNs;
    /** IRQ debugging information. */
    struct
    {
        /** Timestamp (in ns) of last processed (asserted / deasserted) IRQ. */
        uint64_t                       tsProcessedLastNs;
        /** Timestamp (in ns) of last asserted IRQ. */
        uint64_t                       tsAssertedNs;
        /** How many IRQs have been asserted already. */
        uint64_t                       cAsserted;
        /** Accumulated elapsed time (in ns) of all IRQ being asserted. */
        uint64_t                       tsAssertedTotalNs;
        /** Timestamp (in ns) of last deasserted IRQ. */
        uint64_t                       tsDeassertedNs;
        /** How many IRQs have been deasserted already. */
        uint64_t                       cDeasserted;
        /** Accumulated elapsed time (in ns) of all IRQ being deasserted. */
        uint64_t                       tsDeassertedTotalNs;
    } IRQ;
} HDASTATEDBGINFO, *PHDASTATEDBGINFO;
#endif

/**
 * ICH Intel HD Audio Controller state.
 */
typedef struct HDASTATE
{
    /** The PCI device structure. */
    PDMPCIDEV                          PciDev;
    /** R3 Pointer to the device instance. */
    PPDMDEVINSR3                       pDevInsR3;
    /** R0 Pointer to the device instance. */
    PPDMDEVINSR0                       pDevInsR0;
    /** R0 Pointer to the device instance. */
    PPDMDEVINSRC                       pDevInsRC;
    /** Padding for alignment. */
    uint32_t                           u32Padding;
    /** The base interface for LUN\#0. */
    PDMIBASE                           IBase;
    RTGCPHYS                           MMIOBaseAddr;
    /** The HDA's register set. */
    uint32_t                           au32Regs[HDA_NREGS];
    /** Stream state for line-in. */
    HDASTREAM                          LineIn;
    /** Stream state for microphone-in. */
    HDASTREAM                          MicIn;
    /** Stream state for output. */
    HDASTREAM                          Out;
    /** CORB buffer base address. */
    uint64_t                           u64CORBBase;
    /** RIRB buffer base address. */
    uint64_t                           u64RIRBBase;
    /** DMA base address.
     *  Made out of DPLBASE + DPUBASE (3.3.32 + 3.3.33). */
    uint64_t                           u64DPBase;
    /** DMA position buffer enable bit. */
    bool                               fDMAPosition;
    /** Padding for alignment. */
    uint8_t                            u32Padding0[7];
    /** Pointer to CORB buffer. */
    R3PTRTYPE(uint32_t *)              pu32CorbBuf;
    /** Size in bytes of CORB buffer. */
    uint32_t                           cbCorbBuf;
    /** Padding for alignment. */
    uint32_t                           u32Padding1;
    /** Pointer to RIRB buffer. */
    R3PTRTYPE(uint64_t *)              pu64RirbBuf;
    /** Size in bytes of RIRB buffer. */
    uint32_t                           cbRirbBuf;
    /** Indicates if HDA is in reset. */
    bool                               fInReset;
    /** Flag whether the R0 part is enabled. */
    bool                               fR0Enabled;
    /** Flag whether the RC part is enabled. */
    bool                               fRCEnabled;
#ifndef VBOX_WITH_AUDIO_CALLBACKS
    /** The timer instance. */
    PTMTIMERR3                         pTimer;
    /** The current timer expire time (in timer ticks). */
    uint64_t                           tsTimerExpire;
    /** The (fixed) timer interval (in timer ticks). */
    uint64_t                           cTimerTicks;
#endif
#ifdef VBOX_WITH_STATISTICS
# ifndef VBOX_WITH_AUDIO_CALLBACKS
    STAMPROFILE                        StatTimer;
# endif
    STAMCOUNTER                        StatBytesRead;
    STAMCOUNTER                        StatBytesWritten;
#endif
    /** Pointer to HDA codec to use. */
    R3PTRTYPE(PHDACODEC)               pCodec;
    /** List of associated LUN drivers (HDADRIVER). */
    RTLISTANCHORR3                     lstDrv;
    /** The device' software mixer. */
    R3PTRTYPE(PAUDIOMIXER)             pMixer;
    /** Audio sink for PCM output. */
    R3PTRTYPE(PAUDMIXSINK)             pSinkOutput;
    /** Audio mixer sink for line input. */
    R3PTRTYPE(PAUDMIXSINK)             pSinkLineIn;
    /** Audio mixer sink for microphone input. */
    R3PTRTYPE(PAUDMIXSINK)             pSinkMicIn;
    /** The controller's base time stamp which the WALCLK register
     *  derives its current value from. */
    uint64_t                           u64BaseTS;
    /** Last updated WALCLK counter. */
    uint64_t                           u64WalClk;
    /** Response Interrupt Count (RINTCNT).
     *  Needed if Response Interrupt Control (RIC) is being used .*/
    uint8_t                            u8RespIntCnt;
    /** Current IRQ level. */
    uint8_t                            u8IRQL;
    /** Padding for alignment. */
    uint8_t                            au8Padding2[5];
#ifdef DEBUG
    HDASTATEDBGINFO                    Dbg;
#endif
} HDASTATE;
/** Pointer to the ICH Intel HD Audio Controller state. */
typedef HDASTATE *PHDASTATE;

#ifdef VBOX_WITH_AUDIO_CALLBACKS
typedef struct HDACALLBACKCTX
{
    PHDASTATE  pThis;
    PHDADRIVER pDriver;
} HDACALLBACKCTX, *PHDACALLBACKCTX;
#endif

/*********************************************************************************************************************************
*   Internal Functions                                                                                                           *
*********************************************************************************************************************************/
#ifndef VBOX_DEVICE_STRUCT_TESTCASE
# ifdef IN_RING3
static FNPDMDEVRESET hdaReset;
#endif

/*
 * Stubs.
 */
static int hdaRegReadUnimpl(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegWriteUnimpl(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);

/*
 * Global register set read/write functions.
 */
static int hdaRegWriteGCTL(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);
static int hdaRegReadINTSTS(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegReadLPIB(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegReadWALCLK(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegWriteCORBWP(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);
static int hdaRegWriteCORBRP(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int hdaRegWriteCORBCTL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int hdaRegWriteCORBSTS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int hdaRegWriteRIRBWP(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);
static int hdaRegWriteRIRBSTS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int hdaRegWriteSTATESTS(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);
static int hdaRegWriteIRS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int hdaRegReadIRS(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegWriteBase(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);

/*
 * {IOB}SDn read/write functions.
 */
static int  hdaRegWriteSDCBL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int  hdaRegWriteSDCTL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int  hdaRegWriteSDSTS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int  hdaRegWriteSDLVI(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int  hdaRegWriteSDFIFOW(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int  hdaRegWriteSDFIFOS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int  hdaRegWriteSDFMT(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int  hdaRegWriteSDBDPL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
static int  hdaRegWriteSDBDPU(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
inline bool hdaRegWriteSDIsAllowed(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);

/*
 * Generic register read/write functions.
 */
static int hdaRegReadU32(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegWriteU32(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);
static int hdaRegReadU24(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegWriteU24(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);
static int hdaRegReadU16(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegWriteU16(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);
static int hdaRegReadU8(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
static int hdaRegWriteU8(PHDASTATE pThis, uint32_t iReg, uint32_t pu32Value);


#ifdef IN_RING3
static int       hdaSDFMTToStrmCfg(uint32_t u32SDFMT, PPDMAUDIOSTREAMCFG pCfg);

static void      hdaStreamDestroy(PHDASTREAM pStream);
static uint32_t  hdaStreamTransferGetElapsed(PHDASTATE pThis, PHDASTREAM pStream);
static void      hdaStreamTransfer(PHDASTATE pThis, PHDASTREAM pStream, uint32_t cbToProcessMax);
DECLINLINE(void) hdaStreamUpdateLPIB(PHDASTATE pThis, PHDASTREAM pStream, uint32_t u32LPIB);

static int       hdaBDLEFetch(PHDASTATE pThis, PHDABDLE pBDLE, uint64_t u64BaseDMA, uint16_t u16Entry);
static bool      hdaBDLEIsComplete(PHDABDLE pBDLE);
static bool      hdaBDLENeedsInterrupt(PHDABDLE pBDLE);
# ifdef DEBUG
static void      hdaBDLEDumpAll(PHDASTATE pThis, uint64_t u64BaseDMA, uint16_t cBDLE);
# endif /* DEBUG */

# ifdef HDA_USE_DMA_ACCESS_HANDLER
 static DECLCALLBACK(VBOXSTRICTRC) hdaDMAAccessHandler(PVM pVM, PVMCPU pVCpu, RTGCPHYS GCPhys, void *pvPhys, void *pvBuf, size_t cbBuf, PGMACCESSTYPE enmAccessType, PGMACCESSORIGIN enmOrigin, void *pvUser);
 static bool                       hdaStreamRegisterDMAHandlers(PHDASTATE pThis, PHDASTREAM pStream);
 static void                       hdaStreamUnregisterDMAHandlers(PHDASTATE pThis, PHDASTREAM pStream);
# endif /* HDA_USE_DMA_ACCESS_HANDLER */
#endif /* IN_RING3 */

uint64_t          hdaWalClkGetCurrent(PHDASTATE pThis);
#ifdef IN_RING3
bool              hdaWalClkSet(PHDASTATE pThis, uint64_t u64WalClk, bool fForce);
#endif

/*********************************************************************************************************************************
*   Global Variables                                                                                                             *
*********************************************************************************************************************************/

/** Offset of the SD0 register map. */
#define HDA_REG_DESC_SD0_BASE 0x80

/** Turn a short global register name into an memory index and a stringized name. */
#define HDA_REG_IDX(abbrev)         HDA_MEM_IND_NAME(abbrev), #abbrev

/** Turns a short stream register name into an memory index and a stringized name. */
#define HDA_REG_IDX_STRM(reg, suff) HDA_MEM_IND_NAME(reg ## suff), #reg #suff

/** Same as above for a register *not* stored in memory. */
#define HDA_REG_IDX_LOCAL(abbrev)   0, #abbrev

/** Emits a single audio stream register set (e.g. OSD0) at a specified offset. */
#define HDA_REG_MAP_STRM(offset, name) \
    /* offset        size     read mask   write mask  read callback   write callback     index + abbrev                  description */ \
    /* -------       -------  ----------  ----------  --------------  -----------------  ------------------------------  ----------- */ \
    /* Offset 0x80 (SD0) */ \
    { offset,        0x00003, 0x00FF001F, 0x00F0001F, hdaRegReadU24 , hdaRegWriteSDCTL , HDA_REG_IDX_STRM(name, CTL)  , #name " Stream Descriptor Control" }, \
    /* Offset 0x83 (SD0) */ \
    { offset + 0x3,  0x00001, 0x0000001C, 0x0000003C, hdaRegReadU8  , hdaRegWriteSDSTS , HDA_REG_IDX_STRM(name, STS)  , #name " Status" }, \
    /* Offset 0x84 (SD0) */ \
    { offset + 0x4,  0x00004, 0xFFFFFFFF, 0x00000000, hdaRegReadLPIB, hdaRegWriteUnimpl, HDA_REG_IDX_STRM(name, LPIB) , #name " Link Position In Buffer" }, \
    /* Offset 0x88 (SD0) */ \
    { offset + 0x8,  0x00004, 0xFFFFFFFF, 0xFFFFFFFF, hdaRegReadU32, hdaRegWriteSDCBL  , HDA_REG_IDX_STRM(name, CBL)  , #name " Cyclic Buffer Length" }, \
    /* Offset 0x8C (SD0) */ \
    { offset + 0xC,  0x00002, 0x0000FFFF, 0x0000FFFF, hdaRegReadU16, hdaRegWriteSDLVI  , HDA_REG_IDX_STRM(name, LVI)  , #name " Last Valid Index" }, \
    /* Reserved: FIFO Watermark. ** @todo Document this! */ \
    { offset + 0xE,  0x00002, 0x00000007, 0x00000007, hdaRegReadU16, hdaRegWriteSDFIFOW, HDA_REG_IDX_STRM(name, FIFOW), #name " FIFO Watermark" }, \
    /* Offset 0x90 (SD0) */ \
    { offset + 0x10, 0x00002, 0x000000FF, 0x00000000, hdaRegReadU16, hdaRegWriteSDFIFOS, HDA_REG_IDX_STRM(name, FIFOS), #name " FIFO Size" }, \
    /* Offset 0x92 (SD0) */ \
    { offset + 0x12, 0x00002, 0x00007F7F, 0x00007F7F, hdaRegReadU16, hdaRegWriteSDFMT  , HDA_REG_IDX_STRM(name, FMT)  , #name " Stream Format" }, \
    /* Reserved: 0x94 - 0x98. */ \
    /* Offset 0x98 (SD0) */ \
    { offset + 0x18, 0x00004, 0xFFFFFF80, 0xFFFFFF80, hdaRegReadU32, hdaRegWriteSDBDPL , HDA_REG_IDX_STRM(name, BDPL) , #name " Buffer Descriptor List Pointer-Lower Base Address" }, \
    /* Offset 0x9C (SD0) */ \
    { offset + 0x1C, 0x00004, 0xFFFFFFFF, 0xFFFFFFFF, hdaRegReadU32, hdaRegWriteSDBDPU , HDA_REG_IDX_STRM(name, BDPU) , #name " Buffer Descriptor List Pointer-Upper Base Address" }

/** Defines a single audio stream register set (e.g. OSD0). */
#define HDA_REG_MAP_DEF_STREAM(index, name) \
    HDA_REG_MAP_STRM(HDA_REG_DESC_SD0_BASE + (index * 32 /* 0x20 */), name)

/* See 302349 p 6.2. */
static const struct HDAREGDESC
{
    /** Register offset in the register space. */
    uint32_t    offset;
    /** Size in bytes. Registers of size > 4 are in fact tables. */
    uint32_t    size;
    /** Readable bits. */
    uint32_t    readable;
    /** Writable bits. */
    uint32_t    writable;
    /** Read callback. */
    int       (*pfnRead)(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value);
    /** Write callback. */
    int       (*pfnWrite)(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value);
    /** Index into the register storage array. */
    uint32_t    mem_idx;
    /** Abbreviated name. */
    const char *abbrev;
    /** Descripton. */
    const char *desc;
} g_aHdaRegMap[HDA_NREGS] =

{
    /* offset  size     read mask   write mask  read callback            write callback         index + abbrev   */
    /*-------  -------  ----------  ----------  -----------------------  ---------------------- ---------------- */
    { 0x00000, 0x00002, 0x0000FFFB, 0x00000000, hdaRegReadU16          , hdaRegWriteUnimpl     , HDA_REG_IDX(GCAP)         }, /* Global Capabilities */
    { 0x00002, 0x00001, 0x000000FF, 0x00000000, hdaRegReadU8           , hdaRegWriteUnimpl     , HDA_REG_IDX(VMIN)         }, /* Minor Version */
    { 0x00003, 0x00001, 0x000000FF, 0x00000000, hdaRegReadU8           , hdaRegWriteUnimpl     , HDA_REG_IDX(VMAJ)         }, /* Major Version */
    { 0x00004, 0x00002, 0x0000FFFF, 0x00000000, hdaRegReadU16          , hdaRegWriteUnimpl     , HDA_REG_IDX(OUTPAY)       }, /* Output Payload Capabilities */
    { 0x00006, 0x00002, 0x0000FFFF, 0x00000000, hdaRegReadU16          , hdaRegWriteUnimpl     , HDA_REG_IDX(INPAY)        }, /* Input Payload Capabilities */
    { 0x00008, 0x00004, 0x00000103, 0x00000103, hdaRegReadU32          , hdaRegWriteGCTL       , HDA_REG_IDX(GCTL)         }, /* Global Control */
    { 0x0000c, 0x00002, 0x00007FFF, 0x00007FFF, hdaRegReadU16          , hdaRegWriteU16        , HDA_REG_IDX(WAKEEN)       }, /* Wake Enable */
    { 0x0000e, 0x00002, 0x00000007, 0x00000007, hdaRegReadU8           , hdaRegWriteSTATESTS   , HDA_REG_IDX(STATESTS)     }, /* State Change Status */
    { 0x00010, 0x00002, 0xFFFFFFFF, 0x00000000, hdaRegReadUnimpl       , hdaRegWriteUnimpl     , HDA_REG_IDX(GSTS)         }, /* Global Status */
    { 0x00018, 0x00002, 0x0000FFFF, 0x00000000, hdaRegReadU16          , hdaRegWriteUnimpl     , HDA_REG_IDX(OUTSTRMPAY)   }, /* Output Stream Payload Capability */
    { 0x0001A, 0x00002, 0x0000FFFF, 0x00000000, hdaRegReadU16          , hdaRegWriteUnimpl     , HDA_REG_IDX(INSTRMPAY)    }, /* Input Stream Payload Capability */
    { 0x00020, 0x00004, 0xC00000FF, 0xC00000FF, hdaRegReadU32          , hdaRegWriteU32        , HDA_REG_IDX(INTCTL)       }, /* Interrupt Control */
    { 0x00024, 0x00004, 0xC00000FF, 0x00000000, hdaRegReadINTSTS       , hdaRegWriteUnimpl     , HDA_REG_IDX(INTSTS)       }, /* Interrupt Status */
    { 0x00030, 0x00004, 0xFFFFFFFF, 0x00000000, hdaRegReadWALCLK       , hdaRegWriteUnimpl     , HDA_REG_IDX_LOCAL(WALCLK) }, /* Wall Clock Counter */
    { 0x00034, 0x00004, 0x000000FF, 0x000000FF, hdaRegReadU32          , hdaRegWriteU32        , HDA_REG_IDX(SSYNC)        }, /* Stream Synchronization */
    { 0x00040, 0x00004, 0xFFFFFF80, 0xFFFFFF80, hdaRegReadU32          , hdaRegWriteBase       , HDA_REG_IDX(CORBLBASE)    }, /* CORB Lower Base Address */
    { 0x00044, 0x00004, 0xFFFFFFFF, 0xFFFFFFFF, hdaRegReadU32          , hdaRegWriteBase       , HDA_REG_IDX(CORBUBASE)    }, /* CORB Upper Base Address */
    { 0x00048, 0x00002, 0x000000FF, 0x000000FF, hdaRegReadU16          , hdaRegWriteCORBWP     , HDA_REG_IDX(CORBWP)       }, /* CORB Write Pointer */
    { 0x0004A, 0x00002, 0x000080FF, 0x000080FF, hdaRegReadU16          , hdaRegWriteCORBRP     , HDA_REG_IDX(CORBRP)       }, /* CORB Read Pointer */
    { 0x0004C, 0x00001, 0x00000003, 0x00000003, hdaRegReadU8           , hdaRegWriteCORBCTL    , HDA_REG_IDX(CORBCTL)      }, /* CORB Control */
    { 0x0004D, 0x00001, 0x00000001, 0x00000001, hdaRegReadU8           , hdaRegWriteCORBSTS    , HDA_REG_IDX(CORBSTS)      }, /* CORB Status */
    { 0x0004E, 0x00001, 0x000000F3, 0x00000000, hdaRegReadU8           , hdaRegWriteUnimpl     , HDA_REG_IDX(CORBSIZE)     }, /* CORB Size */
    { 0x00050, 0x00004, 0xFFFFFF80, 0xFFFFFF80, hdaRegReadU32          , hdaRegWriteBase       , HDA_REG_IDX(RIRBLBASE)    }, /* RIRB Lower Base Address */
    { 0x00054, 0x00004, 0xFFFFFFFF, 0xFFFFFFFF, hdaRegReadU32          , hdaRegWriteBase       , HDA_REG_IDX(RIRBUBASE)    }, /* RIRB Upper Base Address */
    { 0x00058, 0x00002, 0x000000FF, 0x00008000, hdaRegReadU8           , hdaRegWriteRIRBWP     , HDA_REG_IDX(RIRBWP)       }, /* RIRB Write Pointer */
    { 0x0005A, 0x00002, 0x000000FF, 0x000000FF, hdaRegReadU16          , hdaRegWriteU16        , HDA_REG_IDX(RINTCNT)      }, /* Response Interrupt Count */
    { 0x0005C, 0x00001, 0x00000007, 0x00000007, hdaRegReadU8           , hdaRegWriteU8         , HDA_REG_IDX(RIRBCTL)      }, /* RIRB Control */
    { 0x0005D, 0x00001, 0x00000005, 0x00000005, hdaRegReadU8           , hdaRegWriteRIRBSTS    , HDA_REG_IDX(RIRBSTS)      }, /* RIRB Status */
    { 0x0005E, 0x00001, 0x000000F3, 0x00000000, hdaRegReadU8           , hdaRegWriteUnimpl     , HDA_REG_IDX(RIRBSIZE)     }, /* RIRB Size */
    { 0x00060, 0x00004, 0xFFFFFFFF, 0xFFFFFFFF, hdaRegReadU32          , hdaRegWriteU32        , HDA_REG_IDX(IC)           }, /* Immediate Command */
    { 0x00064, 0x00004, 0x00000000, 0xFFFFFFFF, hdaRegReadU32          , hdaRegWriteUnimpl     , HDA_REG_IDX(IR)           }, /* Immediate Response */
    { 0x00068, 0x00002, 0x00000002, 0x00000002, hdaRegReadIRS          , hdaRegWriteIRS        , HDA_REG_IDX(IRS)          }, /* Immediate Command Status */
    { 0x00070, 0x00004, 0xFFFFFFFF, 0xFFFFFF81, hdaRegReadU32          , hdaRegWriteBase       , HDA_REG_IDX(DPLBASE)      }, /* DMA Position Lower Base */
    { 0x00074, 0x00004, 0xFFFFFFFF, 0xFFFFFFFF, hdaRegReadU32          , hdaRegWriteBase       , HDA_REG_IDX(DPUBASE)      }, /* DMA Position Upper Base */
    /* 4 Input Stream Descriptors (ISD). */
    HDA_REG_MAP_DEF_STREAM(0, SD0),
    HDA_REG_MAP_DEF_STREAM(1, SD1),
    HDA_REG_MAP_DEF_STREAM(2, SD2),
    HDA_REG_MAP_DEF_STREAM(3, SD3),
    /* 4 Output Stream Descriptors (OSD). */
    HDA_REG_MAP_DEF_STREAM(4, SD4),
    HDA_REG_MAP_DEF_STREAM(5, SD5),
    HDA_REG_MAP_DEF_STREAM(6, SD6),
    HDA_REG_MAP_DEF_STREAM(7, SD7)
};

/**
 * HDA register aliases (HDA spec 3.3.45).
 * @remarks Sorted by offReg.
 */
static const struct
{
    /** The alias register offset. */
    uint32_t    offReg;
    /** The register index. */
    int         idxAlias;
} g_aHdaRegAliases[] =
{
    { 0x2084, HDA_REG_SD0LPIB },
    { 0x20a4, HDA_REG_SD1LPIB },
    { 0x20c4, HDA_REG_SD2LPIB },
    { 0x20e4, HDA_REG_SD3LPIB },
    { 0x2104, HDA_REG_SD4LPIB },
    { 0x2124, HDA_REG_SD5LPIB },
    { 0x2144, HDA_REG_SD6LPIB },
    { 0x2164, HDA_REG_SD7LPIB },
};

#ifdef IN_RING3
/** HDABDLEDESC field descriptors for the v7 saved state. */
static SSMFIELD const g_aSSMBDLEDescFields7[] =
{
    SSMFIELD_ENTRY(HDABDLEDESC, u64BufAdr),
    SSMFIELD_ENTRY(HDABDLEDESC, u32BufSize),
    SSMFIELD_ENTRY(HDABDLEDESC, fFlags),
    SSMFIELD_ENTRY_TERM()
};

/** HDABDLESTATE field descriptors for the v6 saved state. */
static SSMFIELD const g_aSSMBDLEStateFields6[] =
{
    SSMFIELD_ENTRY(HDABDLESTATE, u32BDLIndex),
    SSMFIELD_ENTRY(HDABDLESTATE, cbBelowFIFOW),
    SSMFIELD_ENTRY_OLD(FIFO,     HDA_FIFO_MAX), /* Deprecated; now is handled in the stream's circular buffer. */
    SSMFIELD_ENTRY(HDABDLESTATE, u32BufOff),
    SSMFIELD_ENTRY_TERM()
};

/** HDABDLESTATE field descriptors for the v7 saved state. */
static SSMFIELD const g_aSSMBDLEStateFields7[] =
{
    SSMFIELD_ENTRY(HDABDLESTATE, u32BDLIndex),
    SSMFIELD_ENTRY(HDABDLESTATE, cbBelowFIFOW),
    SSMFIELD_ENTRY(HDABDLESTATE, u32BufOff),
    SSMFIELD_ENTRY_TERM()
};

/** HDASTREAMSTATE field descriptors for the v6 saved state. */
static SSMFIELD const g_aSSMStreamStateFields6[] =
{
    SSMFIELD_ENTRY_OLD(cBDLE,      sizeof(uint16_t)), /* Deprecated. */
    SSMFIELD_ENTRY(HDASTREAMSTATE, uCurBDLE),
    SSMFIELD_ENTRY_OLD(fStop,      1),                /* Deprecated; see SSMR3PutBool(). */
    SSMFIELD_ENTRY(HDASTREAMSTATE, fRunning),
    SSMFIELD_ENTRY(HDASTREAMSTATE, fInReset),
    SSMFIELD_ENTRY_TERM()
};

/** HDASTREAMSTATE field descriptors for the v7 saved state. */
static SSMFIELD const g_aSSMStreamStateFields7[] =
{
    SSMFIELD_ENTRY(HDASTREAMSTATE, uCurBDLE),
    SSMFIELD_ENTRY(HDASTREAMSTATE, fInReset),
    SSMFIELD_ENTRY(HDASTREAMSTATE, uTimerTS),
    SSMFIELD_ENTRY_TERM()
};

/** HDASTREAMPERIOD field descriptors for the v7 saved state. */
static SSMFIELD const g_aSSMStreamPeriodFields7[] =
{
    SSMFIELD_ENTRY(HDASTREAMPERIOD, u64StartWalClk),
    SSMFIELD_ENTRY(HDASTREAMPERIOD, u64ElapsedWalClk),
    SSMFIELD_ENTRY(HDASTREAMPERIOD, framesTransferred),
    SSMFIELD_ENTRY(HDASTREAMPERIOD, cIntPending),
    SSMFIELD_ENTRY_TERM()
};
#endif

/**
 * 32-bit size indexed masks, i.e. g_afMasks[2 bytes] = 0xffff.
 */
static uint32_t const g_afMasks[5] =
{
    UINT32_C(0), UINT32_C(0x000000ff), UINT32_C(0x0000ffff), UINT32_C(0x00ffffff), UINT32_C(0xffffffff)
};

#ifdef IN_RING3
/**
 * Updates an HDA stream's current read or write buffer position (depending on the stream type) by
 * updating its associated LPIB register and DMA position buffer (if enabled).
 *
 * @param   pThis               HDA state.
 * @param   pStream             HDA stream to update read / write position for.
 * @param   u32LPIB             Absolute position (in bytes) to set current read / write position to.
 */
DECLINLINE(void) hdaStreamUpdateLPIB(PHDASTATE pThis, PHDASTREAM pStream, uint32_t u32LPIB)
{
    AssertPtrReturnVoid(pThis);
    AssertPtrReturnVoid(pStream);

    Assert(u32LPIB <= pStream->u32CBL);

    Log3Func(("[SD%RU8] LPIB=%RU32 (DMA Position Buffer Enabled: %RTbool)\n",
              pStream->u8SD, u32LPIB, pThis->fDMAPosition));

    /* Update LPIB in any case. */
    HDA_STREAM_REG(pThis, LPIB, pStream->u8SD) = u32LPIB;

    /* Do we need to tell the current DMA position? */
    if (pThis->fDMAPosition)
    {
        int rc2 = PDMDevHlpPCIPhysWrite(pThis->CTX_SUFF(pDevIns),
                                        pThis->u64DPBase + (pStream->u8SD * 2 * sizeof(uint32_t)),
                                        (void *)&u32LPIB, sizeof(uint32_t));
        AssertRC(rc2);
    }
}
#endif

#if defined(IN_RING3) || defined(LOG_ENABLED)
/**
 * Retrieves the number of bytes of a FIFOS register.
 *
 * @return Number of bytes of a given FIFOS register.
 */
DECLINLINE(uint16_t) hdaSDFIFOSToBytes(uint32_t u32RegFIFOS)
{
    uint16_t cb;
    switch (u32RegFIFOS)
    {
        /* Input */
        case HDA_SDINFIFO_120B: cb = 120; break;
        case HDA_SDINFIFO_160B: cb = 160; break;

        /* Output */
        case HDA_SDONFIFO_16B:  cb = 16;  break;
        case HDA_SDONFIFO_32B:  cb = 32;  break;
        case HDA_SDONFIFO_64B:  cb = 64;  break;
        case HDA_SDONFIFO_128B: cb = 128; break;
        case HDA_SDONFIFO_192B: cb = 192; break;
        case HDA_SDONFIFO_256B: cb = 256; break;
        default:
        {
            cb = 0; /* Can happen on stream reset. */
            break;
        }
    }

    return cb;
}
#endif

#if defined(IN_RING3)
/**
 * Retrieves the number of bytes of a FIFOW register.
 *
 * @return Number of bytes of a given FIFOW register.
 */
DECLINLINE(uint8_t) hdaSDFIFOWToBytes(uint32_t u32RegFIFOW)
{
    uint32_t cb;
    switch (u32RegFIFOW)
    {
        case HDA_SDFIFOW_8B:  cb = 8;  break;
        case HDA_SDFIFOW_16B: cb = 16; break;
        case HDA_SDFIFOW_32B: cb = 32; break;
        default:              cb = 0;  break;
    }

#ifdef RT_STRICT
    Assert(RT_IS_POWER_OF_TWO(cb));
#endif
    return cb;
}
#endif

DECLINLINE(PHDASTREAM) hdaStreamFromID(PHDASTATE pThis, uint8_t uSD)
{
    PHDASTREAM pStream;

    switch (uSD)
    {
        case 0: /** @todo Use dynamic indices, based on stream assignment. */
        {
            pStream = &pThis->LineIn;
            break;
        }
# ifdef VBOX_WITH_HDA_MIC_IN
        case 2: /** @todo Use dynamic indices, based on stream assignment. */
        {
            pStream = &pThis->MicIn;
            break;
        }
# endif
        case 4: /** @todo Use dynamic indices, based on stream assignment. */
        {
            pStream = &pThis->Out;
            break;
        }

        default:
        {
            pStream = NULL;
            LogFunc(("Stream with ID=%RU8 not handled\n", uSD));
            break;
        }
    }

    return pStream;
}

static uint32_t hdaGetINTSTS(PHDASTATE pThis)
{
    uint32_t intSts = 0;

    if (   HDA_REG_FLAG_VALUE(pThis, RIRBSTS, RIRBOIS)
        || HDA_REG_FLAG_VALUE(pThis, RIRBSTS, RINTFL)
        || HDA_REG_FLAG_VALUE(pThis, CORBSTS, CMEI))
    {
        intSts |= HDA_INTSTS_FLAG_CIS; /* Touch Controller Interrupt Status (CIS). */
    }

    if (HDA_REG(pThis, STATESTS) & HDA_REG(pThis, WAKEEN))
    {
        intSts |= HDA_INTSTS_FLAG_CIS; /* Touch Controller Interrupt Status (CIS). */
    }

    Log3Func(("RIRBOIS=0x%x, RIRBSTS=0x%x, STATESTS=0x%x\n",
              HDA_REG_FLAG_VALUE(pThis, RIRBSTS, RIRBOIS), HDA_REG_FLAG_VALUE(pThis, RIRBSTS, RINTFL),
              (HDA_REG(pThis, STATESTS) & HDA_STATESTS_SCSF_MASK)));

#define HDA_MARK_SIS(x) /* Stream Interrupt Status (SIS). */                \
        if (   (INTCTL_SX(pThis, x))                                        \
            && (   (SDSTS(pThis, x) & HDA_REG_FIELD_FLAG_MASK(SDSTS, DE))   \
                || (SDSTS(pThis, x) & HDA_REG_FIELD_FLAG_MASK(SDSTS, FE))   \
                || (SDSTS(pThis, x) & HDA_REG_FIELD_FLAG_MASK(SDSTS, BCIS)) \
               )                                                            \
           )                                                                \
    {                                                                       \
        Log3Func(("[SD%RU8] Marked %R[sdsts]\n", x, SDSTS(pThis, x)));      \
        intSts |= RT_BIT(x);                                                \
    }

    HDA_MARK_SIS(0);
    HDA_MARK_SIS(1);
    HDA_MARK_SIS(2);
    HDA_MARK_SIS(3);
    HDA_MARK_SIS(4);
    HDA_MARK_SIS(5);
    HDA_MARK_SIS(6);
    HDA_MARK_SIS(7);

#undef HDA_MARK_SIS

    if (   (intSts & HDA_INTSTS_FLAG_CIS)
        || (intSts & HDA_INTSTS_MASK_SIS))
    {
        intSts |= HDA_INTSTS_FLAG_GIS; /* Touch Global Interrupt Status (GIS). */
    }

    Log3Func(("-> 0x%x\n", intSts));

    return intSts;
}

#ifndef DEBUG
static int hdaProcessInterrupt(PHDASTATE pThis)
#else
static int hdaProcessInterrupt(PHDASTATE pThis, const char *pszSource)
#endif
{
    HDA_REG(pThis, INTSTS) = hdaGetINTSTS(pThis);

    Log3Func(("IRQL=%RU8\n", pThis->u8IRQL));

    /* Global Interrupt Enable (GIE) set? */
    if (   HDA_REG_FLAG_VALUE(pThis, INTCTL, GIE)
        && ((HDA_REG(pThis, INTSTS) & ~HDA_INTSTS_FLAG_GIS) & HDA_REG(pThis, INTCTL)))
    {
        if (!pThis->u8IRQL)
        {
#ifdef DEBUG
            if (!pThis->Dbg.IRQ.tsProcessedLastNs)
                pThis->Dbg.IRQ.tsProcessedLastNs = RTTimeNanoTS();

            const uint64_t tsLastElapsedNs = RTTimeNanoTS() - pThis->Dbg.IRQ.tsProcessedLastNs;

            if (!pThis->Dbg.IRQ.tsAssertedNs)
                pThis->Dbg.IRQ.tsAssertedNs = RTTimeNanoTS();

            const uint64_t tsAssertedElapsedNs = RTTimeNanoTS() - pThis->Dbg.IRQ.tsAssertedNs;

            pThis->Dbg.IRQ.cAsserted++;
            pThis->Dbg.IRQ.tsAssertedTotalNs += tsAssertedElapsedNs;

            const uint64_t avgAssertedUs = (pThis->Dbg.IRQ.tsAssertedTotalNs / pThis->Dbg.IRQ.cAsserted) / 1000;

            if (avgAssertedUs > (1000 / HDA_TIMER_HZ) /* ms */ * 1000) /* Exceeds time slot? */
                Log3Func(("Asserted (%s): %zuus elapsed (%zuus on average) -- %zuus alternation delay\n",
                          pszSource, tsAssertedElapsedNs / 1000,
                          avgAssertedUs,
                          (pThis->Dbg.IRQ.tsDeassertedNs - pThis->Dbg.IRQ.tsAssertedNs) / 1000));
#endif
            Log3Func(("Asserted (%s): %RU64us between alternation (WALCLK=%RU64)\n",
                      pszSource, tsLastElapsedNs / 1000, pThis->u64WalClk));

            PDMDevHlpPCISetIrq(pThis->CTX_SUFF(pDevIns), 0, 1 /* Assert */);
            pThis->u8IRQL = 1;

#ifdef DEBUG
            pThis->Dbg.IRQ.tsAssertedNs = RTTimeNanoTS();
            pThis->Dbg.IRQ.tsProcessedLastNs = pThis->Dbg.IRQ.tsAssertedNs;
#endif
        }
    }
    else
    {
        if (pThis->u8IRQL)
        {
#ifdef DEBUG
            if (!pThis->Dbg.IRQ.tsProcessedLastNs)
                pThis->Dbg.IRQ.tsProcessedLastNs = RTTimeNanoTS();

            const uint64_t tsLastElapsedNs = RTTimeNanoTS() - pThis->Dbg.IRQ.tsProcessedLastNs;

            if (!pThis->Dbg.IRQ.tsDeassertedNs)
                pThis->Dbg.IRQ.tsDeassertedNs = RTTimeNanoTS();

            const uint64_t tsDeassertedElapsedNs = RTTimeNanoTS() - pThis->Dbg.IRQ.tsDeassertedNs;

            pThis->Dbg.IRQ.cDeasserted++;
            pThis->Dbg.IRQ.tsDeassertedTotalNs += tsDeassertedElapsedNs;

            const uint64_t avgDeassertedUs = (pThis->Dbg.IRQ.tsDeassertedTotalNs / pThis->Dbg.IRQ.cDeasserted) / 1000;

            if (avgDeassertedUs > (1000 / HDA_TIMER_HZ) /* ms */ * 1000) /* Exceeds time slot? */
                Log3Func(("Deasserted (%s): %zuus elapsed (%zuus on average)\n",
                          pszSource, tsDeassertedElapsedNs / 1000, avgDeassertedUs));

            Log3Func(("Deasserted (%s): %RU64us between alternation (WALCLK=%RU64)\n",
                      pszSource, tsLastElapsedNs / 1000, pThis->u64WalClk));
#endif
            PDMDevHlpPCISetIrq(pThis->CTX_SUFF(pDevIns), 0, 0 /* Deassert */);
            pThis->u8IRQL = 0;

#ifdef DEBUG
            pThis->Dbg.IRQ.tsDeassertedNs    = RTTimeNanoTS();
            pThis->Dbg.IRQ.tsProcessedLastNs = pThis->Dbg.IRQ.tsDeassertedNs;
#endif
        }
    }

    return VINF_SUCCESS;
}

#ifdef IN_RING3
/**
 * Reschedules pending interrupts for all audio streams which have complete
 * audio periods but did not have the chance to issue their (pending) interrupts yet.
 *
 * @param   pThis               The HDA device state.
 */
static void hdaReschedulePendingInterrupts(PHDASTATE pThis)
{
    bool fInterrupt = false;

#define HDA_STREAM_MARK_INT(_aStream) \
    if (   hdaStreamPeriodIsComplete(&_aStream.State.Period) \
        && hdaStreamPeriodNeedsInterrupt(&_aStream.State.Period) \
        && hdaWalClkSet(pThis, hdaStreamPeriodGetAbsElapsedWalClk(&_aStream.State.Period), false /* fForce */)) \
    { \
        fInterrupt = true; \
    }

    HDA_STREAM_MARK_INT(pThis->Out);
    HDA_STREAM_MARK_INT(pThis->LineIn);
#ifdef VBOX_WITH_HDA_MIC_IN
    HDA_STREAM_MARK_INT(pThis->MicIn);
#endif

#undef HDA_STREAM_MARK_INT

    LogFunc(("fInterrupt=%RTbool\n", fInterrupt));

    if (fInterrupt)
    {
#ifndef DEBUG
        hdaProcessInterrupt(pThis);
#else
        hdaProcessInterrupt(pThis, __FUNCTION__);
#endif
    }
}
#endif

/**
 * Looks up a register at the exact offset given by @a offReg.
 *
 * @returns Register index on success, -1 if not found.
 * @param   pThis               The HDA device state.
 * @param   offReg              The register offset.
 */
static int hdaRegLookup(PHDASTATE pThis, uint32_t offReg)
{
    RT_NOREF(pThis);

    /*
     * Aliases.
     */
    if (offReg >= g_aHdaRegAliases[0].offReg)
    {
        for (unsigned i = 0; i < RT_ELEMENTS(g_aHdaRegAliases); i++)
            if (offReg == g_aHdaRegAliases[i].offReg)
                return g_aHdaRegAliases[i].idxAlias;
        Assert(g_aHdaRegMap[RT_ELEMENTS(g_aHdaRegMap) - 1].offset < offReg);
        return -1;
    }

    /*
     * Binary search the
     */
    int idxEnd  = RT_ELEMENTS(g_aHdaRegMap);
    int idxLow  = 0;
    for (;;)
    {
        int idxMiddle = idxLow + (idxEnd - idxLow) / 2;
        if (offReg < g_aHdaRegMap[idxMiddle].offset)
        {
            if (idxLow == idxMiddle)
                break;
            idxEnd = idxMiddle;
        }
        else if (offReg > g_aHdaRegMap[idxMiddle].offset)
        {
            idxLow = idxMiddle + 1;
            if (idxLow >= idxEnd)
                break;
        }
        else
            return idxMiddle;
    }

#ifdef RT_STRICT
    for (unsigned i = 0; i < RT_ELEMENTS(g_aHdaRegMap); i++)
        Assert(g_aHdaRegMap[i].offset != offReg);
#endif
    return -1;
}

/**
 * Looks up a register covering the offset given by @a offReg.
 *
 * @returns Register index on success, -1 if not found.
 * @param   pThis               The HDA device state.
 * @param   offReg              The register offset.
 */
static int hdaRegLookupWithin(PHDASTATE pThis, uint32_t offReg)
{
     RT_NOREF(pThis);

    /*
     * Aliases.
     */
    if (offReg >= g_aHdaRegAliases[0].offReg)
    {
        for (unsigned i = 0; i < RT_ELEMENTS(g_aHdaRegAliases); i++)
        {
            uint32_t off = offReg - g_aHdaRegAliases[i].offReg;
            if (off < 4 && off < g_aHdaRegMap[g_aHdaRegAliases[i].idxAlias].size)
                return g_aHdaRegAliases[i].idxAlias;
        }
        Assert(g_aHdaRegMap[RT_ELEMENTS(g_aHdaRegMap) - 1].offset < offReg);
        return -1;
    }

    /*
     * Binary search the register map.
     */
    int idxEnd  = RT_ELEMENTS(g_aHdaRegMap);
    int idxLow  = 0;
    for (;;)
    {
        int idxMiddle = idxLow + (idxEnd - idxLow) / 2;
        if (offReg < g_aHdaRegMap[idxMiddle].offset)
        {
            if (idxLow == idxMiddle)
                break;
            idxEnd = idxMiddle;
        }
        else if (offReg >= g_aHdaRegMap[idxMiddle].offset + g_aHdaRegMap[idxMiddle].size)
        {
            idxLow = idxMiddle + 1;
            if (idxLow >= idxEnd)
                break;
        }
        else
            return idxMiddle;
    }

#ifdef RT_STRICT
    for (unsigned i = 0; i < RT_ELEMENTS(g_aHdaRegMap); i++)
        Assert(offReg - g_aHdaRegMap[i].offset >= g_aHdaRegMap[i].size);
#endif
    return -1;
}

#ifdef IN_RING3

static int hdaCmdSync(PHDASTATE pThis, bool fLocal)
{
    int rc = VINF_SUCCESS;
    if (fLocal)
    {
        Assert((HDA_REG_FLAG_VALUE(pThis, CORBCTL, DMA)));
        Assert(pThis->u64CORBBase);
        AssertPtr(pThis->pu32CorbBuf);
        Assert(pThis->cbCorbBuf);

        rc = PDMDevHlpPhysRead(pThis->CTX_SUFF(pDevIns), pThis->u64CORBBase, pThis->pu32CorbBuf, pThis->cbCorbBuf);
        if (RT_FAILURE(rc))
            AssertRCReturn(rc, rc);
#ifdef DEBUG_CMD_BUFFER
        uint8_t i = 0;
        do
        {
            LogFunc(("CORB%02x: ", i));
            uint8_t j = 0;
            do
            {
                const char *pszPrefix;
                if ((i + j) == HDA_REG(pThis, CORBRP));
                    pszPrefix = "[R]";
                else if ((i + j) == HDA_REG(pThis, CORBWP));
                    pszPrefix = "[W]";
                else
                    pszPrefix = "   "; /* three spaces */
                LogFunc(("%s%08x", pszPrefix, pThis->pu32CorbBuf[i + j]));
                j++;
            } while (j < 8);
            LogFunc(("\n"));
            i += 8;
        } while(i != 0);
#endif
    }
    else
    {
        Assert((HDA_REG_FLAG_VALUE(pThis, RIRBCTL, DMA)));
        rc = PDMDevHlpPCIPhysWrite(pThis->CTX_SUFF(pDevIns), pThis->u64RIRBBase, pThis->pu64RirbBuf, pThis->cbRirbBuf);
        if (RT_FAILURE(rc))
            AssertRCReturn(rc, rc);
#ifdef DEBUG_CMD_BUFFER
        uint8_t i = 0;
        do {
            LogFunc(("RIRB%02x: ", i));
            uint8_t j = 0;
            do {
                const char *prefix;
                if ((i + j) == HDA_REG(pThis, RIRBWP))
                    prefix = "[W]";
                else
                    prefix = "   ";
                LogFunc((" %s%016lx", prefix, pThis->pu64RirbBuf[i + j]));
            } while (++j < 8);
            LogFunc(("\n"));
            i += 8;
        } while (i != 0);
#endif
    }
    return rc;
}

static int hdaCORBCmdProcess(PHDASTATE pThis)
{
    PFNHDACODECVERBPROCESSOR pfn = (PFNHDACODECVERBPROCESSOR)NULL;

    int rc = hdaCmdSync(pThis, true);
    if (RT_FAILURE(rc))
        AssertRCReturn(rc, rc);

    uint8_t corbRp = HDA_REG(pThis, CORBRP);
    uint8_t corbWp = HDA_REG(pThis, CORBWP);
    uint8_t rirbWp = HDA_REG(pThis, RIRBWP);

    Assert((corbWp != corbRp));

    Log3Func(("CORB(RP:%x, WP:%x) RIRBWP:%x\n",
              HDA_REG(pThis, CORBRP), HDA_REG(pThis, CORBWP), HDA_REG(pThis, RIRBWP)));

    while (corbRp != corbWp)
    {
        uint32_t cmd;
        uint64_t resp = 0;
        pfn = NULL;
        corbRp++;
        cmd = pThis->pu32CorbBuf[corbRp];

        rc = pThis->pCodec->pfnLookup(pThis->pCodec, HDA_CODEC_CMD(cmd, 0 /* Codec index */), &pfn);
        if (RT_SUCCESS(rc))
        {
            AssertPtr(pfn);
            rc = pfn(pThis->pCodec, HDA_CODEC_CMD(cmd, 0 /* LUN */), &resp);
        }

        if (RT_FAILURE(rc))
            AssertRCReturn(rc, rc);
        (rirbWp)++;

        LogFunc(("verb:%08x->%016lx\n", cmd, resp));
        if (   (resp & CODEC_RESPONSE_UNSOLICITED)
            && !HDA_REG_FLAG_VALUE(pThis, GCTL, UR))
        {
            LogFunc(("unexpected unsolicited response.\n"));
            HDA_REG(pThis, CORBRP) = corbRp;
            return rc;
        }

        pThis->pu64RirbBuf[rirbWp] = resp;

        pThis->u8RespIntCnt++;
        if (pThis->u8RespIntCnt == RINTCNT_N(pThis))
            break;
    }

    HDA_REG(pThis, CORBRP) = corbRp;
    HDA_REG(pThis, RIRBWP) = rirbWp;

    rc = hdaCmdSync(pThis, false);

    Log3Func(("CORB(RP:%x, WP:%x) RIRBWP:%x\n",
              HDA_REG(pThis, CORBRP), HDA_REG(pThis, CORBWP), HDA_REG(pThis, RIRBWP)));

    if (HDA_REG_FLAG_VALUE(pThis, RIRBCTL, RIC)) /* Response Interrupt Control (RIC) enabled? */
    {
        if (pThis->u8RespIntCnt)
        {
            pThis->u8RespIntCnt = 0;

            HDA_REG(pThis, RIRBSTS) |= HDA_REG_FIELD_FLAG_MASK(RIRBSTS, RINTFL);

#ifndef DEBUG
            rc = hdaProcessInterrupt(pThis);
#else
            rc = hdaProcessInterrupt(pThis, __FUNCTION__);
#endif
        }
    }

    if (RT_FAILURE(rc))
        AssertRCReturn(rc, rc);

    return rc;
}

static int hdaStreamCreate(PHDASTREAM pStream)
{
    AssertPtrReturn(pStream, VERR_INVALID_POINTER);

    pStream->u8SD           = UINT8_MAX;

    pStream->State.fRunning = false;
    pStream->State.fInReset = false;

    int rc = RTCircBufCreate(&pStream->State.pCircBuf, _64K); /** @todo Make this configurable. */

#ifdef HDA_USE_DMA_ACCESS_HANDLER
    RTListInit(&pStream->State.lstDMAHandlers);
#endif

    if (RT_SUCCESS(rc))
        rc = hdaStreamPeriodCreate(&pStream->State.Period);

#ifdef DEBUG
    int rc2 = RTCritSectInit(&pStream->Dbg.CritSect);
    AssertRC(rc2);
#endif

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static void hdaStreamDestroy(PHDASTREAM pStream)
{
    AssertPtrReturnVoid(pStream);

    LogFlowFunc(("[SD%RU8] Destroy\n", pStream->u8SD));

    if (pStream->State.pCircBuf)
    {
        RTCircBufDestroy(pStream->State.pCircBuf);
        pStream->State.pCircBuf = NULL;
    }

    hdaStreamPeriodDestroy(&pStream->State.Period);

#ifdef DEBUG
    int rc2 = RTCritSectDelete(&pStream->Dbg.CritSect);
    AssertRC(rc2);
#endif

    LogFlowFuncLeave();
}

static int hdaStreamInit(PHDASTATE pThis, PHDASTREAM pStream, uint8_t uSD)
{
    AssertPtrReturn(pThis,   VERR_INVALID_POINTER);
    AssertPtrReturn(pStream, VERR_INVALID_POINTER);

    pStream->u8SD       = uSD;
    pStream->u64BDLBase = RT_MAKE_U64(HDA_STREAM_REG(pThis, BDPL, pStream->u8SD),
                                      HDA_STREAM_REG(pThis, BDPU, pStream->u8SD));
    pStream->u16LVI     = HDA_STREAM_REG(pThis, LVI, pStream->u8SD);
    pStream->u32CBL     = HDA_STREAM_REG(pThis, CBL, pStream->u8SD);
    pStream->u16FIFOS   = hdaSDFIFOSToBytes(HDA_STREAM_REG(pThis, FIFOS, pStream->u8SD));

    /* Make sure to also update the stream's DMA counter (base on its current LPIB value). */
    hdaStreamUpdateLPIB(pThis, pStream, HDA_STREAM_REG(pThis, LPIB, pStream->u8SD));

    int rc = hdaSDFMTToStrmCfg(HDA_STREAM_REG(pThis, FMT, uSD), &pStream->State.strmCfg);
    if (RT_FAILURE(rc))
        LogRel(("HDA: Warning: Format 0x%x for stream #%RU8 not supported\n", HDA_STREAM_REG(pThis, FMT, uSD), uSD));

    LogFunc(("[SD%RU8] DMA @ 0x%x (%RU32 bytes), LVI=%RU16, FIFOS=%RU16, rc=%Rrc\n",
             pStream->u8SD, pStream->u64BDLBase, pStream->u32CBL, pStream->u16LVI, pStream->u16FIFOS, rc));

    return rc;
}

/**
 * Resets an HDA stream.
 *
 * @param   pThis               HDA state.
 * @param   pStream             HDA stream to reset.
 * @param   uSD                 Stream descriptor (SD) number to use for this stream.
 */
static void hdaStreamReset(PHDASTATE pThis, PHDASTREAM pStream, uint8_t uSD)
{
    AssertPtrReturnVoid(pThis);
    AssertPtrReturnVoid(pStream);
    AssertReturnVoid(uSD <= 7); /** @todo Use a define for MAX_STREAMS! */

#ifdef VBOX_STRICT
    AssertReleaseMsg(!RT_BOOL(HDA_STREAM_REG(pThis, CTL, uSD) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN)),
                     ("Cannot reset stream %RU8 while in running state\n", uSD));
#endif

    LogFunc(("[SD%RU8]: Reset\n", uSD));

    /*
     * Set reset state.
     */
    Assert(ASMAtomicReadBool(&pStream->State.fInReset) == false); /* No nested calls. */
    ASMAtomicXchgBool(&pStream->State.fInReset, true);
    ASMAtomicXchgBool(&pStream->State.fRunning, false);

    /*
     * Initialize the registers.
     */

    HDA_STREAM_REG(pThis, STS,   uSD) = HDA_REG_FIELD_FLAG_MASK(SDSTS, FIFORDY);
    /* According to the ICH6 datasheet, 0x40000 is the default value for stream descriptor register 23:20
     * bits are reserved for stream number 18.2.33, resets SDnCTL except SRST bit. */
    HDA_STREAM_REG(pThis, CTL,   uSD) = 0x40000 | (HDA_STREAM_REG(pThis, CTL, uSD) & HDA_REG_FIELD_FLAG_MASK(SDCTL, SRST));
    /* ICH6 defines default values (120 bytes for input and 192 bytes for output descriptors) of FIFO size. 18.2.39. */
    HDA_STREAM_REG(pThis, FIFOS, uSD) = uSD < 4 ? HDA_SDINFIFO_120B : HDA_SDONFIFO_192B;
    /* See 18.2.38: Always defaults to 0x4 (32 bytes). */
    HDA_STREAM_REG(pThis, FIFOW, uSD) = HDA_SDFIFOW_32B;
    HDA_STREAM_REG(pThis, LPIB,  uSD) = 0;
    HDA_STREAM_REG(pThis, CBL,   uSD) = 0;
    HDA_STREAM_REG(pThis, LVI,   uSD) = 0;
    HDA_STREAM_REG(pThis, FMT,   uSD) = 0;
    HDA_STREAM_REG(pThis, BDPU,  uSD) = 0;
    HDA_STREAM_REG(pThis, BDPL,  uSD) = 0;

#ifdef HDA_USE_DMA_ACCESS_HANDLER
    hdaStreamUnregisterDMAHandlers(pThis, pStream);
#endif

    RT_ZERO(pStream->State.BDLE);
    pStream->State.uCurBDLE = 0;

    if (pStream->State.pCircBuf)
        RTCircBufReset(pStream->State.pCircBuf);

    /* (Re-)initialize the stream with current values. */
    int rc2 = hdaStreamInit(pThis, pStream, uSD);
    AssertRC(rc2);

    /* Reset the stream's period. */
    hdaStreamPeriodReset(&pStream->State.Period);

#ifdef DEBUG
    pStream->Dbg.cReadsTotal      = 0;
    pStream->Dbg.cbReadTotal      = 0;
    pStream->Dbg.tsLastReadNs     = 0;
    pStream->Dbg.cWritesTotal     = 0;
    pStream->Dbg.cbWrittenTotal   = 0;
    pStream->Dbg.cWritesHz        = 0;
    pStream->Dbg.cbWrittenHz      = 0;
    pStream->Dbg.tsWriteSlotBegin = 0;
#endif

    /* Report that we're done resetting this stream. */
    HDA_STREAM_REG(pThis, CTL,   uSD) = 0;

    LogFunc(("[SD%RU8] Reset\n", uSD));

    /* Exit reset mode. */
    ASMAtomicXchgBool(&pStream->State.fInReset, false);
}
#endif /* IN_RING3 */

/* Register access handlers. */

static int hdaRegReadUnimpl(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
    RT_NOREF(pThis, iReg);
    *pu32Value = 0;
    return VINF_SUCCESS;
}

static int hdaRegWriteUnimpl(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    RT_NOREF(pThis, iReg, u32Value);
    return VINF_SUCCESS;
}

/* U8 */
static int hdaRegReadU8(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
    Assert(((pThis->au32Regs[g_aHdaRegMap[iReg].mem_idx] & g_aHdaRegMap[iReg].readable) & 0xffffff00) == 0);
    return hdaRegReadU32(pThis, iReg, pu32Value);
}

static int hdaRegWriteU8(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    Assert((u32Value & 0xffffff00) == 0);
    return hdaRegWriteU32(pThis, iReg, u32Value);
}

/* U16 */
static int hdaRegReadU16(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
    Assert(((pThis->au32Regs[g_aHdaRegMap[iReg].mem_idx] & g_aHdaRegMap[iReg].readable) & 0xffff0000) == 0);
    return hdaRegReadU32(pThis, iReg, pu32Value);
}

static int hdaRegWriteU16(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    Assert((u32Value & 0xffff0000) == 0);
    return hdaRegWriteU32(pThis, iReg, u32Value);
}

/* U24 */
static int hdaRegReadU24(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
    Assert(((pThis->au32Regs[g_aHdaRegMap[iReg].mem_idx] & g_aHdaRegMap[iReg].readable) & 0xff000000) == 0);
    return hdaRegReadU32(pThis, iReg, pu32Value);
}

static int hdaRegWriteU24(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    Assert((u32Value & 0xff000000) == 0);
    return hdaRegWriteU32(pThis, iReg, u32Value);
}

/* U32 */
static int hdaRegReadU32(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
    uint32_t iRegMem = g_aHdaRegMap[iReg].mem_idx;

    *pu32Value = pThis->au32Regs[iRegMem] & g_aHdaRegMap[iReg].readable;
    return VINF_SUCCESS;
}

static int hdaRegWriteU32(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    uint32_t iRegMem = g_aHdaRegMap[iReg].mem_idx;

    pThis->au32Regs[iRegMem]  = (u32Value & g_aHdaRegMap[iReg].writable)
                              | (pThis->au32Regs[iRegMem] & ~g_aHdaRegMap[iReg].writable);
    return VINF_SUCCESS;
}

static int hdaRegWriteGCTL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    RT_NOREF(iReg);
    if (u32Value & HDA_REG_FIELD_FLAG_MASK(GCTL, RST))
    {
        /* Set the CRST bit to indicate that we're leaving reset mode. */
        HDA_REG(pThis, GCTL) |= HDA_REG_FIELD_FLAG_MASK(GCTL, RST);

        if (pThis->fInReset)
        {
            LogFunc(("Leaving reset\n"));
            pThis->fInReset = false;
        }
    }
    else
    {
#ifdef IN_RING3
        /* Enter reset state. */
        if (   HDA_REG_FLAG_VALUE(pThis, CORBCTL, DMA)
            || HDA_REG_FLAG_VALUE(pThis, RIRBCTL, DMA))
        {
            LogFunc(("Entering reset with DMA(RIRB:%s, CORB:%s)\n",
                     HDA_REG_FLAG_VALUE(pThis, CORBCTL, DMA) ? "on" : "off",
                     HDA_REG_FLAG_VALUE(pThis, RIRBCTL, DMA) ? "on" : "off"));
        }

        /* Clear the CRST bit to indicate that we're in reset mode. */
        HDA_REG(pThis, GCTL) &= ~HDA_REG_FIELD_FLAG_MASK(GCTL, RST);
        pThis->fInReset = true;

        /* As the CRST bit now is set, we now can proceed resetting stuff. */
        hdaReset(pThis->CTX_SUFF(pDevIns));
#else
        return VINF_IOM_R3_MMIO_WRITE;
#endif
    }
    if (u32Value & HDA_REG_FIELD_FLAG_MASK(GCTL, FSH))
    {
        /* Flush: GSTS:1 set, see 6.2.6. */
        HDA_REG(pThis, GSTS) |= HDA_REG_FIELD_FLAG_MASK(GSTS, FSH); /* Set the flush state. */
        /* DPLBASE and DPUBASE should be initialized with initial value (see 6.2.6). */
    }
    return VINF_SUCCESS;
}

static int hdaRegWriteSTATESTS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    uint32_t iRegMem = g_aHdaRegMap[iReg].mem_idx;

    uint32_t v = pThis->au32Regs[iRegMem];
    uint32_t nv = u32Value & HDA_STATESTS_SCSF_MASK;
    pThis->au32Regs[iRegMem] &= ~(v & nv); /* write of 1 clears corresponding bit */
    return VINF_SUCCESS;
}

static int hdaRegReadINTSTS(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
#ifdef IN_RING3
    RT_NOREF(iReg);

    *pu32Value = HDA_REG(pThis, INTSTS);

    return VINF_SUCCESS;
#else
    RT_NOREF(pThis, iReg, pu32Value);
    return VINF_IOM_R3_MMIO_WRITE;
#endif
}

static int hdaRegReadLPIB(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
    const uint8_t  uSD     = HDA_SD_NUM_FROM_REG(pThis, LPIB, iReg);
          uint32_t u32LPIB = HDA_STREAM_REG(pThis, LPIB, uSD);

#if defined(LOG_ENABLED)
    const uint32_t u32CBL  = HDA_STREAM_REG(pThis, CBL, uSD);
    LogFlowFunc(("[SD%RU8] LPIB=%RU32, CBL=%RU32\n", uSD, u32LPIB, u32CBL));
#endif

    *pu32Value = u32LPIB;
    return VINF_SUCCESS;
}

/**
 * Retrieves the currently set value for the wall clock.
 *
 * @return  IPRT status code.
 * @return  Currently set wall clock value.
 * @param   pThis               HDA state.
 *
 * @remark  Operation is atomic.
 */
uint64_t hdaWalClkGetCurrent(PHDASTATE pThis)
{
    return ASMAtomicReadU64(&pThis->u64WalClk);
}

#ifdef IN_RING3
/**
 * Returns the current maximum value the wall clock counter can be set to.
 * This maximum value depends on all currently handled HDA streams and their own current timing.
 *
 * @return  Current maximum value the wall clock counter can be set to.
 * @param   pThis               HDA state.
 *
 * @remark  Does not actually set the wall clock counter.
 */
uint64_t hdaWalClkGetMax(PHDASTATE pThis)
{
    const uint64_t u64WalClkCur       = ASMAtomicReadU64(&pThis->u64WalClk);
    const uint64_t u64OutAbsWalClk    = hdaStreamPeriodGetAbsElapsedWalClk(&pThis->Out.State.Period);
    const uint64_t u64LineInAbsWalClk = hdaStreamPeriodGetAbsElapsedWalClk(&pThis->LineIn.State.Period);
#ifdef VBOX_WITH_HDA_MIC_IN
    const uint64_t u64MicInAbsWalClk  = hdaStreamPeriodGetAbsElapsedWalClk(&pThis->MicIn.State.Period));
#endif

    uint64_t u64WalClkNew = RT_MAX(u64WalClkCur, u64OutAbsWalClk);
             u64WalClkNew = RT_MAX(u64WalClkNew, u64LineInAbsWalClk);
#ifdef VBOX_WITH_HDA_MIC_IN
             u64WalClkNew = RT_MAX(u64WalClkNew, u64MicInAbsWalClk);
#endif

    Log3Func(("%RU64 -> Out=%RU64, LineIn=%RU64 -> %RU64\n",
              u64WalClkCur, u64OutAbsWalClk, u64LineInAbsWalClk, u64WalClkNew));

    return u64WalClkNew;
}

/**
 * Sets the actual WALCLK register to the specified wall clock value.
 * The specified wall clock value only will be set (unless fForce is set to @true) if all
 * handled HDA streams have passed (in time) that value. This guarantees that the WALCLK
 * register stays in sync with all handled HDA streams.
 *
 * @return  @true if the WALCLK register has been updated, @false if not.
 * @param   pThis               HDA state.
 * @param   u64WalClk           Wall clock value to set WALCLK register to.
 * @param   fForce              Whether to force setting the wall clock value or not.
 */
bool hdaWalClkSet(PHDASTATE pThis, uint64_t u64WalClk, bool fForce)
{
    const bool     fOutPassed         = hdaStreamPeriodHasPassedAbsWalClk (&pThis->Out.State.Period,    u64WalClk);
    const uint64_t u64OutAbsWalClk    = hdaStreamPeriodGetAbsElapsedWalClk(&pThis->Out.State.Period);
    const bool     fLineInPassed      = hdaStreamPeriodHasPassedAbsWalClk (&pThis->LineIn.State.Period, u64WalClk);
    const uint64_t u64LineInAbsWalClk = hdaStreamPeriodGetAbsElapsedWalClk(&pThis->LineIn.State.Period);
#ifdef VBOX_WITH_HDA_MIC_IN
    const bool     fMicInPassed       = hdaStreamPeriodHasPassedAbsWalClk (&pThis->MicIn.State.Period,  u64WalClk);
    const uint64_t u64MicInAbsWalClk  = hdaStreamPeriodGetAbsElapsedWalClk(&pThis->MicIn.State.Period));
#endif

#ifdef VBOX_STRICT
    const uint64_t u64WalClkCur = ASMAtomicReadU64(&pThis->u64WalClk);
#endif
          uint64_t u64WalClkSet = u64WalClk;

    /* Only drive the WALCLK register forward if all (active) stream periods have passed
     * the specified point in time given by u64WalClk. */
    if (  (   fOutPassed
           && fLineInPassed
#ifdef VBOX_WITH_HDA_MIC_IN
           && fMicInPassed
#endif
          )
       || fForce)
    {
        if (!fForce)
        {
            /* Get the maximum value of all periods we need to handle.
             * Not the most elegant solution, but works for now ... */
            u64WalClk = RT_MAX(u64WalClkSet, u64OutAbsWalClk);
            u64WalClk = RT_MAX(u64WalClkSet, u64LineInAbsWalClk);
#ifdef VBOX_WITH_HDA_MIC_IN
            u64WalClk = RT_MAX(u64WalClkSet, u64MicInAbsWalClk);
#endif
            AssertMsg(u64WalClkSet > u64WalClkCur,
                      ("Setting WALCLK to a stale or backward value (%RU64 -> %RU64) isn't a good idea really. "
                       "Good luck with stuck audio stuff.\n", u64WalClkCur, u64WalClkSet));
        }

        /* Set the new WALCLK value. */
        ASMAtomicWriteU64(&pThis->u64WalClk, u64WalClkSet);
    }

    const uint64_t u64WalClkNew = hdaWalClkGetCurrent(pThis);

    Log3Func(("Cur: %RU64, New: %RU64 (force %RTbool) -> %RU64 %s\n",
              u64WalClkCur, u64WalClk, fForce,
              u64WalClkNew, u64WalClkNew == u64WalClk ? "[OK]" : "[DELAYED]"));

    return (u64WalClkNew == u64WalClk);
}
#endif /* IN_RING3 */

static int hdaRegReadWALCLK(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
#ifdef IN_RING3
    RT_NOREF(iReg);

    *pu32Value = RT_LO_U32(ASMAtomicReadU64(&pThis->u64WalClk));

    Log3Func(("%RU32 (max @ %RU64)\n",*pu32Value, hdaWalClkGetMax(pThis)));

    return VINF_SUCCESS;
#else
    RT_NOREF(pThis, iReg, pu32Value);
    return VINF_IOM_R3_MMIO_WRITE;
#endif
}

static int hdaRegWriteCORBRP(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    RT_NOREF(iReg);
    if (u32Value & HDA_REG_FIELD_FLAG_MASK(CORBRP, RST))
        HDA_REG(pThis, CORBRP) = HDA_CORBRP_RST;    /* Clears the pointer. */
    else
        HDA_REG(pThis, CORBRP) &= ~HDA_CORBRP_RST;  /* Only CORBRP_RST bit is writable. */

    return VINF_SUCCESS;
}

static int hdaRegWriteCORBCTL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
#ifdef IN_RING3
    int rc = hdaRegWriteU8(pThis, iReg, u32Value);
    AssertRC(rc);
    if (   HDA_REG(pThis, CORBWP)                  != HDA_REG(pThis, CORBRP)
        && HDA_REG_FLAG_VALUE(pThis, CORBCTL, DMA) != 0)
    {
        return hdaCORBCmdProcess(pThis);
    }
    return rc;
#else
    RT_NOREF(pThis, iReg, u32Value);
    return VINF_IOM_R3_MMIO_WRITE;
#endif
}

static int hdaRegWriteCORBSTS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    RT_NOREF(iReg);
    uint32_t v = HDA_REG(pThis, CORBSTS);
    HDA_REG(pThis, CORBSTS) &= ~(v & u32Value);
    return VINF_SUCCESS;
}

static int hdaRegWriteCORBWP(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
#ifdef IN_RING3
    int rc;
    rc = hdaRegWriteU16(pThis, iReg, u32Value);
    if (RT_FAILURE(rc))
        AssertRCReturn(rc, rc);
    if (HDA_REG(pThis, CORBWP) == HDA_REG(pThis, CORBRP))
        return VINF_SUCCESS;
    if (!HDA_REG_FLAG_VALUE(pThis, CORBCTL, DMA))
        return VINF_SUCCESS;
    rc = hdaCORBCmdProcess(pThis);
    return rc;
#else
    RT_NOREF(pThis, iReg, u32Value);
    return VINF_IOM_R3_MMIO_WRITE;
#endif
}

static int hdaRegWriteSDCBL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    if (!hdaRegWriteSDIsAllowed(pThis, iReg, u32Value))
        return VINF_SUCCESS;

    int rc = hdaRegWriteU32(pThis, iReg, u32Value);
    if (RT_SUCCESS(rc))
    {
        uint8_t uSD = HDA_SD_NUM_FROM_REG(pThis, CBL, iReg);

        PHDASTREAM pStream = hdaStreamFromID(pThis, uSD);
        if (pStream)
            pStream->u32CBL = u32Value;

        LogFlowFunc(("[SD%RU8] CBL=%RU32\n", uSD, u32Value));
    }
    else
        AssertRCReturn(rc, VINF_SUCCESS);

    return rc;
}

static int hdaRegWriteSDCTL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
#if defined(LOG_ENABLED) || defined(IN_RING3)
    bool const fRun      = RT_BOOL(u32Value & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN));
    bool const fInRun    = RT_BOOL(HDA_REG_IND(pThis, iReg) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN));
#endif
    bool const fReset    = RT_BOOL(u32Value & HDA_REG_FIELD_FLAG_MASK(SDCTL, SRST));
    bool const fInReset  = RT_BOOL(HDA_REG_IND(pThis, iReg) & HDA_REG_FIELD_FLAG_MASK(SDCTL, SRST));

    uint8_t uSD = HDA_SD_NUM_FROM_REG(pThis, CTL, iReg);

    PHDASTREAM pStream = hdaStreamFromID(pThis, uSD);
    if (!pStream)
    {
        LogFunc(("Warning: Changing SDCTL on non-attached stream with ID=%RU8 (iReg=0x%x)\n", uSD, iReg));
        return hdaRegWriteU24(pThis, iReg, u32Value); /* Write 3 bytes. */
    }

    LogFunc(("[SD%RU8] fRun=%RTbool, fInRun=%RTbool, fReset=%RTbool, fInReset=%RTbool, %R[sdctl]\n",
             uSD, fRun, fInRun, fReset, fInReset, u32Value));

    if (fInReset)
    {
        /* Guest is resetting HDA's stream, we're expecting guest will mark stream as exit. */
        Assert(!fReset);
        LogFunc(("Guest initiated exit of stream reset\n"));
    }
    else if (fReset)
    {
#ifdef IN_RING3
        /* ICH6 datasheet 18.2.33 says that RUN bit should be cleared before initiation of reset. */
        Assert(!fInRun && !fRun);

        LogFunc(("Guest initiated enter to stream reset\n"));
        hdaStreamReset(pThis, pStream, uSD);
#else
        return VINF_IOM_R3_MMIO_WRITE;
#endif
    }
    else
    {
#ifdef IN_RING3
        /*
         * We enter here to change DMA states only.
         */
        if (fInRun != fRun)
        {
            Assert(!fReset && !fInReset);
            LogFunc(("[SD%RU8] fRun=%RTbool\n", uSD, fRun));

            PHDADRIVER pDrv;
            switch (uSD)
            {
                case 0: /** @todo Use a variable here. Later. */
                {
                    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
                        pDrv->pConnector->pfnEnableIn(pDrv->pConnector,
                                                      pDrv->LineIn.pStrmIn, fRun);
                    break;
                }
# ifdef VBOX_WITH_HDA_MIC_IN
                case 2: /** @todo Use a variable here. Later. */
                {
                    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
                        pDrv->pConnector->pfnEnableIn(pDrv->pConnector,
                                                      pDrv->MicIn.pStrmIn, fRun);
                    break;
                }
# endif
                case 4: /** @todo Use a variable here. Later. */
                {
                    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
                        pDrv->pConnector->pfnEnableOut(pDrv->pConnector,
                                                       pDrv->Out.pStrmOut, fRun);
                    break;
                }

                default:
                    AssertMsgFailed(("Changing RUN bit on non-attached stream, register %RU32\n", iReg));
                    break;
            }

            /* First, set the running flag. */
            ASMAtomicXchgBool(&pStream->State.fRunning, fRun);

            if (fRun)
            {
                /* Re-init the stream. */
                int rc2 = hdaStreamInit(pThis, pStream, uSD);
                AssertRC(rc2);

                /* (Re-)init the stream's period. */
                hdaStreamPeriodInit(&pStream->State.Period,
                                    pStream->u8SD, pStream->u16LVI, pStream->u32CBL, &pStream->State.strmCfg);

                /* Begin a new period for this stream. */
                rc2 = hdaStreamPeriodBegin(&pStream->State.Period, hdaWalClkGetCurrent(pThis)/* Use current wall clock time */);
                AssertRC(rc2);

                /* Update the stream's time stamp before starting the timer. */
                pStream->State.uTimerTS = TMTimerGet(pThis->pTimer);

#ifndef VBOX_WITH_AUDIO_CALLBACKS
                if (!TMTimerIsActive(pThis->pTimer))
                {
                    pThis->tsTimerExpire = TMTimerGet(pThis->pTimer) + pThis->cTimerTicks;
# ifdef HDA_DEBUG_TIMING
                    pThis->Dbg.tsTimerLastCalledNs = pThis->tsTimerExpire;
# endif
                    /* Fire off timer. */
                    rc2 = TMTimerSet(pThis->pTimer, pThis->tsTimerExpire);
                    AssertRC(rc2);
                }
#endif
            }
            else
            {
                /* Make sure to (re-)schedule outstanding (delayed) interrupts. */
                hdaReschedulePendingInterrupts(pThis);

                /* Reset the period. */
                hdaStreamPeriodReset(&pStream->State.Period);
            }
        }
#else /* IN_RING3 */
        return VINF_IOM_R3_MMIO_WRITE;
#endif /* !IN_RING3 */
    }

    return hdaRegWriteU24(pThis, iReg, u32Value);
}

static int hdaRegWriteSDSTS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
#ifdef IN_RING3
    uint32_t v = HDA_REG_IND(pThis, iReg);

    /* Clear the corresponding bits written by the guest. */
    v &= ~(u32Value & v);

    HDA_REG_IND(pThis, iReg) = v;

    PHDASTREAM pStream = hdaStreamFromID(pThis, HDA_SD_NUM_FROM_REG(pThis, STS, iReg));
    if (pStream)
    {
        /* Some guests tend to write SDnSTS even if the stream is not running.
         * So make sure to check if the RUN bit is set first. */
        const bool fInRun = RT_BOOL(HDA_STREAM_REG(pThis, CTL, pStream->u8SD) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN));

        Log3Func(("[SD%RU8] fRun=%RTbool %R[sdsts]\n", pStream->u8SD, fInRun, v));

        PHDASTREAMPERIOD pPeriod = &pStream->State.Period;

        if (hdaStreamPeriodLock(pPeriod))
        {
            const bool fNeedsInterrupt = hdaStreamPeriodNeedsInterrupt(pPeriod);
            if (fNeedsInterrupt)
                hdaStreamPeriodReleaseInterrupt(pPeriod);

            if (hdaStreamPeriodIsComplete(pPeriod))
            {
                hdaStreamPeriodEnd(pPeriod);

                if (fInRun)
                    hdaStreamPeriodBegin(pPeriod, hdaWalClkGetCurrent(pThis) /* Use current wall clock time */);
            }

            hdaStreamPeriodUnlock(pPeriod); /* Unlock before processing interrupt. */

#ifndef DEBUG
            hdaProcessInterrupt(pThis);
#else
            hdaProcessInterrupt(pThis, __FUNCTION__);
#endif
        }
    }

    return VINF_SUCCESS;
#else /* IN_RING3 */
    RT_NOREF(pThis, iReg, u32Value);
    return VINF_IOM_R3_MMIO_WRITE;
#endif /* !IN_RING3 */
}

static int hdaRegWriteSDLVI(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    if (!hdaRegWriteSDIsAllowed(pThis, iReg, u32Value))
        return VINF_SUCCESS;

#ifdef IN_RING3
    int rc = hdaRegWriteU16(pThis, iReg, u32Value);
    if (RT_SUCCESS(rc))
    {
        uint8_t uSD = HDA_SD_NUM_FROM_REG(pThis, LVI, iReg);

        PHDASTREAM pStream = hdaStreamFromID(pThis, uSD);
        if (pStream)
        {
            pStream->u16LVI = u32Value;
# ifdef HDA_USE_DMA_ACCESS_HANDLER
            if (pStream->u8SD >= 4) /* Only register for output streams. */ /** @todo Use a define. */
            {
                /* Try registering the DMA handlers.
                 * As we can't be sure in which order LVI + BDL base are set, try registering in both routines. */
                if (hdaStreamRegisterDMAHandlers(pThis, pStream))
                    LogFunc(("[SD%RU8] DMA logging enabled\n", pStream->u8SD));
            }
# endif
        }

        Log2Func(("[SD%RU8] LVI=%RU32\n", uSD, u32Value));
    }
    else
        AssertRCReturn(rc, VINF_SUCCESS);

    return rc;
#else /* IN_RING3 */
    return VINF_IOM_R3_MMIO_WRITE;
#endif /* !IN_RING3 */
}

static int hdaRegWriteSDFIFOW(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    switch (u32Value)
    {
        case HDA_SDFIFOW_8B:
        case HDA_SDFIFOW_16B:
        case HDA_SDFIFOW_32B:
            return hdaRegWriteU16(pThis, iReg, u32Value);
        default:
            break;
    }

    AssertFailed();

    LogRel(("HDA: Attempt to write unsupported value (%x) for SDFIFOW\n", u32Value));
    return hdaRegWriteU16(pThis, iReg, HDA_SDFIFOW_32B);
}

/**
 * @note This method could be called for changing value on Output Streams
 *       only (ICH6 datasheet 18.2.39).
 */
static int hdaRegWriteSDFIFOS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    uint32_t u32FIFOS = 0;

    switch (iReg)
    {
        /* SDInFIFOS is RO, n=0-3. */
        case HDA_REG_SD0FIFOS:
        case HDA_REG_SD1FIFOS:
        case HDA_REG_SD2FIFOS:
        case HDA_REG_SD3FIFOS:
        {
            LogRel(("HDA: Guest tries to change read-only value of FIFO size of input stream, ignoring\n"));
            break;
        }
        case HDA_REG_SD4FIFOS:
        case HDA_REG_SD5FIFOS:
        case HDA_REG_SD6FIFOS:
        case HDA_REG_SD7FIFOS:
        {
            switch(u32Value)
            {
                case HDA_SDONFIFO_16B:
                case HDA_SDONFIFO_32B:
                case HDA_SDONFIFO_64B:
                case HDA_SDONFIFO_128B:
                case HDA_SDONFIFO_192B:
                    u32FIFOS = u32Value;
                    break;

                case HDA_SDONFIFO_256B: /** @todo r=andy Investigate this. */
                    LogRel(("HDA: 256-bit FIFO output size is unsupported, defaulting to 192-bit\n"));
                    /* fall thru */
                default:
                    u32FIFOS = HDA_SDONFIFO_192B;
                    break;
            }

            break;
        }
        default:
        {
            AssertMsgFailed(("Something weird happened with register lookup routine\n"));
            break;
        }
    }

    if (u32FIFOS)
    {
        uint8_t uSD = HDA_SD_NUM_FROM_REG(pThis, LVI, iReg);

        PHDASTREAM pStream = hdaStreamFromID(pThis, uSD);
        if (pStream)
            pStream->u16FIFOS = RT_LO_U16(u32FIFOS) + 1;

        LogFunc(("[SD%RU8] Updating FIFOS to %RU32 bytes\n", uSD, hdaSDFIFOSToBytes(u32FIFOS)));

        return hdaRegWriteU16(pThis, iReg, u32FIFOS);
    }

    return VINF_SUCCESS;
}

#ifdef IN_RING3
static int hdaSDFMTToStrmCfg(uint32_t u32SDFMT, PPDMAUDIOSTREAMCFG pCfg)
{
    AssertPtrReturn(pCfg, VERR_INVALID_POINTER);

# define EXTRACT_VALUE(v, mask, shift) ((v & ((mask) << (shift))) >> (shift))

    int rc = VINF_SUCCESS;

    uint32_t u32Hz     = (u32SDFMT & HDA_SDFMT_BASE_RATE_MASK) ? 44100 : 48000;
    uint32_t u32HzMult = 1;
    uint32_t u32HzDiv  = 1;

    switch (EXTRACT_VALUE(u32SDFMT, HDA_SDFMT_MULT_MASK, HDA_SDFMT_MULT_SHIFT))
    {
        case 0: u32HzMult = 1; break;
        case 1: u32HzMult = 2; break;
        case 2: u32HzMult = 3; break;
        case 3: u32HzMult = 4; break;
        default:
            LogFunc(("Unsupported multiplier %x\n",
                     EXTRACT_VALUE(u32SDFMT, HDA_SDFMT_MULT_MASK, HDA_SDFMT_MULT_SHIFT)));
            rc = VERR_NOT_SUPPORTED;
            break;
    }
    switch (EXTRACT_VALUE(u32SDFMT, HDA_SDFMT_DIV_MASK, HDA_SDFMT_DIV_SHIFT))
    {
        case 0: u32HzDiv = 1; break;
        case 1: u32HzDiv = 2; break;
        case 2: u32HzDiv = 3; break;
        case 3: u32HzDiv = 4; break;
        case 4: u32HzDiv = 5; break;
        case 5: u32HzDiv = 6; break;
        case 6: u32HzDiv = 7; break;
        case 7: u32HzDiv = 8; break;
        default:
            LogFunc(("Unsupported divisor %x\n",
                     EXTRACT_VALUE(u32SDFMT, HDA_SDFMT_DIV_MASK, HDA_SDFMT_DIV_SHIFT)));
            rc = VERR_NOT_SUPPORTED;
            break;
    }

    PDMAUDIOFMT enmFmt = AUD_FMT_S16; /* Default to 16-bit signed. */
    switch (EXTRACT_VALUE(u32SDFMT, HDA_SDFMT_BITS_MASK, HDA_SDFMT_BITS_SHIFT))
    {
        case 0:
            LogFunc(("Requested 8-bit\n"));
            enmFmt = AUD_FMT_S8;
            break;
        case 1:
            LogFunc(("Requested 16-bit\n"));
            enmFmt = AUD_FMT_S16;
            break;
        case 2:
            LogFunc(("Requested 20-bit\n"));
            break;
        case 3:
            LogFunc(("Requested 24-bit\n"));
            break;
        case 4:
            LogFunc(("Requested 32-bit\n"));
            enmFmt = AUD_FMT_S32;
            break;
        default:
            AssertMsgFailed(("Unsupported bits shift %x\n",
                             EXTRACT_VALUE(u32SDFMT, HDA_SDFMT_BITS_MASK, HDA_SDFMT_BITS_SHIFT)));
            rc = VERR_NOT_SUPPORTED;
            break;
    }

    if (RT_SUCCESS(rc))
    {
        pCfg->uHz           = u32Hz * u32HzMult / u32HzDiv;
        pCfg->cChannels     = (u32SDFMT & 0xf) + 1;
        pCfg->enmFormat     = enmFmt;
        pCfg->enmEndianness = PDMAUDIOHOSTENDIANNESS;

        LogFunc(("uHz=%RU32, cChannels=%RU8, enmFmt=%ld\n", pCfg->uHz, pCfg->cChannels, pCfg->enmFormat));
    }

# undef EXTRACT_VALUE

    LogFlowFuncLeaveRC(rc);
    return rc;
}
#endif /* IN_RING3 */

static int hdaRegWriteSDFMT(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    if (!hdaRegWriteSDIsAllowed(pThis, iReg, u32Value))
        return VINF_SUCCESS;

#ifdef IN_RING3
    uint8_t uSD = HDA_SD_NUM_FROM_REG(pThis, FMT, iReg);

    LogFlowFuncEnter();

    PDMAUDIOSTREAMCFG strmCfg;
    int rc = hdaSDFMTToStrmCfg(u32Value, &strmCfg);
    if (RT_FAILURE(rc))
    {
        LogRel(("HDA: Guest requested unsupported format (0x%x) for stream #%RU8, ignoring\n", u32Value, uSD));
        return VINF_SUCCESS; /* Always return success to the MMIO handler. */
    }

    /* Write the wanted stream format into the register in any case.
     *
     * This is important for e.g. MacOS guests, as those try to initialize streams which are not reported
     * by the device emulation (wants 4 channels, only have 2 channels at the moment).
     *
     * When ignoring those (invalid) formats, this leads to MacOS thinking that the device is malfunctioning
     * and therefore disabling the device completely. */
    rc = hdaRegWriteU16(pThis, iReg, u32Value);
    AssertRC(rc);

    PHDASTREAM pStream = hdaStreamFromID(pThis, uSD);
    if (!pStream)
    {
        LogRel(("HDA: Guest requested format (0x%x) for unhandled stream #%RU8, ignoring\n", u32Value, uSD));
        return rc;
    }

    /** @todo Validate config? */
    memcpy(&pStream->State.strmCfg, &strmCfg, sizeof(PDMAUDIOSTREAMCFG));

# ifdef VBOX_WITH_HDA_CODEC_EMU
    PHDADRIVER pDrv;
    switch (iReg)
    {
        case HDA_REG_SD0FMT:
            RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
            {
                int rc2 = hdaCodecOpenStream(pThis->pCodec, PI_INDEX, &strmCfg);
                AssertRC(rc2);
            }
            break;
#  ifdef VBOX_WITH_HDA_MIC_IN
        case HDA_REG_SD2FMT:
            RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
            {
                int rc2 = hdaCodecOpenStream(pThis->pCodec, MC_INDEX, &strmCfg);
                AssertRC(rc2);
            }
            break;
#  endif
        case HDA_REG_SD4FMT:
            RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
            {
                int rc2 = hdaCodecOpenStream(pThis->pCodec, PO_INDEX, &strmCfg);
                AssertRC(rc2);
            }
            break;
        default:
            LogFunc(("Warning: Changing SDFMT on non-attached stream with ID=%RU8 (iReg=0x%x)\n", uSD, iReg));
            break;
    }
# endif /* VBOX_WITH_HDA_CODEC_EMU */
    return rc;
#else /* !IN_RING3 */
    RT_NOREF(pThis, iReg, u32Value);
    return VINF_IOM_R3_MMIO_WRITE;
#endif
}

/* Note: Will be called for both, BDPL and BDPU, registers. */
DECLINLINE(int) hdaRegWriteSDBDPX(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value, uint8_t uSD)
{
    if (!hdaRegWriteSDIsAllowed(pThis, iReg, u32Value))
        return VINF_SUCCESS;

#ifdef IN_RING3
    int rc = hdaRegWriteU32(pThis, iReg, u32Value);
    if (RT_SUCCESS(rc))
    {
        PHDASTREAM pStream = hdaStreamFromID(pThis, uSD);
        if (pStream)
        {
            pStream->u64BDLBase = RT_MAKE_U64(HDA_STREAM_REG(pThis, BDPL, uSD),
                                              HDA_STREAM_REG(pThis, BDPU, uSD));
# ifdef HDA_USE_DMA_ACCESS_HANDLER
            if (pStream->u8SD >= 4) /* Only register for output streams. */ /** @todo Use a define. */
            {
                /* Try registering the DMA handlers.
                 * As we can't be sure in which order LVI + BDL base are set, try registering in both routines. */
                if (hdaStreamRegisterDMAHandlers(pThis, pStream))
                    LogFunc(("[SD%RU8] DMA logging enabled\n", pStream->u8SD));
            }
# endif
        }
    }
    else
        AssertRCReturn(rc, VINF_SUCCESS);

    return rc;
#else /* IN_RING3 */
    RT_NOREF(uSD);
    return VINF_IOM_R3_MMIO_WRITE;
#endif /* !IN_RING3 */
}

static int hdaRegWriteSDBDPL(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    return hdaRegWriteSDBDPX(pThis, iReg, u32Value, HDA_SD_NUM_FROM_REG(pThis, BDPL, iReg));
}

static int hdaRegWriteSDBDPU(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    return hdaRegWriteSDBDPX(pThis, iReg, u32Value, HDA_SD_NUM_FROM_REG(pThis, BDPU, iReg));
}

/**
 * Checks whether a write to a specific SDnXXX register is allowed or not.
 *
 * @return  bool                Returns @true if write is allowed, @false if not.
 * @param   pThis               Pointer to HDA state.
 * @param   iReg                Register to write.
 * @param   u32Value            Value to write.
 */
inline bool hdaRegWriteSDIsAllowed(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    RT_NOREF(u32Value);

    /* Check if the SD's RUN bit is set. */
    bool fIsRunning = RT_BOOL(HDA_REG_IND(pThis, iReg) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN));
    if (fIsRunning)
    {
#ifdef VBOX_STRICT
        AssertMsgFailed(("[SD%RU8] Cannot write to register 0x%x (0x%x) when RUN bit is set\n",
                         HDA_SD_NUM_FROM_REG(pThis, CTL, iReg), iReg, u32Value));
#endif
        return false;
    }

    return true;
}

static int hdaRegReadIRS(PHDASTATE pThis, uint32_t iReg, uint32_t *pu32Value)
{
    int rc = VINF_SUCCESS;
    /* regarding 3.4.3 we should mark IRS as busy in case CORB is active */
    if (   HDA_REG(pThis, CORBWP) != HDA_REG(pThis, CORBRP)
        || HDA_REG_FLAG_VALUE(pThis, CORBCTL, DMA))
        HDA_REG(pThis, IRS) = HDA_REG_FIELD_FLAG_MASK(IRS, ICB);  /* busy */

    rc = hdaRegReadU32(pThis, iReg, pu32Value);
    return rc;
}

static int hdaRegWriteIRS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    RT_NOREF(iReg);
    int rc = VINF_SUCCESS;

    /*
     * If the guest set the ICB bit of IRS register, HDA should process the verb in IC register,
     * write the response to IR register, and set the IRV (valid in case of success) bit of IRS register.
     */
    if (   u32Value & HDA_REG_FIELD_FLAG_MASK(IRS, ICB)
        && !HDA_REG_FLAG_VALUE(pThis, IRS, ICB))
    {
#ifdef IN_RING3
        PFNHDACODECVERBPROCESSOR    pfn = NULL;
        uint64_t                    resp;
        uint32_t cmd = HDA_REG(pThis, IC);
        if (HDA_REG(pThis, CORBWP) != HDA_REG(pThis, CORBRP))
        {
            /*
             * 3.4.3 defines behavior of immediate Command status register.
             */
            LogRel(("guest attempted process immediate verb (%x) with active CORB\n", cmd));
            return rc;
        }
        HDA_REG(pThis, IRS) = HDA_REG_FIELD_FLAG_MASK(IRS, ICB);  /* busy */
        LogFunc(("IC:%x\n", cmd));

        rc = pThis->pCodec->pfnLookup(pThis->pCodec,
                                      HDA_CODEC_CMD(cmd, 0 /* LUN */),
                                      &pfn);
        if (RT_FAILURE(rc))
            AssertRCReturn(rc, rc);
        rc = pfn(pThis->pCodec,
                 HDA_CODEC_CMD(cmd, 0 /* LUN */), &resp);
        if (RT_FAILURE(rc))
            AssertRCReturn(rc, rc);

        HDA_REG(pThis, IR) = (uint32_t)resp;
        LogFunc(("IR:%x\n", HDA_REG(pThis, IR)));
        HDA_REG(pThis, IRS) = HDA_REG_FIELD_FLAG_MASK(IRS, IRV);  /* result is ready  */
        HDA_REG(pThis, IRS) &= ~HDA_REG_FIELD_FLAG_MASK(IRS, ICB); /* busy is clear */
#else /* !IN_RING3 */
        rc = VINF_IOM_R3_MMIO_WRITE;
#endif
        return rc;
    }
    /*
     * Once the guest read the response, it should clean the IRV bit of the IRS register.
     */
    if (   u32Value & HDA_REG_FIELD_FLAG_MASK(IRS, IRV)
        && HDA_REG_FLAG_VALUE(pThis, IRS, IRV))
        HDA_REG(pThis, IRS) &= ~HDA_REG_FIELD_FLAG_MASK(IRS, IRV);
    return rc;
}

static int hdaRegWriteRIRBWP(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    RT_NOREF(iReg);
    if (u32Value & HDA_REG_FIELD_FLAG_MASK(RIRBWP, RST))
        HDA_REG(pThis, RIRBWP) = 0;

    /* The remaining bits are O, see 6.2.22 */
    return VINF_SUCCESS;
}

static int hdaRegWriteBase(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    uint32_t iRegMem = g_aHdaRegMap[iReg].mem_idx;
    int rc = hdaRegWriteU32(pThis, iReg, u32Value);
    if (RT_FAILURE(rc))
        AssertRCReturn(rc, rc);

    switch(iReg)
    {
        case HDA_REG_CORBLBASE:
            pThis->u64CORBBase &= UINT64_C(0xFFFFFFFF00000000);
            pThis->u64CORBBase |= pThis->au32Regs[iRegMem];
            break;
        case HDA_REG_CORBUBASE:
            pThis->u64CORBBase &= UINT64_C(0x00000000FFFFFFFF);
            pThis->u64CORBBase |= ((uint64_t)pThis->au32Regs[iRegMem] << 32);
            break;
        case HDA_REG_RIRBLBASE:
            pThis->u64RIRBBase &= UINT64_C(0xFFFFFFFF00000000);
            pThis->u64RIRBBase |= pThis->au32Regs[iRegMem];
            break;
        case HDA_REG_RIRBUBASE:
            pThis->u64RIRBBase &= UINT64_C(0x00000000FFFFFFFF);
            pThis->u64RIRBBase |= ((uint64_t)pThis->au32Regs[iRegMem] << 32);
            break;
        case HDA_REG_DPLBASE:
        {
            pThis->u64DPBase = pThis->au32Regs[iRegMem] & DPBASE_ADDR_MASK;
            Assert(pThis->u64DPBase % 128 == 0); /* Must be 128-byte aligned. */

            /* Also make sure to handle the DMA position enable bit. */
            pThis->fDMAPosition = pThis->au32Regs[iRegMem] & RT_BIT_32(0);
            LogRel(("HDA: %s DMA position buffer\n", pThis->fDMAPosition ? "Enabled" : "Disabled"));
            break;
        }
        case HDA_REG_DPUBASE:
            pThis->u64DPBase = RT_MAKE_U64(RT_LO_U32(pThis->u64DPBase) & DPBASE_ADDR_MASK, pThis->au32Regs[iRegMem]);
            break;
        default:
            AssertMsgFailed(("Invalid index\n"));
            break;
    }

    LogFunc(("CORB base:%llx RIRB base: %llx DP base: %llx\n",
             pThis->u64CORBBase, pThis->u64RIRBBase, pThis->u64DPBase));
    return rc;
}

static int hdaRegWriteRIRBSTS(PHDASTATE pThis, uint32_t iReg, uint32_t u32Value)
{
    uint32_t v = HDA_REG_IND(pThis, iReg);

    /* Clear the corresponding bits written by the guest. */
    HDA_REG(pThis, RIRBSTS) &= ~(v & u32Value);

#ifndef DEBUG
    return hdaProcessInterrupt(pThis);
#else
    return hdaProcessInterrupt(pThis, __FUNCTION__);
#endif
}

#ifdef IN_RING3
# ifdef HDA_USE_DMA_ACCESS_HANDLER
/**
 * Registers access handlers for a stream's BDLE DMA accesses.
 *
 * @returns true if registration was successful, false if not.
 * @param   pThis               HDA state.
 * @param   pStream             HDA stream to register BDLE access handlers for.
 */
static bool hdaStreamRegisterDMAHandlers(PHDASTATE pThis, PHDASTREAM pStream)
{
    /* At least LVI and the BDL base must be set. */
    if (   !pStream->u16LVI
        || !pStream->u64BDLBase)
    {
        return false;
    }

    hdaStreamUnregisterDMAHandlers(pThis, pStream);

    LogFunc(("Registering ...\n"));

    int rc = VINF_SUCCESS;

    /*
     * Create BDLE ranges.
     */

    struct BDLERANGE
    {
        RTGCPHYS uAddr;
        uint32_t uSize;
    } arrRanges[16]; /** @todo Use a define. */

    size_t cRanges = 0;

    for (uint16_t i = 0; i < pStream->u16LVI + 1; i++)
    {
        HDABDLE BDLE;
        rc = hdaBDLEFetch(pThis, &BDLE, pStream->u64BDLBase, i /* Index */);
        if (RT_FAILURE(rc))
            break;

        bool fAddRange = true;
        BDLERANGE *pRange;

        if (cRanges)
        {
            pRange = &arrRanges[cRanges - 1];

            /* Is the current range a direct neighbor of the current BLDE? */
            if ((pRange->uAddr + pRange->uSize) == BDLE.Desc.u64BufAdr)
            {
                /* Expand the current range by the current BDLE's size. */
                pRange->uSize += BDLE.Desc.u32BufSize;

                /* Adding a new range in this case is not needed anymore. */
                fAddRange = false;

                LogFunc(("Expanding range %zu by %RU32 (%RU32 total now)\n", cRanges - 1, BDLE.Desc.u32BufSize, pRange->uSize));
            }
        }

        /* Do we need to add a new range? */
        if (   fAddRange
            && cRanges < RT_ELEMENTS(arrRanges))
        {
            pRange = &arrRanges[cRanges];

            pRange->uAddr = BDLE.Desc.u64BufAdr;
            pRange->uSize = BDLE.Desc.u32BufSize;

            LogFunc(("Adding range %zu - 0x%x (%RU32)\n", cRanges, pRange->uAddr, pRange->uSize));

            cRanges++;
        }
    }

    LogFunc(("%zu ranges total\n", cRanges));

    /*
     * Register all ranges as DMA access handlers.
     */

    for (size_t i = 0; i < cRanges; i++)
    {
        BDLERANGE *pRange = &arrRanges[i];

        PHDADMAACCESSHANDLER pHandler = (PHDADMAACCESSHANDLER)RTMemAllocZ(sizeof(HDADMAACCESSHANDLER));
        if (!pHandler)
        {
            rc = VERR_NO_MEMORY;
            break;
        }

        RTListAppend(&pStream->State.lstDMAHandlers, &pHandler->Node);

        pHandler->pStream = pStream; /* Save a back reference to the owner. */

        char szDesc[32];
        RTStrPrintf(szDesc, sizeof(szDesc), "HDA[SD%RU8 - RANGE%02zu]", pStream->u8SD, i);

        int rc2 = PGMR3HandlerPhysicalTypeRegister(PDMDevHlpGetVM(pThis->pDevInsR3), PGMPHYSHANDLERKIND_WRITE,
                                                   hdaDMAAccessHandler,
                                                   NULL, NULL, NULL,
                                                   NULL, NULL, NULL,
                                                   szDesc, &pHandler->hAccessHandlerType);
        AssertRCBreak(rc2);

        pHandler->BDLEAddr  = pRange->uAddr;
        pHandler->BDLESize  = pRange->uSize;

        /* Get first and last pages of the BDLE range. */
        RTGCPHYS pgFirst = pRange->uAddr & ~PAGE_OFFSET_MASK;
        RTGCPHYS pgLast  = RT_ALIGN(pgFirst + pRange->uSize, PAGE_SIZE);

        /* Calculate the region size (in pages). */
        RTGCPHYS regionSize = RT_ALIGN(pgLast - pgFirst, PAGE_SIZE);

        pHandler->GCPhysFirst = pgFirst;
        pHandler->GCPhysLast  = pHandler->GCPhysFirst + (regionSize - 1);

        LogFunc(("\tRegistering region '%s': 0x%x - 0x%x (region size: %zu)\n",
                 szDesc, pHandler->GCPhysFirst, pHandler->GCPhysLast, regionSize));
        LogFunc(("\tBDLE @ 0x%x - 0x%x (%RU32)\n",
                 pHandler->BDLEAddr, pHandler->BDLEAddr + pHandler->BDLESize, pHandler->BDLESize));

        rc2 = PGMHandlerPhysicalRegister(PDMDevHlpGetVM(pThis->pDevInsR3),
                                         pHandler->GCPhysFirst, pHandler->GCPhysLast,
                                         pHandler->hAccessHandlerType, pHandler, NIL_RTR0PTR, NIL_RTRCPTR,
                                         szDesc);
        AssertRCBreak(rc2);

        pHandler->fRegistered = true;
    }

    LogFunc(("Registration ended with rc=%Rrc\n", rc));

    return RT_SUCCESS(rc);
}

/**
 * Unregisters access handlers of a stream's BDLEs.
 *
 * @param   pThis               HDA state.
 * @param   pStream             HDA stream to unregister BDLE access handlers for.
 */
static void hdaStreamUnregisterDMAHandlers(PHDASTATE pThis, PHDASTREAM pStream)
{
    LogFunc(("\n"));

    PHDADMAACCESSHANDLER pHandler, pHandlerNext;
    RTListForEachSafe(&pStream->State.lstDMAHandlers, pHandler, pHandlerNext, HDADMAACCESSHANDLER, Node)
    {
        if (!pHandler->fRegistered) /* Handler not registered? Skip. */
            continue;

        LogFunc(("Unregistering 0x%x - 0x%x (%zu)\n",
                 pHandler->GCPhysFirst, pHandler->GCPhysLast, pHandler->GCPhysLast - pHandler->GCPhysFirst));

        int rc2 = PGMHandlerPhysicalDeregister(PDMDevHlpGetVM(pThis->pDevInsR3),
                                               pHandler->GCPhysFirst);
        AssertRC(rc2);

        RTListNodeRemove(&pHandler->Node);

        RTMemFree(pHandler);
        pHandler = NULL;
    }

    Assert(RTListIsEmpty(&pStream->State.lstDMAHandlers));
}
# endif /* HDA_USE_DMA_ACCESS_HANDLER */

# ifdef LOG_ENABLED
static void hdaBDLEDumpAll(PHDASTATE pThis, uint64_t u64BDLBase, uint16_t cBDLE)
{
    LogFlowFunc(("BDLEs @ 0x%x (%RU16):\n", u64BDLBase, cBDLE));
    if (!u64BDLBase)
        return;

    uint32_t cbBDLE = 0;
    for (uint16_t i = 0; i < cBDLE; i++)
    {
        HDABDLEDESC bd;
        PDMDevHlpPhysRead(pThis->CTX_SUFF(pDevIns), u64BDLBase + i * sizeof(HDABDLEDESC), &bd, sizeof(bd));

        LogFunc(("\t#%03d BDLE(adr:0x%llx, size:%RU32, ioc:%RTbool)\n",
                 i, bd.u64BufAdr, bd.u32BufSize, bd.fFlags & HDA_BDLE_FLAG_IOC));

        cbBDLE += bd.u32BufSize;
    }

    LogFlowFunc(("Total: %RU32 bytes\n", cbBDLE));

    if (!pThis->u64DPBase) /* No DMA base given? Bail out. */
        return;

    LogFlowFunc(("DMA counters:\n"));

    for (int i = 0; i < cBDLE; i++)
    {
        uint32_t uDMACnt;
        PDMDevHlpPhysRead(pThis->CTX_SUFF(pDevIns), (pThis->u64DPBase & DPBASE_ADDR_MASK) + (i * 2 * sizeof(uint32_t)),
                          &uDMACnt, sizeof(uDMACnt));

        LogFlowFunc(("\t#%03d DMA @ 0x%x\n", i , uDMACnt));
    }
}
# endif /* LOG_ENABLED */

/**
 * Fetches a Bundle Descriptor List Entry (BDLE) from the DMA engine.
 *
 * @param   pThis                   Pointer to HDA state.
 * @param   pBDLE                   Where to store the fetched result.
 * @param   u64BaseDMA              Address base of DMA engine to use.
 * @param   u16Entry                BDLE entry to fetch.
 */
static int hdaBDLEFetch(PHDASTATE pThis, PHDABDLE pBDLE, uint64_t u64BaseDMA, uint16_t u16Entry)
{
    AssertPtrReturn(pThis,   VERR_INVALID_POINTER);
    AssertPtrReturn(pBDLE,   VERR_INVALID_POINTER);
    AssertReturn(u64BaseDMA, VERR_INVALID_PARAMETER);

    if (!u64BaseDMA)
    {
        LogRel2(("HDA: Unable to fetch BDLE #%RU16 - no base DMA address set (yet)\n", u16Entry));
        return VERR_NOT_FOUND;
    }
    /** @todo Compare u16Entry with LVI. */

    int rc = PDMDevHlpPhysRead(pThis->CTX_SUFF(pDevIns), u64BaseDMA + (u16Entry * sizeof(HDABDLEDESC)),
                               &pBDLE->Desc, sizeof(pBDLE->Desc));

    if (RT_SUCCESS(rc))
    {
        /* Reset internal state. */
        RT_ZERO(pBDLE->State);
        pBDLE->State.u32BDLIndex = u16Entry;
    }

    Log3Func(("Entry #%d @ 0x%x: %R[bdle], rc=%Rrc\n", u16Entry, u64BaseDMA + (u16Entry * sizeof(HDABDLEDESC)), pBDLE, rc));

    return rc;
}

/**
 * Returns the number of outstanding stream data bytes which need to be processed
 * by the DMA engine assigned to this stream.
 *
 * @return Number of bytes for the DMA engine to process.
 */
DECLINLINE(uint32_t) hdaStreamGetTransferSize(PHDASTATE pThis, PHDASTREAM pStream)
{
    AssertPtrReturn(pThis, 0);
    AssertPtrReturn(pStream, 0);

    if (!(HDA_STREAM_REG(pThis, CTL, pStream->u8SD) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN)))
    {
        AssertFailed(); /* Should never happen. */
        return 0;
    }

    /* Determine how much for the current BDL entry we have left to transfer. */
    PHDABDLE pBDLE  = &pStream->State.BDLE;
    const uint32_t cbBDLE = RT_MIN(pBDLE->Desc.u32BufSize, pBDLE->Desc.u32BufSize - pBDLE->State.u32BufOff);

    /* Determine how much we (still) can stuff in the stream's internal FIFO.  */
    const uint32_t cbCircBuf   = (uint32_t)RTCircBufFree(pStream->State.pCircBuf);

    uint32_t cbToTransfer = cbBDLE;

    /* Make sure that we don't transfer more than our FIFO can hold at the moment.
     * As the host sets the overall pace it needs to process some of the FIFO data first before
     * we can issue a new DMA data transfer. */
    if (cbToTransfer > cbCircBuf)
        cbToTransfer = cbCircBuf;

    Log3Func(("[SD%RU8] LPIB=%RU32 CBL=%RU32 cbCircBuf=%RU32, -> cbToTransfer=%RU32 %R[bdle]\n", pStream->u8SD,
              HDA_STREAM_REG(pThis, LPIB, pStream->u8SD), pStream->u32CBL, cbCircBuf, cbToTransfer, pBDLE));
    return cbToTransfer;
}


DECLINLINE(void) hdaStreamTransferInc(PHDASTATE pThis, PHDASTREAM pStream, uint32_t cbInc)
{
    AssertPtrReturnVoid(pThis);
    AssertPtrReturnVoid(pStream);

    if (!cbInc)
        return;

    const uint32_t u32LPIB = HDA_STREAM_REG(pThis, LPIB, pStream->u8SD);

    Log3Func(("[SD%RU8] %RU32 + %RU32 -> %RU32, CBL=%RU32\n",
              pStream->u8SD, u32LPIB, cbInc, u32LPIB + cbInc, pStream->u32CBL));

    hdaStreamUpdateLPIB(pThis, pStream, u32LPIB + cbInc);
}

/**
 * Tells whether a given BDLE is complete or not.
 *
 * @return  @true if BDLE is complete, @false if not.
 * @param   pBDLE               BDLE to retrieve status for.
 */
static bool hdaBDLEIsComplete(PHDABDLE pBDLE)
{
    bool fIsComplete = false;

    if (   !pBDLE->Desc.u32BufSize /* There can be BDLEs with 0 size. */
        || (pBDLE->State.u32BufOff >= pBDLE->Desc.u32BufSize))
    {
        Assert(pBDLE->State.u32BufOff == pBDLE->Desc.u32BufSize);
        fIsComplete = true;
    }

    Log3Func(("%R[bdle] => %s\n", pBDLE, fIsComplete ? "COMPLETE" : "INCOMPLETE"));

    return fIsComplete;
}

/**
 * Tells whether a given BDLE needs an interrupt or not.
 *
 * @return  @true if BDLE needs an interrupt, @false if not.
 * @param   pBDLE               BDLE to retrieve status for.
 */
static bool hdaBDLENeedsInterrupt(PHDABDLE pBDLE)
{
    return (pBDLE->Desc.fFlags & HDA_BDLE_FLAG_IOC);
}

/**
 * Writes audio data from an HDA input stream's FIFO to its associated DMA area.
 *
 * @return  IPRT status code.
 * @param   pThis               HDA state.
 * @param   pStream             HDA input stream to write audio data to.
 * @param   cbToWrite           How much (in bytes) to write.
 * @param   pcbWritten          Returns written bytes on success. Optional.
 */
static int hdaDMAWrite(PHDASTATE pThis, PHDASTREAM pStream, uint32_t cbToWrite, uint32_t *pcbWritten)
{
    AssertPtrReturn(pThis,   VERR_INVALID_POINTER);
    AssertPtrReturn(pStream, VERR_INVALID_POINTER);
    /* pcbWritten is optional. */

    PHDABDLE   pBDLE    = &pStream->State.BDLE;
    PRTCIRCBUF pCircBuf = pStream->State.pCircBuf;
    AssertPtr(pCircBuf);

    int rc = VINF_SUCCESS;

    uint32_t cbWrittenTotal = 0;

    void *pvBuf  = NULL;
    size_t cbBuf = 0;

    uint8_t abSilence[HDA_FIFO_MAX + 1] = { 0 };

    while (cbToWrite)
    {
        size_t cbChunk = RT_MIN(cbToWrite, pStream->u16FIFOS);

        size_t cbBlock = 0;
        RTCircBufAcquireReadBlock(pCircBuf, cbChunk, &pvBuf, &cbBlock);

        if (cbBlock)
        {
            cbBuf = cbBlock;
        }
        else /* No audio data available? Send silence. */
        {
            pvBuf = &abSilence;
            cbBuf = cbChunk;
        }

        /* Sanity checks. */
        Assert(cbBuf <= pBDLE->Desc.u32BufSize - pBDLE->State.u32BufOff);
        Assert(cbBuf % HDA_FRAME_SIZE == 0);
        Assert((cbBuf >> 1) >= 1);

#ifdef VBOX_AUDIO_DEBUG_DUMP_PCM_DATA
        RTFILE fh;
        RTFileOpen(&fh, VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaDMAWrite.pcm",
                   RTFILE_O_OPEN_CREATE | RTFILE_O_APPEND | RTFILE_O_WRITE | RTFILE_O_DENY_NONE);
        RTFileWrite(fh, pvBuf, cbBuf, NULL);
        RTFileClose(fh);
#endif
        int rc2 = PDMDevHlpPCIPhysWrite(pThis->CTX_SUFF(pDevIns),
                                        pBDLE->Desc.u64BufAdr + pBDLE->State.u32BufOff + cbWrittenTotal,
                                        pvBuf, cbBuf);
        AssertRC(rc2);

#ifdef VBOX_WITH_STATISTICS
        STAM_COUNTER_ADD(&pThis->StatBytesWritten, cbBuf);
#endif
        if (cbBlock)
            RTCircBufReleaseReadBlock(pCircBuf, cbBlock);

        Assert(cbToWrite >= cbBuf);
        cbToWrite -= (uint32_t)cbBuf;

        cbWrittenTotal += (uint32_t)cbBuf;
    }

    if (RT_SUCCESS(rc))
    {
        if (pcbWritten)
            *pcbWritten = cbWrittenTotal;
    }
    else
        LogFunc(("Failed with %Rrc\n", rc));

    return rc;
}

/**
 * Reads (transfers) available audio data from a given input mixer sink into an HDA stream's FIFO buffer.
 *
 * @returns IPRT status code.
 * @param   pThis               HDA state.
 * @param   pStream             HDA stream to read audio data to.
 * @param   pSink               Mixer sink to read from.
 * @param   pcbRead             How much (in bytes) of audio input has been read.
 */
static int hdaReadAudio(PHDASTATE pThis, PHDASTREAM pStream, PAUDMIXSINK pSink, uint32_t *pcbRead)
{
    AssertPtrReturn(pThis,   VERR_INVALID_POINTER);
    AssertPtrReturn(pStream, VERR_INVALID_POINTER);
    AssertPtrReturn(pSink,   VERR_INVALID_POINTER);
    /* pcbRead is optional. */

    /* Stream not active? */
    if (!ASMAtomicReadBool(&pStream->State.fRunning))
    {
        Log3Func(("[SD%RU8] Not running\n", pStream->u8SD));

        if (pcbRead)
            *pcbRead = 0;
        return VINF_SUCCESS;
    }

    PRTCIRCBUF pCircBuf = pStream->State.pCircBuf;
    AssertPtr(pCircBuf);

    int rc = VINF_SUCCESS;

    size_t cbToRead = RTCircBufFree(pCircBuf);
    while (cbToRead)
    {
        void *pvBuf;
        size_t cbBuf;

        RTCircBufAcquireWriteBlock(pCircBuf, cbToRead, &pvBuf, &cbBuf);

        uint32_t cbRead = 0;

        if (cbBuf)
        {
            rc = AudioMixerProcessSinkIn(pSink, AUDMIXOP_BLEND, pvBuf, (uint32_t)cbBuf, &cbRead);

#ifdef VBOX_AUDIO_DEBUG_DUMP_PCM_DATA
            if (cbRead)
            {
                RTFILE fh;
                RTFileOpen(&fh, VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaReadAudio.pcm",
                           RTFILE_O_OPEN_CREATE | RTFILE_O_APPEND | RTFILE_O_WRITE | RTFILE_O_DENY_NONE);
                RTFileWrite(fh, pvBuf, cbRead, NULL);
                RTFileClose(fh);
            }
#endif
        }

        RTCircBufReleaseWriteBlock(pCircBuf, cbRead);

        if (   RT_FAILURE(rc)
            || !cbRead)
        {
            break;
        }

        Assert(cbToRead >= cbRead);
        cbToRead -= cbRead;
    }

    /* Always return the available data which now is in the stream's FIFO buffer. */
    uint32_t cbReadTotal = (uint32_t)RTCircBufUsed(pCircBuf);

    if (pcbRead)
        *pcbRead = cbReadTotal;

    Log3Func(("[SD%RU8] cbReadTotal=%RU32, rc=%Rrc\n", pStream->u8SD, cbReadTotal, rc));
    return rc;
}

#ifdef HDA_USE_DMA_ACCESS_HANDLER
/**
 * HC access handler for the FIFO.
 *
 * @returns VINF_SUCCESS if the handler have carried out the operation.
 * @returns VINF_PGM_HANDLER_DO_DEFAULT if the caller should carry out the access operation.
 * @param   pVM             VM Handle.
 * @param   pVCpu           The cross context CPU structure for the calling EMT.
 * @param   GCPhys          The physical address the guest is writing to.
 * @param   pvPhys          The HC mapping of that address.
 * @param   pvBuf           What the guest is reading/writing.
 * @param   cbBuf           How much it's reading/writing.
 * @param   enmAccessType   The access type.
 * @param   enmOrigin       Who is making the access.
 * @param   pvUser          User argument.
 */
static DECLCALLBACK(VBOXSTRICTRC) hdaDMAAccessHandler(PVM pVM, PVMCPU pVCpu, RTGCPHYS GCPhys, void *pvPhys,
                                                      void *pvBuf, size_t cbBuf,
                                                      PGMACCESSTYPE enmAccessType, PGMACCESSORIGIN enmOrigin, void *pvUser)
{
    RT_NOREF(pVM, pVCpu, pvPhys, pvBuf, enmOrigin);

    PHDADMAACCESSHANDLER pHandler = (PHDADMAACCESSHANDLER)pvUser;
    AssertPtr(pHandler);

    PHDASTREAM pStream = pHandler->pStream;
    AssertPtr(pStream);

    Assert(GCPhys >= pHandler->GCPhysFirst);
    Assert(GCPhys <= pHandler->GCPhysLast);
    Assert(enmAccessType == PGMACCESSTYPE_WRITE);

    /* Not within BDLE range? Bail out. */
    if (   (GCPhys         < pHandler->BDLEAddr)
        || (GCPhys + cbBuf > pHandler->BDLEAddr + pHandler->BDLESize))
    {
        return VINF_PGM_HANDLER_DO_DEFAULT;
    }

    switch(enmAccessType)
    {
        case PGMACCESSTYPE_WRITE:
        {
# ifdef DEBUG
            PHDASTREAMDBGINFO pStreamDbg = &pStream->Dbg;

            const uint64_t tsNowNs     = RTTimeNanoTS();
            const uint32_t tsElapsedMs = (tsNowNs - pStreamDbg->tsWriteSlotBegin) / 1000 / 1000;

            uint64_t cWritesHz   = ASMAtomicReadU64(&pStreamDbg->cWritesHz);
            uint64_t cbWrittenHz = ASMAtomicReadU64(&pStreamDbg->cbWrittenHz);

            if (tsElapsedMs >= (1000 / HDA_TIMER_HZ))
            {
                LogFunc(("[SD%RU8] %RU32ms elapsed, cbWritten=%RU64, cWritten=%RU64 -- %RU32 bytes on average per time slot (%zums)\n",
                         pStream->u8SD, tsElapsedMs, cbWrittenHz, cWritesHz,
                         ASMDivU64ByU32RetU32(cbWrittenHz, cWritesHz ? cWritesHz : 1), 1000 / HDA_TIMER_HZ));

                pStreamDbg->tsWriteSlotBegin = tsNowNs;

                cWritesHz   = 0;
                cbWrittenHz = 0;
            }

            cWritesHz   += 1;
            cbWrittenHz += cbBuf;

            ASMAtomicIncU64(&pStreamDbg->cWritesTotal);
            ASMAtomicAddU64(&pStreamDbg->cbWrittenTotal, cbBuf);

            ASMAtomicWriteU64(&pStreamDbg->cWritesHz,   cWritesHz);
            ASMAtomicWriteU64(&pStreamDbg->cbWrittenHz, cbWrittenHz);

            LogFunc(("[SD%RU8] Writing %3zu @ 0x%x (off %zu)\n",
                     pStream->u8SD, cbBuf, GCPhys, GCPhys - pHandler->BDLEAddr));

            LogFunc(("[SD%RU8] cWrites=%RU64, cbWritten=%RU64 -> %RU32 bytes on average\n",
                     pStream->u8SD, pStreamDbg->cWritesTotal, pStreamDbg->cbWrittenTotal,
                     ASMDivU64ByU32RetU32(pStreamDbg->cbWrittenTotal, pStreamDbg->cWritesTotal)));
# endif

# ifdef VBOX_AUDIO_DEBUG_DUMP_PCM_DATA
            RTFILE fh;
            RTFileOpen(&fh, VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaDMAAccessWrite.pcm",
                       RTFILE_O_OPEN_CREATE | RTFILE_O_APPEND | RTFILE_O_WRITE | RTFILE_O_DENY_NONE);
            RTFileWrite(fh, pvBuf, cbBuf, NULL);
            RTFileClose(fh);
# endif

# ifdef HDA_USE_DMA_ACCESS_HANDLER_WRITING
            PRTCIRCBUF pCircBuf = pStream->State.pCircBuf;
            AssertPtr(pCircBuf);

            uint8_t *pbBuf = (uint8_t *)pvBuf;
            while (cbBuf)
            {
                /* Make sure we only copy as much as the stream's FIFO can hold (SDFIFOS, 18.2.39). */
                void *pvChunk;
                size_t cbChunk;
                RTCircBufAcquireWriteBlock(pCircBuf, cbBuf, &pvChunk, &cbChunk);

                if (cbChunk)
                {
                    memcpy(pvChunk, pbBuf, cbChunk);

                    pbBuf  += cbChunk;
                    Assert(cbBuf >= cbChunk);
                    cbBuf  -= cbChunk;
                }
                else
                {
                    //AssertMsg(RTCircBufFree(pCircBuf), ("No more space but still %zu bytes to write\n", cbBuf));
                    break;
                }

                LogFunc(("[SD%RU8] cbChunk=%zu\n", pStream->u8SD, cbChunk));

                RTCircBufReleaseWriteBlock(pCircBuf, cbChunk);
            }
# endif /* HDA_USE_DMA_ACCESS_HANDLER_WRITING */
            break;
        }

        default:
            AssertMsgFailed(("Access type not implemented\n"));
            break;
    }

    return VINF_PGM_HANDLER_DO_DEFAULT;
}
#endif /* HDA_USE_DMA_ACCESS_HANDLER */

/**
 * Reads DMA data from a given HDA output stream into its associated FIFO buffer.
 *
 * @return  IPRT status code.
 * @param   pThis               HDA state.
 * @param   pStream             HDA output stream to read DMA data from.
 * @param   cbToRead            How much (in bytes) to read from DMA.
 * @param   pcbRead             Returns read bytes from DMA. Optional.
 */
static int hdaDMARead(PHDASTATE pThis, PHDASTREAM pStream, uint32_t cbToRead, uint32_t *pcbRead)
{
    AssertPtrReturn(pThis,   VERR_INVALID_POINTER);
    AssertPtrReturn(pStream, VERR_INVALID_POINTER);
    /* pcbRead is optional. */

    int rc = VINF_SUCCESS;

    uint32_t cbReadTotal = 0;

    PHDABDLE   pBDLE     = &pStream->State.BDLE;
    PRTCIRCBUF pCircBuf  = pStream->State.pCircBuf;
    AssertPtr(pCircBuf);

#ifdef HDA_DEBUG_SILENCE
    uint64_t   csSilence = 0;

    pStream->Dbg.cSilenceThreshold = 100;
    pStream->Dbg.cbSilenceReadMin  = _1M;
#endif

    while (cbToRead)
    {
        /* Make sure we only copy as much as the stream's FIFO can hold (SDFIFOS, 18.2.39). */
        void *pvBuf;
        size_t cbBuf;
        RTCircBufAcquireWriteBlock(pCircBuf, RT_MIN(cbToRead, pStream->u16FIFOS), &pvBuf, &cbBuf);

        if (cbBuf)
        {
            /*
             * Read from the current BDLE's DMA buffer.
             */
            int rc2 = PDMDevHlpPhysRead(pThis->CTX_SUFF(pDevIns),
                                        pBDLE->Desc.u64BufAdr + pBDLE->State.u32BufOff + cbReadTotal, pvBuf, cbBuf);
            AssertRC(rc2);

#ifdef HDA_DEBUG_SILENCE
            uint16_t *pu16Buf = (uint16_t *)pvBuf;
            for (size_t i = 0; i < cbBuf / sizeof(uint16_t); i++)
            {
                if (*pu16Buf == 0)
                {
                    csSilence++;
                }
                else
                    break;
                pu16Buf++;
            }
#endif

#ifdef VBOX_AUDIO_DEBUG_DUMP_PCM_DATA
            if (cbBuf)
            {
                RTFILE fh;
                rc2 = RTFileOpen(&fh, VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaDMARead.pcm",
                                 RTFILE_O_OPEN_CREATE | RTFILE_O_APPEND | RTFILE_O_WRITE | RTFILE_O_DENY_NONE);
                if (RT_SUCCESS(rc2))
                {
                    RTFileWrite(fh, pvBuf, cbBuf, NULL);
                    RTFileClose(fh);
                }
                else
                    AssertRC(rc2);
            }
#endif

#if 0
            pStream->Dbg.cbReadTotal += cbBuf;
            const uint64_t cbWritten = ASMAtomicReadU64(&pStream->Dbg.cbWrittenTotal);
            Log3Func(("cbRead=%RU64, cbWritten=%RU64 -> %RU64 bytes %s\n",
                      pStream->Dbg.cbReadTotal, cbWritten,
                      pStream->Dbg.cbReadTotal >= cbWritten ? pStream->Dbg.cbReadTotal - cbWritten : cbWritten - pStream->Dbg.cbReadTotal,
                      pStream->Dbg.cbReadTotal > cbWritten ? "too much" : "too little"));
#endif

#ifdef VBOX_WITH_STATISTICS
            STAM_COUNTER_ADD(&pThis->StatBytesRead, cbBuf);
#endif
        }

        RTCircBufReleaseWriteBlock(pCircBuf, cbBuf);

        if (!cbBuf)
        {
            rc = VERR_BUFFER_OVERFLOW;
            break;
        }

        cbReadTotal += (uint32_t)cbBuf;
        Assert(pBDLE->State.u32BufOff + cbReadTotal <= pBDLE->Desc.u32BufSize);

        Assert(cbToRead >= cbBuf);
        cbToRead    -= (uint32_t)cbBuf;
    }

#ifdef HDA_DEBUG_SILENCE

    if (csSilence)
        pStream->Dbg.csSilence += csSilence;

    if (   csSilence == 0
        && pStream->Dbg.csSilence   >  pStream->Dbg.cSilenceThreshold
        && pStream->Dbg.cbReadTotal >= pStream->Dbg.cbSilenceReadMin)
    {
        LogFunc(("Silent block detected: %RU64 audio samples\n", pStream->Dbg.csSilence));
        pStream->Dbg.csSilence = 0;
    }
#endif

    if (RT_SUCCESS(rc))
    {
        if (pcbRead)
            *pcbRead = cbReadTotal;
    }

    return rc;
}

/**
 * Writes (transfers) audio data from an HDA output stream to its associated FIFO buffer.
 *
 * @returns IPRT status code.
 * @param   pThis               HDA state.
 * @param   pStream             HDA output stream to write data for.
 * @param   cbMax               How much (in bytes) to transfer from the given HDA output stream
 *                              to its associated FIFO buffer.
 */
static int hdaWriteAudio(PHDASTATE pThis, PHDASTREAM pStream, uint32_t cbMax)
{
    AssertPtrReturn(pThis,   VERR_INVALID_POINTER);
    AssertPtrReturn(pStream, VERR_INVALID_POINTER);

    /* Stream not active? */
    if (!ASMAtomicReadBool(&pStream->State.fRunning))
    {
        Log3Func(("[SD%RU8] Not running\n", pStream->u8SD));
        return VINF_SUCCESS;
    }

    int rc = VINF_SUCCESS;

    PRTCIRCBUF pCircBuf = pStream->State.pCircBuf;
    AssertPtr(pCircBuf);

    uint32_t cbWrittenTotal = 0;
    uint32_t cbToWrite      = RT_MIN(cbMax, (uint32_t)RTCircBufUsed(pCircBuf));

    Log3Func(("cbToWrite=%RU32\n", cbToWrite));

    while (cbToWrite >= HDA_FRAME_SIZE)
    {
        void *pvBuf;
        size_t cbBuf;

        uint32_t cbWritten = 0;

        RTCircBufAcquireReadBlock(pCircBuf, cbToWrite, &pvBuf, &cbBuf);

        if (cbBuf)
        {
            PHDADRIVER pDrv;
            RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
            {
                if (pDrv->pConnector->pfnIsActiveOut(pDrv->pConnector, pDrv->Out.pStrmOut))
                {
                    uint32_t cbWrittenToStream = 0;
                    int rc2 = pDrv->pConnector->pfnWrite(pDrv->pConnector, pDrv->Out.pStrmOut,
                                                         pvBuf, (uint32_t)cbBuf, &cbWrittenToStream);
                    if (rc2 == VERR_NOT_AVAILABLE)
                        continue;

                    /* Lagging behind? */
                    if (cbWrittenToStream < (uint32_t)cbBuf)
                        LogFunc(("\tLUN#%RU8: Warning: Only written %RU32 / %RU32 bytes, rc=%Rrc\n",
                                 pDrv->uLUN, cbWrittenToStream, cbToWrite, rc2));

                    /* The primary driver sets the overall pace. */
                    if (pDrv->fFlags & PDMAUDIODRVFLAG_PRIMARY)
                    {
                        cbWritten = cbWrittenToStream;

                        if (RT_FAILURE(rc2))
                        {
                            if (RT_SUCCESS(rc))
                                rc = rc2;
                            break;
                        }
                    }
                }
                else /* Stream disabled, not fatal. */
                {
                    /* Keep going. */
                }
            }
        }

#ifdef VBOX_AUDIO_DEBUG_DUMP_PCM_DATA
        if (cbWritten)
        {
            RTFILE fh;
            RTFileOpen(&fh, VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaWriteAudio.pcm",
                       RTFILE_O_OPEN_CREATE | RTFILE_O_APPEND | RTFILE_O_WRITE | RTFILE_O_DENY_NONE);
            RTFileWrite(fh, pvBuf, cbWritten, NULL);
            RTFileClose(fh);
        }
#endif
        RTCircBufReleaseReadBlock(pCircBuf, cbWritten);

        Assert(cbToWrite >= cbWritten);
        cbToWrite      -= (uint32_t)cbWritten;

        cbWrittenTotal += (uint32_t)cbWritten;

        if (!cbWritten) /* Nothing written? Bail out. */
            break;

        if (RT_FAILURE(rc))
            break;
    }

    Log3Func(("Returning cbWrittenTotal=%RU32 (%zu bytes left), rc=%Rrc\n", cbWrittenTotal, RTCircBufUsed(pCircBuf), rc));
    return rc;
}

/**
 * @interface_method_impl{HDACODEC,pfnReset}
 */
static DECLCALLBACK(int) hdaCodecReset(PHDACODEC pCodec)
{
    PHDASTATE pThis = pCodec->pHDAState;
    NOREF(pThis);
    return VINF_SUCCESS;
}


static DECLCALLBACK(void) hdaCloseIn(PHDASTATE pThis, PDMAUDIORECSOURCE enmRecSource)
{
    NOREF(enmRecSource);

    LogFlowFuncEnter();

    PHDADRIVER pDrv;
    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
    {
        AudioMixerRemoveStream(pThis->pSinkLineIn, pDrv->LineIn.phStrmIn);
        pDrv->LineIn.phStrmIn = NULL;

        if (pDrv->LineIn.pStrmIn)
        {
            pDrv->pConnector->pfnDestroyIn(pDrv->pConnector, pDrv->LineIn.pStrmIn);
            LogFlowFunc(("LUN#%RU8: Destroyed input\n", pDrv->uLUN));
            pDrv->LineIn.pStrmIn = NULL;
        }
    }
}

static DECLCALLBACK(void) hdaCloseOut(PHDASTATE pThis)
{
    LogFlowFuncEnter();

    PHDADRIVER pDrv;
    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
    {
        AudioMixerRemoveStream(pThis->pSinkOutput, pDrv->Out.phStrmOut);
        pDrv->Out.phStrmOut = NULL;

        if (pDrv->Out.pStrmOut)
        {
            pDrv->pConnector->pfnDestroyOut(pDrv->pConnector, pDrv->Out.pStrmOut);
            LogFlowFunc(("LUN#%RU8: Destroyed output\n", pDrv->uLUN));
            pDrv->Out.pStrmOut = NULL;
        }
    }
}

static DECLCALLBACK(int) hdaOpenIn(PHDASTATE pThis,
                                   const char *pszName, PDMAUDIORECSOURCE enmRecSource,
                                   PPDMAUDIOSTREAMCFG pCfg)
{
    PAUDMIXSINK pSink;

    switch (enmRecSource)
    {
# ifdef VBOX_WITH_HDA_MIC_IN
        case PDMAUDIORECSOURCE_MIC:
            pSink = pThis->pSinkMicIn;
            break;
# endif
        case PDMAUDIORECSOURCE_LINE_IN:
            pSink = pThis->pSinkLineIn;
            break;
        default:
            AssertMsgFailed(("Audio source %ld not supported\n", enmRecSource));
            return VERR_NOT_SUPPORTED;
    }

    int rc = VINF_SUCCESS;
    char *pszDesc;

    PHDADRIVER pDrv;
    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
    {
        AudioMixerRemoveStream(pSink, pDrv->LineIn.phStrmIn);
        pDrv->LineIn.phStrmIn = NULL;

        if (pDrv->LineIn.pStrmIn)
        {
            pDrv->pConnector->pfnDestroyIn(pDrv->pConnector, pDrv->LineIn.pStrmIn);
            LogFlowFunc(("LUN#%RU8: Destroyed input with rc=%Rrc\n", pDrv->uLUN, rc));
            pDrv->LineIn.pStrmIn = NULL;
        }

        if (RTStrAPrintf(&pszDesc, "[LUN#%RU8] %s", pDrv->uLUN, pszName) <= 0)
        {
            rc = VERR_NO_MEMORY;
            break;
        }

        rc = pDrv->pConnector->pfnCreateIn(pDrv->pConnector, pszDesc, enmRecSource, pCfg, &pDrv->LineIn.pStrmIn);
        LogFlowFunc(("LUN#%RU8: Created input \"%s\", with rc=%Rrc\n", pDrv->uLUN, pszDesc, rc));
        if (rc == VINF_SUCCESS) /* Note: Could return VWRN_ALREADY_EXISTS. */
        {
            rc = AudioMixerAddStreamIn(pSink,
                                       pDrv->pConnector, pDrv->LineIn.pStrmIn,
                                       0 /* uFlags */, &pDrv->LineIn.phStrmIn);
        }

        RTStrFree(pszDesc);
    }

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(int) hdaOpenOut(PHDASTATE pThis,
                                    const char *pszName, PPDMAUDIOSTREAMCFG pCfg)
{
    int rc = VINF_SUCCESS;
    char *pszDesc;

    PHDADRIVER pDrv;
    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
    {
        AudioMixerRemoveStream(pThis->pSinkOutput, pDrv->Out.phStrmOut);
        pDrv->Out.phStrmOut = NULL;

        if (pDrv->Out.pStrmOut)
        {
            pDrv->pConnector->pfnDestroyOut(pDrv->pConnector, pDrv->Out.pStrmOut);
            LogFlowFunc(("LUN#%RU8: Destroyed output with rc=%Rrc\n", pDrv->uLUN, rc));
            pDrv->Out.pStrmOut = NULL;
        }

        if (RTStrAPrintf(&pszDesc, "[LUN#%RU8] %s (%RU32Hz, %RU8 %s)",
                         pDrv->uLUN, pszName, pCfg->uHz, pCfg->cChannels, pCfg->cChannels > 1 ? "Channels" : "Channel") <= 0)
        {
            rc = VERR_NO_MEMORY;
            break;
        }

        rc = pDrv->pConnector->pfnCreateOut(pDrv->pConnector, pszDesc, pCfg, &pDrv->Out.pStrmOut);
        LogFlowFunc(("LUN#%RU8: Created output \"%s\", with rc=%Rrc\n", pDrv->uLUN, pszDesc, rc));
        if (rc == VINF_SUCCESS) /* Note: Could return VWRN_ALREADY_EXISTS. */
        {
            rc = AudioMixerAddStreamOut(pThis->pSinkOutput,
                                        pDrv->pConnector, pDrv->Out.pStrmOut,
                                        0 /* uFlags */, &pDrv->Out.phStrmOut);
        }

        RTStrFree(pszDesc);
    }

    LogFlowFuncLeaveRC(rc);
    return rc;
}

static DECLCALLBACK(int) hdaSetVolume(PHDASTATE pThis, ENMSOUNDSOURCE enmSource,
                                      bool fMute, uint8_t uVolLeft, uint8_t uVolRight)
{
    int             rc = VINF_SUCCESS;
    PDMAUDIOVOLUME  vol = { fMute, uVolLeft, uVolRight };
    PAUDMIXSINK     pSink;

    /* Convert the audio source to corresponding sink. */
    switch (enmSource)
    {
        case PO_INDEX:
            pSink = pThis->pSinkOutput;
            break;
        case PI_INDEX:
            pSink = pThis->pSinkLineIn;
            break;
        case MC_INDEX:
            pSink = pThis->pSinkMicIn;
            break;
        default:
            AssertFailedReturn(VERR_INVALID_PARAMETER);
            break;
    }

    /* Set the volume. Codec already converted it to the correct range. */
    AudioMixerSetSinkVolume(pSink, &vol);

    LogFlowFuncLeaveRC(rc);
    return rc;
}

#ifndef VBOX_WITH_AUDIO_CALLBACKS
static DECLCALLBACK(void) hdaTimer(PPDMDEVINS pDevIns, PTMTIMER pTimer, void *pvUser)
{
    RT_NOREF(pDevIns);
    PHDASTATE pThis = (PHDASTATE)pvUser;
    Assert(pThis == PDMINS_2_DATA(pDevIns, PHDASTATE));
    AssertPtr(pThis);

    STAM_PROFILE_START(&pThis->StatTimer, a);

#ifdef HDA_DEBUG_TIMING
    const uint64_t cTicksNow       = TMTimerGet(pTimer);
    const uint64_t cTicksPerSec    = TMTimerGetFreq(pTimer);

    const uint64_t cTicksElapsed   = cTicksNow - pThis->Out.State.uTimerTS;
    const uint64_t cTicksElapsedMs = cTicksElapsed / (cTicksPerSec / 1000);

    Log3Func(("%RU64 ticks elapsed -> %RU64ms\n", cTicksElapsed, cTicksElapsedMs));
#endif

    /*
     * Process all driver nodes.
     */
    uint32_t cbInMax  = 0;
    uint32_t cbOutMin = UINT32_MAX;

    PHDADRIVER pDrv;
    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
    {
        uint32_t cbIn = 0;
        uint32_t cbOut = 0;

        int rc2 = pDrv->pConnector->pfnQueryStatus(pDrv->pConnector, &cbIn, &cbOut, NULL /* pcSamplesLive */);
        if (RT_SUCCESS(rc2))
            rc2 = pDrv->pConnector->pfnPlayOut(pDrv->pConnector, NULL /* pcSamplesPlayed */);

        Log3Func(("LUN#%RU8: rc=%Rrc, cbIn=%RU32, cbOut=%RU32\n", pDrv->uLUN, rc2, cbIn, cbOut));

        /* If we there was an error handling (available) output or there simply is no output available,
         * then calculate the minimum data rate which must be processed by the device emulation in order
         * to function correctly.
         *
         * This is not the optimal solution, but as we have to deal with this on a timer-based approach
         * (until we have the audio callbacks) we need to have device' DMA engines running. */
        if (!pDrv->pConnector->pfnIsValidOut(pDrv->pConnector, pDrv->Out.pStrmOut))
        {
            /* Use the codec's (fixed) sampling rate. */
            cbOut = RT_MAX(cbOut, hdaStreamTransferGetElapsed(pThis, &pThis->Out));
        }

        cbOutMin = RT_MIN(cbOutMin, cbOut);
        cbInMax  = RT_MAX(cbInMax, cbIn);
    }

    Log3Func(("cbInMax=%RU32, cbOutMin=%RU32\n", cbInMax, cbOutMin));

    if (cbOutMin == UINT32_MAX)
        cbOutMin = 0;

    /*
     * Playback.
     */
    if (cbOutMin)
    {
        Assert(cbOutMin != UINT32_MAX);
        hdaStreamTransfer(pThis, &pThis->Out, cbOutMin /* cbToProcessMax */);

        int rc2 = hdaWriteAudio(pThis, &pThis->Out, cbOutMin);
        AssertRC(rc2);
    }

    /*
     * Recording (Line-In).
     */
    uint32_t cbRead;
    int rc2 = hdaReadAudio(pThis, &pThis->LineIn, pThis->pSinkLineIn, &cbRead);
    if (   RT_SUCCESS(rc2)
        && cbRead)
    {
        hdaStreamTransfer(pThis, &pThis->LineIn, cbRead);
    }

#ifdef HDA_DEBUG_TIMING
    pThis->Dbg.tsTimerLastCalledNs = cTicksNow;
#endif

    /* Kick the timer again. */
    pThis->tsTimerExpire += pThis->cTimerTicks;
    TMTimerSet(pTimer, pThis->tsTimerExpire);

    STAM_PROFILE_STOP(&pThis->StatTimer, a);
}

#else /* VBOX_WITH_AUDIO_CALLBACKS */

static DECLCALLBACK(int) hdaCallbackInput(PDMAUDIOCALLBACKTYPE enmType, void *pvCtx, size_t cbCtx, void *pvUser, size_t cbUser)
{
    Assert(enmType == PDMAUDIOCALLBACKTYPE_INPUT);
    AssertPtrReturn(pvCtx,  VERR_INVALID_POINTER);
    AssertReturn(cbCtx,     VERR_INVALID_PARAMETER);
    AssertPtrReturn(pvUser, VERR_INVALID_POINTER);
    AssertReturn(cbUser,    VERR_INVALID_PARAMETER);

    PHDACALLBACKCTX pCtx = (PHDACALLBACKCTX)pvCtx;
    AssertReturn(cbCtx == sizeof(HDACALLBACKCTX), VERR_INVALID_PARAMETER);

    PPDMAUDIOCALLBACKDATAIN pData = (PPDMAUDIOCALLBACKDATAIN)pvUser;
    AssertReturn(cbUser == sizeof(PDMAUDIOCALLBACKDATAIN), VERR_INVALID_PARAMETER);

    return hdaStreamTransfer(pCtx->pThis, PI_INDEX, UINT32_MAX, &pData->cbOutRead);
}

static DECLCALLBACK(int) hdaCallbackOutput(PDMAUDIOCALLBACKTYPE enmType, void *pvCtx, size_t cbCtx, void *pvUser, size_t cbUser)
{
    Assert(enmType == PDMAUDIOCALLBACKTYPE_OUTPUT);
    AssertPtrReturn(pvCtx,  VERR_INVALID_POINTER);
    AssertReturn(cbCtx,     VERR_INVALID_PARAMETER);
    AssertPtrReturn(pvUser, VERR_INVALID_POINTER);
    AssertReturn(cbUser,    VERR_INVALID_PARAMETER);

    PHDACALLBACKCTX pCtx = (PHDACALLBACKCTX)pvCtx;
    AssertReturn(cbCtx == sizeof(HDACALLBACKCTX), VERR_INVALID_PARAMETER);

    PPDMAUDIOCALLBACKDATAOUT pData = (PPDMAUDIOCALLBACKDATAOUT)pvUser;
    AssertReturn(cbUser == sizeof(PDMAUDIOCALLBACKDATAOUT), VERR_INVALID_PARAMETER);

    PHDASTATE pThis = pCtx->pThis;

    int rc = hdaStreamTransfer(pCtx->pThis, PO_INDEX, UINT32_MAX, &pData->cbOutWritten);
    if (   RT_SUCCESS(rc)
        && pData->cbOutWritten)
    {
        PHDADRIVER pDrv;
        RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
        {
            uint32_t cSamplesPlayed;
            int rc2 = pDrv->pConnector->pfnPlayOut(pDrv->pConnector, &cSamplesPlayed);
            LogFlowFunc(("LUN#%RU8: cSamplesPlayed=%RU32, rc=%Rrc\n", pDrv->uLUN, cSamplesPlayed, rc2));
        }
    }
}
#endif /* VBOX_WITH_AUDIO_CALLBACKS */

static uint32_t hdaStreamTransferGetElapsed(PHDASTATE pThis, PHDASTREAM pStream)
{
    const uint64_t cTicksNow     = TMTimerGet(pThis->pTimer);
    const uint64_t cTicksPerSec  = TMTimerGetFreq(pThis->pTimer);

    const uint64_t cTicksElapsed = cTicksNow - pStream->State.uTimerTS;
#ifdef DEBUG
    const uint64_t cMsElapsed    = cTicksElapsed / (cTicksPerSec / 1000);
#endif

    AssertPtr(pThis->pCodec);

    PDMAUDIOPCMPROPS strmProps;
    int rc = DrvAudioStreamCfgToProps(&pStream->State.strmCfg, &strmProps);
    AssertRC(rc);

    /* A stream *always* runs with 48 kHz device-wise, regardless of the actual stream input/output format (Hz) being set. */
    uint32_t csPerPeriod = (int)((strmProps.cChannels * cTicksElapsed * 48000 /* Hz */ + cTicksPerSec) / cTicksPerSec / 2);
    uint32_t cbPerPeriod = csPerPeriod << strmProps.cShift;

    Log3Func(("[SD%RU8] %RU64ms (%zu samples, %zu bytes) elapsed\n", pStream->u8SD, cMsElapsed, csPerPeriod, cbPerPeriod));

    return cbPerPeriod;
}

/**
 * Transfers data of an HDA stream according to its usage (input / output).
 *
 * For an SDO (output) stream this means reading DMA data from the device to
 * the HDA stream's internal FIFO buffer.
 *
 * For an SDI (input) stream this is reading audio data from the HDA stream's
 * internal FIFO buffer and writing it as DMA data to the device.
 *
 * @returns IPRT status code.
 * @param   pThis               HDA state.
 * @param   pStream             HDA stream to update.
 * @param   cbToProcessMax      Maximum of data (in bytes) to process.
 */
static void hdaStreamTransfer(PHDASTATE pThis, PHDASTREAM pStream, uint32_t cbToProcessMax)
{
    AssertPtrReturnVoid(pThis);
    AssertPtrReturnVoid(pStream);
    AssertReturnVoid(cbToProcessMax);

    Log3Func(("[SD%RU8] Enter ==========================================\n", pStream->u8SD));

    if (ASMAtomicReadBool(&pThis->fInReset)) /* HDA controller in reset mode? Bail out. */
    {
        LogFunc(("[SD%RU8] In reset mode, skipping\n", pStream->u8SD));
        return;
    }

    int  rc       = VINF_SUCCESS;
    bool fProceed = true;

    PHDASTREAMPERIOD pPeriod = &pStream->State.Period;

    rc = hdaStreamPeriodLock(pPeriod);
    AssertRCReturnVoid(rc);

    /* Stream not active? */
    if (!ASMAtomicReadBool(&pStream->State.fRunning))
    {
        Log3Func(("[SD%RU8] Not running\n", pStream->u8SD));
        fProceed = false;
    }
    /* Stream not running? */
    else if (!(HDA_STREAM_REG(pThis, CTL, pStream->u8SD) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN)))
    {
        Log3Func(("[SD%RU8] RUN bit not set\n", pStream->u8SD));
        fProceed = false;
    }
    /* Period complete? */
    else if (hdaStreamPeriodIsComplete(pPeriod))
    {
        Log3Func(("[SD%RU8] Period is complete, nothing to do\n", pStream->u8SD));
        fProceed = false;
    }

    if (!fProceed)
    {
        hdaStreamPeriodUnlock(pPeriod);
        return;
    }

    /* Sanity checks. */
    Assert(pStream->u8SD <= 7); /** @todo Use a define for MAX_STREAMS! */
    Assert(pStream->u64BDLBase);
    Assert(pStream->u32CBL);

    /* State sanity checks. */
    Assert(ASMAtomicReadBool(&pStream->State.fInReset) == false);

    /* Fetch first / next BDL entry. */
    PHDABDLE pBDLE = &pStream->State.BDLE;
    if (hdaBDLEIsComplete(pBDLE))
    {
        rc = hdaBDLEFetch(pThis, pBDLE, pStream->u64BDLBase, pStream->State.uCurBDLE);
        AssertRC(rc);
    }

    const uint32_t cbPeriodRemaining = hdaStreamPeriodGetRemainingFrames(pPeriod) * HDA_FRAME_SIZE;
    Assert(cbPeriodRemaining); /* Paranoia. */

    const uint32_t cbElapsed         = hdaStreamTransferGetElapsed(pThis, pStream);

    if (!cbElapsed)
    {
        hdaStreamPeriodUnlock(pPeriod);
        return;
    }

    /* Limit the data to read, as this routine could be delayed and therefore
     * report wrong (e.g. too much) cbElapsed bytes. */
    uint32_t cbLeft                  = RT_MIN(RT_MIN(cbPeriodRemaining, cbElapsed), cbToProcessMax);

    Log3Func(("[SD%RU8] cbPeriodRemaining=%RU32, cbElapsed=%RU32, cbToProcessMax=%RU32 -> cbLeft=%RU32\n",
              pStream->u8SD, cbPeriodRemaining, cbElapsed, cbToProcessMax, cbLeft));

    Assert(cbLeft % HDA_FRAME_SIZE == 0);

    while (cbLeft)
    {
        uint32_t cbChunk = RT_MIN(hdaStreamGetTransferSize(pThis, pStream), cbLeft);
        if (!cbChunk)
            break;

        uint32_t cbDMA   = 0;

        switch (pStream->u8SD)
        {
            case 0: /* Line-In */
#ifdef VBOX_WITH_HDA_MIC_IN
            case 2: /* Microphone-In */
#endif
            {
                rc = hdaDMAWrite(pThis, pStream, cbChunk, &cbDMA /* pcbWritten */);
                if (RT_FAILURE(rc))
                    LogRel(("HDA: Writing to stream #%RU8 DMA failed with %Rrc\n", pStream->u8SD, rc));
                break;
            }

            case 4: /* Output */
            {
                rc = hdaDMARead(pThis, pStream, cbChunk, &cbDMA /* pcbRead */);
                if (RT_FAILURE(rc))
                    LogRel(("HDA: Reading from stream #%RU8 DMA failed with %Rrc\n", pStream->u8SD, rc));
                break;
            }

            default:
                AssertMsgFailed(("Unsupported stream #%RU8\n", pStream->u8SD));
                rc = VERR_NOT_SUPPORTED;
                break;
        }

        if (cbDMA)
        {
            Assert(cbDMA % HDA_FRAME_SIZE == 0);

            /* We always increment the position of DMA buffer counter because we're always reading
             * into an intermediate buffer. */
            pBDLE->State.u32BufOff += (uint32_t)cbDMA;
            Assert(pBDLE->State.u32BufOff <= pBDLE->Desc.u32BufSize);

            hdaStreamTransferInc(pThis, pStream, cbDMA);

            uint32_t framesDMA = cbDMA / HDA_FRAME_SIZE;

            /* Add the transferred frames to the period. */
            hdaStreamPeriodInc(pPeriod, framesDMA);

            /* Save the timestamp of when the last successful DMA transfer has been for this stream. */
            pStream->State.uTimerTS = TMTimerGet(pThis->pTimer);

            Assert(cbLeft >= cbDMA);
            cbLeft        -= cbDMA;
        }

        if (hdaBDLEIsComplete(pBDLE))
        {
            Log3Func(("[SD%RU8] Complete: %R[bdle]\n", pStream->u8SD, pBDLE));

            if (hdaBDLENeedsInterrupt(pBDLE))
            {
                /* If the IOCE ("Interrupt On Completion Enable") bit of the SDCTL register is set
                 * we need to generate an interrupt.
                 */
                if (HDA_STREAM_REG(pThis, CTL, pStream->u8SD) & HDA_REG_FIELD_FLAG_MASK(SDCTL, ICE))
                    hdaStreamPeriodAcquireInterrupt(pPeriod);
            }

            if (pStream->State.uCurBDLE == pStream->u16LVI)
            {
                Assert(pStream->u32CBL == HDA_STREAM_REG(pThis, LPIB, pStream->u8SD));

                pStream->State.uCurBDLE = 0;
                hdaStreamUpdateLPIB(pThis, pStream, 0 /* LPIB */);
            }
            else
                pStream->State.uCurBDLE++;

            hdaBDLEFetch(pThis, pBDLE, pStream->u64BDLBase, pStream->State.uCurBDLE);

            Log3Func(("[SD%RU8] Fetching: %R[bdle]\n", pStream->u8SD, pBDLE));
        }

        if (RT_FAILURE(rc))
            break;
    }

    if (hdaStreamPeriodIsComplete(pPeriod))
    {
        Log3Func(("[SD%RU8] Period complete -- Current: %R[bdle]\n", pStream->u8SD, &pStream->State.BDLE));

        /* Set the stream's BCIS bit.
         *
         * Note: This only must be done if the whole period is complete, and not if only
         * one specific BDL entry is complete (if it has the IOC bit set).
         *
         * This will otherwise confuses the guest when it 1) deasserts the interrupt,
         * 2) reads SDSTS (with BCIS set) and then 3) too early reads a (wrong) WALCLK value.
         *
         * snd_hda_intel on Linux will tell. */
        HDA_STREAM_REG(pThis, STS, pStream->u8SD) |= HDA_REG_FIELD_FLAG_MASK(SDSTS, BCIS);

        /* Try updating the wall clock. */
        const uint64_t u64WalClk  = hdaStreamPeriodGetAbsElapsedWalClk(pPeriod);
        const bool     fWalClkSet = hdaWalClkSet(pThis, u64WalClk, false /* fForce */);

        /* Does the period have any interrupts outstanding? */
        if (hdaStreamPeriodNeedsInterrupt(pPeriod))
        {
            if (fWalClkSet)
            {
                Log3Func(("[SD%RU8] Set WALCLK to %RU64, triggering interrupt\n", pStream->u8SD, u64WalClk));

                /* Trigger an interrupt first and let hdaRegWriteSDSTS() deal with
                 * ending / beginning a period. */
#ifndef DEBUG
                hdaProcessInterrupt(pThis);
#else
                hdaProcessInterrupt(pThis, __FUNCTION__);
#endif
            }
        }
        else
        {
            /* End the period first ... */
            hdaStreamPeriodEnd(pPeriod);

            /* ... and immediately begin the next one. */
            hdaStreamPeriodBegin(pPeriod, hdaWalClkGetCurrent(pThis));
        }
    }

    hdaStreamPeriodUnlock(pPeriod);

    Log3Func(("[SD%RU8] Returning %Rrc ==========================================\n", pStream->u8SD, rc));

    if (RT_FAILURE(rc))
        LogFunc(("[SD%RU8] Failed with rc=%Rrcc\n", pStream->u8SD, rc));
}
#endif /* IN_RING3 */

/* MMIO callbacks */

/**
 * @callback_method_impl{FNIOMMMIOREAD, Looks up and calls the appropriate handler.}
 *
 * @note During implementation, we discovered so-called "forgotten" or "hole"
 *       registers whose description is not listed in the RPM, datasheet, or
 *       spec.
 */
PDMBOTHCBDECL(int) hdaMMIORead(PPDMDEVINS pDevIns, void *pvUser, RTGCPHYS GCPhysAddr, void *pv, unsigned cb)
{
    RT_NOREF(pvUser);
    PHDASTATE   pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);
    int         rc;

    /*
     * Look up and log.
     */
    uint32_t        offReg = GCPhysAddr - pThis->MMIOBaseAddr;
    int             idxRegDsc = hdaRegLookup(pThis, offReg);    /* Register descriptor index. */
#ifdef LOG_ENABLED
    unsigned const  cbLog     = cb;
    uint32_t        offRegLog = offReg;
#endif

    Log3Func(("offReg=%#x cb=%#x\n", offReg, cb));
    Assert(cb == 4); Assert((offReg & 3) == 0);

    if (pThis->fInReset && idxRegDsc != HDA_REG_GCTL)
        LogFunc(("Access to registers except GCTL is blocked while reset\n"));

    if (idxRegDsc == -1)
        LogRel(("HDA: Invalid read access @0x%x (bytes=%u)\n", offReg, cb));

    if (idxRegDsc != -1)
    {
        /* ASSUMES gapless DWORD at end of map. */
        if (g_aHdaRegMap[idxRegDsc].size == 4)
        {
            /*
             * Straight forward DWORD access.
             */
            rc = g_aHdaRegMap[idxRegDsc].pfnRead(pThis, idxRegDsc, (uint32_t *)pv);
            Log3Func(("\tRead %s => %x (%Rrc)\n", g_aHdaRegMap[idxRegDsc].abbrev, *(uint32_t *)pv, rc));
        }
        else
        {
            /*
             * Multi register read (unless there are trailing gaps).
             * ASSUMES that only DWORD reads have sideeffects.
             */
            uint32_t u32Value = 0;
            unsigned cbLeft   = 4;
            do
            {
                uint32_t const  cbReg        = g_aHdaRegMap[idxRegDsc].size;
                uint32_t        u32Tmp       = 0;

                rc = g_aHdaRegMap[idxRegDsc].pfnRead(pThis, idxRegDsc, &u32Tmp);
                Log3Func(("\tRead %s[%db] => %x (%Rrc)*\n", g_aHdaRegMap[idxRegDsc].abbrev, cbReg, u32Tmp, rc));
                if (rc != VINF_SUCCESS)
                    break;
                u32Value |= (u32Tmp & g_afMasks[cbReg]) << ((4 - cbLeft) * 8);

                cbLeft -= cbReg;
                offReg += cbReg;
                idxRegDsc++;
            } while (cbLeft > 0 && g_aHdaRegMap[idxRegDsc].offset == offReg);

            if (rc == VINF_SUCCESS)
                *(uint32_t *)pv = u32Value;
            else
                Assert(!IOM_SUCCESS(rc));
        }
    }
    else
    {
        rc = VINF_IOM_MMIO_UNUSED_FF;
        Log3Func(("\tHole at %x is accessed for read\n", offReg));
    }

    /*
     * Log the outcome.
     */
#ifdef LOG_ENABLED
    if (cbLog == 4)
        Log3Func(("\tReturning @%#05x -> %#010x %Rrc\n", offRegLog, *(uint32_t *)pv, rc));
    else if (cbLog == 2)
        Log3Func(("\tReturning @%#05x -> %#06x %Rrc\n", offRegLog, *(uint16_t *)pv, rc));
    else if (cbLog == 1)
        Log3Func(("\tReturning @%#05x -> %#04x %Rrc\n", offRegLog, *(uint8_t *)pv, rc));
#endif
    return rc;
}


DECLINLINE(int) hdaWriteReg(PHDASTATE pThis, int idxRegDsc, uint32_t u32Value, char const *pszLog)
{
    RT_NOREF(pszLog);
    if (pThis->fInReset && idxRegDsc != HDA_REG_GCTL)
    {
        LogRel2(("HDA: Access to register 0x%x is blocked while reset\n", idxRegDsc));
        return VINF_SUCCESS;
    }

#ifdef LOG_ENABLED
    uint32_t idxRegMem = g_aHdaRegMap[idxRegDsc].mem_idx;
    uint32_t const u32CurValue = pThis->au32Regs[idxRegMem];
#endif
    int rc = g_aHdaRegMap[idxRegDsc].pfnWrite(pThis, idxRegDsc, u32Value);
    LogFunc(("write %#x -> %s[%db]; %x => %x%s\n", u32Value, g_aHdaRegMap[idxRegDsc].abbrev,
             g_aHdaRegMap[idxRegDsc].size, u32CurValue, pThis->au32Regs[idxRegMem], pszLog));
    return rc;
}


/**
 * @callback_method_impl{FNIOMMMIOWRITE, Looks up and calls the appropriate handler.}
 */
PDMBOTHCBDECL(int) hdaMMIOWrite(PPDMDEVINS pDevIns, void *pvUser, RTGCPHYS GCPhysAddr, void const *pv, unsigned cb)
{
    RT_NOREF(pvUser);
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);
    int       rc;

    /*
     * The behavior of accesses that aren't aligned on natural boundraries is
     * undefined. Just reject them outright.
     */
    /** @todo IOM could check this, it could also split the 8 byte accesses for us. */
    Assert(cb == 1 || cb == 2 || cb == 4 || cb == 8);
    if (GCPhysAddr & (cb - 1))
        return PDMDevHlpDBGFStop(pDevIns, RT_SRC_POS, "misaligned write access: GCPhysAddr=%RGp cb=%u\n", GCPhysAddr, cb);

    /*
     * Look up and log the access.
     */
    uint32_t    offReg = GCPhysAddr - pThis->MMIOBaseAddr;
    int         idxRegDsc = hdaRegLookup(pThis, offReg);
    uint32_t    idxRegMem = idxRegDsc != -1 ? g_aHdaRegMap[idxRegDsc].mem_idx : UINT32_MAX;
    uint64_t    u64Value;
    if (cb == 4)        u64Value = *(uint32_t const *)pv;
    else if (cb == 2)   u64Value = *(uint16_t const *)pv;
    else if (cb == 1)   u64Value = *(uint8_t const *)pv;
    else if (cb == 8)   u64Value = *(uint64_t const *)pv;
    else
    {
        u64Value = 0;   /* shut up gcc. */
        AssertReleaseMsgFailed(("%u\n", cb));
    }

#ifdef LOG_ENABLED
    uint32_t const u32LogOldValue = idxRegDsc >= 0 ? pThis->au32Regs[idxRegMem] : UINT32_MAX;
    if (idxRegDsc == -1)
        LogFunc(("@%#05x u32=%#010x cb=%d\n", offReg, *(uint32_t const *)pv, cb));
    else if (cb == 4)
        LogFunc(("@%#05x u32=%#010x %s\n", offReg, *(uint32_t *)pv, g_aHdaRegMap[idxRegDsc].abbrev));
    else if (cb == 2)
        LogFunc(("@%#05x u16=%#06x (%#010x) %s\n", offReg, *(uint16_t *)pv, *(uint32_t *)pv, g_aHdaRegMap[idxRegDsc].abbrev));
    else if (cb == 1)
        LogFunc(("@%#05x u8=%#04x (%#010x) %s\n", offReg, *(uint8_t *)pv, *(uint32_t *)pv, g_aHdaRegMap[idxRegDsc].abbrev));

    if (idxRegDsc >= 0 && g_aHdaRegMap[idxRegDsc].size != cb)
        LogFunc(("\tsize=%RU32 != cb=%u!!\n", g_aHdaRegMap[idxRegDsc].size, cb));
#endif

    /*
     * Try for a direct hit first.
     */
    if (idxRegDsc != -1 && g_aHdaRegMap[idxRegDsc].size == cb)
    {
        rc = hdaWriteReg(pThis, idxRegDsc, u64Value, "");
#ifdef LOG_ENABLED
        LogFunc(("\t%#x -> %#x\n", u32LogOldValue, idxRegMem != UINT32_MAX ? pThis->au32Regs[idxRegMem] : UINT32_MAX));
#endif
    }
    /*
     * Partial or multiple register access, loop thru the requested memory.
     */
    else
    {
        /*
         * If it's an access beyond the start of the register, shift the input
         * value and fill in missing bits. Natural alignment rules means we
         * will only see 1 or 2 byte accesses of this kind, so no risk of
         * shifting out input values.
         */
        if (idxRegDsc == -1 && (idxRegDsc = hdaRegLookupWithin(pThis, offReg)) != -1)
        {
            uint32_t const cbBefore = offReg - g_aHdaRegMap[idxRegDsc].offset; Assert(cbBefore > 0 && cbBefore < 4);
            offReg    -= cbBefore;
            idxRegMem = g_aHdaRegMap[idxRegDsc].mem_idx;
            u64Value <<= cbBefore * 8;
            u64Value  |= pThis->au32Regs[idxRegMem] & g_afMasks[cbBefore];
            LogFunc(("\tWithin register, supplied %u leading bits: %#llx -> %#llx ...\n",
                     cbBefore * 8, ~g_afMasks[cbBefore] & u64Value, u64Value));
        }

        /* Loop thru the write area, it may cover multiple registers. */
        rc = VINF_SUCCESS;
        for (;;)
        {
            uint32_t cbReg;
            if (idxRegDsc != -1)
            {
                idxRegMem = g_aHdaRegMap[idxRegDsc].mem_idx;
                cbReg = g_aHdaRegMap[idxRegDsc].size;
                if (cb < cbReg)
                {
                    u64Value |= pThis->au32Regs[idxRegMem] & g_afMasks[cbReg] & ~g_afMasks[cb];
                    LogFunc(("\tSupplying missing bits (%#x): %#llx -> %#llx ...\n",
                             g_afMasks[cbReg] & ~g_afMasks[cb], u64Value & g_afMasks[cb], u64Value));
                }
#ifdef LOG_ENABLED
                uint32_t const u32LogOldVal = pThis->au32Regs[idxRegMem];
#endif
                rc = hdaWriteReg(pThis, idxRegDsc, u64Value, "*");
                LogFunc(("\t%#x -> %#x\n", u32LogOldVal, pThis->au32Regs[idxRegMem]));
            }
            else
            {
                LogRel(("HDA: Invalid write access @0x%x\n", offReg));
                cbReg = 1;
            }
            if (rc != VINF_SUCCESS)
                break;
            if (cbReg >= cb)
                break;

            /* Advance. */
            offReg += cbReg;
            cb     -= cbReg;
            u64Value >>= cbReg * 8;
            if (idxRegDsc == -1)
                idxRegDsc = hdaRegLookup(pThis, offReg);
            else
            {
                idxRegDsc++;
                if (   (unsigned)idxRegDsc >= RT_ELEMENTS(g_aHdaRegMap)
                    || g_aHdaRegMap[idxRegDsc].offset != offReg)
                {
                    idxRegDsc = -1;
                }
            }
        }
    }

    return rc;
}


/* PCI callback. */

#ifdef IN_RING3
/**
 * @callback_method_impl{FNPCIIOREGIONMAP}
 */
static DECLCALLBACK(int) hdaPciIoRegionMap(PPDMDEVINS pDevIns, PPDMPCIDEV pPciDev, uint32_t iRegion,
                                           RTGCPHYS GCPhysAddress, RTGCPHYS cb, PCIADDRESSSPACE enmType)
{
    RT_NOREF(iRegion, enmType);
    PHDASTATE pThis = RT_FROM_MEMBER(pPciDev, HDASTATE, PciDev);

    /*
     * 18.2 of the ICH6 datasheet defines the valid access widths as byte, word, and double word.
     *
     * Let IOM talk DWORDs when reading, saves a lot of complications. On
     * writing though, we have to do it all ourselves because of sideeffects.
     */
    Assert(enmType == PCI_ADDRESS_SPACE_MEM);
    int rc = PDMDevHlpMMIORegister(pDevIns, GCPhysAddress, cb, NULL /*pvUser*/,
                                     IOMMMIO_FLAGS_READ_DWORD
                                   | IOMMMIO_FLAGS_WRITE_PASSTHRU,
                                   hdaMMIOWrite, hdaMMIORead, "HDA");

    if (RT_FAILURE(rc))
        return rc;

    if (pThis->fR0Enabled)
    {
        rc = PDMDevHlpMMIORegisterR0(pDevIns, GCPhysAddress, cb, NIL_RTR0PTR /*pvUser*/,
                                     "hdaMMIOWrite", "hdaMMIORead");
        if (RT_FAILURE(rc))
            return rc;
    }

    if (pThis->fRCEnabled)
    {
        rc = PDMDevHlpMMIORegisterRC(pDevIns, GCPhysAddress, cb, NIL_RTRCPTR /*pvUser*/,
                                     "hdaMMIOWrite", "hdaMMIORead");
        if (RT_FAILURE(rc))
            return rc;
    }

    pThis->MMIOBaseAddr = GCPhysAddress;
    return VINF_SUCCESS;
}


/* Saved state callbacks. */

static int hdaSaveStream(PPDMDEVINS pDevIns, PSSMHANDLE pSSM, PHDASTREAM pStrm)
{
    RT_NOREF(pDevIns);
#ifdef VBOX_STRICT
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);
#endif

    Log2Func(("[SD%RU8]\n", pStrm->u8SD));

    /* Save stream ID. */
    int rc = SSMR3PutU8(pSSM, pStrm->u8SD);
    AssertRCReturn(rc, rc);
    Assert(pStrm->u8SD <= 7); /** @todo Use a define. */

    rc = SSMR3PutStructEx(pSSM, &pStrm->State, sizeof(HDASTREAMSTATE), 0 /*fFlags*/, g_aSSMStreamStateFields7, NULL);
    AssertRCReturn(rc, rc);

#ifdef VBOX_STRICT /* Sanity checks. */
    uint64_t u64BaseDMA = RT_MAKE_U64(HDA_STREAM_REG(pThis, BDPL, pStrm->u8SD),
                                      HDA_STREAM_REG(pThis, BDPU, pStrm->u8SD));
    uint16_t u16LVI     = HDA_STREAM_REG(pThis, LVI, pStrm->u8SD);
    uint32_t u32CBL     = HDA_STREAM_REG(pThis, CBL, pStrm->u8SD);

    Assert(u64BaseDMA == pStrm->u64BDLBase);
    Assert(u16LVI     == pStrm->u16LVI);
    Assert(u32CBL     == pStrm->u32CBL);
#endif

    rc = SSMR3PutStructEx(pSSM, &pStrm->State.BDLE.Desc, sizeof(HDABDLEDESC),
                          0 /*fFlags*/, g_aSSMBDLEDescFields7, NULL);
    AssertRCReturn(rc, rc);

    rc = SSMR3PutStructEx(pSSM, &pStrm->State.BDLE.State, sizeof(HDABDLESTATE),
                          0 /*fFlags*/, g_aSSMBDLEStateFields7, NULL);
    AssertRCReturn(rc, rc);

    rc = SSMR3PutStructEx(pSSM, &pStrm->State.Period, sizeof(HDASTREAMPERIOD),
                          0 /* fFlags */, g_aSSMStreamPeriodFields7, NULL);
    AssertRCReturn(rc, rc);

#ifdef VBOX_STRICT /* Sanity checks. */
    PHDABDLE pBDLE = &pStrm->State.BDLE;
    if (u64BaseDMA)
    {
        Assert(pStrm->State.uCurBDLE <= u16LVI);

        HDABDLE curBDLE;
        rc = hdaBDLEFetch(pThis, &curBDLE, u64BaseDMA, pStrm->State.uCurBDLE);
        AssertRC(rc);

        Assert(curBDLE.Desc.u32BufSize == pBDLE->Desc.u32BufSize);
        Assert(curBDLE.Desc.u64BufAdr  == pBDLE->Desc.u64BufAdr);
        Assert(curBDLE.Desc.fFlags     == pBDLE->Desc.fFlags);
    }
    else
    {
        Assert(pBDLE->Desc.u64BufAdr  == 0);
        Assert(pBDLE->Desc.u32BufSize == 0);
    }
#endif

    uint32_t cbCircBufSize = 0;
    uint32_t cbCircBufUsed = 0;

    if (pStrm->State.pCircBuf)
    {
        cbCircBufSize = (uint32_t)RTCircBufSize(pStrm->State.pCircBuf);
        cbCircBufUsed = (uint32_t)RTCircBufUsed(pStrm->State.pCircBuf);
    }

    rc = SSMR3PutU32(pSSM, cbCircBufSize);
    AssertRCReturn(rc, rc);

    rc = SSMR3PutU32(pSSM, cbCircBufUsed);
    AssertRCReturn(rc, rc);

    if (cbCircBufUsed)
    {
        /*
         * We now need to get the circular buffer's data without actually modifying
         * the internal read / used offsets -- otherwise we would end up with broken audio
         * data after saving the state.
         *
         * So get the current read offset and serialize the buffer data manually based on that.
         */
        size_t cbCircBufOffRead = RTCircBufOffsetRead(pStrm->State.pCircBuf);

        void  *pvBuf;
        size_t cbBuf;
        RTCircBufAcquireReadBlock(pStrm->State.pCircBuf, cbCircBufUsed, &pvBuf, &cbBuf);

        if (cbBuf)
        {
            size_t cbToRead = cbCircBufUsed;
            size_t cbEnd    = 0;

            if (cbCircBufUsed > cbCircBufOffRead)
                cbEnd = cbCircBufUsed - cbCircBufOffRead;

            if (cbEnd) /* Save end of buffer first. */
            {
                rc = SSMR3PutMem(pSSM, (uint8_t *)pvBuf + cbCircBufSize - cbEnd /* End of buffer */, cbEnd);
                AssertRCReturn(rc, rc);

                Assert(cbToRead >= cbEnd);
                cbToRead -= cbEnd;
            }

            if (cbToRead) /* Save remaining stuff at start of buffer (if any). */
            {
                rc = SSMR3PutMem(pSSM, (uint8_t *)pvBuf - cbCircBufUsed /* Start of buffer */, cbToRead);
                AssertRCReturn(rc, rc);
            }
        }

        RTCircBufReleaseReadBlock(pStrm->State.pCircBuf, 0 /* Don't advance read pointer -- see comment above */);
    }

    Log2Func(("[SD%RU8] LPIB=%RU32, CBL=%RU32, LVI=%RU32\n",
              pStrm->u8SD,
              HDA_STREAM_REG(pThis, LPIB, pStrm->u8SD), HDA_STREAM_REG(pThis, CBL, pStrm->u8SD), HDA_STREAM_REG(pThis, LVI, pStrm->u8SD)));

#ifdef LOG_ENABLED
    hdaBDLEDumpAll(pThis, pStrm->u64BDLBase, pStrm->u16LVI + 1);
#endif

    return rc;
}

/**
 * @callback_method_impl{FNSSMDEVSAVEEXEC}
 */
static DECLCALLBACK(int) hdaSaveExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM)
{
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    /* Save Codec nodes states. */
    hdaCodecSaveState(pThis->pCodec, pSSM);

    /* Save MMIO registers. */
    AssertCompile(RT_ELEMENTS(pThis->au32Regs) >= HDA_NREGS_SAVED);
    SSMR3PutU32(pSSM, RT_ELEMENTS(pThis->au32Regs));
    SSMR3PutMem(pSSM, pThis->au32Regs, sizeof(pThis->au32Regs));

    /* Save controller-specifc internals. */
    SSMR3PutU64(pSSM, pThis->u64WalClk);
    SSMR3PutU8(pSSM, pThis->u8IRQL);

    /* Save number of streams. */
#ifdef VBOX_WITH_HDA_MIC_IN
    SSMR3PutU32(pSSM, 3);
#else
    SSMR3PutU32(pSSM, 2);
#endif

    /* Save stream states. */
    int rc = hdaSaveStream(pDevIns, pSSM, &pThis->Out);
    AssertRCReturn(rc, rc);
#ifdef VBOX_WITH_HDA_MIC_IN
    rc = hdaSaveStream(pDevIns, pSSM, &pThis->MicIn);
    AssertRCReturn(rc, rc);
#endif
    rc = hdaSaveStream(pDevIns, pSSM, &pThis->LineIn);
    AssertRCReturn(rc, rc);

    Log2Func(("DPLBASE=%RU64, DPUBASE=%RU64, DMAPos=%RTbool\n",
              HDA_REG(pThis, DPLBASE), HDA_REG(pThis, DPUBASE), RT_BOOL(HDA_REG(pThis, DPLBASE) & RT_BIT_32(0))));

    return rc;
}


/**
 * Does required post processing when loading a saved state.
 *
 * @param   pThis               Pointer to HDA state.
 */
static int hdaLoadExecPost(PHDASTATE pThis)
{
    int rc = VINF_SUCCESS;

    bool fStartTimer = false; /* Whether to resume the device timer. */

    /*
     * Update stuff after the state changes.
     */
    bool fEnableIn = RT_BOOL(HDA_SDCTL(pThis, 0 /** @todo Use a define. */) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN));
    if (fEnableIn)
    {
        /* (Re-)initialize the stream with current values. */
        int rc2 = hdaStreamInit(pThis, &pThis->LineIn, 0 /** @todo Use a define. */);
        AssertRC(rc2);

        /* Set the running flag. */
        ASMAtomicXchgBool(&pThis->LineIn.State.fRunning, true);

        /* Resume the stream's period. */
        hdaStreamPeriodResume(&pThis->LineIn.State.Period);
    }
#ifdef VBOX_WITH_HDA_MIC_IN
    bool fEnableMicIn = RT_BOOL(HDA_SDCTL(pThis, 2 /** @todo Use a define. */) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN));
    if (fEnableMicIn)
    {
        /* (Re-)initialize the stream with current values. */
        int rc2 = hdaStreamInit(pThis, &pThis->MicIn, 2 /** @todo Use a define. */);
        AssertRC(rc2);

        /* Set the running flag. */
        ASMAtomicXchgBool(&pThis->MicIn.State.fRunning, true);

        /* Resume the stream's period. */
        hdaStreamPeriodResume(&pThis->MicIn.State.Period);
    }
#endif
    bool fEnableOut = RT_BOOL(HDA_SDCTL(pThis, 4 /** @todo Use a define. */) & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN));
    if (fEnableOut)
    {
        /* (Re-)initialize the stream with current values. */
        int rc2 = hdaStreamInit(pThis, &pThis->Out, 4 /** @todo Use a define. */);
        AssertRC(rc2);

        /* Set the running flag. */
        ASMAtomicXchgBool(&pThis->Out.State.fRunning, true);

        /* Resume the stream's period. */
        hdaStreamPeriodResume(&pThis->Out.State.Period);
    }

    PHDADRIVER pDrv;
    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
    {
        int rc2 = pDrv->pConnector->pfnEnableIn(pDrv->pConnector, pDrv->LineIn.pStrmIn, fEnableIn);
        if (RT_FAILURE(rc2))
            LogRel2(("Audio: Unable to resume line-in, rc=%Rrc\n", rc2));
#ifdef VBOX_WITH_HDA_MIC_IN
        rc2 = pDrv->pConnector->pfnEnableIn(pDrv->pConnector, pDrv->MicIn.pStrmIn, fEnableMicIn);
        if (RT_FAILURE(rc2))
            LogRel2(("Audio: Unable to resume microphone-in, rc=%Rrc\n", rc2));
#endif
        rc2 = pDrv->pConnector->pfnEnableOut(pDrv->pConnector, pDrv->Out.pStrmOut, fEnableOut);
        if (RT_FAILURE(rc2))
            LogRel2(("Audio: Unable to resume output, rc=%Rrc\n", rc2));
    }

#ifndef VBOX_WITH_AUDIO_CALLBACKS
    if (   fEnableIn
# ifdef VBOX_WITH_HDA_MIC_IN
        || fEnableMicIn
# endif
        || fEnableOut)
    {
        fStartTimer = true;
    }
#endif

#ifdef HDA_USE_DMA_ACCESS_HANDLER
    hdaStreamRegisterDMAHandlers(pThis, &pThis->Out);
#endif

#ifndef VBOX_WITH_AUDIO_CALLBACKS
    if (   fStartTimer
        && pThis->pTimer
        && !TMTimerIsActive(pThis->pTimer))
    {
        pThis->tsTimerExpire = TMTimerGet(pThis->pTimer) + pThis->cTimerTicks;

        /* Resume timer. */
        int rc2 = TMTimerSet(pThis->pTimer, pThis->tsTimerExpire);
        AssertRC(rc2);
    }
#endif

    LogFlowFuncLeaveRC(rc);
    return rc;
}


/**
 * Handles loading of all saved state versions older than the current one.
 *
 * @param   pThis               Pointer to HDA state.
 * @param   pSSM                Pointer to SSM handle.
 * @param   uVersion            Saved state version to load.
 * @param   uPass               Loading stage to handle.
 */
static int hdaLoadExecLegacy(PHDASTATE pThis, PSSMHANDLE pSSM, uint32_t uVersion, uint32_t uPass)
{
    RT_NOREF(uPass);

    int rc = VINF_SUCCESS;

    /*
     * Load MMIO registers.
     */
    uint32_t cRegs;
    switch (uVersion)
    {
        case HDA_SSM_VERSION_1:
            /* Starting with r71199, we would save 112 instead of 113
               registers due to some code cleanups.  This only affected trunk
               builds in the 4.1 development period. */
            cRegs = 113;
            if (SSMR3HandleRevision(pSSM) >= 71199)
            {
                uint32_t uVer = SSMR3HandleVersion(pSSM);
                if (   VBOX_FULL_VERSION_GET_MAJOR(uVer) == 4
                    && VBOX_FULL_VERSION_GET_MINOR(uVer) == 0
                    && VBOX_FULL_VERSION_GET_BUILD(uVer) >= 51)
                    cRegs = 112;
            }
            break;

        case HDA_SSM_VERSION_2:
        case HDA_SSM_VERSION_3:
            cRegs = 112;
            AssertCompile(RT_ELEMENTS(pThis->au32Regs) >= HDA_NREGS_SAVED);
            break;

        /* Since version 4 we store the register count to stay flexible. */
        case HDA_SSM_VERSION_4:
        case HDA_SSM_VERSION_5:
        case HDA_SSM_VERSION_6:
            rc = SSMR3GetU32(pSSM, &cRegs); AssertRCReturn(rc, rc);
            if (cRegs != RT_ELEMENTS(pThis->au32Regs))
                LogRel(("HDA: SSM version cRegs is %RU32, expected %RU32\n", cRegs, RT_ELEMENTS(pThis->au32Regs)));
            break;

        default:
            LogRel(("HDA: Unsupported / too new saved state version (%RU32)\n", uVersion));
            return VERR_SSM_UNSUPPORTED_DATA_UNIT_VERSION;
    }

    if (cRegs >= RT_ELEMENTS(pThis->au32Regs))
    {
        SSMR3GetMem(pSSM, pThis->au32Regs, sizeof(pThis->au32Regs));
        SSMR3Skip(pSSM, sizeof(uint32_t) * (cRegs - RT_ELEMENTS(pThis->au32Regs)));
    }
    else
        SSMR3GetMem(pSSM, pThis->au32Regs, sizeof(uint32_t) * cRegs);

    /* Make sure to update the base addresses first before initializing any streams down below. */
    pThis->u64CORBBase  = RT_MAKE_U64(HDA_REG(pThis, CORBLBASE), HDA_REG(pThis, CORBUBASE));
    pThis->u64RIRBBase  = RT_MAKE_U64(HDA_REG(pThis, RIRBLBASE), HDA_REG(pThis, RIRBUBASE));
    pThis->u64DPBase    = RT_MAKE_U64(HDA_REG(pThis, DPLBASE) & DPBASE_ADDR_MASK, HDA_REG(pThis, DPUBASE));

    /* Also make sure to update the DMA position bit if this was enabled when saving the state. */
    pThis->fDMAPosition = RT_BOOL(HDA_REG(pThis, DPLBASE) & RT_BIT_32(0));

    /*
     * Note: Saved states < v5 store LVI (u32BdleMaxCvi) for
     *       *every* BDLE state, whereas it only needs to be stored
     *       *once* for every stream. Most of the BDLE state we can
     *       get out of the registers anyway, so just ignore those values.
     *
     *       Also, only the current BDLE was saved, regardless whether
     *       there were more than one (and there are at least two entries,
     *       according to the spec).
     */
#define HDA_SSM_LOAD_BDLE_STATE_PRE_V5(v, x)                            \
    {                                                                       \
        rc = SSMR3Skip(pSSM, sizeof(uint32_t));        /* Begin marker */   \
        AssertRCReturn(rc, rc);                                             \
        rc = SSMR3GetU64(pSSM, &x.Desc.u64BufAdr);     /* u64BdleCviAddr */ \
        AssertRCReturn(rc, rc);                                             \
        rc = SSMR3Skip(pSSM, sizeof(uint32_t));        /* u32BdleMaxCvi */  \
        AssertRCReturn(rc, rc);                                             \
        rc = SSMR3GetU32(pSSM, &x.State.u32BDLIndex);  /* u32BdleCvi */     \
        AssertRCReturn(rc, rc);                                             \
        rc = SSMR3GetU32(pSSM, &x.Desc.u32BufSize);    /* u32BdleCviLen */  \
        AssertRCReturn(rc, rc);                                             \
        rc = SSMR3GetU32(pSSM, &x.State.u32BufOff);    /* u32BdleCviPos */  \
        AssertRCReturn(rc, rc);                                             \
        bool fIOC;                                                          \
        rc = SSMR3GetBool(pSSM, &fIOC);                /* fBdleCviIoc */    \
        AssertRCReturn(rc, rc);                                             \
        x.Desc.fFlags = fIOC ? HDA_BDLE_FLAG_IOC : 0;                       \
        rc = SSMR3GetU32(pSSM, &x.State.cbBelowFIFOW); /* cbUnderFifoW */   \
        AssertRCReturn(rc, rc);                                             \
        rc = SSMR3Skip(pSSM, sizeof(uint8_t) * 256);   /* FIFO */           \
        AssertRCReturn(rc, rc);                                             \
        rc = SSMR3Skip(pSSM, sizeof(uint32_t));        /* End marker */     \
        AssertRCReturn(rc, rc);                                             \
    }

    /*
     * Load BDLEs (Buffer Descriptor List Entries) and DMA counters.
     */
    switch (uVersion)
    {
        case HDA_SSM_VERSION_1:
        case HDA_SSM_VERSION_2:
        case HDA_SSM_VERSION_3:
        case HDA_SSM_VERSION_4:
        {
            /* Only load the internal states.
             * The rest will be initialized from the saved registers later. */

            /* Note 1: Only the *current* BDLE for a stream was saved! */
            /* Note 2: The stream's saving order is/was fixed, so don't touch! */

            /* Output */
            rc = hdaStreamInit(pThis, &pThis->Out,    4 /* Stream number, hardcoded */);
            if (RT_FAILURE(rc))
                break;
            HDA_SSM_LOAD_BDLE_STATE_PRE_V5(uVersion, pThis->Out.State.BDLE);
            pThis->Out.State.uCurBDLE = pThis->Out.State.BDLE.State.u32BDLIndex;

            /* Microphone-In */
            rc = hdaStreamInit(pThis, &pThis->MicIn,  2 /* Stream number, hardcoded */);
            if (RT_FAILURE(rc))
                break;
            HDA_SSM_LOAD_BDLE_STATE_PRE_V5(uVersion, pThis->MicIn.State.BDLE);
            pThis->MicIn.State.uCurBDLE = pThis->MicIn.State.BDLE.State.u32BDLIndex;

            /* Line-In */
            rc = hdaStreamInit(pThis, &pThis->LineIn, 0 /* Stream number, hardcoded */);
            if (RT_FAILURE(rc))
                break;
            HDA_SSM_LOAD_BDLE_STATE_PRE_V5(uVersion, pThis->LineIn.State.BDLE);
            pThis->LineIn.State.uCurBDLE = pThis->LineIn.State.BDLE.State.u32BDLIndex;
            break;
        }

#undef HDA_SSM_LOAD_BDLE_STATE_PRE_V5

        default: /* Since v5 we support flexible stream and BDLE counts. */
        {
            uint32_t cStreams;
            rc = SSMR3GetU32(pSSM, &cStreams);
            if (RT_FAILURE(rc))
                break;

            if (cStreams > 8)
                cStreams = 8; /* Sanity. */

            /* Load stream states. */
            for (uint32_t i = 0; i < cStreams; i++)
            {
                uint8_t uStreamID;
                rc = SSMR3GetU8(pSSM, &uStreamID);
                if (RT_FAILURE(rc))
                    break;

                PHDASTREAM pStrm = hdaStreamFromID(pThis, uStreamID);
                HDASTREAM  StreamDummy;

                if (!pStrm)
                {
                    pStrm = &StreamDummy;
                    LogRel2(("HDA: Warning: Stream ID=%RU32 not supported, skipping to load ...\n", uStreamID));
                }

                rc = hdaStreamInit(pThis, pStrm, uStreamID);
                if (RT_FAILURE(rc))
                {
                    LogRel(("HDA: Stream #%RU32: Initialization of stream %RU8 failed, rc=%Rrc\n", i, uStreamID, rc));
                    break;
                }

                /*
                 * Load BDLEs (Buffer Descriptor List Entries) and DMA counters.
                 */

                if (uVersion == HDA_SSM_VERSION_5)
                {
                    /* Get the current BDLE entry and skip the rest. */
                    uint16_t cBDLE;

                    rc = SSMR3Skip(pSSM, sizeof(uint32_t));         /* Begin marker */
                    AssertRC(rc);
                    rc = SSMR3GetU16(pSSM, &cBDLE);                 /* cBDLE */
                    AssertRC(rc);
                    rc = SSMR3GetU16(pSSM, &pStrm->State.uCurBDLE); /* uCurBDLE */
                    AssertRC(rc);
                    rc = SSMR3Skip(pSSM, sizeof(uint32_t));         /* End marker */
                    AssertRC(rc);

                    uint32_t u32BDLEIndex;
                    for (uint16_t a = 0; a < cBDLE; a++)
                    {
                        rc = SSMR3Skip(pSSM, sizeof(uint32_t));     /* Begin marker */
                        AssertRC(rc);
                        rc = SSMR3GetU32(pSSM, &u32BDLEIndex);      /* u32BDLIndex */
                        AssertRC(rc);

                        /* Does the current BDLE index match the current BDLE to process? */
                        if (u32BDLEIndex == pStrm->State.uCurBDLE)
                        {
                            rc = SSMR3GetU32(pSSM, &pStrm->State.BDLE.State.cbBelowFIFOW); /* cbBelowFIFOW */
                            AssertRC(rc);
                            rc = SSMR3Skip(pSSM, sizeof(uint8_t) * 256);                   /* FIFO, deprecated */
                            AssertRC(rc);
                            rc = SSMR3GetU32(pSSM, &pStrm->State.BDLE.State.u32BufOff);    /* u32BufOff */
                            AssertRC(rc);
                            rc = SSMR3Skip(pSSM, sizeof(uint32_t));                        /* End marker */
                            AssertRC(rc);
                        }
                        else /* Skip not current BDLEs. */
                        {
                            rc = SSMR3Skip(pSSM,   sizeof(uint32_t)      /* cbBelowFIFOW */
                                                 + sizeof(uint8_t) * 256 /* au8FIFO */
                                                 + sizeof(uint32_t)      /* u32BufOff */
                                                 + sizeof(uint32_t));    /* End marker */
                            AssertRC(rc);
                        }
                    }
                }
                else
                {
                    rc = SSMR3GetStructEx(pSSM, &pStrm->State, sizeof(HDASTREAMSTATE),
                                          0 /* fFlags */, g_aSSMStreamStateFields6, NULL);
                    if (RT_FAILURE(rc))
                        break;

                    /* Get HDABDLEDESC. */
                    uint32_t uMarker;
                    rc = SSMR3GetU32(pSSM, &uMarker);      /* Begin marker. */
                    AssertRC(rc);
                    Assert(uMarker == UINT32_C(0x19200102) /* SSMR3STRUCT_BEGIN */);
                    rc = SSMR3GetU64(pSSM, &pStrm->State.BDLE.Desc.u64BufAdr);
                    AssertRC(rc);
                    rc = SSMR3GetU32(pSSM, &pStrm->State.BDLE.Desc.u32BufSize);
                    AssertRC(rc);
                    bool fFlags = false;
                    rc = SSMR3GetBool(pSSM, &fFlags);      /* Saved states < v7 only stored the IOC as boolean flag. */
                    AssertRC(rc);
                    pStrm->State.BDLE.Desc.fFlags = fFlags ? HDA_BDLE_FLAG_IOC : 0;
                    rc = SSMR3GetU32(pSSM, &uMarker);      /* End marker. */
                    AssertRC(rc);
                    Assert(uMarker == UINT32_C(0x19920406) /* SSMR3STRUCT_END */);

                    rc = SSMR3GetStructEx(pSSM, &pStrm->State.BDLE.State, sizeof(HDABDLESTATE),
                                          0 /* fFlags */, g_aSSMBDLEStateFields6, NULL);
                    if (RT_FAILURE(rc))
                        break;

                    Log2Func(("[SD%RU8] LPIB=%RU32, CBL=%RU32, LVI=%RU32\n",
                              uStreamID,
                              HDA_STREAM_REG(pThis, LPIB, uStreamID), HDA_STREAM_REG(pThis, CBL, uStreamID), HDA_STREAM_REG(pThis, LVI, uStreamID)));
#ifdef LOG_ENABLED
                    hdaBDLEDumpAll(pThis, pStrm->u64BDLBase, pStrm->u16LVI + 1);
#endif
                }

                rc = hdaSDFMTToStrmCfg(HDA_STREAM_REG(pThis, FMT, uStreamID), &pStrm->State.strmCfg);
                if (RT_FAILURE(rc))
                {
                    LogRel(("HDA: Stream #%RU8: Loading format failed, rc=%Rrc\n", uStreamID, rc));
                    /* Continue. */
                }

            } /* for cStreams */
            break;
        } /* default */
    }

    return rc;
}

/**
 * @callback_method_impl{FNSSMDEVLOADEXEC}
 */
static DECLCALLBACK(int) hdaLoadExec(PPDMDEVINS pDevIns, PSSMHANDLE pSSM, uint32_t uVersion, uint32_t uPass)
{
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    Assert(uPass == SSM_PASS_FINAL); NOREF(uPass);

    LogRel2(("hdaLoadExec: uVersion=%RU32, uPass=0x%x\n", uVersion, uPass));

    /*
     * Load Codec nodes states.
     */
    int rc = hdaCodecLoadState(pThis->pCodec, pSSM, uVersion);
    if (RT_FAILURE(rc))
    {
        LogRel(("HDA: Failed loading codec state (version %RU32, pass 0x%x), rc=%Rrc\n", uVersion, uPass, rc));
        return rc;
    }

    if (uVersion < HDA_SSM_VERSION) /* Handle older saved states? */
    {
        rc = hdaLoadExecLegacy(pThis, pSSM, uVersion, uPass);
        if (RT_SUCCESS(rc))
            rc = hdaLoadExecPost(pThis);

        return rc;
    }

    /*
     * Load MMIO registers.
     */
    uint32_t cRegs;
    rc = SSMR3GetU32(pSSM, &cRegs); AssertRCReturn(rc, rc);
    if (cRegs != RT_ELEMENTS(pThis->au32Regs))
        LogRel(("HDA: SSM version cRegs is %RU32, expected %RU32\n", cRegs, RT_ELEMENTS(pThis->au32Regs)));

    if (cRegs >= RT_ELEMENTS(pThis->au32Regs))
    {
        SSMR3GetMem(pSSM, pThis->au32Regs, sizeof(pThis->au32Regs));
        SSMR3Skip(pSSM, sizeof(uint32_t) * (cRegs - RT_ELEMENTS(pThis->au32Regs)));
    }
    else
        SSMR3GetMem(pSSM, pThis->au32Regs, sizeof(uint32_t) * cRegs);

    /* Make sure to update the base addresses first before initializing any streams down below. */
    pThis->u64CORBBase  = RT_MAKE_U64(HDA_REG(pThis, CORBLBASE), HDA_REG(pThis, CORBUBASE));
    pThis->u64RIRBBase  = RT_MAKE_U64(HDA_REG(pThis, RIRBLBASE), HDA_REG(pThis, RIRBUBASE));
    pThis->u64DPBase    = RT_MAKE_U64(HDA_REG(pThis, DPLBASE) & DPBASE_ADDR_MASK, HDA_REG(pThis, DPUBASE));

    /* Also make sure to update the DMA position bit if this was enabled when saving the state. */
    pThis->fDMAPosition = RT_BOOL(HDA_REG(pThis, DPLBASE) & RT_BIT_32(0));

    /*
     * Load controller-specifc internals.
     * Don't annoy other team mates (forgot this for state v7).
     */
    if (   SSMR3HandleRevision(pSSM) >= 116273
        || SSMR3HandleVersion(pSSM)  >= VBOX_FULL_VERSION_MAKE(5, 1, 24))
    {
        rc = SSMR3GetU64(pSSM, &pThis->u64WalClk);
        AssertRC(rc);

        rc = SSMR3GetU8(pSSM, &pThis->u8IRQL);
        AssertRC(rc);
    }

    /*
     * Load streams.
     */
    uint32_t cStreams;
    rc = SSMR3GetU32(pSSM, &cStreams);
    AssertRC(rc);

    if (cStreams > 8)
        cStreams = 8; /* Sanity. */

    Log2Func(("cStreams=%RU32\n", cStreams));

    /* Load stream states. */
    for (uint32_t i = 0; i < cStreams; i++)
    {
        uint8_t uStreamID;
        rc = SSMR3GetU8(pSSM, &uStreamID);
        AssertRC(rc);

        PHDASTREAM pStrm = hdaStreamFromID(pThis, uStreamID);
        HDASTREAM  StreamDummy;

        if (!pStrm)
        {
            pStrm = &StreamDummy;
            LogRel2(("HDA: Warning: Loading of stream #%RU8 not supported, skipping to load ...\n", uStreamID));
        }

        rc = hdaStreamInit(pThis, pStrm, uStreamID);
        if (RT_FAILURE(rc))
        {
            LogRel(("HDA: Stream #%RU8: Loading initialization failed, rc=%Rrc\n", uStreamID, rc));
            /* Continue. */
        }

        /*
         * Load BDLEs (Buffer Descriptor List Entries) and DMA counters.
         */
        rc = SSMR3GetStructEx(pSSM, &pStrm->State, sizeof(HDASTREAMSTATE),
                              0 /* fFlags */, g_aSSMStreamStateFields7,
                              NULL);
        AssertRC(rc);

        rc = SSMR3GetStructEx(pSSM, &pStrm->State.BDLE.Desc, sizeof(HDABDLEDESC),
                              0 /* fFlags */, g_aSSMBDLEDescFields7, NULL);
        AssertRC(rc);

        rc = SSMR3GetStructEx(pSSM, &pStrm->State.BDLE.State, sizeof(HDABDLESTATE),
                              0 /* fFlags */, g_aSSMBDLEStateFields7, NULL);
        AssertRC(rc);

        /*
         * Load period state.
         * Don't annoy other team mates (forgot this for state v7).
         */
        hdaStreamPeriodInit(&pStrm->State.Period,
                            pStrm->u8SD, pStrm->u16LVI, pStrm->u32CBL, &pStrm->State.strmCfg);

        if (   SSMR3HandleRevision(pSSM) >= 116273
            || SSMR3HandleVersion(pSSM)  >= VBOX_FULL_VERSION_MAKE(5, 1, 24))
        {
            rc = SSMR3GetStructEx(pSSM, &pStrm->State.Period, sizeof(HDASTREAMPERIOD),
                                  0 /* fFlags */, g_aSSMStreamPeriodFields7, NULL);
            AssertRC(rc);
        }

        /*
         * Load internal (FIFO) buffer.
         */

        uint32_t cbCircBufSize = 0;
        rc = SSMR3GetU32(pSSM, &cbCircBufSize); /* cbCircBuf */
        AssertRC(rc);

        uint32_t cbCircBufUsed = 0;
        rc = SSMR3GetU32(pSSM, &cbCircBufUsed); /* cbCircBuf */
        AssertRC(rc);

        if (cbCircBufSize) /* If 0, skip the buffer. */
        {
            /* Paranoia. */
            AssertReleaseMsg(cbCircBufSize <= _1M,
                             ("HDA: Saved state contains bogus DMA buffer size (%RU32) for stream #%RU8",
                              cbCircBufSize, uStreamID));
            AssertReleaseMsg(cbCircBufUsed <= cbCircBufSize,
                             ("HDA: Saved state contains invalid DMA buffer usage (%RU32/%RU32) for stream #%RU8",
                              cbCircBufUsed, cbCircBufSize, uStreamID));
            AssertPtr(pStrm->State.pCircBuf);

            /* Do we need to cre-create the circular buffer do fit the data size? */
            if (cbCircBufSize != (uint32_t)RTCircBufSize(pStrm->State.pCircBuf))
            {
                RTCircBufDestroy(pStrm->State.pCircBuf);
                pStrm->State.pCircBuf = NULL;

                rc = RTCircBufCreate(&pStrm->State.pCircBuf, cbCircBufSize);
                AssertRC(rc);
            }

            if (   RT_SUCCESS(rc)
                && cbCircBufUsed)
            {
                void  *pvBuf;
                size_t cbBuf;

                RTCircBufAcquireWriteBlock(pStrm->State.pCircBuf, cbCircBufUsed, &pvBuf, &cbBuf);

                if (cbBuf)
                {
                    rc = SSMR3GetMem(pSSM, pvBuf, cbBuf);
                    AssertRC(rc);
                }

                RTCircBufReleaseWriteBlock(pStrm->State.pCircBuf, cbBuf);

                Assert(cbBuf == cbCircBufUsed);
            }
        }

        Log2Func(("[SD%RU8] LPIB=%RU32, CBL=%RU32, LVI=%RU32\n",
                  uStreamID,
                  HDA_STREAM_REG(pThis, LPIB, uStreamID), HDA_STREAM_REG(pThis, CBL, uStreamID), HDA_STREAM_REG(pThis, LVI, uStreamID)));
#ifdef LOG_ENABLED
        hdaBDLEDumpAll(pThis, pStrm->u64BDLBase, pStrm->u16LVI + 1);
#endif
        /** @todo (Re-)initialize active periods? */

    } /* for cStreams */

    rc = hdaLoadExecPost(pThis);
    AssertRC(rc);

    LogFlowFuncLeaveRC(rc);
    return rc;
}

/* Debug and log type formatters. */

/**
 * @callback_method_impl{FNRTSTRFORMATTYPE}
 */
static DECLCALLBACK(size_t) hdaDbgFmtBDLE(PFNRTSTROUTPUT pfnOutput, void *pvArgOutput,
                                           const char *pszType, void const *pvValue,
                                           int cchWidth, int cchPrecision, unsigned fFlags,
                                           void *pvUser)
{
    RT_NOREF(pszType, cchWidth, cchPrecision, fFlags, pvUser);
    PHDABDLE pBDLE = (PHDABDLE)pvValue;
    return RTStrFormat(pfnOutput,  pvArgOutput, NULL, 0,
                       "BDLE(idx:%RU32, off:%RU32, fifow:%RU32, DMA[%RU32 bytes @ 0x%x], fFlags=0x%x)",
                       pBDLE->State.u32BDLIndex, pBDLE->State.u32BufOff, pBDLE->State.cbBelowFIFOW,
                       pBDLE->Desc.u32BufSize, pBDLE->Desc.u64BufAdr, pBDLE->Desc.fFlags);
}

/**
 * @callback_method_impl{FNRTSTRFORMATTYPE}
 */
static DECLCALLBACK(size_t) hdaDbgFmtSDCTL(PFNRTSTROUTPUT pfnOutput, void *pvArgOutput,
                                           const char *pszType, void const *pvValue,
                                           int cchWidth, int cchPrecision, unsigned fFlags,
                                           void *pvUser)
{
    RT_NOREF(pszType, cchWidth, cchPrecision, fFlags, pvUser);
    uint32_t uSDCTL = (uint32_t)(uintptr_t)pvValue;
    return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                       "SDCTL(raw:%#x, DIR:%s, TP:%RTbool, STRIPE:%x, DEIE:%RTbool, FEIE:%RTbool, IOCE:%RTbool, RUN:%RTbool, RESET:%RTbool)",
                       uSDCTL,
                       (uSDCTL & HDA_REG_FIELD_FLAG_MASK(SDCTL, DIR)) ? "OUT" : "IN",
                       RT_BOOL(uSDCTL & HDA_REG_FIELD_FLAG_MASK(SDCTL, TP)),
                       (uSDCTL & HDA_REG_FIELD_MASK(SDCTL, STRIPE)) >> HDA_SDCTL_STRIPE_SHIFT,
                       RT_BOOL(uSDCTL & HDA_REG_FIELD_FLAG_MASK(SDCTL, DEIE)),
                       RT_BOOL(uSDCTL & HDA_REG_FIELD_FLAG_MASK(SDCTL, FEIE)),
                       RT_BOOL(uSDCTL & HDA_REG_FIELD_FLAG_MASK(SDCTL, ICE)),
                       RT_BOOL(uSDCTL & HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN)),
                       RT_BOOL(uSDCTL & HDA_REG_FIELD_FLAG_MASK(SDCTL, SRST)));
}

/**
 * @callback_method_impl{FNRTSTRFORMATTYPE}
 */
static DECLCALLBACK(size_t) hdaDbgFmtSDFIFOS(PFNRTSTROUTPUT pfnOutput, void *pvArgOutput,
                                             const char *pszType, void const *pvValue,
                                             int cchWidth, int cchPrecision, unsigned fFlags,
                                             void *pvUser)
{
    RT_NOREF(pszType, cchWidth, cchPrecision, fFlags, pvUser);
    uint32_t uSDFIFOS = (uint32_t)(uintptr_t)pvValue;
    return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "SDFIFOS(raw:%#x, sdfifos:%RU8 B)", uSDFIFOS, hdaSDFIFOSToBytes(uSDFIFOS));
}

/**
 * @callback_method_impl{FNRTSTRFORMATTYPE}
 */
static DECLCALLBACK(size_t) hdaDbgFmtSDFIFOW(PFNRTSTROUTPUT pfnOutput, void *pvArgOutput,
                                             const char *pszType, void const *pvValue,
                                             int cchWidth, int cchPrecision, unsigned fFlags,
                                             void *pvUser)
{
    RT_NOREF(pszType, cchWidth, cchPrecision, fFlags, pvUser);
    uint32_t uSDFIFOW = (uint32_t)(uintptr_t)pvValue;
    return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "SDFIFOW(raw: %#0x, sdfifow:%d B)", uSDFIFOW, hdaSDFIFOWToBytes(uSDFIFOW));
}

/**
 * @callback_method_impl{FNRTSTRFORMATTYPE}
 */
static DECLCALLBACK(size_t) hdaDbgFmtSDSTS(PFNRTSTROUTPUT pfnOutput, void *pvArgOutput,
                                           const char *pszType, void const *pvValue,
                                           int cchWidth, int cchPrecision, unsigned fFlags,
                                           void *pvUser)
{
    RT_NOREF(pszType, cchWidth, cchPrecision, fFlags, pvUser);
    uint32_t uSdSts = (uint32_t)(uintptr_t)pvValue;
    return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                       "SDSTS(raw:%#0x, fifordy:%RTbool, dese:%RTbool, fifoe:%RTbool, bcis:%RTbool)",
                       uSdSts,
                       RT_BOOL(uSdSts & HDA_REG_FIELD_FLAG_MASK(SDSTS, FIFORDY)),
                       RT_BOOL(uSdSts & HDA_REG_FIELD_FLAG_MASK(SDSTS, DE)),
                       RT_BOOL(uSdSts & HDA_REG_FIELD_FLAG_MASK(SDSTS, FE)),
                       RT_BOOL(uSdSts & HDA_REG_FIELD_FLAG_MASK(SDSTS, BCIS)));
}

static int hdaLookUpRegisterByName(const char *pszArgs)
{
    int iReg = 0;
    for (; iReg < HDA_NREGS; ++iReg)
        if (!RTStrICmp(g_aHdaRegMap[iReg].abbrev, pszArgs))
            return iReg;
    return -1;
}


static void hdaDbgPrintRegister(PHDASTATE pThis, PCDBGFINFOHLP pHlp, int iHdaIndex)
{
    Assert(   pThis
           && iHdaIndex >= 0
           && iHdaIndex < HDA_NREGS);
    pHlp->pfnPrintf(pHlp, "%s: 0x%x\n", g_aHdaRegMap[iHdaIndex].abbrev, pThis->au32Regs[g_aHdaRegMap[iHdaIndex].mem_idx]);
}

/**
 * @callback_method_impl{FNDBGFHANDLERDEV}
 */
static DECLCALLBACK(void) hdaInfo(PPDMDEVINS pDevIns, PCDBGFINFOHLP pHlp, const char *pszArgs)
{
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);
    int iHdaRegisterIndex = hdaLookUpRegisterByName(pszArgs);
    if (iHdaRegisterIndex != -1)
        hdaDbgPrintRegister(pThis, pHlp, iHdaRegisterIndex);
    else
        for(iHdaRegisterIndex = 0; (unsigned int)iHdaRegisterIndex < HDA_NREGS; ++iHdaRegisterIndex)
            hdaDbgPrintRegister(pThis, pHlp, iHdaRegisterIndex);
}

static void hdaDbgPrintStream(PHDASTATE pThis, PCDBGFINFOHLP pHlp, int iHdaStrmIndex)
{
    Assert(   pThis
           && iHdaStrmIndex >= 0
           && iHdaStrmIndex < 7);
    pHlp->pfnPrintf(pHlp, "Dump of stream #%02d:\n", iHdaStrmIndex);
    pHlp->pfnPrintf(pHlp, "SD%dCTL: %R[sdctl]\n",     iHdaStrmIndex, HDA_STREAM_REG(pThis, CTL, iHdaStrmIndex));
    pHlp->pfnPrintf(pHlp, "SD%dCTS: %R[sdsts]\n",     iHdaStrmIndex, HDA_STREAM_REG(pThis, STS, iHdaStrmIndex));
    pHlp->pfnPrintf(pHlp, "SD%dFIFOS: %R[sdfifos]\n", iHdaStrmIndex, HDA_STREAM_REG(pThis, FIFOS, iHdaStrmIndex));
    pHlp->pfnPrintf(pHlp, "SD%dFIFOW: %R[sdfifow]\n", iHdaStrmIndex, HDA_STREAM_REG(pThis, FIFOW, iHdaStrmIndex));
}

static int hdaLookUpStreamIndex(PHDASTATE pThis, const char *pszArgs)
{
    RT_NOREF(pThis, pszArgs);
    /** @todo add args parsing */
    return -1;
}

/**
 * @callback_method_impl{FNDBGFHANDLERDEV}
 */
static DECLCALLBACK(void) hdaInfoStream(PPDMDEVINS pDevIns, PCDBGFINFOHLP pHlp, const char *pszArgs)
{
    PHDASTATE   pThis         = PDMINS_2_DATA(pDevIns, PHDASTATE);
    int         iHdaStrmIndex = hdaLookUpStreamIndex(pThis, pszArgs);
    if (iHdaStrmIndex != -1)
        hdaDbgPrintStream(pThis, pHlp, iHdaStrmIndex);
    else
        for(iHdaStrmIndex = 0; iHdaStrmIndex < 8; ++iHdaStrmIndex)
            hdaDbgPrintStream(pThis, pHlp, iHdaStrmIndex);
}

/**
 * @callback_method_impl{FNDBGFHANDLERDEV}
 */
static DECLCALLBACK(void) hdaInfoCodecNodes(PPDMDEVINS pDevIns, PCDBGFINFOHLP pHlp, const char *pszArgs)
{
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    if (pThis->pCodec->pfnDbgListNodes)
        pThis->pCodec->pfnDbgListNodes(pThis->pCodec, pHlp, pszArgs);
    else
        pHlp->pfnPrintf(pHlp, "Codec implementation doesn't provide corresponding callback\n");
}

/**
 * @callback_method_impl{FNDBGFHANDLERDEV}
 */
static DECLCALLBACK(void) hdaInfoCodecSelector(PPDMDEVINS pDevIns, PCDBGFINFOHLP pHlp, const char *pszArgs)
{
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    if (pThis->pCodec->pfnDbgSelector)
        pThis->pCodec->pfnDbgSelector(pThis->pCodec, pHlp, pszArgs);
    else
        pHlp->pfnPrintf(pHlp, "Codec implementation doesn't provide corresponding callback\n");
}

/**
 * @callback_method_impl{FNDBGFHANDLERDEV}
 */
static DECLCALLBACK(void) hdaInfoMixer(PPDMDEVINS pDevIns, PCDBGFINFOHLP pHlp, const char *pszArgs)
{
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    if (pThis->pMixer)
        AudioMixerDebug(pThis->pMixer, pHlp, pszArgs);
    else
        pHlp->pfnPrintf(pHlp, "Mixer not available\n");
}


/* PDMIBASE */

/**
 * @interface_method_impl{PDMIBASE,pfnQueryInterface}
 */
static DECLCALLBACK(void *) hdaQueryInterface(struct PDMIBASE *pInterface, const char *pszIID)
{
    PHDASTATE pThis = RT_FROM_MEMBER(pInterface, HDASTATE, IBase);
    Assert(&pThis->IBase == pInterface);

    PDMIBASE_RETURN_INTERFACE(pszIID, PDMIBASE, &pThis->IBase);
    return NULL;
}


/* PDMDEVREG */

/**
 * Reset notification.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance data.
 *
 * @remark  The original sources didn't install a reset handler, but it seems to
 *          make sense to me so we'll do it.
 */
static DECLCALLBACK(void) hdaReset(PPDMDEVINS pDevIns)
{
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    HDA_REG(pThis, GCAP)     = HDA_MAKE_GCAP(4,4,0,0,1); /* see 6.2.1 */
    HDA_REG(pThis, VMIN)     = 0x00;                     /* see 6.2.2 */
    HDA_REG(pThis, VMAJ)     = 0x01;                     /* see 6.2.3 */
    HDA_REG(pThis, OUTPAY)   = 0x003C;                   /* see 6.2.4 */
    HDA_REG(pThis, INPAY)    = 0x001D;                   /* see 6.2.5 */
    HDA_REG(pThis, CORBSIZE) = 0x42;                     /* see 6.2.1 */
    HDA_REG(pThis, RIRBSIZE) = 0x42;                     /* see 6.2.1 */
    HDA_REG(pThis, CORBRP)   = 0x0;
    HDA_REG(pThis, RIRBWP)   = 0x0;

    LogFunc(("Resetting ...\n"));

# ifndef VBOX_WITH_AUDIO_CALLBACKS
    /*
     * Stop the timer, if any.
     */
    int rc2;
    if (   pThis->pTimer
        && TMTimerIsActive(pThis->pTimer))
    {
        rc2 = TMTimerStop(pThis->pTimer);
        AssertRC(rc2);
    }
# endif

    /*
     * Stop any audio currently playing and/or recording.
     */
    PHDADRIVER pDrv;
    RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
    {
        pDrv->pConnector->pfnEnableIn(pDrv->pConnector, pDrv->LineIn.pStrmIn, false /* Disable */);
# ifdef VBOX_WITH_HDA_MIC_IN
        /* Ignore rc. */
        pDrv->pConnector->pfnEnableIn(pDrv->pConnector, pDrv->MicIn.pStrmIn, false /* Disable */);
# endif
        /* Ditto. */
        pDrv->pConnector->pfnEnableOut(pDrv->pConnector, pDrv->Out.pStrmOut, false /* Disable */);
        /* Ditto. */
    }

    pThis->cbCorbBuf = 256 * sizeof(uint32_t); /** @todo Use a define here. */

    if (pThis->pu32CorbBuf)
        RT_BZERO(pThis->pu32CorbBuf, pThis->cbCorbBuf);
    else
        pThis->pu32CorbBuf = (uint32_t *)RTMemAllocZ(pThis->cbCorbBuf);

    pThis->cbRirbBuf = 256 * sizeof(uint64_t); /** @todo Use a define here. */
    if (pThis->pu64RirbBuf)
        RT_BZERO(pThis->pu64RirbBuf, pThis->cbRirbBuf);
    else
        pThis->pu64RirbBuf = (uint64_t *)RTMemAllocZ(pThis->cbRirbBuf);

    hdaWalClkSet(pThis, 0 /* Always start at 0 */, true /* fForce */);

    for (uint8_t uSD = 0; uSD < 8; uSD++) /** @todo Use a define here. */
    {
        PHDASTREAM pStream = NULL;
        if (uSD == 0)      /** @todo Implement dynamic stream IDs. */
            pStream = &pThis->LineIn;
# ifdef VBOX_WITH_HDA_MIC_IN
        else if (uSD == 2) /** @todo Implement dynamic stream IDs. */
            pStream = &pThis->MicIn;
# endif
        else if (uSD == 4) /** @todo Implement dynamic stream IDs. */
            pStream = &pThis->Out;

        if (pStream)
        {
            /* Remove the RUN bit from SDnCTL in case the stream was in a running state before. */
            HDA_STREAM_REG(pThis, CTL, uSD) &= ~HDA_REG_FIELD_FLAG_MASK(SDCTL, RUN);

            hdaStreamReset(pThis, pStream, uSD);
        }
    }

    /* Emulation of codec "wake up" (HDA spec 5.5.1 and 6.5). */
    HDA_REG(pThis, STATESTS) = 0x1;

    LogRel(("HDA: Reset\n"));
}

/**
 * @interface_method_impl{PDMDEVREG,pfnDestruct}
 */
static DECLCALLBACK(int) hdaDestruct(PPDMDEVINS pDevIns)
{
    PDMDEV_CHECK_VERSIONS_RETURN_QUIET(pDevIns);
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    PHDADRIVER pDrv;
    while (!RTListIsEmpty(&pThis->lstDrv))
    {
        pDrv = RTListGetFirst(&pThis->lstDrv, HDADRIVER, Node);

        RTListNodeRemove(&pDrv->Node);
        RTMemFree(pDrv);
    }

    if (pThis->pMixer)
    {
        AudioMixerDestroy(pThis->pMixer);
        pThis->pMixer = NULL;
    }

    if (pThis->pCodec)
    {
        int rc = hdaCodecDestruct(pThis->pCodec);
        AssertRC(rc);

        RTMemFree(pThis->pCodec);
        pThis->pCodec = NULL;
    }

    RTMemFree(pThis->pu32CorbBuf);
    pThis->pu32CorbBuf = NULL;

    RTMemFree(pThis->pu64RirbBuf);
    pThis->pu64RirbBuf = NULL;

    hdaStreamDestroy(&pThis->LineIn);
#ifdef VBOX_WITH_HDA_MIC_IN
    hdaStreamDestroy(&pThis->MicIn);
#endif
    hdaStreamDestroy(&pThis->Out);

    return VINF_SUCCESS;
}


/**
 * @interface_method_impl{PDMDEVREG,pfnPowerOff}
 */
static DECLCALLBACK(void) hdaPowerOff(PPDMDEVINS pDevIns)
{
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    hdaCloseIn (pThis, PDMAUDIORECSOURCE_LINE_IN);
    hdaCloseOut(pThis);
}


/**
 * Attach command, internal version.
 *
 * This is called to let the device attach to a driver for a specified LUN
 * during runtime. This is not called during VM construction, the device
 * constructor has to attach to all the available drivers.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance.
 * @param   pDrv        Driver to (re-)use for (re-)attaching to.
 *                      If NULL is specified, a new driver will be created and appended
 *                      to the driver list.
 * @param   uLUN        The logical unit which is being detached.
 * @param   fFlags      Flags, combination of the PDMDEVATT_FLAGS_* \#defines.
 */
static int hdaAttachInternal(PPDMDEVINS pDevIns, PHDADRIVER pDrv, unsigned uLUN, uint32_t fFlags)
{
    RT_NOREF(fFlags);
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);

    /*
     * Attach driver.
     */
    char *pszDesc;
    if (RTStrAPrintf(&pszDesc, "Audio driver port (HDA) for LUN#%u", uLUN) <= 0)
        AssertLogRelFailedReturn(VERR_NO_MEMORY);

    PPDMIBASE pDrvBase;
    int rc = PDMDevHlpDriverAttach(pDevIns, uLUN,
                                   &pThis->IBase, &pDrvBase, pszDesc);
    if (RT_SUCCESS(rc))
    {
        if (pDrv == NULL)
            pDrv = (PHDADRIVER)RTMemAllocZ(sizeof(HDADRIVER));
        if (pDrv)
        {
            pDrv->pDrvBase   = pDrvBase;
            pDrv->pConnector = PDMIBASE_QUERY_INTERFACE(pDrvBase, PDMIAUDIOCONNECTOR);
            AssertMsg(pDrv->pConnector != NULL, ("Configuration error: LUN#%u has no host audio interface, rc=%Rrc\n", uLUN, rc));
            pDrv->pHDAState  = pThis;
            pDrv->uLUN       = uLUN;

            /*
             * For now we always set the driver at LUN 0 as our primary
             * host backend. This might change in the future.
             */
            if (pDrv->uLUN == 0)
                pDrv->fFlags |= PDMAUDIODRVFLAG_PRIMARY;

            LogFunc(("LUN#%u: pCon=%p, drvFlags=0x%x\n", uLUN, pDrv->pConnector, pDrv->fFlags));

            /* Attach to driver list if not attached yet. */
            if (!pDrv->fAttached)
            {
                RTListAppend(&pThis->lstDrv, &pDrv->Node);
                pDrv->fAttached = true;
            }
        }
        else
            rc = VERR_NO_MEMORY;
    }
    else if (rc == VERR_PDM_NO_ATTACHED_DRIVER)
        LogFunc(("No attached driver for LUN #%u\n", uLUN));

    if (RT_FAILURE(rc))
    {
        /* Only free this string on failure;
         * must remain valid for the live of the driver instance. */
        RTStrFree(pszDesc);
    }

    LogFunc(("uLUN=%u, fFlags=0x%x, rc=%Rrc\n", uLUN, fFlags, rc));
    return rc;
}

/**
 * Attach command.
 *
 * This is called to let the device attach to a driver for a specified LUN
 * during runtime. This is not called during VM construction, the device
 * constructor has to attach to all the available drivers.
 *
 * @returns VBox status code.
 * @param   pDevIns     The device instance.
 * @param   uLUN        The logical unit which is being detached.
 * @param   fFlags      Flags, combination of the PDMDEVATT_FLAGS_* \#defines.
 */
static DECLCALLBACK(int) hdaAttach(PPDMDEVINS pDevIns, unsigned uLUN, uint32_t fFlags)
{
    return hdaAttachInternal(pDevIns, NULL /* pDrv */, uLUN, fFlags);
}

static DECLCALLBACK(void) hdaDetach(PPDMDEVINS pDevIns, unsigned uLUN, uint32_t fFlags)
{
    RT_NOREF(pDevIns, uLUN, fFlags);
    LogFunc(("iLUN=%u, fFlags=0x%x\n", uLUN, fFlags));
}

/**
 * Re-attaches a new driver to the device's driver chain.
 *
 * @returns VBox status code.
 * @param   pThis       Device instance to re-attach driver to.
 * @param   pDrv        Driver instance used for attaching to.
 *                      If NULL is specified, a new driver will be created and appended
 *                      to the driver list.
 * @param   uLUN        The logical unit which is being re-detached.
 * @param   pszDriver   Driver name.
 */
static int hdaReattach(PHDASTATE pThis, PHDADRIVER pDrv, uint8_t uLUN, const char *pszDriver)
{
    AssertPtrReturn(pThis,     VERR_INVALID_POINTER);
    AssertPtrReturn(pszDriver, VERR_INVALID_POINTER);

    PVM pVM = PDMDevHlpGetVM(pThis->pDevInsR3);
    PCFGMNODE pRoot = CFGMR3GetRoot(pVM);
    PCFGMNODE pDev0 = CFGMR3GetChild(pRoot, "Devices/hda/0/");

    /* Remove LUN branch. */
    CFGMR3RemoveNode(CFGMR3GetChildF(pDev0, "LUN#%u/", uLUN));

    if (pDrv)
    {
        /* Re-use a driver instance => detach the driver before. */
        int rc = PDMDevHlpDriverDetach(pThis->pDevInsR3, PDMIBASE_2_PDMDRV(pDrv->pDrvBase), 0 /* fFlags */);
        if (RT_FAILURE(rc))
            return rc;
    }

#define RC_CHECK() if (RT_FAILURE(rc)) { AssertReleaseRC(rc); break; }

    int rc = VINF_SUCCESS;
    do
    {
        PCFGMNODE pLunL0;
        rc = CFGMR3InsertNodeF(pDev0, &pLunL0, "LUN#%u/", uLUN);        RC_CHECK();
        rc = CFGMR3InsertString(pLunL0, "Driver",       "AUDIO");       RC_CHECK();
        rc = CFGMR3InsertNode(pLunL0,   "Config/",       NULL);         RC_CHECK();

        PCFGMNODE pLunL1, pLunL2;
        rc = CFGMR3InsertNode  (pLunL0, "AttachedDriver/", &pLunL1);    RC_CHECK();
        rc = CFGMR3InsertNode  (pLunL1,  "Config/",        &pLunL2);    RC_CHECK();
        rc = CFGMR3InsertString(pLunL1,  "Driver",          pszDriver); RC_CHECK();

        rc = CFGMR3InsertString(pLunL2, "AudioDriver", pszDriver);      RC_CHECK();

    } while (0);

    if (RT_SUCCESS(rc))
        rc = hdaAttachInternal(pThis->pDevInsR3, pDrv, uLUN, 0 /* fFlags */);

    LogFunc(("pThis=%p, uLUN=%u, pszDriver=%s, rc=%Rrc\n", pThis, uLUN, pszDriver, rc));

#undef RC_CHECK

    return rc;
}

/**
 * @interface_method_impl{PDMDEVREG,pfnConstruct}
 */
static DECLCALLBACK(int) hdaConstruct(PPDMDEVINS pDevIns, int iInstance, PCFGMNODE pCfg)
{
    RT_NOREF(iInstance);
    PDMDEV_CHECK_VERSIONS_RETURN(pDevIns);
    PHDASTATE pThis = PDMINS_2_DATA(pDevIns, PHDASTATE);
    Assert(iInstance == 0);

    /*
     * Validations.
     */
    if (!CFGMR3AreValuesValid(pCfg, "R0Enabled\0"
                                    "RCEnabled\0"
                                    "TimerHz\0"))
        return PDMDEV_SET_ERROR(pDevIns, VERR_PDM_DEVINS_UNKNOWN_CFG_VALUES,
                                N_ ("Invalid configuration for the Intel HDA device"));

    int rc = CFGMR3QueryBoolDef(pCfg, "RCEnabled", &pThis->fRCEnabled, false);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("HDA configuration error: failed to read RCEnabled as boolean"));
    rc = CFGMR3QueryBoolDef(pCfg, "R0Enabled", &pThis->fR0Enabled, false);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("HDA configuration error: failed to read R0Enabled as boolean"));
#ifndef VBOX_WITH_AUDIO_CALLBACKS
    uint16_t uTimerHz;
    rc = CFGMR3QueryU16Def(pCfg, "TimerHz", &uTimerHz, HDA_TIMER_HZ);
    if (RT_FAILURE(rc))
        return PDMDEV_SET_ERROR(pDevIns, rc,
                                N_("HDA configuration error: failed to read Hertz (Hz) rate as unsigned integer"));
#endif

    /*
     * Initialize data (most of it anyway).
     */
    pThis->pDevInsR3                = pDevIns;
    pThis->pDevInsR0                = PDMDEVINS_2_R0PTR(pDevIns);
    pThis->pDevInsRC                = PDMDEVINS_2_RCPTR(pDevIns);
    /* IBase */
    pThis->IBase.pfnQueryInterface  = hdaQueryInterface;

    /* PCI Device */
    PCIDevSetVendorId           (&pThis->PciDev, HDA_PCI_VENDOR_ID); /* nVidia */
    PCIDevSetDeviceId           (&pThis->PciDev, HDA_PCI_DEVICE_ID); /* HDA */

    PCIDevSetCommand            (&pThis->PciDev, 0x0000); /* 04 rw,ro - pcicmd. */
    PCIDevSetStatus             (&pThis->PciDev, VBOX_PCI_STATUS_CAP_LIST); /* 06 rwc?,ro? - pcists. */
    PCIDevSetRevisionId         (&pThis->PciDev, 0x01);   /* 08 ro - rid. */
    PCIDevSetClassProg          (&pThis->PciDev, 0x00);   /* 09 ro - pi. */
    PCIDevSetClassSub           (&pThis->PciDev, 0x03);   /* 0a ro - scc; 03 == HDA. */
    PCIDevSetClassBase          (&pThis->PciDev, 0x04);   /* 0b ro - bcc; 04 == multimedia. */
    PCIDevSetHeaderType         (&pThis->PciDev, 0x00);   /* 0e ro - headtyp. */
    PCIDevSetBaseAddress        (&pThis->PciDev, 0,       /* 10 rw - MMIO */
                                 false /* fIoSpace */, false /* fPrefetchable */, true /* f64Bit */, 0x00000000);
    PCIDevSetInterruptLine      (&pThis->PciDev, 0x00);   /* 3c rw. */
    PCIDevSetInterruptPin       (&pThis->PciDev, 0x01);   /* 3d ro - INTA#. */

#if defined(HDA_AS_PCI_EXPRESS)
    PCIDevSetCapabilityList     (&pThis->PciDev, 0x80);
#elif defined(VBOX_WITH_MSI_DEVICES)
    PCIDevSetCapabilityList     (&pThis->PciDev, 0x60);
#else
    PCIDevSetCapabilityList     (&pThis->PciDev, 0x50);   /* ICH6 datasheet 18.1.16 */
#endif

    /// @todo r=michaln: If there are really no PCIDevSetXx for these, the meaning
    /// of these values needs to be properly documented!
    /* HDCTL off 0x40 bit 0 selects signaling mode (1-HDA, 0 - Ac97) 18.1.19 */
    PCIDevSetByte(&pThis->PciDev, 0x40, 0x01);

    /* Power Management */
    PCIDevSetByte(&pThis->PciDev, 0x50 + 0, VBOX_PCI_CAP_ID_PM);
    PCIDevSetByte(&pThis->PciDev, 0x50 + 1, 0x0); /* next */
    PCIDevSetWord(&pThis->PciDev, 0x50 + 2, VBOX_PCI_PM_CAP_DSI | 0x02 /* version, PM1.1 */ );

#ifdef HDA_AS_PCI_EXPRESS
    /* PCI Express */
    PCIDevSetByte(&pThis->PciDev, 0x80 + 0, VBOX_PCI_CAP_ID_EXP); /* PCI_Express */
    PCIDevSetByte(&pThis->PciDev, 0x80 + 1, 0x60); /* next */
    /* Device flags */
    PCIDevSetWord(&pThis->PciDev, 0x80 + 2,
                   /* version */ 0x1     |
                   /* Root Complex Integrated Endpoint */ (VBOX_PCI_EXP_TYPE_ROOT_INT_EP << 4) |
                   /* MSI */ (100) << 9 );
    /* Device capabilities */
    PCIDevSetDWord(&pThis->PciDev, 0x80 + 4, VBOX_PCI_EXP_DEVCAP_FLRESET);
    /* Device control */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 8, 0);
    /* Device status */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 10, 0);
    /* Link caps */
    PCIDevSetDWord(&pThis->PciDev, 0x80 + 12, 0);
    /* Link control */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 16, 0);
    /* Link status */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 18, 0);
    /* Slot capabilities */
    PCIDevSetDWord(&pThis->PciDev, 0x80 + 20, 0);
    /* Slot control */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 24, 0);
    /* Slot status */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 26, 0);
    /* Root control */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 28, 0);
    /* Root capabilities */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 30, 0);
    /* Root status */
    PCIDevSetDWord(&pThis->PciDev, 0x80 + 32, 0);
    /* Device capabilities 2 */
    PCIDevSetDWord(&pThis->PciDev, 0x80 + 36, 0);
    /* Device control 2 */
    PCIDevSetQWord(&pThis->PciDev, 0x80 + 40, 0);
    /* Link control 2 */
    PCIDevSetQWord(&pThis->PciDev, 0x80 + 48, 0);
    /* Slot control 2 */
    PCIDevSetWord( &pThis->PciDev, 0x80 + 56, 0);
#endif

    /*
     * Register the PCI device.
     */
    rc = PDMDevHlpPCIRegister(pDevIns, &pThis->PciDev);
    if (RT_FAILURE(rc))
        return rc;

    rc = PDMDevHlpPCIIORegionRegister(pDevIns, 0, 0x4000, PCI_ADDRESS_SPACE_MEM, hdaPciIoRegionMap);
    if (RT_FAILURE(rc))
        return rc;

#ifdef VBOX_WITH_MSI_DEVICES
    PDMMSIREG MsiReg;
    RT_ZERO(MsiReg);
    MsiReg.cMsiVectors    = 1;
    MsiReg.iMsiCapOffset  = 0x60;
    MsiReg.iMsiNextOffset = 0x50;
    rc = PDMDevHlpPCIRegisterMsi(pDevIns, &MsiReg);
    if (RT_FAILURE(rc))
    {
        /* That's OK, we can work without MSI */
        PCIDevSetCapabilityList(&pThis->PciDev, 0x50);
    }
#endif

    rc = PDMDevHlpSSMRegister(pDevIns, HDA_SSM_VERSION, sizeof(*pThis), hdaSaveExec, hdaLoadExec);
    if (RT_FAILURE(rc))
        return rc;

    RTListInit(&pThis->lstDrv);

    uint8_t uLUN;
    for (uLUN = 0; uLUN < UINT8_MAX; ++uLUN)
    {
        LogFunc(("Trying to attach driver for LUN #%RU32 ...\n", uLUN));
        rc = hdaAttachInternal(pDevIns, NULL /* pDrv */, uLUN, 0 /* fFlags */);
        if (RT_FAILURE(rc))
        {
            if (rc == VERR_PDM_NO_ATTACHED_DRIVER)
                rc = VINF_SUCCESS;
            else if (rc == VERR_AUDIO_BACKEND_INIT_FAILED)
            {
                hdaReattach(pThis, NULL /* pDrv */, uLUN, "NullAudio");
                PDMDevHlpVMSetRuntimeError(pDevIns, 0 /*fFlags*/, "HostAudioNotResponding",
                    N_("No audio devices could be opened. Selecting the NULL audio backend "
                       "with the consequence that no sound is audible"));
                /* attaching to the NULL audio backend will never fail */
                rc = VINF_SUCCESS;
            }
            break;
        }
    }

    LogFunc(("cLUNs=%RU8, rc=%Rrc\n", uLUN, rc));

    if (RT_SUCCESS(rc))
    {
        rc = AudioMixerCreate("HDA Mixer", 0 /* uFlags */, &pThis->pMixer);
        if (RT_SUCCESS(rc))
        {
            /* Set a default audio format for our mixer. */
            PDMAUDIOSTREAMCFG streamCfg;
            streamCfg.uHz           = 44100;
            streamCfg.cChannels     = 2;
            streamCfg.enmFormat     = AUD_FMT_S16;
            streamCfg.enmEndianness = PDMAUDIOHOSTENDIANNESS;

            rc = AudioMixerSetDeviceFormat(pThis->pMixer, &streamCfg);
            AssertRC(rc);

            /* Add all required audio sinks. */
            rc = AudioMixerAddSink(pThis->pMixer, "[Playback] PCM Output",
                                   AUDMIXSINKDIR_OUTPUT, &pThis->pSinkOutput);
            AssertRC(rc);

            rc = AudioMixerAddSink(pThis->pMixer, "[Recording] Line In",
                                   AUDMIXSINKDIR_INPUT, &pThis->pSinkLineIn);
            AssertRC(rc);

            rc = AudioMixerAddSink(pThis->pMixer, "[Recording] Microphone In",
                                   AUDMIXSINKDIR_INPUT, &pThis->pSinkMicIn);
            AssertRC(rc);

            /* There is no master volume control. Set the master to max. */
            PDMAUDIOVOLUME vol = { false, 255, 255 };
            rc = AudioMixerSetMasterVolume(pThis->pMixer, &vol);
            AssertRC(rc);
        }
    }

    if (RT_SUCCESS(rc))
    {
        /* Construct codec. */
        pThis->pCodec = (PHDACODEC)RTMemAllocZ(sizeof(HDACODEC));
        if (!pThis->pCodec)
            return PDMDEV_SET_ERROR(pDevIns, VERR_NO_MEMORY, N_("Out of memory allocating HDA codec state"));

        /* Audio driver callbacks for multiplexing. */
        pThis->pCodec->pfnCloseIn   = hdaCloseIn;
        pThis->pCodec->pfnCloseOut  = hdaCloseOut;
        pThis->pCodec->pfnOpenIn    = hdaOpenIn;
        pThis->pCodec->pfnOpenOut   = hdaOpenOut;
        pThis->pCodec->pfnReset     = hdaCodecReset;
        pThis->pCodec->pfnSetVolume = hdaSetVolume;

        pThis->pCodec->pHDAState = pThis; /* Assign HDA controller state to codec. */

        /* Construct the codec. */
        rc = hdaCodecConstruct(pDevIns, pThis->pCodec, 0 /* Codec index */, pCfg);
        if (RT_FAILURE(rc))
            AssertRCReturn(rc, rc);

        /* ICH6 datasheet defines 0 values for SVID and SID (18.1.14-15), which together with values returned for
           verb F20 should provide device/codec recognition. */
        Assert(pThis->pCodec->u16VendorId);
        Assert(pThis->pCodec->u16DeviceId);
        PCIDevSetSubSystemVendorId(&pThis->PciDev, pThis->pCodec->u16VendorId); /* 2c ro - intel.) */
        PCIDevSetSubSystemId(      &pThis->PciDev, pThis->pCodec->u16DeviceId); /* 2e ro. */
    }

    if (RT_SUCCESS(rc))
    {
        rc = hdaStreamCreate(&pThis->LineIn);
        AssertRC(rc);
#ifdef VBOX_WITH_HDA_MIC_IN
        rc = hdaStreamCreate(&pThis->MicIn);
        AssertRC(rc);
#endif
        rc = hdaStreamCreate(&pThis->Out);
        AssertRC(rc);

        PHDADRIVER pDrv;
        RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
        {
            /*
             * Only primary drivers are critical for the VM to run. Everything else
             * might not worth showing an own error message box in the GUI.
             */
            if (!(pDrv->fFlags & PDMAUDIODRVFLAG_PRIMARY))
                continue;

            PPDMIAUDIOCONNECTOR pCon = pDrv->pConnector;
            AssertPtr(pCon);

            bool fValidLineIn = pCon->pfnIsValidIn(pCon, pDrv->LineIn.pStrmIn);
#ifdef VBOX_WITH_HDA_MIC_IN
            bool fValidMicIn  = pCon->pfnIsValidIn (pCon, pDrv->MicIn.pStrmIn);
#endif
            bool fValidOut    = pCon->pfnIsValidOut(pCon, pDrv->Out.pStrmOut);

            if (    !fValidLineIn
#ifdef VBOX_WITH_HDA_MIC_IN
                 && !fValidMicIn
#endif
                 && !fValidOut)
            {
                LogRel(("HDA: Falling back to NULL backend (no sound audible)\n"));

                hdaReset(pDevIns);
                hdaReattach(pThis, pDrv, pDrv->uLUN, "NullAudio");

                PDMDevHlpVMSetRuntimeError(pDevIns, 0 /*fFlags*/, "HostAudioNotResponding",
                    N_("No audio devices could be opened. Selecting the NULL audio backend "
                       "with the consequence that no sound is audible"));
            }
            else
            {
                bool fWarn = false;

                PDMAUDIOBACKENDCFG backendCfg;
                int rc2 = pCon->pfnGetConfiguration(pCon, &backendCfg);
                if (RT_SUCCESS(rc2))
                {
                    if (backendCfg.cMaxHstStrmsIn)
                    {
#ifdef VBOX_WITH_HDA_MIC_IN
                        /* If the audio backend supports two or more input streams at once,
                         * warn if one of our two inputs (microphone-in and line-in) failed to initialize. */
                        if (backendCfg.cMaxHstStrmsIn >= 2)
                            fWarn = !fValidLineIn || !fValidMicIn;
                        /* If the audio backend only supports one input stream at once (e.g. pure ALSA, and
                         * *not* ALSA via PulseAudio plugin!), only warn if both of our inputs failed to initialize.
                         * One of the two simply is not in use then. */
                        else if (backendCfg.cMaxHstStrmsIn == 1)
                            fWarn = !fValidLineIn && !fValidMicIn;
                        /* Don't warn if our backend is not able of supporting any input streams at all. */
#else
                        /* We only have line-in as input source. */
                        fWarn = !fValidLineIn;
#endif
                    }

                    if (   !fWarn
                        && backendCfg.cMaxHstStrmsOut)
                    {
                        fWarn = !fValidOut;
                    }
                }
                else
                    AssertReleaseMsgFailed(("Unable to retrieve audio backend configuration for LUN #%RU8, rc=%Rrc\n",
                                            pDrv->uLUN, rc2));

                if (fWarn)
                {
                    char   szMissingStreams[255];
                    size_t len = 0;
                    if (!fValidLineIn)
                    {
                        LogRel(("HDA: WARNING: Unable to open PCM line input for LUN #%RU8!\n", pDrv->uLUN));
                        len = RTStrPrintf(szMissingStreams, sizeof(szMissingStreams), "PCM Input");
                    }
#ifdef VBOX_WITH_HDA_MIC_IN
                    if (!fValidMicIn)
                    {
                        LogRel(("HDA: WARNING: Unable to open PCM microphone input for LUN #%RU8!\n", pDrv->uLUN));
                        len += RTStrPrintf(szMissingStreams + len,
                                           sizeof(szMissingStreams) - len, len ? ", PCM Microphone" : "PCM Microphone");
                    }
#endif
                    if (!fValidOut)
                    {
                        LogRel(("HDA: WARNING: Unable to open PCM output for LUN #%RU8!\n", pDrv->uLUN));
                        len += RTStrPrintf(szMissingStreams + len,
                                           sizeof(szMissingStreams) - len, len ? ", PCM Output" : "PCM Output");
                    }

                    PDMDevHlpVMSetRuntimeError(pDevIns, 0 /*fFlags*/, "HostAudioNotResponding",
                                               N_("Some HDA audio streams (%s) could not be opened. Guest applications generating audio "
                                                  "output or depending on audio input may hang. Make sure your host audio device "
                                                  "is working properly. Check the logfile for error messages of the audio "
                                                  "subsystem"), szMissingStreams);
                }
            }
        }
    }

    if (RT_SUCCESS(rc))
    {
        hdaReset(pDevIns);

        /*
         * 18.2.6,7 defines that values of this registers might be cleared on power on/reset
         * hdaReset shouldn't affects these registers.
         */
        HDA_REG(pThis, WAKEEN)   = 0x0;
        HDA_REG(pThis, STATESTS) = 0x0;

        /*
         * Debug and string formatter types.
         */
        PDMDevHlpDBGFInfoRegister(pDevIns, "hda",         "HDA info. (hda [register case-insensitive])",    hdaInfo);
        PDMDevHlpDBGFInfoRegister(pDevIns, "hdastream",   "HDA stream info. (hdastream [stream number])",   hdaInfoStream);
        PDMDevHlpDBGFInfoRegister(pDevIns, "hdcnodes",    "HDA codec nodes.",                               hdaInfoCodecNodes);
        PDMDevHlpDBGFInfoRegister(pDevIns, "hdcselector", "HDA codec's selector states [node number].",     hdaInfoCodecSelector);
        PDMDevHlpDBGFInfoRegister(pDevIns, "hdamixer",    "HDA mixer state.",                               hdaInfoMixer);

        rc = RTStrFormatTypeRegister("bdle",    hdaDbgFmtBDLE,    NULL);
        AssertRC(rc);
        rc = RTStrFormatTypeRegister("sdctl",   hdaDbgFmtSDCTL,   NULL);
        AssertRC(rc);
        rc = RTStrFormatTypeRegister("sdsts",   hdaDbgFmtSDSTS,   NULL);
        AssertRC(rc);
        rc = RTStrFormatTypeRegister("sdfifos", hdaDbgFmtSDFIFOS, NULL);
        AssertRC(rc);
        rc = RTStrFormatTypeRegister("sdfifow", hdaDbgFmtSDFIFOW, NULL);
        AssertRC(rc);

        /*
         * Some debug assertions.
         */
        for (unsigned i = 0; i < RT_ELEMENTS(g_aHdaRegMap); i++)
        {
            struct HDAREGDESC const *pReg     = &g_aHdaRegMap[i];
            struct HDAREGDESC const *pNextReg = i + 1 < RT_ELEMENTS(g_aHdaRegMap) ?  &g_aHdaRegMap[i + 1] : NULL;

            /* binary search order. */
            AssertReleaseMsg(!pNextReg || pReg->offset + pReg->size <= pNextReg->offset,
                             ("[%#x] = {%#x LB %#x}  vs. [%#x] = {%#x LB %#x}\n",
                              i, pReg->offset, pReg->size, i + 1, pNextReg->offset, pNextReg->size));

            /* alignment. */
            AssertReleaseMsg(   pReg->size == 1
                             || (pReg->size == 2 && (pReg->offset & 1) == 0)
                             || (pReg->size == 3 && (pReg->offset & 3) == 0)
                             || (pReg->size == 4 && (pReg->offset & 3) == 0),
                             ("[%#x] = {%#x LB %#x}\n", i, pReg->offset, pReg->size));

            /* registers are packed into dwords - with 3 exceptions with gaps at the end of the dword. */
            AssertRelease(((pReg->offset + pReg->size) & 3) == 0 || pNextReg);
            if (pReg->offset & 3)
            {
                struct HDAREGDESC const *pPrevReg = i > 0 ?  &g_aHdaRegMap[i - 1] : NULL;
                AssertReleaseMsg(pPrevReg, ("[%#x] = {%#x LB %#x}\n", i, pReg->offset, pReg->size));
                if (pPrevReg)
                    AssertReleaseMsg(pPrevReg->offset + pPrevReg->size == pReg->offset,
                                     ("[%#x] = {%#x LB %#x}  vs. [%#x] = {%#x LB %#x}\n",
                                      i - 1, pPrevReg->offset, pPrevReg->size, i + 1, pReg->offset, pReg->size));
            }
#if 0
            if ((pReg->offset + pReg->size) & 3)
            {
                AssertReleaseMsg(pNextReg, ("[%#x] = {%#x LB %#x}\n", i, pReg->offset, pReg->size));
                if (pNextReg)
                    AssertReleaseMsg(pReg->offset + pReg->size == pNextReg->offset,
                                     ("[%#x] = {%#x LB %#x}  vs. [%#x] = {%#x LB %#x}\n",
                                      i, pReg->offset, pReg->size, i + 1,  pNextReg->offset, pNextReg->size));
            }
#endif
            /* The final entry is a full DWORD, no gaps! Allows shortcuts. */
            AssertReleaseMsg(pNextReg || ((pReg->offset + pReg->size) & 3) == 0,
                             ("[%#x] = {%#x LB %#x}\n", i, pReg->offset, pReg->size));
        }
    }

# ifndef VBOX_WITH_AUDIO_CALLBACKS
    if (RT_SUCCESS(rc))
    {
        /* Create the emulation timer.
         *
         * Note:  Use TMCLOCK_VIRTUAL_SYNC here, as the guest's HDA driver
         *        relies on exact (virtual) DMA timing and uses DMA Position Buffers
         *        instead of the LPIB registers.
         */
        rc = PDMDevHlpTMTimerCreate(pDevIns, TMCLOCK_VIRTUAL_SYNC, hdaTimer, pThis,
                                    TMTIMER_FLAGS_NO_CRIT_SECT, "DevIchHda", &pThis->pTimer);
        AssertRCReturn(rc, rc);

        if (RT_SUCCESS(rc))
        {
            pThis->cTimerTicks = TMTimerGetFreq(pThis->pTimer) / uTimerHz;
            LogFunc(("Timer ticks=%RU64 (%RU16 Hz)\n", pThis->cTimerTicks, uTimerHz));
        }
    }
# else
    if (RT_SUCCESS(rc))
    {
        PHDADRIVER pDrv;
        RTListForEach(&pThis->lstDrv, pDrv, HDADRIVER, Node)
        {
            /* Only register primary driver.
             * The device emulation does the output multiplexing then. */
            if (pDrv->fFlags != PDMAUDIODRVFLAG_PRIMARY)
                continue;

            PDMAUDIOCALLBACK AudioCallbacks[2];

            HDACALLBACKCTX Ctx = { pThis, pDrv };

            AudioCallbacks[0].enmType     = PDMAUDIOCALLBACKTYPE_INPUT;
            AudioCallbacks[0].pfnCallback = hdaCallbackInput;
            AudioCallbacks[0].pvCtx       = &Ctx;
            AudioCallbacks[0].cbCtx       = sizeof(HDACALLBACKCTX);

            AudioCallbacks[1].enmType     = PDMAUDIOCALLBACKTYPE_OUTPUT;
            AudioCallbacks[1].pfnCallback = hdaCallbackOutput;
            AudioCallbacks[1].pvCtx       = &Ctx;
            AudioCallbacks[1].cbCtx       = sizeof(HDACALLBACKCTX);

            rc = pDrv->pConnector->pfnRegisterCallbacks(pDrv->pConnector, AudioCallbacks, RT_ELEMENTS(AudioCallbacks));
            if (RT_FAILURE(rc))
                break;
        }
    }
# endif

# ifdef VBOX_WITH_STATISTICS
    if (RT_SUCCESS(rc))
    {
        /*
         * Register statistics.
         */
#  ifndef VBOX_WITH_AUDIO_CALLBACKS
        PDMDevHlpSTAMRegister(pDevIns, &pThis->StatTimer,            STAMTYPE_PROFILE, "/Devices/HDA/Timer",             STAMUNIT_TICKS_PER_CALL, "Profiling hdaTimer.");
#  endif
        PDMDevHlpSTAMRegister(pDevIns, &pThis->StatBytesRead,        STAMTYPE_COUNTER, "/Devices/HDA/BytesRead"   ,      STAMUNIT_BYTES,          "Bytes read from HDA emulation.");
        PDMDevHlpSTAMRegister(pDevIns, &pThis->StatBytesWritten,     STAMTYPE_COUNTER, "/Devices/HDA/BytesWritten",      STAMUNIT_BYTES,          "Bytes written to HDA emulation.");
    }
# endif

#ifdef VBOX_AUDIO_DEBUG_DUMP_PCM_DATA
    RTFileDelete(VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaDMAAccessRead.pcm");
    RTFileDelete(VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaDMAAccessWrite.pcm");
    RTFileDelete(VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaDMARead.pcm");
    RTFileDelete(VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaDMAWrite.pcm");
    RTFileDelete(VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaReadAudio.pcm");
    RTFileDelete(VBOX_AUDIO_DEBUG_DUMP_PCM_DATA_PATH "hdaWriteAudio.pcm");
#endif

    LogFlowFuncLeaveRC(rc);
    return rc;
}

/**
 * The device registration structure.
 */
const PDMDEVREG g_DeviceICH6_HDA =
{
    /* u32Version */
    PDM_DEVREG_VERSION,
    /* szName */
    "hda",
    /* szRCMod */
    "VBoxDDRC.rc",
    /* szR0Mod */
    "VBoxDDR0.r0",
    /* pszDescription */
    "Intel HD Audio Controller",
    /* fFlags */
    PDM_DEVREG_FLAGS_DEFAULT_BITS | PDM_DEVREG_FLAGS_RC | PDM_DEVREG_FLAGS_R0,
    /* fClass */
    PDM_DEVREG_CLASS_AUDIO,
    /* cMaxInstances */
    1,
    /* cbInstance */
    sizeof(HDASTATE),
    /* pfnConstruct */
    hdaConstruct,
    /* pfnDestruct */
    hdaDestruct,
    /* pfnRelocate */
    NULL,
    /* pfnMemSetup */
    NULL,
    /* pfnPowerOn */
    NULL,
    /* pfnReset */
    hdaReset,
    /* pfnSuspend */
    NULL,
    /* pfnResume */
    NULL,
    /* pfnAttach */
    hdaAttach,
    /* pfnDetach */
    hdaDetach,
    /* pfnQueryInterface. */
    NULL,
    /* pfnInitComplete */
    NULL,
    /* pfnPowerOff */
    hdaPowerOff,
    /* pfnSoftReset */
    NULL,
    /* u32VersionEnd */
    PDM_DEVREG_VERSION
};

#endif /* IN_RING3 */
#endif /* !VBOX_DEVICE_STRUCT_TESTCASE */
