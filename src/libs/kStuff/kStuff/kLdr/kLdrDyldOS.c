/* $Id: kLdrDyldOS.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kLdr - The Dynamic Loader, OS specific operations.
 */

/*
 * Copyright (c) 2006-2007 knut st. osmundsen <bird-kStuff-spam@anduin.net>
 *
 * This file is part of kStuff.
 *
 * kStuff is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * In addition to the permissions in the GNU Lesser General Public
 * License, you are granted unlimited permission to link the compiled
 * version of this file into combinations with other programs, and to
 * distribute those combinations without any restriction coming from
 * the use of this file.
 *
 * kStuff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with kStuff; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <k/kLdr.h>
#include "kLdrInternal.h"

#if K_OS == K_OS_OS2
# define INCL_BASE
# define INCL_ERRORS
# include <os2.h>

#elif K_OS == K_OS_WINDOWS
# undef IMAGE_DOS_SIGNATURE
# undef IMAGE_NT_SIGNATURE
# include <Windows.h>

#else
# include <k/kHlpAlloc.h>

#endif


/**
 * Allocates a stack.
 *
 * @returns Pointer to the stack. NULL on allocation failure (assumes out of memory).
 * @param   cb      The size of the stack. This shall be page aligned.
 *                  If 0, a OS specific default stack size will be employed.
 */
void *kldrDyldOSAllocStack(KSIZE cb)
{
#if K_OS == K_OS_OS2
    APIRET rc;
    PVOID pv;

    if (!cb)
        cb = 1 * 1024*1024; /* 1MB */

    rc = DosAllocMem(&pv, cb, OBJ_TILE | PAG_COMMIT | PAG_WRITE | PAG_READ);
    if (rc == NO_ERROR)
        return pv;
    return NULL;

#elif K_OS == K_OS_WINDOWS

    if (!cb)
        cb = 1 *1024*1024; /* 1MB */

    return VirtualAlloc(NULL, cb, MEM_COMMIT, PAGE_READWRITE);

#else
    void *pv;

    if (!cb)
        cb = 1 * 1024*1024; /* 1MB */

    if (!kHlpPageAlloc(&pv, cb, KPROT_READWRITE, K_FALSE))
        return pv;
    return NULL;
#endif
}


/**
 * Invokes the main executable entry point with whatever
 * parameters specific to the host OS and/or module format.
 *
 * @returns
 * @param   uMainEPAddress  The address of the main entry point.
 * @param   pvStack         Pointer to the stack object.
 * @param   cbStack         The size of the stack object.
 */
int kldrDyldOSStartExe(KUPTR uMainEPAddress, void *pvStack, KSIZE cbStack)
{
#if K_OS == K_OS_WINDOWS
    /*
     * Invoke the entrypoint on the current stack for now.
     * Deal with other formats and stack switching another day.
     */
    int rc;
    int (*pfnEP)(void);
    pfnEP = (int (*)(void))uMainEPAddress;

    rc = pfnEP();

    TerminateProcess(GetCurrentProcess(), rc);
    kHlpAssert(!"TerminateProcess failed");
    for (;;)
        TerminateProcess(GetCurrentProcess(), rc);
#endif

    return -1;
}


void kldrDyldDoLoadExeStackSwitch(PKLDRDYLDMOD pExe, void *pvStack, KSIZE cbStack)
{
    /*kHlpAssert(!"not implemented");*/

    /** @todo implement this properly! */
    kldrDyldDoLoadExe(pExe);
}

