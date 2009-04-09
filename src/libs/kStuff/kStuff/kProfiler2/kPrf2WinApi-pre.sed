# $Id: kPrf2WinApi-pre.sed 13 2008-04-20 10:13:43Z bird $
## @file
# This SED script will try normalize a windows header
# in order to make it easy to pick out function prototypes.
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


# Drop all preprocessor lines (#if/#else/#endif/#define/#undef/#pragma/comments)
# (we don't bother with multi line comments ATM.)
/^[[:space:]]*#/b drop_line
/^[[:space:]]*\/\//b drop_line

# Drop empty lines.
/^[[:space:]]*$/b drop_line

# Drop trailing comments and trailing whitespace
s/[[:space:]][[:space:]]*\/\.*$//g
s,[[:space:]][[:space:]]*/\*[^*/]*\*/[[:space:]]*$,,g
s/[[:space:]][[:space:]]*$//g

# Pick out the WINBASEAPI stuff (WinBase.h)
/^WINBASEAPI/b winapi
/^NTSYSAPI/b winapi
/^WINAPI$/b winapi_perhaps
/^APIENTRY$/b winapi_perhaps
h
d
b end

# No WINBASEAPI, so we'll have to carefully check the hold buffer.
:winapi_perhaps
x
/^[A-Z][A-Z0-9_][A-Z0-9_]*[A-Z0-9]$/!b drop_line
G
s/\r/ /g
s/\n/ /g
b winapi

# Make it one line and a bit standardized
:winapi
/;/b winapi_got_it
N
b winapi
:winapi_got_it
s/\n/ /g
s/[[:space:]][[:space:]]*\/\*[^*/]*\*\/[[:space:]]*//g
s/[[:space:]][[:space:]]*(/(/g
s/)[[:space:]][[:space:]]*/)/g
s/(\([^[:space:]]\)/( \1/g
s/\([^[:space:]]\))/\1 )/g
s/[*]\([^[:space:]]\)/* \1/g
s/\([^[:space:]]\)[*]/\1 */g
s/[[:space:]][[:space:]]*/ /g
s/[[:space:]][[:space:]]*,/,/g
s/,/, /g
s/,[[:space:]][[:space:]]*/, /g

# Drop the nasty bit of the sal.h / SpecString.h stuff.
s/[[:space:]]__[a-z][a-z_]*([^()]*)[[:space:]]*/ /g
s/[[:space:]]__out[a-z_]*[[:space:]]*/ /g
s/[[:space:]]__in[a-z_]*[[:space:]]*/ /g
s/[[:space:]]__deref[a-z_]*[[:space:]]*/ /g
s/[[:space:]]__reserved[[:space:]]*/ /g
s/[[:space:]]__nullnullterminated[[:space:]]*/ /g
s/[[:space:]]__checkReturn[[:space:]]*/ /g

# Drop some similar stuff.
s/[[:space:]]OPTIONAL[[:space:]]/ /g
s/[[:space:]]OPTIONAL,/ ,/g

# The __declspec() bit isn't necessary
s/WINBASEAPI *//
s/NTSYSAPI *//
s/DECLSPEC_NORETURN *//
s/__declspec([^()]*) *//

# Normalize spaces.
s/[[:space:]]/ /g

# Clear the hold space
x
s/^.*$//
x
b end

:drop_line
s/^.*$//
h
d

:end

