; $Id: kLdrExeStub-os2.asm 2 2007-11-16 16:07:14Z bird $
;; @file
; kLdr - OS/2 Loader Stub.
;
; This file contains a 64kb code/data/stack segment which is used to kick off
; the loader dll that loads the process.
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

struc KLDRARGS
    .fFlags         resd 1
    .enmSearch      resd 1
    .szExecutable   resb 260
    .szDefPrefix    resb 16
    .szDefSuffix    resb 16
    .szLibPath      resb (4096 - (4 + 4 + 16 + 16 + 260))
endstruc

extern _kLdrDyldLoadExe


segment DATA32 stack CLASS=DATA align=16 use32
..start:
    push    args
    jmp     _kLdrDyldLoadExe

;
; Argument structure.
;
align 4
args:
istruc KLDRARGS
    at KLDRARGS.fFlags,         dd 0
    at KLDRARGS.enmSearch,      dd 2 ;KLDRDYLD_SEARCH_HOST
    at KLDRARGS.szDefPrefix,    db ''
    at KLDRARGS.szDefSuffix,    db '.dll'
;    at KLDRARGS.szExecutable,   db 'tst-0.exe'
    at KLDRARGS.szLibPath,      db ''
iend

segment STACK32 stack CLASS=STACK align=16 use32
; pad up to 64KB.
resb 60*1024

global WEAK$ZERO
WEAK$ZERO EQU 0
group DGROUP, DATA32  STACK32

