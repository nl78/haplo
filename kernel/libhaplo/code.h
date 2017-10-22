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
 
#ifndef __HAPLO_CODE_H__
#define __HAPLO_CODE_H__

#include <haplo/code.h>

#include "func.h"
#include "object.h"
#include "utils.h"


/*
 * parse_param_t is not yet known.
 * we using struct parse_param_t instead.
 */

/*-----------------------------------------------------------------------------
                            E N U M E R A T I O N S
-----------------------------------------------------------------------------*/

typedef enum leaf_action_e
{
	LEAF_ACTION_ASSIGN,		/* assign child, arg is a ref */
	LEAF_ACTION_BLOCK,		/* this is a block there's a code */
	LEAF_ACTION_BREAK,
	LEAF_ACTION_CONTINUE,
	LEAF_ACTION_EXECUTE,		/* execute children */
	LEAF_ACTION_GET,		/* get a member of a HASH or LIST */
	LEAF_ACTION_SET,		/* set a member of a HASH or LIST */
	LEAF_ACTION_FREE,		/* free an object */
	LEAF_ACTION_FUNCTION,		/* there's a function to execute */
	LEAF_ACTION_HASH,		/* construction of Hash */
	LEAF_ACTION_IF,			/* conditionnal branchment */
	LEAF_ACTION_LIST,		/* contruction of list */
	LEAF_ACTION_LOAD,		/* load a file */
	LEAF_ACTION_LOCAL_ASSIGN,	/* Assign local var. */
	LEAF_ACTION_LOCAL_REF,		/* there's local reference to update */
	LEAF_ACTION_OBJECT,		/* the result is a given object */
	LEAF_ACTION_CHILD,		/* just execute children  */
	LEAF_ACTION_REF,		/* there's a reference to update*/
	LEAF_ACTION_RECURSIVE,		/* the result is context->code */
	LEAF_ACTION_RETURN,		/* return keyword */
	LEAF_ACTION_WHILE		/* WHILE statement */
} action_t;


/*-----------------------------------------------------------------------------
                               S T A T U S _ T
-----------------------------------------------------------------------------*/

typedef enum status_e
{
	STATUS_OK,		/* computation achieved with no problem */
	STATUS_CONTINUE,	/* `continuer' keyword encounted */
	STATUS_BREAK,		/* `break' keyword encounted */
	STATUS_ERROR,		/*  error during computation */
	STATUS_RETURN,
} status_t;


/*-----------------------------------------------------------------------------
                                  L E A F _ T
-----------------------------------------------------------------------------*/

typedef struct leaf_s
{
	struct leaf_s		*brother;
	struct leaf_s		*child;
	unsigned short		n;
	action_t		action;
	union
	{
		struct code_s			*code; 		/* BLOCK */
		reference_t			*ref;		/* REF */
		const func_t			*function;	/* FUNCTION */
		const struct parser_param_s	*parser_param;	/* LOAD */
		object_t			*object;	/* OBJECT */
	} arg;
	const char		*filename;
	unsigned int		line;
} leaf_t;


/*-----------------------------------------------------------------------------
                                B R A N C H _ T
-----------------------------------------------------------------------------*/

typedef struct branch_s
{
	leaf_t			*first;
	leaf_t			*last;
	unsigned int		n;
} branch_t;


/*-----------------------------------------------------------------------------
                             P R E C O D E _ T
-----------------------------------------------------------------------------*/

typedef struct precode_s
{
	reference_t	*db[OBJECT_HASH_TABLE_SIZE];	/* local var. */
	unsigned int	n;				/* nb of args. */
	reference_t	**args;				/* ref. of args */
	leaf_t		*first;
	leaf_t		*last;
} precode_t;


/*-----------------------------------------------------------------------------
                                  C O D E _ T
-----------------------------------------------------------------------------*/

typedef struct code_s
{
	unsigned long	n;	/* size of local context       */
	unsigned int	args;	/* numbers of args */
	unsigned int	ops;	/* total of leaf */
	leaf_t		*entry;
} code_t;


/*-----------------------------------------------------------------------------
                               C O N T E X T _ T
-----------------------------------------------------------------------------*/

typedef struct context_s 
{
	object_t		**db;
	unsigned long		n;
	code_t			*code;
	const struct context_s	*father;
} context_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define CODE_CONTEXT_POOL_SIZE	16
#define CODE_LEAF_POOL_SIZE	128

#define CODE(x)			((code_t *)x)
#define PRECODE(x)		((precode_t *)x)
#define RESULT_ASSIGN(x,y)	if (*(x) != y) {	\
		__haplo_object_free(*(x));		\
		*(x)=y;					\
		OBJECT_REF(y);				\
	}

#define RESULT_ASSIGN_NULL(x)	if (*(x)) {		\
		__haplo_object_free(*(x));		\
		*(x)=NULL;				\
	}


/*-----------------------------------------------------------------------------
                              P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void __haplo_code_init(void);
extern void __haplo_code_fini(void);

/*
 * Constructors and destructor
 */

extern void __haplo_code_leaf_free(leaf_t *leaf);
extern leaf_t *__haplo_code_leaf_from_arrow(leaf_t *hash, leaf_t *indice,
					    const char *filename, int line);
extern leaf_t *__haplo_code_leaf_from_hash(leaf_t *key, leaf_t *value,
					   const char *filename, int line);
extern void __haplo_code_hash_set(leaf_t *hash, leaf_t *value);
extern void __haplo_code_add_hash_entry(leaf_t *hash, leaf_t *indice,
					leaf_t *value);
extern leaf_t *__haplo_code_leaf_from_ref(reference_t *ref,
					  const char *filename,
					  int line);
extern leaf_t *__haplo_code_leaf_from_object(object_t *object,
					     const char *filename,
					     int line);
extern leaf_t *__haplo_code_leaf_from_function(func_t *f,
					       const branch_t *argl,
					       const char *filename,
					       int line);
extern leaf_t *__haplo_code_leaf_from_execute(leaf_t *code,
					      const branch_t *argl,
					      const char *filename,
					      int line);
extern leaf_t *__haplo_code_leaf_from_block(code_t *code,
					    const char *filename,
					    int line);
extern leaf_t *__haplo_code_leaf_from_unary(const char *function, leaf_t *a,
					    const char *filename,
					    int line);
extern leaf_t *__haplo_code_leaf_from_binary(const char *function, leaf_t *a,
					     leaf_t *b,
					     const char *filename,
					     int line);
extern leaf_t *__haplo_code_leaf_from_assign(leaf_t *result, reference_t *ref,
					     const char *filename, int line);
extern leaf_t *__haplo_code_leaf_from_free(reference_t *ref,
					   const char *filename,
					   int line);
extern leaf_t *__haplo_code_leaf_from_list(leaf_t *first,
					   const char *filename,
					   int line);
extern leaf_t *__haplo_code_leaf_from_extract(leaf_t *list, leaf_t *indice,
					      const char *filename,
					      int line);
leaf_t *__haplo_code_leaf_from_list(leaf_t *first,
				    const char *filename,
				    int line);
extern leaf_t *__haplo_code_leaf_from_load(leaf_t *filetoload,
					   const struct parser_param_s *param,
					   const char *filename,
					   int line);
extern leaf_t *__haplo_code_leaf_from_if(leaf_t *cond, leaf_t *t, leaf_t *f,
					 const char *filename,
					 int line);
extern leaf_t *__haplo_code_leaf_from_while(leaf_t *cond, leaf_t *body,
					    const char *filename,
					    int line);
extern leaf_t *__haplo_code_leaf_from_for(leaf_t *pre, leaf_t *cond,
					  leaf_t *post, leaf_t *body,
					  const char *filename, int line);
extern leaf_t *__haplo_code_leaf_from_return(leaf_t *value,
					     const char *filename, int line);
extern leaf_t *__haplo_code_leaf_from_break(const char *filename, int line);
extern leaf_t *__haplo_code_leaf_from_continue(const char *filename, int line);

extern void __haplo_code_add_child(leaf_t *father, leaf_t *child);
extern void __haplo_code_add_leaf(precode_t *code, leaf_t *leaf);


/*
 * Execution
 */
extern object_t *__haplo_code_leaf_execute(const leaf_t *leaf);


/*
 * Object related stuff.
 */
extern precode_t * __haplo_precode_new(void);
extern void __haplo_precode_free(precode_t *precode);
extern code_t *__haplo_code_from_precode(const precode_t *precode);
extern unsigned int __haplo_code_count_object(const object_t *object,
					      const leaf_t *leaf);
extern void __haplo_code_free(code_t *code);
extern void __haplo_code_display(const code_t *code);
extern code_t * __haplo_code_copy(const code_t *code);
extern void __haplo_code_print(const code_t *code);

/*
 * Object freeing stuff
 */
extern void __haplo_code_remove_ref(object_t *object);


#endif /* __HAPLO_CODE_H__ */


