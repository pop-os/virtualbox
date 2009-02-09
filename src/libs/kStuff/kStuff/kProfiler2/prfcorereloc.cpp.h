/* $Id: prfcorereloc.cpp.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kProfiler Mark 2 - Core SetBasePtr Code Template.
 */

/*
 * Copyright (c) 2006-2007 knut st. osmundsen <bird-src-spam@anduin.net>
 *
 * This file is part of kProfiler.
 *
 * kProfiler is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * kProfiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with kProfiler; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


/**
 * Set (or modify) the base pointer for the profiler.
 *
 * The purpose of the base pointer is to allow profiling of relocatable code. Set the
 * base pointer right after initializing the data set, and update it when relocating
 * the code (both by calling this function), and Bob's your uncle! :-)
 *
 * @param   pHdr        The header returned from the initializer.
 * @param   uBasePtr    The new base pointer value.
 */
KPRF_DECL_FUNC(void, SetBasePtr)(KPRF_TYPE(P,HDR) pHdr, KPRF_TYPE(,UPTR) uBasePtr)
{
    pHdr->uBasePtr = uBasePtr;
}


