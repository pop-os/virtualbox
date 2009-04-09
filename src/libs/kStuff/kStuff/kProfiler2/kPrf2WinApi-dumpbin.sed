# $Id: kPrf2WinApi-dumpbin.sed 13 2008-04-20 10:13:43Z bird $
## @file
# Strip down dumpbin /export output.
#

#
# Copyright (c) 2008 knut st. osmundsen <bird-src-spam@anduin.net>
#
# This file is part of kProfiler.
#
# kProfiler is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# kProfiler is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with kProfiler; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#


#
# State switch
#
x
/^exports$/b exports
/^summary$/b summary
b header

#
# Header
#
:header
x
/^[[:space:]][[:space:]]*ordinal[[:space:]]*name[[:space:]]*$/b switch_to_exports
b drop_line

#
# Exports
#
:switch_to_exports
s/^.*$/exports/
h
b drop_line

:exports
x
/^[[:space:]][[:space:]]*Summary[[:space:]]*$/b switch_to_summary
s/^[[:space:]]*//
s/[[:space:]]*$//
s/[[:space:]][[:space:]]*/ /g
/^$/b drop_line

# Filter out APIs that hasn't been implemented.
/AddLocalAlternateComputerNameA/b drop_line
/AddLocalAlternateComputerNameW/b drop_line
/EnumerateLocalComputerNamesA/b drop_line
/EnumerateLocalComputerNamesW/b drop_line
/RemoveLocalAlternateComputerNameA/b drop_line
/RemoveLocalAlternateComputerNameW/b drop_line
/SetLocalPrimaryComputerNameA/b drop_line
/SetLocalPrimaryComputerNameW/b drop_line
/__C_specific_handler/b drop_line
/__misaligned_access/b drop_line
/_local_unwind/b drop_line

b end

#
# Summary
#
:switch_to_summary
s/^.*$/summary/
h
b drop_line

:summary
x
b drop_line

#
# Tail
#
:drop_line
d
:end

