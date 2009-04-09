/* $Id: kAvlGet.h 7 2008-02-04 02:08:02Z bird $ */
/** @file
 * kAvlTmpl - Templated AVL Trees, Get a Node.
 */

/*
 * Copyright (c) 1999-2007 knut st. osmundsen <bird-src-spam@anduin.net>
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
 * Gets a node from the tree (does not remove it!)
 *
 * @returns Pointer to the node holding the given key.
 * @param   pRoot       Pointer to the AVL-tree root structure.
 * @param   Key         Key value of the node which is to be found.
 */
KAVL_DECL(KAVLNODE *) KAVL_FN(Get)(KAVLROOT *pRoot, KAVLKEY Key)
{
    KAVLNODE *pNode;
#ifdef KAVL_LOOKTHRU_CACHE
    KAVLTREEPTR *ppEntry;
#endif

    KAVL_READ_LOCK(pRoot);
    if (pRoot->mpRoot == KAVL_NULL)
    {
        KAVL_READ_UNLOCK(pRoot);
        return NULL;
    }

#ifdef KAVL_LOOKTHRU_CACHE
    ppEntry = &pRoot->maLookthru[KAVL_LOOKTHRU_HASH(Key)];
    pNode = KAVL_GET_POINTER_NULL(ppEntry);
    if (!pNode || KAVL_NE(pNode->mKey, Key))
#endif
    {
        pNode = KAVL_GET_POINTER(&pRoot->mpRoot);
        while (KAVL_NE(pNode->mKey, Key))
        {
            if (KAVL_G(pNode->mKey, Key))
            {
                if (pNode->mpLeft == KAVL_NULL)
                {
                    KAVL_READ_UNLOCK(pRoot);
                    return NULL;
                }
                pNode = KAVL_GET_POINTER(&pNode->mpLeft);
            }
            else
            {
                if (pNode->mpRight == KAVL_NULL)
                {
                    KAVL_READ_UNLOCK(pRoot);
                    return NULL;
                }
                pNode = KAVL_GET_POINTER(&pNode->mpRight);
            }
        }
    
#ifdef KAVL_LOOKTHRU_CACHE
        KAVL_SET_POINTER(ppEntry, pNode);
#endif
    }

    KAVL_READ_UNLOCK(pRoot);
    return pNode;
}

