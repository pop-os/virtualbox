/* $Id: kHlpDefs.h 2 2007-11-16 16:07:14Z bird $ */
/** @file
 * kHlpDefs - Helper Definitions.
 */

/*
 * Copyright (c) 2007 knut st. osmundsen <bird-src-spam@anduin.net>
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

#ifndef ___k_kHlpDefs_h___
#define ___k_kHlpDefs_h___

#include <k/kDefs.h>

/** @defgroup grp_kHlpDefs - Definitions
 * @addtogroup grp_kHlp
 * @{ */

/** @def KHLP_DECL
 * Declares a kHlp function according to build context.
 * @param type          The return type.
 */
#if defined(KHLP_BUILDING_DYNAMIC)
# define KHLP_DECL(type)    K_DECL_EXPORT(type)
#elif defined(KHLP_BUILT_DYNAMIC)
# define KHLP_DECL(type)    K_DECL_IMPORT(type)
#else
# define KHLP_DECL(type)    type
#endif

/** @} */

#endif

