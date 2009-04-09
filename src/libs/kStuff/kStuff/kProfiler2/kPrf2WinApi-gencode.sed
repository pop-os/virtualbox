# $Id: kPrf2WinApi-gencode.sed 11 2008-04-20 09:18:23Z bird $
## @file
# Generate code (for kernel32).
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

# Example:
#       BOOL WINAPI FindActCtxSectionGuid( DWORD dwFlags, const GUID * lpExtensionGuid, ULONG ulSectionId, const GUID * lpGuidToFind, PACTCTX_SECTION_KEYED_DATA ReturnedData );
#
# Should be turned into:
#       typedef BOOL WINAPI FN_FindActCtxSectionGuid( DWORD dwFlags, const GUID * lpExtensionGuid, ULONG ulSectionId, const GUID * lpGuidToFind, PACTCTX_SECTION_KEYED_DATA ReturnedData );
#       __declspec(dllexport) BOOL WINAPI kPrf2Wrap_FindActCtxSectionGuid( DWORD dwFlags, const GUID * lpExtensionGuid, ULONG ulSectionId, const GUID * lpGuidToFind, PACTCTX_SECTION_KEYED_DATA ReturnedData )
#       {
#           static FN_FindActCtxSectionGuid *pfn = 0;
#           if (!pfn)
#               kPrfWrapResolve((void **)&pfn, "FindActCtxSectionGuid", &g_Kernel32);
#           return pfn( dwFlags, lpExtensionGuid, ulSectionId, lpGuidToFind, ReturnedData );
#       }
#

# Ignore empty lines.
/^[[:space:]]*$/b delete

# Some hacks.
/([[:space:]]*VOID[[:space:]]*)/b no_hacking_void
s/([[:space:]]*\([A-Z][A-Z0-9_]*\)[[:space:]]*)/( \1 a)/
:no_hacking_void


# Save the pattern space.
h

# Make the typedef.
s/[[:space:]]\([A-Za-z_][A-Za-z0-9_]*\)(/ FN_\1(/
s/^/typedef /
p

# Function definition
g
s/\n//g
s/\r//g
s/[[:space:]]\([A-Za-z_][A-Za-z0-9_]*\)(/ kPrf2Wrap_\1(/
s/^/__declspec(dllexport) /
s/;//
p
i\
{

#     static FN_FindActCtxSectionGuid *pfn = 0;
#     if (!pfn)
g
s/^.*[[:space:]]\([A-Za-z_][A-Za-z0-9_]*\)(.*$/    static FN_\1 *pfn = 0;/
p
i\
    if (!pfn)

#       kPrfWrapResolve((void **)&pfn, "FindActCtxSectionGuid", &g_Kernel32);
g
s/^.*[[:space:]]\([A-Za-z_][A-Za-z0-9_]*\)(.*$/        kPrf2WrapResolve((void **)\&pfn, "\1\", \&g_Kernel32);/
p

#     The invocation and return statement.
#     Some trouble here....
g
/^VOID WINAPI/b void_return
/^void WINAPI/b void_return
/^VOID __cdecl/b void_return
/^void __cdecl/b void_return
/^VOID NTAPI/b void_return
/^void NTAPI/b void_return
s/^.*(/    return pfn(/
b morph_params

:void_return
s/^.*(/    pfn(/

:morph_params
s/ *\[\] *//
s/ \*/ /g
s/, *[a-zA-Z_][^,)]* \([a-zA-Z_][a-zA-Z_0-9]* *)\)/, \1/g
s/( *[a-zA-Z_][^,)]* \([a-zA-Z_][a-zA-Z_0-9]* *[,)]\)/( \1/g
s/, *[a-zA-Z_][^,)]* \([a-zA-Z_][a-zA-Z_0-9]* *,\)/, \1/g
s/, *[a-zA-Z_][^,)]* \([a-zA-Z_][a-zA-Z_0-9]* *,\)/, \1/g
s/, *[a-zA-Z_][^,)]* \([a-zA-Z_][a-zA-Z_0-9]* *,\)/, \1/g
s/, *[a-zA-Z_][^,)]* \([a-zA-Z_][a-zA-Z_0-9]* *,\)/, \1/g
s/( VOID )/ ()/
s/( void )/ ()/
p
i\
}
i\

# Done
:delete
d

