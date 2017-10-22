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

/*
 * Here are defined all registred functions and types.
 */


#include "extensions.h"
/*
 * We should disable strict ansi, otherwise some (math-)prototypes won't be
 * defined. We should do this before first #include. (BUG of GNU Libc ?)
 */
#ifdef __STRICT_ANSI__
#	undef __STRICT_ANSI__
#endif

#include <haplo/config.h>

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif
#ifndef PATH_MAX
#	define PATH_MAX	1024
#endif /* PATH_MAX */


#include "builtin.h"
#include "code.h"
#include "error.h"
#include "func.h"
#include "hash.h"
#include "help.h"
#include "list.h"
#include "memory.h"
#include "olist.h"
#include "parser.h"
#include "timer.h"
#include "utils.h"

#define BUILTIN_CACHE_SIZE	32


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

const object_type_t		*__haplo_object_type_float;
const object_type_t		*__haplo_object_type_string;
const object_type_t		*__haplo_object_type_list;
const object_type_t		*__haplo_object_type_boolean;
const object_type_t		*__haplo_object_type_code;
const object_type_t		*__haplo_object_type_hash;

static haplo_mcache_t	*builtin_float_cache   = NULL;
static haplo_mcache_t	*builtin_boolean_cache = NULL;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

haplo_boolean_t *haplo_boolean(int value);
static haplo_boolean_t * boolean_and(const haplo_boolean_t *a,
				     const haplo_boolean_t *b);
static haplo_boolean_t * boolean_or(const haplo_boolean_t *a,
				    const haplo_boolean_t *b);
static haplo_boolean_t * boolean_xor(const haplo_boolean_t *a,
				     const haplo_boolean_t *b);
static haplo_boolean_t * boolean_not(const haplo_boolean_t *a);
static haplo_boolean_t * boolean_copy(const haplo_boolean_t *a);
static void boolean_display(const haplo_boolean_t *a);
static void boolean_free(haplo_boolean_t *a);

haplo_float_t *haplo_float(double value);
static haplo_float_t * float_add(const haplo_float_t *a,
				 const haplo_float_t *b);
static haplo_float_t * float_sub(const haplo_float_t *a,
				 const haplo_float_t *b);
static haplo_float_t * float_mul(const haplo_float_t *a,
				 const haplo_float_t *b);
static haplo_float_t * float_div(const haplo_float_t *a,
				 const haplo_float_t *b);
static haplo_float_t * float_pow(const haplo_float_t *a,
				 const haplo_float_t *b);
static haplo_float_t * float_unary_minus(const haplo_float_t *a);
static void float_print(const haplo_float_t *a);
static haplo_boolean_t * float_equal(const haplo_float_t *a,
				     const haplo_float_t *b);
static haplo_boolean_t * float_greater(const haplo_float_t *a,
				       const haplo_float_t *b);
static haplo_boolean_t * float_greater_e(const haplo_float_t *a,
					 const haplo_float_t *b);
static haplo_boolean_t * float_lower(const haplo_float_t *a,
				     const haplo_float_t *b);
static haplo_boolean_t * float_lower_e(const haplo_float_t *a,
				       const haplo_float_t *b);
static void float_print(const haplo_float_t *a);
static haplo_float_t *float_copy(const haplo_float_t *a);
static void float_display(const haplo_float_t *a);
static void float_free(haplo_float_t *a);

static haplo_string_t *string_add_string(const haplo_string_t *s1,
					 const haplo_string_t *s2);
static haplo_string_t *string_add_float(const haplo_string_t *s,
					const haplo_float_t *f);
static haplo_string_t *float_add_string(const haplo_float_t *f,
					const haplo_string_t *s);
static void string_print(const haplo_string_t *s);
static void string_display(const haplo_string_t *s);

static haplo_string_t *file_pwd(void);


static void builtin_abort(void);
static void builtin_bordered(const haplo_string_t *s);
static void builtin_centered(const haplo_string_t *s);
static void builtin_underlined(const haplo_string_t *s);

HAPLO_MATH1_PROTO(acos);
HAPLO_MATH1_PROTO(asin);
HAPLO_MATH1_PROTO(atan);
HAPLO_MATH1_PROTO(cos);
HAPLO_MATH1_PROTO(cosh);
HAPLO_MATH1_PROTO(exp);
HAPLO_MATH1_PROTO(log);
HAPLO_MATH1_PROTO(log10);
HAPLO_MATH1_PROTO(sin);
HAPLO_MATH1_PROTO(sinh);
HAPLO_MATH1_PROTO(sqrt);
HAPLO_MATH1_PROTO(tan);
HAPLO_MATH1_PROTO(tanh);
HAPLO_MATH1_PROTO(fabs);

HAPLO_MATH2_PROTO(atan2);
HAPLO_MATH2_PROTO(pow);

#ifdef HAVE_ACOSH
HAPLO_MATH1_PROTO(acosh);
#endif
#ifdef  HAVE_ASINH
HAPLO_MATH1_PROTO(asinh);
#endif
#ifdef HAVE_ATANH
HAPLO_MATH1_PROTO(atanh);
#endif
#ifdef HAVE_CBRT
HAPLO_MATH1_PROTO(cbrt);
#endif
#ifdef HAVE_ERF
HAPLO_MATH1_PROTO(erf);
#endif
#ifdef HAVE_ERFC
HAPLO_MATH1_PROTO(erfc);
#endif
#ifdef HAVE_EXPM1
HAPLO_MATH1_PROTO(expm1);
#endif
#ifdef HAVE_J0
HAPLO_MATH1_PROTO(j0);
#endif
#ifdef HAVE_J1
HAPLO_MATH1_PROTO(j1);
#endif
#ifdef HAVE_LGAMMA
HAPLO_MATH1_PROTO(lgamma);
#endif
#ifdef HAVE_LOG1P
HAPLO_MATH1_PROTO(log1p);
#endif
#ifdef HAVE_LOGB
HAPLO_MATH1_PROTO(logb);
#endif
#ifdef HAVE_Y0
HAPLO_MATH1_PROTO(y0);
#endif
#ifdef HAVE_Y1
HAPLO_MATH1_PROTO(y1);
#endif
#ifdef HAVE_HYPOT
HAPLO_MATH2_PROTO(hypot);
#endif


void __haplo_builtin_init(void);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 *
 */
haplo_boolean_t * haplo_boolean(int value)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result = value;

	return(result);
}

	
/**
 * booleand op
 *
 * @param a is a boolean
 * @param b is a boolean
 * @return a && b
 */
static haplo_boolean_t * boolean_and(const haplo_boolean_t *a,
				     const haplo_boolean_t *b)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result = (*a) & (*b);

	return(result);
}


/**
 * booleand op
 *
 * @param a is a boolean
 * @param b is a boolean
 * @return a || b
 */
static haplo_boolean_t * boolean_or(const haplo_boolean_t *a,
				    const haplo_boolean_t *b)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result = (*a) | (*b);

	return(result);
}


/**
 * booleand op
 *
 * @param a is a boolean
 * @param b is a boolean
 * @return a ^ b
 */
static haplo_boolean_t * boolean_xor(const haplo_boolean_t *a,
				     const haplo_boolean_t *b)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result = (*a) ^ (*b);

	return(result);
}


/**
 * booleand op
 *
 * @param a is a boolean
 * @return ! a
 */
static haplo_boolean_t * boolean_not(const haplo_boolean_t *a)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result = (*a)?0:1;

	return(result);
}


/**
 * Copy constructor
 *
 * @param a is a boolean
 * @return a copy of a
 */
static haplo_boolean_t * boolean_copy(const haplo_boolean_t *a)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result=*a;

	return(result);
}


/**
 * Display function 
 *
 * @param a is a boolean
 */
static void boolean_display(const haplo_boolean_t *a)
{
	if (*a)
		fputs("TRUE", stdout);
	else
		fputs("FALSE", stdout);

	return;
}


/**
 *
 */
static void boolean_free(haplo_boolean_t *a)
{
	HAPLO_MCACHE_RELEASE(builtin_boolean_cache, a);

	return;
}


/**
 *
 */
haplo_float_t *haplo_float(double value)
{
	haplo_float_t *f=HAPLO_MCACHE_GET(builtin_float_cache);

	*f = value;

	return(f);
	
}


/**
 * float op
 *
 * @param a is a float
 * @param b is a float
 * @return a + b
 */
static haplo_float_t * float_add(const haplo_float_t *a,
				 const haplo_float_t *b)
{
	haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);

	*result = *a + *b;

	return(result);
}


/**
 * float op
 *
 */
static void float_incr(haplo_float_t *a)
{
	*a += 1.0;

	return;
}


/**
 * float op
 *
 */
static void float_decr(haplo_float_t *a)
{
	*a -= 1.0;
	
	return;
}


/**
 * float op
 *
 * @param a is a float
 * @param b is a float
 * @return a - b
 */
static haplo_float_t * float_sub(const haplo_float_t *a,
				 const haplo_float_t *b)
{
	haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);

	*result = *a - *b;

	return(result);
}


/**
 * float op
 *
 * @param a is a float
 * @param b is a float
 * @return a * b
 */
static haplo_float_t * float_mul(const haplo_float_t *a,
				 const haplo_float_t *b)
{
	haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);

	*result = (*a) * (*b);

	return(result);
}


/**
 * float op
 *
 * @param a is a float
 * @param b is a float
 * @return a / b
 */
static haplo_float_t * float_div(const haplo_float_t *a,
				 const haplo_float_t *b)
{
	haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);

	*result = (*a) / (*b);

	return(result);
}


/**
 * float op
 *
 * @param a is a float
 * @param b is a float
 * @return a ^ b
 */
static haplo_float_t * float_pow(const haplo_float_t *a,
				 const haplo_float_t *b)
{
	haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);

	*result = pow(*a, *b);

	return(result);
}


/**
 * float op
 *
 * @param a is a float
 * @return -a
 */
static haplo_float_t * float_unary_minus(const haplo_float_t *a)
{
	haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);

	*result = - (*a);

	return(result);
}


/**
 * float comparison
 *
 * @param a is a float
 * @param b is a float
 * @return a == b
 */
static haplo_boolean_t * float_equal(const haplo_float_t *a,
				     const haplo_float_t *b)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result=(fabs(*a - *b) < DBL_EPSILON)?1:0;

	return(result);
}


/**
 * float comparison
 *
 * @param a is a float
 * @param b is a float
 * @return a > b
 */
static haplo_boolean_t * float_greater(const haplo_float_t *a,
				       const haplo_float_t *b)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result =(*a > *b)?1:0;

	return(result);
}


/**
 * float comparison
 *
 * @param a is a float
 * @param b is a float
 * @return a >= b
 */
static haplo_boolean_t * float_greater_e(const haplo_float_t *a,
					 const haplo_float_t *b)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result = (*a >= *b)?1:0;

	return(result);
}


/**
 * float comparison
 *
 * @param a is a float
 * @param b is a float
 * @return a < b
 */
static haplo_boolean_t * float_lower(const haplo_float_t *a,
				     const haplo_float_t *b)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result = (*a < *b)?1:0;

	return(result);
}


/**
 * float comparison
 *
 * @param a is a float
 * @param b is a float
 * @return a <= b
 */
static haplo_boolean_t * float_lower_e(const haplo_float_t *a,
				       const haplo_float_t *b)
{
	haplo_boolean_t *result=HAPLO_MCACHE_GET(builtin_boolean_cache);

	*result = (*a <= *b)?1:0;

	return(result);
}


/**
 * Print function
 *
 * @param a is a float
 */
static void float_print(const haplo_float_t *a)
{
	printf("%.8g\n", *a);
	return;
}


/**
 * copy constructor
 *
 * @param a is a float
 * @return a copy of a
 */
static haplo_float_t *float_copy(const haplo_float_t *a)
{
	haplo_float_t *result=HAPLO_MCACHE_GET(builtin_float_cache);

	*result = *a;
	
	return(result);
}


/**
 *
 */
static void float_free(haplo_float_t *a)
{
	HAPLO_MCACHE_RELEASE(builtin_float_cache, a);

	return;
}


/**
 * display function
 *
 * @param a is a float
 */
static void float_display(const haplo_float_t *a)
{
	printf("%.8g", *a);
	return;
}


/**
 * string op
 *
 * @param s1 is a string
 * @param s2 is a string
 * @return concatenation of s1 and s2
 */
static haplo_string_t *string_add_string(const haplo_string_t *s1,
					 const haplo_string_t *s2)
{
	haplo_string_t *result;
	
	HAPLO_ALLOC(result, strlen(s1)+strlen(s2)+1);
	strcpy(result, s1);
	strcat(result, s2);
	
	return(result);
}


/**
 * string/float op
 *
 * @param s is a string
 * @param f is a float
 * @return a concatenation of s and string converted f
 */
static haplo_string_t *string_add_float(const haplo_string_t *s,
					const haplo_float_t *f)
{
	haplo_string_t *result;
	haplo_string_t s2[20];
	
	snprintf(s2, 20, "%.8g", *f);
	HAPLO_ALLOC(result, strlen(s)+strlen(s2)+1);
	strcpy(result, s);
	strcat(result, s2);

	return(result);
}


/**
 * string/float op
 *
 * @param f is a float
 * @param s is a string
 * @return a concatenation of string converted f and s
 */
static haplo_string_t *float_add_string(const haplo_float_t *f,
					const haplo_string_t *s)
{
	haplo_string_t *result;
	haplo_string_t s2[20];
	
	snprintf(s2, 20, "%.8g", *f);
	HAPLO_ALLOC(result, strlen(s)+strlen(s2)+1);
	strcpy(result, s2);
	strcat(result, s);

	return(result);
}


/**
 * print function
 *
 * @param s is a string
 */
static void string_print(const haplo_string_t *s)
{
	puts(s);
	return;
}


/**
 * display function
 *
 * @param s is a string
 */
static void string_display(const haplo_string_t *s)
{
	putc('"', stdout);
	fputs(s, stdout);
	putc('"', stdout);
	
	return;
}


/**
 * print working directory name
 */
static haplo_string_t * file_pwd(void)
{
	char pwd[PATH_MAX];

	(void)getcwd(pwd, PATH_MAX);

	return(haplo_strdup(pwd));
}


/**
 * 
 */

static haplo_string_t * builtin_version(void)
{
	char version[32];

	snprintf(version, 32, "%d.%d", HAPLO_MAJOR, HAPLO_MINOR);

	return(haplo_strdup(version));
}

/*
 * Mathematical functions
 */
HAPLO_MATH1_FUNC(acos)
HAPLO_MATH1_FUNC(asin)
HAPLO_MATH1_FUNC(atan)
HAPLO_MATH1_FUNC(cos)
HAPLO_MATH1_FUNC(cosh)
HAPLO_MATH1_FUNC(exp)
HAPLO_MATH1_FUNC(log)
HAPLO_MATH1_FUNC(log10)
HAPLO_MATH1_FUNC(sin)
HAPLO_MATH1_FUNC(sinh)
HAPLO_MATH1_FUNC(sqrt)
HAPLO_MATH1_FUNC(tan)
HAPLO_MATH1_FUNC(tanh)
HAPLO_MATH1_FUNC(fabs)

HAPLO_MATH2_FUNC(atan2)
HAPLO_MATH2_FUNC(pow)

#ifdef HAVE_ACOSH
HAPLO_MATH1_FUNC(acosh)
#endif
#ifdef HAVE_ASINH
HAPLO_MATH1_FUNC(asinh)
#endif
#ifdef HAVE_ATANH
HAPLO_MATH1_FUNC(atanh)
#endif
#ifdef HAVE_CBRT
HAPLO_MATH1_FUNC(cbrt)
#endif
#ifdef HAVE_ERF
HAPLO_MATH1_FUNC(erf)
#endif
#ifdef HAVE_ERFC
HAPLO_MATH1_FUNC(erfc)
#endif
#ifdef HAVE_ERFC
HAPLO_MATH1_FUNC(expm1)
#endif
#ifdef HAVE_EXPM1
HAPLO_MATH1_FUNC(j0)
#endif
#ifdef HAVE_GETCWD
HAPLO_MATH1_FUNC(j1)
#endif
#ifdef HAVE_LGAMMA
HAPLO_MATH1_FUNC(lgamma)
#endif
#ifdef HAVE_LOG1P
HAPLO_MATH1_FUNC(log1p)
#endif
#ifdef HAVE_LOGB
HAPLO_MATH1_FUNC(logb)
#endif
#ifdef HAVE_Y0
HAPLO_MATH1_FUNC(y0)
#endif
#ifdef HAVE_Y1
HAPLO_MATH1_FUNC(y1)
#endif
#ifdef HAVE_HYPOT
HAPLO_MATH2_FUNC(hypot)
#endif


static haplo_type_descr_t builtin_types[]={
	{ "boolean",
	  HAPLO_TYPE_DISPLAY(boolean_display),
	  HAPLO_TYPE_FREE(boolean_free),
	  HAPLO_TYPE_COPY(boolean_copy) },
	{ "code",
	  HAPLO_TYPE_DISPLAY(__haplo_code_display),
	  HAPLO_TYPE_FREE(__haplo_code_free),
	  HAPLO_TYPE_COPY(__haplo_code_copy) },
	{ "float",
	  HAPLO_TYPE_DISPLAY(float_display),
	  HAPLO_TYPE_FREE(float_free),
	  HAPLO_TYPE_COPY(float_copy) },
	{ "hash",
	  HAPLO_TYPE_DISPLAY(__haplo_hash_display),
	  HAPLO_TYPE_FREE(__haplo_hash_free),
	  HAPLO_TYPE_COPY(__haplo_hash_copy) },
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
	{ "library",
	  NULL,NULL,
	  HAPLO_TYPE_FREE(HAPLO_FREE_FUNC),
	  NULL, NULL },
#endif /* HAPLO_MODULE_IMPL */
	{ "list",
	  HAPLO_TYPE_DISPLAY(__haplo_olist_display),
	  HAPLO_TYPE_FREE(__haplo_olist_free),
	  HAPLO_TYPE_COPY(__haplo_olist_copy) },
	{ "string",
	  HAPLO_TYPE_DISPLAY(string_display),
	  HAPLO_TYPE_FREE(HAPLO_FREE_FUNC),
	  HAPLO_TYPE_COPY(haplo_strdup) },
	{ "timer",
	  HAPLO_TYPE_DISPLAY(__haplo_timer_display),
	  HAPLO_TYPE_FREE(__haplo_timer_free),
	  HAPLO_TYPE_COPY(__haplo_timer_copy) },
	{ NULL,
	  NULL, NULL,
	  NULL, NULL,
	  NULL, NULL }
};


static haplo_func_descr_t builtin_functions[]={
	{ "add_path", HAPLO_FUNC(haplo_parser_path_add), ":string" },	
	{ "print", HAPLO_FUNC(haplo_code_print), ":code" },
	{ "print", HAPLO_FUNC(__haplo_hash_print), ":hash" },
	{ "&", HAPLO_FUNC(boolean_and),	"boolean:boolean:boolean" },
	{ "|", HAPLO_FUNC(boolean_or), "boolean:boolean:boolean" },
	{ "~", HAPLO_FUNC(boolean_xor), "boolean:boolean:boolean" },
	{ "!", HAPLO_FUNC(boolean_not),	"boolean:boolean" },
	{ "+", HAPLO_FUNC(float_add), "float:float:float" },
	{ "incr", HAPLO_FUNC(float_incr), "!float" },
	{ "decr", HAPLO_FUNC(float_decr), "!float" },
	{ "-", HAPLO_FUNC(float_sub), "float:float:float" },
	{ "*", HAPLO_FUNC(float_mul), "float:float:float" },
	{ "/", HAPLO_FUNC(float_div), "float:float:float" },
	{ "--", HAPLO_FUNC(float_unary_minus), "float:float" },
	{ "++", HAPLO_FUNC(float_copy), "float:float" },
	{ "^", HAPLO_FUNC(float_pow), "float:float:float" },
	{ "=", HAPLO_FUNC(float_equal), "boolean:float:float" },
	{ ">", HAPLO_FUNC(float_greater), "boolean:float:float" },
	{ ">=", HAPLO_FUNC(float_greater_e), "boolean:float:float" },
	{ "<", HAPLO_FUNC(float_lower), "boolean:float:float" },
	{ "<=", HAPLO_FUNC(float_lower_e), "boolean:float:float" },
	{ "print", HAPLO_FUNC(float_print), ":float" },
	{ "print", HAPLO_FUNC(string_print), ":string" },
	{ "+", HAPLO_FUNC(string_add_string), "string:string:string" },
	{ "+", HAPLO_FUNC(string_add_float), "string:string:float" },
	{ "+", HAPLO_FUNC(float_add_string), "string:float:string" },
	{ "bordered", HAPLO_FUNC(builtin_bordered), ":string" },
	{ "centered", HAPLO_FUNC(builtin_centered), ":string" },
	{ "underlined", HAPLO_FUNC(builtin_underlined), ":string" },
	{ "cd", HAPLO_FUNC(chdir), ":string" },
	{ "pwd", HAPLO_FUNC(file_pwd), "string" },
	{ "type", HAPLO_FUNC(__haplo_object_type_list_display), "" },
	{ "bind", HAPLO_FUNC(__haplo_func_list_display), "" },
	{ "bind", HAPLO_FUNC(__haplo_func_bind_display), ":string" },
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
	{ "use", HAPLO_FUNC(__haplo_module_load), ":string" },
	{ "use", HAPLO_FUNC(__haplo_module_load_options), ":string:string" },
	{ "use", HAPLO_FUNC(__haplo_module_status), "" },
	{ "isused", HAPLO_FUNC(__haplo_module_isloaded), "boolean:string"},
	{ "load_library", HAPLO_FUNC(__haplo_module_library_load),
	  "library:string" } ,
	{ "bind", HAPLO_FUNC(__haplo_func_bind), ":library:string:string" },
	{ "type", HAPLO_FUNC(__haplo_object_type_register_0), ":string" } ,
	{ "type", HAPLO_FUNC(__haplo_object_type_register_1),
	  ":string:string" } ,
	{ "type", HAPLO_FUNC(__haplo_object_type_register_2),
	  ":string:string:string" } ,
	{ "type", HAPLO_FUNC(__haplo_object_type_register_3),
	  ":string:string:string:string" } ,
	{ "add_module_path",  HAPLO_FUNC(haplo_module_path_add), ":string" } ,
#endif /* HAPLO_MODULE_IMPL */
	{ "help", HAPLO_FUNC(__haplo_help_banner), "" },
	{ "help", HAPLO_FUNC(__haplo_help), ":string" },
	{ "help", HAPLO_FUNC(__haplo_help_code), ":code" },
	{ "html", HAPLO_FUNC(__haplo_html), ":string:string" },
	{ "abort", HAPLO_FUNC(builtin_abort), "" },
	{ "length", HAPLO_FUNC(__haplo_olist_length), "float:list" },
	{ "keys", HAPLO_FUNC(__haplo_hash_keys), "list:hash" },
	{ "values", HAPLO_FUNC(__haplo_hash_values), "list:hash" },
	{ "version", HAPLO_FUNC(builtin_version), "string"},
	{ "timer", HAPLO_FUNC(__haplo_timer_new), "timer" },
	{ "start", HAPLO_FUNC(haplo_timer_start), "!timer"},
	{ "stop", HAPLO_FUNC(haplo_timer_stop),  "!timer"},
	{ "print", HAPLO_FUNC(haplo_timer_print), ":timer"},
	{ "profile", HAPLO_FUNC(__haplo_func_profile), ":string"},
	{ "unprofile", HAPLO_FUNC(__haplo_func_unprofile), ":string"},

	HAPLO_MATH1_REGISTER(acos),
	HAPLO_MATH1_REGISTER(asin),
	HAPLO_MATH1_REGISTER(atan),
	HAPLO_MATH1_REGISTER(cos),
	HAPLO_MATH1_REGISTER(cosh),
	HAPLO_MATH1_REGISTER(exp),
	HAPLO_MATH1_REGISTER(log),
	HAPLO_MATH1_REGISTER(log10),
	HAPLO_MATH1_REGISTER(sin),
	HAPLO_MATH1_REGISTER(sinh),
	HAPLO_MATH1_REGISTER(sqrt),
	HAPLO_MATH1_REGISTER(tan),
	HAPLO_MATH1_REGISTER(tanh),
	HAPLO_MATH1_REGISTER(fabs),	
	HAPLO_MATH2_REGISTER(atan2),
	HAPLO_MATH2_REGISTER(pow),
#ifdef HAVE_ACOSH
	HAPLO_MATH1_REGISTER(acosh),
#endif
#ifdef HAVE_ASINH
	HAPLO_MATH1_REGISTER(asinh),
#endif
#ifdef HAVE_ATANH
	HAPLO_MATH1_REGISTER(atanh),
#endif
#ifdef HAVE_CBRT
	HAPLO_MATH1_REGISTER(cbrt),
#endif
#ifdef HAVE_ERF
	HAPLO_MATH1_REGISTER(erf),
#endif
#ifdef HAVE_ERFC
	HAPLO_MATH1_REGISTER(erfc),
#endif
#ifdef HAVE_EXPM1
	HAPLO_MATH1_REGISTER(expm1),
#endif
#ifdef HAVE_J0
	HAPLO_MATH1_REGISTER(j0),
#endif
#ifdef HAVE_J1
	HAPLO_MATH1_REGISTER(j1),
#endif
#ifdef HAVE_LGAMMA
	HAPLO_MATH1_REGISTER(lgamma),
#endif
#ifdef HAVE_LOG1P
	HAPLO_MATH1_REGISTER(log1p),
#endif
#ifdef HAVE_LOGB
	HAPLO_MATH1_REGISTER(logb),
#endif
#ifdef HAVE_Y0
	HAPLO_MATH1_REGISTER(y0),
#endif
#ifdef HAVE_Y1
	HAPLO_MATH1_REGISTER(y1),
#endif
#ifdef HAVE_HYPOT
	HAPLO_MATH2_REGISTER(hypot),
#endif
	{ NULL, NULL, NULL, NULL }
};


/**
 * Initialize builtin functions
 */
void __haplo_builtin_init(void)
{
	/*
	 * caches initializations
	 */
	builtin_float_cache=haplo_mcache_new(BUILTIN_CACHE_SIZE,
						   sizeof(haplo_float_t));

	builtin_boolean_cache=haplo_mcache_new(BUILTIN_CACHE_SIZE,
						     sizeof(haplo_boolean_t));

	/*
	 * Types and functions registrations
	 */
	__haplo_object_type_register(builtin_types, NULL);
	__haplo_func_register(builtin_functions, NULL);

	/*
	 * Initialize cache
	 */
	__haplo_object_type_float   = haplo_object_type_get("float");
	__haplo_object_type_string  = haplo_object_type_get("string");
	__haplo_object_type_list    = haplo_object_type_get("list");
	__haplo_object_type_boolean = haplo_object_type_get("boolean");
	__haplo_object_type_code    = haplo_object_type_get("code");
	__haplo_object_type_hash    = haplo_object_type_get("hash");

	return;
}


/**
 *
 */
void __haplo_builtin_fini(void)
{
	haplo_mcache_free(builtin_float_cache);
	haplo_mcache_free(builtin_boolean_cache);

	return;
}


/**
 *
 */
static void builtin_abort(void)
{
	haplo_info("Execution aborted due to user request.");
	
	exit(EXIT_FAILURE);
}


/**
 *
 */
static void builtin_bordered(const haplo_string_t *s)
{
	haplo_bordered("%s", s);
	return;
}


/**
 *
 */
static void builtin_centered(const haplo_string_t *s)
{
	haplo_centered("%s", s);
	return;
}


/**
 *
 */
static void builtin_underlined(const haplo_string_t *s)
{
	haplo_underlined("%s", s);
	return;
}
