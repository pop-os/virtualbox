# $Id: kPrf2WinApi-genimp.sed 11 2008-04-20 09:18:23Z bird $
##
# Generate imports from normalized dumpbin output.
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

# Normalize the input a bit.
s/[[:space:]][[:space:]]*/ /g
s/^[[:space:]]//
s/[[:space:]]$//
/^$/b drop_line

# Expects a single name - no ordinals yet.
/\@/b have_at

s/^\(.*\)$/  \1=kPrf2Wrap_\1/
b end

:have_at
h
s/^\([^ ]\)\(@[0-9]*\)$/  \1\2=kPrf2Wrap_\1/
p
g
s/^\([^ ]\)\(@[0-9]*\)$/  \1=kPrf2Wrap_\1/
b end

:drop_line
d
b end

:end
