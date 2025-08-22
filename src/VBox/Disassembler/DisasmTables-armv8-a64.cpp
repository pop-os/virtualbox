/* $Id: DisasmTables-armv8-a64.cpp $ */
/** @file
 * VBox disassembler - Tables for ARMv8 A64.
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
#include <VBox/dis.h>
#include <VBox/disopcode-armv8.h>
#include "DisasmInternal-armv8.h"


/*********************************************************************************************************************************
*   Global Variables                                                                                                             *
*********************************************************************************************************************************/

#define DIS_ARMV8_OP(a_fValue, a_szOpcode, a_uOpcode, a_fOpType) \
    { a_fValue, 0, NULL, OP(a_szOpcode, 0, 0, 0, a_uOpcode, 0, 0, 0, a_fOpType) }
#define DIS_ARMV8_OP_EX(a_fValue, a_szOpcode, a_uOpcode, a_fOpType, a_fFlags) \
    { a_fValue, a_fFlags, NULL, OP(a_szOpcode, 0, 0, 0, a_uOpcode, 0, 0, 0, a_fOpType) }
#define DIS_ARMV8_OP_ALT_DECODE(a_fValue, a_szOpcode, a_uOpcode, a_fOpType, a_aAltDecode) \
    { a_fValue, 0, &g_aArmV8A64Insn ## a_aAltDecode ## Decode[0], OP(a_szOpcode, 0, 0, 0, a_uOpcode, 0, 0, 0, a_fOpType) }


#ifndef DIS_CORE_ONLY
static char g_szInvalidOpcode[] = "Invalid Opcode";
#endif

#define INVALID_OPCODE  \
    DIS_ARMV8_OP(0, g_szInvalidOpcode,    OP_ARMV8_INVALID, DISOPTYPE_INVALID)


/* Invalid opcode */
DECL_HIDDEN_CONST(DISOPCODE) g_ArmV8A64InvalidOpcode[1] =
{
    OP(g_szInvalidOpcode, 0, 0, 0, 0, 0, 0, 0, DISOPTYPE_INVALID)
};


/* Include the secondary tables. */
#include "DisasmTables-armv8-a64-simd-fp.cpp.h"
#include "DisasmTables-armv8-a64-ld-st.cpp.h"

/* UDF */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Rsvd)
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,    0, 16, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Rsvd)
    DIS_ARMV8_OP(0x00000000, "udf" ,            OP_ARMV8_A64_UDF,       DISOPTYPE_INVALID)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Rsvd, 0xffff0000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0xffff0000, 16);

/* ADR/ADRP */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Adr)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,  0, 5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmAdr, 0, 0, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Adr)
    DIS_ARMV8_OP(0x10000000, "adr" ,            OP_ARMV8_A64_ADR,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x90000000, "adrp" ,           OP_ARMV8_A64_ADRP,      DISOPTYPE_HARMLESS)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Adr, 0x9f000000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(31), 31);


/* ADD/ADDS/SUB/SUBS - shifted immediate variant */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(AddSubImm)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,    31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,  0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,  5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,   10, 12, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseSh12,  22,  1, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(AddSubImmS)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,    31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,  0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,  5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,   10, 12, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseSh12,  22,  1, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(AddSubImm)
    DIS_ARMV8_OP(           0x11000000, "add" ,            OP_ARMV8_A64_ADD,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP_ALT_DECODE(0x31000000, "adds" ,           OP_ARMV8_A64_ADDS,      DISOPTYPE_HARMLESS, AddSubImmS),
    DIS_ARMV8_OP(           0x51000000, "sub" ,            OP_ARMV8_A64_SUB,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP_ALT_DECODE(0x71000000, "subs" ,           OP_ARMV8_A64_SUBS,      DISOPTYPE_HARMLESS, AddSubImmS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(AddSubImm, 0x7f800000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(29) | RT_BIT_32(30), 29);


/* ADD/ADDS/SUB/SUBS - shifted register variant */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(AddSubShiftReg)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseShift,         22,  2, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseShiftAmount,   10,  6, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(AddSubShiftReg)
    DIS_ARMV8_OP(0x0b000000, "add" ,            OP_ARMV8_A64_ADD,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x2b000000, "adds" ,           OP_ARMV8_A64_ADDS,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x4b000000, "sub" ,            OP_ARMV8_A64_SUB,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x6b000000, "subs" ,           OP_ARMV8_A64_SUBS,      DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(AddSubShiftReg, 0x7f200000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(29) | RT_BIT_32(30), 29);


/* AND/ORR/EOR/ANDS */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(LogicalImm)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmsImmrN,     10, 13, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(LogicalImmZ)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmsImmrN,     10, 13, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(LogicalImm)
    DIS_ARMV8_OP(           0x12000000, "and" ,            OP_ARMV8_A64_AND,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x32000000, "orr" ,            OP_ARMV8_A64_ORR,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x52000000, "eor" ,            OP_ARMV8_A64_EOR,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP_ALT_DECODE(0x72000000, "ands" ,           OP_ARMV8_A64_ANDS,      DISOPTYPE_HARMLESS, LogicalImmZ),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(LogicalImm, 0x7f800000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(29) | RT_BIT_32(30), 29);


/* MOVN/MOVZ/MOVK */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(MoveWide)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,            5, 16, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseHw,            21,  2, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(MoveWide)
    DIS_ARMV8_OP(0x12800000, "movn",            OP_ARMV8_A64_MOVN,      DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
    DIS_ARMV8_OP(0x52800000, "movz" ,           OP_ARMV8_A64_MOVZ,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x72800000, "movk" ,           OP_ARMV8_A64_MOVK,      DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(MoveWide, 0x7f800000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(29) | RT_BIT_32(30), 29);


/* SBFM/BFM/UBFM */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Bitfield)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,           16,  6, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,           10,  6, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Bitfield)
    DIS_ARMV8_OP(0x13000000, "sbfm",            OP_ARMV8_A64_SBFM,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x33000000, "bfm",             OP_ARMV8_A64_BFM,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x53000000, "ubfm",            OP_ARMV8_A64_UBFM,      DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Bitfield, 0x7f800000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(29) | RT_BIT_32(30), 29);


/* EXTR */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Extract) /** @todo N must match SF, and for sf == 0 -> imms<5> == 0. */
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,           10,  6, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Extract)
    DIS_ARMV8_OP(0x13800000, "extr",            OP_ARMV8_A64_EXTR,      DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Extract, 0x7fa00000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0, 0);


/* ADD/ADDS/SUB/SUBS - shifted immediate variant */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(AddSubImmTags)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,   0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,   5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmX16, 16,  6, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,    10,  4, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(AddSubImmTags)
    DIS_ARMV8_OP(0x91800000, "addg",            OP_ARMV8_A64_ADDG,      DISOPTYPE_HARMLESS), /* FEAT_MTE */
    DIS_ARMV8_OP(0xd1800000, "subg" ,           OP_ARMV8_A64_SUBG,      DISOPTYPE_HARMLESS), /* FEAT_MTE */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(AddSubImmTags, 0xffc0c000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(30), 30);


/*
 * C4.1.65 of the ARMv8 architecture reference manual has the following table for the
 * data processing (immediate) instruction classes:
 *
 *     Bit  25 24 23
 *     +-------------------------------------------
 *           0  0  x PC-rel. addressing.
 *           0  1  0 Add/subtract (immediate)
 *           0  1  1 Add/subtract (immediate, with tags)
 *           1  0  0 Logical (immediate)
 *           1  0  1 Move wide (immediate)
 *           1  1  0 Bitfield
 *           1  1  1 Extract
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(DataProcessingImm)
    DIS_ARMV8_DECODE_MAP_ENTRY(Adr),
    DIS_ARMV8_DECODE_MAP_ENTRY(Adr),
    DIS_ARMV8_DECODE_MAP_ENTRY(AddSubImm),
    DIS_ARMV8_DECODE_MAP_ENTRY(AddSubImmTags),
    DIS_ARMV8_DECODE_MAP_ENTRY(LogicalImm),
    DIS_ARMV8_DECODE_MAP_ENTRY(MoveWide),
    DIS_ARMV8_DECODE_MAP_ENTRY(Bitfield),
    DIS_ARMV8_DECODE_MAP_ENTRY(Extract)
DIS_ARMV8_DECODE_MAP_DEFINE_END(DataProcessingImm, RT_BIT_32(23) | RT_BIT_32(24) | RT_BIT_32(25),  23);


/* B.cond/BC.cond */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(CondBr)
    DIS_ARMV8_INSN_DECODE(kDisParmParseCond,           0,  4, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmRel,         5, 19, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(CondBr)
    DIS_ARMV8_OP(0x54000000, "b",               OP_ARMV8_A64_B,         DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_RELATIVE_CONTROLFLOW | DISOPTYPE_COND_CONTROLFLOW),
    DIS_ARMV8_OP(0x54000010, "bc" ,             OP_ARMV8_A64_BC,        DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_RELATIVE_CONTROLFLOW | DISOPTYPE_COND_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(CondBr, 0xff000010 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(4), 4);


/* SVC/HVC/SMC/BRK/HLT/TCANCEL/DCPS1/DCPS2/DCPS3 */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Excp)
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,            5, 16, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Excp)
    DIS_ARMV8_OP(0xd4000001, "svc",             OP_ARMV8_A64_SVC,       DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT),
    DIS_ARMV8_OP(0xd4000002, "hvc",             OP_ARMV8_A64_HVC,       DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT | DISOPTYPE_PRIVILEGED),
    DIS_ARMV8_OP(0xd4000003, "smc",             OP_ARMV8_A64_SMC,       DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT | DISOPTYPE_PRIVILEGED),
    DIS_ARMV8_OP(0xd4200000, "brk",             OP_ARMV8_A64_BRK,       DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT),
    DIS_ARMV8_OP(0xd4400000, "hlt",             OP_ARMV8_A64_HLT,       DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT),
    DIS_ARMV8_OP(0xd4600000, "tcancel",         OP_ARMV8_A64_TCANCEL,   DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT), /* FEAT_TME */
    DIS_ARMV8_OP(0xd4a00001, "dcps1",           OP_ARMV8_A64_DCPS1,     DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT),
    DIS_ARMV8_OP(0xd4a00002, "dcps2",           OP_ARMV8_A64_DCPS2,     DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT),
    DIS_ARMV8_OP(0xd4a00003, "dcps3",           OP_ARMV8_A64_DCPS3,     DISOPTYPE_CONTROLFLOW | DISOPTYPE_INTERRUPT),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Excp, 0xffe0001f /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeLookup, 0xffe0001f, 0);


/* WFET/WFIT */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(SysReg)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(SysReg)
    DIS_ARMV8_OP(0xd5031000, "wfet",            OP_ARMV8_A64_WFET,      DISOPTYPE_HARMLESS), /* FEAT_WFxT */
    DIS_ARMV8_OP(0x54000010, "wfit" ,           OP_ARMV8_A64_WFIT,      DISOPTYPE_HARMLESS), /* FEAT_WFxT */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(SysReg, 0xffffffe0 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0xfe0, 5);


/* Various hint instructions */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Hints)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Hints)
    DIS_ARMV8_OP(0xd503201f, "nop",             OP_ARMV8_A64_NOP,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0xd503203f, "yield",           OP_ARMV8_A64_YIELD,     DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0xd503205f, "wfe",             OP_ARMV8_A64_WFE,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0xd503207f, "wfi",             OP_ARMV8_A64_WFI,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0xd503209f, "sev",             OP_ARMV8_A64_SEV,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0xd50320bf, "sevl",            OP_ARMV8_A64_SEVL,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0xd50320df, "dgh",             OP_ARMV8_A64_DGH,       DISOPTYPE_HARMLESS), /* FEAT_DGH */
    DIS_ARMV8_OP(0xd50320ff, "xpaclri",         OP_ARMV8_A64_XPACLRI,   DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd503211f, "pacia1716",       OP_ARMV8_A64_PACIA1716, DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503215f, "pacib1716",       OP_ARMV8_A64_PACIB1716, DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503219f, "autia1716",       OP_ARMV8_A64_AUTIA1716, DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd50321df, "autib1716",       OP_ARMV8_A64_AUTIB1716, DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503221f, "esb",             OP_ARMV8_A64_ESB,       DISOPTYPE_HARMLESS), /* FEAT_RAS */
    DIS_ARMV8_OP(0xd503223f, "psb csync",       OP_ARMV8_A64_PSB,       DISOPTYPE_HARMLESS), /* FEAT_SPE */
    DIS_ARMV8_OP(0xd503225f, "tsb csync",       OP_ARMV8_A64_TSB,       DISOPTYPE_HARMLESS), /* FEAT_TRF */
    DIS_ARMV8_OP(0xd503227f, "gcsb dsync",      OP_ARMV8_A64_GCSB,      DISOPTYPE_HARMLESS), /* FEAT_GCS */
    DIS_ARMV8_OP(0xd503229f, "csdb",            OP_ARMV8_A64_CSDB,      DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd50322df, "clrbhb",          OP_ARMV8_A64_CLRBHB,    DISOPTYPE_HARMLESS), /* FEAT_CLRBHB */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503231f, "paciaz",          OP_ARMV8_A64_PACIAZ,    DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd503233f, "paciasp",         OP_ARMV8_A64_PACIASP,   DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd503235f, "pacibz",          OP_ARMV8_A64_PACIBZ,    DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd503237f, "pacibsp",         OP_ARMV8_A64_PACIBSP,   DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd503239f, "autiaz",          OP_ARMV8_A64_AUTIAZ,    DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd50323bf, "autiasp",         OP_ARMV8_A64_AUTIASP,   DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd50323df, "autibz",          OP_ARMV8_A64_AUTIBZ,    DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd50323ff, "autibsp",         OP_ARMV8_A64_AUTIBSP,   DISOPTYPE_HARMLESS), /* FEAT_PAuth */
    DIS_ARMV8_OP(0xd503241f, "bti",             OP_ARMV8_A64_BTI,       DISOPTYPE_HARMLESS), /* FEAT_BTI   */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503245f, "bti c",           OP_ARMV8_A64_BTI_C,     DISOPTYPE_HARMLESS), /* FEAT_BTI   */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503249f, "bti j",           OP_ARMV8_A64_BTI_J,     DISOPTYPE_HARMLESS), /* FEAT_BTI   */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd50324df, "bti jc",          OP_ARMV8_A64_BTI_JC,    DISOPTYPE_HARMLESS), /* FEAT_BTI   */
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503251f, "chkfeat x16",     OP_ARMV8_A64_CHKFEAT,   DISOPTYPE_HARMLESS), /* FEAT_CHK   */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Hints, 0xffffffff /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0xfe0, 5);


/* CLREX */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(DecBarriers)
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,            8,  4, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(DecBarriers)
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503304f, "clrex",           OP_ARMV8_A64_CLREX,     DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd503309f, "dsb",             OP_ARMV8_A64_DSB,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0xd50330bf, "dmb",             OP_ARMV8_A64_DMB,       DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(DecBarriers, 0xfffff0ff /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(5) | RT_BIT_32(6) | RT_BIT_32(7), 5);


/* ISB */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Isb)
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,            8,  4, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Isb)
    DIS_ARMV8_OP(0xd50330df, "isb",             OP_ARMV8_A64_ISB,      DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Isb, 0xfffff0ff /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0, 0);


/* SB */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Sb)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Sb)
    DIS_ARMV8_OP(0xd50330ff, "sb",              OP_ARMV8_A64_SB,       DISOPTYPE_HARMLESS), /* FEAT_SB */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Sb, 0xffffffff /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0, 0);


/* TCOMMIT */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(TCommit)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(TCommit)
    DIS_ARMV8_OP(0xd503307f, "tcommit",         OP_ARMV8_A64_TCOMMIT,  DISOPTYPE_HARMLESS), /* FEAT_TME */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(TCommit, 0xffffffff /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0, 0);


/* Barrier instructions, we divide these instructions further based on the op2 field. */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(DecodeBarriers)
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,                     /** @todo DSB - Encoding (FEAT_XS) */
    DIS_ARMV8_DECODE_MAP_ENTRY(DecBarriers),                /* CLREX */
    DIS_ARMV8_DECODE_MAP_ENTRY(TCommit),
    DIS_ARMV8_DECODE_MAP_ENTRY(DecBarriers),                /* DSB - Encoding */
    DIS_ARMV8_DECODE_MAP_ENTRY(DecBarriers),                /* DMB */
    DIS_ARMV8_DECODE_MAP_ENTRY(Isb),
    DIS_ARMV8_DECODE_MAP_ENTRY(Sb),
DIS_ARMV8_DECODE_MAP_DEFINE_END(DecodeBarriers, RT_BIT_32(5) | RT_BIT_32(6) | RT_BIT_32(7), 5);


/* MSR (and potentially CFINV,XAFLAG,AXFLAG) */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(PState)
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,            8,  4, 1 /*idxParam*/), /* CRm field encodes the immediate value, gets validated by the next decoder stage. */
    DIS_ARMV8_INSN_DECODE(kDisParmParsePState,         0,  0, 0 /*idxParam*/), /* This is special for the MSR instruction. */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(PState)
    DIS_ARMV8_OP(0xd500401f, "msr",             OP_ARMV8_A64_MSR,       DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(PState, 0xfff8f01f /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0, 0);


/* TSTART/TTEST */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(SysResult)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(SysResult)
    DIS_ARMV8_OP(0xd5233060, "tstart",          OP_ARMV8_A64_TSTART,    DISOPTYPE_HARMLESS | DISOPTYPE_PRIVILEGED),  /* FEAT_TME */
    DIS_ARMV8_OP(0xd5233160, "ttest",           OP_ARMV8_A64_TTEST,     DISOPTYPE_HARMLESS),                         /* FEAT_TME */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(SysResult, 0xfffffffe /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(8) | RT_BIT_32(9) | RT_BIT_32(10) | RT_BIT_32(11), 8);


/* SYS */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Sys)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSysInsExtraStr, 5, 18, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(SysFallback)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSysIns,         5, 18, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Sys) /** @todo DISOPTYPE_PRIVILEGED */
    DIS_ARMV8_OP(0xd5087100,            "ic\0ialluis",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 1 op2=0 */
    DIS_ARMV8_OP(0xd5087500,            "ic\0iallu",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 5 op2=0 */
    DIS_ARMV8_OP(0xd5087620,            "dc\0ivac",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd5087640,            "dc\0isw",              OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 6 op2=2 */
    DIS_ARMV8_OP(0xd5087660,            "dc\0igvac",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 6 op2=3 */
    DIS_ARMV8_OP(0xd5087680,            "dc\0igsw",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 6 op2=4 */
    DIS_ARMV8_OP(0xd50876a0,            "dc\0igdvac",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd50876c0,            "dc\0igdsw",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 6 op2=6 */
    DIS_ARMV8_OP(0xd5087780,            "gcspushx\0",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 7 op2=4 */
    DIS_ARMV8_OP(0xd50877a0,            "gcspopcx\0",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd50877c0,            "gcspopx\0",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 7 op2=6 */
    DIS_ARMV8_OP(0xd5087800,            "at\0s1e1r",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 8 op2=0 */
    DIS_ARMV8_OP(0xd5087820,            "at\0s1e1w",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 8 op2=1 */
    DIS_ARMV8_OP(0xd5087840,            "at\0s1e0r",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 8 op2=2 */
    DIS_ARMV8_OP(0xd5087860,            "at\0s1e0w",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 8 op2=3 */
    DIS_ARMV8_OP(0xd5087900,            "at\0s1e1rp",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 9 op2=0 */
    DIS_ARMV8_OP(0xd5087920,            "at\0s1e1wp",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 9 op2=1 */
    DIS_ARMV8_OP(0xd5087940,            "at\0s1e1a",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm= 9 op2=2 */
    DIS_ARMV8_OP(0xd5087a40,            "dc\0csw",              OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm=10 op2=2 */
    DIS_ARMV8_OP(0xd5087a80,            "dc\0cgsw",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm=10 op2=4 */
    DIS_ARMV8_OP(0xd5087ac0,            "dc\0cgdsw",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm=10 op2=6 */
    DIS_ARMV8_OP(0xd5087e40,            "dc\0cisw",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm=14 op2=2 */
    DIS_ARMV8_OP(0xd5087e80,            "dc\0cigsw",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm=14 op2=4 */
    DIS_ARMV8_OP(0xd5087ec0,            "dc\0cigdsw",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm=14 op2=6 */
    DIS_ARMV8_OP(0xd5087f20,            "dc\0civaps",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm=15 op2=1 */
    DIS_ARMV8_OP(0xd5087fa0,            "dc\0cigdvaps",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 7 CRm=15 op2=5 */
    DIS_ARMV8_OP(0xd5088100,            "tlbi\0vmalle1os",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=0 */
    DIS_ARMV8_OP(0xd5088120,            "tlbi\0vae1os",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd5088140,            "tlbi\0aside1os",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=2 */
    DIS_ARMV8_OP(0xd5088160,            "tlbi\0vaae1os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=3 */
    DIS_ARMV8_OP(0xd50881a0,            "tlbi\0vale1os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd50881e0,            "tlbi\0vaale1os",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=7 */
    DIS_ARMV8_OP(0xd5088220,            "tlbi\0rvae1is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd5088260,            "tlbi\0rvaae1is",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 2 op2=3 */
    DIS_ARMV8_OP(0xd50882a0,            "tlbi\0rvale1is",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd50882e0,            "tlbi\0rvaale1is",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 2 op2=7 */
    DIS_ARMV8_OP(0xd5088300,            "tlbi\0vmalle1is",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=0 */
    DIS_ARMV8_OP(0xd5088320,            "tlbi\0vae1is",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd5088340,            "tlbi\0aside1is",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=2 */
    DIS_ARMV8_OP(0xd5088360,            "tlbi\0vaae1is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=3 */
    DIS_ARMV8_OP(0xd50883a0,            "tlbi\0vale1is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd50883e0,            "tlbi\0vaale1is",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=7 */
    DIS_ARMV8_OP(0xd5088520,            "tlbi\0rvae1os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd5088560,            "tlbi\0rvaae1os",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 5 op2=3 */
    DIS_ARMV8_OP(0xd50885a0,            "tlbi\0rvale1os",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd50885e0,            "tlbi\0rvaale1os",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 5 op2=7 */
    DIS_ARMV8_OP(0xd5088620,            "tlbi\0rvae1",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd5088660,            "tlbi\0rvaae1",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 6 op2=3 */
    DIS_ARMV8_OP(0xd50886a0,            "tlbi\0rvale1",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd50886e0,            "tlbi\0rvaale1",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 6 op2=7 */
    DIS_ARMV8_OP(0xd5088700,            "tlbi\0vmalle1",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=0 */
    DIS_ARMV8_OP(0xd5088720,            "tlbi\0vae1",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd5088740,            "tlbi\0aside1",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=2 */
    DIS_ARMV8_OP(0xd5088760,            "tlbi\0vaae1",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=3 */
    DIS_ARMV8_OP(0xd50887a0,            "tlbi\0vale1",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd50887e0,            "tlbi\0vaale1",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=7 */
    DIS_ARMV8_OP(0xd5089100,            "tlbi\0vmalle1osnxs",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=0 */
    DIS_ARMV8_OP(0xd5089120,            "tlbi\0vae1osnxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd5089140,            "tlbi\0aside1osnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=2 */
    DIS_ARMV8_OP(0xd5089160,            "tlbi\0vaae1osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=3 */
    DIS_ARMV8_OP(0xd50891a0,            "tlbi\0vale1osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd50891e0,            "tlbi\0vaale1osnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=7 */
    DIS_ARMV8_OP(0xd5089220,            "tlbi\0rvae1isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd5089260,            "tlbi\0rvaae1isnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 2 op2=3 */
    DIS_ARMV8_OP(0xd50892a0,            "tlbi\0rvale1isnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd50892e0,            "tlbi\0rvaale1isnxs",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 2 op2=7 */
    DIS_ARMV8_OP(0xd5089300,            "tlbi\0vmalle1isnxs",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=0 */
    DIS_ARMV8_OP(0xd5089320,            "tlbi\0vae1isnxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd5089340,            "tlbi\0aside1isnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=2 */
    DIS_ARMV8_OP(0xd5089360,            "tlbi\0vaae1isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=3 */
    DIS_ARMV8_OP(0xd50893a0,            "tlbi\0vale1isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd50893e0,            "tlbi\0vaale1isnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=7 */
    DIS_ARMV8_OP(0xd5089520,            "tlbi\0rvae1osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd5089560,            "tlbi\0rvaae1osnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 5 op2=3 */
    DIS_ARMV8_OP(0xd50895a0,            "tlbi\0rvale1osnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd50895e0,            "tlbi\0rvaale1osnxs",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 5 op2=7 */
    DIS_ARMV8_OP(0xd5089620,            "tlbi\0rvae1nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd5089660,            "tlbi\0rvaae1nxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 6 op2=3 */
    DIS_ARMV8_OP(0xd50896a0,            "tlbi\0rvale1nxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd50896e0,            "tlbi\0rvaale1nxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 6 op2=7 */
    DIS_ARMV8_OP(0xd5089700,            "tlbi\0vmalle1nxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=0 */
    DIS_ARMV8_OP(0xd5089720,            "tlbi\0vae1nxs",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd5089740,            "tlbi\0aside1nxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=2 */
    DIS_ARMV8_OP(0xd5089760,            "tlbi\0vaae1nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=3 */
    DIS_ARMV8_OP(0xd50897a0,            "tlbi\0vale1nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd50897e0,            "tlbi\0vaale1nxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=7 */
    DIS_ARMV8_OP(0xd5097280,            "brb\0iall",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=1 CRn= 7 CRm= 2 op2=4 */
    DIS_ARMV8_OP(0xd50972a0,            "brb\0inj",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=1 CRn= 7 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd50b72e0,            "trcit\0",              OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 2 op2=7 */
    DIS_ARMV8_OP(0xd50b7380,            "cfp\0rctx",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 3 op2=4 */
    DIS_ARMV8_OP(0xd50b73a0,            "dvp\0rctx",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd50b73c0,            "cosp\0rctx",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 3 op2=6 */
    DIS_ARMV8_OP(0xd50b73e0,            "cpp\0rctx",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 3 op2=7 */
    DIS_ARMV8_OP(0xd50b7420,            "dc\0zva",              OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 4 op2=1 */
    DIS_ARMV8_OP(0xd50b7460,            "dc\0gva",              OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 4 op2=3 */
    DIS_ARMV8_OP(0xd50b7480,            "dc\0gzva",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 4 op2=4 */
    DIS_ARMV8_OP(0xd50b7520,            "ic\0ivau",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd50b7700,            "gcspushm\0",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 7 op2=0 */
    DIS_ARMV8_OP(0xd50b7740,            "gcsss1\0",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 7 op2=2 */
    DIS_ARMV8_OP(0xd50b7a20,            "dc\0cvac",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=10 op2=1 */
    DIS_ARMV8_OP(0xd50b7a60,            "dc\0cgvac",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=10 op2=3 */
    DIS_ARMV8_OP(0xd50b7aa0,            "dc\0cgdvac",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=10 op2=5 */
    DIS_ARMV8_OP(0xd50b7b00,            "dc\0cvaoc",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=11 op2=0 */
    DIS_ARMV8_OP(0xd50b7b20,            "dc\0cvau",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=11 op2=1 */
    DIS_ARMV8_OP(0xd50b7be0,            "dc\0cgdvaoc",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=11 op2=7 */
    DIS_ARMV8_OP(0xd50b7c20,            "dc\0cvap",             OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=12 op2=1 */
    DIS_ARMV8_OP(0xd50b7c60,            "dc\0cgvap",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=12 op2=3 */
    DIS_ARMV8_OP(0xd50b7ca0,            "dc\0cgdvap",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=12 op2=5 */
    DIS_ARMV8_OP(0xd50b7d20,            "dc\0cvadp",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=13 op2=1 */
    DIS_ARMV8_OP(0xd50b7d60,            "dc\0cgvadp",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=13 op2=3 */
    DIS_ARMV8_OP(0xd50b7da0,            "dc\0cgdvadp",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=13 op2=5 */
    DIS_ARMV8_OP(0xd50b7e20,            "dc\0civac",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=14 op2=1 */
    DIS_ARMV8_OP(0xd50b7e60,            "dc\0cigvac",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=14 op2=3 */
    DIS_ARMV8_OP(0xd50b7ea0,            "dc\0cigdvac",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=14 op2=5 */
    DIS_ARMV8_OP(0xd50b7f00,            "dc\0civaoc",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=15 op2=0 */
    DIS_ARMV8_OP(0xd50b7fe0,            "dc\0cigdvaoc",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm=15 op2=7 */
    DIS_ARMV8_OP(0xd50c7800,            "at\0s1e2r",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm= 8 op2=0 */
    DIS_ARMV8_OP(0xd50c7820,            "at\0s1e2w",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm= 8 op2=1 */
    DIS_ARMV8_OP(0xd50c7880,            "at\0s12e1r",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm= 8 op2=4 */
    DIS_ARMV8_OP(0xd50c78a0,            "at\0s12e1w",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm= 8 op2=5 */
    DIS_ARMV8_OP(0xd50c78c0,            "at\0s12e0r",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm= 8 op2=6 */
    DIS_ARMV8_OP(0xd50c78e0,            "at\0s12e0w",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm= 8 op2=7 */
    DIS_ARMV8_OP(0xd50c7940,            "at\0s1e2a",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm= 9 op2=2 */
    DIS_ARMV8_OP(0xd50c7e00,            "dc\0cipae",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm=14 op2=0 */
    DIS_ARMV8_OP(0xd50c7ee0,            "dc\0cigdpae",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 7 CRm=14 op2=7 */
    DIS_ARMV8_OP(0xd50c8020,            "tlbi\0ipas2e1is",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 0 op2=1 */
    DIS_ARMV8_OP(0xd50c8040,            "tlbi\0ripas2e1is",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 0 op2=2 */
    DIS_ARMV8_OP(0xd50c80a0,            "tlbi\0ipas2le1is",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 0 op2=5 */
    DIS_ARMV8_OP(0xd50c80c0,            "tlbi\0ripas2le1is",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 0 op2=6 */
    DIS_ARMV8_OP(0xd50c8100,            "tlbi\0alle2os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 1 op2=0 */
    DIS_ARMV8_OP(0xd50c8120,            "tlbi\0vae2os",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd50c8180,            "tlbi\0alle1os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 1 op2=4 */
    DIS_ARMV8_OP(0xd50c81a0,            "tlbi\0vale2os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd50c81c0,            "tlbi\0vmalls12e1os",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 1 op2=6 */
    DIS_ARMV8_OP(0xd50c8220,            "tlbi\0rvae2is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd50c8240,            "tlbi\0vmallws2e1is",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 2 op2=2 */
    DIS_ARMV8_OP(0xd50c82a0,            "tlbi\0rvale2is",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd50c8300,            "tlbi\0alle2is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 3 op2=0 */
    DIS_ARMV8_OP(0xd50c8320,            "tlbi\0vae2is",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd50c8380,            "tlbi\0alle1is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 3 op2=4 */
    DIS_ARMV8_OP(0xd50c83a0,            "tlbi\0vale2is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd50c83c0,            "tlbi\0vmalls12e1is",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 3 op2=6 */
    DIS_ARMV8_OP(0xd50c8400,            "tlbi\0ipas2e1os",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=0 */
    DIS_ARMV8_OP(0xd50c8420,            "tlbi\0ipas2e1",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=1 */
    DIS_ARMV8_OP(0xd50c8440,            "tlbi\0ripas2e1",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=2 */
    DIS_ARMV8_OP(0xd50c8460,            "tlbi\0ripas2e1os",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=3 */
    DIS_ARMV8_OP(0xd50c8480,            "tlbi\0ipas2le1os",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=4 */
    DIS_ARMV8_OP(0xd50c84a0,            "tlbi\0ipas2le1",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=5 */
    DIS_ARMV8_OP(0xd50c84c0,            "tlbi\0ripas2le1",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=6 */
    DIS_ARMV8_OP(0xd50c84e0,            "tlbi\0ripas2le1os",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=7 */
    DIS_ARMV8_OP(0xd50c8520,            "tlbi\0rvae2os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd50c8540,            "tlbi\0vmallws2e1os",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 5 op2=2 */
    DIS_ARMV8_OP(0xd50c85a0,            "tlbi\0rvale2os",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd50c8620,            "tlbi\0rvae2",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd50c8640,            "tlbi\0vmallws2e1",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 6 op2=2 */
    DIS_ARMV8_OP(0xd50c86a0,            "tlbi\0rvale2",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd50c8700,            "tlbi\0alle2",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 7 op2=0 */
    DIS_ARMV8_OP(0xd50c8720,            "tlbi\0vae2",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd50c8780,            "tlbi\0alle1",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 7 op2=4 */
    DIS_ARMV8_OP(0xd50c87a0,            "tlbi\0vale2",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd50c87c0,            "tlbi\0vmalls12e1",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 7 op2=6 */
    DIS_ARMV8_OP(0xd50c9020,            "tlbi\0ipas2e1isnxs",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 0 op2=1 */
    DIS_ARMV8_OP(0xd50c9040,            "tlbi\0ripas2e1isnxs",  OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 0 op2=2 */
    DIS_ARMV8_OP(0xd50c90a0,            "tlbi\0ipas2le1isnxs",  OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 0 op2=5 */
    DIS_ARMV8_OP(0xd50c90c0,            "tlbi\0ripas2le1isnxs", OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 0 op2=6 */
    DIS_ARMV8_OP(0xd50c9100,            "tlbi\0alle2osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 1 op2=0 */
    DIS_ARMV8_OP(0xd50c9120,            "tlbi\0vae2osnxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd50c9180,            "tlbi\0alle1osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 1 op2=4 */
    DIS_ARMV8_OP(0xd50c91a0,            "tlbi\0vale2osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd50c91c0,            "tlbi\0vmalls12e1osnxs",OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 1 op2=6 */
    DIS_ARMV8_OP(0xd50c9220,            "tlbi\0rvae2isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd50c9240,            "tlbi\0vmallws2e1isnxs",OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 2 op2=2 */
    DIS_ARMV8_OP(0xd50c92a0,            "tlbi\0rvale2isnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd50c9300,            "tlbi\0alle2isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 3 op2=0 */
    DIS_ARMV8_OP(0xd50c9320,            "tlbi\0vae2isnxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd50c9380,            "tlbi\0alle1isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 3 op2=4 */
    DIS_ARMV8_OP(0xd50c93a0,            "tlbi\0vale2isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd50c93c0,            "tlbi\0vmalls12e1isnxs",OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 3 op2=6 */
    DIS_ARMV8_OP(0xd50c9400,            "tlbi\0ipas2e1osnxs",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=0 */
    DIS_ARMV8_OP(0xd50c9420,            "tlbi\0ipas2e1nxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=1 */
    DIS_ARMV8_OP(0xd50c9440,            "tlbi\0ripas2e1nxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=2 */
    DIS_ARMV8_OP(0xd50c9460,            "tlbi\0ripas2e1osnxs",  OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=3 */
    DIS_ARMV8_OP(0xd50c9480,            "tlbi\0ipas2le1osnxs",  OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=4 */
    DIS_ARMV8_OP(0xd50c94a0,            "tlbi\0ipas2le1nxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=5 */
    DIS_ARMV8_OP(0xd50c94c0,            "tlbi\0ripas2le1nxs",   OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=6 */
    DIS_ARMV8_OP(0xd50c94e0,            "tlbi\0ripas2le1osnxs", OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=7 */
    DIS_ARMV8_OP(0xd50c9520,            "tlbi\0rvae2osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd50c9540,            "tlbi\0vmallws2e1osnxs",OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 5 op2=2 */
    DIS_ARMV8_OP(0xd50c95a0,            "tlbi\0rvale2osnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd50c9620,            "tlbi\0rvae2nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd50c9640,            "tlbi\0vmallws2e1nxs",  OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 6 op2=2 */
    DIS_ARMV8_OP(0xd50c96a0,            "tlbi\0rvale2nxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd50c9700,            "tlbi\0alle2nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 7 op2=0 */
    DIS_ARMV8_OP(0xd50c9720,            "tlbi\0vae2nxs",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd50c9780,            "tlbi\0alle1nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 7 op2=4 */
    DIS_ARMV8_OP(0xd50c97a0,            "tlbi\0vale2nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd50c97c0,            "tlbi\0vmalls12e1nxs",  OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 7 op2=6 */
    DIS_ARMV8_OP(0xd50e7000,            "apas\0",               OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 7 CRm= 0 op2=0 */
    DIS_ARMV8_OP(0xd50e7800,            "at\0s1e3r",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 7 CRm= 8 op2=0 */
    DIS_ARMV8_OP(0xd50e7820,            "at\0s1e3w",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 7 CRm= 8 op2=1 */
    DIS_ARMV8_OP(0xd50e7940,            "at\0s1e3a",            OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 7 CRm= 9 op2=2 */
    DIS_ARMV8_OP(0xd50e7e20,            "dc\0cipapa",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 7 CRm=14 op2=1 */
    DIS_ARMV8_OP(0xd50e7ea0,            "dc\0cigdpapa",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 7 CRm=14 op2=5 */
    DIS_ARMV8_OP(0xd50e8100,            "tlbi\0alle3os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 1 op2=0 */
    DIS_ARMV8_OP(0xd50e8120,            "tlbi\0vae3os",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd50e8180,            "tlbi\0paallos",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 1 op2=4 */
    DIS_ARMV8_OP(0xd50e81a0,            "tlbi\0vale3os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd50e8220,            "tlbi\0rvae3is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd50e82a0,            "tlbi\0rvale3is",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd50e8300,            "tlbi\0alle3is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 3 op2=0 */
    DIS_ARMV8_OP(0xd50e8320,            "tlbi\0vae3is",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd50e83a0,            "tlbi\0vale3is",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd50e8460,            "tlbi\0rpaos",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 4 op2=3 */
    DIS_ARMV8_OP(0xd50e84e0,            "tlbi\0rpalos",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 4 op2=7 */
    DIS_ARMV8_OP(0xd50e8520,            "tlbi\0rvae3os",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd50e85a0,            "tlbi\0rvale3os",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd50e8620,            "tlbi\0rvae3",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd50e86a0,            "tlbi\0rvale3",         OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd50e8700,            "tlbi\0alle3",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 7 op2=0 */
    DIS_ARMV8_OP(0xd50e8720,            "tlbi\0vae3",           OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd50e8780,            "tlbi\0paall",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 7 op2=4 */
    DIS_ARMV8_OP(0xd50e87a0,            "tlbi\0vale3",          OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd50e9100,            "tlbi\0alle3osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 1 op2=0 */
    DIS_ARMV8_OP(0xd50e9120,            "tlbi\0vae3osnxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd50e91a0,            "tlbi\0vale3osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd50e9220,            "tlbi\0rvae3isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd50e92a0,            "tlbi\0rvale3isnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd50e9300,            "tlbi\0alle3isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 3 op2=0 */
    DIS_ARMV8_OP(0xd50e9320,            "tlbi\0vae3isnxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd50e93a0,            "tlbi\0vale3isnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd50e9520,            "tlbi\0rvae3osnxs",     OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd50e95a0,            "tlbi\0rvale3osnxs",    OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd50e9620,            "tlbi\0rvae3nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd50e96a0,            "tlbi\0rvale3nxs",      OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd50e9700,            "tlbi\0alle3nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 7 op2=0 */
    DIS_ARMV8_OP(0xd50e9720,            "tlbi\0vae3nxs",        OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd50e97a0,            "tlbi\0vale3nxs",       OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 7 op2=5 */
    DIS_ARMV8_OP_ALT_DECODE(0xd5080000, "sys",                  OP_ARMV8_A64_SYS, DISOPTYPE_HARMLESS, SysFallback),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Sys, 0xfff80000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeBinaryLookupWithDefault, 0xffffffe0 /*fMask*/, 0);

/* SYSL */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(SysL)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseSysInsExtraStr, 5, 18, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(SysLDefault)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseSysIns,         5, 18, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(SysL)
    DIS_ARMV8_OP(0xd52b7720,            "gcspopm\0",    OP_ARMV8_A64_SYSL, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd52b7760,            "gcsss2\0",     OP_ARMV8_A64_SYSL, DISOPTYPE_HARMLESS), /* op1=3 CRn= 7 CRm= 7 op2=3 */
    DIS_ARMV8_OP_ALT_DECODE(0xd5280000, "sysl",         OP_ARMV8_A64_SYSL, DISOPTYPE_HARMLESS, SysLDefault),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(SysL, 0xfff80000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeBinaryLookupWithDefault, 0xffffffe0 /*fMask*/, 0);

/* SYSP */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(SysP)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSysInsExtraStr, 5, 18, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64PlusOne, 0,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(SysPDefault)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSysIns,         5, 18, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64PlusOne, 0,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(SysP)
    DIS_ARMV8_OP(0xd5488120,            "tlbip\0vae1os",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd5488160,            "tlbip\0vaae1os",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=3 */
    DIS_ARMV8_OP(0xd54881a0,            "tlbip\0vale1os",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd54881e0,            "tlbip\0vaale1os",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 1 op2=7 */
    DIS_ARMV8_OP(0xd5488220,            "tlbip\0rvae1is",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd5488260,            "tlbip\0rvaae1is",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 2 op2=3 */
    DIS_ARMV8_OP(0xd54882a0,            "tlbip\0rvale1is",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd54882e0,            "tlbip\0rvaale1is",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 2 op2=7 */
    DIS_ARMV8_OP(0xd5488320,            "tlbip\0vae1is",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd5488360,            "tlbip\0vaae1is",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=3 */
    DIS_ARMV8_OP(0xd54883a0,            "tlbip\0vale1is",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd54883e0,            "tlbip\0vaale1is",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 3 op2=7 */
    DIS_ARMV8_OP(0xd5488520,            "tlbip\0rvae1os",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd5488560,            "tlbip\0rvaae1os",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 5 op2=3 */
    DIS_ARMV8_OP(0xd54885a0,            "tlbip\0rvale1os",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd54885e0,            "tlbip\0rvaale1os",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 5 op2=7 */
    DIS_ARMV8_OP(0xd5488620,            "tlbip\0rvae1",         OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd5488660,            "tlbip\0rvaae1",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 6 op2=3 */
    DIS_ARMV8_OP(0xd54886a0,            "tlbip\0rvale1",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd54886e0,            "tlbip\0rvaale1",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 6 op2=7 */
    DIS_ARMV8_OP(0xd5488720,            "tlbip\0vae1",          OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd5488760,            "tlbip\0vaae1",         OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=3 */
    DIS_ARMV8_OP(0xd54887a0,            "tlbip\0vale1",         OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd54887e0,            "tlbip\0vaale1",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 8 CRm= 7 op2=7 */
    DIS_ARMV8_OP(0xd5489120,            "tlbip\0vae1osnxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd5489160,            "tlbip\0vaae1osnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=3 */
    DIS_ARMV8_OP(0xd54891a0,            "tlbip\0vale1osnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd54891e0,            "tlbip\0vaale1osnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 1 op2=7 */
    DIS_ARMV8_OP(0xd5489220,            "tlbip\0rvae1isnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd5489260,            "tlbip\0rvaae1isnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 2 op2=3 */
    DIS_ARMV8_OP(0xd54892a0,            "tlbip\0rvale1isnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd54892e0,            "tlbip\0rvaale1isnxs",  OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 2 op2=7 */
    DIS_ARMV8_OP(0xd5489320,            "tlbip\0vae1isnxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd5489360,            "tlbip\0vaae1isnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=3 */
    DIS_ARMV8_OP(0xd54893a0,            "tlbip\0vale1isnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd54893e0,            "tlbip\0vaale1isnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 3 op2=7 */
    DIS_ARMV8_OP(0xd5489520,            "tlbip\0rvae1osnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd5489560,            "tlbip\0rvaae1osnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 5 op2=3 */
    DIS_ARMV8_OP(0xd54895a0,            "tlbip\0rvale1osnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd54895e0,            "tlbip\0rvaale1osnxs",  OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 5 op2=7 */
    DIS_ARMV8_OP(0xd5489620,            "tlbip\0rvae1nxs",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd5489660,            "tlbip\0rvaae1nxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 6 op2=3 */
    DIS_ARMV8_OP(0xd54896a0,            "tlbip\0rvale1nxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd54896e0,            "tlbip\0rvaale1nxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 6 op2=7 */
    DIS_ARMV8_OP(0xd5489720,            "tlbip\0vae1nxs",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd5489760,            "tlbip\0vaae1nxs",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=3 */
    DIS_ARMV8_OP(0xd54897a0,            "tlbip\0vale1nxs",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd54897e0,            "tlbip\0vaale1nxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=0 CRn= 9 CRm= 7 op2=7 */
    DIS_ARMV8_OP(0xd54c8020,            "tlbip\0ipas2e1is",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 0 op2=1 */
    DIS_ARMV8_OP(0xd54c8040,            "tlbip\0ripas2e1is",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 0 op2=2 */
    DIS_ARMV8_OP(0xd54c80a0,            "tlbip\0ipas2le1is",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 0 op2=5 */
    DIS_ARMV8_OP(0xd54c80c0,            "tlbip\0ripas2le1is",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 0 op2=6 */
    DIS_ARMV8_OP(0xd54c8120,            "tlbip\0vae2os",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd54c81a0,            "tlbip\0vale2os",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd54c8220,            "tlbip\0rvae2is",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd54c82a0,            "tlbip\0rvale2is",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd54c8320,            "tlbip\0vae2is",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd54c83a0,            "tlbip\0vale2is",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd54c8400,            "tlbip\0ipas2e1os",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=0 */
    DIS_ARMV8_OP(0xd54c8420,            "tlbip\0ipas2e1",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=1 */
    DIS_ARMV8_OP(0xd54c8440,            "tlbip\0ripas2e1",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=2 */
    DIS_ARMV8_OP(0xd54c8460,            "tlbip\0ripas2e1os",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=3 */
    DIS_ARMV8_OP(0xd54c8480,            "tlbip\0ipas2le1os",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=4 */
    DIS_ARMV8_OP(0xd54c84a0,            "tlbip\0ipas2le1",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=5 */
    DIS_ARMV8_OP(0xd54c84c0,            "tlbip\0ripas2le1",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=6 */
    DIS_ARMV8_OP(0xd54c84e0,            "tlbip\0ripas2le1os",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 4 op2=7 */
    DIS_ARMV8_OP(0xd54c8520,            "tlbip\0rvae2os",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd54c85a0,            "tlbip\0rvale2os",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd54c8620,            "tlbip\0rvae2",         OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd54c86a0,            "tlbip\0rvale2",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd54c8720,            "tlbip\0vae2",          OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd54c87a0,            "tlbip\0vale2",         OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 8 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd54c9020,            "tlbip\0ipas2e1isnxs",  OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 0 op2=1 */
    DIS_ARMV8_OP(0xd54c9040,            "tlbip\0ripas2e1isnxs", OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 0 op2=2 */
    DIS_ARMV8_OP(0xd54c90a0,            "tlbip\0ipas2le1isnxs", OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 0 op2=5 */
    DIS_ARMV8_OP(0xd54c90c0,            "tlbip\0ripas2le1isnxs",OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 0 op2=6 */
    DIS_ARMV8_OP(0xd54c9120,            "tlbip\0vae2osnxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd54c91a0,            "tlbip\0vale2osnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd54c9220,            "tlbip\0rvae2isnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd54c92a0,            "tlbip\0rvale2isnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd54c9320,            "tlbip\0vae2isnxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd54c93a0,            "tlbip\0vale2isnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd54c9400,            "tlbip\0ipas2e1osnxs",  OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=0 */
    DIS_ARMV8_OP(0xd54c9420,            "tlbip\0ipas2e1nxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=1 */
    DIS_ARMV8_OP(0xd54c9440,            "tlbip\0ripas2e1nxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=2 */
    DIS_ARMV8_OP(0xd54c9460,            "tlbip\0ripas2e1osnxs", OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=3 */
    DIS_ARMV8_OP(0xd54c9480,            "tlbip\0ipas2le1osnxs", OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=4 */
    DIS_ARMV8_OP(0xd54c94a0,            "tlbip\0ipas2le1nxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=5 */
    DIS_ARMV8_OP(0xd54c94c0,            "tlbip\0ripas2le1nxs",  OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=6 */
    DIS_ARMV8_OP(0xd54c94e0,            "tlbip\0ripas2le1osnxs",OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 4 op2=7 */
    DIS_ARMV8_OP(0xd54c9520,            "tlbip\0rvae2osnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd54c95a0,            "tlbip\0rvale2osnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd54c9620,            "tlbip\0rvae2nxs",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd54c96a0,            "tlbip\0rvale2nxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd54c9720,            "tlbip\0vae2nxs",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd54c97a0,            "tlbip\0vale2nxs",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=4 CRn= 9 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd54e8120,            "tlbip\0vae3os",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd54e81a0,            "tlbip\0vale3os",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd54e8220,            "tlbip\0rvae3is",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd54e82a0,            "tlbip\0rvale3is",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd54e8320,            "tlbip\0vae3is",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd54e83a0,            "tlbip\0vale3is",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd54e8520,            "tlbip\0rvae3os",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd54e85a0,            "tlbip\0rvale3os",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd54e8620,            "tlbip\0rvae3",         OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd54e86a0,            "tlbip\0rvale3",        OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd54e8720,            "tlbip\0vae3",          OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd54e87a0,            "tlbip\0vale3",         OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 8 CRm= 7 op2=5 */
    DIS_ARMV8_OP(0xd54e9120,            "tlbip\0vae3osnxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 1 op2=1 */
    DIS_ARMV8_OP(0xd54e91a0,            "tlbip\0vale3osnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 1 op2=5 */
    DIS_ARMV8_OP(0xd54e9220,            "tlbip\0rvae3isnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 2 op2=1 */
    DIS_ARMV8_OP(0xd54e92a0,            "tlbip\0rvale3isnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 2 op2=5 */
    DIS_ARMV8_OP(0xd54e9320,            "tlbip\0vae3isnxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 3 op2=1 */
    DIS_ARMV8_OP(0xd54e93a0,            "tlbip\0vale3isnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 3 op2=5 */
    DIS_ARMV8_OP(0xd54e9520,            "tlbip\0rvae3osnxs",    OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 5 op2=1 */
    DIS_ARMV8_OP(0xd54e95a0,            "tlbip\0rvale3osnxs",   OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 5 op2=5 */
    DIS_ARMV8_OP(0xd54e9620,            "tlbip\0rvae3nxs",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 6 op2=1 */
    DIS_ARMV8_OP(0xd54e96a0,            "tlbip\0rvale3nxs",     OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 6 op2=5 */
    DIS_ARMV8_OP(0xd54e9720,            "tlbip\0vae3nxs",       OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 7 op2=1 */
    DIS_ARMV8_OP(0xd54e97a0,            "tlbip\0vale3nxs",      OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS), /* op1=6 CRn= 9 CRm= 7 op2=5 */
    DIS_ARMV8_OP_ALT_DECODE(0xd5480000, "sysp",                 OP_ARMV8_A64_SYSP, DISOPTYPE_HARMLESS, SysPDefault),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(SysP, 0xfff80000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeBinaryLookupWithDefault, 0xffffffe0 /*fMask*/, 0);


/* MSR */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Msr)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSysReg,         5, 15, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Msr)
    DIS_ARMV8_OP(0xd5100000, "msr",             OP_ARMV8_A64_MSR,       DISOPTYPE_PRIVILEGED | DISOPTYPE_PRIVILEGED),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Msr, 0xfff00000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0, 0);


/* MRS */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Mrs)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseSysReg,         5, 15, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Mrs)
    DIS_ARMV8_OP(0xd5300000, "mrs",             OP_ARMV8_A64_MRS,       DISOPTYPE_PRIVILEGED | DISOPTYPE_PRIVILEGED),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Mrs, 0xfff00000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, 0, 0);


/* BR/BRAAZ/BRABZ */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Br)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Br)
    DIS_ARMV8_OP(0xd61f0000, "br",             OP_ARMV8_A64_BR,         DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW),
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd61f081f, "braaz",          OP_ARMV8_A64_BRAAZ,      DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW),
    DIS_ARMV8_OP(0xd61f0c1f, "brabz",          OP_ARMV8_A64_BRABZ,      DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Br, 0xfffffc1f /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


/* BLR/BLRAAZ/BLRABZ */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Blr)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Blr)
    DIS_ARMV8_OP(0xd63f0000, "blr",            OP_ARMV8_A64_BLR,        DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd63f081f, "blraaz",         OP_ARMV8_A64_BLRAAZ,     DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
    DIS_ARMV8_OP(0xd63f0c1f, "blrabz",         OP_ARMV8_A64_BLRABZ,     DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Blr, 0xfffffc1f /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Ret)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(RetPAuth)
    DIS_ARMV8_INSN_DECODE(kDisParmParseRegFixed31,     5,  5, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Ret)
    DIS_ARMV8_OP(           0xd65f0000, "ret",            OP_ARMV8_A64_RET,        DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
    INVALID_OPCODE,
    DIS_ARMV8_OP_ALT_DECODE(0xd65f081f, "retaa",          OP_ARMV8_A64_RETAA,      DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW, RetPAuth),
    DIS_ARMV8_OP_ALT_DECODE(0xd65f0c1f, "retab",          OP_ARMV8_A64_RETAB,      DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW, RetPAuth),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Ret, 0xfffffc1f /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Eret)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Eret)
    DIS_ARMV8_OP(0xd69f03e0, "eret",           OP_ARMV8_A64_ERET,       DISOPTYPE_PRIVILEGED | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd69f0bff, "eretaa",         OP_ARMV8_A64_ERETAA,     DISOPTYPE_PRIVILEGED | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
    DIS_ARMV8_OP(0xd69f0fff, "eretab",         OP_ARMV8_A64_ERETAB,     DISOPTYPE_PRIVILEGED | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Eret, 0xffffffff /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Drps)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Drps)
    DIS_ARMV8_OP(0xd6bf03e0, "drps",           OP_ARMV8_A64_DRPS,       DISOPTYPE_PRIVILEGED | DISOPTYPE_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Drps, 0xffffffff /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


/* BRAA/BRAB */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(BraaBrab)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          0,  5, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(BraaBrab)
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd71f0800, "braa",           OP_ARMV8_A64_BRAA,       DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
    DIS_ARMV8_OP(0xd71f0c00, "brab",           OP_ARMV8_A64_BRAB,       DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(BraaBrab, 0xfffffc00 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


/* BRAA/BRAB */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(BlraaBlrab) /** @todo Could use the same decoder as for braa/brab and save a bit of table size. */
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          0,  5, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(BlraaBlrab)
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xd73f0800, "blraa",          OP_ARMV8_A64_BLRAA,      DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
    DIS_ARMV8_OP(0xd73f0c00, "blrab",          OP_ARMV8_A64_BLRAB,      DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(BlraaBlrab, 0xfffffc00 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


/* Unconditional branch (register) instructions, we divide these instructions further based on the opc field. */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(UncondBrReg)
    DIS_ARMV8_DECODE_MAP_ENTRY(Br),          /* BR/BRAAZ/BRABZ */
    DIS_ARMV8_DECODE_MAP_ENTRY(Blr),         /* BLR/BLRAA/BLRAAZ/BLRAB/BLRABZ */
    DIS_ARMV8_DECODE_MAP_ENTRY(Ret),         /* RET/RETAA/RETAB */
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_ENTRY(Eret),        /* ERET/ERETAA/ERETAB */
    DIS_ARMV8_DECODE_MAP_ENTRY(Drps),        /* DRPS */
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_ENTRY(BraaBrab),    /* BRAA/BRAB */
    DIS_ARMV8_DECODE_MAP_ENTRY(BlraaBlrab),  /* BRAA/BRAB */
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY
DIS_ARMV8_DECODE_MAP_DEFINE_END(UncondBrReg, RT_BIT_32(21) | RT_BIT_32(22) | RT_BIT_32(23) | RT_BIT_32(24), 21);


/* B/BL */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(UncondBrImm)
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmRel,         0,  26, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(UncondBrImm)
    DIS_ARMV8_OP(0x14000000, "b",              OP_ARMV8_A64_B,         DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_RELATIVE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
    DIS_ARMV8_OP(0x94000000, "bl",             OP_ARMV8_A64_BL,        DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_RELATIVE_CONTROLFLOW | DISOPTYPE_UNCOND_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(UncondBrImm, 0xfc000000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(31), 31);


/* CBZ/CBNZ */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(CmpBrImm)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmRel,         5, 19, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(CmpBrImm)
    DIS_ARMV8_OP(0x34000000, "cbz",             OP_ARMV8_A64_CBZ,       DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_RELATIVE_CONTROLFLOW | DISOPTYPE_COND_CONTROLFLOW),
    DIS_ARMV8_OP(0x35000000, "cbnz",            OP_ARMV8_A64_CBNZ,      DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_RELATIVE_CONTROLFLOW | DISOPTYPE_COND_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(CmpBrImm, 0x7f000000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(24), 24);


/* TBZ/TBNZ */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(TestBrImm)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),  /* Not an SF bit but has the same meaning. */
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmTbz,         0,  0, 1 /*idxParam*/), /* Hardcoded bit offsets in parser. */
    DIS_ARMV8_INSN_DECODE(kDisParmParseImmRel,         5, 14, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(TestBrImm)
    DIS_ARMV8_OP(0x36000000, "tbz",             OP_ARMV8_A64_TBZ,       DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_RELATIVE_CONTROLFLOW | DISOPTYPE_COND_CONTROLFLOW),
    DIS_ARMV8_OP(0x37000000, "tbnz",            OP_ARMV8_A64_TBNZ,      DISOPTYPE_HARMLESS | DISOPTYPE_CONTROLFLOW | DISOPTYPE_RELATIVE_CONTROLFLOW | DISOPTYPE_COND_CONTROLFLOW),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(TestBrImm, 0x7f000000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(24), 24);


DIS_ARMV8_DECODE_TBL_DEFINE_BEGIN(BrExcpSys)
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfe000000, RT_BIT_32(26) | RT_BIT_32(28) | RT_BIT_32(30),                  CondBr),          /* op0: 010, op1: 0xxxxxxxxxxxxx, op2: - (including o1 from the conditional branch (immediate) class to save us one layer). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xff000000, RT_BIT_32(26) | RT_BIT_32(28) | RT_BIT_32(30) | RT_BIT_32(31),  Excp),            /* op0: 110, op1: 00xxxxxxxxxxxx, op2: -. */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfffff000, 0xd5031000,                                                     SysReg),          /* op0: 110, op1: 01000000110001, op2: -. */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfffff01f, 0xd503201f,                                                     Hints),           /* op0: 110, op1: 01000000110010, op2: 11111. */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfffff01f, 0xd503301f,                                                     DecodeBarriers),  /* op0: 110, op1: 01000000110011, op2: - (we include Rt:  11111 from the next stage here). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfff8f01f, 0xd500401f,                                                     PState),          /* op0: 110, op1: 0100000xxx0100, op2: - (we include Rt:  11111 from the next stage here). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfffff0e0, 0xd5233060,                                                     SysResult),       /* op0: 110, op1: 0100100xxxxxxx, op2: - (we include op1, CRn and op2 from the next stage here). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfff80000, 0xd5080000,                                                     Sys),             /* op0: 110, op1: 0100x01xxxxxxx, op2: - (we include the L field of the next stage here to differentiate between SYS/SYSL/SYSP as they have a different string representation). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfff80000, 0xd5280000,                                                     SysL),            /* op0: 110, op1: 0100x01xxxxxxx, op2: - (we include the L field of the next stage here to differentiate between SYS/SYSL/SYSP as they have a different string representation). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfff80000, 0xd5480000,                                                     SysP),            /* op0: 110, op1: 0100x01xxxxxxx, op2: - (we include the L field of the next stage here to differentiate between SYS/SYSL/SYSP as they have a different string representation). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfff00000, 0xd5100000,                                                     Msr),             /* op0: 110, op1: 0100x1xxxxxxxx, op2: - (we include the L field of the next stage here to differentiate between MSR/MRS as they have a different string representation). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfff00000, 0xd5300000,                                                     Mrs),             /* op0: 110, op1: 0100x1xxxxxxxx, op2: - (we include the L field of the next stage here to differentiate between MSR/MRS as they have a different string representation). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0xfe1f0000, 0xd61f0000,                                                     UncondBrReg),     /* op0: 110, op1: 1xxxxxxxxxxxxx, op2: - (we include the op2 field from the next stage here as it should be always 11111). */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0x7c000000, 0x14000000,                                                     UncondBrImm),     /* op0: x00, op1: xxxxxxxxxxxxxx, op2: -. */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0x7e000000, 0x34000000,                                                     CmpBrImm),        /* op0: x01, op1: 0xxxxxxxxxxxxx, op2: -. */
    DIS_ARMV8_DECODE_TBL_ENTRY_INIT(0x7e000000, 0x36000000,                                                     TestBrImm),       /* op0: x01, op1: 1xxxxxxxxxxxxx, op2: -. */
DIS_ARMV8_DECODE_TBL_DEFINE_END(BrExcpSys);


/* AND/ORR/EOR/ANDS */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(LogShiftRegN0)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseShift,         22,  2, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseShiftAmount,   10,  6, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(LogShiftRegN0)
    DIS_ARMV8_OP(0x0a000000, "and",             OP_ARMV8_A64_AND,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x2a000000, "orr",             OP_ARMV8_A64_ORR,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x4a000000, "eor",             OP_ARMV8_A64_EOR,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x6a000000, "ands",            OP_ARMV8_A64_ANDS,      DISOPTYPE_HARMLESS)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(LogShiftRegN0, 0x7f200000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(29) | RT_BIT_32(30), 29);


/* AND/ORR/EOR/ANDS */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(LogShiftRegN1)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseShift,         22,  2, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseShiftAmount,   10,  6, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(LogShiftRegN1)
    DIS_ARMV8_OP(0x0a200000, "bic",             OP_ARMV8_A64_BIC,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x2a200000, "orn",             OP_ARMV8_A64_ORN,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x4a200000, "eon",             OP_ARMV8_A64_EON,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x6a200000, "bics",            OP_ARMV8_A64_BICS,      DISOPTYPE_HARMLESS)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(LogShiftRegN1, 0x7f200000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(29) | RT_BIT_32(30), 29);


DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(LogShiftRegN)
    DIS_ARMV8_DECODE_MAP_ENTRY(LogShiftRegN0),       /* Logical (shifted register) - N = 0 */
    DIS_ARMV8_DECODE_MAP_ENTRY(LogShiftRegN1),       /* Logical (shifted register) - N = 1 */
DIS_ARMV8_DECODE_MAP_DEFINE_END(LogShiftRegN, RT_BIT_32(21), 21);


/* ADD/ADDS/SUB/SUBS */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(AddSubExtReg)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseOption,        13,  3, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseShiftAmount,   10,  3, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(AddSubExtRegS)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseOption,        13,  3, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseShiftAmount,   10,  3, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(AddSubExtReg)
    DIS_ARMV8_OP(           0x0b200000, "add",             OP_ARMV8_A64_ADD,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP_ALT_DECODE(0x2b200000, "adds",            OP_ARMV8_A64_ADDS,      DISOPTYPE_HARMLESS, AddSubExtRegS),
    DIS_ARMV8_OP(           0x4b200000, "sub",             OP_ARMV8_A64_SUB,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP_ALT_DECODE(0x6b200000, "subs",            OP_ARMV8_A64_SUBS,      DISOPTYPE_HARMLESS, AddSubExtRegS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(AddSubExtReg, 0x7fe00000 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(29) | RT_BIT_32(30), 29);


DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(AddSubShiftExtReg)
    DIS_ARMV8_DECODE_MAP_ENTRY(AddSubShiftReg),      /* Add/Subtract (shifted register) */
    DIS_ARMV8_DECODE_MAP_ENTRY(AddSubExtReg),        /* Add/Subtract (extended register) */
DIS_ARMV8_DECODE_MAP_DEFINE_END(AddSubShiftExtReg, RT_BIT_32(21), 21);


DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(LogicalAddSubReg)
    DIS_ARMV8_DECODE_MAP_ENTRY(LogShiftRegN),        /* Logical (shifted register) */
    DIS_ARMV8_DECODE_MAP_ENTRY(AddSubShiftExtReg),   /* Add/subtract (shifted/extended register) */
DIS_ARMV8_DECODE_MAP_DEFINE_END(LogicalAddSubReg, RT_BIT_32(24), 24);


/* CCMN/CCMP */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(CondCmpReg)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,            0,  4, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseCond,          12,  4, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(CondCmpReg)
    DIS_ARMV8_OP(0x3a400000, "ccmn",            OP_ARMV8_A64_CCMN,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x7a400000, "ccmp",            OP_ARMV8_A64_CCMP,      DISOPTYPE_HARMLESS)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(CondCmpReg, 0x7fe00c10 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(30), 30);


/* CCMN/CCMP */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(CondCmpImm)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,           16,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,            0,  4, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseCond,          12,  4, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(CondCmpImm)
    DIS_ARMV8_OP(0x3a400800, "ccmn",            OP_ARMV8_A64_CCMN,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x7a400800, "ccmp",            OP_ARMV8_A64_CCMP,      DISOPTYPE_HARMLESS)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(CondCmpImm, 0x7fe00c10 /*fFixedInsn*/,
                                       kDisArmV8OpcDecodeNop, RT_BIT_32(30), 30);


/**
 * C4.1.95 - Data Processing - Register
 *
 * The conditional compare instructions differentiate between register and immediate
 * variant based on the 11th bit (part of op3).
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(CondCmp)
    DIS_ARMV8_DECODE_MAP_ENTRY(CondCmpReg),          /* Conditional compare register */
    DIS_ARMV8_DECODE_MAP_ENTRY(CondCmpImm),          /* Conditional compare immediate */
DIS_ARMV8_DECODE_MAP_DEFINE_END(CondCmp, RT_BIT_32(11), 11);


/* UDIV/SDIV/LSLV/LSRV/ASRV/RORV/CRC32.../SMAX/UMAX/SMIN/UMIN */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Reg2Src32Bit)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Reg2Src32Bit)
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP(0x1ac00800, "udiv",            OP_ARMV8_A64_UDIV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac00c00, "sdiv",            OP_ARMV8_A64_SDIV,      DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP(0x1ac02000, "lslv",            OP_ARMV8_A64_LSLV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac02400, "lsrv",            OP_ARMV8_A64_LSRV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac02800, "asrv",            OP_ARMV8_A64_ASRV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac02c00, "rorv",            OP_ARMV8_A64_RORV,      DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP(0x1ac04000, "crc32b",          OP_ARMV8_A64_CRC32B,    DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac04400, "crc32h",          OP_ARMV8_A64_CRC32H,    DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac04800, "crc32w",          OP_ARMV8_A64_CRC32W,    DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
    DIS_ARMV8_OP(0x1ac05000, "crc32cb",         OP_ARMV8_A64_CRC32CB,   DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac05400, "crc32ch",         OP_ARMV8_A64_CRC32CH,   DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac05800, "crc32cw",         OP_ARMV8_A64_CRC32CW,   DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
    DIS_ARMV8_OP(0x1ac06000, "smax",            OP_ARMV8_A64_SMAX,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac06400, "umax",            OP_ARMV8_A64_UMAX,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac06800, "smin",            OP_ARMV8_A64_SMIN,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1ac06c00, "umin",            OP_ARMV8_A64_UMIN,      DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Reg2Src32Bit, 0xffe0fc00 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


/* UDIV/SDIV/LSLV/LSRV/ASRV/RORV/CRC32.../SMAX/UMAX/SMIN/UMIN */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Reg2Src64Bit)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(Reg2SrcCrc32X)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,        5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,       16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(Reg2SrcSubp)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,         16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(Reg2SrcIrg)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,       16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(Reg2SrcGmi)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(Reg2SrcPacga)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,         16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Reg2Src64Bit)
    DIS_ARMV8_OP_ALT_DECODE(0x9ac00000, "subp",            OP_ARMV8_A64_SUBP,      DISOPTYPE_HARMLESS, Reg2SrcSubp),
    INVALID_OPCODE,
    DIS_ARMV8_OP(           0x9ac00800, "udiv",            OP_ARMV8_A64_UDIV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x9ac00c00, "sdiv",            OP_ARMV8_A64_SDIV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP_ALT_DECODE(0x9ac01000, "irg",             OP_ARMV8_A64_IRG,       DISOPTYPE_HARMLESS, Reg2SrcIrg),
    DIS_ARMV8_OP_ALT_DECODE(0x9ac01400, "gmi",             OP_ARMV8_A64_GMI,       DISOPTYPE_HARMLESS, Reg2SrcGmi),
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP(           0x9ac02000, "lslv",            OP_ARMV8_A64_LSLV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x9ac02400, "lsrv",            OP_ARMV8_A64_LSRV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x9ac02800, "asrv",            OP_ARMV8_A64_ASRV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x9ac02c00, "rorv",            OP_ARMV8_A64_RORV,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP_ALT_DECODE(0x9ac03000, "pacga",           OP_ARMV8_A64_PACGA,     DISOPTYPE_HARMLESS, Reg2SrcPacga),
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP_ALT_DECODE(0x9ac04c00, "crc32x",          OP_ARMV8_A64_CRC32X,    DISOPTYPE_HARMLESS, Reg2SrcCrc32X),
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP_ALT_DECODE(0x9ac05c00, "crc32cx",         OP_ARMV8_A64_CRC32CX,   DISOPTYPE_HARMLESS, Reg2SrcCrc32X),
    DIS_ARMV8_OP(           0x9ac06000, "smax",            OP_ARMV8_A64_SMAX,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x9ac06400, "umax",            OP_ARMV8_A64_UMAX,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x9ac06800, "smin",            OP_ARMV8_A64_SMIN,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x9ac06c00, "umin",            OP_ARMV8_A64_UMIN,      DISOPTYPE_HARMLESS)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Reg2Src64Bit, 0xffe0fc00 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


/* SUBPS */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Subps)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprSp,         16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Subps)
    DIS_ARMV8_OP(0xbac00000, "subps",           OP_ARMV8_A64_SUBPS,     DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Subps, 0xffe0fc00 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


/**
 * C4.1.95 - Data Processing - Register - 2-source
 *
 * Differentiate between 32-bit and 64-bit groups based on the SF bit.
 * Not done as a general decoder step because there are different instructions in each group.
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(Reg2Src)
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg2Src32Bit),         /* Data-processing (2-source, 32-bit) */
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg2Src64Bit),         /* Data-processing (2-source, 64-bit) */
DIS_ARMV8_DECODE_MAP_DEFINE_END_SINGLE_BIT(Reg2Src, 31);


/**
 * C4.1.95 - Data Processing - Register - 2-source
 *
 * Differentiate between SUBPS and the rest based on the S bit.
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(Reg2SrcSubps)
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg2Src),             /* Data-processing (2-source) */
    DIS_ARMV8_DECODE_MAP_ENTRY(Subps),               /* Subps */
DIS_ARMV8_DECODE_MAP_DEFINE_END_SINGLE_BIT(Reg2SrcSubps, 29);


/* RBIT/REV16/REV/CLZ/CLS/CTZ/CNT/ABS/REV32 */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Reg1SrcInsn)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Reg1SrcInsn)
    DIS_ARMV8_OP(0x5ac00000, "rbit",            OP_ARMV8_A64_RBIT,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x5ac00400, "rev16",           OP_ARMV8_A64_REV16,     DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x5ac00800, "rev",             OP_ARMV8_A64_REV,       DISOPTYPE_HARMLESS), /** @todo REV32 if SF1 is 1 (why must this be so difficult ARM?). */
    DIS_ARMV8_OP(0x5ac00c00, "rev",             OP_ARMV8_A64_REV,       DISOPTYPE_HARMLESS), /** @todo SF must be 1, otherwise unallocated. */
    DIS_ARMV8_OP(0x5ac01000, "clz",             OP_ARMV8_A64_CLZ,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x5ac01400, "cls",             OP_ARMV8_A64_CLS,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x5ac01800, "ctz",             OP_ARMV8_A64_CTZ,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x5ac01c00, "cnt",             OP_ARMV8_A64_CNT,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x5ac02000, "abs",             OP_ARMV8_A64_ABS,       DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Reg1SrcInsn, 0x7ffffc00 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(12) | RT_BIT_32(13) | RT_BIT_32(14) | RT_BIT_32(15), 10);


/**
 * C4.1.95 - Data Processing - Register - 1-source
 *
 * Differentiate between standard and FEAT_PAuth instructions based on opcode2 field.
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(Reg1Src)
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg1SrcInsn),          /* Data-processing (1-source) */
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,               /* Data-processing (1-source, FEAT_PAuth) */
DIS_ARMV8_DECODE_MAP_DEFINE_END_SINGLE_BIT(Reg1Src, 16);


/**
 * C4.1.95 - Data Processing - Register - 2-source / 1-source
 *
 * The 2-source and 1-source instruction classes differentiate based on bit 30.
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(Reg2Src1Src)
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg2SrcSubps),        /* Data-processing (2-source) */
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg1Src),             /* Data-processing (1-source) */
DIS_ARMV8_DECODE_MAP_DEFINE_END_SINGLE_BIT(Reg2Src1Src, 30);


/* CSEL/CSINC/CSINV/CSNEG */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(CondSel)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseCond,          12,  4, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(CondSel)
    DIS_ARMV8_OP(0x1a800000, "csel",            OP_ARMV8_A64_CSEL,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1a800400, "csinc",           OP_ARMV8_A64_CSINC,     DISOPTYPE_HARMLESS),
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP(0x5a800000, "csinv",           OP_ARMV8_A64_CSINC,     DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x5a800400, "csneg",           OP_ARMV8_A64_CSNEG,     DISOPTYPE_HARMLESS)
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(CondSel, 0x7fe00c00 /*fFixedInsn*/, kDisArmV8OpcDecodeCollate,
                                       RT_BIT_32(10) | RT_BIT_32(11) | RT_BIT_32(30), 10);


/* MADD/MSUB (32-bit) */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Reg3Src32)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,        5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,       16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,       10,  5, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Reg3Src32)
    DIS_ARMV8_OP(0x1b000000, "madd",            OP_ARMV8_A64_MADD,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x1b008000, "msub",            OP_ARMV8_A64_MSUB,      DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Reg3Src32, 0xffe08000 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(15), 15);


/* MADD/MSUB (64-bit) /SMADDL/SMSUBL/SMULH/UMADDL/UMSUBL/UMULH */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(Reg3Src64)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,       16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,       10,  5, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(Reg3Src64_32)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,        5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,       16,  5, 2 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,       10,  5, 3 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER_ALTERNATIVE(Reg3Src64Mul) /** @todo Ra == 11111 (or is it ignored?) */
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,       16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(Reg3Src64)
    DIS_ARMV8_OP(           0x9b000000, "madd",            OP_ARMV8_A64_MADD,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(           0x9b008000, "msub",            OP_ARMV8_A64_MSUB,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP_ALT_DECODE(0x9b200000, "smaddl",          OP_ARMV8_A64_SMADDL,    DISOPTYPE_HARMLESS, Reg3Src64_32),
    DIS_ARMV8_OP_ALT_DECODE(0x9b208000, "smsubl",          OP_ARMV8_A64_SMSUBL,    DISOPTYPE_HARMLESS, Reg3Src64_32),
    DIS_ARMV8_OP_ALT_DECODE(0x9b400000, "smulh",           OP_ARMV8_A64_SMULH,     DISOPTYPE_HARMLESS, Reg3Src64Mul),
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    INVALID_OPCODE,
    DIS_ARMV8_OP_ALT_DECODE(0x9ba00000, "umaddl",          OP_ARMV8_A64_UMADDL,    DISOPTYPE_HARMLESS, Reg3Src64_32),
    DIS_ARMV8_OP_ALT_DECODE(0x9ba08000, "umsubl",          OP_ARMV8_A64_UMSUBL,    DISOPTYPE_HARMLESS, Reg3Src64_32),
    DIS_ARMV8_OP_ALT_DECODE(0x9bc00000, "umulh",           OP_ARMV8_A64_UMULH,     DISOPTYPE_HARMLESS, Reg3Src64Mul),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(Reg3Src64, 0xffe08000 /*fFixedInsn*/, kDisArmV8OpcDecodeCollate,
                                       RT_BIT_32(15) | RT_BIT_32(21) | RT_BIT_32(22) | RT_BIT_32(23), 15);


/**
 * C4.1.95.12 - Data Processing - Register - 3-source
 *
 * We differentiate further based on SF because there are different instructions encoded
 * for 32-bit and 64-bit.
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(Reg3Src)
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src32),        /* 3-source 32-bit */
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src64),        /* 3-source 64-bit */
DIS_ARMV8_DECODE_MAP_DEFINE_END_SINGLE_BIT(Reg3Src, 31);


/* ADC/ADCS/SBC/SBCS */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(AddSubCarry)
    DIS_ARMV8_INSN_DECODE(kDisParmParseSf,            31,  1, DIS_ARMV8_INSN_PARAM_UNSET),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          0,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,          5,  5, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr,         16,  5, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(AddSubCarry)
    DIS_ARMV8_OP(0x1a000000, "adc",             OP_ARMV8_A64_ADC,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x3a000000, "adcs",            OP_ARMV8_A64_ADCS,      DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x5a000000, "sbc",             OP_ARMV8_A64_SBC,       DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x7a000000, "sbcs",            OP_ARMV8_A64_SBCS,      DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(AddSubCarry, 0x7fe0fc00 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(29) | RT_BIT_32(30), 29);


/* RMIF */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(RotateIntoFlags)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr64,        5,  5, 0 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,           15,  6, 1 /*idxParam*/),
    DIS_ARMV8_INSN_DECODE(kDisParmParseImm,            0,  4, 2 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(RotateIntoFlags)
    INVALID_OPCODE,
    DIS_ARMV8_OP(0xba000400, "rmif",            OP_ARMV8_A64_RMIF,      DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(RotateIntoFlags, 0xffe07c10 /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(29), 29);


/* SETF8/SETF16 */
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_DECODER(EvaluateIntoFlags)
    DIS_ARMV8_INSN_DECODE(kDisParmParseGprZr32,        5,  5, 0 /*idxParam*/),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_BEGIN(EvaluateIntoFlags)
    DIS_ARMV8_OP(0x3a00080d, "setf8",           OP_ARMV8_A64_SETF8,     DISOPTYPE_HARMLESS),
    DIS_ARMV8_OP(0x3a00480d, "setf16",          OP_ARMV8_A64_SETF16,    DISOPTYPE_HARMLESS),
DIS_ARMV8_DECODE_INSN_CLASS_DEFINE_END(EvaluateIntoFlags, 0xfffffc1f /*fFixedInsn*/, kDisArmV8OpcDecodeNop,
                                       RT_BIT_32(14), 14);


/**
 * C4.1.95 - Data Processing - Register
 *
 * Differentiate between add/sub (with carry) / rotate right / evaluate by op3<1:0>.
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(AddSubRotateEval)
    DIS_ARMV8_DECODE_MAP_ENTRY(AddSubCarry),
    DIS_ARMV8_DECODE_MAP_ENTRY(RotateIntoFlags),
    DIS_ARMV8_DECODE_MAP_ENTRY(EvaluateIntoFlags),
DIS_ARMV8_DECODE_MAP_DEFINE_END(AddSubRotateEval, RT_BIT_32(10) | RT_BIT_32(11), 10);


/*
 * C4.1.95 - Data Processing - Register
 *
 * The op1 field is already decoded in the previous step and is 1 when being here,
 * leaving us with the following possible values:
 *
 *     Bit  24 23 22 21
 *     +-------------------------------------------
 *           0  0  0  0 Add/subtract with carry / Rotate right into flags / Evaluate into flags (depending on op3)
 *           0  0  0  1 UNALLOC
 *           0  0  1  0 Conditional compare (register / immediate)
 *           0  0  1  1 UNALLOC
 *           0  1  0  0 Conditional select
 *           0  1  0  1 UNALLOC
 *           0  1  1  0 Data processing (2-source or 1-source depending on op0).
 *           0  1  1  1 UNALLOC
 *           1  x  x  x Data processing 3-source
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(DataProcReg)
    DIS_ARMV8_DECODE_MAP_ENTRY(AddSubRotateEval),
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_ENTRY(CondCmp),
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_ENTRY(CondSel),
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg2Src1Src),
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src),
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src),
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src),
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src),
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src),
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src),
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src),
    DIS_ARMV8_DECODE_MAP_ENTRY(Reg3Src),
DIS_ARMV8_DECODE_MAP_DEFINE_END(DataProcReg, RT_BIT_32(21) | RT_BIT_32(22) | RT_BIT_32(23) | RT_BIT_32(24), 21);


/*
 * C4.1 of the ARMv8 architecture reference manual has the following table for the
 * topmost decoding level (Level 0 in our terms), x means don't care:
 *
 *     Bit  28 27 26 25
 *     +-------------------------------------------
 *           0  0  0  0 Reserved or SME encoding (depends on bit 31).
 *           0  0  0  1 UNALLOC
 *           0  0  1  0 SVE encodings
 *           0  0  1  1 UNALLOC
 *           1  0  0  x Data processing immediate
 *           1  0  1  x Branch, exception generation and system instructions
 *           x  1  x  0 Loads and stores
 *           x  1  0  1 Data processing - register
 *           x  1  1  1 Data processing - SIMD and floating point
 *
 * In order to save us some fiddling with the don't care bits we blow up the lookup table
 * which gives us 16 possible values (4 bits) we can use as an index into the decoder
 * lookup table for the next level:
 *     Bit  28 27 26 25
 *     +-------------------------------------------
 *      0    0  0  0  0 Reserved or SME encoding (depends on bit 31).
 *      1    0  0  0  1 UNALLOC
 *      2    0  0  1  0 SVE encodings
 *      3    0  0  1  1 UNALLOC
 *      4    0  1  0  0 Loads and stores
 *      5    0  1  0  1 Data processing - register (using op1 (bit 28) from the next stage to differentiate further already)
 *      6    0  1  1  0 Loads and stores
 *      7    0  1  1  1 Data processing - SIMD and floating point
 *      8    1  0  0  0 Data processing immediate
 *      9    1  0  0  1 Data processing immediate
 *     10    1  0  1  0 Branch, exception generation and system instructions
 *     11    1  0  1  1 Branch, exception generation and system instructions
 *     12    1  1  0  0 Loads and stores
 *     13    1  1  0  1 Data processing - register (using op1 (bit 28) from the next stage to differentiate further already)
 *     14    1  1  1  0 Loads and stores
 *     15    1  1  1  1 Data processing - SIMD and floating point
 */
DIS_ARMV8_DECODE_MAP_DEFINE_BEGIN(DecodeL0)
    DIS_ARMV8_DECODE_MAP_ENTRY(Rsvd),                               /* Reserved class or SME encoding (@todo). */
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,                             /* Unallocated */
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,                             /** @todo SVE */
    DIS_ARMV8_DECODE_MAP_INVALID_ENTRY,                             /* Unallocated */
    DIS_ARMV8_DECODE_MAP_ENTRY(LdStOp0Lo),                          /* Load/Stores. */
    DIS_ARMV8_DECODE_MAP_ENTRY(LogicalAddSubReg),                   /* Data processing (register) (see op1 in C4.1.68). */
    DIS_ARMV8_DECODE_MAP_ENTRY(LdStOp0Lo),                          /* Load/Stores. */
    DIS_ARMV8_DECODE_MAP_ENTRY(DataProcSimdFpBit28_0),              /* Data processing (SIMD & FP) (op0<0> 0) */
    DIS_ARMV8_DECODE_MAP_ENTRY(DataProcessingImm),                  /* Data processing (immediate). */
    DIS_ARMV8_DECODE_MAP_ENTRY(DataProcessingImm),                  /* Data processing (immediate). */
    DIS_ARMV8_DECODE_MAP_ENTRY(BrExcpSys),                          /* Branches / Exception generation and system instructions. */
    DIS_ARMV8_DECODE_MAP_ENTRY(BrExcpSys),                          /* Branches / Exception generation and system instructions. */
    DIS_ARMV8_DECODE_MAP_ENTRY(LdStOp0Lo),                          /* Load/Stores. */
    DIS_ARMV8_DECODE_MAP_ENTRY(DataProcReg),                        /* Data processing (register) (see op1 in C4.1.68). */
    DIS_ARMV8_DECODE_MAP_ENTRY(LdStOp0Lo),                          /* Load/Stores. */
    DIS_ARMV8_DECODE_MAP_ENTRY(DataProcSimdFpBit28_1)               /* Data processing (SIMD & FP) (op0<0> 1). */
DIS_ARMV8_DECODE_MAP_DEFINE_END_NON_STATIC(DecodeL0, RT_BIT_32(25) | RT_BIT_32(26) | RT_BIT_32(27) | RT_BIT_32(28), 25);
