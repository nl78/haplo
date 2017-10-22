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

#ifndef __HAPLO_BUILTIN_H__
#define __HAPLO_BUILTIN_H__

#include <haplo/builtin.h>

#include "object.h"


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define LIST_HOMOGENEOUS	1

#define OBJECT_FLOAT		__haplo_object_type_float
#define OBJECT_STRING		__haplo_object_type_string
#define OBJECT_LIST		__haplo_object_type_list
#define OBJECT_BOOLEAN		__haplo_object_type_boolean
#define OBJECT_CODE		__haplo_object_type_code
#define OBJECT_HASH		__haplo_object_type_hash


#define HAPLO_MATH1_PROTO(x)					\
	static haplo_float_t * math_ ## x (const haplo_float_t *arg)
	
#define HAPLO_MATH2_PROTO(x)					\
	static haplo_float_t * math_ ## x (const haplo_float_t *arg1, \
		const haplo_float_t *arg2)

#define HAPLO_MATH1_FUNC(x)						\
	HAPLO_MATH1_PROTO(x)						\
	{ 								\
		haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);\
		*result=x (*arg);					\
		return(result);						\
	}
	
#define HAPLO_MATH2_FUNC(x)						\
	HAPLO_MATH2_PROTO(x)						\
	{ 								\
		haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);\
		*result=x (*arg1, *arg2);				\
		return(result);						\
	}


#define HAPLO_MATH1_REGISTER(x) 					\
	{ #x, HAPLO_FUNC(math_ ## x), "float:float"}
#define HAPLO_MATH2_REGISTER(x) 					\
	{ #x, HAPLO_FUNC(math_ ## x), "float:float:float" }


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

extern const object_type_t	*__haplo_object_type_float;
extern const object_type_t	*__haplo_object_type_string;
extern const object_type_t	*__haplo_object_type_list;
extern const object_type_t	*__haplo_object_type_boolean;
extern const object_type_t	*__haplo_object_type_code;
extern const object_type_t	*__haplo_object_type_hash;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void __haplo_builtin_init(void);
extern void __haplo_builtin_fini(void);
extern void __haplo_info_display(void);

#endif /* __HAPLO_BUILTIN_H__ */

