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
 
#ifndef __HAPLO_SLINK_H__
#define __HAPLO_SLINK_H__

/*-----------------------------------------------------------------------------
                                S L I N K _ T 
-----------------------------------------------------------------------------*/

typedef struct slink_s
{
	void		*data;
	struct slink_s	*next;
} slink_t;


/*-----------------------------------------------------------------------------
                           S L I N K _ F U N C _ T
-----------------------------------------------------------------------------*/

typedef void (*slink_func_t)(void *);


/*-----------------------------------------------------------------------------
                                   M A C R O S
-----------------------------------------------------------------------------*/

#define SLINK_FUNC(f)		(slink_func_t)(f)

/*-----------------------------------------------------------------------------
                              P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern slink_t *__haplo_slink_new(void *data);
extern slink_t *__haplo_slink_append(slink_t *prev, void *data);
extern slink_t *__haplo_slink_prepend(slink_t *next, void *data);
extern void __haplo_slink_free(slink_t *first);
extern void __haplo_slink_free_f(slink_t *first, slink_func_t f);
extern void __haplo_slink_loop(slink_t *first, slink_func_t f);
extern unsigned int __haplo_slink_length(slink_t *first);
extern void __haplo_slink_init(void);
extern void __haplo_slink_fini(void);



#endif /* __HAPLO_SIMPLE_H__ */
