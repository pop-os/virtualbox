/* $Id: tst-3-driver.c 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kLdr - Dynamic Loader testcase no. 3, Driver.
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
 * kStuff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with kStuff; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "tst.h"
#include <k/kErr.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
# include <malloc.h>
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/** Select the appropriate KLDRSYMKIND bit define. */
#define MY_KLDRSYMKIND_BITS     ( sizeof(void *) == 4 ? KLDRSYMKIND_32BIT : KLDRSYMKIND_64BIT )


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** The numbers of errors. */
static int g_cErrors = 0;


/**
 * Report failure.
 */
static int Failure(const char *pszFormat, ...)
{
    va_list va;

    g_cErrors++;

    printf("tst-3-driver: ");
    va_start(va, pszFormat);
    vprintf(pszFormat, va);
    va_end(va);
    printf("\n");
    return 1;
}


/**
 * External symbol used by the testcase module.
 */
static int Tst3Ext(int iFortyTwo)
{
    if (iFortyTwo != 42)
        return 256;
    return 42;
}


/**
 * Callback for resolving the Tst3Ext import.
 */
static int GetImport(PKLDRMOD pMod, KU32 iImport, KU32 iSymbol, const char *pchSymbol, KSIZE cchSymbol,
                     const char *pszVersion, PKLDRADDR puValue, KU32 *pfKind, void *pvUser)
{
    if (*pfKind != KLDRSYMKIND_REQ_FLAT)
        return -1;

    if (    !strncmp(pchSymbol, "Tst3Ext", strlen("Tst3Ext"))
        ||  !strncmp(pchSymbol, "_Tst3Ext", strlen("_Tst3Ext")))
    {
        *puValue = (KUPTR)&Tst3Ext;
        *pfKind = KLDRSYMKIND_CODE | (sizeof(pfKind) == 4 ? KLDRSYMKIND_32BIT : KLDRSYMKIND_64BIT);
        return 0;
    }

    return -2;
}


/**
 * Performs the tests on one module.
 * @returns non sense.
 */
int TestModule(const char *pszFile)
{
    PKLDRMOD pMod;
    KLDRSIZE cbImage;
    void *pvBits;
    int rc;

    printf("tst-3-driver: testing '%s'...\n", pszFile);

    /* open it. */
    rc = kLdrModOpen(pszFile, &pMod);
    if (rc)
        return Failure("kLdrModOpen(%s,) -> %#d (%s)\n", pszFile, rc, kErrName(rc));

    /* get bits. */
    cbImage = kLdrModSize(pMod);
    pvBits = malloc((KSIZE)cbImage + 0xfff);
    if (pvBits)
    {
        void *pvBits2 = (void *)( ((KUPTR)pvBits + 0xfff) & ~(KUPTR)0xfff );

        KLDRADDR BaseAddress = (KUPTR)pvBits2;
        rc = kLdrModGetBits(pMod, pvBits2, BaseAddress, GetImport, NULL);
        if (!rc)
        {
            KLDRADDR EntryPoint;

            /* call into it */
            rc = kLdrModQuerySymbol(pMod, pvBits2, BaseAddress, NIL_KLDRMOD_SYM_ORDINAL, "_Tst3", strlen("_Tst3"), NULL, NULL, NULL,
                                    &EntryPoint, NULL);
            if (rc == KLDR_ERR_SYMBOL_NOT_FOUND)
                rc = kLdrModQuerySymbol(pMod, pvBits2, BaseAddress, NIL_KLDRMOD_SYM_ORDINAL, "Tst3", strlen("Tst3"), NULL, NULL, NULL,
                                        &EntryPoint, NULL);
            if (!rc)
            {
                int (*pfnEntryPoint)(int) = (int (*)(int)) ((KUPTR)EntryPoint);
                rc = pfnEntryPoint(42);
                if (rc == 42)
                {
                    /* relocate twice and try again. */
                    rc = kLdrModRelocateBits(pMod, pvBits2, BaseAddress + 0x22000, BaseAddress, GetImport, NULL);
                    if (!rc)
                    {
                        rc = kLdrModRelocateBits(pMod, pvBits2, BaseAddress, BaseAddress + 0x22000, GetImport, NULL);
                        if (!rc)
                        {
                            rc = pfnEntryPoint(42);
                            if (rc == 42)
                            {
                                printf("tst-3-driver: success.\n");
                            }
                            else
                                Failure("pfnEntryPoint(42) -> %d (2nd)\n", rc);
                        }
                        else
                            Failure("kLdrModRelocateBits(,,, + 0x22000,,,) -> %#x (%s)\n", rc, kErrName(rc));
                    }
                    else
                        Failure("kLdrModRelocateBits(,, + 0x22000,,,,) -> %#x (%s)\n", rc, kErrName(rc));
                }
                else
                    Failure("pfnEntryPoint(42) -> %d (1st)\n", rc);
            }
            else
                Failure("kLdrModQuerySymbol -> %#x (%s)\n", rc, kErrName(rc));
        }
        else
            Failure("kLdrModGetBits -> %#x (%s)\n", rc, kErrName(rc));
        free(pvBits);
    }
    else
        Failure("malloc(%lx) -> NULL\n", (long)cbImage);

    /* clean up */
    rc = kLdrModClose(pMod);
    if (rc)
        Failure("kLdrModOpen(%s,) -> %#x (%s)\n", pszFile, rc, kErrName(rc));
    return 0;
}


int main(int argc, char **argv)
{
    int i;

    /*
     * Test all the given modules (requires arguments).
     */
    for (i = 1; i < argc; i++)
    {
        TestModule(argv[i]);
    }


    /*
     * Summary
     */
    if (!g_cErrors)
        printf("tst-3-driver: SUCCESS\n");
    else
        printf("tst-3-driver: FAILURE - %d errors\n", g_cErrors);
    return !!g_cErrors;
}
