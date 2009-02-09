/* $Id: kLdr.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kLdr - The Dynamic Loader.
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


/** @mainpage   kLdr - The Dynamic Loader
 *
 * The purpose of kLdr is to provide a generic interface for querying
 * information about and loading executable image modules.
 *
 * kLdr defines the term executable image to include all kinds of files that contains
 * binary code that can be executed on a CPU - linker objects (OBJs/Os), shared
 * objects (SOs), dynamic link libraries (DLLs), executables (EXEs), and all kinds
 * of kernel modules / device drivers (SYSs).
 *
 * kLdr provides two types of services:
 *      -# Inspect or/and load individual modules (kLdrMod).
 *      -# Work as a dynamic loader - construct and maintain an address space (kLdrDy).
 *
 * The kLdrMod API works on KLDRMOD structures where all the internals are exposed, while
 * the kLdrDy API works opque KLDRDY structures. KLDRDY are in reality simple wrappers
 * around KLDRMOD with some extra linking and attributes.
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <k/kLdr.h>
#include "kLdrInternal.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** Flag indicating whether we've initialized the loader or not.
 *
 * 0 if not initialized.
 * -1 if we're initializing or terminating.
 * 1 if we've successfully initialized it.
 * -2 if initialization failed.
 */
static int volatile g_fInitialized;



/**
 * Initializes the loader.
 * @returns 0 on success, non-zero OS status code on failure.
 */
int kldrInit(void)
{
    int rc;

    /* check we're already good. */
    if (g_fInitialized == 1)
        return 0;

    /* a tiny serialization effort. */
    for (;;)
    {
        if (g_fInitialized == 1)
            return 0;
        if (g_fInitialized == -2)
            return -1;
        /** @todo atomic test and set if we care. */
        if (g_fInitialized == 0)
        {
            g_fInitialized = -1;
            break;
        }
        kHlpSleep(1);
    }

    /*
     * Do the initialization.
     */
    rc = kHlpHeapInit();
    if (!rc)
    {
        rc = kLdrDyldSemInit();
        if (!rc)
        {
            rc = kldrDyldInit();
            if (!rc)
            {
                g_fInitialized = 1;
                return 0;
            }
            kLdrDyldSemTerm();
        }
        kHlpHeapTerm();
    }
    g_fInitialized = -2;
    return rc;
}


/**
 * Terminates the loader.
 */
void kldrTerm(void)
{
    /* can't terminate unless it's initialized. */
    if (g_fInitialized != 1)
        return;
    g_fInitialized = -1;

    /*
     * Do the termination.
     */
    kLdrDyldSemTerm();
    kHlpHeapTerm();

    /* done */
    g_fInitialized = 0;
}

