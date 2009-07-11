/* $Id: alloc-ef-cpp.cpp $ */
/** @file
 * IPRT - Memory Allocation, C++ electric fence.
 */

/*
 * Copyright (C) 2006-2007 Sun Microsystems, Inc.
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "alloc-ef.h"

#ifdef RTALLOC_EFENCE_CPP /* rest of the file */

#include <new>


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/** @todo test this on MSC */

/* MSC declares the operators as cdecl it seems. */
#ifdef _MSC_VER
# define RT_EF_CDECL    __cdecl
#else
# define RT_EF_CDECL
#endif

/* MSC doesn't use the standard namespace. */
#ifdef _MSC_VER
# define RT_EF_SIZE_T   size_t
#else
# define RT_EF_SIZE_T   std::size_t
#endif


void *RT_EF_CDECL operator new(RT_EF_SIZE_T cb) throw(std::bad_alloc)
{
    void *pv = rtMemAlloc("new", RTMEMTYPE_NEW, cb, ((void **)&cb)[-1], 0, NULL, NULL);
    if (!pv)
        throw std::bad_alloc();
    return pv;
}


void *RT_EF_CDECL operator new(RT_EF_SIZE_T cb, const std::nothrow_t &) throw()
{
    void *pv = rtMemAlloc("new nothrow", RTMEMTYPE_NEW, cb, ((void **)&cb)[-1], 0, NULL, NULL);
    return pv;
}


void RT_EF_CDECL operator delete(void *pv) throw()
{
    rtMemFree("delete", RTMEMTYPE_DELETE, pv, ((void **)&pv)[-1], 0, NULL, NULL);
}


void RT_EF_CDECL operator delete(void * pv, const std::nothrow_t &) throw()
{
    rtMemFree("delete nothrow", RTMEMTYPE_DELETE, pv, ((void **)&pv)[-1], 0, NULL, NULL);
}


/*
 *
 * Array object form.
 * Array object form.
 * Array object form.
 *
 */

void *RT_EF_CDECL operator new[](RT_EF_SIZE_T cb) throw(std::bad_alloc)
{
    void *pv = rtMemAlloc("new[]", RTMEMTYPE_NEW_ARRAY, cb, ((void **)&cb)[-1], 0, NULL, NULL);
    if (!pv)
        throw std::bad_alloc();
    return pv;
}


void * RT_EF_CDECL operator new[](RT_EF_SIZE_T cb, const std::nothrow_t &) throw()
{
    void *pv = rtMemAlloc("new[] nothrow", RTMEMTYPE_NEW_ARRAY, cb, ((void **)&cb)[-1], 0, NULL, NULL);
    return pv;
}


void RT_EF_CDECL operator delete[](void * pv) throw()
{
    rtMemFree("delete[]", RTMEMTYPE_DELETE_ARRAY, pv, ((void **)&pv)[-1], 0, NULL, NULL);
}


void RT_EF_CDECL operator delete[](void *pv, const std::nothrow_t &) throw()
{
    rtMemFree("delete[] nothrow", RTMEMTYPE_DELETE_ARRAY, pv, ((void **)&pv)[-1], 0, NULL, NULL);
}

#endif /* RTALLOC_EFENCE_CPP */
