/* $Id: kAvlGetWithParent.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kAvlTmpl - Templated AVL Trees, Get Node With Parent.
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


/**
 * Gets a node from the tree and its parent node (if any).
 * The tree remains unchanged.
 *
 * @returns   Pointer to the node holding the given key.
 * @param     ppTree    Pointer to the AVL-tree root node pointer.
 * @param     ppParent  Pointer to a variable which will hold the pointer to the partent node on
 *                      return. When no node is found, this will hold the last searched node.
 * @param     Key       Key value of the node which is to be found.
 */
KAVL_DECL(KAVLNODE *) KAVL_FN(GetWithParent)(KAVLTREEPTR *ppTree, KAVLNODE **ppParent, KAVLKEY Key)
{
    register KAVLNODE *pNode = KAVL_GET_POINTER_NULL(ppTree);
    register KAVLNODE *pParent = NULL;

    while (     pNode != NULL
           &&   KAVL_NE(pNode->mKey, Key))
    {
        pParent = pNode;
        if (KAVL_G(pNode->mKey, Key))
            pNode = KAVL_GET_POINTER_NULL(&pNode->mpLeft);
        else
            pNode = KAVL_GET_POINTER_NULL(&pNode->mpRight);
    }

    *ppParent = pParent;
    return pNode;
}

