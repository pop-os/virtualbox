/* $Id: kAvlEnum.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kAvlTmpl - Templated AVL Trees, Node Enumeration.
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
 * Enumeration control data.
 *
 * This is initialized by BeginEnum and used by GetNext to figure out what
 * to do next.
 */
typedef struct KAVL_TYPE(,ENUMDATA)
{
    KBOOL               fFromLeft;
    KI8                 cEntries;
    KU8                 achFlags[KAVL_MAX_STACK];
    KAVLNODE *          aEntries[KAVL_MAX_STACK];
} KAVL_TYPE(,ENUMDATA), *KAVL_TYPE(P,ENUMDATA);


/**
 * Get the next node in the tree enumeration.
 *
 * The current implementation of this function willl not walk the mpList
 * chain like the DoWithAll function does. This may be changed later.
 *
 * @returns Pointer to the first node in the tree.
 * @param   pEnumData   Pointer to enumeration control data.
 */
KAVL_DECL(KAVLNODE *) KAVL_FN(GetNext)(KAVL_TYPE(,ENUMDATA) *pEnumData)
{
    if (pEnumData->fFromLeft)
    {   /* from left */
        while (pEnumData->cEntries > 0)
        {
            KAVLNODE *pNode = pEnumData->aEntries[pEnumData->cEntries - 1];

            /* left */
            if (pEnumData->achFlags[pEnumData->cEntries - 1] == 0)
            {
                pEnumData->achFlags[pEnumData->cEntries - 1]++;
                if (pNode->mpLeft != KAVL_NULL)
                {
                    pEnumData->achFlags[pEnumData->cEntries] = 0; /* 0 left, 1 center, 2 right */
                    pEnumData->aEntries[pEnumData->cEntries++] = KAVL_GET_POINTER(&pNode->mpLeft);
                    continue;
                }
            }

            /* center */
            if (pEnumData->achFlags[pEnumData->cEntries - 1] == 1)
            {
                pEnumData->achFlags[pEnumData->cEntries - 1]++;
                return pNode;
            }

            /* right */
            pEnumData->cEntries--;
            if (pNode->mpRight != KAVL_NULL)
            {
                pEnumData->achFlags[pEnumData->cEntries] = 0;
                pEnumData->aEntries[pEnumData->cEntries++] = KAVL_GET_POINTER(&pNode->mpRight);
            }
        } /* while */
    }
    else
    {   /* from right */
        while (pEnumData->cEntries > 0)
        {
            KAVLNODE *pNode = pEnumData->aEntries[pEnumData->cEntries - 1];

            /* right */
            if (pEnumData->achFlags[pEnumData->cEntries - 1] == 0)
            {
                pEnumData->achFlags[pEnumData->cEntries - 1]++;
                if (pNode->mpRight != KAVL_NULL)
                {
                    pEnumData->achFlags[pEnumData->cEntries] = 0;  /* 0 right, 1 center, 2 left */
                    pEnumData->aEntries[pEnumData->cEntries++] = KAVL_GET_POINTER(&pNode->mpRight);
                    continue;
                }
            }

            /* center */
            if (pEnumData->achFlags[pEnumData->cEntries - 1] == 1)
            {
                pEnumData->achFlags[pEnumData->cEntries - 1]++;
                return pNode;
            }

            /* left */
            pEnumData->cEntries--;
            if (pNode->mpLeft != KAVL_NULL)
            {
                pEnumData->achFlags[pEnumData->cEntries] = 0;
                pEnumData->aEntries[pEnumData->cEntries++] = KAVL_GET_POINTER(&pNode->mpLeft);
            }
        } /* while */
    }

    return NULL;
}


/**
 * Starts an enumeration of all nodes in the given AVL tree.
 *
 * The current implementation of this function willl not walk the mpList
 * chain like the DoWithAll function does. This may be changed later.
 *
 * @returns Pointer to the first node in the enumeration.
 * @param   ppTree      Pointer to the AVL-tree root node pointer.
 * @param   pEnumData   Pointer to enumeration control data.
 * @param   fFromLeft   K_TRUE:  Left to right.
 *                      K_FALSE: Right to left.
 */
KAVL_DECL(KAVLNODE *) KAVL_FN(BeginEnum)(KAVLTREEPTR *ppTree, KAVL_TYPE(,ENUMDATA) *pEnumData, KBOOL fFromLeft)
{
    if (*ppTree != KAVL_NULL)
    {
        pEnumData->fFromLeft = fFromLeft;
        pEnumData->cEntries = 1;
        pEnumData->aEntries[0] = KAVL_GET_POINTER(ppTree);
        pEnumData->achFlags[0] = 0;
    }
    else
        pEnumData->cEntries = 0;

    return KAVL_FN(GetNext)(pEnumData);
}

