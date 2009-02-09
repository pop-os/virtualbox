/* $Id: kDbgModLdr.cpp 2 2007-11-16 16:07:14Z bird $ */
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
#include "kLdr.h"
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
static int kDbgModPeQueryLine(PKDBGMOD pMod, int32_t iSegment, KDBGADDR off, PKDBGLINE pLine)
{
    //PKDBGMODLDR pThis = (PKDBGMODLDR)pMod;
    return KERR_NOT_IMPLEMENTED;
}


/**
 * @copydoc KDBGMODOPS::pfnQuerySymbol
 */
static int kDbgModPeQuerySymbol(PKDBGMOD pMod, int32_t iSegment, KDBGADDR off, PKDBGSYMBOL pSym)
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
static int kDbgModLdrOpen(PKDBGHLPFILE pFile, KFOFF off, KFOFF cb, PKLDRMOD pLdrMod, PKDBGMOD *ppMod)
{
    return KERR_NOT_IMPLEMENTED;
}


/**
 * Methods for a PE module.
 */
const KDBGMODOPS g_kDbgModPeOps =
{
    "kLdr",
    kDbgModLdrOpen,
    kDbgModLdrClose,
    kDbgModLdrQuerySymbol,
    kDbgModLdrQueryLine
    "kLdr"
};




