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

#ifndef __HAPLO_IDENTITY_H__
#define __HAPLO_IDENTITY_H__


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define LOGIN_SZ	16
#define PASSWD_SZ	16

#if PASSWD_SZ < 16
#	error "PASSWD_SZ should be at least 16."
#endif


/*-----------------------------------------------------------------------------
                             I D E N T I T Y _ T 
-----------------------------------------------------------------------------*/

typedef struct identity_t
{
	unsigned char	login[LOGIN_SZ];
	unsigned char	passwd[PASSWD_SZ];
} identity_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void identity_get(identity_t *id);
extern int identity_cmp(const identity_t *id1, const identity_t *id2);

#endif /* __HAPLO_PASSWORD_H__ */


