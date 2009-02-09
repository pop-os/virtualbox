; $Id: kLdrA-os2.asm 2 2007-11-16 16:07:14Z bird $
;; @file
; kLdr - The Dynamic Loader, OS/2 Assembly Helpers.
;

;
; Copyright (c) 2006-2007 knut st. osmundsen <bird-kStuff-spam@anduin.net>
;
; This file is part of kStuff.
;
; kStuff is free software; you can redistribute it and/or
; modify it under the terms of the GNU Lesser General Public
; License as published by the Free Software Foundation; either
; version 2.1 of the License, or (at your option) any later version.
;
; In addition to the permissions in the GNU Lesser General Public
; License, you are granted unlimited permission to link the compiled
; version of this file into combinations with other programs, and to
; distribute those combinations without any restriction coming from
; the use of this file.
;
; kStuff is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; Lesser General Public License for more details.
;
; You should have received a copy of the GNU Lesser General Public
; License along with kStuff; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
; 02110-1301, USA
;

segment TEXT32 public align=16 CLASS=CODE use32

;
; _DLL_InitTerm
;
..start:
extern _DLL_InitTerm
    jmp _DLL_InitTerm


;
; kLdrLoadExe wrapper which loads the bootstrap stack.
;
global _kLdrDyldLoadExe
_kLdrDyldLoadExe:
    push    ebp
    mov     ebp, esp

    ; switch stack.
;    extern _abStack
;    lea     esp, [_abStack + 8192 - 4]
    push    dword [ebp + 8 + 20]
    push    dword [ebp + 8 + 16]
    push    dword [ebp + 8 + 12]
    push    dword [ebp + 8 +  8]

    ; call worker on the new stack.
    extern  _kldrDyldLoadExe
    call    _kldrDyldLoadExe

    ; we shouldn't return!
we_re_not_supposed_to_get_here:
    int3
    int3
    jmp short we_re_not_supposed_to_get_here

