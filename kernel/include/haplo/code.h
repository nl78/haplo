/*
 * Copyright (C) 1998-2004 Nicolas LAURENT
 * This file is part of `Haplo'
 * 
 *
 * `Haplo'  is free software;  you can  redistribute  it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation;  either version 2  of the License, or
 * (at your option) any later version.
 *
 * `Haplo' is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
 * for more details.
 * 
 * You should have  received  a copy of the   GNU General Public  License
 * along with `Haplo'.  If not, write to  the
 *
 *                                        Free Software Foundation,  Inc.
 *                                        675 Mass Ave, Cambridge, MA
 *                                        02139, USA.
 *
 */

#ifndef __HAPLO_HAPLO_CODE_H__
#define __HAPLO_HAPLO_CODE_H__

#include <haplo/defs.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                            H A P L O _ C O D E _ T
-----------------------------------------------------------------------------*/

typedef void haplo_code_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void *haplo_code_eval(const haplo_code_t *code,
			     const char *signature, ...);

extern void haplo_code_print(const haplo_code_t *code);

__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_CODE_H__ */
