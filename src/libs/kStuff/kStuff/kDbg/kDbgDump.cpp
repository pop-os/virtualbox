/* $Id: kDbgDump.cpp 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kDbgDump - Debug Info Dumper.
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
#include <string.h>
#include <stdio.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** @name Options
 * @{ */
static int g_fGlobalSyms = 1;
static int g_fPrivateSyms = 1;
static int g_fLineNumbers = 0;
/** @} */


/**
 * Dumps one file.
 *
 * @returns main exit status.
 * @param   pszFile     The file to dump (path to it).
 */
static int DumpFile(const char *pszFile)
{
    PKDBGMOD pDbgMod;
    int rc = kDbgModuleOpen(&pDbgMod, pszFile, NULL);
    if (rc)
    {
        printf("kDbgDump: error: kDbgModuleOpen('%s',) failed with rc=%d.\n", pszFile, rc);
        return 1;
    }



    return 0;
}


/**
 * Prints the version number
 * @return 0
 */
static int ShowVersion()
{
    printf("kDbgDump v0.0.1\n");
    return 0;
}


/**
 * Prints the program syntax.
 *
 * @returns 1
 * @param   argv0   The program name.
 */
static int ShowSyntax(const char *argv0)
{
    ShowVersion();
    printf("syntax: %s [options] <files>\n"
           "\n",
           argv0);
    return 1;
}

int main(int argc, char **argv)
{
    int rcRet = 0;

    /*
     * Parse arguments.
     */
    int fArgsDone = 0;
    for (int i = 1; i < argc; i++)
    {
        const char *psz = argv[i];

        if (!fArgsDone && psz[0] == '-' && psz[1])
        {
            /* convert long option to short. */
            if (*++psz == '-')
            {
                psz++;
                if (!*psz) /* -- */
                {
                    fArgsDone = 1;
                    continue;
                }
                if (!strcmp(psz, "line-numbers"))
                    psz = "l";
                else if (!strcmp(psz, "no-line-numbers"))
                    psz = "L";
                else if (!strcmp(psz, "global-syms")    || !strcmp(psz, "public-syms"))
                    psz = "g";
                else if (!strcmp(psz, "no-global-syms") || !strcmp(psz, "no-public-syms"))
                    psz = "G";
                else if (!strcmp(psz, "privat-syms")    || !strcmp(psz, "local-syms"))
                    psz = "p";
                else if (!strcmp(psz, "no-privat-syms") || !strcmp(psz, "no-local-syms"))
                    psz = "P";
                else if (!strcmp(psz, "version"))
                    psz = "v";
                else if (!strcmp(psz, "help"))
                    psz = "h";
                else
                {
                    fprintf(stderr, "%s: syntax error: unknown option '--%s'\n", argv[0], psz);
                    return 1;
                }
            }

            /* eat short options. */
            while (*psz)
                switch (*psz++)
                {
                    case 'l': g_fLineNumbers = 1; break;
                    case 'L': g_fLineNumbers = 0; break;
                    case 'p': g_fPrivateSyms = 1; break;
                    case 'P': g_fPrivateSyms = 0; break;
                    case 'g': g_fGlobalSyms = 1; break;
                    case 'G': g_fGlobalSyms = 0; break;
                    case '?':
                    case 'H':
                    case 'h': return ShowSyntax(argv[0]);
                    case 'v': return ShowVersion();
                    default:
                        fprintf(stderr, "%s: syntax error: unknown option '-%c'.\n", argv[0], psz[-1]);
                        return 1;
                }
        }
        else
        {
            /* Dump does it's own bitching if something goes wrong. */
            int rc = DumpFile(psz);
            if (rc && !rcRet)
                rc = rcRet;
        }
    }

    return rcRet;
}

