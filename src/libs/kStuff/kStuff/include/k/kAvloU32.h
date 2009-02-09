/* $Id: kAvloU32.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kAvl - AVL Tree Implementation, KU32 keys, Offset Based.
 */

/*
 * Copyright (c) 2007 knut st. osmundsen <bird-src-spam@anduin.net>
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

#ifndef ___k_kAvloU32_h___
#define ___k_kAvloU32_h___

typedef KI32 KAVLOU32PTR;

typedef struct KAVLOU32
{
    KU32                u32;
    KU8                 cFloorsToGo;
    KAVLOU32PTR         offLeft;
    KAVLOU32PTR         offRight;
} KAVLOU32, *PKAVLOU32, **PPKAVLOU32;

#define mKey                    u32
#define mHeight                 cFloorsToGo
#define mpLeft                  offLeft
#define mpRight                 offRight

/*#define KAVL_EQUAL_ALLOWED*/
#define KAVL_CHECK_FOR_EQUAL_INSERT
#define KAVL_MAX_STACK          32
/*#define KAVL_RANGE */
#define KAVL_OFFSET
#define KAVL_STD_KEY_COMP
#define KAVLKEY                 KU32
#define KAVLTREEPTR             KAVLOU32PTR
#define KAVLNODE                KAVLOU32
#define KAVL_FN(name)           kAvloU32 ## name
#define KAVL_TYPE(prefix,name)  prefix ## KAVLOU32 ## name
#define KAVL_INT(name)          KAVLOU32INT ## name
#define KAVL_DECL(rettype)      K_DECL_INLINE(rettype)

#include <k/kAvlTmpl/kAvlBase.h>
#include <k/kAvlTmpl/kAvlDoWithAll.h>
#include <k/kAvlTmpl/kAvlEnum.h>
#include <k/kAvlTmpl/kAvlGet.h>
#include <k/kAvlTmpl/kAvlGetBestFit.h>
#include <k/kAvlTmpl/kAvlGetWithParent.h>
#include <k/kAvlTmpl/kAvlRemove2.h>
#include <k/kAvlTmpl/kAvlRemoveBestFit.h>
#include <k/kAvlTmpl/kAvlUndef.h>

#endif


