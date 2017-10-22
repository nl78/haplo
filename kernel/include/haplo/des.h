/*
 * Copyright (C) 2007 Nicolas LAURENT
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

#ifndef __HAPLO_HAPLO_DES_H__
#define __HAPLO_HAPLO_DES_H__

#include <haplo/defs.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void haplo_des_encipher(const unsigned char *clear, 
			       unsigned char *encrypted,
			       const unsigned char *key,
			       int size);

extern void haplo_des_decipher(const unsigned char *encrypted, 
			       unsigned char *clear,
			       const unsigned char *key,
			       int size);

__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_DES_H__ */
