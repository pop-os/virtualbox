/* $Id: kHlpBareThread.c 24 2009-02-08 13:58:54Z bird $ */
/** @file
 * kHlpBare - Thread Manipulation.
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
#include <k/kHlpThread.h>

#if K_OS == K_OS_DARWIN
# include <mach/mach_time.h>

#elif K_OS == K_OS_LINUX
# include <k/kHlpSys.h>

#elif K_OS == K_OS_OS2
# define INCL_BASE
# define INCL_ERRORS
# include <os2.h>
#elif  K_OS == K_OS_WINDOWS
# include <Windows.h>
#else
# error "port me"
#endif


/**
 * Sleep for a number of milliseconds.
 * @param   cMillies    Number of milliseconds to sleep.
 */
void kHlpSleep(unsigned cMillies)
{
#if K_OS == K_OS_DARWIN
    static struct mach_timebase_info   s_Info;
    static KBOOL                s_fNanoseconds = K_UNKNOWN;
    KU64 uNow = mach_absolute_time();
    KU64 uDeadline;
    KU64 uPeriod;

    if (s_fNanoseconds == K_UNKNOWN)
    {
        if (mach_timebase_info(&s_Info))
            s_fNanoseconds = K_TRUE; /* the easy way out */
        else if (s_Info.denom == s_Info.numer)
            s_fNanoseconds = K_TRUE;
        else
            s_fNanoseconds = K_FALSE;
    }

    uPeriod = (KU64)cMillies * 1000 * 1000;
    if (!s_fNanoseconds)
        uPeriod = (double)uPeriod * s_Info.denom / s_Info.numer; /* Use double to avoid 32-bit trouble. */
    uDeadline = uNow + uPeriod;
    mach_wait_until(uDeadline);

#elif K_OS == K_OS_LINUX
    /** @todo find the right syscall... */

#elif K_OS == K_OS_OS2
    DosSleep(cMillies);
#elif  K_OS == K_OS_WINDOWS
    Sleep(cMillies);
#else
    usleep(cMillies * 1000);
#endif
}

