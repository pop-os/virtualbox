/* $Id: mz.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * MZ structures, types and defines.
 */

#ifndef ___k_kLdrFmts_mz_h___
#define ___k_kLdrFmts_mz_h___

#include <k/kDefs.h>
#include <k/kTypes.h>

#pragma pack(1) /* not required */

typedef struct _IMAGE_DOS_HEADER
{
    KU16       e_magic;
    KU16       e_cblp;
    KU16       e_cp;
    KU16       e_crlc;
    KU16       e_cparhdr;
    KU16       e_minalloc;
    KU16       e_maxalloc;
    KU16       e_ss;
    KU16       e_sp;
    KU16       e_csum;
    KU16       e_ip;
    KU16       e_cs;
    KU16       e_lfarlc;
    KU16       e_ovno;
    KU16       e_res[4];
    KU16       e_oemid;
    KU16       e_oeminfo;
    KU16       e_res2[10];
    KU32       e_lfanew;
} IMAGE_DOS_HEADER;
typedef IMAGE_DOS_HEADER *PIMAGE_DOS_HEADER;

#ifndef IMAGE_DOS_SIGNATURE
# define IMAGE_DOS_SIGNATURE K_LE2H_U16('M' | ('Z' << 8))
#endif

#pragma pack()

#endif

