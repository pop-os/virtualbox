/* $Id: kDbgSpace.cpp 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kDbg - The Debug Info Reader, Address Space Manager.
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
#include <k/kHlpString.h>
#include <k/kAvl.h>


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/** Pointer to a name space module. */
typedef struct KDBGSPACEMOD *PKDBGSPACEMOD;

/**
 * Tracks a module segment in the address space.
 *
 * These segments are organized in two trees, by address in the
 * KDBGSPACE::pSegRoot tree and by selector value in the
 * KDBGSPACE::pSegSelRoot tree.
 *
 * While the debug module reader could easily provide us with
 * segment names and it could perhaps be interesting to lookup
 * a segment by its name in some situations, this has been
 * considered too much bother for now. :-)
 */
typedef struct KDBGSPACESEG
{
    /** The module segment index. */
    KI32                    iSegment;
    /** The address space module structure this segment belongs to. */
    PKDBGSPACEMOD           pSpaceMod;
} KDBGSPACESEG;
typedef KDBGSPACESEG *PKDBGSPACESEG;


/**
 * Track a module in the name space.
 *
 * Each module in the address space can be addressed efficiently
 * by module name. The module name has to be unique.
 */
typedef struct KDBGSPACEMOD
{
    /** The module name hash. */
    KU32                    u32Hash;
    /** The length of the module name. */
    KU32                    cchName;
    /** The module name. */
    char                   *pszName;
    /** The next module in the same bucket. */
    PKDBGSPACEMOD           pNext;
    /** Pointer to the debug module reader. */
    PKDBGMOD                pMod;
    /** The number of segments. */
    KU32                    cSegs;
    /** The segment array. (variable length) */
    KDBGSPACESEG            aSegs[1];
} KDBGSPACEMOD;


typedef struct KDBGCACHEDSYM *PKDBGCACHEDSYM;
/**
 * A cached symbol.
 */
typedef struct KDBGCACHEDSYM
{
    /** The symbol name hash. */
    KU32                    u32Hash;
    /** The next symbol in the same bucket. */
    PKDBGCACHEDSYM          pNext;
    /** The next symbol belonging to the same module as this. */
    PKDBGCACHEDSYM          pNextMod;
    /** The cached symbol information. */
    KDBGSYMBOL              Sym;
} KDBGCACHEDSYM;


/**
 * A symbol cache.
 */
typedef struct KDBGSYMCACHE
{
    /** The symbol cache magic. (KDBGSYMCACHE_MAGIC) */
    KU32                    u32Magic;
    /** The maximum number of symbols.*/
    KU32                    cMax;
    /** The current number of symbols.*/
    KU32                    cCur;
    /** The number of hash buckets. */
    KU32                    cBuckets;
    /** The address lookup tree. */
    PKDBGADDRAVL            pAddrTree;
    /** Array of hash buckets.
     * The size is selected according to the cache size. */
    PKDBGCACHEDSYM         *paBuckets[1];
} KDBGSYMCACHE;
typedef KDBGSYMCACHE *PKDBGSYMCACHE;


/**
 * A user symbol record.
 *
 * The user symbols are organized in the KDBGSPACE::pUserRoot tree
 * and form an overlay that overrides the debug info retrieved from
 * the KDBGSPACE::pSegRoot tree.
 *
 * In the current implementation the user symbols are unique and
 * one would have to first delete a symbol in order to add another
 * at the same address. This may be changed later, perhaps.
 */
typedef struct KDBGSPACEUSERSYM
{

} KDBGSPACEUSERSYM;
typedef KDBGSPACEUSERSYM *PKDBGSPACEUSERSYM;



/**
 * Address space.
 */
typedef struct KDBGSPACE
{
    /** The addresspace magic. (KDBGSPACE_MAGIC) */
    KU32            u32Magic;
    /** User defined address space identifier or data pointer. */
    KUPTR           uUser;
    /** The name of the address space. (Optional) */
    const char     *pszName;


} KDBGSPACE;
/** Pointer to an address space. */
typedef struct KDBGSPACE *PKDBGSPACE;
/** Pointer to an address space pointer. */
typedef PKDBGSPACE *PPKDBGSPACE;


KDBG_DECL(int) kDbgSpaceCreate(PPDBGSPACE ppSpace, KDBGADDR LowAddr, DBGADDR HighAddr,
                               KUPTR uUser, const char *pszName)
{
    /*
     * Validate input.
     */
    kDbgAssertPtrReturn(ppSpace);
    *ppSpace = NULL;
    kDbgAssertPtrNullReturn(pszName);
    kDbgAssertReturn(LowAddr < HighAddr);

    /*
     * Create and initialize the address space.
     */
    PKDBGSPACE pSpace = (PKDBGSPACE)kHlpAlloc(sizeof(*pSpace));
    if (!pSpace)
        return KERR_NO_MEMORY;
    pSpace->u32Magic = KDBGSPACE_MAGIC;
    pSpace->uUser = uUser;
    pSpace->pszName = pszName;

}
