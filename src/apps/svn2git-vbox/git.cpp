/* $Id: git.cpp $ */
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
#include <iprt/env.h>
#include <iprt/err.h>
#include <iprt/file.h>
#include <iprt/list.h>
#include <iprt/path.h>
#include <iprt/pipe.h>
#include <iprt/process.h>

#include "svn2git-internal.h"


/*********************************************************************************************************************************
*   Structures and Typedefs                                                                                                      *
*********************************************************************************************************************************/

#define GIT_BINARY "git"

/**
 * A svn revision to fast-import mark mapping.
 */
typedef struct S2GSVNREV2MARK
{
    /** The SVN revision number. */
    uint64_t        idSvnRev;
    /** The commit mark corresponding to the SVN revision number. */
    uint64_t        idGitMark;
} S2GSVNREV2MARK;
typedef S2GSVNREV2MARK *PS2GSVNREV2MARK;
typedef const S2GSVNREV2MARK *PCS2GSVNREV2MARK;


/**
 * Git branch
 */
typedef struct S2GBRANCH
{
    /** List node. */
    RTLISTNODE      NdBranches;
    /** Pointer to the base of the SVN revision to mark mapping. */
    PS2GSVNREV2MARK paSvnRev2Mark;
    /** Number of entries in the mapping array. */
    uint32_t        cSvnRev2MarkEntries;
    /** Maximum number of entries the mapping array can hold. */
    uint32_t        cSvnRev2MarkEntriesMax;
    /** The git commit mark this branch was created from, UINT64_MAX means not being available. */
    uint64_t        idGitMarkMerge;
    /** The name of the branch. */
    RT_GCC_EXTENSION
    char            szName[RT_FLEXIBLE_ARRAY];
} S2GBRANCH;
typedef S2GBRANCH *PS2GBRANCH;
typedef const S2GBRANCH *PCS2GBRANCH;


/**
 * Git repository state.
 */
typedef struct S2GREPOSITORYGITINT
{
    /** Process handle to the fast-import process. */
    RTPROCESS       hProcFastImport;
    /** The pipe we write the command stream to. */
    RTPIPE          hPipeWrite;
    /** stderr of git fast-import. */
    RTPIPE          hPipeStderr;
    /** stdout of git fast-import. */
    RTPIPE          hPipeStdout;

    /** The git repository path. */
    char            *pszGitRepoPath;
    /** Flag whether this is an incremental update. */
    bool            fIncremental;

    /* The dump file handle. */
    RTFILE          hFileDump;

    /** The next file mark. */
    uint64_t        idFileMark;
    /** The next commit mark. */
    uint64_t        idCommitMark;

    /** List of branches. */
    RTLISTANCHOR    LstBranches;

    /** Buffer for files being added/modified/deleted. */
    S2GSCRATCHBUF   BufModifiedFiles;
    /** Scratch buffer. */
    S2GSCRATCHBUF   BufScratch;
} S2GREPOSITORYGITINT;
typedef S2GREPOSITORYGITINT *PS2GREPOSITORYGITINT;
typedef const S2GREPOSITORYGITINT *PCS2GREPOSITORYGITINT;


static PS2GBRANCH s2gGitBranchCreateWorker(const char *pachName, size_t cchName)
{
    size_t cbName = (cchName + 1) * sizeof(char);
    PS2GBRANCH pBranch = (PS2GBRANCH)RTMemAllocZ(RT_UOFFSETOF_DYN(S2GBRANCH, szName[cbName]));
    if (pBranch)
    {
        memcpy(&pBranch->szName[0], pachName, cchName * sizeof(char));
        pBranch->szName[cchName]        = '\0';
        pBranch->paSvnRev2Mark          = 0;
        pBranch->cSvnRev2MarkEntries    = 0;
        pBranch->cSvnRev2MarkEntriesMax = 0;
        pBranch->idGitMarkMerge         = UINT64_MAX;
    }

    return pBranch;
}


DECLINLINE(PS2GBRANCH) s2gGitGetBranch(PS2GREPOSITORYGITINT pThis, const char *pszBranch)
{
    PS2GBRANCH pBranch;
    RTListForEach(&pThis->LstBranches, pBranch, S2GBRANCH, NdBranches)
    {
        if (!RTStrCmp(pBranch->szName, pszBranch))
            return pBranch;
    }

    return NULL;
}


static int s2gGitBranchAssociateMarkWithSvnRev(PS2GBRANCH pBranch, uint64_t idCommitMark, uint64_t idSvnRev)
{
    if (pBranch->cSvnRev2MarkEntries == pBranch->cSvnRev2MarkEntriesMax)
    {
        size_t cbNew = (pBranch->cSvnRev2MarkEntriesMax + _4K) * sizeof(*pBranch->paSvnRev2Mark);
        PS2GSVNREV2MARK paNew = (PS2GSVNREV2MARK)RTMemRealloc(pBranch->paSvnRev2Mark, cbNew);
        if (!paNew)
            return VERR_NO_MEMORY;

        pBranch->paSvnRev2Mark           = paNew;
        pBranch->cSvnRev2MarkEntriesMax += _4K;
    }

    pBranch->paSvnRev2Mark[pBranch->cSvnRev2MarkEntries].idSvnRev  = idSvnRev;
    pBranch->paSvnRev2Mark[pBranch->cSvnRev2MarkEntries].idGitMark = idCommitMark;
    pBranch->cSvnRev2MarkEntries++;
    return VINF_SUCCESS;
}


static int s2gGitBranchQueryMarkFromSvnRev(PS2GREPOSITORYGITINT pThis, const char *pszBranch,
                                           uint64_t idSvnRev, uint64_t *pidMark)
{
    PS2GBRANCH pBranch = s2gGitGetBranch(pThis, pszBranch);
    if (!pBranch)
        return VERR_NOT_FOUND;

    /* Search for the matching mark. */
    /** @todo Inefficient but the space won't be huge most of the time and we go backwards,
     *        branching is usually done from direct ancestor commit. */
    for (uint32_t i = pBranch->cSvnRev2MarkEntries; i > 0; i--)
    {
        if (pBranch->paSvnRev2Mark[i - 1].idSvnRev == idSvnRev)
        {
            *pidMark = pBranch->paSvnRev2Mark[i - 1].idGitMark;
            return VINF_SUCCESS;
        }
    }

    return VERR_NOT_FOUND;
}


static int s2gGitExecWrapper(const char *pszExec, const char *pszCwd, const char * const *papszArgs)
{
    RTPROCESS hProc;
    int rc = RTProcCreateEx(pszExec, papszArgs, RTENV_DEFAULT, RTPROC_FLAGS_SEARCH_PATH | RTPROC_FLAGS_CWD,
                            NULL, NULL, NULL, NULL, NULL, (void *)pszCwd, &hProc);
    if (RT_SUCCESS(rc))
    {
        RTPROCSTATUS Sts;
        rc = RTProcWait(hProc, RTPROCWAIT_FLAGS_BLOCK, &Sts);
        if (RT_SUCCESS(rc))
        {
            if (   Sts.enmReason != RTPROCEXITREASON_NORMAL
                || Sts.iStatus != 0)
                rc = VERR_INVALID_HANDLE; /** @todo */
        }
    }

    return rc;
}


static int s2gGitExecWrapperStdOut(const char *pszExec, const char *pszCwd, const char * const *papszArgs,
                                   PS2GSCRATCHBUF pStdOut)
{
    RTPROCESS hProc;
    RTPIPE hPipeStdOutR = NIL_RTPIPE;
    RTPIPE hPipeStdOutW = NIL_RTPIPE;
    int rc = RTPipeCreate(&hPipeStdOutR, &hPipeStdOutW, RTPIPE_C_INHERIT_WRITE);
    if (RT_SUCCESS(rc))
    {
        RTHANDLE HndOut;
        HndOut.enmType = RTHANDLETYPE_PIPE;
        HndOut.u.hPipe = hPipeStdOutW;

        rc = RTProcCreateEx(pszExec, papszArgs, RTENV_DEFAULT, RTPROC_FLAGS_SEARCH_PATH | RTPROC_FLAGS_CWD,
                            NULL, &HndOut, NULL,
                            NULL, NULL, (void *)pszCwd,
                            &hProc);
        RTPipeClose(hPipeStdOutW);
        if (RT_SUCCESS(rc))
        {
            /* Read stdout until we get a broken pipe. */
            for (;;)
            {
                void *pvStdout = s2gScratchBufEnsureSize(pStdOut, _2K);
                if (!pvStdout)
                {
                    rc = VERR_NO_MEMORY;
                    break;
                }

                size_t cbRead = 0;
                rc = RTPipeReadBlocking(hPipeStdOutR, pvStdout, _2K, &cbRead);
                if (RT_FAILURE(rc))
                    break;

                s2gScratchBufAdvance(pStdOut, cbRead);
            }

            if (rc == VERR_BROKEN_PIPE)
                rc = VINF_SUCCESS;
            else if (RT_FAILURE(rc))
                RTProcTerminate(hProc);

            RTPROCSTATUS Sts;
            int rc2 = RTProcWait(hProc, RTPROCWAIT_FLAGS_BLOCK, &Sts);
            if (RT_SUCCESS(rc2))
            {
                if (   Sts.enmReason != RTPROCEXITREASON_NORMAL
                    || Sts.iStatus != 0)
                    rc2 = VERR_INVALID_HANDLE; /** @todo */
            }

            if (RT_SUCCESS(rc))
                rc = rc2;
        }

        RTPipeClose(hPipeStdOutR);
    }

    if (RT_SUCCESS(rc))
    {
        /* Ensure proper termination. */
        char *pb = (char *)s2gScratchBufEnsureSize(pStdOut, 1);
        if (pb)
        {
            *pb = '\0';
        }
        else
            rc = VERR_NO_MEMORY;
    }

    return rc;
}


DECLINLINE(int) s2gGitWrite(PS2GREPOSITORYGITINT pThis, const void *pvBuf, size_t cbWrite)
{
    int rc = VINF_SUCCESS;

    if (pThis->hPipeWrite != NIL_RTPIPE)
        rc = RTPipeWriteBlocking(pThis->hPipeWrite, pvBuf, cbWrite, NULL /*pcbWritten*/);

    if (pThis->hFileDump != NIL_RTFILE)
    {
        int rc2 = RTFileWrite(pThis->hFileDump, pvBuf, cbWrite, NULL /*pcbWritten*/);
        if (RT_SUCCESS(rc))
            rc = rc2;
    }

    return rc;
}


static int s2gGitRepositoryQueryBranches(const char *pszGitRepoPath, PRTLISTANCHOR pLstBranches)
{
    S2GSCRATCHBUF StdOut;
    s2gScratchBufInit(&StdOut);

    const char *apszArgs[] = { GIT_BINARY, "branch", NULL };
    int rc = s2gGitExecWrapperStdOut(GIT_BINARY, pszGitRepoPath, &apszArgs[0], &StdOut);
    if (RT_SUCCESS(rc))
    {
        char *pch = StdOut.pbBuf;
        while (*pch != '\0')
        {
            /* We should always start at a new line, which might start with an asterisk to denote the active branch. */
            if (*pch == '*')
                pch++;

            /* Now there are 1-2 spaces. */
            while (*pch == ' ')
                pch++;

            /* Now starts the branch name, followed by a newline. */
            char *pchName = pch;
            while (   *pch != '\r'
                   && *pch != '\n')
                pch++;

            size_t cchName = pch - pchName;
            PS2GBRANCH pBranch = s2gGitBranchCreateWorker(pchName, cchName);
            if (!pBranch)
            {
                rc = VERR_NO_MEMORY;
                break;
            }

            RTListAppend(pLstBranches, &pBranch->NdBranches);

            /* Get past the new line. */
            while (*pch == '\r' || *pch == '\n')
                pch++;
        }
    }

    return rc;
}


static int s2gGitRepositoryMapCommitToSvnRev(const char *pszGitRepoPath, const char *pszCommitHashOrBranch, uint32_t *pidSvnRev)
{
    S2GSCRATCHBUF StdOut;
    s2gScratchBufInit(&StdOut);

    const char *apszArgs[] = { GIT_BINARY, "log", pszCommitHashOrBranch, "-1", NULL };
    int rc = s2gGitExecWrapperStdOut(GIT_BINARY, pszGitRepoPath, &apszArgs[0], &StdOut);
    if (RT_SUCCESS(rc))
    {
        const char *pszRevision = RTStrStr(StdOut.pbBuf, "svn:sync-xref-src-repo-rev: ");
        if (pszRevision)
        {
            pszRevision += sizeof("svn:sync-xref-src-repo-rev: ") - 1;
            if (*pszRevision == 'r')
            {
                uint32_t idRev = RTStrToUInt32(pszRevision + 1);
                *pidSvnRev = idRev;
            }
        }
    }

    return rc;
}

DECLHIDDEN(int) s2gGitRepositoryCreate(PS2GREPOSITORYGIT phGitRepo, const char *pszGitRepoPath, const char *pszDefaultBranch,
                                       const char *pszGitPushOrigin, const char *pszDumpFilename, uint32_t *pidRevLast)
{
    int rc = VINF_SUCCESS;
    bool fIncremental = RTPathExists(pszGitRepoPath);
    RTLISTANCHOR LstBranches;

    RTListInit(&LstBranches);
    if (!fIncremental)
    {
        rc = RTDirCreate(pszGitRepoPath, 0700, RTDIRCREATE_FLAGS_NO_SYMLINKS);
        if (RT_SUCCESS(rc))
        {
            const char *apszArgs[] = { GIT_BINARY, "--bare", "init", "--initial-branch", pszDefaultBranch, NULL };
            rc = s2gGitExecWrapper(GIT_BINARY, pszGitRepoPath, &apszArgs[0]);
            if (RT_SUCCESS(rc))
            {
                const char *apszArgsCfg[] = { GIT_BINARY, "config", "core.ignorecase", "false", NULL };
                rc = s2gGitExecWrapper(GIT_BINARY, pszGitRepoPath, &apszArgsCfg[0]);
                if (RT_SUCCESS(rc))
                {
                    if (pszGitPushOrigin)
                    {
                        const char *apszArgsRemote[] = { GIT_BINARY, "remote", "add", "origin", pszGitPushOrigin, NULL };
                        rc = s2gGitExecWrapper(GIT_BINARY, pszGitRepoPath, &apszArgsRemote[0]);
                    }

                    if (RT_SUCCESS(rc))
                    {
                        PS2GBRANCH pBranch = s2gGitBranchCreateWorker(pszDefaultBranch, strlen(pszDefaultBranch));
                        if (pBranch)
                            RTListAppend(&LstBranches, &pBranch->NdBranches);
                        else
                            rc = VERR_NO_MEMORY;
                    }
                }
            }
        }
    }
    else
    {
        /*
         * Query all branches on the existing repository and try to get the latest subversion
         * revision the repository has across all branches.
         */
        rc = s2gGitRepositoryQueryBranches(pszGitRepoPath, &LstBranches);
        if (RT_SUCCESS(rc))
        {
            *pidRevLast = 0;

            PS2GBRANCH pIt;
            RTListForEach(&LstBranches, pIt, S2GBRANCH, NdBranches)
            {
                /* Try to gather the svn revision to continue at from the commit log. */
                uint32_t idRev = 0;
                rc = s2gGitRepositoryMapCommitToSvnRev(pszGitRepoPath, pIt->szName, &idRev);
                if (RT_FAILURE(rc))
                    break;

                if (idRev > *pidRevLast)
                    *pidRevLast = idRev;
            }
        }
    }

    if (RT_SUCCESS(rc))
    {
        PS2GREPOSITORYGITINT pThis = (PS2GREPOSITORYGITINT)RTMemAllocZ(sizeof(*pThis));
        if (pThis)
        {
            pThis->pszGitRepoPath = RTStrDup(pszGitRepoPath);
            if (pThis->pszGitRepoPath)
            {
                s2gScratchBufInit(&pThis->BufModifiedFiles);
                s2gScratchBufInit(&pThis->BufScratch);
                pThis->fIncremental = fIncremental;
                pThis->idCommitMark = 1;
                pThis->hFileDump    = NIL_RTFILE;
                RTListMove(&pThis->LstBranches, &LstBranches);

                if (pszDumpFilename)
                    rc = RTFileOpen(&pThis->hFileDump, pszDumpFilename, RTFILE_O_WRITE | RTFILE_O_CREATE_REPLACE | RTFILE_O_DENY_NONE);

                if (RT_SUCCESS(rc))
                {
                    RTPIPE hPipeFiR = NIL_RTPIPE;
                    rc = RTPipeCreate(&hPipeFiR, &pThis->hPipeWrite, RTPIPE_C_INHERIT_READ);
                    if (RT_SUCCESS(rc))
                    {
                        RTHANDLE HndIn;
                        HndIn.enmType = RTHANDLETYPE_PIPE;
                        HndIn.u.hPipe = hPipeFiR;

                        const char *apszArgs[] = { GIT_BINARY, "fast-import", NULL };
                        rc = RTProcCreateEx(GIT_BINARY, &apszArgs[0], RTENV_DEFAULT, RTPROC_FLAGS_SEARCH_PATH | RTPROC_FLAGS_CWD,
                                            &HndIn, NULL, NULL,
                                            NULL, NULL, (void *)pszGitRepoPath,
                                            &pThis->hProcFastImport);
                        RTPipeClose(hPipeFiR);
                        if (RT_SUCCESS(rc))
                        {
                            if (fIncremental)
                            {
                                /* Reload all branches. */
                                PS2GBRANCH pIt;
                                RTListForEach(&pThis->LstBranches, pIt, S2GBRANCH, NdBranches)
                                {
                                    s2gScratchBufReset(&pThis->BufScratch);
                                    rc = s2gScratchBufPrintf(&pThis->BufScratch,
                                                             "reset refs/heads/%s\n"
                                                             "from refs/heads/%s^0\n\n",
                                                             pIt->szName, pIt->szName);
                                    if (RT_SUCCESS(rc))
                                        rc = s2gGitWrite(pThis, pThis->BufScratch.pbBuf, pThis->BufScratch.offBuf);
                                    if (RT_FAILURE(rc))
                                        break;
                                }
                            }

                            if (RT_SUCCESS(rc))
                            {
                                *phGitRepo = pThis;
                                return VINF_SUCCESS;
                            }
                        }
                        else
                            RTPipeClose(pThis->hPipeWrite);
                    }
                }

                if (pThis->hFileDump != NIL_RTFILE)
                {
                    RTFileClose(pThis->hFileDump);
                    RTFileDelete(pszDumpFilename);
                }

                RTStrFree(pThis->pszGitRepoPath);
            }
            else
                rc = VERR_NO_MEMORY;

            RTMemFree(pThis);
        }
        else
            rc = VERR_NO_MEMORY;
    }

    return rc;
}


DECLHIDDEN(int) s2gGitRepositoryFlush(S2GREPOSITORYGIT hGitRepo)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;
    return s2gGitWrite(pThis, "checkpoint\n", sizeof("checkpoint\n") - 1);
}


DECLHIDDEN(int) s2gGitRepositoryDone(S2GREPOSITORYGIT hGitRepo)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    int rc = s2gGitRepositoryFlush(hGitRepo);
    if (RT_FAILURE(rc))
        return rc;

    RTPipeClose(pThis->hPipeWrite);
    pThis->hPipeWrite = NIL_RTPIPE;

    RTPROCSTATUS Sts;
    rc = RTProcWait(pThis->hProcFastImport, RTPROCWAIT_FLAGS_BLOCK, &Sts);
    if (RT_SUCCESS(rc))
    {
        if (   Sts.enmReason != RTPROCEXITREASON_NORMAL
            || Sts.iStatus != 0)
            rc = VERR_INVALID_HANDLE; /** @todo */
        pThis->hProcFastImport = NIL_RTPROCESS;
    }

    if (pThis->hFileDump != NIL_RTFILE)
    {
        RTFileClose(pThis->hFileDump);
        pThis->hFileDump = NIL_RTFILE;
    }

    return rc;
}


DECLHIDDEN(int) s2gGitRepositoryPush(S2GREPOSITORYGIT hGitRepo)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    if (pThis->fIncremental)
    {
        const char *apszArgsPush[] = { GIT_BINARY, "push", "--all", "origin", NULL };
        return s2gGitExecWrapper(GIT_BINARY, pThis->pszGitRepoPath, &apszArgsPush[0]);
    }

    /* Do a possibly lenghty repack first. */
    const char *apszArgsRepack[] = { GIT_BINARY, "repack", "-adf", NULL };
    int rc = s2gGitExecWrapper(GIT_BINARY, pThis->pszGitRepoPath, &apszArgsRepack[0]);
    if (RT_SUCCESS(rc))
    {
        /* Do the push now. */
        const char *apszArgsPush[] = { GIT_BINARY, "push", "--force", "--all", "origin", NULL };
        rc = s2gGitExecWrapper(GIT_BINARY, pThis->pszGitRepoPath, &apszArgsPush[0]);
    }

    return rc;
}


DECLHIDDEN(int) s2gGitRepositoryClose(S2GREPOSITORYGIT hGitRepo)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    int rc = VINF_SUCCESS;
    if (pThis->hPipeWrite != NIL_RTPIPE)
    {
        rc = s2gGitRepositoryDone(hGitRepo);
        if (RT_FAILURE(rc))
            return rc;
    }

    s2gScratchBufFree(&pThis->BufModifiedFiles);
    s2gScratchBufFree(&pThis->BufScratch);
    RTStrFree(pThis->pszGitRepoPath);
    RTMemFree(pThis);
    return rc;
}


DECLHIDDEN(int) s2gGitRepositoryClone(S2GREPOSITORYGIT hGitRepo, const char *pszWorktree)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    const char *apszArgs[] = { GIT_BINARY, "clone", pThis->pszGitRepoPath, pszWorktree, NULL };
    return s2gGitExecWrapper(GIT_BINARY, pThis->pszGitRepoPath, &apszArgs[0]);
}


DECLHIDDEN(int) s2gGitRepositoryCheckout(const char *pszWorktree, const char *pszCommitHash)
{
    const char *apszArgs[] = { GIT_BINARY, "checkout", "--quiet", pszCommitHash, NULL };
    return s2gGitExecWrapper(GIT_BINARY, pszWorktree, &apszArgs[0]);
}


DECLHIDDEN(int) s2gGitRepositoryQueryCommits(S2GREPOSITORYGIT hGitRepo, PS2GGITCOMMIT2SVNREV *ppaCommits, uint32_t *pcCommits)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    S2GSCRATCHBUF StdOut;
    s2gScratchBufInit(&StdOut);

    const char *apszArgsCount[] = { GIT_BINARY, "rev-list", "--count", "--all", NULL };
    int rc = s2gGitExecWrapperStdOut(GIT_BINARY, pThis->pszGitRepoPath, &apszArgsCount[0], &StdOut);
    if (RT_SUCCESS(rc))
    {
        uint32_t cCommits = 0;
        rc = RTStrToUInt32Ex(StdOut.pbBuf, NULL, 10, &cCommits);
        if (RT_SUCCESS(rc))
        {
            s2gScratchBufReset(&StdOut);
            const char *apszArgs[] = { GIT_BINARY, "log", "--no-abbrev", "--oneline", "--format=%H %B", NULL };
            rc = s2gGitExecWrapperStdOut(GIT_BINARY, pThis->pszGitRepoPath, &apszArgs[0], &StdOut);
            if (RT_SUCCESS(rc))
            {
                PS2GGITCOMMIT2SVNREV paCommits = (PS2GGITCOMMIT2SVNREV)RTMemAllocZ(cCommits * sizeof(*paCommits));
                if (paCommits)
                {
                    char *pch = StdOut.pbBuf;
                    uint32_t idCommit = 0;
                    while (*pch != '\0')
                    {
                        if (pch[RTSHA1_DIGEST_LEN] != ' ')
                        {
                            AssertFailed();
                            rc = VERR_INVALID_STATE;
                            break;
                        }

                        memcpy(&paCommits[idCommit].szCommitHash[0], pch, RTSHA1_DIGEST_LEN);
                        paCommits[idCommit].szCommitHash[RTSHA1_DIGEST_LEN] = '\0';

                        pch += RTSHA1_DIGEST_LEN;
                        const char *pchRevision = RTStrStr(pch, "svn:sync-xref-src-repo-rev: ");
                        if (pchRevision)
                        {
                            pchRevision += sizeof("svn:sync-xref-src-repo-rev: ") - 1;
                            if (*pchRevision == 'r')
                            {
                                char *pszNext = NULL;
                                rc = RTStrToUInt32Ex(pchRevision + 1, &pszNext, 10, &paCommits[idCommit].idSvnRev);
                                if (RT_SUCCESS(rc))
                                    pch = pszNext;
                                else if (!strncmp(pchRevision + 1, "<NULL>", sizeof("<NULL>") - 1))
                                {
                                    paCommits[idCommit].idSvnRev = 17427;
                                    rc = VINF_SUCCESS;
                                    pch = (char *)pchRevision + 1 + sizeof("<NULL>") - 1;
                                }
                                else
                                    AssertFailed();
                            }
                            else
                            {
                                AssertFailed();
                                rc = VERR_INVALID_STATE;
                            }
                        }
                        else
                        {
                            AssertFailed();
                            rc = VERR_INVALID_STATE;
                        }

                        if (RT_FAILURE(rc))
                            break;

                        while (*pch == '\n')
                            pch++;

                        idCommit++;
                    }

                    if (RT_SUCCESS(rc))
                    {
                        *ppaCommits = paCommits;
                        *pcCommits  = cCommits;
                    }
                    else
                        RTMemFree(paCommits);
                }
                else
                    rc = VERR_NO_MEMORY;
            }
        }
    }

    s2gScratchBufFree(&StdOut);
    return rc;
}


DECLHIDDEN(bool) s2gGitBranchExists(S2GREPOSITORYGIT hGitRepo, const char *pszName)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;
    return s2gGitGetBranch(pThis, pszName) != NULL;
}


DECLHIDDEN(int) s2gGitBranchCreate(S2GREPOSITORYGIT hGitRepo, const char *pszName, const char *pszBranchAncestor,
                                   uint32_t idRevAncestor)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    uint64_t idMark = 0;
    int rc = s2gGitBranchQueryMarkFromSvnRev(pThis, pszBranchAncestor, idRevAncestor, &idMark);
    if (RT_FAILURE(rc))
        return rc;

    PS2GBRANCH pBranch = s2gGitBranchCreateWorker(pszName, strlen(pszName));
    if (pBranch)
    {
        pBranch->idGitMarkMerge = idMark;
        RTListAppend(&pThis->LstBranches, &pBranch->NdBranches);
        return VINF_SUCCESS;
    }

    return VERR_NO_MEMORY;
}


DECLHIDDEN(int) s2gGitTransactionStart(S2GREPOSITORYGIT hGitRepo)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    if ((pThis->idCommitMark % 10000) == 0)
    {
        int rc = s2gGitWrite(pThis, "checkpoint\n", sizeof("checkpoint\n") - 1);
        if (RT_FAILURE(rc))
            return rc;

        pThis->idCommitMark = 1;
    }

    pThis->idFileMark = UINT64_MAX - 1;
    s2gScratchBufReset(&pThis->BufModifiedFiles);
    return VINF_SUCCESS;
}


DECLHIDDEN(int) s2gGitTransactionCommit(S2GREPOSITORYGIT hGitRepo, const char *pszCommitter, const char *pszCommitterEmail,
                                        const char *pszAuthor, const char *pszAuthorEmail,
                                        const char *pszLog, int64_t cEpochSecs, const char *pszBranch, uint32_t idSvnRev)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    AssertReturn(pszCommitter && pszCommitterEmail, VERR_INVALID_PARAMETER);

    PS2GBRANCH pBranch = s2gGitGetBranch(pThis, pszBranch);
    if (!pBranch)
        return VERR_NOT_FOUND;

    s2gScratchBufReset(&pThis->BufScratch);
    size_t cchLog = strlen(pszLog);
    uint64_t const idMark = pThis->idCommitMark++;
    int rc = s2gScratchBufPrintf(&pThis->BufScratch,
                                 "commit refs/heads/%s\n"
                                 "mark :%RU64\n"
                                 "committer %s <%s> %RI64 +0000\n",
                                 pszBranch, idMark,
                                 pszCommitter, pszCommitterEmail, cEpochSecs);
    if (RT_SUCCESS(rc) && pszAuthor && pszAuthorEmail)
        rc = s2gScratchBufPrintf(&pThis->BufScratch,
                                 "author %s <%s> %RI64 +0000\n",
                                 pszAuthor, pszAuthorEmail, cEpochSecs);
    if (RT_SUCCESS(rc))
        rc = s2gScratchBufPrintf(&pThis->BufScratch,
                                 "data %zu\n"
                                 "%s\n",
                                 cchLog, pszLog);
    if (RT_SUCCESS(rc) && pBranch->idGitMarkMerge != UINT64_MAX)
    {
        rc = s2gScratchBufPrintf(&pThis->BufScratch, "merge :%RU64\ndeleteall\n", pBranch->idGitMarkMerge);
        pBranch->idGitMarkMerge = UINT64_MAX;
    }
    if (RT_SUCCESS(rc))
    {
        rc = s2gGitWrite(pThis, pThis->BufScratch.pbBuf, pThis->BufScratch.offBuf);
        if (RT_SUCCESS(rc) && pThis->BufModifiedFiles.offBuf)
            rc = s2gGitWrite(pThis, pThis->BufModifiedFiles.pbBuf, pThis->BufModifiedFiles.offBuf);
        if (RT_SUCCESS(rc))
            rc = s2gGitBranchAssociateMarkWithSvnRev(pBranch, idMark, idSvnRev);
    }

    return rc;
}


static int s2gGitTransactionFileAddWorker(S2GREPOSITORYGIT hGitRepo, const char *pszPath, const char *pszMode, uint64_t cbFile)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;
    s2gScratchBufReset(&pThis->BufScratch);
    uint64_t idFileMark = pThis->idFileMark--;
    int rc = s2gScratchBufPrintf(&pThis->BufModifiedFiles, "M %s :%RU64 %s\n",
                                 pszMode, idFileMark, pszPath);
    if (RT_SUCCESS(rc))
    {
        rc = s2gScratchBufPrintf(&pThis->BufScratch,
                                 "blob\n"
                                 "mark :%RU64\n"
                                 "data %RU64\n",
                                 idFileMark, cbFile);
        if (RT_SUCCESS(rc))
            rc = s2gGitWrite(pThis, pThis->BufScratch.pbBuf, pThis->BufScratch.offBuf);
    }

    return rc;
}


DECLHIDDEN(int) s2gGitTransactionFileAdd(S2GREPOSITORYGIT hGitRepo, const char *pszPath, bool fIsExec, uint64_t cbFile)
{
    return s2gGitTransactionFileAddWorker(hGitRepo, pszPath, fIsExec ? "100755" : "100644", cbFile);
}


DECLHIDDEN(int) s2gGitTransactionFileWriteData(S2GREPOSITORYGIT hGitRepo, const void *pvBuf, size_t cb)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;

    int rc = VINF_SUCCESS;
    if (cb)
        rc = s2gGitWrite(pThis, pvBuf, cb);

    if (RT_SUCCESS(rc)) /* Need to print an ending line after the file data. */
        rc = s2gGitWrite(pThis, "\n", sizeof("\n") - 1);

    return rc;
}


DECLHIDDEN(int) s2gGitTransactionFileRemove(S2GREPOSITORYGIT hGitRepo, const char *pszPath)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;
    return s2gScratchBufPrintf(&pThis->BufModifiedFiles, "D %s\n", pszPath);
}


DECLHIDDEN(int) s2gGitTransactionSubmoduleAdd(S2GREPOSITORYGIT hGitRepo, const char *pszPath, const char *pszSha1CommitId)
{
    PS2GREPOSITORYGITINT pThis = hGitRepo;
    return s2gScratchBufPrintf(&pThis->BufModifiedFiles, "M 160000 %s %s\n", pszSha1CommitId, pszPath);
}


DECLHIDDEN(int) s2gGitTransactionLinkAdd(S2GREPOSITORYGIT hGitRepo, const char *pszPath, const void *pvData, size_t cbData)
{
    int rc = s2gGitTransactionFileAddWorker(hGitRepo, pszPath, "120000", cbData);
    if (RT_SUCCESS(rc))
        rc = s2gGitTransactionFileWriteData(hGitRepo, pvData, cbData);

    return rc;
}

