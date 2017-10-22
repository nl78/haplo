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

#ifndef __HAPLO_FEM_CARA_H__
#define __HAPLO_FEM_CARA_H__

#include <fem/module.h>

__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                          F E M _ C A R A _ M A S K _ T 
-----------------------------------------------------------------------------*/

typedef unsigned long int fem_cara_mask_t;


/*-----------------------------------------------------------------------------
                           F E M _ C A R A _ L I S T _ T 
-----------------------------------------------------------------------------*/

typedef struct fem_cara_list_s
{
	unsigned short		nb;		/* number of carateristics */
	double			*values;	/* values of caracteristics */
	fem_cara_mask_t		mask;		/* fixed caracteritics */
	const char *const	*names;		/* names of caracteritics */
} fem_cara_list_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define FEM_CARA_GET(c, x)	c->values[x];


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern fem_cara_list_t fem_cara_list_new(const char *const *cara);
extern void fem_cara_list_free(fem_cara_list_t cara_list);
extern fem_cara_list_t fem_cara_list_copy(const fem_cara_list_t *cara_list);
extern void fem_cara_list_display(const fem_cara_list_t *cara_list);
extern int fem_cara_list_set(fem_cara_list_t *cara_list, const char *c,
			       double value);
int fem_cara_list_unset(fem_cara_list_t *cara_list, const char *c);
extern int fem_cara_list_get(const fem_cara_list_t *cara_list, const char *c,
			       double *value);
extern int fem_cara_cmp(const fem_cara_list_t *cara, fem_cara_mask_t data);
extern fem_cara_mask_t fem_cara_mask(const char *const *names, const char *s);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_CARA_H__ */
