/* $Id: kAvlUndef.h 7 2008-02-04 02:08:02Z bird $ */
/** @file
 * kAvlTmpl - Undefines All Macros (both config and temp).
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
 *
 * As a special exception, since this is a source file and not a header
 * file, you are granted permission to #include this file as you wish
 * without this in itself causing the resulting program or whatever to be
 * covered by the LGPL  license. This exception does not however invalidate
 * any other reasons why the resulting program/whatever should not be
 * covered the LGPL or GPL.
 */

/*
 * The configuration.
 */
#undef KAVL_EQUAL_ALLOWED
#undef KAVL_CHECK_FOR_EQUAL_INSERT
#undef KAVL_MAX_STACK
#undef KAVL_RANGE
#undef KAVL_OFFSET
#undef KAVL_STD_KEY_COMP
#undef KAVL_LOOKTHRU
#undef KAVL_LOOKTHRU_HASH
#undef KAVL_LOCKED
#undef KAVL_WRITE_LOCK
#undef KAVL_WRITE_UNLOCK
#undef KAVL_READ_LOCK
#undef KAVL_READ_UNLOCK
#undef KAVLKEY
#undef KAVLNODE
#undef KAVLTREEPTR
#undef KAVLROOT
#undef KAVL_FN
#undef KAVL_TYPE
#undef KAVL_INT
#undef KAVL_DECL
#undef mKey
#undef mKeyLast
#undef mHeight
#undef mpLeft
#undef mpRight
#undef mpList
#undef mpRoot
#undef maLookthru

/*
 * The internal macros.
 */
#undef KAVL_HEIGHTOF
#undef KAVL_GET_POINTER
#undef KAVL_GET_POINTER_NULL
#undef KAVL_SET_POINTER
#undef KAVL_SET_POINTER_NULL
#undef KAVL_NULL
#undef KAVL_G
#undef KAVL_E
#undef KAVL_NE
#undef KAVL_R_IS_IDENTICAL
#undef KAVL_R_IS_INTERSECTING
#undef KAVL_R_IS_IN_RANGE

