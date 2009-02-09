/* $Id: kHlpEnv.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpEnv - Environment Manipulation.
 */

/*
 * Copyright (c) 2006-2007 knut st. osmundsen <bird-src-spam@anduin.net>
 *
 * This file is part of kStuff.
 *
 * kStuff is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kStuff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with kStuff; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef ___k_kHlpEnv_h___
#define ___k_kHlpEnv_h___

#include <k/kHlpDefs.h>
#include <k/kTypes.h>

/** @defgroup grp_kHlpEnv kHlpEnv - Environment Manipulation
 * @addtogroup grp_kHlp
 * @{*/

#ifdef __cplusplus
extern "C" {
#endif

KHLP_DECL(int)  kHlpGetEnv(const char *pszVar, char *pszVal, KSIZE cchVal);
KHLP_DECL(int)  kHlpGetEnvUZ(const char *pszVar, KSIZE *pcb);

#ifdef __cplusplus
}
#endif

/** @} */

#endif

