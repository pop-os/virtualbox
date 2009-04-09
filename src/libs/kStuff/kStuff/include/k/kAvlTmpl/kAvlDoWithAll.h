/* $Id: kAvlDoWithAll.h 7 2008-02-04 02:08:02Z bird $ */
/** @file
 * kAvlTmpl - Templated AVL Trees, The Callback Iterator.
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

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/**
 * Stack used by DoWithAll to avoid recusive function calls.
 */
typedef struct
{
    unsigned        cEntries;
    KAVLNODE       *aEntries[KAVL_MAX_STACK];
    char            achFlags[KAVL_MAX_STACK];
    KAVLROOT        pRoot;
} KAVL_INT(STACK2);


/**
 * Iterates thru all nodes in the given tree.
 *
 * @returns   0 on success. Return from callback on failure.
 * @param     pRoot        Pointer to the AVL-tree root structure.
 * @param     fFromLeft    K_TRUE:  Left to right.
 *                         K_FALSE: Right to left.
 * @param     pfnCallBack  Pointer to callback function.
 * @param     pvUser       User parameter passed on to the callback function.
 */
KAVL_DECL(int) KAVL_FN(DoWithAll)(KAVLROOT *pRoot, KBOOL fFromLeft, KAVL_TYPE(PFN,CALLBACK) pfnCallBack, void *pvUser)
{
    KAVL_INT(STACK2)    AVLStack;
    KAVLNODE           *pNode;
#ifdef KAVL_EQUAL_ALLOWED
    KAVLNODE           *pEqual;
#endif
    int                 rc;

    KAVL_READ_LOCK(pRoot);
    if (pRoot->mpRoot == KAVL_NULL)
    {
        KAVL_READ_UNLOCK(pRoot);
        return 0;
    }

    AVLStack.cEntries = 1;
    AVLStack.achFlags[0] = 0;
    AVLStack.aEntries[0] = KAVL_GET_POINTER(&pRoot->mpRoot);

    if (fFromLeft)
    {   /* from left */
        while (AVLStack.cEntries > 0)
        {
            pNode = AVLStack.aEntries[AVLStack.cEntries - 1];

            /* left */
            if (!AVLStack.achFlags[AVLStack.cEntries - 1]++)
            {
                if (pNode->mpLeft != KAVL_NULL)
                {
                    AVLStack.achFlags[AVLStack.cEntries] = 0; /* 0 first, 1 last */
                    AVLStack.aEntries[AVLStack.cEntries++] = KAVL_GET_POINTER(&pNode->mpLeft);
                    continue;
                }
            }

            /* center */
            rc = pfnCallBack(pNode, pvUser);
            if (rc)
                return rc;
#ifdef KAVL_EQUAL_ALLOWED
            if (pNode->mpList != KAVL_NULL)
                for (pEqual = KAVL_GET_POINTER(&pNode->mpList); pEqual; pEqual = KAVL_GET_POINTER_NULL(&pEqual->mpList))
                {
                    rc = pfnCallBack(pEqual, pvUser);
                    if (rc)
                    {
                        KAVL_READ_UNLOCK(pRoot);
                        return rc;
                    }
                }
#endif

            /* right */
            AVLStack.cEntries--;
            if (pNode->mpRight != KAVL_NULL)
            {
                AVLStack.achFlags[AVLStack.cEntries] = 0;
                AVLStack.aEntries[AVLStack.cEntries++] = KAVL_GET_POINTER(&pNode->mpRight);
            }
        } /* while */
    }
    else
    {   /* from right */
        while (AVLStack.cEntries > 0)
        {
            pNode = AVLStack.aEntries[AVLStack.cEntries - 1];

            /* right */
            if (!AVLStack.achFlags[AVLStack.cEntries - 1]++)
            {
                if (pNode->mpRight != KAVL_NULL)
                {
                    AVLStack.achFlags[AVLStack.cEntries] = 0;  /* 0 first, 1 last */
                    AVLStack.aEntries[AVLStack.cEntries++] = KAVL_GET_POINTER(&pNode->mpRight);
                    continue;
                }
            }

            /* center */
            rc = pfnCallBack(pNode, pvUser);
            if (rc)
                return rc;
#ifdef KAVL_EQUAL_ALLOWED
            if (pNode->mpList != KAVL_NULL)
                for (pEqual = KAVL_GET_POINTER(&pNode->mpList); pEqual; pEqual = KAVL_GET_POINTER_NULL(&pEqual->pList))
                {
                    rc = pfnCallBack(pEqual, pvUser);
                    if (rc)
                    {
                        KAVL_READ_UNLOCK(pRoot);
                        return rc;
                    }
                }
#endif

            /* left */
            AVLStack.cEntries--;
            if (pNode->mpLeft != KAVL_NULL)
            {
                AVLStack.achFlags[AVLStack.cEntries] = 0;
                AVLStack.aEntries[AVLStack.cEntries++] = KAVL_GET_POINTER(&pNode->mpLeft);
            }
        } /* while */
    }

    KAVL_READ_UNLOCK(pRoot);
    return 0;
}

