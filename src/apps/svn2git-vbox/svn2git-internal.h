/* $Id: svn2git-internal.h $ */
/** @file
 * IPRT - Internal svn2git header.
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

#ifndef VBOX_INCLUDED_SRC_svn2git_vbox_svn2git_internal_h
#define VBOX_INCLUDED_SRC_svn2git_vbox_svn2git_internal_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#include <iprt/cdefs.h>
#include <iprt/errcore.h>
#include <iprt/mem.h>
#include <iprt/sha.h>
#include <iprt/string.h>

RT_C_DECLS_BEGIN

typedef struct S2GSCRATCHBUF
{
    /** Pointer to the buffer. */
    char            *pbBuf;
    /** Size of the buffer. */
    size_t          cbBuf;
    /** Offset where to append data next. */
    size_t          offBuf;
} S2GSCRATCHBUF;
typedef S2GSCRATCHBUF *PS2GSCRATCHBUF;


static void s2gScratchBufInit(PS2GSCRATCHBUF pBuf)
{
    pBuf->pbBuf = NULL;
    pBuf->cbBuf  = 0;
    pBuf->offBuf = 0;
}


DECLINLINE(void) s2gScratchBufFree(PS2GSCRATCHBUF pBuf)
{
    if (pBuf->pbBuf)
        RTMemFree(pBuf->pbBuf);
}


DECLINLINE(void) s2gScratchBufReset(PS2GSCRATCHBUF pBuf)
{
    pBuf->offBuf = 0;
}


DECLINLINE(void) s2gScratchBufAdvance(PS2GSCRATCHBUF pBuf, size_t cb)
{
    pBuf->offBuf += cb;
}


DECLINLINE(void *) s2gScratchBufEnsureSize(PS2GSCRATCHBUF pBuf, size_t cbFree)
{
    if (pBuf->cbBuf - pBuf->offBuf < cbFree)
    {
        size_t cbAdd = RT_ALIGN_Z(cbFree - (pBuf->cbBuf - pBuf->offBuf), _4K);
        char *pbBufNew = (char *)RTMemRealloc(pBuf->pbBuf, pBuf->cbBuf + cbAdd);
        if (!pbBufNew)
            return NULL;

        pBuf->pbBuf  = pbBufNew;
        pBuf->cbBuf += cbAdd;
    }

    return pBuf->pbBuf + pBuf->offBuf;
}


DECLINLINE(int) s2gScratchBufPrintf(PS2GSCRATCHBUF pBuf, const char *pszFmt, ...) RT_IPRT_FORMAT_ATTR(1, 2)
{
    /* Ensure we have at least 1 byte free to not make RTStrePrintf2V assert. */
    if (!s2gScratchBufEnsureSize(pBuf, 1))
        return VERR_NO_MEMORY;

    va_list va;
    va_start(va, pszFmt);

    int rc = VINF_SUCCESS;
    ssize_t cchReq = RTStrPrintf2V(pBuf->pbBuf + pBuf->offBuf, pBuf->cbBuf - pBuf->offBuf,
                                   pszFmt, va);
    if (cchReq < 0)
    {
        size_t cbBufNew = RT_ALIGN_Z((-cchReq) + pBuf->cbBuf, _4K);
        char *pbBufNew = (char *)RTMemRealloc(pBuf->pbBuf, cbBufNew);
        if (pbBufNew)
        {
            pBuf->pbBuf = pbBufNew;
            pBuf->cbBuf  = cbBufNew;
            cchReq = RTStrPrintf2V(pBuf->pbBuf + pBuf->offBuf, pBuf->cbBuf - pBuf->offBuf,
                                   pszFmt, va);
            Assert(cchReq > 0);
        }
        else
            rc = VERR_NO_MEMORY;
    }
    if (RT_SUCCESS(rc))
        pBuf->offBuf += cchReq;

    va_end(va);
    return rc;
}


DECLINLINE(int) s2gScratchBufWrite(PS2GSCRATCHBUF pBuf, const void *pvBuf, size_t cbWrite)
{
    if (pBuf->cbBuf - pBuf->offBuf < cbWrite)
    {
        size_t cbBufNew = RT_ALIGN_Z(cbWrite - (pBuf->cbBuf - pBuf->offBuf), _4K);
        char *pbBufNew = (char *)RTMemRealloc(pBuf->pbBuf, cbBufNew);
        if (!pbBufNew)
            return VERR_NO_MEMORY;

        pBuf->pbBuf = pbBufNew;
        pBuf->cbBuf = cbBufNew;
    }

    memcpy(pBuf->pbBuf + pBuf->offBuf, pvBuf, cbWrite);
    pBuf->offBuf += cbWrite;
    return VINF_SUCCESS;
}


/* git.cpp */
typedef struct S2GREPOSITORYGITINT *S2GREPOSITORYGIT;
typedef S2GREPOSITORYGIT *PS2GREPOSITORYGIT;

typedef struct S2GGITCOMMIT2SVNREV
{
    /** The internal subversion revision number. */
    uint32_t            idSvnRev;
    /** The corresponding commit hash. */
    char                szCommitHash[RTSHA1_DIGEST_LEN + 1];
} S2GGITCOMMIT2SVNREV;
typedef S2GGITCOMMIT2SVNREV *PS2GGITCOMMIT2SVNREV;
typedef const S2GGITCOMMIT2SVNREV *PCS2GGITCOMMIT2SVNREV;

DECLHIDDEN(int) s2gGitRepositoryCreate(PS2GREPOSITORYGIT phGitRepo, const char *pszGitRepoPath, const char *pszDefaultBranch,
                                       const char *pszGitPushOrigin, const char *pszDumpFilename, uint32_t *pidRevLast);
DECLHIDDEN(int) s2gGitRepositoryFlush(S2GREPOSITORYGIT hGitRepo);
DECLHIDDEN(int) s2gGitRepositoryDone(S2GREPOSITORYGIT hGitRepo);
DECLHIDDEN(int) s2gGitRepositoryPush(S2GREPOSITORYGIT hGitRepo);
DECLHIDDEN(int) s2gGitRepositoryClose(S2GREPOSITORYGIT hGitRepo);
DECLHIDDEN(int) s2gGitRepositoryClone(S2GREPOSITORYGIT hGitRepo, const char *pszWorktree);
DECLHIDDEN(int) s2gGitRepositoryCheckout(const char *pszWorktree, const char *pszCommitHash);
DECLHIDDEN(int) s2gGitRepositoryQueryCommits(S2GREPOSITORYGIT hGitRepo, PS2GGITCOMMIT2SVNREV *ppaCommits, uint32_t *pcCommits);

DECLHIDDEN(bool) s2gGitBranchExists(S2GREPOSITORYGIT hGitRepo, const char *pszName);
DECLHIDDEN(int) s2gGitBranchCreate(S2GREPOSITORYGIT hGitRepo, const char *pszName, const char *pszBranchAncestor,
                                   uint32_t idRevAncestor);

DECLHIDDEN(int) s2gGitTransactionStart(S2GREPOSITORYGIT hGitRepo);
DECLHIDDEN(int) s2gGitTransactionCommit(S2GREPOSITORYGIT hGitRepo, const char *pszCommitter, const char *pszCommitterEmail,
                                        const char *pszAuthor, const char *pszAuthorEmail,
                                        const char *pszLog, int64_t cEpochSecs, const char *pszBranch, uint32_t idSvnRev);
DECLHIDDEN(int) s2gGitTransactionFileAdd(S2GREPOSITORYGIT hGitRepo, const char *pszPath, bool fIsExec, uint64_t cbFile);
DECLHIDDEN(int) s2gGitTransactionFileWriteData(S2GREPOSITORYGIT hGitRepo, const void *pvBuf, size_t cb);
DECLHIDDEN(int) s2gGitTransactionFileRemove(S2GREPOSITORYGIT hGitRepo, const char *pszPath);
DECLHIDDEN(int) s2gGitTransactionSubmoduleAdd(S2GREPOSITORYGIT hGitRepo, const char *pszPath, const char *pszSha1CommitId);
DECLHIDDEN(int) s2gGitTransactionLinkAdd(S2GREPOSITORYGIT hGitRepo, const char *pszPath, const void *pvData, size_t cbData);


RT_C_DECLS_END

#endif /* !VBOX_INCLUDED_SRC_svn2git_vbox_svn2git_internal_h */

