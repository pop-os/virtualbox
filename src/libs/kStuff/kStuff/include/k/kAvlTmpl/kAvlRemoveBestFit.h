/* $Id: kAvlRemoveBestFit.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kAvlTmpl - Templated AVL Trees, Remove Best Fitting Node.
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
 * Finds the best fitting node in the tree for the given Key value and removes the node.
 *
 * @returns Pointer to the removed node.
 * @param   ppTree      Pointer to Pointer to the tree root node.
 * @param   Key         The Key of which is to be found a best fitting match for..
 * @param   fAbove      K_TRUE:  Returned node is have the closest key to Key from above.
 *                      K_FALSE: Returned node is have the closest key to Key from below.
 *
 * @remark  This implementation uses GetBestFit and then Remove and might therefore
 *          not be the most optimal kind of implementation, but it reduces the complexity
 *          code size, and the likelyhood for bugs.
 */
KAVL_DECL(KAVLNODE *) KAVL_FN(RemoveBestFit)(KAVLTREEPTR *ppTree, KAVLKEY Key, KBOOL fAbove)
{
    /*
     * If we find anything we'll have to remove the node and return it.
     * Now, if duplicate keys are allowed we'll remove a duplicate before
     * removing the in-tree node as this is way cheaper.
     */
    KAVLNODE *pNode = KAVL_FN(GetBestFit)(ppTree, Key, fAbove);
    if (pNode != NULL)
    {
#ifdef KAVL_EQUAL_ALLOWED
        if (pNode->mpList != KAVL_NULL)
        {
            KAVLNODE *pRet = KAVL_GET_POINTER(&pNode->mpList);
            KAVL_SET_POINTER_NULL(&pNode->mpList, &pRet->mpList);
            return pRet;
        }
#endif
        pNode = KAVL_FN(Remove)(ppTree, pNode->mKey);
    }
    return pNode;
}

