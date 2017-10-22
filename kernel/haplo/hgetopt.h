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

#ifndef __HAPLO_GETOPT_H__
#define __HAPLO_GETOPT_H__

#include <haplo.h>


/*-----------------------------------------------------------------------------
                            O P T I O N _ T Y P E
-----------------------------------------------------------------------------*/

enum option_type
{
	NONE,
	OPTARG,
	PARAM,
	PARAMONLY,
	OPTIONS,
	ARGS
};


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define OPT_F(x)		{ (int (*)(void))(x) }


/*-----------------------------------------------------------------------------
                               A C T I O N _ T
-----------------------------------------------------------------------------*/

struct option_s;

typedef union
{
	int (*none)(void);
	int (*options)(const struct option_s *);
	int (*optarg)(const char *);
	int (*param)(haplo_param_t *, const char *);
	int (*paramonly)(haplo_param_t *);
	int (*args)(int, char  *const []);
} action_t;


/*-----------------------------------------------------------------------------
                               O P T I O N _ T
-----------------------------------------------------------------------------*/

typedef struct option_s
{
	const char		*name;
	const char		*description;
	enum option_type	type;
	action_t		action;
} option_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern int hgetopt(haplo_param_t *param, const option_t *options,
		   int argc, char * const argv[], int pass);
extern int usage(const option_t *options);

#endif /* __HAPLO_GETOPT_H__ */
