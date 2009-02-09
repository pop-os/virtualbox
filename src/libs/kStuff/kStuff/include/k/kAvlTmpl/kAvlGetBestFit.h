/* $Id: kAvlGetBestFit.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kAvlTmpl - Templated AVL Trees, Get Best Fitting Node.
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
 * Finds the best fitting node in the tree for the given Key value.
 *
 * @returns Pointer to the best fitting node found.
 * @param   ppTree      Pointer to Pointer to the tree root node.
 * @param   Key         The Key of which is to be found a best fitting match for..
 * @param   fAbove      K_TRUE:  Returned node is have the closest key to Key from above.
 *                      K_FALSE: Returned node is have the closest key to Key from below.
 * @sketch  The best fitting node is always located in the searchpath above you.
 *          >= (above): The node where you last turned left.
 *          <= (below): the node where you last turned right.
 */
KAVL_DECL(KAVLNODE *) KAVL_FN(GetBestFit)(KAVLTREEPTR *ppTree, KAVLKEY Key, KBOOL fAbove)
{
    register KAVLNODE  *pNode;
    KAVLNODE           *pNodeLast;

    if (*ppTree == KAVL_NULL)
        return NULL;

    pNode = KAVL_GET_POINTER(ppTree);
    pNodeLast = NULL;
    if (fAbove)
    {   /* pNode->mKey >= Key */
        while (KAVL_NE(pNode->mKey, Key))
        {
            if (KAVL_G(pNode->mKey, Key))
            {
                if (pNode->mpLeft == KAVL_NULL)
                    return pNode;
                pNodeLast = pNode;
                pNode = KAVL_GET_POINTER(&pNode->mpLeft);
            }
            else
            {
                if (pNode->mpRight == KAVL_NULL)
                    return pNodeLast;
                pNode = KAVL_GET_POINTER(&pNode->mpRight);
            }
        }
    }
    else
    {   /* pNode->mKey <= Key */
        while (KAVL_NE(pNode->mKey, Key))
        {
            if (KAVL_G(pNode->mKey, Key))
            {
                if (pNode->mpLeft == KAVL_NULL)
                    return pNodeLast;
                pNode = KAVL_GET_POINTER(&pNode->mpLeft);
            }
            else
            {
                if (pNode->mpRight == KAVL_NULL)
                    return pNode;
                pNodeLast = pNode;
                pNode = KAVL_GET_POINTER(&pNode->mpRight);
            }
        }
    }

    /* perfect match or nothing. */
    return pNode;
}

