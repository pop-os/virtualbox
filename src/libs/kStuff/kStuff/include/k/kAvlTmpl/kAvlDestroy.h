/* $Id: kAvlDestroy.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kAvlTmpl - Templated AVL Trees, Destroy the tree.
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
 * Destroys the specified tree, starting with the root node and working our way down.
 *
 * @returns 0 on success.
 * @returns Return value from callback on failure. On failure, the tree will be in
 *          an unbalanced condition and only further calls to the Destroy should be
 *          made on it. Note that the node we fail on will be considered dead and
 *          no action is taken to link it back into the tree.
 * @param   ppTree          Pointer to the AVL-tree root node pointer.
 * @param   pfnCallBack     Pointer to callback function.
 * @param   pvUser          User parameter passed on to the callback function.
 */
KAVL_DECL(int) KAVL_FN(Destroy)(KAVLTREEPTR *ppTree, KAVL_TYPE(PFN,CALLBACK) pfnCallBack, void *pvUser)
{
    unsigned    cEntries;
    KAVLNODE   *apEntries[KAVL_MAX_STACK];
    int         rc;

    if (*ppTree == KAVL_NULL)
        return 0;

    cEntries = 1;
    apEntries[0] = KAVL_GET_POINTER(ppTree);
    while (cEntries > 0)
    {
        /*
         * Process the subtrees first.
         */
        KAVLNODE *pNode = apEntries[cEntries - 1];
        if (pNode->mpLeft != KAVL_NULL)
            apEntries[cEntries++] = KAVL_GET_POINTER(&pNode->mpLeft);
        else if (pNode->mpRight != KAVL_NULL)
            apEntries[cEntries++] = KAVL_GET_POINTER(&pNode->mpRight);
        else
        {
#ifdef KAVL_EQUAL_ALLOWED
            /*
             * Process nodes with the same key.
             */
            while (pNode->pList != KAVL_NULL)
            {
                KAVLNODE *pEqual = KAVL_GET_POINTER(&pNode->pList);
                KAVL_SET_POINTER(&pNode->pList, KAVL_GET_POINTER_NULL(&pEqual->pList));
                pEqual->pList = KAVL_NULL;

                rc = pfnCallBack(pEqual, pvUser);
                if (rc)
                    return rc;
            }
#endif

            /*
             * Unlink the node.
             */
            if (--cEntries > 0)
            {
                KAVLNODE *pParent = apEntries[cEntries - 1];
                if (KAVL_GET_POINTER(&pParent->mpLeft) == pNode)
                    pParent->mpLeft = KAVL_NULL;
                else
                    pParent->mpRight = KAVL_NULL;
            }
            else
                *ppTree = KAVL_NULL;

            kHlpAssert(pNode->mpLeft == KAVL_NULL);
            kHlpAssert(pNode->mpRight == KAVL_NULL);
            rc = pfnCallBack(pNode, pvUser);
            if (rc)
                return rc;
        }
    } /* while */
    kHlpAssert(*ppTree == KAVL_NULL);

    return 0;
}

