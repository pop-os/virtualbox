/* $Id: kDbgSymbol.cpp 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kDbg - The Debug Info Reader, Symbols.
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
#include "kDbgInternal.h"
#include <k/kHlpAlloc.h>


/**
 * Duplicates a symbol.
 *
 * To save heap space, the returned symbol will not own more heap space than
 * it strictly need to. So, it's not possible to append stuff to the symbol
 * or anything of that kind.
 *
 * @returns Pointer to the duplicate.
 *          This must be freed using kDbgSymbolFree().
 * @param   pSymbol     The symbol to be duplicated.
 */
KDBG_DECL(PKDBGSYMBOL) kDbgSymbolDup(PCKDBGSYMBOL pSymbol)
{
    kDbgAssertPtrReturn(pSymbol, NULL);
    KSIZE cb = K_OFFSETOF(KDBGSYMBOL, szName[pSymbol->cchName + 1]);
    PKDBGSYMBOL pNewSymbol = (PKDBGSYMBOL)kHlpDup(pSymbol, cb);
    if (pNewSymbol)
        pNewSymbol->cbSelf = cb;
    return pNewSymbol;
}


/**
 * Frees a symbol obtained from the kDbg API.
 *
 * @returns 0 on success.
 * @returns KERR_INVALID_POINTER if pSymbol isn't a valid pointer.
 *
 * @param   pSymbol     The symbol to be freed. The null pointer is ignored.
 */
KDBG_DECL(int) kDbgSymbolFree(PKDBGSYMBOL pSymbol)
{
    if (!pSymbol)
    {
        kDbgAssertPtrReturn(pSymbol, KERR_INVALID_POINTER);
        pSymbol->cbSelf = 0;
        kHlpFree(pSymbol);
    }
    return 0;
}

