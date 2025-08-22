#!/usr/bin/env python
# -*- coding: utf-8 -*-
# $Id: PyCommonVmm.py $

"""
Common python main() support code for exceptions and profiling.
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
__version__ = "$Revision: 169409 $"

# Standard python imports.
import io;
import os;
import re;
import sys;
import traceback;
# profiling:
import cProfile;
import pstats


def printException(oXcpt):
    print('----- Exception Caught! -----', flush = True);
    cMaxLines = 1;
    try:    cchMaxLen = os.get_terminal_size()[0] * cMaxLines;
    except: cchMaxLen = 80 * cMaxLines;
    cchMaxLen -= len('     =  ...');

    oTB = traceback.TracebackException.from_exception(oXcpt, limit = None, capture_locals = True);
    # No locals for the outer frame.
    oTB.stack[0].locals = {};
    # Suppress insanely long variable values.
    for oFrameSummary in oTB.stack:
        if oFrameSummary.locals:
            #for sToDelete in ['ddAsmRules', 'aoInstructions',]:
            #    if sToDelete in oFrameSummary.locals:
            #        del oFrameSummary.locals[sToDelete];
            for sKey, sValue in oFrameSummary.locals.items():
                if len(sValue) > cchMaxLen - len(sKey):
                    sValue = sValue[:cchMaxLen - len(sKey)] + ' ...';
                if '\n' in sValue:
                    sValue = sValue.split('\n')[0] + ' ...';
                oFrameSummary.locals[sKey] = sValue;
    idxFrame = 0;
    asFormatted = [];
    oReFirstFrameLine = re.compile(r'^  File ".*", line \d+, in ')
    for sLine in oTB.format():
        if oReFirstFrameLine.match(sLine):
            idxFrame += 1;
        asFormatted.append(sLine);
    for sLine in asFormatted:
        if oReFirstFrameLine.match(sLine):
            idxFrame -= 1;
            sLine = '#%u %s' % (idxFrame, sLine.lstrip());
        print(sLine);
    print('----', flush = True);

def printProfilerStats(oProfiler, sSortColumn, cMaxRows):
    # Format the raw stats into a string stream and split that up into lines.
    oStringStream = io.StringIO();
    pstats.Stats(oProfiler, stream = oStringStream).strip_dirs().sort_stats(sSortColumn).print_stats(cMaxRows);
    asStats = oStringStream.getvalue().split('\n');

    # Print everything up to the column headers.
    idx = 0;
    while idx < len(asStats) and asStats[idx].find('filename:lineno(function)') < 0:
        print(asStats[idx]);
        idx += 1;
    if idx >= len(asStats): return False;

    # Strip the preamble and find the column count.
    asStats     = asStats[idx:];
    asHeaders   = asStats[0].split();
    cColumns    = len(asHeaders);
    iSortColumn = asHeaders.index(sSortColumn);
    assert iSortColumn >= 0, 'asHeaders=%s sSortColumn=%s' % (asHeaders, sSortColumn,);

    # Split the stats up into an array columns and calc max column widths.
    acchStats = [6 for _ in range(cColumns)];
    aasStats  = [];
    for iLine, sLine in enumerate(asStats):
        asCells = sLine.split(maxsplit = cColumns - 1);
        if len(asCells) == cColumns:
            if iLine > 16 and asCells[iSortColumn] in { '0.000', '0.001', '0.002', '0.003', '0.004', '0.005' }:
                break;
            aasStats.append(asCells);
            for i in range(cColumns):
                acchStats[i] = max(acchStats[i], len(asCells[i]));
        elif not sLine.strip():
            break;
        else:
            assert False, 'iLine=%s sLine=%s asCells=%s cColumns=%s' % (iLine, sLine, asCells, cColumns,)

    # print it all.
    for asRow in aasStats:
        print('  '.join([sCell.rjust(acchStats[i]) for i, sCell in enumerate(asRow[:-1])]) + ' ' + asRow[-1]);
    return True;

def mainWrapperCatchXcptAndDoProfiling(fnMain, sSortColumn = 'tottime', cMaxRows = 64):
    fProfileIt = 'VBOX_PROFILE_PYTHON' in os.environ;
    oProfiler  = cProfile.Profile() if fProfileIt else None;
    try:
        if not oProfiler:
            iRcExit = fnMain(sys.argv);
        else:
            iRcExit = oProfiler.runcall(fnMain, sys.argv);
    except Exception as oXcptOuter:
        printException(oXcptOuter);
        iRcExit = 2;
    except KeyboardInterrupt as oXcptOuter:
        printException(oXcptOuter);
        iRcExit = 2;
    if oProfiler:
        if not oProfiler:
            oProfiler.print_stats(sort = sSortColumn);
        else:
            printProfilerStats(oProfiler, sSortColumn, cMaxRows);
    return iRcExit;

