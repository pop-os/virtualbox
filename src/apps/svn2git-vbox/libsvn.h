/* $Id: libsvn.h $ */
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

#ifndef VBOX_INCLUDED_SRC_svn2git_vbox_libsvn_h
#define VBOX_INCLUDED_SRC_svn2git_vbox_libsvn_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

#include <iprt/cdefs.h>

#if !defined(SVN2GIT_WITH_LAZYLOAD)

# include <apr_lib.h>
# include <apr_getopt.h>
# include <apr_general.h>

# include <svn_fs.h>
# include <svn_pools.h>
# include <svn_repos.h>
# include <svn_types.h>
# include <svn_version.h>
# include <svn_subst.h>
# include <svn_props.h>
# include <svn_time.h>

#else

/* APR defines */
#define APR_SUCCESS         0

#define APR_HASH_KEY_STRING (-1)

#define FALSE               0
#define TRUE                1

/* APR types. */
typedef int                     apr_status_t;
typedef struct apr_allocator_t  apr_allocator_t;
typedef struct apr_pool_t       apr_pool_t;
typedef struct apr_hash_t       apr_hash_t;
typedef struct apr_hash_index_t apr_hash_index_t;
typedef size_t                  apr_size_t;
typedef ssize_t                 apr_ssize_t;
typedef int64_t                 apr_int64_t;
typedef apr_int64_t             apr_time_t;

/* SVN defines. */
#define SVN_PROP_PREFIX "svn:"
#define SVN_PROP_EOL_STYLE SVN_PROP_PREFIX "eol-style"
#define SVN_PROP_KEYWORDS  SVN_PROP_PREFIX "keywords"

#define svn_error_trace(expr) do {} while(0)
#define svn_pool_create(parent_pool) svn_pool_create_ex(parent_pool, NULL)
#define svn_pool_destroy(pool) apr_pool_destroy(pool)
#define svn_pool_clear(pool)   apr_pool_clear(pool)

/* SVN types. */
typedef int svn_boolean_t;
typedef long int svn_revnum_t;

typedef struct svn_stream_t     svn_stream_t;
typedef struct svn_fs_id_t      svn_fs_id_t;
typedef struct svn_fs_root_t    svn_fs_root_t;
typedef struct svn_repos_t      svn_repos_t;
typedef struct svn_fs_t         svn_fs_t;
typedef struct svn_fs_history_t svn_fs_history_t;

typedef enum svn_subst_eol_style
{
    svn_subst_eol_style_unknown = 0,
    svn_subst_eol_style_none,
    svn_subst_eol_style_native,
    svn_subst_eol_style_fixed
} svn_subst_eol_style_t;

typedef enum svn_fs_path_change_kind
{
    svn_fs_path_change_modify = 0,
    svn_fs_path_change_add,
    svn_fs_path_change_delete,
    svn_fs_path_change_replace,
    svn_fs_path_change_reset
} svn_fs_path_change_kind_t;

typedef enum svn_node_kind
{
    svn_node_none = 0,
    svn_node_file,
    svn_node_dir,
    svn_node_unknown,
    svn_node_symlink
} svn_node_kind_t;

typedef struct svn_error_t
{
    apr_status_t       apr_err;
    const char         *message;
    struct svn_error_t *child;
    apr_pool_t         *pool;
    const char         *file;
    long               line;
} svn_error_t;

typedef struct svn_string_t
{
    const char *data;
    apr_size_t len;
} svn_string_t;

typedef struct svn_fs_path_change2_t
{
    const svn_fs_id_t         *node_rev_id;
    svn_fs_path_change_kind_t change_kind;
    svn_boolean_t             text_mod;
    svn_boolean_t             prop_mod;
    svn_node_kind_t           node_kind;
    svn_boolean_t             copyfrom_known;
    svn_revnum_t              copyfrom_rev;
    const char                *copyfrom_path;
} svn_fs_path_change2_t;

typedef struct svn_fs_dirent_t
{
    const char        *name;
    const svn_fs_id_t *id;
    svn_node_kind_t   kind;
} svn_fs_dirent_t;


RT_C_DECLS_BEGIN

/* APR functions. */
apr_status_t apr_initialize(void);
void         apr_terminate(void);
void         apr_pool_clear(apr_pool_t *p);
void         apr_pool_destroy(apr_pool_t *p);
unsigned int     apr_hash_count(apr_hash_t *ht);
apr_hash_index_t *apr_hash_first(apr_pool_t *p, apr_hash_t *ht);
apr_hash_index_t *apr_hash_next(apr_hash_index_t *hi);
void             apr_hash_this(apr_hash_index_t *hi, const void **key, apr_ssize_t *klen, void **val);
void             *apr_hash_get(apr_hash_t *ht, const void *key, apr_ssize_t klen);

/* SVN functions */
apr_pool_t  *svn_pool_create_ex(apr_pool_t *parent_pool, apr_allocator_t *allocator);
svn_error_t *svn_repos_open3(svn_repos_t **repos_p, const char *path, apr_hash_t *fs_config,
                             apr_pool_t *result_pool, apr_pool_t *scratch_pool);
svn_fs_t    *svn_repos_fs(svn_repos_t *repos);
svn_error_t *svn_fs_youngest_rev(svn_revnum_t *youngest_p, svn_fs_t *fs, apr_pool_t *pool);
svn_error_t *svn_fs_node_prop(svn_string_t **value_p, svn_fs_root_t *root, const char *path, const char *propname,
                              apr_pool_t *pool);
svn_error_t *svn_fs_dir_entries(apr_hash_t **entries_p, svn_fs_root_t *root, const char *path, apr_pool_t *pool);
svn_error_t *svn_fs_revision_root(svn_fs_root_t **root_p, svn_fs_t *fs, svn_revnum_t rev, apr_pool_t *pool);
svn_error_t *svn_fs_file_contents(svn_stream_t **contents, svn_fs_root_t *root, const char *path, apr_pool_t *pool);
svn_error_t *svn_fs_node_proplist(apr_hash_t **table_p, svn_fs_root_t *root, const char *path, apr_pool_t *pool);
svn_error_t *svn_fs_is_dir(svn_boolean_t *is_dir, svn_fs_root_t *root, const char *path, apr_pool_t *pool);
svn_error_t *svn_fs_revision_proplist(apr_hash_t **table_p, svn_fs_t *fs, svn_revnum_t rev, apr_pool_t *pool);
svn_error_t *svn_fs_paths_changed2(apr_hash_t **changed_paths2_p, svn_fs_root_t *root, apr_pool_t *pool);
svn_error_t *svn_fs_revision_prop(svn_string_t **value_p, svn_fs_t *fs, svn_revnum_t rev,
                                  const char *propname, apr_pool_t *pool);
svn_error_t *svn_fs_node_history(svn_fs_history_t **history_p, svn_fs_root_t *root, const char *path, apr_pool_t *pool);
svn_error_t *svn_fs_history_prev(svn_fs_history_t **prev_history_p, svn_fs_history_t *history,
                                 svn_boolean_t cross_copies, apr_pool_t *pool);
svn_error_t *svn_fs_history_location(const char **path, svn_revnum_t *revision, svn_fs_history_t *history, apr_pool_t *pool);

void svn_subst_eol_style_from_value(svn_subst_eol_style_t *style, const char **eol, const char *value);
svn_error_t *svn_subst_build_keywords3(apr_hash_t **kw, const char *keywords_string, const char *rev, const char *url,
                                       const char *repos_root_url, apr_time_t date, const char *author, apr_pool_t *pool);
svn_stream_t *svn_subst_stream_translated(svn_stream_t *stream, const char *eol_str, svn_boolean_t repair,
                                          apr_hash_t *keywords, svn_boolean_t expand, apr_pool_t *result_pool);

svn_stream_t *svn_stream_disown(svn_stream_t *stream, apr_pool_t *pool);
svn_error_t  *svn_stream_read_full(svn_stream_t *stream, char *buffer, apr_size_t *len);

svn_error_t *svn_time_from_cstring(apr_time_t *whem, const char *data, apr_pool_t *pool);

RT_C_DECLS_END

#endif

#endif /* !VBOX_INCLUDED_SRC_svn2git_vbox_libsvn_h */

