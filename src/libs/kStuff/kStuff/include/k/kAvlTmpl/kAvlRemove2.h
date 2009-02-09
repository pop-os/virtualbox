/* $Id: kAvlRemove2.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kAvlTmpl - Templated AVL Trees, Remove A Specific Node.
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
 * Removes the specified node from the tree.
 *
 * @returns Pointer to the removed node (NULL if not in the tree)
 * @param   ppTree      Pointer to Pointer to the tree root node.
 * @param   Key         The Key of which is to be found a best fitting match for..
 *
 * @remark  This implementation isn't the most efficient, but this short and
 *          easier to manage.
 */
KAVL_DECL(KAVLNODE *) KAVL_FN(Remove2)(KAVLTREEPTR *ppTree, KAVLNODE *pNode)
{
#ifdef KAVL_EQUAL_ALLOWED
    /*
     * Find the right node by key and see if it's what we want.
     */
    KAVLNODE *pParent;
    KAVLNODE *pCurNode = KAVL_FN(GetWithParent)(ppTree, pNode->mKey, &pParent);
    if (!pCurNode)
        return NULL;
    if (pCurNode != pNode)
    {
        /*
         * It's not the one we want, but it could be in the duplicate list.
         */
        while (pCurNode->mpList != KAVL_NULL)
        {
            KAVLNODE *pNext = KAVL_GET_POINTER(&pCurNode->mpList);
            if (pNext == pNode)
            {
                KAVL_SET_POINTER_NULL(&pCurNode->mpList, KAVL_GET_POINTER_NULL(&pNode->mpList));
                pNode->mpList = KAVL_NULL;
                return pNode;
            }
            pCurNode = pNext;
        }
        return NULL;
    }

    /*
     * Ok, it's the one we want alright.
     *
     * Simply remove it if it's the only one with they Key,
     * if there are duplicates we'll have to unlink it and
     * insert the first duplicate in our place.
     */
    if (pNode->mpList == KAVL_NODE)
        KAVL_FN(Remove)(ppTree, pNode->mKey);
    else
    {
        KAVLNODE *pNewUs = KAVL_GET_POINTER(&pNode->mpList);

        pNewUs->mHeight = pNode->mHeight;

        if (pNode->mpLeft != KAVL_NULL)
            KAVL_SET_POINTER(&pNewUs->mpLeft, KAVL_GET_POINTER(&pNode->mpLeft))
        else
            pNewUs->mpLeft = KAVL_NULL;

        if (pNode->mpRight != KAVL_NULL)
            KAVL_SET_POINTER(&pNewUs->mpRight, KAVL_GET_POINTER(&pNode->mpRight))
        else
            pNewUs->mpRight = KAVL_NULL;

        if (pParent)
        {
            if (KAVL_GET_POINTER_NULL(&pParent->mpLeft) == pNode)
                KAVL_SET_POINTER(&pParent->mpLeft, pNewUs);
            else
                KAVL_SET_POINTER(&pParent->mpRight, pNewUs);
        }
        else
            KAVL_SET_POINTER(ppTree, pNewUs);
    }
    return pNode;

#else
    /*
     * Delete it, if we got the wrong one, reinsert it.
     *
     * This ASSUMS that the caller is NOT going to hand us a lot
     * of wrong nodes but just uses this API for his convenience.
     */
    KAVLNODE *pRemovedNode = KAVL_FN(Remove)(ppTree, pNode->mKey);
    if (pRemovedNode == pNode)
        return pRemovedNode;

    KAVL_FN(Insert)(ppTree, pRemovedNode);
    return NULL;
#endif
}

