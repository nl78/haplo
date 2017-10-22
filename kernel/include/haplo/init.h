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

#ifndef __HAPLO_HAPLO_INIT_H__
#define __HAPLO_HAPLO_INIT_H__

#include <haplo/defs.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                          H A P L O _ P A R A M _ T 
-----------------------------------------------------------------------------*/

typedef struct haplo_param_t
{
	int		colors;
	int 		autoload;
	int 		banner;
	const char	*init_filename;
	const char	*filename;
	void		*parser_param;
	char *		(*user_getline)(const char *);
	void		(*user_freeline)(char *);
	char *		(*file_getline)(const char *);
	void		(*file_freeline)(char *);
} haplo_param_t;	


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define HAPLO_HOME_VAR		"HAPLO_HOME"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern char *haplo_param_path(const char *dir);
extern void haplo_init(haplo_param_t *param);
extern void haplo_fini(void);
extern void haplo_param_default(haplo_param_t *param);


__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_INIT_H__ */
