/* $Id: kDbgHlpCrt.cpp 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kDbg - The Debug Info Reader, Helpers, CRT Based Implementation.
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
#include "kDbgHlp.h"
#include "kDbg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _MSC_VER
# include <io.h>
#endif



/**
 * The stdio base implementation of KDBGHLPFILE.
 */
typedef struct KDBGHLPFILE
{
    /** Pointer to the stdio file stream. */
    FILE *pStrm;
} KDBGHLPFILE;

/** @def HAVE_FSEEKO
 * Define HAVE_FSEEKO to indicate that fseeko and ftello should be used. */
#if !defined(_MSC_VER)
# define HAVE_FSEEKO
#endif


void *kDbgHlpAlloc(size_t cb)
{
    return malloc(cb);
}


void *kDbgHlpAllocZ(size_t cb)
{
    return calloc(1, cb);
}


void *kDbgHlpAllocDup(const void *pv, size_t cb)
{
    void *pvNew = malloc(cb);
    if (pvNew)
        memcpy(pvNew, pv, cb);
    return pvNew;
}


void *kDbgHlpReAlloc(void *pv, size_t cb)
{
    return realloc(pv, cb);
}


void kDbgHlpFree(void *pv)
{
    free(pv);
}


int kDbgHlpCrtConvErrno(int rc)
{
    switch (rc)
    {
        case 0:             return 0;
        case EINVAL:        return KERR_INVALID_PARAMETER;
        case ENOMEM:        return KERR_NO_MEMORY;
        case EISDIR:
        case ENOENT:        return KERR_FILE_NOT_FOUND;
        default:            return KERR_GENERAL_FAILURE;
    }
}


int kDbgHlpOpenRO(const char *pszFilename, PKDBGHLPFILE *ppFile)
{
    PKDBGHLPFILE pFile = (PKDBGHLPFILE)kDbgHlpAlloc(sizeof(*pFile));
    if (!pFile)
        return KERR_NO_MEMORY;

    pFile->pStrm = fopen(pszFilename, "rb");
    if (pFile->pStrm)
    {
        *ppFile = pFile;
        return 0;
    }
    return kDbgHlpCrtConvErrno(errno);
}


void kDbgHlpClose(PKDBGHLPFILE pFile)
{
    if (pFile)
    {
        fclose(pFile->pStrm);
        pFile->pStrm = NULL;
        kDbgHlpFree(pFile);
    }
}


uintptr_t kDbgHlpNativeFileHandle(PKDBGHLPFILE pFile)
{
    int fd = fileno(pFile->pStrm);
#ifdef _MSC_VER
    return _get_osfhandle(fd);
#else
    return fd;
#endif
}


int64_t kDbgHlpFileSize(PKDBGHLPFILE pFile)
{
    int64_t cbFile;
    int64_t offCur = kDbgHlpTell(pFile);
    if (offCur >= 0)
    {
        if (kDbgHlpSeekByEnd(pFile, 0) == 0)
            cbFile = kDbgHlpTell(pFile);
        else
            cbFile = -1;
        kDbgHlpSeek(pFile, offCur);
    }
    return cbFile;
}


int kDbgHlpReadAt(PKDBGHLPFILE pFile, int64_t off, void *pv, size_t cb)
{
    int rc  = kDbgHlpSeek(pFile, off);
    if (!rc)
        rc = kDbgHlpRead(pFile, pv, cb);
    return rc;
}


int kDbgHlpRead(PKDBGHLPFILE pFile, void *pv, size_t cb)
{
    if (fread(pv, cb, 1, pFile->pStrm) == 1)
        return 0;
    return -1;
}


int kDbgHlpSeek(PKDBGHLPFILE pFile, int64_t off)
{
#ifdef HAVE_FSEEKO
    if (!fseeko(pFile->pStrm, off, SEEK_SET))
        return 0;
#else
    long l = (long)off;
    if (l != off)
        return KERR_OUT_OF_RANGE;
    if (!fseek(pFile->pStrm, l, SEEK_SET))
        return 0;
#endif
    return kDbgHlpCrtConvErrno(errno);
}


int kDbgHlpSeekByCur(PKDBGHLPFILE pFile, int64_t off)
{
#ifdef HAVE_FSEEKO
    if (!fseeko(pFile->pStrm, off, SEEK_CUR))
        return 0;
#else
    long l = (long)off;
    if (l != off)
        return KERR_OUT_OF_RANGE;
    if (!fseek(pFile->pStrm, l, SEEK_CUR))
        return 0;
#endif
    return kDbgHlpCrtConvErrno(errno);
}


int kDbgHlpSeekByEnd(PKDBGHLPFILE pFile, int64_t off)
{
#ifdef HAVE_FSEEKO
    if (!fseeko(pFile->pStrm, -off, SEEK_END))
        return 0;
#else
    long l = (long)off;
    if (l != off)
        return KERR_OUT_OF_RANGE;
    if (!fseek(pFile->pStrm, -l, SEEK_END))
        return 0;
#endif
    return kDbgHlpCrtConvErrno(errno);
}


int64_t kDbgHlpTell(PKDBGHLPFILE pFile)
{
#ifdef HAVE_FSEEKO
    return ftello(pFile->pStrm);
#else
    return ftell(pFile->pStrm);
#endif
}

