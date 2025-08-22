/* $Id: svn2git.cpp $ */
/** @file
 * svn2git - Convert a svn repository to git.
 */

/*
 * Copyright (C) 2025 Oracle and/or its affiliates.
 *
 * This file is part of VirtualBox base platform packages, as
 * available from https://www.virtualbox.org.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, in version 3 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */


/*********************************************************************************************************************************
*   Header Files                                                                                                                 *
*********************************************************************************************************************************/
#include <iprt/dir.h>
#include <iprt/err.h>
#include <iprt/file.h>
#include <iprt/getopt.h>
#include <iprt/initterm.h>
#include <iprt/json.h>
#include <iprt/list.h>
#include <iprt/mem.h>
#include <iprt/message.h>
#include <iprt/path.h>
#include <iprt/sha.h>
#include <iprt/string.h>
#include <iprt/stdarg.h>
#include <iprt/stream.h>
#include <iprt/time.h>

#include "libsvn.h"
#include "svn2git-internal.h"


/*********************************************************************************************************************************
*   Structures and Typedefs                                                                                                      *
*********************************************************************************************************************************/

/**
 * Change entry.
 */
typedef struct S2GSVNREVCHANGE
{
    RTLISTNODE            NdChanges;
    const char            *pszPath;
    svn_fs_path_change2_t *pChange;
} S2GSVNREVCHANGE;
typedef S2GSVNREVCHANGE *PS2GSVNREVCHANGE;
typedef const S2GSVNREVCHANGE *PCS2GSVNREVCHANGE;


/**
 * Author entry.
 */
typedef struct S2GAUTHOR
{
    /** String space core, the subversion author is the key. */
    RTSTRSPACECORE  Core;
    /** The matching git author. */
    const char      *pszGitAuthor;
    /** The E-Mail to use for git commits. */
    const char      *pszGitEmail;
    /** The allocated string buffer for everything. */
    RT_GCC_EXTENSION
    char            achStr[RT_FLEXIBLE_ARRAY];
} S2GAUTHOR;
typedef S2GAUTHOR *PS2GAUTHOR;
typedef const S2GAUTHOR *PCS2GAUTHOR;


/**
 * Externals revision to git commit hash map.
 */
typedef struct S2GEXTREVMAP
{
    /** List node. */
    RTLISTNODE      NdExternals;
    /** Name of the external  */
    const char      *pszName;
    /** Number of entries in the map sorted by revision. */
    uint32_t        cEntries;
    /** Pointer to the array to commit ID hash strings, indexed by revision. */
    const char      **papszRev2CommitHash;
    /** The string table for the git commnit hashes. */
    RT_GCC_EXTENSION
    char            achStr[RT_FLEXIBLE_ARRAY];
} S2GEXTREVMAP;
typedef S2GEXTREVMAP *PS2GEXTREVMAP;
typedef const S2GEXTREVMAP *PCS2GEXTREVMAP;


/**
 * A directory entry.
 */
typedef struct S2GDIRENTRY
{
    /** List node. */
    RTLISTNODE      NdDir;
    /** Flag whether the entry is a directory. */
    bool            fIsDir;
    /** Entry name */
    const char      *pszName;
} S2GDIRENTRY;
typedef S2GDIRENTRY *PS2GDIRENTRY;
typedef const S2GDIRENTRY *PCS2GDIRENTRY;


/**
 * svn -> git branch map.
 */
typedef struct S2GBRANCH
{
    /** List node. */
    RTLISTNODE      NdBranches;
    /** The git branch to use. */
    const char      *pszGitBranch;
    /** Size of the prefix in characters. */
    size_t          cchSvnPrefix;
    /** Flag whether the branch was created. */
    bool            fCreated;
    /** The prefix in svn to match. */
    RT_GCC_EXTENSION
    char            achSvnPrefix[RT_FLEXIBLE_ARRAY];
} S2GBRANCH;
typedef S2GBRANCH *PS2GBRANCH;
typedef const S2GBRANCH *PCS2GBRANCH;


/**
 * The state for a single revision.
 */
typedef struct S2GSVNREV
{
    /** The pool for this revision. */
    apr_pool_t      *pPoolRev;
    /** The SVN revision root. */
    svn_fs_root_t   *pSvnFsRoot;

    /** The revision number. */
    uint32_t        idRev;
    /** The Unix epoch in seconds of the svn commit. */
    int64_t         cEpochSecs;
    /** The APR time of the commit (for keyword substitution). */
    apr_time_t      AprTime;
    /** The svn author. */
    const char      *pszSvnAuthor;
    /** The commit message. */
    const char      *pszSvnLog;
    /** The sync-xref-src-repo-rev. */
    const char      *pszSvnXref;

    /** The Git comitters's E-Mail. */
    const char      *pszGitComitterEmail;
    /** The Git comitters's name. */
    const char      *pszGitComitter;

    /** The Git authors's E-Mail (can be NULL, used for merging pull requests to credit people). */
    const char      *pszGitAuthorEmail;
    /** The Git authors's name (can be NULL, used for merging pull requests to credit people). */
    const char      *pszGitAuthor;

    /* The branch this revision operates on. */
    PS2GBRANCH      pBranch;

    /** List of changes in that revision, sorted by the path. */
    RTLISTANCHOR    LstChanges;

    /** The string revision number (for use as svn:sync-xref-src-repo-rev
     * in the commit message if the source repository doesn't has the property). */
    char            szRev[32];
} S2GSVNREV;
/** Pointer to the SVN revision state. */
typedef S2GSVNREV *PS2GSVNREV;
/** Pointer to a const SVN revision state. */
typedef const S2GSVNREV *PCS2GSVNREV;


/**
 * The svn -> git conversion context.
 */
typedef struct S2GCTX
{
    /** The start revision number. */
    uint32_t        idRevStart;
    /** The end revision number. */
    uint32_t        idRevEnd;
    /** The path to the JSON config file. */
    const char      *pszCfgFilename;
    /** The input subversion repository path. */
    const char      *pszSvnRepo;
    /** Dump filename, optional. */
    const char      *pszDumpFilename;

    /** The git repository path. */
    char            *pszGitRepoPath;
    /** The default git branch. */
    const char      *pszGitDefBranch;
    /** The push origin of the git repository if configured. */
    char            *pszGitPushOrigin;

    /** Path to the temporary directory in verification mode. */
    const char      *pszVerifyTmpPath;
    /** Path to the revision map to export in export mode, optional. */
    const char      *pszRevisionMapPath;

    /** Default author if the svn commit has none set. */
    char            *pszAuthorDefault;
    /** Flag whether the svn:sync-xref-src-repo-rev in the
     * source repository is available for being used in the commit message. */
    bool            fSvnSyncXrefSrcRepoRevAvail;
    /** Flag whether to ignore non existing branch mappings. */
    bool            fIgnoreNonExistingBranchMappings;

    /** @name Subversion related members.
     * @{ */
    /** The default pool. */
    apr_pool_t      *pPoolDefault;
    /** The scratch pool. */
    apr_pool_t      *pPoolScratch;
    /** The repository handle. */
    svn_repos_t     *pSvnRepos;
    /** The filesystem layer handle for the repository. */
    svn_fs_t        *pSvnFs;
    /** @} */

    /** @name Git repository related members.
     * @{ */
    /** The destination git repository. */
    S2GREPOSITORYGIT hGitRepo;
    /** @} */

    /** SVN -> Git author information string space. */
    RTSTRSPACE       StrSpaceAuthors;
    /** Scratch buffer. */
    S2GSCRATCHBUF    BufScratch;
    /** List of known externals with their revision to git commit hash map. */
    RTLISTANCHOR     LstExternals;
    /** List of known branches. */
    RTLISTANCHOR     LstBranches;
} S2GCTX;
/** Pointer to an svn -> git conversion context. */
typedef S2GCTX *PS2GCTX;
/** Pointer to a const svn -> git conversion context. */
typedef const S2GCTX *PCS2GCTX;


/*********************************************************************************************************************************
*   Global Variables                                                                                                             *
*********************************************************************************************************************************/
/** Verbosity level. */
static int        g_cVerbosity = 0;
/** Working directory. */
static const char *g_pszWorkingDir = NULL;
/** Temporary storage for a path. */
static char       g_szPath[RTPATH_MAX + 1];


static void s2gFilenameToPath(const char **ppszPath, const char *pszFilename)
{
    if (   RTPathStartsWithRoot(pszFilename)
        || !g_pszWorkingDir)
        RTStrCopy(g_szPath, sizeof(g_szPath), pszFilename);
    else
    {
        /* Build the complete path. */
        RTStrCopy(g_szPath, sizeof(g_szPath), g_pszWorkingDir);
        RTStrCat(g_szPath, sizeof(g_szPath), RTPATH_SLASH_STR);
        RTStrCat(g_szPath, sizeof(g_szPath), pszFilename);
    }

    *ppszPath = &g_szPath[0];
}


/**
 * Display usage
 *
 * @returns success if stdout, syntax error if stderr.
 */
static RTEXITCODE s2gUsage(const char *argv0)
{
    RTMsgInfo("usage: %s --config <config file> [options and operations] <input subversion repository>\n"
              "\n"
              "Operations and Options (processed in place):\n"
              "  --verbose                                Noisier.\n"
              "  --quiet                                  Quiet execution.\n"
              "  --working-directory <path>               The base directory to work from, all relative paths are prepended with this when opening files.\n"
              "  --rev-start <revision>                   The revision to start conversion at\n"
              "  --rev-end   <revision>                   The last revision to convert (default is last repository revision)\n"
              "  --dump-file <file path>                  File to dump the fast-import stream to\n"
              "  --verify-result <tmp path>               Verify SVN and git repository for the given revisions,\n"
              "                                           takes a path to temporarily create a worktree for the git repository\n"
              "  --export-revision-map <path>             Exports the SVN revision to commit hash map for the repository\n"
              , argv0);
    return RTEXITCODE_SUCCESS;
}


/**
 * Initializes the given context to default values.
 */
static void s2gCtxInit(PS2GCTX pThis)
{
    pThis->idRevStart                       = UINT32_MAX;
    pThis->idRevEnd                         = UINT32_MAX;
    pThis->pszCfgFilename                   = NULL;
    pThis->pszSvnRepo                       = NULL;
    pThis->pszGitRepoPath                   = NULL;
    pThis->pszGitDefBranch                  = "main";
    pThis->pszGitPushOrigin                 = NULL;
    pThis->pszDumpFilename                  = NULL;
    pThis->pszVerifyTmpPath                 = NULL;
    pThis->pszRevisionMapPath               = NULL;
    pThis->pszAuthorDefault                 = NULL;
    pThis->fSvnSyncXrefSrcRepoRevAvail      = false;
    pThis->fIgnoreNonExistingBranchMappings = false;

    pThis->pPoolDefault     = NULL;
    pThis->pPoolScratch     = NULL;
    pThis->pSvnRepos        = NULL;
    pThis->pSvnFs           = NULL;

    pThis->StrSpaceAuthors  = NULL;
    s2gScratchBufInit(&pThis->BufScratch);
    RTListInit(&pThis->LstExternals);
    RTListInit(&pThis->LstBranches);
}


static void s2gCtxDestroy(PS2GCTX pThis)
{
    if (pThis->pPoolScratch)
    {
        svn_pool_destroy(pThis->pPoolScratch);
        pThis->pPoolScratch = NULL;
    }

    if (pThis->pPoolDefault)
    {
        svn_pool_destroy(pThis->pPoolDefault);
        pThis->pPoolDefault = NULL;
    }

    s2gScratchBufFree(&pThis->BufScratch);
}


/**
 * Parses the arguments.
 */
static RTEXITCODE s2gParseArguments(PS2GCTX pThis, int argc, char **argv)
{
    /*
     * Option config.
     */
    static RTGETOPTDEF const s_aOpts[] =
    {
        { "--config",                           'c', RTGETOPT_REQ_STRING  },
        { "--verbose",                          'v', RTGETOPT_REQ_NOTHING },
        { "--working-directory",                'w', RTGETOPT_REQ_STRING  },
        { "--rev-start",                        's', RTGETOPT_REQ_UINT32  },
        { "--rev-end",                          'e', RTGETOPT_REQ_UINT32  },
        { "--dump-file",                        'd', RTGETOPT_REQ_STRING  },
        { "--verify-result",                    'y', RTGETOPT_REQ_STRING  },
        { "--export-revision-map",              'm', RTGETOPT_REQ_STRING  },
    };

    RTGETOPTUNION   ValueUnion;
    RTGETOPTSTATE   GetOptState;
    int rc = RTGetOptInit(&GetOptState, argc, argv, &s_aOpts[0], RT_ELEMENTS(s_aOpts), 1, RTGETOPTINIT_FLAGS_OPTS_FIRST);
    AssertReleaseRCReturn(rc, RTEXITCODE_FAILURE);

    /*
     * Process the options.
     */
    while ((rc = RTGetOpt(&GetOptState, &ValueUnion)) != 0)
    {
        switch (rc)
        {
            case 'h':
                return s2gUsage(argv[0]);

            case 'c':
                if (pThis->pszCfgFilename)
                    return RTMsgErrorExit(RTEXITCODE_SYNTAX, "Config file is already set to '%s'", pThis->pszCfgFilename);
                pThis->pszCfgFilename = ValueUnion.psz;
                break;

            case 'v':
                g_cVerbosity++;
                break;

            case 'w':
                g_pszWorkingDir = ValueUnion.psz;
                break;

            case 's':
                pThis->idRevStart = ValueUnion.u32;
                break;

            case 'e':
                pThis->idRevEnd = ValueUnion.u32;
                break;

            case 'd':
                pThis->pszDumpFilename = ValueUnion.psz;
                break;

            case 'y':
                pThis->pszVerifyTmpPath = ValueUnion.psz;
                break;

            case 'm':
                pThis->pszRevisionMapPath = ValueUnion.psz;
                break;

            case 'V':
            {
                /* The following is assuming that svn does it's job here. */
                static const char s_szRev[] = "$Revision: 170013 $";
                const char *psz = RTStrStripL(strchr(s_szRev, ' '));
                RTMsgInfo("r%.*s\n", strchr(psz, ' ') - psz, psz);
                return RTEXITCODE_SUCCESS;
            }

            case VINF_GETOPT_NOT_OPTION:
                if (pThis->pszSvnRepo)
                    return RTMsgErrorExit(RTEXITCODE_SYNTAX, "Subversion path is already set to '%s'", pThis->pszSvnRepo);
                pThis->pszSvnRepo = ValueUnion.psz;
                break;

            /*
             * Errors and bugs.
             */
            default:
                return RTGetOptPrintError(rc, &ValueUnion);
        }
    }

    /*
     * Check that we've got all we need.
     */
    if (!pThis->pszCfgFilename)
        return RTMsgErrorExit(RTEXITCODE_SYNTAX, "Missing --config <filename> argument");
    return RTEXITCODE_SUCCESS;
}


static RTEXITCODE s2gLoadConfigAuthor(PS2GCTX pThis, RTJSONVAL hAuthor)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    RTJSONVAL hValSvnAuthor = NIL_RTJSONVAL;
    int rc = RTJsonValueQueryByName(hAuthor, "svn", &hValSvnAuthor);
    if (RT_SUCCESS(rc))
    {
        RTJSONVAL hValGitAuthor = NIL_RTJSONVAL;
        rc = RTJsonValueQueryByName(hAuthor, "git", &hValGitAuthor);
        if (RT_SUCCESS(rc))
        {
            RTJSONVAL hValGitEmail = NIL_RTJSONVAL;
            rc = RTJsonValueQueryByName(hAuthor, "email", &hValGitEmail);
            if (RT_SUCCESS(rc))
            {
                const char *pszSvnAuthor = RTJsonValueGetString(hValSvnAuthor);
                const char *pszGitAuthor = RTJsonValueGetString(hValGitAuthor);
                const char *pszGitEmail  = RTJsonValueGetString(hValGitEmail);

                if (pszSvnAuthor && pszGitAuthor && pszGitEmail)
                {
                    size_t cchSvnAuthor = strlen(pszSvnAuthor);
                    size_t cchGitAuthor = strlen(pszGitAuthor);
                    size_t cchGitEmail  = strlen(pszGitEmail);
                    size_t cbAuthor = RT_UOFFSETOF_DYN(S2GAUTHOR, achStr[cchSvnAuthor + cchGitAuthor + cchGitEmail + 3]);
                    PS2GAUTHOR pAuthor = (PS2GAUTHOR)RTMemAllocZ(cbAuthor);
                    if (pAuthor)
                    {
                        memcpy(&pAuthor->achStr[0], pszSvnAuthor, cchSvnAuthor);
                        pAuthor->achStr[cchSvnAuthor] = '\0';
                        pAuthor->Core.pszString = &pAuthor->achStr[0];
                        pAuthor->Core.cchString = cchSvnAuthor;

                        pAuthor->pszGitAuthor = &pAuthor->achStr[cchSvnAuthor + 1];
                        memcpy(&pAuthor->achStr[cchSvnAuthor + 1], pszGitAuthor, cchGitAuthor);
                        pAuthor->achStr[cchSvnAuthor + 1 + cchGitAuthor] = '\0';

                        pAuthor->pszGitEmail = &pAuthor->pszGitAuthor[cchGitAuthor + 1];
                        memcpy(&pAuthor->achStr[cchSvnAuthor + 1 + cchGitAuthor + 1], pszGitEmail, cchGitEmail);
                        pAuthor->achStr[cchSvnAuthor + 1 + cchGitAuthor + 1 + cchGitEmail] = '\0';

                        if (!RTStrSpaceInsert(&pThis->StrSpaceAuthors, &pAuthor->Core))
                        {
                            RTMemFree(pAuthor);
                            rcExit  = RTMsgErrorExit(RTEXITCODE_FAILURE, "Duplicate author '%s'", pszSvnAuthor);
                        }
                        else if (g_cVerbosity >= 3)
                            RTMsgInfo(" Author map: %s %s %s\n", pAuthor->Core.pszString, pAuthor->pszGitAuthor, pAuthor->pszGitEmail);
                    }
                    else
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate %zu bytes for author '%s'", cbAuthor, pszSvnAuthor);
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "The author object is malformed");

                RTJsonValueRelease(hValGitEmail);
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query 'email' from author object: %Rrc", rc);

            RTJsonValueRelease(hValGitAuthor);
        }
        else
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query 'git' from author object: %Rrc", rc);

        RTJsonValueRelease(hValSvnAuthor);
    }
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query 'svn' from author object: %Rrc", rc);

    return rcExit;
}


static RTEXITCODE s2gLoadConfigAuthorMap(PS2GCTX pThis, RTJSONVAL hJsonValAuthorMap)
{
    if (RTJsonValueGetType(hJsonValAuthorMap) != RTJSONVALTYPE_ARRAY)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "'AuthorMap' in '%s' is not a JSON array", pThis->pszCfgFilename);

    RTJSONIT hIt = NIL_RTJSONIT;
    int rc = RTJsonIteratorBeginArray(hJsonValAuthorMap, &hIt);
    if (rc == VERR_JSON_IS_EMPTY) /* Weird but okay. */
        return RTEXITCODE_SUCCESS;
    else if (RT_FAILURE(rc))
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over author map: %Rrc", rc);

    AssertRC(rc);
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    for (;;)
    {
        RTJSONVAL hAuthor = NIL_RTJSONVAL;
        rc = RTJsonIteratorQueryValue(hIt, &hAuthor,  NULL /*ppszName*/);
        if (RT_FAILURE(rc))
        {
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over author map: %Rrc", rc);
            break;
        }

        rcExit = s2gLoadConfigAuthor(pThis, hAuthor);
        RTJsonValueRelease(hAuthor);
        if (rcExit == RTEXITCODE_FAILURE)
            break;

        rc = RTJsonIteratorNext(hIt);
        if (rc == VERR_JSON_ITERATOR_END)
            break;
        else if (RT_FAILURE(rc))
        {
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over author map: %Rrc", rc);
            break;
        }
    }

    RTJsonIteratorFree(hIt);
    return rcExit;
}


static RTEXITCODE s2gLoadConfigExternalMap(PS2GCTX pThis, const char *pszName, const char *pszFilename)
{
    RTJSONVAL hRoot = NIL_RTJSONVAL;
    RTERRINFOSTATIC ErrInfo;
    RTErrInfoInitStatic(&ErrInfo);

    const char *pszPath = NULL;
    s2gFilenameToPath(&pszPath, pszFilename);
    int rc = RTJsonParseFromFile(&hRoot, RTJSON_PARSE_F_JSON5, pszPath, &ErrInfo.Core);
    if (RT_SUCCESS(rc))
    {
        RTEXITCODE rcExit = RTEXITCODE_SUCCESS;

        if (RTJsonValueGetType(hRoot) == RTJSONVALTYPE_OBJECT)
        {
            uint32_t cItems = RTJsonValueGetObjectMemberCount(hRoot);

            size_t cbName = (strlen(pszName) + 1) * sizeof(char);
            size_t cbExternal = RT_UOFFSETOF_DYN(S2GEXTREVMAP, achStr[  cbName
                                                                      + cItems * (RTSHA1_DIGEST_LEN + 1) * sizeof(char)]);
            PS2GEXTREVMAP pExternal = (PS2GEXTREVMAP)RTMemAllocZ(cbExternal);
            if (pExternal)
            {
                size_t offStr = cbName;
                memcpy(&pExternal->achStr[0], pszName, cbName);
                pExternal->pszName = &pExternal->achStr[0];

                pExternal->cEntries = 0;

                RTJSONIT hIt = NIL_RTJSONIT;
                rc = RTJsonIteratorBeginObject(hRoot, &hIt);
                if (RT_SUCCESS(rc))
                {
                    for (;;)
                    {
                        const char *pszShaCommitHash = NULL;
                        RTJSONVAL hRevision = NIL_RTJSONVAL;
                        rc = RTJsonIteratorQueryValue(hIt, &hRevision,  &pszShaCommitHash);
                        if (RT_FAILURE(rc))
                        {
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over revision map: %Rrc", rc);
                            break;
                        }

                        int64_t i64RevNum = 0;
                        rc = RTJsonValueQueryInteger(hRevision, &i64RevNum);
                        if (RT_FAILURE(rc))
                        {
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Revision for '%s' is not a number", pszShaCommitHash);
                            RTJsonValueRelease(hRevision);
                            break;
                        }
                        RTJsonValueRelease(hRevision);

                        if (strlen(pszShaCommitHash) != RTSHA1_DIGEST_LEN)
                        {
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Commit hash '%s' is malformed", pszShaCommitHash);
                            break;
                        }

                        if (i64RevNum < 0)
                        {
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Revision %RI64 for '%s' is negative",
                                                    i64RevNum, pszShaCommitHash);
                            break;
                        }

                        if (i64RevNum >= pExternal->cEntries)
                        {
                            const char **papszNew = (const char **)RTMemReallocZ(pExternal->papszRev2CommitHash,
                                                                                 pExternal->cEntries * sizeof(const char **),
                                                                                 (i64RevNum + 1) * sizeof(const char **));
                            if (!papszNew)
                            {
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate memory for the revision map");
                                break;
                            }

                            pExternal->papszRev2CommitHash = papszNew;
                            pExternal->cEntries            = i64RevNum + 1;
                        }

                        if (pExternal->papszRev2CommitHash[i64RevNum] != NULL)
                        {
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Revision %RI64 for '%s' is already used",
                                                    i64RevNum, pszShaCommitHash);
                            break;
                        }

                        pExternal->papszRev2CommitHash[i64RevNum] = &pExternal->achStr[offStr];
                        memcpy(&pExternal->achStr[offStr], pszShaCommitHash, RTSHA1_DIGEST_LEN + 1);
                        offStr += RTSHA1_DIGEST_LEN + 1;

                        rc = RTJsonIteratorNext(hIt);
                        if (rc == VERR_JSON_ITERATOR_END)
                            break;
                        else if (RT_FAILURE(rc))
                        {
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over revision map: %Rrc", rc);
                            break;
                        }
                    }
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over revision map: %Rrc", rc);

                if (rcExit == RTEXITCODE_SUCCESS)
                    RTListAppend(&pThis->LstExternals, &pExternal->NdExternals);
                else
                {
                    if (pExternal->papszRev2CommitHash)
                        RTMemFree(pExternal->papszRev2CommitHash);
                    RTMemFree(pExternal);
                }
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate %zu bytes for the external '%s'",
                                        cbExternal, pszFilename);
        }
        else
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "External map '%s' is not a JSON object", pszFilename);

        RTJsonValueRelease(hRoot);
        return rcExit;
    }

    return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to load external map file '%s': %s", pszFilename, ErrInfo.Core.pszMsg);

}


static RTEXITCODE s2gLoadConfigExternal(PS2GCTX pThis, RTJSONVAL hExternal)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    RTJSONVAL hValName = NIL_RTJSONVAL;
    int rc = RTJsonValueQueryByName(hExternal, "name", &hValName);
    if (RT_SUCCESS(rc))
    {
        RTJSONVAL hValFile = NIL_RTJSONVAL;
        rc = RTJsonValueQueryByName(hExternal, "file", &hValFile);
        if (RT_SUCCESS(rc))
        {
            const char *pszName     = RTJsonValueGetString(hValName);
            const char *pszFilename = RTJsonValueGetString(hValFile);
            if (pszName && pszFilename)
                rcExit = s2gLoadConfigExternalMap(pThis, pszName, pszFilename);
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "The external object is malformed");

            RTJsonValueRelease(hValFile);
        }
        else
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query 'file' from external object: %Rrc", rc);

        RTJsonValueRelease(hValName);
    }
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query 'name' from external object: %Rrc", rc);

    return rcExit;
}


static RTEXITCODE s2gLoadConfigExternals(PS2GCTX pThis, RTJSONVAL hJsonValExternals)
{
    if (RTJsonValueGetType(hJsonValExternals) != RTJSONVALTYPE_ARRAY)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "'ExternalsMap' in '%s' is not a JSON array", pThis->pszCfgFilename);

    RTJSONIT hIt = NIL_RTJSONIT;
    int rc = RTJsonIteratorBeginArray(hJsonValExternals, &hIt);
    if (rc == VERR_JSON_IS_EMPTY) /* Weird but okay. */
        return RTEXITCODE_SUCCESS;
    else if (RT_FAILURE(rc))
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over externals map: %Rrc", rc);

    AssertRC(rc);
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    for (;;)
    {
        RTJSONVAL hExternal = NIL_RTJSONVAL;
        rc = RTJsonIteratorQueryValue(hIt, &hExternal,  NULL /*ppszName*/);
        if (RT_FAILURE(rc))
        {
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over externals map: %Rrc", rc);
            break;
        }

        rcExit = s2gLoadConfigExternal(pThis, hExternal);
        RTJsonValueRelease(hExternal);
        if (rcExit == RTEXITCODE_FAILURE)
            break;

        rc = RTJsonIteratorNext(hIt);
        if (rc == VERR_JSON_ITERATOR_END)
            break;
        else if (RT_FAILURE(rc))
        {
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over externals map: %Rrc", rc);
            break;
        }
    }

    RTJsonIteratorFree(hIt);
    return rcExit;
}


static RTEXITCODE s2gLoadConfigBranch(PS2GCTX pThis, RTJSONVAL hBranch)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    RTJSONVAL hValSvnPrefix = NIL_RTJSONVAL;
    int rc = RTJsonValueQueryByName(hBranch, "svn", &hValSvnPrefix);
    if (RT_SUCCESS(rc))
    {
        RTJSONVAL hValGitBranch = NIL_RTJSONVAL;
        rc = RTJsonValueQueryByName(hBranch, "git", &hValGitBranch);
        if (RT_SUCCESS(rc))
        {
            const char *pszSvnPrefix = RTJsonValueGetString(hValSvnPrefix);
            const char *pszGitBranch = RTJsonValueGetString(hValGitBranch);
            if (pszSvnPrefix && pszGitBranch)
            {
                size_t const cchSvnPrefix = strlen(pszSvnPrefix);
                size_t const cbGitBranch  = (strlen(pszGitBranch) + 1) * sizeof(char);
                size_t const cbString   =   cchSvnPrefix * sizeof(char)
                                          + cbGitBranch;
                size_t const cbBranch = RT_UOFFSETOF_DYN(S2GBRANCH, achSvnPrefix[cbString]);
                PS2GBRANCH pBranch = (PS2GBRANCH)RTMemAllocZ(cbBranch);
                if (pBranch)
                {
                    pBranch->cchSvnPrefix = cchSvnPrefix;
                    pBranch->pszGitBranch = &pBranch->achSvnPrefix[cchSvnPrefix];
                    memcpy(&pBranch->achSvnPrefix[cchSvnPrefix], pszGitBranch, cbGitBranch);
                    memcpy(&pBranch->achSvnPrefix[0], pszSvnPrefix, cchSvnPrefix);
                    RTListAppend(&pThis->LstBranches, &pBranch->NdBranches);
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate %zu bytes of memory for branch '%s'",
                                            cbBranch, pszSvnPrefix);
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "The branch object is malformed");

            RTJsonValueRelease(hValGitBranch);
        }
        else
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query 'git' from external object: %Rrc", rc);

        RTJsonValueRelease(hValSvnPrefix);
    }
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query 'svn' from external object: %Rrc", rc);

    return rcExit;
}


static RTEXITCODE s2gLoadConfigBranches(PS2GCTX pThis, RTJSONVAL hJsonValBranches)
{
    if (RTJsonValueGetType(hJsonValBranches) != RTJSONVALTYPE_ARRAY)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "'BranchMap' in '%s' is not a JSON array", pThis->pszCfgFilename);

    RTJSONIT hIt = NIL_RTJSONIT;
    int rc = RTJsonIteratorBeginArray(hJsonValBranches, &hIt);
    if (rc == VERR_JSON_IS_EMPTY) /* Weird but okay. */
        return RTEXITCODE_SUCCESS;
    else if (RT_FAILURE(rc))
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over branch map: %Rrc", rc);

    AssertRC(rc);
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    for (;;)
    {
        RTJSONVAL hBranch = NIL_RTJSONVAL;
        rc = RTJsonIteratorQueryValue(hIt, &hBranch,  NULL /*ppszName*/);
        if (RT_FAILURE(rc))
        {
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over branch map: %Rrc", rc);
            break;
        }

        rcExit = s2gLoadConfigBranch(pThis, hBranch);
        RTJsonValueRelease(hBranch);
        if (rcExit == RTEXITCODE_FAILURE)
            break;

        rc = RTJsonIteratorNext(hIt);
        if (rc == VERR_JSON_ITERATOR_END)
            break;
        else if (RT_FAILURE(rc))
        {
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to iterate over branch map: %Rrc", rc);
            break;
        }
    }

    RTJsonIteratorFree(hIt);
    return rcExit;
}


static RTEXITCODE s2gLoadConfig(PS2GCTX pThis)
{
    RTJSONVAL hRoot = NIL_RTJSONVAL;
    RTERRINFOSTATIC ErrInfo;
    RTErrInfoInitStatic(&ErrInfo);
    int rc = RTJsonParseFromFile(&hRoot, RTJSON_PARSE_F_JSON5, pThis->pszCfgFilename, &ErrInfo.Core);
    if (RT_SUCCESS(rc))
    {
        RTEXITCODE rcExit = RTEXITCODE_SUCCESS;

        rc = RTJsonValueQueryStringByName(hRoot, "GitRepoPath", &pThis->pszGitRepoPath);
        if (RT_SUCCESS(rc))
        {
            rc = RTJsonValueQueryBooleanByName(hRoot, "IgnoreNonExistingBranchMappings", &pThis->fIgnoreNonExistingBranchMappings);
            if (RT_SUCCESS(rc) || rc == VERR_NOT_FOUND)
            {
                rc = RTJsonValueQueryBooleanByName(hRoot, "SvnSyncXrefSrcRepoRevAvail", &pThis->fSvnSyncXrefSrcRepoRevAvail);
                if (RT_SUCCESS(rc) || rc == VERR_NOT_FOUND)
                {
                    rc = RTJsonValueQueryStringByName(hRoot, "DefaultSvnAuthor", &pThis->pszAuthorDefault);
                    if (RT_SUCCESS(rc) || rc == VERR_NOT_FOUND)
                    {
                        rc = RTJsonValueQueryStringByName(hRoot, "GitPushOrigin", &pThis->pszGitPushOrigin);
                        if (RT_SUCCESS(rc) || rc == VERR_NOT_FOUND)
                            rc = VINF_SUCCESS;
                        else
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query \"GitPushOrigin\" from '%s': %Rrc", pThis->pszCfgFilename, rc);
                    }
                    else
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query \"DefaultSvnAuthor\" from '%s': %Rrc", pThis->pszCfgFilename, rc);
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query \"SvnSyncXrefSrcRepoRevAvail\" from '%s': %Rrc", pThis->pszCfgFilename, rc);
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query \"IgnoreNonExistingBranchMappings\" from '%s': %Rrc", pThis->pszCfgFilename, rc);

            if (RT_SUCCESS(rc))
            {
                RTJSONVAL hAuthorMap = NIL_RTJSONVAL;
                rc = RTJsonValueQueryByName(hRoot, "AuthorMap", &hAuthorMap);
                if (RT_SUCCESS(rc))
                {
                    rcExit = s2gLoadConfigAuthorMap(pThis, hAuthorMap);
                    RTJsonValueRelease(hAuthorMap);
                    if (rcExit == RTEXITCODE_SUCCESS)
                    {
                        RTJSONVAL hExternalsMap = NIL_RTJSONVAL;
                        rc = RTJsonValueQueryByName(hRoot, "ExternalsMap", &hExternalsMap);
                        if (RT_SUCCESS(rc))
                        {
                            rcExit = s2gLoadConfigExternals(pThis, hExternalsMap);
                            RTJsonValueRelease(hExternalsMap);
                        }
                        else if (rc != VERR_NOT_FOUND)
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query ExternalsMap from '%s': %Rrc",
                                                    pThis->pszCfgFilename, rc);
                        else
                            rc = VINF_SUCCESS;

                        if (RT_SUCCESS(rc))
                        {
                            RTJSONVAL hBranchMap = NIL_RTJSONVAL;
                            rc = RTJsonValueQueryByName(hRoot, "BranchMap", &hBranchMap);
                            if (RT_SUCCESS(rc))
                            {
                                rcExit = s2gLoadConfigBranches(pThis, hBranchMap);
                                RTJsonValueRelease(hBranchMap);
                            }
                            else
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query BranchMap from '%s': %Rrc",
                                                        pThis->pszCfgFilename, rc);
                        }
                    }
                }
                else if (rc != VERR_NOT_FOUND)
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query AuthorMap from '%s': %Rrc", pThis->pszCfgFilename, rc);
            }
        }
        else
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query GitRepoPath from '%s': %Rrc", pThis->pszCfgFilename, rc);

        RTJsonValueRelease(hRoot);
        return rcExit;
    }

    return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to load config file '%s': %s", pThis->pszCfgFilename, ErrInfo.Core.pszMsg);
}


static RTEXITCODE s2gSvnInit(PS2GCTX pThis)
{
    /* Initialize APR first. */
    if (apr_initialize() != APR_SUCCESS)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "apr_initialize() failed\n");

    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    pThis->pPoolDefault = svn_pool_create(NULL);
    if (pThis->pPoolDefault)
    {
        pThis->pPoolScratch = svn_pool_create(NULL);
        if (pThis->pPoolScratch)
        {
            svn_error_t *pSvnErr = svn_repos_open3(&pThis->pSvnRepos, pThis->pszSvnRepo, NULL, pThis->pPoolDefault, pThis->pPoolScratch);
            if (!pSvnErr)
            {
                pThis->pSvnFs = svn_repos_fs(pThis->pSvnRepos);
                if (pThis->idRevEnd == UINT32_MAX)
                {
                    svn_revnum_t idRevYoungest;
                    svn_fs_youngest_rev(&idRevYoungest, pThis->pSvnFs, pThis->pPoolDefault);
                    pThis->idRevEnd = idRevYoungest;
                }
            }
            else
            {
                svn_error_trace(pSvnErr);
                rcExit = RTEXITCODE_FAILURE;
            }
        }
        else
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create scratch APR pool\n");
    }
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create default APR pool\n");

    return rcExit;
}


static const char *s2gSvnChangeKindToStr(svn_fs_path_change_kind_t enmKind)
{
    switch (enmKind)
    {
        case svn_fs_path_change_modify:
            return "Modified";
        case svn_fs_path_change_add:
            return "Added";
        case svn_fs_path_change_delete:
            return "Deleted";
        case svn_fs_path_change_replace:
            return "Replaced";
        case svn_fs_path_change_reset:
            return "Resetted";
    }

    AssertFailed();
    return "<UNKNOWN>";
}


static bool s2gPathIsExec(svn_fs_root_t *pSvnFsRoot, const char *pszSvnPath, apr_pool_t *pSvnPool)
{
    svn_string_t *pPropVal = NULL;
    svn_error_t *pSvnErr = svn_fs_node_prop(&pPropVal, pSvnFsRoot, pszSvnPath, "svn:executable", pSvnPool);
    if (pSvnErr)
        svn_error_trace(pSvnErr);

    return pPropVal != NULL;
}


static bool s2gPathIsSymlink(svn_fs_root_t *pSvnFsRoot, const char *pszSvnPath, apr_pool_t *pSvnPool)
{
    svn_string_t *pPropVal = NULL;
    svn_error_t *pSvnErr = svn_fs_node_prop(&pPropVal, pSvnFsRoot, pszSvnPath, "svn:special", pSvnPool);
    if (pSvnErr)
        svn_error_trace(pSvnErr);

    return pPropVal != NULL;
}


static RTEXITCODE s2gSvnPathIsEmptyDirEx(svn_fs_root_t *pSvnFsRoot, apr_pool_t *pPool, const char *pszSvnPath, bool *pfIsEmpty)
{
    apr_hash_t *pEntries = NULL;
    svn_error_t *pSvnErr = svn_fs_dir_entries(&pEntries, pSvnFsRoot, pszSvnPath, pPool);
    if (pSvnErr)
    {
        svn_error_trace(pSvnErr);
        return RTEXITCODE_FAILURE;
    }

    *pfIsEmpty = apr_hash_count(pEntries) == 0;
    return RTEXITCODE_SUCCESS;
}


DECLINLINE(RTEXITCODE) s2gSvnPathIsEmptyDir(PCS2GSVNREV pRev, const char *pszSvnPath, bool *pfIsEmpty)
{
    return s2gSvnPathIsEmptyDirEx(pRev->pSvnFsRoot, pRev->pPoolRev, pszSvnPath, pfIsEmpty);
}


DECLINLINE(RTEXITCODE) s2gSvnPathWasEmptyDir(PS2GCTX pThis, uint32_t idRev, const char *pszSvnPath,
                                             bool *pfWasExisting, bool *pfWasEmpty)
{
    /* Create a new temporary pool. */
    apr_pool_t *pPool = svn_pool_create(pThis->pPoolDefault);
    if (!pPool)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Allocating pool trying to check '%s' failed", pszSvnPath);

    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    svn_fs_root_t *pSvnFsRoot = NULL;
    svn_error_t *pSvnErr = svn_fs_revision_root(&pSvnFsRoot, pThis->pSvnFs, idRev, pPool);
    if (!pSvnErr)
    {
        apr_hash_t *pEntries = NULL;
        pSvnErr = svn_fs_dir_entries(&pEntries, pSvnFsRoot, pszSvnPath, pPool);
        if (!pSvnErr)
        {
            *pfWasExisting = true;
            *pfWasEmpty = apr_hash_count(pEntries) == 0;
        }
        else if (pSvnErr->apr_err == 160013) /* Path not found */
        {
            *pfWasExisting = false;
            *pfWasEmpty = false;
        }
        else
        {
            svn_error_trace(pSvnErr);
            rcExit = RTEXITCODE_FAILURE;
        }
    }
    else
    {
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    svn_pool_destroy(pPool);
    return rcExit;
}


static RTEXITCODE s2gSvnDumpBlob(PS2GCTX pThis, PS2GSVNREV pRev, svn_fs_root_t *pSvnFsRoot, const char *pszSvnPath, const char *pszGitPath)
{
    /* Create a new temporary pool. */
    apr_pool_t *pPool = svn_pool_create(pRev->pPoolRev);
    if (!pPool)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Allocating pool trying to dump '%s' failed", pszSvnPath);

    bool fIsExec = s2gPathIsExec(pSvnFsRoot, pszSvnPath, pPool);
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;

    if (!s2gPathIsSymlink(pSvnFsRoot, pszSvnPath, pPool))
    {
        svn_stream_t *pSvnStrmIn = NULL;
        svn_error_t *pSvnErr = svn_fs_file_contents(&pSvnStrmIn, pSvnFsRoot, pszSvnPath, pPool);
        if (!pSvnErr)
        {
            /* Do EOL style conversions and keyword substitutions. */
            apr_hash_t *pProps = NULL;
            pSvnErr = svn_fs_node_proplist(&pProps, pSvnFsRoot, pszSvnPath, pPool);
            if (!pSvnErr)
            {
                svn_string_t *pSvnStrEolStyle = (svn_string_t *)apr_hash_get(pProps, SVN_PROP_EOL_STYLE, APR_HASH_KEY_STRING);
                svn_string_t *pSvnStrKeywords = (svn_string_t *)apr_hash_get(pProps, SVN_PROP_KEYWORDS,  APR_HASH_KEY_STRING);
                if (pSvnStrEolStyle || pSvnStrKeywords)
                {
                    apr_hash_t *pHashKeywords = NULL;
                    const char *pszEolStr = NULL;
                    svn_subst_eol_style_t SvnEolStyle = svn_subst_eol_style_none;

                    if (pSvnStrEolStyle)
                        svn_subst_eol_style_from_value(&SvnEolStyle, &pszEolStr, pSvnStrEolStyle->data);

                    if (pSvnStrKeywords)
                    {
                        /** @todo */
                        char aszRev[32];
                        RTStrPrintf2(aszRev, sizeof(aszRev), "%u", pRev->idRev);

                        char aszUrl[4096];
                        RTStrPrintf2(aszUrl, sizeof(aszUrl), "https://localhost/vbox/svn");
                        char *pb = &aszUrl[sizeof("https://localhost/vbox/svn") - 1];
                        while (*pszSvnPath)
                        {
                            if (*pszSvnPath == ' ')
                            {
                                *pb++ = '%';
                                *pb++ = '2';
                                *pb++ = '0';
                            }
                            else
                                *pb++ = *pszSvnPath;
                            pszSvnPath++;
                        }
                        *pb = '\0';

                        pSvnErr = svn_subst_build_keywords3(&pHashKeywords, pSvnStrKeywords->data,
                                                            aszRev, aszUrl,
                                                            "https://localhost/vbox/svn", pRev->AprTime,
                                                            pRev->pszGitComitterEmail, pPool);
                    }

                    if (!pSvnErr)
                    {
                        pSvnStrmIn = svn_subst_stream_translated(svn_stream_disown(pSvnStrmIn, pPool),
                                                                 pszEolStr, FALSE, pHashKeywords, TRUE,
                                                                 pPool);
                        if (!pSvnStrmIn)
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to inject translated stream for '%s'", pszSvnPath);
                    }
                }

                if (!pSvnErr && rcExit == RTEXITCODE_SUCCESS)
                {
                    /* Determine stream length, due to substitutions this is  almost always different compared to what svn reports. */
                    s2gScratchBufReset(&pThis->BufScratch);
                    uint64_t cbFile = 0;
                    for (;;)
                    {
                        void *pv = s2gScratchBufEnsureSize(&pThis->BufScratch, _4K);
                        apr_size_t cbThisRead = _4K;
                        pSvnErr = svn_stream_read_full(pSvnStrmIn, (char *)pv, &cbThisRead);
                        if (pSvnErr)
                            break;
                        s2gScratchBufAdvance(&pThis->BufScratch, cbThisRead);

                        cbFile += cbThisRead;
                        if (cbThisRead < _4K)
                            break;
                    }

                    /* Add the file and stream the data. */
                    int rc = s2gGitTransactionFileAdd(pThis->hGitRepo, pszGitPath, fIsExec, cbFile);
                    if (RT_SUCCESS(rc))
                    {
                        rc = s2gGitTransactionFileWriteData(pThis->hGitRepo, pThis->BufScratch.pbBuf, cbFile);
                        if (RT_FAILURE(rc))
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to write data for file '%s' to git repository under '%s': %Rrc",
                                                    pszSvnPath, pszGitPath, rc);
                    }
                    else
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to add file '%s' to git repository under '%s': %Rrc",
                                                pszSvnPath, pszGitPath, rc);
                }
            }
        }

        if (pSvnErr)
        {
            AssertFailed();
            svn_error_trace(pSvnErr);
            rcExit = RTEXITCODE_FAILURE;
        }
    }
    else
    {
        svn_stream_t *pSvnStrmIn = NULL;
        svn_error_t *pSvnErr = svn_fs_file_contents(&pSvnStrmIn, pSvnFsRoot, pszSvnPath, pPool);
        if (!pSvnErr)
        {
            /* Determine stream length, due to substitutions this is  almost always different compared to what svn reports. */
            s2gScratchBufReset(&pThis->BufScratch);
            uint64_t cbFile = 0;
            for (;;)
            {
                void *pv = s2gScratchBufEnsureSize(&pThis->BufScratch, _4K);
                apr_size_t cbThisRead = _4K;
                pSvnErr = svn_stream_read_full(pSvnStrmIn, (char *)pv, &cbThisRead);
                if (pSvnErr)
                    break;
                s2gScratchBufAdvance(&pThis->BufScratch, cbThisRead);

                cbFile += cbThisRead;
                if (cbThisRead < _4K)
                    break;
            }

            if (!pSvnErr)
            {
                size_t const cchLink = sizeof("link ") - 1;
                if (!strncmp(pThis->BufScratch.pbBuf, "link ", cchLink))
                {
                    /* Add the file and stream the data. */
                    int rc = s2gGitTransactionLinkAdd(pThis->hGitRepo, pszGitPath, pThis->BufScratch.pbBuf + cchLink, cbFile - cchLink);
                    if (RT_FAILURE(rc))
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to add symlink '%s' to git repository under '%s': %Rrc",
                                                pszSvnPath, pszGitPath, rc);
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' is a special file but not a symlink, NOT IMPLEMENTED", pszSvnPath);
            }
        }

        if (pSvnErr)
        {
            AssertFailed();
            svn_error_trace(pSvnErr);
            rcExit = RTEXITCODE_FAILURE;
        }
    }

    svn_pool_destroy(pPool);
    return rcExit;
}


static RTEXITCODE s2gSvnProcessExternals(PS2GCTX pThis, PS2GSVNREV pRev, const char *pszSvnPath)
{
    svn_string_t *pProp = NULL;
    svn_error_t *pSvnErr = svn_fs_node_prop(&pProp, pRev->pSvnFsRoot, pszSvnPath, "svn:externals", pRev->pPoolRev);
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    if (!pSvnErr)
    {
        if (pProp)
        {
            const char *pszExternals = pProp->data;
            /* Go through all known externals and add them as a submodule if existing. */
            PCS2GEXTREVMAP pIt;
            RTListForEach(&pThis->LstExternals, pIt, S2GEXTREVMAP, NdExternals)
            {
                const char *pszExternal = RTStrStr(pszExternals, pIt->pszName);
                if (pszExternal)
                {
                    pszExternal += strlen(pIt->pszName);

                    /* We need a revision parameter, otherwise we can't map it to a commit hash. */
                    while (   *pszExternal == ' '
                           || *pszExternal == '\t')
                        pszExternal++;

                    if (strncmp(pszExternal, "-r", 2) == 0)
                    {
                        pszExternal += sizeof("-r") - 1;

                        while (   *pszExternal == ' '
                               || *pszExternal == '\t')
                            pszExternal++;

                        /* Try to convert to a revision number. */
                        uint32_t idExternalRev = 0;
                        int rc = RTStrToUInt32Full(pszExternal, 10, &idExternalRev);
                        if (   rc == VINF_SUCCESS
                            || rc == VERR_TRAILING_SPACES
                            || rc == VERR_TRAILING_CHARS)
                        {
                            if (   idExternalRev < pIt->cEntries
                                && pIt->papszRev2CommitHash[idExternalRev])
                            {
                                rc = s2gGitTransactionSubmoduleAdd(pThis->hGitRepo, pIt->pszName, pIt->papszRev2CommitHash[idExternalRev]);
                                if (RT_FAILURE(rc))
                                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE,
                                                            "Adding submodule for external '%s' with commit hash '%s' for revision number r%u failed: %Rrc",
                                                            pIt->pszName, pIt->papszRev2CommitHash[idExternalRev], idExternalRev);
                            }
                            else
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Revision number r%u for external '%s' lacks a git commit hash",
                                                        idExternalRev, pIt->pszName);
                        }
                        else
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to extract revision number for external '%s': %Rrc",
                                                    pIt->pszName, rc);
                    }
                    else
                        RTMsgWarning("No revision parameter for external '%s', skipping", pIt->pszName);

                    break;
                }
            }
        }
    }
    else
    {
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    return rcExit;
}


static RTEXITCODE s2gSvnAddGitIgnore(PS2GCTX pThis, const char *pszGitPath, const void *pvData, size_t cbData)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    char szPath[RTPATH_MAX];
    int rc;
    if (*pszGitPath == '\0')
    {
        strcpy(&szPath[0], ".gitignore");
        rc = 1;
    }
    else
        rc = RTStrPrintf2(&szPath[0], sizeof(szPath), "%s/%s", pszGitPath, ".gitignore");
    if (rc > 0)
    {
        rc = s2gGitTransactionFileAdd(pThis->hGitRepo, szPath, false /*fIsExec*/, cbData);
        if (RT_SUCCESS(rc))
            rc = s2gGitTransactionFileWriteData(pThis->hGitRepo, pvData, cbData);

        if (RT_FAILURE(rc))
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to add .gitignore '%s': %Rrc", szPath, rc);
    }
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to prepare .gitginore path for '%s'", pszGitPath);

    return rcExit;
}


static RTEXITCODE s2gSvnDeleteGitIgnore(PS2GCTX pThis, const char *pszGitPath)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    char szPath[RTPATH_MAX];
    int rc;
    if (*pszGitPath == '\0')
    {
        strcpy(&szPath[0], ".gitignore");
        rc = 1;
    }
    else
        rc = RTStrPrintf2(&szPath[0], sizeof(szPath), "%s/%s", pszGitPath, ".gitignore");
    if (rc > 0)
    {
        rc = s2gGitTransactionFileRemove(pThis->hGitRepo, szPath);
        if (RT_FAILURE(rc))
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to remove .gitignore '%s': %Rrc", szPath, rc);
    }
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to prepare .gitginore path for '%s'", pszGitPath);

    return rcExit;
}



static RTEXITCODE s2gSvnProcessIgnoreContent(PS2GSCRATCHBUF pBuf, const char *pszSvnIgnore, bool fGlobal)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;

    /** @todo This assumes no whitespace fun in filenames. */
    while (*pszSvnIgnore != '\0')
    {
        /* Skip any new line characters. */
        while (   *pszSvnIgnore == '\r'
               || *pszSvnIgnore == '\n')
            pszSvnIgnore++;

        if (*pszSvnIgnore != '\0')
        {
            size_t cchName = 0;
            char *pch = (char *)s2gScratchBufEnsureSize(pBuf, _1K); /** @todo A single filename can't be longer than this. */
            if (!fGlobal)
                *pch++ = '/';

            while (   *pszSvnIgnore != '\r'
                   && *pszSvnIgnore != '\n'
                   && *pszSvnIgnore != '\0')
            {
                /* Patterns containing slashes or backslashes are not supported by git. */
                if (   *pszSvnIgnore == '/'
                    || *pszSvnIgnore == '\\')
                {
                    while (   *pszSvnIgnore != '\r'
                           && *pszSvnIgnore != '\n'
                           && *pszSvnIgnore != '\0')
                        pszSvnIgnore++;
                    cchName = 0;
                    break;
                }

                cchName++;
                if (*pszSvnIgnore == '*')
                {
                    /* Multiple asterisks are not supported by git, convert to a single one. */
                    pszSvnIgnore++;
                    *pch++ = '*';
                    while (*pszSvnIgnore == '*')
                        pszSvnIgnore++;
                }
                else
                    *pch++ = *pszSvnIgnore++;
            }

            if (cchName)
            {
                if (!fGlobal)
                    cchName++;
                *pch++ = '\n';
                cchName++;
                s2gScratchBufAdvance(pBuf, cchName);
            }
        }
    }

    return rcExit;
}


static RTEXITCODE s2gSvnProcessIgnores(PS2GCTX pThis, PS2GSVNREV pRev, const char *pszSvnPath, const char *pszGitPath)
{
    s2gScratchBufReset(&pThis->BufScratch);

    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    svn_string_t *pProp = NULL;
    svn_error_t *pSvnErr = svn_fs_node_prop(&pProp, pRev->pSvnFsRoot, pszSvnPath, "svn:ignore", pRev->pPoolRev);
    if (!pSvnErr)
    {
        if (pProp)
            rcExit = s2gSvnProcessIgnoreContent(&pThis->BufScratch, pProp->data, false /*fGlobal*/);
        else
        {
            /*
             * The property got delete, so if the directory containing the .gitignore is not empty
             * and there is a .gitignore we have to delete it because.
             */
            bool fIsEmpty = false;
            rcExit = s2gSvnPathIsEmptyDir(pRev, pszSvnPath, &fIsEmpty);
            if (   rcExit == RTEXITCODE_SUCCESS
                && !fIsEmpty)
                rcExit = s2gSvnDeleteGitIgnore(pThis, pszGitPath);
        }

        /* Process global ignores only in the root path. */
        if (   rcExit == RTEXITCODE_SUCCESS
            && *pszGitPath == '\0')
        {
            pProp = NULL;
            pSvnErr = svn_fs_node_prop(&pProp, pRev->pSvnFsRoot, pszSvnPath, "svn:global-ignores", pRev->pPoolRev);
            if (!pSvnErr)
            {
                if (pProp)
                    rcExit = s2gSvnProcessIgnoreContent(&pThis->BufScratch, pProp->data, true /*fGlobal*/);
            }
            else
            {
                svn_error_trace(pSvnErr);
                rcExit = RTEXITCODE_FAILURE;
            }
        }

        if (   rcExit == RTEXITCODE_SUCCESS
            && pThis->BufScratch.offBuf)
            rcExit = s2gSvnAddGitIgnore(pThis, pszGitPath, pThis->BufScratch.pbBuf, pThis->BufScratch.offBuf);
    }
    else
    {
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    return rcExit;
}


static RTEXITCODE s2gSvnHasIgnores(PS2GSVNREV pRev, const char *pszSvnPath, bool *pfHasIgnores)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    svn_string_t *pProp = NULL;
    svn_error_t *pSvnErr = svn_fs_node_prop(&pProp, pRev->pSvnFsRoot, pszSvnPath, "svn:ignore", pRev->pPoolRev);
    if (!pSvnErr)
        *pfHasIgnores = pProp != NULL;
    else
    {
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    return rcExit;
}


static RTEXITCODE s2gSvnHasGitIgnore(PS2GSVNREV pRev, const char *pszSvnPath, bool *pfHasGitIgnore)
{
    *pfHasGitIgnore = false;

    apr_hash_t *pEntries = NULL;
    svn_error_t *pSvnErr = svn_fs_dir_entries(&pEntries, pRev->pSvnFsRoot, pszSvnPath, pRev->pPoolRev);
    if (pSvnErr)
    {
        svn_error_trace(pSvnErr);
        return RTEXITCODE_FAILURE;
    }

    for (apr_hash_index_t *pIt = apr_hash_first(pRev->pPoolRev, pEntries); pIt; pIt = apr_hash_next(pIt))
    {
        const void *vkey;
        apr_hash_this(pIt, &vkey, NULL, NULL);
        const char *pszEntry = (const char *)vkey;

        if (!RTStrCmp(pszEntry, ".gitignore"))
        {
            *pfHasGitIgnore = true;
            return RTEXITCODE_SUCCESS;
        }
    }

    return RTEXITCODE_SUCCESS;
}


static RTEXITCODE s2gSvnDumpDirRecursiveWorker(PS2GCTX pThis, PS2GSVNREV pRev, svn_fs_root_t *pSvnFsRoot, apr_pool_t *pPool,
                                               const char *pszSvnPath, const char *pszGitPath)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    apr_hash_t *pEntries;
    svn_error_t *pSvnErr = svn_fs_dir_entries(&pEntries, pSvnFsRoot, pszSvnPath, pPool);
    if (!pSvnErr)
    {
        RTLISTANCHOR LstEntries;
        RTListInit(&LstEntries);

        for (apr_hash_index_t *pIt = apr_hash_first(pPool, pEntries); pIt; pIt = apr_hash_next(pIt))
        {
            const void *vkey;
            void *value;
            apr_hash_this(pIt, &vkey, NULL, &value);
            const char *pszEntry = (const char *)vkey;
            svn_fs_dirent_t *pEntry = (svn_fs_dirent_t *)value;

            /* Insert the change into the list sorted by path. */
            /** @todo Speedup. */
            PS2GDIRENTRY pItEntries;
            RTListForEach(&LstEntries, pItEntries, S2GDIRENTRY, NdDir)
            {
                int iCmp = strcmp(pItEntries->pszName, pszEntry);
                if (!iCmp)
                    return RTMsgErrorExit(RTEXITCODE_FAILURE, "Duplicate directory entry found in rev %d: %s",
                                          pRev->idRev, pszEntry);
                else if (iCmp > 0)
                    break;
            }
            PS2GDIRENTRY pNew = (PS2GDIRENTRY)RTMemAllocZ(sizeof(*pNew));
            if (!pNew)
                return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate new directory entry for path: %s/%s",
                                      pszSvnPath, pszEntry);
            pNew->pszName = pszEntry;

            AssertRelease(pEntry->kind == svn_node_dir || pEntry->kind == svn_node_file);
            pNew->fIsDir  = pEntry->kind == svn_node_dir;
            RTListNodeInsertBefore(&LstEntries, &pNew->NdDir);
        }

        /* Walk the entries and recurse into directories. */
        PS2GDIRENTRY pIt, pItNext;
        RTListForEachSafe(&LstEntries, pIt, pItNext, S2GDIRENTRY, NdDir)
        {
            RTListNodeRemove(&pIt->NdDir);

            if (g_cVerbosity >= 5)
                RTMsgInfo("Processing %s/%s\n", pszSvnPath, pIt->pszName);

            /* Paths containing .git are invalid as git thinks these are other repositories. */
            if (!RTStrCmp(pIt->pszName, ".git"))
            {
                RTMsgWarning("Skipping invalid path '%s/%s'\n", pszSvnPath, pIt->pszName);
                continue;
            }

            char szSvnPath[RTPATH_MAX];
            char szGitPath[RTPATH_MAX];
            RTStrPrintf2(&szSvnPath[0], sizeof(szSvnPath), "%s/%s", pszSvnPath, pIt->pszName);
            if (*pszGitPath == '\0')
                RTStrPrintf2(&szGitPath[0], sizeof(szGitPath), "%s", pIt->pszName);
            else
                RTStrPrintf2(&szGitPath[0], sizeof(szGitPath), "%s/%s", pszGitPath, pIt->pszName);

            if (pIt->fIsDir)
            {
                /* If the directory is empty we need to add a .gitignore. */
                bool fIsEmpty = false;
                rcExit = s2gSvnPathIsEmptyDirEx(pSvnFsRoot, pPool, szSvnPath, &fIsEmpty);
                if (rcExit == RTEXITCODE_SUCCESS)
                {
                    if (fIsEmpty)
                        rcExit = s2gSvnAddGitIgnore(pThis, szGitPath, NULL /*pvData*/, 0 /*cbData*/);
                    else
                        rcExit = s2gSvnDumpDirRecursiveWorker(pThis, pRev, pSvnFsRoot, pPool, szSvnPath, szGitPath);
                }
            }
            else
                rcExit = s2gSvnDumpBlob(pThis, pRev, pSvnFsRoot, szSvnPath, szGitPath);
            RTMemFree(pIt);

            if (rcExit != RTEXITCODE_SUCCESS)
                break;
        }

        /* Free any leftover entries. */
        RTListForEachSafe(&LstEntries, pIt, pItNext, S2GDIRENTRY, NdDir)
        {
            RTListNodeRemove(&pIt->NdDir);
            RTMemFree(pIt);
        }
    }
    else
    {
        AssertFailed();
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    return rcExit;
}


static RTEXITCODE s2gSvnDumpDirRecursive(PS2GCTX pThis, PS2GSVNREV pRev, uint32_t idRevFrom, const char *pszSvnPath,
                                         const char *pszGitPath)
{
    apr_pool_t *pPoolRev = svn_pool_create(pThis->pPoolDefault);
    if (!pPoolRev)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create APR pool for revision r%u", idRevFrom);

    /* Open revision. */
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    svn_fs_root_t *pSvnFsRoot = NULL;
    svn_error_t *pSvnErr = svn_fs_revision_root(&pSvnFsRoot, pThis->pSvnFs, idRevFrom, pPoolRev);
    if (!pSvnErr)
        rcExit = s2gSvnDumpDirRecursiveWorker(pThis, pRev, pSvnFsRoot, pPoolRev, pszSvnPath, pszGitPath);
    else
    {
        AssertFailed();
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    svn_pool_destroy(pPoolRev);
    return rcExit;
}


DECLINLINE(PS2GBRANCH) s2gBranchGetFromPath(PS2GCTX pThis, const char *pszPath)
{
    PS2GBRANCH pBranch;
    RTListForEach(&pThis->LstBranches, pBranch, S2GBRANCH, NdBranches)
    {
        if (!RTStrNCmp(pszPath, &pBranch->achSvnPrefix[0], pBranch->cchSvnPrefix))
            return pBranch;
    }

    return NULL;
}

static RTEXITCODE s2gSvnExportSinglePath(PS2GCTX pThis, PS2GSVNREV pRev, const char *pszSvnPath, const char *pszGitPath,
                                         bool fIsDir, svn_fs_path_change2_t *pChange)
{
    /* Different strategies depending on the type and change type. */
    RTEXITCODE rcExit = RTEXITCODE_FAILURE;
    if (fIsDir)
    {
        if (   pChange->change_kind == svn_fs_path_change_add
            || pChange->change_kind == svn_fs_path_change_modify)
        {
            rcExit = RTEXITCODE_SUCCESS;

            /* Dump the directory content if copied from another source. */
            if (pChange->copyfrom_path != NULL)
            {
                PCS2GBRANCH pBranch = s2gBranchGetFromPath(pThis, pChange->copyfrom_path);
                if (   pBranch != pRev->pBranch
                    && !pRev->pBranch->fCreated)
                {
                    RTMsgInfo("Creating branch %s from %s@%u in revision %u\n",
                              pRev->pBranch->pszGitBranch, pBranch->pszGitBranch, pChange->copyfrom_rev, pRev->idRev);
                    int rc = s2gGitBranchCreate(pThis->hGitRepo, pRev->pBranch->pszGitBranch, pBranch->pszGitBranch,
                                                pChange->copyfrom_rev);
                    if (RT_SUCCESS(rc))
                        pRev->pBranch->fCreated = true;
                    else
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create git branch '%s': %Rrc",
                                                pBranch->pszGitBranch, rc);
                }

                if (rcExit == RTEXITCODE_SUCCESS)
                    rcExit = s2gSvnDumpDirRecursive(pThis, pRev, pChange->copyfrom_rev, pChange->copyfrom_path, pszGitPath);
            }

            if (rcExit == RTEXITCODE_SUCCESS)
            {
                /* Check properties. */
                if (pChange->prop_mod)
                {
                    rcExit = s2gSvnProcessExternals(pThis, pRev, pszSvnPath);
                    if (rcExit == RTEXITCODE_SUCCESS)
                    {
                        /* Process svn:ignore but only if there is no .gitignore in the source tree. */
                        bool fHasGitIgnore = false;
                        rcExit = s2gSvnHasGitIgnore(pRev, pszSvnPath, &fHasGitIgnore);
                        if (   rcExit == RTEXITCODE_SUCCESS
                            && !fHasGitIgnore)
                            rcExit = s2gSvnProcessIgnores(pThis, pRev, pszSvnPath, pszGitPath);
                    }
                }

                if (rcExit == RTEXITCODE_SUCCESS)
                {
                    /* If the directory is empty and doesn't has any svn:ignore properties set we need to add a .gitignore. */
                    bool fIsEmpty = false;
                    bool fHasIgnores = false;
                    rcExit = s2gSvnPathIsEmptyDir(pRev, pszSvnPath, &fIsEmpty);
                    if (rcExit == RTEXITCODE_SUCCESS)
                        rcExit = s2gSvnHasIgnores(pRev, pszSvnPath, &fHasIgnores);
                    if (   rcExit == RTEXITCODE_SUCCESS
                        && fIsEmpty
                        && !fHasIgnores)
                        rcExit = s2gSvnAddGitIgnore(pThis, pszGitPath, NULL /*pvData*/, 0 /*cbData*/);

                    /*
                     * Need to delete .gitignore in the parent if the directory doesn't has svn:ignores set
                     * and there is no .gitignore in the tree.
                     */
                    char szSvnPath[RTPATH_MAX];
                    RTStrCopy(szSvnPath, sizeof(szSvnPath), pszSvnPath);
                    RTPathStripFilename(szSvnPath);

                    bool fHasGitIgnore = false;
                    rcExit = s2gSvnHasGitIgnore(pRev, szSvnPath, &fHasGitIgnore);
                    if (   rcExit == RTEXITCODE_SUCCESS
                        && !fHasGitIgnore)
                    {
                        rcExit = s2gSvnHasIgnores(pRev, szSvnPath, &fHasIgnores);
                        if (   rcExit == RTEXITCODE_SUCCESS
                            && !fHasIgnores)
                        {
                            char szGitPath[RTPATH_MAX];
                            RTStrCopy(szGitPath, sizeof(szGitPath), pszGitPath);
                            RTPathStripFilename(szGitPath);
                            RTStrCat(szGitPath, sizeof(szGitPath), "/.gitignore");

                            int rc = s2gGitTransactionFileRemove(pThis->hGitRepo, szGitPath);
                            if (RT_FAILURE(rc))
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to remove '%s' from git repository", szGitPath);
                        }
                    }
                }
            }
        }
        else if (pChange->change_kind == svn_fs_path_change_replace)
        {
            /* Replaced with an empty path -> delete. */
            int rc = s2gGitTransactionFileRemove(pThis->hGitRepo, pszGitPath);
            if (RT_SUCCESS(rc))
            {
                if (pChange->copyfrom_known != 0)
                {
                    /* A replaced path needs dumping entirely recursively from the source. */
                    if (pChange->copyfrom_path)
                        rcExit = s2gSvnDumpDirRecursive(pThis, pRev, pChange->copyfrom_rev, pChange->copyfrom_path, pszGitPath);
                    else
                    {
                        /** @todo Check whether the directory is empty now and add a .gitignore file if it has not already due to
                         * svn:ignore properties.
                         */
                        char szSvnPath[RTPATH_MAX];
                        RTStrCopy(szSvnPath, sizeof(szSvnPath), pszSvnPath);
                        RTPathStripFilename(szSvnPath);

                        bool fIsEmpty = false;
                        rcExit = s2gSvnPathIsEmptyDir(pRev, szSvnPath, &fIsEmpty);
                        if (   rcExit == RTEXITCODE_SUCCESS
                            && fIsEmpty)
                        {
                            char szGitPath[RTPATH_MAX];
                            RTStrCopy(szGitPath, sizeof(szGitPath), pszGitPath);
                            RTPathStripFilename(szGitPath);

                            rcExit = s2gSvnAddGitIgnore(pThis, szGitPath, NULL /*pvData*/, 0 /*cbData*/);
                        }
                    }
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Replacing %s without known source", pszSvnPath);
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to remove '%s' from git repository", pszGitPath);
        }
        else
            AssertReleaseFailed();
    }
    else
    {
        /* File being added, just dump the contents to the git repository. */
        /** @todo Handle copies from branches where the source branch is part of the git repository. */
        if (   pChange->change_kind == svn_fs_path_change_add
            || pChange->change_kind == svn_fs_path_change_modify
            || pChange->change_kind == svn_fs_path_change_replace)
        {
            rcExit = s2gSvnDumpBlob(pThis, pRev, pRev->pSvnFsRoot, pszSvnPath, pszGitPath);
            if (   rcExit == RTEXITCODE_SUCCESS
                && pChange->change_kind == svn_fs_path_change_add
                && !RTStrStr(pszSvnPath, ".gitignore")) /* We don't want to delete .gitignore files which exist in the svn repository. */
            {
                /*
                 * Remove any possible existing .gitignore file we added in the parent previously
                 * because the directory was empty.
                 */
                char szSvnPath[RTPATH_MAX];
                RTStrCopy(szSvnPath, sizeof(szSvnPath), pszSvnPath);
                RTPathStripFilename(szSvnPath);

                bool fHasGitIgnore = false;
                rcExit = s2gSvnHasGitIgnore(pRev, szSvnPath, &fHasGitIgnore);
                if (   rcExit == RTEXITCODE_SUCCESS
                    && !fHasGitIgnore)
                {
                    bool fWasEmpty = false;
                    bool fWasExisting = false;
                    rcExit = s2gSvnPathWasEmptyDir(pThis, pRev->idRev - 1, szSvnPath, &fWasExisting, &fWasEmpty);
                    if (   rcExit == RTEXITCODE_SUCCESS
                        && fWasExisting
                        && fWasEmpty)
                    {
                        char szGitPath[RTPATH_MAX];
                        RTStrCopy(szGitPath, sizeof(szGitPath), pszGitPath);
                        RTPathStripFilename(szGitPath);
                        RTStrCat(szGitPath, sizeof(szGitPath), "/.gitignore");

                        int rc = s2gGitTransactionFileRemove(pThis->hGitRepo, szGitPath);
                        if (RT_FAILURE(rc))
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to remove '%s' from git repository", szGitPath);
                    }
                }
            }
        }
        else if (pChange->change_kind == svn_fs_path_change_delete)
        {
            int rc = s2gGitTransactionFileRemove(pThis->hGitRepo, pszGitPath);
            if (RT_SUCCESS(rc))
            {
                char szSvnPath[RTPATH_MAX];
                RTStrCopy(szSvnPath, sizeof(szSvnPath), pszSvnPath);
                RTPathStripFilename(szSvnPath);

                bool fIsEmpty = false;
                bool fHasIgnores = false;
                rcExit = s2gSvnPathIsEmptyDir(pRev, szSvnPath, &fIsEmpty);
                if (rcExit == RTEXITCODE_SUCCESS)
                    rcExit = s2gSvnHasIgnores(pRev, szSvnPath, &fHasIgnores);
                if (   rcExit == RTEXITCODE_SUCCESS
                    && fIsEmpty
                    && !fHasIgnores) /* Don't overwrite .gitignore files due to existing svn:ignore properties. */
                {
                    char szGitPath[RTPATH_MAX];
                    RTStrCopy(szGitPath, sizeof(szGitPath), pszGitPath);
                    RTPathStripFilename(szGitPath);

                    rcExit = s2gSvnAddGitIgnore(pThis, szGitPath, NULL /*pvData*/, 0 /*cbData*/);
                }
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to remove '%s' from git repository", pszGitPath);
        }
        else
            AssertReleaseFailed();
    }

    return rcExit;
}


static RTEXITCODE s2gSvnRevisionExportPaths(PS2GCTX pThis, PS2GSVNREV pRev)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    apr_hash_t *pSvnChanges;

    RT_GCC_NO_WARN_DEPRECATED_BEGIN
    svn_error_t *pSvnErr = svn_fs_paths_changed2(&pSvnChanges, pRev->pSvnFsRoot, pRev->pPoolRev);
    RT_GCC_NO_WARN_DEPRECATED_END
    if (!pSvnErr)
    {
        for (apr_hash_index_t *pIt = apr_hash_first(pRev->pPoolRev, pSvnChanges); pIt; pIt = apr_hash_next(pIt))
        {
            const void *vkey;
            void *value;
            apr_hash_this(pIt, &vkey, NULL, &value);
            const char *pszPath = (const char *)vkey;
            svn_fs_path_change2_t *pChange = (svn_fs_path_change2_t *)value;

            /* Ignore /branches */
            if (   !RTStrCmp(pszPath, "/branches")
                || !RTStrCmp(pszPath, "/tags"))
                continue;

            /* Paths containing .git are invalid as git thinks these are other repositories. */
            const char *psz = RTStrStr(pszPath, "/.git");
            if (   psz
                && (   psz[5] == '\0'
                    || psz[5] == '/'))
            {
                RTMsgWarning("Skipping invalid path '%s'\n", pszPath);
                continue;
            }

            PS2GSVNREVCHANGE pNew = (PS2GSVNREVCHANGE)RTMemAllocZ(sizeof(*pNew));
            if (!pNew)
                return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate new change record for path: %s", pszPath);
            pNew->pszPath = pszPath;
            pNew->pChange = pChange;

            if (!RTListIsEmpty(&pRev->LstChanges))
            {
                /* Insert the change into the list sorted by path. */
                /** @todo Speedup. */
                PS2GSVNREVCHANGE pItChanges;
                RTListForEach(&pRev->LstChanges, pItChanges, S2GSVNREVCHANGE, NdChanges)
                {
                    int iCmp = strcmp(pItChanges->pszPath, pszPath);
                    if (!iCmp)
                        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Duplicate key found in rev %d: %s", pRev->idRev, pszPath);
                    else if (iCmp > 0)
                        break;
                }

                RTListNodeInsertBefore(&pItChanges->NdChanges, &pNew->NdChanges);
            }
            else
                RTListAppend(&pRev->LstChanges, &pNew->NdChanges);
        }

        /* Get the branch we are working on for this revision. */
        PCS2GSVNREVCHANGE pFirst = RTListGetFirst(&pRev->LstChanges, S2GSVNREVCHANGE, NdChanges);
        if (pFirst)
        {
            pRev->pBranch = s2gBranchGetFromPath(pThis, pFirst->pszPath);
            if (!pRev->pBranch)
            {
                if (pThis->fIgnoreNonExistingBranchMappings)
                    return RTEXITCODE_SUCCESS;

                return RTMsgErrorExit(RTEXITCODE_FAILURE, "No branch mapping for path '%s'", pFirst->pszPath);
            }

            /* Work on the changes. */
            PS2GSVNREVCHANGE pIt;
            RTListForEach(&pRev->LstChanges, pIt, S2GSVNREVCHANGE, NdChanges)
            {
                svn_fs_path_change2_t *pChange = pIt->pChange;

                if (g_cVerbosity > 1)
                    RTMsgInfo("    %s %s\n", pIt->pszPath, s2gSvnChangeKindToStr(pChange->change_kind));

                /* Query whether this is a directory. */
                bool fIsDir = false;
                svn_boolean_t SvnIsDir;
                pSvnErr = svn_fs_is_dir(&SvnIsDir, pRev->pSvnFsRoot, pIt->pszPath, pRev->pPoolRev);
                if (pSvnErr)
                {
                    svn_error_trace(pSvnErr);
                    rcExit = RTEXITCODE_FAILURE;
                    break;
                }
                fIsDir = RT_BOOL(SvnIsDir);

                /*
                 * If this is a directory which was just added without any properties check whether the next entry starts with the path,
                 * meaning we can skip it as git doesn't handle empty directories and we don't want unnecessary .gitignore
                 * files.
                 */
                if (   fIsDir
                    && pChange->change_kind == svn_fs_path_change_add
                    && pChange->prop_mod == 0
                    && pChange->copyfrom_path == NULL
                    && pChange->copyfrom_known == 0)
                {
                    PS2GSVNREVCHANGE pNext = RTListGetNext(&pRev->LstChanges, pIt, S2GSVNREVCHANGE, NdChanges);
                    if (   pNext
                        && RTStrStartsWith(pNext->pszPath, pIt->pszPath))
                        continue;
                }

                if (!pRev->pBranch)
                    continue;

                if (!RTStrNCmp(pIt->pszPath, &pRev->pBranch->achSvnPrefix[0], pRev->pBranch->cchSvnPrefix))
                {
                    const char *pszGitPath = pIt->pszPath + pRev->pBranch->cchSvnPrefix;
                    if (*pszGitPath == '/')
                        pszGitPath++;

                    rcExit = s2gSvnExportSinglePath(pThis, pRev, pIt->pszPath, pszGitPath, fIsDir, pChange);
                    if (rcExit != RTEXITCODE_SUCCESS)
                        break;
                }
                else
                {
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Unsupported cross branch commit for path: %s", pIt->pszPath);
                    break;
                }
            }
        }
        else
            RTMsgWarning("Skipping empty commit");
    }
    else
    {
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    return rcExit;
}


static RTEXITCODE s2gSvnInitRevision(PS2GCTX pThis, uint32_t idRev, PS2GSVNREV pRev)
{
    RTListInit(&pRev->LstChanges);
    pRev->pPoolRev = svn_pool_create(pThis->pPoolDefault);
    if (!pRev->pPoolRev)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create APR pool for revision r%u", idRev);

    /* Open revision and fetch properties. */
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    pRev->idRev               = idRev;
    pRev->pszGitComitter      = NULL;
    pRev->pszGitComitterEmail = NULL;
    pRev->pszGitAuthor        = NULL;
    pRev->pszGitAuthorEmail   = NULL;
    pRev->pBranch             = NULL;
    svn_error_t *pSvnErr = svn_fs_revision_root(&pRev->pSvnFsRoot, pThis->pSvnFs, idRev, pRev->pPoolRev);
    if (!pSvnErr)
    {
        apr_hash_t *pRevPros = NULL;

        RT_GCC_NO_WARN_DEPRECATED_BEGIN
        pSvnErr = svn_fs_revision_proplist(&pRevPros, pThis->pSvnFs, idRev, pRev->pPoolRev);
        RT_GCC_NO_WARN_DEPRECATED_END
        if (!pSvnErr)
        {
            svn_string_t *pSvnAuthor = (svn_string_t *)apr_hash_get(pRevPros, "svn:author",                 APR_HASH_KEY_STRING);
            svn_string_t *pSvnDate   = (svn_string_t *)apr_hash_get(pRevPros, "svn:date",                   APR_HASH_KEY_STRING);
            svn_string_t *pSvnLog    = (svn_string_t *)apr_hash_get(pRevPros, "svn:log",                    APR_HASH_KEY_STRING);

            if (pThis->fSvnSyncXrefSrcRepoRevAvail)
            {
                svn_string_t *pSvnXRef = (svn_string_t *)apr_hash_get(pRevPros, "svn:sync-xref-src-repo-rev", APR_HASH_KEY_STRING);
                pRev->pszSvnXref = pSvnXRef ? pSvnXRef->data : NULL;
            }
            else
            {
                RTStrPrintf2(pRev->szRev, sizeof(pRev->szRev), "%u", idRev);
                pRev->pszSvnXref = pRev->szRev;
            }

            AssertRelease(pSvnDate && pSvnLog);
            if (pSvnAuthor || pThis->pszAuthorDefault)
            {
                pSvnErr = svn_time_from_cstring(&pRev->AprTime, pSvnDate->data, pRev->pPoolRev);
                if (!pSvnErr)
                {
                    pRev->pszSvnAuthor = pSvnAuthor ? pSvnAuthor->data : pThis->pszAuthorDefault;
                    pRev->pszSvnLog    = pSvnLog->data;

                    PCS2GAUTHOR pAuthor = (PCS2GAUTHOR)RTStrSpaceGet(&pThis->StrSpaceAuthors, pRev->pszSvnAuthor);
                    if (pAuthor)
                    {
                        pRev->pszGitComitter      = pAuthor->pszGitAuthor;
                        pRev->pszGitComitterEmail = pAuthor->pszGitEmail;

                        RTTIMESPEC Tm;
                        RTTimeSpecFromString(&Tm, pSvnDate->data);
                        pRev->cEpochSecs = RTTimeSpecGetSeconds(&Tm);
                        if (g_cVerbosity)
                            RTMsgInfo("    %s %s %s\n", pRev->pszSvnAuthor, pSvnDate->data, pRev->pszSvnXref ? pRev->pszSvnXref : "");

                        return RTEXITCODE_SUCCESS;
                    }
                    else
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Author '%s' is not known", pRev->pszSvnAuthor);
                }
                else
                {
                    svn_error_trace(pSvnErr);
                    rcExit = RTEXITCODE_FAILURE;
                }
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "r%u has no author set but no default author is configured", idRev);
        }
        else
        {
            svn_error_trace(pSvnErr);
            rcExit = RTEXITCODE_FAILURE;
        }
    }
    else
    {
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    svn_pool_destroy(pRev->pPoolRev);
    return rcExit;
}

static RTEXITCODE s2gSvnExportRevision(PS2GCTX pThis, uint32_t idRev)
{
    S2GSVNREV Rev;

    RTMsgInfo("Exporting revision r%u\n", idRev);

    RTListInit(&Rev.LstChanges);
    Rev.pPoolRev = svn_pool_create(pThis->pPoolDefault);
    if (!Rev.pPoolRev)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create APR pool for revision r%u", idRev);

    /* Open revision and fetch properties. */
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    Rev.idRev               = idRev;
    Rev.pszGitComitter      = NULL;
    Rev.pszGitComitterEmail = NULL;
    Rev.pszGitAuthor        = NULL;
    Rev.pszGitAuthorEmail   = NULL;
    Rev.pBranch             = NULL;
    svn_error_t *pSvnErr = svn_fs_revision_root(&Rev.pSvnFsRoot, pThis->pSvnFs, idRev, Rev.pPoolRev);
    if (!pSvnErr)
    {
        apr_hash_t *pRevPros = NULL;

        RT_GCC_NO_WARN_DEPRECATED_BEGIN
        pSvnErr = svn_fs_revision_proplist(&pRevPros, pThis->pSvnFs, idRev, Rev.pPoolRev);
        RT_GCC_NO_WARN_DEPRECATED_END
        if (!pSvnErr)
        {
            svn_string_t *pSvnAuthor = (svn_string_t *)apr_hash_get(pRevPros, "svn:author",                 APR_HASH_KEY_STRING);
            svn_string_t *pSvnDate   = (svn_string_t *)apr_hash_get(pRevPros, "svn:date",                   APR_HASH_KEY_STRING);
            svn_string_t *pSvnLog    = (svn_string_t *)apr_hash_get(pRevPros, "svn:log",                    APR_HASH_KEY_STRING);

            if (pThis->fSvnSyncXrefSrcRepoRevAvail)
            {
                svn_string_t *pSvnXRef = (svn_string_t *)apr_hash_get(pRevPros, "svn:sync-xref-src-repo-rev", APR_HASH_KEY_STRING);
                Rev.pszSvnXref = pSvnXRef ? pSvnXRef->data : NULL;
            }
            else
            {
                RTStrPrintf2(Rev.szRev, sizeof(Rev.szRev), "%u", idRev);
                Rev.pszSvnXref = Rev.szRev;
            }

            AssertRelease(pSvnDate && pSvnLog);
            if (pSvnAuthor || pThis->pszAuthorDefault)
            {
                pSvnErr = svn_time_from_cstring(&Rev.AprTime, pSvnDate->data, Rev.pPoolRev);
                if (!pSvnErr)
                {
                    Rev.pszSvnAuthor = pSvnAuthor ? pSvnAuthor->data : pThis->pszAuthorDefault;
                    Rev.pszSvnLog    = pSvnLog->data;

                    PCS2GAUTHOR pAuthor = (PCS2GAUTHOR)RTStrSpaceGet(&pThis->StrSpaceAuthors, Rev.pszSvnAuthor);
                    if (pAuthor)
                    {
                        Rev.pszGitComitter      = pAuthor->pszGitAuthor;
                        Rev.pszGitComitterEmail = pAuthor->pszGitEmail;

                        RTTIMESPEC Tm;
                        RTTimeSpecFromString(&Tm, pSvnDate->data);
                        Rev.cEpochSecs = RTTimeSpecGetSeconds(&Tm);
                        if (g_cVerbosity)
                            RTMsgInfo("    %s %s %s\n", Rev.pszSvnAuthor, pSvnDate->data, Rev.pszSvnXref ? Rev.pszSvnXref : "");

                        /* Check whether the log indicates that this is a github merge. */
                        char achAuthorInfo[_4K]; /* Temporary storage for the author information. */
                        const char *pszGithubMerge = RTStrStr(Rev.pszSvnLog, "github-merge-author: ");
                        if (pszGithubMerge)
                        {
                            uint32_t cchAuthorInfo = 0;

                            pszGithubMerge += sizeof("github-merge-author: ") - 1;
                            /* Parse the name up until the first < is encountered. */
                            while (*pszGithubMerge != '<')
                                achAuthorInfo[cchAuthorInfo++] = *pszGithubMerge++;
                            achAuthorInfo[cchAuthorInfo++] = '\0';
                            Rev.pszGitAuthor      = &achAuthorInfo[0];

                            /* Now the E-Mail. */
                            Rev.pszGitAuthorEmail = &achAuthorInfo[cchAuthorInfo];
                            while (*pszGithubMerge != '>')
                                achAuthorInfo[cchAuthorInfo++] = *pszGithubMerge++;
                            achAuthorInfo[cchAuthorInfo++] = '\0';

                            Rev.pszGitAuthor      = RTStrStrip((char *)Rev.pszGitAuthor);
                            Rev.pszGitAuthorEmail = RTStrStrip((char *)Rev.pszGitAuthorEmail);
                            /* Some basic author info sanity checks. */
                            if (   strlen(Rev.pszGitAuthor) == 0
                                || strlen(Rev.pszGitAuthorEmail) == 0
                                || !RTStrStr(Rev.pszGitAuthorEmail, "@"))
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "r%u has a malformed github-merge-author tag set", idRev);
                        }

                        if (rcExit == RTEXITCODE_SUCCESS)
                        {
                            int rc = s2gGitTransactionStart(pThis->hGitRepo);
                            if (RT_SUCCESS(rc))
                            {
                                rcExit = s2gSvnRevisionExportPaths(pThis, &Rev);
                                if (   rcExit == RTEXITCODE_SUCCESS
                                    && Rev.pBranch)
                                {
                                    size_t cchRevLog = strlen(Rev.pszSvnLog);

                                    s2gScratchBufReset(&pThis->BufScratch);
                                    rc = s2gScratchBufPrintf(&pThis->BufScratch, "%s%s\nsvn:sync-xref-src-repo-rev: r%s\n",
                                                             Rev.pszSvnLog, Rev.pszSvnLog[cchRevLog] == '\n' ? "" : "\n",
                                                             Rev.pszSvnXref);
                                    if (RT_SUCCESS(rc))
                                        rc = s2gGitTransactionCommit(pThis->hGitRepo, Rev.pszGitComitter, Rev.pszGitComitterEmail,
                                                                     Rev.pszGitAuthor, Rev.pszGitAuthorEmail,
                                                                     pThis->BufScratch.pbBuf, Rev.cEpochSecs, Rev.pBranch->pszGitBranch,
                                                                     Rev.idRev);
                                    if (RT_FAILURE(rc))
                                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to commit git transaction with: %Rrc", rc);
                                }
                            }
                            else
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to start new git transaction with: %Rrc", rc);
                        }
                    }
                    else
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Author '%s' is not known", Rev.pszSvnAuthor);
                }
                else
                {
                    svn_error_trace(pSvnErr);
                    rcExit = RTEXITCODE_FAILURE;
                }
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "r%u has no author set but no default author is configured", idRev);
        }
        else
        {
            svn_error_trace(pSvnErr);
            rcExit = RTEXITCODE_FAILURE;
        }
    }
    else
    {
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    svn_pool_destroy(Rev.pPoolRev);
    return rcExit;
}


static RTEXITCODE s2gSvnExport(PS2GCTX pThis)
{
    for (uint32_t idRev = pThis->idRevStart; idRev <= pThis->idRevEnd; idRev++)
    {
        RTEXITCODE rcExit = s2gSvnExportRevision(pThis, idRev);
        if (rcExit != RTEXITCODE_SUCCESS)
            return rcExit;
    }

    int rc = s2gGitRepositoryDone(pThis->hGitRepo);
    if (RT_FAILURE(rc))
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Flushing git repository '%s' failed: %Rrc",
                              pThis->pszGitRepoPath, rc);

    return RTEXITCODE_SUCCESS;
}


static RTEXITCODE s2gExportRevisionMap(PS2GCTX pThis)
{
    PS2GGITCOMMIT2SVNREV paCommits = NULL;
    uint32_t cCommits = 0;
    int rc = s2gGitRepositoryQueryCommits(pThis->hGitRepo, &paCommits, &cCommits);
    if (RT_FAILURE(rc))
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query commit list from git repository '%s': %Rrc",
                              pThis->pszGitRepoPath, rc);

    const char *pszPath = NULL;
    s2gFilenameToPath(&pszPath, pThis->pszRevisionMapPath);

    PRTSTREAM pStream = NULL;
    rc = RTStrmOpen(pszPath, "wt", &pStream);
    if (RT_FAILURE(rc))
    {
        RTMemFree(paCommits);
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create revision map '%s': %Rrc",
                              pszPath, rc);
    }

    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    int cch = RTStrmPrintf(pStream, "{\n");
    if (cch == 2)
    {
        for (uint32_t i = 0; i < cCommits; i++)
        {
            cch = RTStrmPrintf(pStream, "%s: %u,\n", paCommits[i].szCommitHash, paCommits[i].idSvnRev);
            if (cch < RTSHA1_DIGEST_LEN + 4 + 1)
            {
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to write revision map '%s'", pszPath);
                break;
            }
        }

        if (rcExit == RTEXITCODE_SUCCESS)
        {
            cch = RTStrmPrintf(pStream, "}\n");
            if (cch != 2)
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to write revision map '%s'", pszPath);
        }
    }
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to write revision map '%s'", pszPath);

    RTMemFree(paCommits);
    RTStrmClose(pStream);
    return rcExit;
}


static RTEXITCODE s2gSvnFindMatchingRevision(PS2GCTX pThis, uint32_t idRevInternal, uint32_t *pidRev)
{
    /* Work backwards from the youngest revision and try to get svn:sync-xref-src-repo-rev and check whether it matches. */
    uint32_t idRev = pThis->idRevEnd;
    apr_pool_t *pPool = svn_pool_create(NULL);
    if (!pPool)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create APR pool");

    while (idRev)
    {
        svn_string_t *pSvnXRef = NULL;
        RT_GCC_NO_WARN_DEPRECATED_BEGIN
        svn_error_t *pSvnErr = svn_fs_revision_prop(&pSvnXRef, pThis->pSvnFs, idRev, "svn:sync-xref-src-repo-rev", pPool);
        RT_GCC_NO_WARN_DEPRECATED_END
        if (!pSvnErr)
        {
            if (g_cVerbosity >= 4)
                RTMsgInfo("Searching r%u: %s\n", idRev, pSvnXRef ? pSvnXRef->data : "");

            if (pSvnXRef)
            {
                uint32_t idRevRef = RTStrToUInt32(pSvnXRef->data);
                if (idRevRef)
                {
                    if (idRevRef < idRevInternal)
                    {
                        *pidRev = idRev;
                        svn_pool_destroy(pPool);
                        return RTEXITCODE_SUCCESS;
                    }
                }
                else
                {
                    RTMsgErrorExit(RTEXITCODE_FAILURE, "r%u's svn:sync-xref-src-repo-rev property contains invalid data: %s",
                                   idRev, pSvnXRef->data);
                    break;
                }
            }
            else
            {
                RTMsgErrorExit(RTEXITCODE_FAILURE, "r%u misses svn:sync-xref-src-repo-rev property", idRev);
                break;
            }
        }
        else
        {
            svn_error_trace(pSvnErr);
            break;
        }

        idRev--;
        svn_pool_clear(pPool);
    }

    svn_pool_destroy(pPool);
    return RTMsgErrorExit(RTEXITCODE_FAILURE, "Couldn't match internal revision r%u to external one", idRevInternal);
}


static RTEXITCODE s2gGitInit(PS2GCTX pThis)
{
    uint32_t idRevLast = 0;

    const char *pszGitPath = NULL;
    s2gFilenameToPath(&pszGitPath, pThis->pszGitRepoPath);
    int rc = s2gGitRepositoryCreate(&pThis->hGitRepo, pszGitPath, pThis->pszGitDefBranch,
                                    pThis->pszGitPushOrigin, pThis->pszDumpFilename, &idRevLast);
    if (RT_SUCCESS(rc))
    {
        if (pThis->idRevStart == UINT32_MAX)
        {
            if (idRevLast != 0)
            {
                uint32_t idRevPublic = 0;
                if (pThis->fSvnSyncXrefSrcRepoRevAvail)
                {
                    /*
                     * We need to match the revision to the one of the repository as svn:sync-xref-src-repo-rev
                     * is a property.
                     */
                    RTEXITCODE rcExit = s2gSvnFindMatchingRevision(pThis, idRevLast + 1, &idRevPublic);
                    if (rcExit != RTEXITCODE_SUCCESS)
                        return rcExit;

                    RTMsgInfo("Matched internal revision r%u to public r%u, continuing at that revision\n",
                              idRevLast + 1, idRevPublic);
                }
                else
                    idRevPublic = idRevLast;

                pThis->idRevStart = idRevPublic + 1;
            }
            else
                pThis->idRevStart = 1;
        }
        return RTEXITCODE_SUCCESS;
    }

    return RTMsgErrorExit(RTEXITCODE_FAILURE, "Creating the git repository under '%s' failed with: %Rrc",
                          pThis->pszGitRepoPath, rc);
}


static RTEXITCODE s2gSvnGetInternalRevisionFromPublic(PS2GCTX pThis, uint32_t idRev, uint32_t *pidRevInternal)
{
    apr_pool_t *pPool = svn_pool_create(NULL);
    if (!pPool)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create APR pool");

    svn_string_t *pSvnXRef = NULL;
    RT_GCC_NO_WARN_DEPRECATED_BEGIN
    svn_error_t *pSvnErr = svn_fs_revision_prop(&pSvnXRef, pThis->pSvnFs, idRev, "svn:sync-xref-src-repo-rev", pPool);
    RT_GCC_NO_WARN_DEPRECATED_END
    if (!pSvnErr)
    {
        if (g_cVerbosity >= 4)
            RTMsgInfo("Searching r%u: %s\n", idRev, pSvnXRef ? pSvnXRef->data : "");

        uint32_t idRevRef = pSvnXRef ? RTStrToUInt32(pSvnXRef->data) : 17427; /** @todo This is for the case of r1 for our OSE repository. */
        if (idRevRef)
        {
            *pidRevInternal = idRevRef;
            svn_pool_destroy(pPool);
            return RTEXITCODE_SUCCESS;
        }
        else
            RTMsgErrorExit(RTEXITCODE_FAILURE, "r%u's svn:sync-xref-src-repo-rev property contains invalid data: %s",
                           idRev, pSvnXRef->data);
    }
    else
        svn_error_trace(pSvnErr);

    svn_pool_destroy(pPool);
    return RTEXITCODE_FAILURE;
}


static RTEXITCODE s2gSvnVerifyBlobIgnoreLineEndings(const char *pszGitPath, const void *pvGit, uint64_t cbGit,
                                                    const char *pszSvnPath, const void *pvSvn, uint64_t cbSvn)
{
    const uint8_t *pbGit = (const uint8_t *)pvGit;
    uint64_t cbGitLeft = cbGit;
    const uint8_t *pbSvn = (const uint8_t *)pvSvn;
    uint64_t cbSvnLeft = cbSvn;

    for (;;)
    {
        if (*pbGit != *pbSvn)
        {
            /* Check whether the difference is a line ending. */
            if (*pbGit == '\r' && *pbSvn == '\n')
            {
                pbGit++;
                cbGitLeft--;
                if (*pbGit != '\n')
                    return RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' has broken line endings",
                                          pszGitPath, cbGitLeft);
            }
            else
                return RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' and '%s' differ in content (cbGitLeft=%RU64, cbSvnLeft=%RU64)",
                                      pszSvnPath, pszGitPath, cbGitLeft, cbSvnLeft);
        }

        pbGit++;
        pbSvn++;
        cbGitLeft--;
        cbSvnLeft--;
        if (!cbGitLeft && !cbSvnLeft)
            break;
        else if (!cbGitLeft || !cbSvnLeft)
            return RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' and '%s' differ in content (cbGitLeft=%RU64, cbSvnLeft=%RU64)",
                                  pszSvnPath, pszGitPath, cbGitLeft, cbSvnLeft);
    }

    return RTEXITCODE_SUCCESS;
}


static RTEXITCODE s2gSvnVerifyBlob(PS2GCTX pThis, PCS2GSVNREV pRev, const char *pszSvnPath, const char *pszGitPath)
{
    /* Create a new temporary pool. */
    apr_pool_t *pPool = svn_pool_create(pRev->pPoolRev);
    if (!pPool)
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Allocating pool trying to dump '%s' failed", pszSvnPath);

    //bool fIsExec = s2gPathIsExec(pSvnFsRoot, pszSvnPath, pPool);
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;

    /** @todo Symlinks. */
    if (!s2gPathIsSymlink(pRev->pSvnFsRoot, pszSvnPath, pPool))
    {
        svn_stream_t *pSvnStrmIn = NULL;
        svn_error_t *pSvnErr = svn_fs_file_contents(&pSvnStrmIn, pRev->pSvnFsRoot, pszSvnPath, pPool);
        if (!pSvnErr)
        {
            /* Do EOL style conversions and keyword substitutions. */
            apr_hash_t *pProps = NULL;
            pSvnErr = svn_fs_node_proplist(&pProps, pRev->pSvnFsRoot, pszSvnPath, pPool);
            if (!pSvnErr)
            {
                svn_string_t *pSvnStrEolStyle = (svn_string_t *)apr_hash_get(pProps, SVN_PROP_EOL_STYLE, APR_HASH_KEY_STRING);
                svn_string_t *pSvnStrKeywords = (svn_string_t *)apr_hash_get(pProps, SVN_PROP_KEYWORDS,  APR_HASH_KEY_STRING);
                if (pSvnStrEolStyle || pSvnStrKeywords)
                {
                    apr_hash_t *pHashKeywords = NULL;
                    const char *pszEolStr = NULL;
                    svn_subst_eol_style_t SvnEolStyle = svn_subst_eol_style_none;

                    if (pSvnStrEolStyle)
                        svn_subst_eol_style_from_value(&SvnEolStyle, &pszEolStr, pSvnStrEolStyle->data);

                    if (pSvnStrKeywords)
                    {
                        /*
                         * Need to find the revision where the file was changed last and extract
                         * the necessary information required for substitution.
                         */
                        svn_fs_history_t *pSvnHistory = NULL;

                        RT_GCC_NO_WARN_DEPRECATED_BEGIN
                        svn_fs_node_history(&pSvnHistory, pRev->pSvnFsRoot, pszSvnPath, pPool);
                        svn_fs_history_prev(&pSvnHistory, pSvnHistory, true, pPool);
                        RT_GCC_NO_WARN_DEPRECATED_END
                        svn_revnum_t revnum = 0;
                        const char *psz = NULL;
                        svn_fs_history_location(&psz, &revnum, pSvnHistory, pPool);

                        apr_hash_t *pRevPros = NULL;

                        RT_GCC_NO_WARN_DEPRECATED_BEGIN
                        pSvnErr = svn_fs_revision_proplist(&pRevPros, pThis->pSvnFs, revnum, pPool);
                        RT_GCC_NO_WARN_DEPRECATED_END
                        svn_string_t *pSvnAuthor = (svn_string_t *)apr_hash_get(pRevPros, "svn:author", APR_HASH_KEY_STRING);
                        svn_string_t *pSvnDate   = (svn_string_t *)apr_hash_get(pRevPros, "svn:date",   APR_HASH_KEY_STRING);

                        char aszRev[32];
                        RTStrPrintf2(aszRev, sizeof(aszRev), "%ld", revnum);

                        apr_time_t AprTimeLast = 0;
                        if (pSvnDate)
                            svn_time_from_cstring(&AprTimeLast, pSvnDate->data, pPool);

                        PCS2GAUTHOR pAuthor = (PCS2GAUTHOR)RTStrSpaceGet(&pThis->StrSpaceAuthors, pSvnAuthor->data);
                        if (pAuthor)
                        {
                            char aszUrl[4096];
                            RTStrPrintf2(aszUrl, sizeof(aszUrl), "https://localhost/vbox/svn");
                            char *pb = &aszUrl[sizeof("https://localhost/vbox/svn") - 1];
                            const char *pszTmp = pszSvnPath;
                            while (*pszTmp)
                            {
                                if (*pszTmp == ' ')
                                {
                                    *pb++ = '%';
                                    *pb++ = '2';
                                    *pb++ = '0';
                                }
                                else
                                    *pb++ = *pszTmp;
                                pszTmp++;
                            }
                            *pb = '\0';

                            pSvnErr = svn_subst_build_keywords3(&pHashKeywords, pSvnStrKeywords->data,
                                                                aszRev, aszUrl,
                                                                "https://localhost/vbox/svn", AprTimeLast,
                                                                pAuthor->pszGitEmail, pPool);
                        }
                    }

                    if (!pSvnErr)
                    {
                        pSvnStrmIn = svn_subst_stream_translated(svn_stream_disown(pSvnStrmIn, pPool),
                                                                 pszEolStr, FALSE, pHashKeywords, TRUE,
                                                                 pPool);
                        if (!pSvnStrmIn)
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to inject translated stream for '%s'", pszSvnPath);
                    }
                }

                if (!pSvnErr && rcExit == RTEXITCODE_SUCCESS)
                {
                    /* Read the content. */
                    s2gScratchBufReset(&pThis->BufScratch);
                    uint64_t cbFile = 0;
                    for (;;)
                    {
                        void *pv = s2gScratchBufEnsureSize(&pThis->BufScratch, _4K);
                        apr_size_t cbThisRead = _4K;
                        pSvnErr = svn_stream_read_full(pSvnStrmIn, (char *)pv, &cbThisRead);
                        if (pSvnErr)
                            break;
                        s2gScratchBufAdvance(&pThis->BufScratch, cbThisRead);

                        cbFile += cbThisRead;
                        if (cbThisRead < _4K)
                            break;
                    }

                    void *pvFile = NULL;
                    size_t cbGitFile = 0;
                    int rc = RTFileReadAll(pszGitPath, &pvFile, &cbGitFile);
                    if (RT_SUCCESS(rc))
                    {
                        if (cbGitFile == cbFile)
                        {
                            if (memcmp(pThis->BufScratch.pbBuf, pvFile, cbFile))
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' and '%s' differ in content",
                                                        pszSvnPath, pszGitPath);
                        }
                        else
                        {
                            /** @todo We have some files with different line endings in the git worktree vs. how they are stored
                             *        and the SVN due to .gitattributes. For those files line ending differences are ignored.
                             *        Make this more generic using the config. */
                            const char *pszSuffix = RTPathSuffix(pszGitPath);
                            if (   pszSuffix
                                && !RTStrCmp(pszSuffix, ".csv"))
                                rcExit = s2gSvnVerifyBlobIgnoreLineEndings(pszGitPath, pvFile, cbGitFile,
                                                                           pszSvnPath, pThis->BufScratch.pbBuf,
                                                                           cbFile);
                            else
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' and '%s' differ in size (%RU64 vs %zu)",
                                                        pszSvnPath, pszGitPath, cbFile, cbGitFile);
                        }
                        RTFileReadAllFree(pvFile, cbGitFile);
                    }
                    else
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to read '%s'", pszGitPath);
                }
            }
        }

        if (pSvnErr)
        {
            AssertFailed();
            svn_error_trace(pSvnErr);
            rcExit = RTEXITCODE_FAILURE;
        }
    }
    else
    {
        svn_stream_t *pSvnStrmIn = NULL;
        svn_error_t *pSvnErr = svn_fs_file_contents(&pSvnStrmIn, pRev->pSvnFsRoot, pszSvnPath, pPool);
        if (!pSvnErr)
        {
            /* Determine stream length, due to substitutions this is  almost always different compared to what svn reports. */
            s2gScratchBufReset(&pThis->BufScratch);
            uint64_t cbFile = 0;
            for (;;)
            {
                void *pv = s2gScratchBufEnsureSize(&pThis->BufScratch, _4K);
                apr_size_t cbThisRead = _4K;
                pSvnErr = svn_stream_read_full(pSvnStrmIn, (char *)pv, &cbThisRead);
                if (pSvnErr)
                    break;
                s2gScratchBufAdvance(&pThis->BufScratch, cbThisRead);

                cbFile += cbThisRead;
                if (cbThisRead < _4K)
                    break;
            }

            if (!pSvnErr)
            {
                size_t const cchLink = sizeof("link ") - 1;
                if (!strncmp(pThis->BufScratch.pbBuf, "link ", cchLink))
                {
                    /** @todo */
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' is a special file but not a symlink, NOT IMPLEMENTED", pszSvnPath);
            }
        }

        if (pSvnErr)
        {
            AssertFailed();
            svn_error_trace(pSvnErr);
            rcExit = RTEXITCODE_FAILURE;
        }
    }

    svn_pool_destroy(pPool);
    return rcExit;
}


static RTEXITCODE s2gSvnQueryGitEntriesForPath(const char *pszGitPath, PRTLISTANCHOR pLstGitEntries, uint32_t *pcEntries)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    RTDIR hDir = NIL_RTDIR;
    int rc = RTDirOpen(&hDir, pszGitPath);
    if (RT_SUCCESS(rc))
    {
        PRTDIRENTRYEX pEntry = NULL;
        size_t cbDirEntry = 0;
        uint32_t cEntries = 0;

        for (;;)
        {
            rc = RTDirReadExA(hDir, &pEntry, &cbDirEntry, RTFSOBJATTRADD_NOTHING, RTPATH_F_ON_LINK);
            if (rc == VERR_NO_MORE_FILES)
                break;
            else if (RT_FAILURE(rc))
            {
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to read from directory '%s': %Rrc", pszGitPath, rc);
                break;
            }

            if (RTDirEntryExIsStdDotLink(pEntry))
                continue;

            PS2GDIRENTRY pNew = (PS2GDIRENTRY)RTMemAllocZ(sizeof(*pNew));
            if (!pNew)
            {
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate new directory entry for path: %s/%s",
                                        pszGitPath, pEntry->szName);
                break;
            }
            pNew->pszName = RTStrDup(pEntry->szName);
            if (!pNew->pszName)
            {
                RTMemFree(pNew);
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate new directory entry for path: %s/%s",
                                        pszGitPath, pEntry->szName);
                break;
            }

            pNew->fIsDir  = RT_BOOL(pEntry->Info.Attr.fMode & RTFS_TYPE_DIRECTORY);
            RTListAppend(pLstGitEntries, &pNew->NdDir);
            cEntries++;
        }

        if (pEntry)
            RTDirReadExAFree(&pEntry, &cbDirEntry);

        if (rcExit != RTEXITCODE_SUCCESS)
        {
            /** @todo Free list of entries. */
        }
        else
            *pcEntries = cEntries;

        RTDirClose(hDir);
    }
    else
        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to open directory '%s': %Rrc", pszGitPath, rc);

    return rcExit;
}


static PS2GDIRENTRY s2gFindGitEntry(PRTLISTANCHOR pLstEntries, const char *pszName)
{
    PS2GDIRENTRY pIt;
    RTListForEach(pLstEntries, pIt, S2GDIRENTRY, NdDir)
    {
        if (!RTStrCmp(pszName, pIt->pszName))
        {
            RTListNodeRemove(&pIt->NdDir);
            return pIt;
        }
    }

    return NULL;
}


static RTEXITCODE s2gSvnVerifyIgnores(PS2GCTX pThis, PS2GSVNREV pRev, const char *pszSvnPath, const char *pszGitPath,
                                      uint32_t cGitPathEntries, bool fSvnDirEmpty, uint32_t uLvl)
{
    s2gScratchBufReset(&pThis->BufScratch);

    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    svn_string_t *pProp = NULL;
    svn_error_t *pSvnErr = svn_fs_node_prop(&pProp, pRev->pSvnFsRoot, pszSvnPath, "svn:ignore", pRev->pPoolRev);
    if (!pSvnErr)
    {
        if (pProp)
            rcExit = s2gSvnProcessIgnoreContent(&pThis->BufScratch, pProp->data, false /*fGlobal*/);

        /* Process global ignores only in the root path. */
        if (   rcExit == RTEXITCODE_SUCCESS
            && uLvl == 0)
        {
            pProp = NULL;
            pSvnErr = svn_fs_node_prop(&pProp, pRev->pSvnFsRoot, pszSvnPath, "svn:global-ignores", pRev->pPoolRev);
            if (!pSvnErr)
            {
                if (pProp)
                    rcExit = s2gSvnProcessIgnoreContent(&pThis->BufScratch, pProp->data, true /*fGlobal*/);
            }
            else
            {
                svn_error_trace(pSvnErr);
                rcExit = RTEXITCODE_FAILURE;
            }
        }

        if (rcExit == RTEXITCODE_SUCCESS)
        {
            char szGitPath[RTPATH_MAX];
            RTStrPrintf2(&szGitPath[0], sizeof(szGitPath), "%s/.gitignore", pszGitPath);

            if (pThis->BufScratch.offBuf)
            {
                void *pvFile = NULL;
                size_t cbGitFile = 0;
                int rc = RTFileReadAll(szGitPath, &pvFile, &cbGitFile);
                if (RT_SUCCESS(rc))
                {
                    if (cbGitFile == pThis->BufScratch.offBuf)
                    {
                        if (memcmp(pThis->BufScratch.pbBuf, pvFile, pThis->BufScratch.offBuf))
                            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' and '%s' differ in content",
                                                    pszSvnPath, pszGitPath);
                    }
                    else
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "'%s' and '%s' differ in size (%RU64 vs %zu)",
                                                pszSvnPath, szGitPath, pThis->BufScratch.offBuf, cbGitFile);
                    RTFileReadAllFree(pvFile, cbGitFile);
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to read '%s'", pszGitPath);
            }
            else if (cGitPathEntries == 1 && fSvnDirEmpty)
            {
                /* Check that the .gitignore file is 0 bytes. */
                uint64_t cbFile = 0;
                int rc = RTFileQuerySizeByPath(szGitPath, &cbFile);
                if (RT_SUCCESS(rc))
                {
                    if (cbFile != 0)
                        rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE,
                                                "Empty git path '%s' without svn:properties has non empty .gitignore",
                                                pszGitPath);
                }
                else
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE,
                                            "Failed to query file size of '%s': %Rrc", szGitPath, rc);
            }
            else
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE,
                                        "Non empty git path '%s' has .gitignore but no svn:ignore properties set",
                                        pszGitPath);
        }
    }
    else
    {
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    return rcExit;
}


static RTEXITCODE s2gSvnVerifyRecursiveWorker(PS2GCTX pThis, PS2GSVNREV pRev, const char *pszSvnPath, const char *pszGitPath,
                                              uint32_t uLvl)
{
    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    apr_hash_t *pEntries;
    svn_error_t *pSvnErr = svn_fs_dir_entries(&pEntries, pRev->pSvnFsRoot, pszSvnPath, pRev->pPoolRev);
    if (!pSvnErr)
    {
        RTLISTANCHOR LstEntries;
        RTListInit(&LstEntries);

        RTLISTANCHOR LstGitEntries;
        uint32_t cGitEntries = 0;
        RTListInit(&LstGitEntries);
        rcExit = s2gSvnQueryGitEntriesForPath(pszGitPath, &LstGitEntries, &cGitEntries);
        if (rcExit != RTEXITCODE_SUCCESS)
            return rcExit;

        for (apr_hash_index_t *pIt = apr_hash_first(pRev->pPoolRev, pEntries); pIt; pIt = apr_hash_next(pIt))
        {
            const void *vkey;
            void *value;
            apr_hash_this(pIt, &vkey, NULL, &value);
            const char *pszEntry = (const char *)vkey;
            svn_fs_dirent_t *pEntry = (svn_fs_dirent_t *)value;

            /* Insert the change into the list sorted by path. */
            /** @todo Speedup. */
            PS2GDIRENTRY pItEntries;
            RTListForEach(&LstEntries, pItEntries, S2GDIRENTRY, NdDir)
            {
                int iCmp = strcmp(pItEntries->pszName, pszEntry);
                if (!iCmp)
                    return RTMsgErrorExit(RTEXITCODE_FAILURE, "Duplicate directory entry found in rev %d: %s",
                                          pRev->idRev, pszEntry);
                else if (iCmp > 0)
                    break;
            }
            PS2GDIRENTRY pNew = (PS2GDIRENTRY)RTMemAllocZ(sizeof(*pNew));
            if (!pNew)
                return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to allocate new directory entry for path: %s/%s",
                                      pszSvnPath, pszEntry);
            pNew->pszName = pszEntry;

            AssertRelease(pEntry->kind == svn_node_dir || pEntry->kind == svn_node_file);
            pNew->fIsDir  = pEntry->kind == svn_node_dir;
            RTListNodeInsertBefore(&LstEntries, &pNew->NdDir);
        }

        bool fSvnDirEmpty = RTListIsEmpty(&LstEntries);

        /* Walk the entries and recurse into directories. */
        PS2GDIRENTRY pIt, pItNext;
        RTListForEachSafe(&LstEntries, pIt, pItNext, S2GDIRENTRY, NdDir)
        {
            RTListNodeRemove(&pIt->NdDir);

            if (g_cVerbosity >= 5)
                RTMsgInfo("Processing %s/%s\n", pszSvnPath, pIt->pszName);

            /* Paths containing .git are invalid as git thinks these are other repositories. */
            if (!RTStrCmp(pIt->pszName, ".git"))
                continue;

            /* Try to find the matching entry in the git path. */
            PS2GDIRENTRY pGit = s2gFindGitEntry(&LstGitEntries, pIt->pszName);
            if (!pGit)
            {
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "SVN path '%s/%s' not available in git repository",
                                        pszSvnPath, pIt->pszName);
                break;
            }

            if (pGit->fIsDir != pIt->fIsDir)
            {
                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "SVN path '%s/%s' and git path disagree about fIsDir",
                                        pszSvnPath, pIt->pszName);
                RTStrFree((char *)pGit->pszName);
                RTMemFree(pGit);
                break;
            }

            char szSvnPath[RTPATH_MAX];
            char szGitPath[RTPATH_MAX];
            RTStrPrintf2(&szSvnPath[0], sizeof(szSvnPath), "%s/%s", pszSvnPath, pIt->pszName);
            RTStrPrintf2(&szGitPath[0], sizeof(szGitPath), "%s/%s", pszGitPath, pIt->pszName);

            if (pIt->fIsDir)
                rcExit = s2gSvnVerifyRecursiveWorker(pThis, pRev, szSvnPath, szGitPath, uLvl + 1);
            else
                rcExit = s2gSvnVerifyBlob(pThis, pRev, szSvnPath, szGitPath);
            RTMemFree(pIt);

            if (rcExit != RTEXITCODE_SUCCESS)
                break;
        }

        if (rcExit == RTEXITCODE_SUCCESS)
        {
            /*
             * Now there might be some entries left in the git path like .gitignore files.
             * Verify those.
             */
            RTListForEach(&LstGitEntries, pIt, S2GDIRENTRY, NdDir)
            {
                if (!RTStrCmp(pIt->pszName, ".gitignore"))
                {
                    /*
                     * .gitignore files appear either when there is a svn:ignore property set on the svn directory
                     * or when the SVN directory is empty.
                     */
                    rcExit = s2gSvnVerifyIgnores(pThis, pRev, pszSvnPath, pszGitPath, cGitEntries, fSvnDirEmpty, uLvl);
                }
                else if (   !RTStrCmp(pIt->pszName, ".git")
                         && uLvl == 0)
                { /* .git at top level is okay. */ }
                else
                {
                    /** @todo Externals */
                    if (!strcmp(pIt->pszName, "kBuild"))
                        continue;
                    rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "File '%s/%s' in git repository is unknown to svn",
                                            pszGitPath, pIt->pszName);
                    break;
                }
            }
        }

        /* Free any leftover entries. */
        RTListForEachSafe(&LstEntries, pIt, pItNext, S2GDIRENTRY, NdDir)
        {
            RTListNodeRemove(&pIt->NdDir);
            RTMemFree(pIt);
        }

        RTListForEachSafe(&LstGitEntries, pIt, pItNext, S2GDIRENTRY, NdDir)
        {
            RTListNodeRemove(&pIt->NdDir);
            RTStrFree((char *)pIt->pszName);
            RTMemFree(pIt);
        }
    }
    else
    {
        AssertFailed();
        svn_error_trace(pSvnErr);
        rcExit = RTEXITCODE_FAILURE;
    }

    return rcExit;
}


static RTEXITCODE s2gSvnVerifyRevision(PS2GCTX pThis, uint32_t idSvnRev, const char *pszGitPath)
{
    S2GSVNREV Rev;
    RTEXITCODE rcExit = s2gSvnInitRevision(pThis, idSvnRev, &Rev);
    if (rcExit == RTEXITCODE_FAILURE)
        return rcExit;

    rcExit = s2gSvnVerifyRecursiveWorker(pThis, &Rev, "/trunk", pszGitPath, 0);
    svn_pool_destroy(Rev.pPoolRev);
    return rcExit;
}


static RTEXITCODE s2gSvnVerify(PS2GCTX pThis)
{
    PS2GGITCOMMIT2SVNREV paCommits = NULL;
    uint32_t cCommits = 0;
    int rc = s2gGitRepositoryQueryCommits(pThis->hGitRepo, &paCommits, &cCommits);
    if (RT_FAILURE(rc))
        return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to query commit list from git repository '%s': %Rrc",
                              pThis->pszVerifyTmpPath, rc);

    /* Create a worktree if it doesn't exist. */
    if (!RTPathExists(pThis->pszVerifyTmpPath))
    {
        rc = s2gGitRepositoryClone(pThis->hGitRepo, pThis->pszVerifyTmpPath);
        if (RT_FAILURE(rc))
            return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to create worktree '%s': %Rrc",
                                  pThis->pszVerifyTmpPath, rc);
    }

    RTEXITCODE rcExit = RTEXITCODE_SUCCESS;
    for (uint32_t idRev = pThis->idRevStart; idRev <= pThis->idRevEnd; idRev++)
    {
        /* Get the internal revision number and try to match it to a git commit. */
        uint32_t idRevInternal = 0;
        rcExit = s2gSvnGetInternalRevisionFromPublic(pThis, idRev, &idRevInternal);
        if (rcExit != RTEXITCODE_SUCCESS)
            break;

        uint32_t idx = 0;
        while (   idx < cCommits
               && paCommits[idx].idSvnRev != idRevInternal)
            idx++;

        if (idx == cCommits)
        {
            /* Assume an empty svn commit. */
            /** @todo Verify */
            RTMsgWarning("Failed to find commit hash for revision r%u\n", idRev);
            continue;
        }

        rc = s2gGitRepositoryCheckout(pThis->pszVerifyTmpPath, paCommits[idx].szCommitHash);
        if (RT_FAILURE(rc))
        {
            rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to checkout commit '%s' in worktree '%s': %Rrc",
                                    paCommits[idx].szCommitHash, pThis->pszVerifyTmpPath, rc);
            break;
        }

        RTMsgInfo("Verifying r%u -> %s\n", idRev, paCommits[idx].szCommitHash);

        rcExit = s2gSvnVerifyRevision(pThis, idRev, pThis->pszVerifyTmpPath);
        if (rcExit != RTEXITCODE_SUCCESS)
            break;
    }

    if (rcExit == RTEXITCODE_SUCCESS) /* Leave the worktree for manual inspection in case of an error. */
    {
        rc = RTDirRemoveRecursive(pThis->pszVerifyTmpPath, RTDIRRMREC_F_CONTENT_AND_DIR);
        if (RT_FAILURE(rc))
            return RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to completely remove worktree '%s': %Rrc",
                                  pThis->pszVerifyTmpPath, rc);
    }

    RTMemFree(paCommits);
    return rcExit;
}


int main(int argc, char *argv[])
{
    int rc = RTR3InitExe(argc, &argv, 0);
    if (RT_FAILURE(rc))
        return RTEXITCODE_FAILURE;

    S2GCTX This;
    s2gCtxInit(&This);
    RTEXITCODE rcExit = s2gParseArguments(&This, argc, argv);
    if (rcExit == RTEXITCODE_SUCCESS)
    {
        rcExit = s2gLoadConfig(&This);
        if (rcExit == RTEXITCODE_SUCCESS)
        {
            rcExit = s2gSvnInit(&This);
            if (rcExit == RTEXITCODE_SUCCESS)
            {
                rcExit = s2gGitInit(&This);
                if (rcExit == RTEXITCODE_SUCCESS)
                {
                    /* Check which of the mapped branches exist in the git repository already. */
                    PS2GBRANCH pIt;
                    RTListForEach(&This.LstBranches, pIt, S2GBRANCH, NdBranches)
                    {
                        pIt->fCreated = s2gGitBranchExists(This.hGitRepo, pIt->pszGitBranch);
                    }

                    if (!This.pszVerifyTmpPath)
                    {
                        rcExit = s2gSvnExport(&This);
                        if (   rcExit == RTEXITCODE_SUCCESS
                            && This.pszGitPushOrigin)
                        {
                            rc = s2gGitRepositoryPush(This.hGitRepo);
                            if (RT_FAILURE(rc))
                                rcExit = RTMsgErrorExit(RTEXITCODE_FAILURE, "Failed to push git repository '%s' to '%s': %Rrc",
                                                        This.pszGitRepoPath, This.pszGitPushOrigin, rc);
                        }
                        if (   rcExit == RTEXITCODE_SUCCESS
                            && This.pszRevisionMapPath)
                            rcExit = s2gExportRevisionMap(&This);
                    }
                    else
                        rcExit = s2gSvnVerify(&This);
                }

                s2gGitRepositoryClose(This.hGitRepo);
            }
        }
    }
    s2gCtxDestroy(&This);

    return rcExit;
}

