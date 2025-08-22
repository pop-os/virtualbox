#!/usr/bin/env python
# -*- coding: utf-8 -*-
# $Id: ArmBsdSpec.py $

"""
ARM BSD / OpenSource specification reader.
"""

from __future__ import print_function;

__copyright__ = \
"""
Copyright (C) 2025 Oracle and/or its affiliates.

This file is part of VirtualBox base platform packages, as
available from https://www.virtualbox.org.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, in version 3 of the
License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <https://www.gnu.org/licenses>.

SPDX-License-Identifier: GPL-3.0-only
"""
__version__ = "$Revision: 169505 $"

# Standard python imports.
import collections;
import functools;
import json;
import operator;
import os;
import re;
import tarfile;
import time;

# AST imports:
from ArmAst import assertJsonAttribsInSet
from ArmAst import ArmAstBase
from ArmAst import ArmAstBinaryOp
from ArmAst import ArmAstDotAtom
from ArmAst import ArmAstConcat
from ArmAst import ArmAstFunction
from ArmAst import ArmAstIdentifier
from ArmAst import ArmAstBool
from ArmAst import ArmAstInteger
from ArmAst import ArmAstSet
from ArmAst import ArmAstValue
from ArmAst import ArmAstField
from ArmAst import ArmAstIfList


#
# Instructions and their properties.
#

class ArmEncodesetField(object):
    """
    ARM Encodeset.Bits & Encodeset.Field.
    """
    def __init__(self, oJson, iFirstBit, cBitsWidth, fFixed, fValue, sName = None):
        self.oJson      = oJson;
        self.iFirstBit  = iFirstBit;
        self.cBitsWidth = cBitsWidth;
        self.fFixed     = fFixed;
        self.fValue     = fValue;
        self.sName      = sName; ##< None if Encodeset.Bits.

    def __str__(self):
        sRet = '[%2u:%-2u] = %#x/%#x/%#x' % (
            self.iFirstBit + self.cBitsWidth - 1, self.iFirstBit, self.fValue, self.fFixed, self.getMask()
        );
        if self.sName:
            sRet += ' # %s' % (self.sName,)
        return sRet;

    def __repr__(self):
        return self.__str__();

    def clone(self):
        return ArmEncodesetField(self.oJson, self.iFirstBit, self.cBitsWidth, self.fFixed, self.fValue, self.sName);

    def getMask(self):
        """ Field mask (unshifted). """
        return (1 << self.cBitsWidth) - 1;

    def getShiftedMask(self):
        """ Field mask, shifted. """
        return ((1 << self.cBitsWidth) - 1) << self.iFirstBit;

    @staticmethod
    def fromJson(oJson):
        assert oJson['_type'] in ('Instruction.Encodeset.Field', 'Instruction.Encodeset.Bits'), oJson['_type'];

        oRange = oJson['range'];
        assert oRange['_type'] == 'Range';
        iFirstBit           = int(oRange['start']);
        cBitsWidth          = int(oRange['width']);
        sName               = oJson['name'] if oJson['_type'] == 'Instruction.Encodeset.Field' else None;
        (fValue, fFixed, _, _) = ArmAstValue.parseValue(oJson['value']['value'], cBitsWidth);
        return ArmEncodesetField(oJson, iFirstBit, cBitsWidth, fFixed, fValue, sName);

    @staticmethod
    def encodesetFromJson(oJson):
        assert oJson['_type'] == 'Instruction.Encodeset.Encodeset', oJson['_type'];
        aoSet   = [];
        fFields = 0;
        for oJsonValue in oJson['values']:
            oNewField = ArmEncodesetField.fromJson(oJsonValue);
            fNewMask  = oNewField.getShiftedMask();
            assert (fNewMask & fFields) == 0;
            aoSet.append(oNewField);
            fFields  |= fNewMask;
        return (aoSet, fFields);

    @staticmethod
    def encodesetAddParentFields(aoFields, fFields, aoParentFields):
        for oParentField in aoParentFields:
            fNewMask  = oParentField.getShiftedMask();
            if (fNewMask & fFields) != fNewMask:
                aoFields.append(oParentField.clone()); # (paranoid: clone)
                fFields |= fNewMask;
        return (aoFields, fFields);


class ArmInstructionBase(object):
    """
    Base class for ArmInstruction, ArmInstructionSet and ArmInstructionGroup

    Instances of ArmInstruction will have ArmInstructionGroup (or maybe
    ArmInstructionGroup) as parent.

    Instances of ArmInstructionGroup have ArmInstructionSet as parent.

    Instances of ArmInstructionSet doesn't have a parent, so it is None.
    """

    koReValidName = re.compile('^[_A-Za-z][_A-Za-z0-9]+$');

    def __init__(self, oJson, sName, aoFields, fFields, oCondition, oParent):
        self.oJson           = oJson;
        self.sName           = sName;
        self.oParent         = oParent;
        self.aoFields        = aoFields     # type: List[ArmEncodesetField]
        self.fFields         = fFields;
        self.oCondition      = oCondition;

        assert ArmInstructionBase.koReValidName.match(sName), '%s' % (sName);
        assert (oJson['_type'] == 'Instruction.InstructionSet') == (oParent is None);
        assert not oParent or isinstance(oParent, (ArmInstructionGroup, ArmInstructionSet));


    def getUpIterator(self):
        """ Get an iterator the starts with 'self' and goes up the parent chain. """
        class UpIterator(object):
            def __init__(self, oNode):
                self.oNext = oNode;

            def __iter__(self):
                return self;

            def __next__(self):
                oRet = self.oNext;
                if oRet:
                    self.oNext = oRet.oParent;
                    return oRet;
                raise StopIteration();
        return UpIterator(self);

    def getFieldByName(self, sName, fRaiseXpctIfNotFound = True):
        """ Looks up a named field in the aoFields. """
        for oField in self.aoFields:
            if oField.sName and oField.sName == sName:
                return oField;
        if fRaiseXpctIfNotFound:
            raise Exception('Could not find field %s in instruction %s' % (sName, self.sName,));
        return None;


class ArmInstructionOrganizerBase(ArmInstructionBase):
    """ Common base class for ArmInstructionSet and ArmInstructionGroup. """

    koReValidName = re.compile('^[_A-Za-z][_A-Za-z0-9]+$');

    def __init__(self, oJson, sName, aoFields, fFields, oCondition, oParent = None):
        ArmInstructionBase.__init__(self, oJson, sName, aoFields, fFields, oCondition, oParent);
        self.aoInstructions     = [];   ##< The instruction directly under this object (no in sub-set or groups).
        self.dInstructions      = {};   ##< The instructions in self.aoInstructions indexed by name.
        self.aoAllInstructions  = [];   ##< All the instructions in this set.
        self.dAllInstructions   = {};   ##< The instructions in self.aoAllInstructions indexed by name.
        self.aoGroups           = [];   ##< Groups under this object.

    def toString(self):
        return '%s-name=%s Fields=#%u/%#010x cond=%s parent=%s' \
             % ('set' if isinstance(self, ArmInstructionSet) else 'group', self.sName,
                len(self.aoFields), self.fFields, self.oCondition.toString(), self.oParent.sName if self.oParent else '<none>',);

    def addInstruction(self, oInstr):
        """ Recursively adds the instruction to the ALL collections."""
        self.aoAllInstructions.append(oInstr);
        assert oInstr.sName not in self.dAllInstructions;
        self.dAllInstructions[oInstr.sName] = oInstr;

        if self.oParent:
            self.oParent.addInstruction(oInstr);
        else:
            g_dAllArmInstructionsBySet[self.sName].append(oInstr);

    def addImmediateInstruction(self, oInstr):
        """ Adds an instruction immediately below this group/set. """
        assert oInstr.sName not in self.dInstructions;
        assert oInstr.oParent == self;

        self.aoInstructions.append(oInstr);
        self.dInstructions[oInstr.sName] = oInstr;

        if self.oParent:
            assert isinstance(self, ArmInstructionGroup);
            g_dAllArmInstructionsByGroup[self.sName].append(oInstr);

        self.addInstruction(oInstr);

class ArmInstructionSet(ArmInstructionOrganizerBase):
    """ Representation of a Instruction.InstructionSet object. """

    def __init__(self, oJson, sName, aoFields, fFields, oCondition, cBitsWidth):
        ArmInstructionOrganizerBase.__init__(self, oJson, sName, aoFields, fFields, oCondition);
        self.cBitsWidth = cBitsWidth;
        assert cBitsWidth == 32;

    def __str__(self):
        return self.toString();

    def __repr__(self):
        return self.toString();

    def toString(self):
        return ArmInstructionOrganizerBase.toString(self) + ' read_bits=%u' % (self.cBitsWidth,);

    @staticmethod
    def fromJson(oJson):
        assert oJson['_type'] == 'Instruction.InstructionSet';
        sName = oJson['name'];

        (aoFields, fFields) = ArmEncodesetField.encodesetFromJson(oJson['encoding']);
        oCondition          = ArmAstBase.fromJson(oJson['condition']);
        print('debug: Instruction set %s' % (sName,));
        return ArmInstructionSet(oJson, sName, aoFields, fFields, oCondition, int(oJson['read_width']));


class ArmInstructionGroup(ArmInstructionOrganizerBase):
    """ Representation of a Instruction.InstructionGroup object. """

    def __init__(self, oJson, sName, aoFields, fFields, oCondition, oParent):
        ArmInstructionOrganizerBase.__init__(self, oJson, sName, aoFields, fFields, oCondition, oParent);

    def __str__(self):
        return self.toString();

    def __repr__(self):
        return self.toString();

    def toString(self):
        return ArmInstructionOrganizerBase.toString(self);

    @staticmethod
    def fromJson(oJson, oParent):
        assert oJson['_type'] == 'Instruction.InstructionGroup';
        sName = oJson['name'];

        (aoFields, fFields) = ArmEncodesetField.encodesetFromJson(oJson['encoding']);
        oCondition          = ArmAstBase.fromJson(oJson['condition']);
        #print('debug: Instruction group %s' % (sName,));
        return ArmInstructionGroup(oJson, sName, aoFields, fFields, oCondition, oParent);



class ArmInstruction(ArmInstructionBase):
    """
    ARM instruction
    """
    koReValidName = re.compile('^[_A-Za-z][_A-Za-z0-9]+$');

    def __init__(self, oJson, sName, sMemonic, sAsmDisplay, aoFields, fFields, oCondition, oParent):
        ArmInstructionBase.__init__(self, oJson, sName, aoFields, fFields, oCondition, oParent);
        self.sMnemonic       = sMemonic;
        self.sAsmDisplay     = sAsmDisplay;
        self.fFixedMask      = 0;
        self.fFixedValue     = 0;
        for oField in aoFields:
            self.fFixedMask  |= oField.fFixed << oField.iFirstBit;
            self.fFixedValue |= oField.fValue << oField.iFirstBit;

        # State related to decoder.
        self.fDecoderLeafCheckNeeded = False;    ##< Whether we need to check fixed value/mask in leaf decoder functions.

        # Check input.
        assert self.koReValidName.match(sName), 'sName=%s' % (sName);

    def toString(self, cchName = 0, fEncoding = False):
        if self.sName == self.sMnemonic:
            sRet = 'sName=%-*s' % (cchName, self.sName,);
        else:
            sRet = 'sName=%-*s sMnemonic=%-*s' % (cchName, self.sName, cchName, self.sMnemonic);
        if not fEncoding:
            return '%s fFixedValue/Mask=%#x/%#x #encoding=%s' % (sRet, self.fFixedValue, self.fFixedMask, len(self.aoFields));
        return '%s fFixedValue/Mask=%#x/%#x encoding=\n    %s' \
             % (sRet, self.fFixedValue, self.fFixedMask, ',\n    '.join([str(s) for s in self.aoFields]),);

    def __str__(self):
        return self.toString();

    def __repr__(self):
        return self.toString();

    def getNamedNonFixedFieldsSortedByPosition(self):
        """
        Gets aoFields filtered by sName and fFixed != getMask(), sorted by iFirstBit.

        Note! This is used for generating argument lists.
        """
        return sorted([oField for oField in self.aoFields if oField.sName and oField.fFixed != oField.getMask()],
                      key = operator.attrgetter('iFirstBit'));

    def getCName(self):
        # Get rid of trailing underscore as it seems pointless.
        if self.sName[-1] != '_' or self.sName[:-1] in g_dAllArmInstructionsByName:
            return self.sName;
        return self.sName[:-1];

    def getInstrSetName(self):
        """ Returns the instruction set name. """
        oCur = self.oParent;
        while True:
            oParent = oCur.oParent;
            if oParent:
                oCur = oParent;
            else:
                return oCur.sName;

    def getSetAndGroupNames(self):
        asNames = [];
        oParent = self.oParent;
        while oParent:
            asNames.append(oParent.sName)
            oParent = oParent.oParent;
        return asNames;

    def getSetAndGroupNamesWithLabels(self):
        asNames = self.getSetAndGroupNames();
        if len(asNames) > 1:
            return 'Instruction Set: %s  Group%s: %s' % (asNames[-1], 's' if len(asNames) > 2 else '', ', '.join(asNames[:-1]),);
        return 'Instruction Set: %s' % (asNames[-1],);



#
# Features and their properties.
#

## To deal with bugs, things we get wrong and, 'missing' stuff.
g_dArmFeatureSupportExprOverrides = {
    'FEAT_AA32':    ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL0', 'ID_AA64PFR0_EL1'),]), '==', ArmAstInteger(2)),
    'FEAT_AA32EL0': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL0', 'ID_AA64PFR0_EL1'),]), '==', ArmAstInteger(2)),
    'FEAT_AA64EL2': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL2', 'ID_AA64PFR0_EL1'),]), '>=', ArmAstInteger(1)),
    'FEAT_AA64EL3': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL2', 'ID_AA64PFR0_EL1'),]), '>=', ArmAstInteger(1)),

    # Obsolete/whatever:
    'FEAT_AA32EL1': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL1', 'ID_AA64PFR0_EL1'),]), '==', ArmAstInteger(2)),
    'FEAT_AA32EL2': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL2', 'ID_AA64PFR0_EL1'),]), '==', ArmAstInteger(2)),
    'FEAT_AA32EL3': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL3', 'ID_AA64PFR0_EL1'),]), '==', ArmAstInteger(2)),
    'FEAT_AA64EL0': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL0', 'ID_AA64PFR0_EL1'),]), '>=', ArmAstInteger(1)),
    'FEAT_AA64EL1': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EL1', 'ID_AA64PFR0_EL1'),]), '>=', ArmAstInteger(1)),
    'FEAT_AA64':    ArmAstBinaryOp(ArmAstBinaryOp(ArmAstIdentifier('FEAT_AA64EL0'), '||', ArmAstIdentifier('FEAT_AA64EL1')),
                                   '||',
                                   ArmAstBinaryOp(ArmAstIdentifier('FEAT_AA64EL2'), '||', ArmAstIdentifier('FEAT_AA64EL3'))),

    # Spec bugs:
    'FEAT_S2FWB':   ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('FWB', 'ID_AA64MMFR2_EL1'),]), '>=', ArmAstInteger(1)),
    'FEAT_UAO':     ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('UAO', 'ID_AA64MMFR2_EL1'),]), '>=', ArmAstInteger(1)),

    # Spec bugs in 2024-12:
    'FEAT_PMUv3_TH2': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('EDGE', 'PMMIR_EL1'),]), '>=', ArmAstInteger(2)),
    'FEAT_PACIMP': ArmAstBinaryOp.andListToTree([
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('GPI', 'ID_AA64ISAR1_EL1'),]), '>=', ArmAstInteger(1)),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('API', 'ID_AA64ISAR1_EL1'),]), '>=', ArmAstInteger(1)),
    ]),
    'FEAT_PACQARMA5': ArmAstBinaryOp.andListToTree([
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('GPA', 'ID_AA64ISAR1_EL1'),]), '>=', ArmAstInteger(1)),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('APA', 'ID_AA64ISAR1_EL1'),]), '>=', ArmAstInteger(1)),
    ]),
    'FEAT_PACQARMA3': ArmAstBinaryOp.andListToTree([
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('GPA3', 'ID_AA64ISAR2_EL1'),]), '>=', ArmAstInteger(1)),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('APA3', 'ID_AA64ISAR2_EL1'),]), '>=', ArmAstInteger(1)),
    ]),

    # Missing info:
    'FEAT_F8F16MM': ArmAstField('F8MM4', 'ID_AA64FPFR0_EL1'),
    'FEAT_F8F32MM': ArmAstField('F8MM8', 'ID_AA64FPFR0_EL1'),
    'FEAT_SVE_F16F32MM': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('F16MM', 'ID_AA64ZFR0_EL1'),]),
                                        '>=', ArmAstInteger(1)),
    'FEAT_PAuth':   ArmAstBinaryOp.orListToTree([
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('API',  'ID_AA64ISAR1_EL1'),]), '>=', ArmAstInteger(1)),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('APA',  'ID_AA64ISAR1_EL1'),]), '>=', ArmAstInteger(1)),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('APA3', 'ID_AA64ISAR2_EL1'),]), '>=', ArmAstInteger(1)),
    ]),

    ## @todo FEAT_GICv3, FEAT_GICv3p, FEAT_GICv4 & FEAT_GICv4p1 detection is most probably incomplete or wrong.
    'FEAT_GICv3':   ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('GIC', 'ID_AA64PFR0_EL1'),]), '>=', ArmAstInteger(1)),
    'FEAT_GICv3p1': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('GIC', 'ID_AA64PFR0_EL1'),]), '>=', ArmAstInteger(1)),
    'FEAT_GICv4':   ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('GIC', 'ID_AA64PFR0_EL1'),]), '>=', ArmAstInteger(1)),
    'FEAT_GICv4p1': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('GIC', 'ID_AA64PFR0_EL1'),]), '>=', ArmAstInteger(3)),
    'FEAT_GICv3_NMI':  ArmAstBinaryOp(ArmAstIdentifier('FEAT_GICv3'), '&&', ArmAstIdentifier('FEAT_NMI')), # ?? from linux patch
    'FEAT_GICv3_TDIR': ArmAstBinaryOp(ArmAstIdentifier('FEAT_GICv3'), '&&', ArmAstField('TDS', 'ICH_VTR_EL2'),), # ??

    # Missing in 2024-12:
    'FEAT_SSVE_FEXPA': ArmAstField('SFEXPA', 'ID_AA64SMFR0_EL1'),

    # Odd ones:
    'FEAT_CHK':     ArmAstIdentifier('FEAT_GCS'),
    'FEAT_ETE':     ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('TraceVer', 'ID_AA64DFR0_EL1'),]),
                                   '>=', ArmAstInteger(1)),
    'FEAT_ETEv1p1': ArmAstBinaryOp(ArmAstIdentifier('FEAT_ETE'), '&&',
                                   ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]),
                                                  '>=', ArmAstInteger(1))),
    'FEAT_ETEv1p2': ArmAstBinaryOp(ArmAstIdentifier('FEAT_ETE'), '&&',
                                   ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]),
                                                  '>=', ArmAstInteger(2))),
    'FEAT_ETEv1p3': ArmAstBinaryOp(ArmAstIdentifier('FEAT_ETE'), '&&',
                                   ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]),
                                                  '>=', ArmAstInteger(3))),

    'FEAT_ETMv4':   ArmAstBinaryOp.andListToTree([ # See 7.3.22 in IHI0064H_b_etm_v4_architecture_specification.pdf
            ArmAstIdentifier('FEAT_ETE'),
            ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('PRESENT',   'TRCDEVARCH'),]), '==', ArmAstInteger(1)),
            ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('ARCHITECT', 'TRCDEVARCH'),]), '==', ArmAstInteger(0x23b)),
            ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('ARCHVER',   'TRCDEVARCH'),]), '==', ArmAstInteger(4)),
            ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('ARCHPART',  'TRCDEVARCH'),]), '==', ArmAstInteger(0xa13)),
        ]),
    'FEAT_ETMv4p1': ArmAstBinaryOp.andListToTree([
        ArmAstIdentifier('FEAT_ETMv4'),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]), '>=', ArmAstInteger(1)),
    ]),
    'FEAT_ETMv4p2': ArmAstBinaryOp.andListToTree([
        ArmAstIdentifier('FEAT_ETMv4'),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]), '>=', ArmAstInteger(2)),
    ]),
    'FEAT_ETMv4p3': ArmAstBinaryOp.andListToTree([
        ArmAstIdentifier('FEAT_ETMv4'),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]), '>=', ArmAstInteger(3)),
    ]),
    'FEAT_ETMv4p4': ArmAstBinaryOp.andListToTree([
        ArmAstIdentifier('FEAT_ETMv4'),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]), '>=', ArmAstInteger(4)),
    ]),
    'FEAT_ETMv4p5': ArmAstBinaryOp.andListToTree([
        ArmAstIdentifier('FEAT_ETMv4'),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]), '>=', ArmAstInteger(5)),
    ]),
    'FEAT_ETMv4p6': ArmAstBinaryOp.andListToTree([
        ArmAstIdentifier('FEAT_ETMv4'),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('REVISION', 'TRCDEVARCH'),]), '>=', ArmAstInteger(6)),
    ]),
    'FEAT_VPIPT': ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('L1Ip', 'CTR_EL0'),]), '==', ArmAstInteger(2)), # removed
    #'FEAT_RASSAv1p1': ArmAstIdentifier('FEAT_RASv1p1'), # Incomplete detection. ARMv8.4 implies this, but optional back to v8.2.

    # Too complex detection logic.
    'FEAT_LPA2': ArmAstBinaryOp.orListToTree([ # Possible that OR is slightly wrong here, but it's the simpler way out...
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('TGran4',    'ID_AA64MMFR0_EL1'),]), '>=', ArmAstInteger(1)),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('TGran16',   'ID_AA64MMFR0_EL1'),]), '>=', ArmAstInteger(2)),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('TGran4_2',  'ID_AA64MMFR0_EL1'),]), '>=', ArmAstInteger(3)),
        ArmAstBinaryOp(ArmAstFunction('UInt', [ArmAstField('TGran16_2', 'ID_AA64MMFR0_EL1'),]), '>=', ArmAstInteger(3)),
    ]),
    'FEAT_SME2p2': ArmAstBinaryOp.andListToTree([ ArmAstIdentifier('FEAT_SME'), ArmAstIdentifier('FEAT_SVE2p2'), ]),
};

class ArmFeature(object):
    """
    ARM instruction set feature.
    """

    def __init__(self, oJson, sName, aoConstraints, sType = 'Parameters.Boolean'):
        self.oJson          = oJson;
        self.sName          = sName;
        self.aoConstraints  = aoConstraints;
        self.sType          = sType;

        # Find the best is-supported expression, prioritizing registers in the
        # AArch64 set over AArch32 and ext.
        dSupportExpr = {};
        for oConstraint in aoConstraints:
            oExpr = self.extractFeatureIndicator(oConstraint, sName);
            if oExpr:
                dSupportExpr[oExpr.toString()] = oExpr;

        def calcWeight(sStr):
            iRet = 10 if sStr.find('AArch64.') >= 0 else 0;
            if sStr.find('AArch32.') >= 0: iRet -= 1;
            if sStr.find('ext.') >= 0:     iRet -= 2;
            return iRet;

        def cmpExpr(sExpr1, sExpr2):
            iWeight1 = calcWeight(sExpr1);
            iWeight2 = calcWeight(sExpr2);
            if iWeight1 != iWeight2:
                return -1 if iWeight1 > iWeight2 else 1;
            if sExpr1 == sExpr2:
                return 0;
            return -1 if sExpr1 < sExpr2 else 1;

        asKeys = sorted(dSupportExpr.keys(), key = functools.cmp_to_key(cmpExpr));
        self.oSupportExpr = dSupportExpr[asKeys[0]] if asKeys else None;

        # Manual overrides and fixes.
        if sName in g_dArmFeatureSupportExprOverrides:
            self.oSupportExpr = g_dArmFeatureSupportExprOverrides[sName];
            #print('debug3: sName=%s override: %s' % (sName, self.oSupportExpr.toString()))

        # Find the variables in the expression (for optimizing explosion).
        self.asSupportExprVars = sorted(set(ArmFeature.extractVariables(self.oSupportExpr)));

    @staticmethod
    def extractFeatureIndicator(oConstraint, sFeature):
        """
        Returns the part of the constrains AST tree that (probably) detects the feature.
        Returns None if not found.

        Analyze the constraints and try figure out if there is a single system
        register that indicates this feature or not.  This is generally found
        as a FEAT_SELF <-> UInt(reg.field) >= x.  It may be nested and repeated.
        """
        if isinstance(oConstraint, ArmAstBinaryOp):
            # Is it a 'sFeature <-> sExpr'?
            if (    oConstraint.sOp == '<->'
                and isinstance(oConstraint.oLeft, ArmAstIdentifier)
                and oConstraint.oLeft.sName == sFeature):
                return oConstraint.oRight;

            # If this is an '-->' (implies) operator and the left side is our
            # feature, then the right side describing things implied by it.
            # If the operator is used with something else on the left side,
            # those are preconditions for the feature and the right hand side
            # may contain the <-> expression we're seeking.
            if (    oConstraint.sOp == '-->'
                and (   not isinstance(oConstraint.oLeft, ArmAstIdentifier)
                     or oConstraint.oLeft.sName != sFeature)):
                return ArmFeature.extractFeatureIndicator(oConstraint.oRight, sFeature);
        return None;

    @staticmethod
    def extractVariables(oConstraint):
        """ Returns a list of the identifiers as a list of strings. """
        if not oConstraint:
            return [];
        if isinstance(oConstraint, ArmAstIdentifier):
            return [ oConstraint.sName, ];
        if isinstance(oConstraint, ArmAstDotAtom):
            return [ oConstraint.toString(), ];
        if isinstance(oConstraint, ArmAstBinaryOp):
            return ArmFeature.extractVariables(oConstraint.oLeft) + ArmFeature.extractVariables(oConstraint.oRight);
        if isinstance(oConstraint, ArmAstField):
            return [oConstraint.sState + '.' + oConstraint.sName, ];

        aoRet = [];
        if isinstance(oConstraint, ArmAstFunction):
            for oArg in oConstraint.aoArgs:
                aoRet += ArmFeature.extractVariables(oArg);
        elif isinstance(oConstraint, (ArmAstSet, ArmAstConcat)):
            for oValue in oConstraint.aoValues:
                aoRet += ArmFeature.extractVariables(oValue);
        return aoRet;


    @staticmethod
    def fromJson(oJson):
        sType = oJson['_type'];
        assert sType in ('Parameters.Boolean', 'Parameters.Integer',), '_type=%s' % (oJson['_type'],);
        sName         = oJson['name'];
        aoConstraints = [ArmAstBase.fromJson(oItem, ArmAstBase.ksModeConstraints) for oItem in oJson['constraints']];
        return ArmFeature(oJson, sName, aoConstraints, sType);


#
# System Registers and their properties.
#

class ArmRange(object):
    """ Bit range in a field. """
    def __init__(self, iFirstBit, cBitsWidth):
        self.iFirstBit  = iFirstBit;
        self.cBitsWidth = cBitsWidth;

    def isEqualTo(self, oOther):
        """ Compare two ranges. """
        if isinstance(oOther, ArmRange):
            if self.iFirstBit == oOther.iFirstBit:
                if self.cBitsWidth == oOther.cBitsWidth:
                    return True;
        return False;

class ArmFieldsBase(object):
    """ Base class for all fields. """

    def __init__(self, oParent, aoRanges, sName):
        self.oParent  = oParent;
        self.aoRanges = aoRanges    # Type: List[ArmRange]
        self.sName    = sName;

    def toString(self):
        """ Approximate string representation of the field. """
        if len(self.aoRanges) == 1:
            return '%s@%u:%u' % (self.sName, self.aoRanges[0].iFirstBit, self.aoRanges[0].cBitsWidth,);
        return '%s@%s' % (self.sName, ','.join(['%u:%u' % (oRange.iFirstBit, oRange.cBitsWidth) for oRange in self.aoRanges]),);

    @staticmethod
    def addFieldListToLookupDict(daoFields, aoFields):
        """
        Adds the fields from aoFields to the lookup dictionary daoFields (returning it).

        The daoFields is indexed by field name and each entry is a list of fields
        by that name.
        """
        for oField in aoFields:
            if oField.sName:
                if oField.sName not in daoFields:
                    daoFields[oField.sName]  = [oField];
                else:
                    daoFields[oField.sName] += [oField];
            if isinstance(oField, ArmFieldsConditionalField):
                ArmFieldsBase.addFieldListToLookupDict(daoFields, [oCondField for _, oCondField in oField.atCondFields]);
        return daoFields;


    @staticmethod
    def rangesFromJson(adJson):
        """ Converts the rangeset array to a list of ranges. """
        aoRet = [];
        for dJson in adJson:
            assert dJson['_type'] == 'Range';
            aoRet.append(ArmRange(int(dJson['start']), int(dJson['width'])));
        return aoRet;


    khAttribSetReserved = frozenset(['_type', 'description', 'rangeset', 'value']);
    @staticmethod
    def fromJsonReserved(dJson, oParent):
        assertJsonAttribsInSet(dJson, ArmFieldsBase.khAttribSetReserved);
        return ArmFieldsReserved(oParent, ArmFieldsBase.rangesFromJson(dJson['rangeset']));


    khAttribSetImplementationDefined = frozenset(['_type', 'constraints', 'description', 'display', 'name',
                                                  'rangeset', 'resets', 'volatile']);
    @staticmethod
    def fromJsonImplementationDefined(dJson, oParent):
        assertJsonAttribsInSet(dJson, ArmFieldsBase.khAttribSetImplementationDefined);
        return ArmFieldsImplementationDefined(oParent, ArmFieldsBase.rangesFromJson(dJson['rangeset']), dJson['name']);


    khAttribSetField = frozenset(['_type', 'access', 'description', 'display', 'name',
                                  'rangeset', 'resets', 'values', 'volatile']);
    @staticmethod
    def fromJsonField(dJson, oParent):
        assertJsonAttribsInSet(dJson, ArmFieldsBase.khAttribSetField);
        return ArmFieldsField(oParent, ArmFieldsBase.rangesFromJson(dJson['rangeset']), dJson['name']);


    khAttribSetConditionalField = frozenset(['_type', 'description', 'display', 'fields', 'name',
                                             'rangeset', 'reservedtype', 'resets', 'volatile']);
    khAttribSetConditionalFieldEntry = frozenset(['condition', 'field']);
    @staticmethod
    def fromJsonConditionalField(dJson, oParent):
        assertJsonAttribsInSet(dJson, ArmFieldsBase.khAttribSetConditionalField);
        atCondFields = [];
        oNew = ArmFieldsConditionalField(oParent, ArmFieldsBase.rangesFromJson(dJson['rangeset']), dJson['name'], atCondFields);
        for dJsonField in dJson['fields']:
            assertJsonAttribsInSet(dJsonField, ArmFieldsBase.khAttribSetConditionalFieldEntry);
            atCondFields.append((ArmAstBase.fromJson(dJsonField['condition'], ArmAstBase.ksModeConstraints),
                                 ArmFieldsBase.fromJson(dJsonField['field'], oNew)));
        return oNew;


    khAttribSetConstantField = frozenset(['_type', 'access', 'description', 'name', 'rangeset', 'value']);
    @staticmethod
    def fromJsonConstantField(dJson, oParent):
        assertJsonAttribsInSet(dJson, ArmFieldsBase.khAttribSetConstantField);
        return ArmFieldsConstantField(oParent, ArmFieldsBase.rangesFromJson(dJson['rangeset']), dJson['name']);


    khAttribSetArray = frozenset(['_type', 'access', 'description', 'display', 'index_variable', 'indexes', 'name',
                                  'rangeset', 'resets' , 'values', 'volatile']);
    @staticmethod
    def fromJsonArray(dJson, oParent):
        assertJsonAttribsInSet(dJson, ArmFieldsBase.khAttribSetArray);
        aoIndexes = ArmFieldsArray.Index.fromJsonArray(dJson['indexes']);
        aoRanges  = ArmFieldsBase.rangesFromJson(dJson['rangeset']);
        assert len(aoIndexes) <= len(aoRanges), \
               '%s: aoIndexes=%s\naoRanges=%s' % (dJson['name'], dJson['indexes'], dJson['rangeset'],);

        cIndexSteps   = sum(oIndex.cSteps for oIndex in aoIndexes);
        cBitsRanges   = sum(oRange.cBitsWidth for oRange in aoRanges);
        cBitsPerEntry = cBitsRanges // cIndexSteps;
        assert cBitsPerEntry * cIndexSteps == cBitsRanges;

        return ArmFieldsArray(oParent, aoRanges, dJson['name'], dJson['index_variable'], aoIndexes, cIndexSteps, cBitsPerEntry);


    khAttribSetVector = frozenset(['reserved_type', 'size',]) | khAttribSetArray;
    @staticmethod
    def fromJsonVector(dJson, oParent):
        assertJsonAttribsInSet(dJson, ArmFieldsBase.khAttribSetVector);
        aoIndexes = ArmFieldsArray.Index.fromJsonArray(dJson['indexes']);
        aoRanges  = ArmFieldsBase.rangesFromJson(dJson['rangeset']);
        assert len(aoIndexes) <= len(aoRanges), \
               '%s: aoIndexes=%s\naoRanges=%s' % (dJson['name'], dJson['indexes'], dJson['rangeset'],);

        cIndexSteps   = sum(oIndex.cSteps for oIndex in aoIndexes);
        cBitsRanges   = sum(oRange.cBitsWidth for oRange in aoRanges);
        cBitsPerEntry = cBitsRanges // cIndexSteps;
        assert cBitsPerEntry * cIndexSteps == cBitsRanges;

        atCondSizes = [];
        for dJsonSize in dJson['size']:
            atCondSizes.append((ArmAstBase.fromJson(dJsonSize['condition']),
                                ArmAstBase.fromJson(dJsonSize['value'], ArmAstBase.ksModeConstraints))); ## @todo hackish mode

        return ArmFieldsVector(oParent, aoRanges, dJson['name'], dJson['index_variable'], aoIndexes, cIndexSteps,
                               cBitsPerEntry, atCondSizes);


    khAttribSetDynamic = frozenset(['_type', 'description', 'display', 'instances', 'name', 'rangeset', 'resets', 'volatile']);
    @staticmethod
    def fromJsonDynamic(dJson, oParent):
        assertJsonAttribsInSet(dJson, ArmFieldsBase.khAttribSetDynamic);
        return ArmFieldsDynamic(oParent, ArmFieldsBase.rangesFromJson(dJson['rangeset']), dJson['name'],
                                [ArmFieldset.fromJson(dJsonFieldSet) for dJsonFieldSet in dJson['instances']]);


    kfnTypeMap = {
        'Fields.Reserved':                  fromJsonReserved,
        'Fields.ImplementationDefined':     fromJsonImplementationDefined,
        'Fields.Field':                     fromJsonField,
        'Fields.ConditionalField':          fromJsonConditionalField,
        'Fields.ConstantField':             fromJsonConstantField,
        'Fields.Array':                     fromJsonArray,
        'Fields.Dynamic':                   fromJsonDynamic,
        'Fields.Vector':                    fromJsonVector,
    };

    @staticmethod
    def fromJson(dJson, oParent = None):
        """ Decodes a field. """
        return ArmFieldsBase.kfnTypeMap[dJson['_type']](dJson, oParent);


class ArmFieldsReserved(ArmFieldsBase):
    """ Fields.Reserved """
    def __init__(self, oParent, aoRanges):
        ArmFieldsBase.__init__(self, oParent, aoRanges, None);


class ArmFieldsField(ArmFieldsBase):
    """ Fields.Field """
    def __init__(self, oParent, aoRanges, sName):
        ArmFieldsBase.__init__(self, oParent, aoRanges, sName);


class ArmFieldsImplementationDefined(ArmFieldsBase):
    """ Fields.ImplementationDefined """
    def __init__(self, oParent, aoRanges, sName):
        ArmFieldsBase.__init__(self, oParent, aoRanges, sName);


class ArmFieldsConditionalField(ArmFieldsBase):
    """ Fields.ConditionalField """
    def __init__(self, oParent, aoRanges, sName, atCondFields):
        ArmFieldsBase.__init__(self, oParent, aoRanges, sName);
        self.atCondFields = atCondFields        # Type: List[Tuple(ArmAstBase,ArmFieldsBase)]


class ArmFieldsConstantField(ArmFieldsBase):
    """ Fields.ConstantField """
    def __init__(self, oParent, aoRanges, sName):
        ArmFieldsBase.__init__(self, oParent, aoRanges, sName);


class ArmFieldsArray(ArmFieldsBase):
    """ Fields.Array """

    class Index(object):
        """ Helper for ArmFieldsArray. """
        def __init__(self, idxStart, cSteps):
            self.idxStart = idxStart;
            self.idxEnd   = idxStart + cSteps;
            self.cSteps   = cSteps;

        @staticmethod
        def fromJson(dJson):
            assert dJson['_type'] == 'Range';
            return ArmFieldsArray.Index(int(dJson['start']), int(dJson['width']));

        @staticmethod
        def fromJsonArray(adJson):
            return [ArmFieldsArray.Index.fromJson(dJsonEntry) for dJsonEntry in adJson];

    def __init__(self, oParent, aoRanges, sName, sIdxVarNm, aoIndexes, cEntries, cBitsPerEntry):
        ArmFieldsBase.__init__(self, oParent, aoRanges, sName);
        self.sIdxVarNm     = sIdxVarNm;
        self.aoIndexes     = aoIndexes      # Type: List[ArmFieldsArray.Index]
        self.cEntries      = cEntries;
        self.cBitsPerEntry = cBitsPerEntry;


class ArmFieldsVector(ArmFieldsArray):
    """ Fields.Vector """

    def __init__(self, oParent, aoRanges, sName, sIdxVarNm, aoIndexes, cEntries, cBitsPerEntry, atCondSizes):
        ArmFieldsArray.__init__(self, oParent, aoRanges, sName, sIdxVarNm, aoIndexes, cEntries, cBitsPerEntry);
        self.atCondSizes   = atCondSizes    # Type: List[Tuple[ArmAstBase,ArmAstBase]] # tuple(condition, size-expr)


class ArmFieldsDynamic(ArmFieldsBase):
    """ Fields.Dynamic """
    def __init__(self, oParent, aoRanges, sName, aoFieldsets):
        ArmFieldsBase.__init__(self, oParent, aoRanges, sName);
        self.aoFieldsets = aoFieldsets      # Type: List[ArmFieldset]



class ArmFieldset(object):
    """
    A register field set.
    """

    def __init__(self, dJson, cBitsWidth, aoFields, sName = None):
        self.dJson      = dJson;
        self.cBitsWidth = cBitsWidth;
        self.aoFields   = aoFields          # type: List[ArmFieldsBase]
        self.sName      = sName;

    def toString(self):
        """ Gets display string. """
        return '%u bits%s: %s' \
             % (self.cBitsWidth, ', %s' % (self.sName,) if self.sName else '',
                ', '.join([oField.toString() for oField in sorted(self.aoFields, key = lambda o: o.aoRanges[0].iFirstBit)]),);

    @staticmethod
    def fromJson(dJson):
        assert dJson['_type'] == 'Fieldset', '_type=%s' % (dJson['_type'],);
        oNew = ArmFieldset(dJson, int(dJson['width']), [], dJson['name']);
        oNew.aoFields = [ArmFieldsBase.fromJson(dJsonField, oNew) for dJsonField in dJson['values']];
        return oNew;


class ArmRegEncoding(object):
    """ Register encoding. """

    kdSortOrder = {
        'op0': 1,
        'op1': 2,
        'CRn': 3,
        'CRm': 4,
        'op2': 5,
    };

    def __init__(self, sAsmValue, dNamedValues):
        self.sAsmValue    = sAsmValue;
        self.dNamedValues = collections.OrderedDict();
        self.fHasWildcard = False;
        self.fHasIndex    = False;
        for sKey in sorted(dNamedValues, key = lambda s: ArmRegEncoding.kdSortOrder.get(s, 9)):
            oValue = dNamedValues[sKey];
            self.dNamedValues[sKey] = oValue;
            self.fHasWildcard |= 'x' in oValue.sValue;
            self.fHasIndex    |= '[' in oValue.sValue;

    def toString(self):
        return '%s={%s}' \
             % (self.sAsmValue, ', '.join(['%s=%s' % (sKey, oValue.toString()) for sKey, oValue in self.dNamedValues.items()]),);

    def __str__(self):
        return self.toString();

    def __repr__(self):
        return self.toString();

    def getSysRegIdCreate(self):
        """ Returns the corresponding ARMV8_AARCH64_SYSREG_ID_CREATE invocation. """
        assert len(self.dNamedValues) == len(ArmRegEncoding.kdSortOrder), '%s: %s' % (self.sAsmValue, self.dNamedValues,);
        asArgs = [];
        for sKey in ArmRegEncoding.kdSortOrder:
            (fValue, _, fWildcard, _) = ArmAstValue.parseValue(self.dNamedValues[sKey].sValue, 0);
            if fWildcard == 0:
                asArgs.append('%s' % (fValue,));
            else:
                oXcpt = Exception('wildcard encoding for %s: %s:%s' % (self.sAsmValue, sKey, self.dNamedValues[sKey],));
                print('wtf: %s' % (oXcpt,))
                raise oXcpt;
        return 'ARMV8_AARCH64_SYSREG_ID_CREATE(' + ','.join(asArgs) + ')';

    def compareCStyle(self, iOp0, iOp1, iCRn, iCRm, iOp2):
        """
        Do C-style compare.  Arguments can be integers for exact matches,
        ranges for spans, and None for anything.

        Returns 0 if match.
        Postive return value if the five values given are higher than this one.
        Negative return value if the five values given are smaller than this one.
        """
        for sMine, oOther in ( ('op0', iOp0), ('op1', iOp1), ('CRn', iCRn), ('CRm', iCRm), ('op2', iOp2),):
            if oOther is not None:
                oMyValue = self.dNamedValues.get(sMine);
                if oMyValue:
                    (iMyValue, _, fWildcard, _) = oMyValue.getValueDetails();
                    if fWildcard == 0:
                        if isinstance(oOther, range):
                            if iMyValue not in oOther:
                                iOtherFirst = next(iter(oOther));
                                assert iOtherFirst != iMyValue;
                                return iOtherFirst - iMyValue;
                        elif oOther != iMyValue:
                            return oOther - iMyValue;
                    else:
                        (iMyFirstValue, _, _, _) = ArmAstValue.parseValue(oMyValue.sValue.replace('x', '0'), 0);
                        (iMyLastValue,  _, _, _) = ArmAstValue.parseValue(oMyValue.sValue.replace('x', '1'), 0);
                        if isinstance(oOther, range):
                            if not set(oOther) & set(range(iMyFirstValue, iMyLastValue + 1)):
                                return next(iter(oOther)) - iMyFirstValue;
                        elif oOther < iMyFirstValue:
                            return oOther - iMyFirstValue;
                        elif oOther > iMyLastValue:
                            return oOther - iMyLastValue;
                elif isinstance(oOther, range):
                    iOtherFirst = next(iter(oOther));
                    return -1 if iOtherFirst < 0 else 1;
                else:
                    return -1 if oOther < 0 else 1;
        return 0;

    khAttribSet = frozenset(['_type', 'asmvalue', 'encodings']);
    @staticmethod
    def fromJson(dJson):
        """ Decodes a register encoding object. """
        assert dJson['_type'] == 'Encoding';
        assertJsonAttribsInSet(dJson, ArmRegEncoding.khAttribSet);
        dNamedValues = collections.OrderedDict();
        for sName, dValue in dJson['encodings'].items():
            dNamedValues[sName] = ArmAstBase.fromJson(dValue, ArmAstBase.ksModeValuesOnly);
        return ArmRegEncoding(dJson['asmvalue'], dNamedValues);


## @todo remove this.
class ArmAccessorPermissionBase(object):
    """
    Register accessor permission base class (Accessors.Permission.*).
    """

    def __init__(self, oCondition):
        self.oCondition = oCondition;


    khAttribSetMemory = frozenset(['_type', 'access', 'condition',]);
    @staticmethod
    def fromJsonMemory(dJson, fNested):
        _ = fNested;
        assert dJson['_type'] == 'Accessors.Permission.MemoryAccess';
        assertJsonAttribsInSet(dJson, ArmAccessorPermissionBase.khAttribSetMemory);
        oCondition = ArmAstBase.fromJson(dJson['condition'], ArmAstBase.ksModeConstraints)
        # The 'access' attribute comes in three variations: Accessors.Permission.MemoryAccess list,
        # Accessors.Permission.AccessTypes.Memory.ReadWriteAccess and
        # Accessors.Permission.AccessTypes.Memory.ImplementationDefined.
        oJsonAccess = dJson['access'];
        if isinstance(oJsonAccess, list):
            aoAccesses = [ArmAccessorPermissionBase.fromJsonMemory(dJsonSub, True) for dJsonSub in oJsonAccess]
            return ArmAccessorPermissionMemoryAccessList(oCondition, aoAccesses);

        if oJsonAccess['_type'] == 'Accessors.Permission.AccessTypes.Memory.ReadWriteAccess':
            return ArmAccessorPermissionMemoryAccess(oCondition, ArmAccessorPermissionMemReadWriteAccess.fromJson(oJsonAccess));

        if oJsonAccess['_type'] == 'Accessors.Permission.AccessTypes.Memory.ImplementationDefined':
            aoConstraints = [ArmAccessorPermissionMemReadWriteAccess.fromJson(dJsonConstr)
                            for dJsonConstr in oJsonAccess['constraints']];
            return ArmAccessorPermissionMemoryAccessImplDef(oCondition, aoConstraints);
        raise Exception('Unexpected access attr type: %s' % (oJsonAccess['_type'],));


    kfnTypeMap = {
        'Accessors.Permission.MemoryAccess': fromJsonMemory,
    };

    @staticmethod
    def fromJson(dJson, fNested = False):
        """ Decodes a register accessor object. """
        return ArmAccessorPermissionBase.kfnTypeMap[dJson['_type']](dJson, fNested);


class ArmAccessorPermissionMemReadWriteAccess(object):
    """ Accessors.Permission.AccessTypes.Memory.ReadWriteAccess """
    def __init__(self, sRead, sWrite):
        self.sRead  = sRead;
        self.sWrite = sWrite;

    khAttribSet = frozenset(['_type', 'read', 'write',]);
    @staticmethod
    def fromJson(dJson):
        assert dJson['_type'] == 'Accessors.Permission.AccessTypes.Memory.ReadWriteAccess';
        assertJsonAttribsInSet(dJson, ArmAccessorPermissionMemReadWriteAccess.khAttribSet);
        return ArmAccessorPermissionMemReadWriteAccess(dJson['read'], dJson['write']);


class ArmAccessorPermissionMemoryAccess(ArmAccessorPermissionBase):
    """ Accessors.Permission.MemoryAccess """
    def __init__(self, oCondition, oReadWriteAccess):
        ArmAccessorPermissionBase.__init__(self, oCondition);
        self.oReadWrite = oReadWriteAccess;


class ArmAccessorPermissionMemoryAccessImplDef(ArmAccessorPermissionBase):
    """ Accessors.Permission.MemoryAccess """
    def __init__(self, oCondition, aoConstraints):
        ArmAccessorPermissionBase.__init__(self, oCondition);
        self.aoConstraints = aoConstraints;


class ArmAccessorPermissionMemoryAccessList(ArmAccessorPermissionBase):
    """ Accessors.Permission.MemoryAccess """
    def __init__(self, oCondition, aoAccesses):
        ArmAccessorPermissionBase.__init__(self, oCondition);
        self.aoAccesses = aoAccesses;


class ArmAccessorBase(object):
    """
    Register accessor base class.
    """
    def __init__(self, dJson, oCondition):
        self.dJson = dJson;
        self.oCondition = oCondition;

    khAttribSetBlockAccess = frozenset(['_type', 'access', 'condition']);
    @staticmethod
    def fromJsonBlockAccess(dJson):
        assertJsonAttribsInSet(dJson, ArmAccessorBase.khAttribSetBlockAccess);
        return ArmAccessorBlockAccess(dJson, ArmAstBase.fromJson(dJson['condition'], ArmAstBase.ksModeConstraints));


    khAttribSetBlockAccessArray = frozenset(['_type', 'access', 'condition', 'index_variables', 'indexes',
                                             'offset', 'references']);
    @staticmethod
    def fromJsonBlockAccessArray(dJson):
        assertJsonAttribsInSet(dJson, ArmAccessorBase.khAttribSetBlockAccessArray);
        return ArmAccessorBlockAccessArray(dJson, ArmAstBase.fromJson(dJson['condition'], ArmAstBase.ksModeConstraints));


    khAttribSetExternalDebug = frozenset(['_type', 'access', 'component', 'condition', 'instance', 'offset',
                                          'power_domain', 'range']);
    @staticmethod
    def fromJsonExternalDebug(dJson):
        assertJsonAttribsInSet(dJson, ArmAccessorBase.khAttribSetExternalDebug);
        return ArmAccessorExternalDebug(dJson, ArmAstBase.fromJson(dJson['condition'], ArmAstBase.ksModeConstraints));


    khAttribSetMemoryMapped = frozenset(['_type', 'access', 'component', 'condition', 'frame', 'instance', 'offset',
                                         'power_domain', 'range']);
    @staticmethod
    def fromJsonMemoryMapped(dJson):
        assertJsonAttribsInSet(dJson, ArmAccessorBase.khAttribSetMemoryMapped);
        return ArmAccessorExternalDebug(dJson, ArmAstBase.fromJson(dJson['condition'], ArmAstBase.ksModeConstraints));


    khAttribSetSystem = frozenset(['_type', 'access', 'condition', 'encoding', 'name']);
    @staticmethod
    def fromJsonSystem(dJson):
        assertJsonAttribsInSet(dJson, ArmAccessorBase.khAttribSetSystem);
        assert len(dJson['encoding']) == 1;
        sName     = dJson['name']; # For exception listing
        oEncoding = ArmRegEncoding.fromJson(dJson['encoding'][0]);
        oAccess   = ArmAstIfList.fromJson(dJson['access']) if dJson['access'] else None;
        return ArmAccessorSystem(dJson, ArmAstBase.fromJson(dJson['condition'], ArmAstBase.ksModeConstraints),
                                 sName, oEncoding, oAccess);


    khAttribSetSystemArray = frozenset(['_type', 'access', 'condition', 'encoding', 'index_variable', 'indexes', 'name']);
    @staticmethod
    def fromJsonSystemArray(dJson):
        assertJsonAttribsInSet(dJson, ArmAccessorBase.khAttribSetSystemArray);
        assert len(dJson['encoding']) == 1;
        oEncoding = ArmRegEncoding.fromJson(dJson['encoding'][0]);
        oAccess   = ArmAstIfList.fromJson(dJson['access']) if dJson['access'] else None;
        return ArmAccessorSystemArray(dJson, ArmAstBase.fromJson(dJson['condition'], ArmAstBase.ksModeConstraints),
                                      dJson['name'], oEncoding, oAccess);


    kfnTypeMap = {
        'Accessors.BlockAccess':            fromJsonBlockAccess,
        'Accessors.BlockAccessArray':       fromJsonBlockAccessArray,
        'Accessors.ExternalDebug':          fromJsonExternalDebug,
        'Accessors.MemoryMapped':           fromJsonMemoryMapped,
        'Accessors.SystemAccessor':         fromJsonSystem,
        'Accessors.SystemAccessorArray':    fromJsonSystemArray,
    };

    @staticmethod
    def fromJson(dJson):
        """ Decodes a register accessor object. """
        return ArmAccessorBase.kfnTypeMap[dJson['_type']](dJson);


class ArmAccessorBlockAccess(ArmAccessorBase):
    """ Accessors.BlockAccess """
    def __init__(self, dJson, oCondition):
        ArmAccessorBase.__init__(self, dJson, oCondition);


class ArmAccessorBlockAccessArray(ArmAccessorBase):
    """ Accessors.BlockAccessArray """
    def __init__(self, dJson, oCondition):
        ArmAccessorBase.__init__(self, dJson, oCondition);


class ArmAccessorExternalDebug(ArmAccessorBase):
    """ Accessors.ExternalDebug """
    def __init__(self, dJson, oCondition):
        ArmAccessorBase.__init__(self, dJson, oCondition);


class ArmAccessorMemoryMapped(ArmAccessorBase):
    """ Accessors.MemoryMapped """
    def __init__(self, dJson, oCondition):
        ArmAccessorBase.__init__(self, dJson, oCondition);


class ArmAccessorSystem(ArmAccessorBase):
    """ Accessors.SystemAccessor """
    def __init__(self, dJson, oCondition, sName, oEncoding, oAccess):
        ArmAccessorBase.__init__(self, dJson, oCondition);
        self.sName     = sName;
        self.oEncoding = oEncoding  # Type: ArmRegEncoding
        self.oAccess   = oAccess    # Type: ArmAstIfList # Can be None!


class ArmAccessorSystemArray(ArmAccessorSystem):
    """ Accessors.SystemAccessorArray """
    def __init__(self, dJson, oCondition, sName, oEncoding, oAccess):
        ArmAccessorSystem.__init__(self, dJson, oCondition, sName, oEncoding, oAccess);



class ArmRegister(object):
    """
    ARM system register.
    """

    def __init__(self, oJson, sName, sState, aoFieldsets, fRegArray, oCondition, aoAccessors):
        self.oJson          = oJson;
        self.sName          = sName;
        self.sState         = sState;
        self.aoFieldsets    = aoFieldsets       # Type: List[ArmFieldset]
        self.fRegArray      = fRegArray;
        self.oCondition     = oCondition        # Type: ArmAstBase
        self.aoAccessors    = aoAccessors       # Type: List[ArmAccessorBase]

        self.daoFields      = {}                # Type: Dict[str,List[ArmFieldsBase]]
        for oFieldset in aoFieldsets:
            ArmFieldsBase.addFieldListToLookupDict(self.daoFields, oFieldset.aoFields);

    def getVBoxConstant(self):
        """
        Returns the VBox constant for the register.

        For instance: ARMV8_AARCH64_SYSREG_ID_AA64ZFR0_EL1
        """
        return 'ARMV8_' + self.sState.upper() + '_SYSREG_' + self.sName.upper();

    @staticmethod
    def fromJson(dJson, sStatePrefix = ''):
        sType = dJson['_type'];
        assert sType in ('Register', 'RegisterArray'), '_type=%s' % (sType,);

        sName       = dJson['name'];
        sState      = dJson['state'];
        aoFieldsets = [ArmFieldset.fromJson(dJsonSet) for dJsonSet in dJson['fieldsets']];
        oCondition  = ArmAstBase.fromJson(dJson['condition'], ArmAstBase.ksModeConstraints); ## @todo hackish mode
        aoAccessors = [ArmAccessorBase.fromJson(dJsonAcc) for dJsonAcc in dJson['accessors']];

        return ArmRegister(dJson, sName, sStatePrefix + sState, aoFieldsets, sType == 'RegisterArray', oCondition, aoAccessors);



#
# AArch64 Specification Loader.
#

## @name System Registers
## @{

## The '_meta::version' dictionary from the first entry in the Registers.json file.
## @note Each register has a _meta::version attribute, but we ASSUME these are the
##       same within the file.
g_oArmRegistersVerInfo = None;

## All the system registers.
g_aoAllArmRegisters = []                                        # type: List[ArmRegister]

## All the system registers by 'state'.
g_daoAllArmRegistersByState = {}                                # type: Dict[str, List[ArmRegister]]

## All the system registers by 'state' and then name.
g_ddoAllArmRegistersByStateByName = {}                          # type: Dict[str, Dict[str, ArmRegister]]

## @}

## @name Features
## @{

## The '_meta::version' dictionary from the Features.json file.
g_oArmFeaturesVerInfo = None;

## All the features.
g_aoAllArmFeatures = []                                         # type: List[ArmFeature]

## All the features by name.
g_dAllArmFeaturesByName = {}                                    # type: Dict[str, ArmFeature]
## @}


## @name Instructions
## @{

## The '_meta::version' dictionary from the Instructions.json file.
g_oArmInstructionVerInfo = None;

## All the instructions.
g_aoAllArmInstructions = []                                     # type: List[ArmInstruction]

## All the instructions by name (not mnemonic).
g_dAllArmInstructionsByName = {}                                # type: Dict[str, ArmInstruction]

## All the instructions by instruction set name.
g_dAllArmInstructionsBySet = collections.defaultdict(list)      # type: Dict[str, List[ArmInstruction]]

## All the instructions by (immediate) instruction group name.
g_dAllArmInstructionsByGroup = collections.defaultdict(list)    # type: Dict[str, List[ArmInstruction]]

## The instruction sets.
g_aoArmInstructionSets = []                                     # type: List[ArmInstructionSet]

## The instruction sets by name.
g_dArmInstructionSets = {}                                      # type: Dict[str, ArmInstructionSet]

## The instruction groups.
g_aoArmInstructionGroups = []                                   # type: List[ArmInstructionGroup]

## The instruction groups.
g_dArmInstructionGroups = {}                                    # type: Dict[str, ArmInstructionGroup]

## @}

## Instruction corrections expressed as a list required conditionals.
#
# In addition to supplying missing conditionals (IsFeatureImplemented(FEAT_XXX)
# and missing fixed encoding values (field == <fixed-value>).
#
# The reason why this is a list and not an expression, is that it easier to skip
# stuff that's already present in newer specification and avoiding duplicate tests.
g_dArmEncodingCorrectionConditions = {
    # The sdot and udot vector instructions are missing the 'size' restrictions in the 2024-12 specs.
    'sdot_z_zzz_': (ArmAstBinaryOp(ArmAstIdentifier('size'), '!=', ArmAstValue("'00'")),
                    ArmAstBinaryOp(ArmAstIdentifier('size'), '!=', ArmAstValue("'01'")),),
    'udot_z_zzz_': (ArmAstBinaryOp(ArmAstIdentifier('size'), '!=', ArmAstValue("'00'")),
                    ArmAstBinaryOp(ArmAstIdentifier('size'), '!=', ArmAstValue("'01'")),),
    # These instructions are FEAT_MTE && FEAT_MOPS. The 2024-12 specs missed the former condition.
    'SETGEN_SET_memcms':  (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGETN_SET_memcms': (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGET_SET_memcms':  (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGE_SET_memcms':   (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGMN_SET_memcms':  (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGMTN_SET_memcms': (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGMT_SET_memcms':  (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGM_SET_memcms':   (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGPN_SET_memcms':  (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGPTN_SET_memcms': (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGPT_SET_memcms':  (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),
    'SETGP_SET_memcms':   (ArmAstFunction('IsFeatureImplemented', [ArmAstIdentifier('FEAT_MTE'),]),),

    ## @todo fexpa_z_z: s/FEAT_SME2p2/FEAT_SSVE_FEXPA/ (2024-12 vs 2025-03); Not relevant since we don't support either.
};



def __asmChoicesFilterOutDefaultAndAbsent(adChoices, ddAsmRules):
    """
    Helper that __asmRuleIdToDisplayText uses to filter out any default choice
    that shouldn't be displayed.

    Returns choice list.
    """
    # There are sometime a 'none' tail entry.
    if adChoices[-1] is None:
        adChoices = adChoices[:-1];
    if len(adChoices) > 1:
        # Typically, one of the choices is 'absent' or 'default', eliminate it before we start...
        for iChoice, dChoice in enumerate(adChoices):
            fAllAbsentOrDefault = True;
            for dSymbol in dChoice['symbols']:
                if dSymbol['_type'] != 'Instruction.Symbols.RuleReference':
                    fAllAbsentOrDefault = False;
                    break;
                sRuleId = dSymbol['rule_id'];
                oRule = ddAsmRules[sRuleId];
                if (   ('display' in oRule and oRule['display'])
                    or ('symbols' in oRule and oRule['symbols'])):
                    fAllAbsentOrDefault = False;
                    break;
            if fAllAbsentOrDefault:
                return adChoices[:iChoice] + adChoices[iChoice + 1:];
    return adChoices;


def __asmRuleIdToDisplayText(sRuleId, ddAsmRules, sInstrNm):
    """
    Helper that asmSymbolsToDisplayText uses to process assembly rule references.
    """
    dRule = ddAsmRules[sRuleId];
    sRuleType = dRule['_type'];
    if sRuleType == 'Instruction.Rules.Token':
        assert dRule['default'], '%s: %s' % (sInstrNm, sRuleId);
        return dRule['default'];
    if sRuleType == 'Instruction.Rules.Rule':
        assert dRule['display'], '%s: %s' % (sInstrNm, sRuleId);
        return dRule['display'];
    if sRuleType == 'Instruction.Rules.Choice':
        # Some of these has display = None and we need to sort it out ourselves.
        if dRule['display']:
            return dRule['display'];
        sText = '{';
        assert len(dRule['choices']) > 1;
        for iChoice, dChoice in enumerate(__asmChoicesFilterOutDefaultAndAbsent(dRule['choices'], ddAsmRules)):
            if iChoice > 0:
                sText += ' | ';
            sText += asmSymbolsToDisplayText(dChoice['symbols'], ddAsmRules, sInstrNm);
        sText += '}';

        # Cache it.
        dRule['display'] = sText;
        return sText;

    raise Exception('%s: Unknown assembly rule type: %s for %s' % (sInstrNm, sRuleType, sRuleId));


def asmSymbolsToDisplayText(adSymbols, ddAsmRules, sInstrNm):
    """
    Translates the 'symbols' array of an instruction's 'assembly' property into
     a kind of assembly syntax outline.
    """
    sText = '';
    for dSym in adSymbols:
        sType = dSym['_type'];
        if sType == 'Instruction.Symbols.Literal':
            sText += dSym['value'];
        elif sType == 'Instruction.Symbols.RuleReference':
            sRuleId = dSym['rule_id'];
            sText += __asmRuleIdToDisplayText(sRuleId, ddAsmRules, sInstrNm);
        else:
            raise Exception('%s: Unknown assembly symbol type: %s' % (sInstrNm, sType,));
    return sText;


def parseInstructions(oInstrSet, oParent, aoJson, ddAsmRules):
    for oJson in aoJson:
        sType = oJson['_type'];
        if sType == 'Instruction.InstructionSet':
            if oParent: raise Exception("InstructionSet shouldn't have a parent!");
            assert not oInstrSet;
            oInstrSet = ArmInstructionSet.fromJson(oJson);
            assert oInstrSet.sName not in g_dArmInstructionSets;
            g_dArmInstructionSets[oInstrSet.sName] = oInstrSet;
            g_aoArmInstructionSets.append(oInstrSet);

            parseInstructions(oInstrSet, oInstrSet, oJson['children'], ddAsmRules);

        elif sType == 'Instruction.InstructionGroup':
            if not oParent: raise Exception("InstructionGroup should have a parent!");
            oInstrGroup = ArmInstructionGroup.fromJson(oJson, oParent);
            #if oInstrGroup.sName in g_dArmInstructionGroups: # happens with

            if oInstrGroup.sName in g_dArmInstructionGroups:
                if oInstrGroup.sName == oParent.sName: # sve_intx_clamp, sve_intx_dot2
                    oInstrGroup.sName += '_lvl2'
                else:
                    assert oInstrGroup.sName not in g_dArmInstructionGroups, '%s' % (oInstrGroup.sName,);

            g_dArmInstructionGroups[oInstrGroup.sName] = oInstrGroup;
            g_aoArmInstructionGroups.append(oInstrGroup);
            oParent.aoGroups.append(oInstrGroup);

            parseInstructions(oInstrSet, oInstrGroup, oJson['children'], ddAsmRules);

        elif sType == "Instruction.Instruction":
            if not oParent: raise Exception("Instruction should have a parent!");

            #
            # Start by getting the instruction attributes.
            #
            sInstrNm = oJson['name'];

            oCondition = ArmAstBase.fromJson(oJson['condition']);
            aoCorrectionConditions = g_dArmEncodingCorrectionConditions.get(sInstrNm)
            if aoCorrectionConditions:
                oCondition = addAndConditionsFromList(oCondition, aoCorrectionConditions);

            (aoFields, fFields) = ArmEncodesetField.encodesetFromJson(oJson['encoding']);
            for oUp in oParent.getUpIterator():
                if oUp.fFields & ~fFields:
                    (aoFields, fFields) = ArmEncodesetField.encodesetAddParentFields(aoFields, fFields, oUp.aoFields);
                if not oUp.oCondition.isBoolAndTrue():
                    oCondition = ArmAstBinaryOp(oCondition, '&&', oUp.oCondition.clone());
            if fFields != (1 << oInstrSet.cBitsWidth) - 1:
                raise Exception('Instruction %s has an incomplete encodingset: fFields=%#010x (missing %#010x)'
                                % (sInstrNm, fFields, fFields ^ ((1 << oInstrSet.cBitsWidth) - 1),))

            #sCondBefore = oCondition.toString();
            #print('debug transfer: %s: org:  %s' % (sInstrNm, sCondBefore));
            (oCondition, fMod) = transferConditionsToEncoding(oCondition, aoFields, collections.defaultdict(list), sInstrNm);
            #if fMod:
            #    print('debug transfer: %s: %s' % (sInstrNm, sCondBefore,));
            #    print('              %*s %s' % (len(sInstrNm) + 3, '--->', oCondition.toString(),));
            _ = fMod;

            # Come up with the assembly syntax (sAsmDisplay).
            if 'assembly' in oJson:
                oAsm = oJson['assembly'];
                assert oAsm['_type'] == 'Instruction.Assembly';
                assert 'symbols' in oAsm;
                sAsmDisplay = asmSymbolsToDisplayText(oAsm['symbols'], ddAsmRules, sInstrNm);
            else:
                sAsmDisplay = sInstrNm;

            # We derive the mnemonic from the assembly display string.
            sMnemonic = sAsmDisplay.split()[0];

            #
            # Instantiate it.
            #
            oInstr = ArmInstruction(oJson, sInstrNm, sMnemonic, sAsmDisplay, aoFields, fFields, oCondition, oParent);

            #
            # Add the instruction to the various lists and dictionaries.
            #
            g_aoAllArmInstructions.append(oInstr);
            assert oInstr.sName not in g_dAllArmInstructionsByName;
            g_dAllArmInstructionsByName[oInstr.sName] = oInstr;

            oParent.addImmediateInstruction(oInstr);

        else:
            raise Exception('Unexpected instruction object type: %s' % (sType,));

    return True;


def addAndConditionsFromList(oTree, aoAndConditions):
    """
    Adds the conditions in aoAndConditions that are not already present in
    oTree in an required (AND) form.

    This is used when we add corrections, so that we avoid duplicate feature
    checks and such.
    """
    if oTree.isBoolAndTrue():
        return andConditionListToTree(aoAndConditions);

    def isAndConditionPresent(oTree, oAndCondition):
        if oAndCondition.isSame(oTree):
            return True;
        if isinstance(oTree, ArmAstBinaryOp) and oTree.sOp == '&&':
            return isAndConditionPresent(oTree.oLeft, oAndCondition) or isAndConditionPresent(oTree.oRight, oAndCondition);
        return False;

    aoToAdd = [oTree,];
    for oAndCondition in aoAndConditions:
        if not isAndConditionPresent(oTree, oAndCondition):
            aoToAdd.append(oAndCondition);

    return andConditionListToTree(aoToAdd);


def andConditionListToTree(aoAndConditions):
    """ Creates AST tree of AND binary checks from aoAndConditions. """
    if len(aoAndConditions) <= 1:
        return aoAndConditions[0].clone();
    return ArmAstBinaryOp(aoAndConditions[0].clone(), '&&', andConditionListToTree(aoAndConditions[1:]));


def transferConditionsToEncoding(oCondition, aoFields, dPendingNotEq, sInstrNm, uDepth = 0, fMod = False):
    """
    This is for dealing with stuff like asr_z_p_zi_ and lsr_z_p_zi_ which has
    the same fixed encoding fields in the specs, but differs in the value of
    the named field 'U' as expressed in the conditions.

    This function will recursively take 'Field == value/integer' expression out
    of the condition tree and add them to the encodeset conditions when possible.

    The dPendingNotEq stuff is a hack to deal with stuff like this:
        sdot_z_zzz_:     U == '0' && size != '01' && size != '00'
                     && (IsFeatureImplemented(FEAT_SVE) || IsFeatureImplemented(FEAT_SME))
    The checks can be morphed into the 'size' field encoding criteria as '0b0x'.
    """
    if isinstance(oCondition, ArmAstBinaryOp):
        if oCondition.sOp == '&&':
            # Recurse into each side of an AND expression.
            #print('debug transfer: %s: recursion...' % (sInstrNm,));
            (oCondition.oLeft, fMod)  = transferConditionsToEncoding(oCondition.oLeft,  aoFields, dPendingNotEq,
                                                                     sInstrNm, uDepth + 1, fMod);
            (oCondition.oRight, fMod) = transferConditionsToEncoding(oCondition.oRight, aoFields, dPendingNotEq,
                                                                     sInstrNm, uDepth + 1, fMod);
            if oCondition.oLeft.isBoolAndTrue():
                return (oCondition.oRight, fMod);
            if oCondition.oRight.isBoolAndTrue():
                return (oCondition.oLeft, fMod);

        elif oCondition.sOp in ('==', '!='):
            # The pattern we're looking for is identifier (field) == fixed value.
            #print('debug transfer: %s: binaryop %s vs %s ...' % (sInstrNm, oCondition.oLeft.sType, oCondition.oRight.sType));
            if (    isinstance(oCondition.oLeft, ArmAstIdentifier)
                and isinstance(oCondition.oRight, (ArmAstValue, ArmAstInteger))):
                sFieldName = oCondition.oLeft.sName;
                oValue     = oCondition.oRight;
                #print('debug transfer: %s: binaryop step 2...' % (sInstrNm,));
                for oField in aoFields: # ArmEncodesetField
                    if oField.sName and oField.sName == sFieldName:
                        # ArmAstInteger - not used by spec, only corrections:
                        if isinstance(oValue, ArmAstInteger):
                            if oField.fFixed != 0:
                                raise Exception('%s: Condition checks fixed field value: %s (%#x/%#x) %s %s'
                                                % (sInstrNm, oField.sName, oField.fValue, oField.fFixed,
                                                   oCondition.sOp, oValue.iValue,));
                            assert oField.fValue == 0;
                            if oValue.iValue.bit_length() > oField.cBitsWidth:
                                raise Exception('%s: Condition field value check too wide: %s is %u bits, test value %s (%u bits)'
                                                % (sInstrNm, oField.sName, oField.cBitsWidth, oValue.iValue,
                                                   oValue.iValue.bit_count(),));
                            if oValue.iValue < 0:
                                raise Exception('%s: Condition field checks against negative value: %s, test value is %s'
                                                % (sInstrNm, oField.sName, oValue.iValue));
                            fFixed = (1 << oField.cBitsWidth) - 1;
                            if oCondition.sOp == '!=' and oField.cBitsWidth > 1:
                                dPendingNotEq[oField.sName] += [(oField, oValue.iValue, fFixed, oCondition)];
                                break;

                            print('debug transfer: %s: integer binaryop -> encoding: %s %s %#x/%#x'
                                  % (sInstrNm, oField.sName, oCondition.sOp, oValue.iValue, fFixed));
                            if oCondition.sOp == '==':
                                oField.fValue = oValue.iValue;
                            else:
                                oField.fValue = ~oValue.iValue & fFixed;
                            oField.fFixed = fFixed;
                            return (ArmAstBool(True), True);

                        # ArmAstValue.
                        assert isinstance(oValue, ArmAstValue);
                        (fValue, fFixed, _, _) = ArmAstValue.parseValue(oValue.sValue, oField.cBitsWidth);

                        if oCondition.sOp == '!=' and oField.cBitsWidth > 1 and (fFixed & (fFixed - 1)) != 0:
                            dPendingNotEq[oField.sName] += [(oField, fValue, fFixed, oCondition)];
                            break;
                        if fFixed & oField.fFixed:
                            raise Exception('%s: Condition checks fixed field value: %s (%#x/%#x) %s %s (%#x/%#x)'
                                            % (sInstrNm, oField.sName, oField.fValue, oField.fFixed, oCondition.sOp,
                                               oValue.sValue, fValue, fFixed));
                        #print('debug transfer: %s: value binaryop -> encoding: %s %s %#x (fFixed=%#x)'
                        #      % (sInstrNm, oField.sName, oCondition.sOp, fValue, fFixed,));
                        if oCondition.sOp == '==':
                            oField.fValue |= fValue;
                        else:
                            oField.fValue |= ~fValue & fFixed;
                        oField.fFixed |= fFixed;
                        return (ArmAstBool(True), True);

    #
    # Deal with pending '!=' optimizations for fields larger than a single bit.
    # Currently we only deal with two bit fields.
    #
    if uDepth == 0 and dPendingNotEq:
        def recursiveRemove(oCondition, aoToRemove):
            if isinstance(oCondition, ArmAstBinaryOp):
                if oCondition.sOp == '&&':
                    oCondition.oLeft  = recursiveRemove(oCondition.oLeft, aoToRemove);
                    oCondition.oRight = recursiveRemove(oCondition.oRight, aoToRemove);
                    if oCondition.oLeft.isBoolAndTrue():    return oCondition.oRight;
                    if oCondition.oRight.isBoolAndTrue():   return oCondition.oLeft;
                elif oCondition in aoToRemove:
                    assert isinstance(oCondition.oLeft, ArmAstIdentifier);
                    assert isinstance(oCondition.oRight, (ArmAstValue, ArmAstInteger));
                    assert oCondition.sOp == '!=';
                    return ArmAstBool(True);
            return oCondition;

        for sFieldNm, atOccurences in dPendingNotEq.items():
            # For a two bit field, we need at least two occurences to get any kind of fixed value.
            oField = atOccurences[0][0];
            if oField.cBitsWidth == 2 and len(atOccurences) >= 2:
                dValues = {};
                dFixed  = {};
                for oCurField, fValue, fFixed, _ in atOccurences:
                    assert oCurField is oField;
                    dValues[fValue] = 1;
                    dFixed[fFixed]  = 1;
                if len(dValues) in (2, 3) and len(dFixed) == 1 and 3 in dFixed:
                    afValues = list(dValues);
                    if len(dValues) == 2:
                        fFixed = 2 if (afValues[0] ^ afValues[1]) & 1 else 1; # One of the bits are fixed, the other ignored.
                    else:
                        fFixed = 3;                                           # Both bits are fixed.
                    fValue = afValues[0] & fFixed;
                    print('debug transfer: %s: %u binaryops -> encoding: %s == %#x/%#x'
                          % (sInstrNm, len(atOccurences), sFieldNm, ~fValue & fFixed, fFixed,));
                    oField.fValue |= ~fValue & fFixed;
                    oField.fFixed |= fFixed;

                    # Remove the associated conditions (they'll be leaves).
                    oCondition = recursiveRemove(oCondition, [oCondition for _, _, _, oCondition in atOccurences]);
                    fMod = True;
                else:
                    print('info: %s: transfer cond to enc failed for: %s dValues=%s dFixed=%s'
                          % (sInstrNm, sFieldNm, dValues, dFixed));
            elif oField.cBitsWidth == 3 and len(atOccurences) >= 7:
                print('info: %s: TODO: transfer cond to enc for 3 bit field: %s (%s)' % (sInstrNm, sFieldNm, atOccurences,));

    return (oCondition, fMod);


def parseFeatures(aoJson, fVerbose = False):
    """
    Parses the list of features (parameters).
    """
    global g_aoAllArmFeatures;
    g_aoAllArmFeatures = [ArmFeature.fromJson(oJson) for oJson in aoJson];

    #global g_dAllArmFeaturesByName;
    for oFeature in g_aoAllArmFeatures:
        if oFeature.sName in g_dAllArmFeaturesByName:
            raise Exception('Feature %s is listed twice!' % (oFeature.sName,))
        g_dAllArmFeaturesByName[oFeature.sName] = oFeature;

    for sFeature in ('FEAT_ETMv4p1', 'FEAT_ETMv4p2', 'FEAT_ETMv4p3', 'FEAT_ETMv4p4', 'FEAT_ETMv4p5', 'FEAT_ETMv4p6',
                     'FEAT_GICv3', 'FEAT_GICv3p1', 'FEAT_GICv4', 'FEAT_GICv4p1', 'FEAT_GICv3_NMI', 'FEAT_GICv3_TDIR',
                     'FEAT_VPIPT',
                     'FEAT_AA64', 'FEAT_AA32', 'FEAT_SSVE_FEXPA', # Missing in 2024-12:
                     ):
        if sFeature not in g_dAllArmFeaturesByName:
            oFeature = ArmFeature(None, sFeature, []);
            g_aoAllArmFeatures.append(oFeature);
            g_dAllArmFeaturesByName[sFeature] = oFeature;

    g_aoAllArmFeatures = sorted(g_aoAllArmFeatures, key = operator.attrgetter('sName'));

    if fVerbose:
        # print the features.
        cchMaxName = max(len(oFeature.sName) for oFeature in g_aoAllArmFeatures)
        dTypeNm = {
            'Parameters.Boolean': 'boolean',
            'Parameters.Integer': 'integer',
        };
        for iFeature, oFeature in enumerate(g_aoAllArmFeatures):
            if oFeature.oSupportExpr:
                print('%3u: %s  %-*s := %s'
                      % (iFeature, dTypeNm[oFeature.sType], cchMaxName, oFeature.sName, oFeature.oSupportExpr.toString()));
            else:
                print('%3u: %s  %s' % (iFeature, dTypeNm[oFeature.sType], oFeature.sName,));
            if not oFeature.oSupportExpr or oFeature.oSupportExpr.toString().find('AArch64') < 0:
                for iConstraint, oConstraint in enumerate(oFeature.aoConstraints):
                    print('        #%u: %s' % (iConstraint, oConstraint.toString(),));

    return True;


def parseRegisters(aoJson):
    """
    Parses the list of registers.
    """
    global g_aoAllArmRegisters;
    g_aoAllArmRegisters = [];
    for dJson in aoJson:
        if dJson['_type'] != 'RegisterBlock':
            g_aoAllArmRegisters.append(ArmRegister.fromJson(dJson));
        else:
            ## @todo proper handling of RegisterBlocks.
            sStatePrefix = dJson['name'] + '.';
            for dSubJson in dJson['blocks']:
                g_aoAllArmRegisters.append(ArmRegister.fromJson(dSubJson, sStatePrefix));

    g_aoAllArmRegisters = sorted(g_aoAllArmRegisters, key = operator.attrgetter('sState', 'sName'));

    #global g_daoAllArmRegistersByState;
    #global g_ddoAllArmRegistersByStateByName;
    for oRegister in g_aoAllArmRegisters:
        if oRegister.sState in g_daoAllArmRegistersByState:
            g_daoAllArmRegistersByState[oRegister.sState] += [oRegister,];
            if oRegister.sName in g_ddoAllArmRegistersByStateByName[oRegister.sState]:
                raise Exception('Register %s.%s is listed twice!' % (oRegister.sState, oRegister.sName,))
            g_ddoAllArmRegistersByStateByName[oRegister.sState][oRegister.sName] = oRegister;
        else:
            g_daoAllArmRegistersByState[oRegister.sState]  = [oRegister,];
            g_ddoAllArmRegistersByStateByName[oRegister.sState] = { oRegister.sName: oRegister, };

    ## print the features.
    #cchMaxName = max(len(oFeature.sName) for oFeature in g_aoAllArmFeatures)
    #dTypeNm = {
    #    'Parameters.Boolean': 'boolean',
    #    'Parameters.Integer': 'integer',
    #};
    #for iFeature, oFeature in enumerate(g_aoAllArmFeatures):
    #    if oFeature.oSupportExpr:
    #        print('%3u: %s  %-*s := %s'
    #              % (iFeature, dTypeNm[oFeature.sType], cchMaxName, oFeature.sName, oFeature.oSupportExpr.toString()));
    #    else:
    #        print('%3u: %s  %s' % (iFeature, dTypeNm[oFeature.sType], oFeature.sName,));
    #    if not oFeature.oSupportExpr or oFeature.oSupportExpr.toString().find('AArch64') < 0:
    #        for iConstraint, oConstraint in enumerate(oFeature.aoConstraints):
    #            print('        #%u: %s' % (iConstraint, oConstraint.toString(),));
    #
    return True;


def numToStr1000Sep(iNumber):
    """ Formats iNumber with spaces as thousand separators. """
    sStr   = '%d' % (iNumber,)
    off    = len(sStr);
    offEnd = sStr[0] == '-';
    sRet   = '';
    while True:
        if off - offEnd <= 3:
            return sStr[:off] + sRet;
        sRet = ' ' + sStr[off - 3 : off] + sRet;
        off -= 3;

def nsElapsedAsStr(nsStart):
    """ Given a start timestamp, calculates and formats the number of nanoseconds elapsed. """
    return numToStr1000Sep(time.time_ns() - nsStart);


def _parseArmOpenSourceSpecification(dRawInstructions, dRawFeatures, dRawRegisters):
    """
    Parses the raw json specification, populating the global variables.

    Throws exceptions if there are issues with the specs.
    """

    #
    # Parse the system registers.
    #
    print("*** Parsing registers ...");
    nsStart = time.time_ns()
    global g_oArmRegistersVerInfo;
    g_oArmRegistersVerInfo = dRawRegisters[0]['_meta']['version'];
    parseRegisters(dRawRegisters);
    print("Found %u registers in %u states (%s ns)"
          % (len(g_aoAllArmRegisters), len(g_daoAllArmRegistersByState), nsElapsedAsStr(nsStart),) );

    #
    # Parse the features.
    #
    print("*** Parsing features ...");
    nsStart = time.time_ns()
    global g_oArmFeaturesVerInfo;
    g_oArmFeaturesVerInfo = dRawFeatures['_meta']['version'];
    parseFeatures(dRawFeatures['parameters']);
    print("Found %u feature definitions (%s ns)" % (len(g_aoAllArmFeatures), nsElapsedAsStr(nsStart),) );

    #
    # Parse the Instructions.
    #
    print("*** Parsing instructions ...");
    global g_oArmInstructionVerInfo;
    g_oArmInstructionVerInfo = dRawInstructions['_meta']['version'];
    parseInstructions(None, None, dRawInstructions['instructions'], dRawInstructions['assembly_rules']);

    # Sort the instruction array by name.
    global g_aoAllArmInstructions;
    g_aoAllArmInstructions = sorted(g_aoAllArmInstructions, key = operator.attrgetter('sName', 'sAsmDisplay'));

    print("Found %u instructions (%s ns)" % (len(g_aoAllArmInstructions), nsElapsedAsStr(nsStart),));
    #oBrk = g_dAllArmInstructionsByName['BRK_EX_exception'];
    #print("oBrk=%s" % (oBrk,))
    return True;


def loadArmOpenSourceSpecificationFromTar(sTarFile, sFileInstructions = 'Instructions.json', sFileFeatures = 'Features.json',
                                          sFileRegisters = 'Registers.json'):
    """
    Loads the ARM specifications from a tar file.
    """
    print("*** Loading specs from %s ..." % (sTarFile,));
    nsStart = time.time_ns();
    with tarfile.open(sTarFile, 'r') as oTarFile:
        with oTarFile.extractfile(sFileInstructions) as oFile:
            dRawInstructions = json.load(oFile);
        with oTarFile.extractfile(sFileFeatures) as oFile:
            dRawFeatures     = json.load(oFile);
        with oTarFile.extractfile(sFileRegisters) as oFile:
            dRawRegisters    = json.load(oFile);
    print("*** Done loading specs (%s ns)." % (nsElapsedAsStr(nsStart),));

    return _parseArmOpenSourceSpecification(dRawInstructions, dRawFeatures, dRawRegisters);


def loadArmOpenSourceSpecificationFromFiles(sFileInstructions = 'Instructions.json', sFileFeatures = 'Features.json',
                                            sFileRegisters = 'Registers.json', fInternalCall = False):
    """
    Loads the ARM specifications from individual files.
    """
    if not fInternalCall:
        print("*** Loading specs ...");
    nsStart = time.time_ns();
    with open(sFileInstructions, 'r', encoding = 'utf-8') as oFile:
        dRawInstructions = json.load(oFile);
    with open(sFileFeatures, 'r', encoding = 'utf-8') as oFile:
        dRawFeatures     = json.load(oFile);
    with open(sFileRegisters, 'r', encoding = 'utf-8') as oFile:
        dRawRegisters    = json.load(oFile);
    print("*** Done loading specs (%s ns)." % (nsElapsedAsStr(nsStart),));

    return _parseArmOpenSourceSpecification(dRawInstructions, dRawFeatures, dRawRegisters);


def loadArmOpenSourceSpecificationFromDir(sSpecDir, sFileInstructions = 'Instructions.json', sFileFeatures = 'Features.json',
                                          sFileRegisters = 'Registers.json'):
    """
    Loads the ARM specifications from a directory.
    """
    print("*** Loading specs from %s ..." % (sSpecDir,));
    if sSpecDir:
        if not os.path.isabs(sFileInstructions):
            sFileInstructions = os.path.normpath(os.path.join(sSpecDir, sFileInstructions));
        if not os.path.isabs(sFileFeatures):
            sFileFeatures     = os.path.normpath(os.path.join(sSpecDir, sFileFeatures));
        if not os.path.isabs(sFileRegisters):
            sFileRegisters    = os.path.normpath(os.path.join(sSpecDir, sFileRegisters));

    return loadArmOpenSourceSpecificationFromFiles(sFileInstructions, sFileFeatures, sFileRegisters, True);


def printSpecs(fPrintInstructions = False, fPrintInstructionsWithEncoding = False, fPrintInstructionsWithConds = False,
               fPrintFixedMaskStats = False, fPrintFixedMaskTop10 = False,
               fPrintSysRegs = False):
    """ Prints the specification if requested in the options. """

    if fPrintInstructions or fPrintInstructionsWithEncoding or fPrintInstructionsWithConds:
        for oInstr in g_aoAllArmInstructions:
            print('%08x/%08x %s %s' % (oInstr.fFixedMask, oInstr.fFixedValue, oInstr.getCName(), oInstr.sAsmDisplay));
            if fPrintInstructionsWithEncoding:
                for oField in sorted(oInstr.aoFields, key = operator.attrgetter('iFirstBit')): # ArmEncodesetField
                    print('  %2u L %2u: %010x/%010x%s%s'
                          % (oField.iFirstBit, oField.cBitsWidth, oField.fFixed, oField.fValue,
                             ' ' if oField.sName else '', oField.sName if oField.sName else '',));
            if fPrintInstructionsWithConds and not oInstr.oCondition.isBoolAndTrue():
                print('  condition: %s' % (oInstr.oCondition.toString(),));

    # Print stats on fixed bits:
    if fPrintFixedMaskStats:
        dCounts = collections.Counter();
        for oInstr in g_aoAllArmInstructions:
            cPopCount = bin(oInstr.fFixedMask).count('1');
            dCounts[cPopCount] += 1;

        print('');
        print('Fixed bit pop count distribution:');
        for i in range(33):
            if i in dCounts:
                print('  %2u: %u' % (i, dCounts[i]));

    # Top 10 fixed masks.
    if fPrintFixedMaskTop10:
        dCounts = collections.Counter();
        for oInstr in g_aoAllArmInstructions:
            dCounts[oInstr.fFixedMask] += 1;

        print('');
        print('Top 20 fixed masks:');
        for fFixedMask, cHits in dCounts.most_common(20):
            print('  %#x: %u times' % (fFixedMask, cHits,));

    # System registers.
    if fPrintSysRegs:
        print('');
        print('System registers:');
        for oReg in sorted(g_aoAllArmRegisters, key = operator.attrgetter('sState', 'sName')): # type: ArmRegister
            if oReg.sState != 'AArch64': continue; # temp
            print('   %s.%s' % (oReg.sState, oReg.sName, ));
            print('       Condition: %s' % (oReg.oCondition.toString(),));
            for oFieldset in oReg.aoFieldsets: # type: ArmFieldset
                print('       Fieldsset: %s' % (oFieldset.toString(),));
            for i, oAccessor in enumerate(oReg.aoAccessors): # type: int, ArmAccessorBase
                if isinstance(oAccessor, ArmAccessorSystem):
                    print('       Accessors[%u]: encoding=%s' % (i, oAccessor.oEncoding.toString(),));
                    print('                     name=%s' % (oAccessor.sName,));
                    if not ArmAstBool.isBoolAndTrue(oAccessor.oCondition):
                        print('                     condition=%s' % (oAccessor.oCondition.toString(),));
                    if oAccessor.oAccess: # ArmAstIfList
                        asLines = oAccessor.oAccess.toStringList('                         ');
                        print('\n'.join(asLines));
                else:
                    print('       Accessors[%u]: %s' % (i, oAccessor,));

    return True;

