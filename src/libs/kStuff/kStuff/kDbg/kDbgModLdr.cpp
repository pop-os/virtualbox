/* $Id: kDbgModLdr.cpp 6 2008-02-03 23:37:34Z bird $ */
/** @file
 * kDbg - The Debug Info Reader, kLdr Based.
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
#include <k/kDbg.h>
#include <k/kLdr.h>
#include "kDbgInternal.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/**
 * A kLdr based debug reader.
 */
typedef struct KDBGMODLDR
{
    /** The common module core. */
    KDBGMOD     Core;
    /** Pointer to the loader module. */
    PKLDRMOD    pLdrMod;
} KDBGMODLDR, *PKDBGMODLDR;


/**
 * @copydoc KDBGMODOPS::pfnQueryLine
 */
static int kDbgModLdrQueryLine(PKDBGMOD pMod, KI32 iSegment, KDBGADDR uOffset, PKDBGLINE pLine)
{
    //PKDBGMODLDR pThis = (PKDBGMODLDR)pMod;
    return KERR_NOT_IMPLEMENTED;
}


/**
 * @copydoc KDBGMODOPS::pfnQuerySymbol
 */
static int kDbgModLdrQuerySymbol(PKDBGMOD pMod, KI32 iSegment, KDBGADDR off, PKDBGSYMBOL pSym)
{
    //PKDBGMODLDR pThis = (PKDBGMODLDR)pMod;
    return KERR_NOT_IMPLEMENTED;
}


/**
 * @copydoc KDBGMODOPS::pfnClose
 */
static int kDbgModLdrClose(PKDBGMOD pMod)
{
    //PKDBGMODLDr pThis = (PKDBGMODLDR)pMod;
    return KERR_NOT_IMPLEMENTED;
}


/**
 * @copydocs  KDBGMODOPS::pfnOpen.
 */
static int kDbgModLdrOpen(PKDBGMOD *ppMod, PKRDR pRdr, KBOOL fCloseRdr, KFOFF off, KFOFF cb, struct KLDRMOD *pLdrMod)
{
    return KERR_NOT_IMPLEMENTED;
}


/**
 * Methods for a PE module.
 */
const KDBGMODOPS g_kDbgModLdr =
{
    "kLdr",
    NULL,
    kDbgModLdrOpen,
    kDbgModLdrClose,
    kDbgModLdrQuerySymbol,
    kDbgModLdrQueryLine,
    "kLdr"
};




