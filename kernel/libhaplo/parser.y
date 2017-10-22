/*
 * Copyright (C) 1999, 2000, 2001 Nicolas LAURENT
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
 * This file contains the main loop.
 * Process this file with `bison-2.0'.
 *
 */

%{
#include "extensions.h"
#include <haplo/config.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <haplo/init.h>

#include "builtin.h"
#include "code.h"
#include "error.h"
#include "hash.h"
#include "object.h"
#include "memory.h"
#include "parser.h"
#include "utils.h"

#ifndef PATH_MAX
#	define PATH_MAX	1024
#endif /* PATH_MAX */


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

/*
 * Local macros
 */
#define CURRENT_PRECODE	PRECODE(parser_param->lex.context->data)
#define CURRENT_DB	(CURRENT_PRECODE->db)
#define ROOT_DB		(parser_param->lex.db)
%}


/*-----------------------------------------------------------------------------
                           D E C L A R A T I O N S 
-----------------------------------------------------------------------------*/

%require "2.1"
%pure_parser
%locations
%name-prefix="__haplo_parser_"
%lex-param   { parser_param_t * parser_param }
%parse-param { parser_param_t * parser_param }
%expect 0

%union
{
	leaf_t		*leaf;
	char		*string;	/* Token */
	func_t		*function;	/* Token */
	reference_t	*ref;		/* Token */
	object_t	*object;	/* Token */
	branch_t	branch;
	slink_t		*slink;
}


/*
 * non-terminal symbol
 */
%start 			input				

/*-----------------------------------------------------------------------------
                            I N S T R U C T I O N 
-----------------------------------------------------------------------------*/
%type <leaf>		instruction				/* x01x */
%type <leaf> 		instruction_expression  		/* x02x */
%type <leaf>		instruction_free			/* x03x */
%type <leaf> 		instruction_function		        /* x04x */ 
%type <leaf>		instruction_if				/* x05x */
%type <leaf> 		instruction_keyword     		/* x06x */
%type <leaf> 		instruction_loop			/* x07x */
%type <leaf> 		loop_while 				/* x07x */
%type <leaf> 		loop_for				/* x07x */

/*-----------------------------------------------------------------------------
                                  B L O C K 
-----------------------------------------------------------------------------*/
%type <leaf>		block					/* x10x */
%type <leaf> 		block_end				/* x11x */
%type <leaf> 		block_begin				/* x12x */

/*-----------------------------------------------------------------------------
                             E X P R E S S I O N 
-----------------------------------------------------------------------------*/
%type <leaf>		expression_or_empty			/* x20x */
%type <leaf>		expression				/* x21x */

%type <leaf>		expression_assignment			/* x22x */
%type <ref>		assignment_lvalue			/* x23x */
%type <leaf>		assignment_rvalue_invalid		/* x24x */

%type <leaf>		expression_binary			/* x25x */

%type <leaf>		expression_execution			/* x30x */
%type <branch>		execution_arguments			/* x31x */
%type <branch>		execution_arguments_content		/* x32x */
%type <leaf>		execution_arguments_atom		/* x33x */
%type <leaf>		execution_arguments_atom_or_empty	/* x34x */

%type <leaf>		expression_get                  	/* x37x */
%type <leaf>		expression_object			/* x38x */
%type <leaf>		expression_unary			/* x39x */


%type <leaf>		expression_invalid			/* x26x */
%type <leaf>		expression_invalid_atom			/* x27x */

/*-----------------------------------------------------------------------------
                               F U N C T I O N 
-----------------------------------------------------------------------------*/
%type <leaf>		function_definition			/* x40x */
%type <leaf>		function_definition_begin		/* x41x */
%type <leaf>		function_definition_end			/* x42x */
%type <slink>		function_arguments			/* x43x */
%type <string>		function_arguments_atom			/* x44x */

/*-----------------------------------------------------------------------------
                                   H A S H 
-----------------------------------------------------------------------------*/
%type <leaf>		hash_definition				/* x50x */
%type <leaf>		hash_member_name			/* x51x */
%type <leaf>		hash_content 				/* x52x */

/*-----------------------------------------------------------------------------
                                   L I S T 
-----------------------------------------------------------------------------*/
%type <leaf>		list_definition				/* x60x */
%type <leaf>		list_content				/* x61x */

/*-----------------------------------------------------------------------------
                                   M I S C 
-----------------------------------------------------------------------------*/
%type <string>	        keyword					/* x71x */
%type <string>		operator				/* x72x */

/*                      LEXER                                      x80x */

/*
 * Tokens
 */
%token <string>		STRING
%token <function>	FUNCTION
%token <ref>		REFERENCE
%token <object>		VALUE


/* Operators */
%right			'='
%right			'!'
%nonassoc		EQUAL '<' '>' LE GE
%right			'&' '|' '~'

%left			'-' '+'
%left			'*' '/'
%right			'^'

%right			UNARY	/* precedence of Unary-operator */

%nonassoc		':'
%nonassoc		'{' '}'
%nonassoc		'[' ']'
%nonassoc		'(' ')'
%right			ARROW

/* Keywords */
%token	 		BREAK
%token	 		CONTINUE
%token	 		DEFINE_FUNCTION
%token	 		END
%token	 		FOR
%token	 		FREE
%token			INCLUDE
%token	 		QUIT
/* structural control token */
%token	 		RETURN
%token	 		WHILE
%nonassoc		IF
%nonassoc 		ELSE

/*
 * DESTRUCTORS
 *
 * This is new in bison 2.1. Theses declarations will avoid memory leaks.
 */

%destructor { HAPLO_FREE($$); } function_arguments_atom keyword STRING
%destructor { __haplo_code_leaf_free($$); } instruction instruction_expression instruction_free instruction_function instruction_if instruction_keyword instruction_loop loop_while loop_for block block_end block_begin expression_or_empty expression expression_assignment assignment_rvalue_invalid expression_binary expression_execution execution_arguments_atom execution_arguments_atom_or_empty expression_get expression_object expression_unary expression_invalid expression_invalid_atom function_definition function_definition_begin function_definition_end hash_definition hash_member_name hash_content list_definition list_content
%destructor { __haplo_object_free($$); } VALUE;
%destructor { if ($$) __haplo_object_ref_free($$); } REFERENCE assignment_lvalue;

%{
/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static input_t			parser_input_method[INPUT_NUMBER];
static slink_t			*parser_search_path=NULL;
static slink_t			*parser_files=NULL;

#ifdef HAVE_READLINE
static reference_t		**completion_base;
static const slink_t 		*completion_slink;
#endif

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static void parser_instruction(slink_t *context, int verbose,
			       leaf_t *instruction);
void __haplo_parser_method_set(int method, input_t input);
int haplo_parser_path_add(const char *path);
void __haplo_parser_fini(void);
void __haplo_parser_init(haplo_param_t *haplo_param);
int haplo_main(haplo_param_t *haplo_param);
static void parser_status(const parser_param_t *param);
const char * __haplo_parser_load(const char *filename,
				 const parser_param_t *param);
static void parser_error_line(const char *s, int begin, int end, int old);
static void parser_error(const parser_param_t *param, 
			 const struct yyltype *llocp,
			 int n, ...);
static void yyerror (YYLTYPE *locp, parser_param_t *parser_param, 
		     const char *s);
static int parser_lex_getline(lex_param_t *lex_param);
static reference_t * parser_autoload(struct yyltype *llocp,
				     const char *symbol,
				     parser_param_t *param);
static int parser_lex_string(YYSTYPE *lvalp, struct yyltype *llocp,
			     parser_param_t *param);
static int parser_lex_single(struct yyltype *llocp, parser_param_t *param);
static int __haplo_parser_lex(YYSTYPE *lvalp, struct yyltype *llocp,
			       parser_param_t *param);
static int parser_redirect_stdin(int from);
static const char *parser_file_check(const char *filename);
static char * parser_file_search(const char *filename);
static reference_t *parser_object_get(const parser_param_t *param,
				      const char *name);
#ifdef HAVE_READLINE
char *__haplo_parser_completion(const char *beginning, int seq);
#endif
%}


%%

/*-----------------------------------------------------------------------------
                                G R A M M A R 
-----------------------------------------------------------------------------*/

/**
 * Execute one or more instructions.
 */
input
	: /* nothing */

        | input END
        {
		YYACCEPT;
	}

	| input instruction
	{
		if (!parser_param->lex.context)
			parser_param->lex.flags |= PARSER_LEX_FREE;
		
		parser_instruction(parser_param->lex.context,
			    (parser_param->lex.flags & PARSER_LEX_VERBOSE) ||
			    (parser_param->lex.input == INPUT_USER)
			    ,$2);
	}
;


/*-----------------------------------------------------------------------------
                            I N S T R U C T I O N 
-----------------------------------------------------------------------------*/

/**
 * Simple statement. Example: "a=2+2;"
 */
instruction
	: instruction_expression
	| block
	| instruction_free
	| instruction_function
	| instruction_if
	| instruction_keyword
	| instruction_loop
	| error ';'
	{
		parser_error(parser_param, &@2, 2010);
		parser_param->lex.flags &=
			~(PARSER_LEX_ERROR | PARSER_LEX_ERROR_MSG);
		$$=NULL;
	}

	| error END
	{
		parser_error(parser_param, &@2, 2011);
		$$=NULL;
		YYACCEPT;
	}

	| error '}' 
	{
		parser_error(parser_param, &@2, 2012);
		$$=NULL;
	}
;


/**
 *
 */
instruction_expression
        : expression_or_empty ';'
        {
		$$=$1;
	}

        | expression END
        {
		parser_error(parser_param, &@2, 1020);
		$$=$1;
        }

	| assignment_lvalue '=' assignment_rvalue_invalid ';'
	{
		parser_error(parser_param, &@3, 2021);
		$$=__haplo_code_leaf_from_assign($3, $1, 
						 parser_param->lex.filename,
						 @2.last_line);
	}
;


/**
 *
 */
instruction_free
	: FREE expression ';'
	{
		if ($2 && ($2->n == 0) && ($2->action == LEAF_ACTION_REF))
		{
			reference_t *ref=$2->arg.ref;
			
			
			if (parser_param->lex.context)
			{
				$$=__haplo_code_leaf_from_free(
					ref,
					parser_param->lex.filename,
					@1.last_line);
			} else {
				__haplo_object_ref_free_db(ROOT_DB, ref);
				$$=NULL;
			} 
		} else {
			parser_error(parser_param, &@2, 2030);
			$$=NULL;
		}	
		__haplo_code_leaf_free($2);

	}

        | FREE ';'
        {
		/* free all db */
		if (parser_param->lex.context)
		{
			/* not allowed */
			parser_error(parser_param, &@1, 2031);
		} else {
			__haplo_object_db_free_root(parser_param->lex.db);
			__haplo_object_db_init(parser_param->lex.db);
		}
		
		$$=NULL;
        }
;


/**
 *
 * Error:
 */
instruction_function
	: FUNCTION ';'
	{
		branch_t null={NULL, NULL, 0};
		$$=__haplo_code_leaf_from_function($1,
						   &null,
						   parser_param->lex.filename,
						   @1.last_line);
	}
;


/**
 *
 * Error:
 */
instruction_if
	: IF '(' expression ')' instruction ELSE instruction
	{
		$$=__haplo_code_leaf_from_if($3, $5, $7,
					     parser_param->lex.filename,
					     @1.last_line);
	}

	| IF '(' expression ')' instruction
	{
		$$=__haplo_code_leaf_from_if($3, $5, NULL,
					     parser_param->lex.filename,
					     @1.last_line);
	}
;


/**
 *
 */
instruction_keyword
	: BREAK ';'
	{
		if (! parser_param->in_loop)
			parser_error(parser_param, &@1, 1061);

		$$=__haplo_code_leaf_from_break(parser_param->lex.filename,
						@1.last_line);
	}
	
	| CONTINUE ';'
	{
		if (! parser_param->in_loop)
			parser_error(parser_param, &@1, 1062);

		$$=__haplo_code_leaf_from_continue(parser_param->lex.filename,
						   @1.last_line);
	}
	
	| RETURN ';'
	{
		if (! parser_param->in_function)
			parser_error(parser_param, &@1, 1063);

		$$=__haplo_code_leaf_from_return(NULL,
						 parser_param->lex.filename,
						 @1.last_line);
	}
	
	| RETURN expression ';'
	{
		if (! parser_param->in_function)
			parser_error(parser_param, &@1, 1064);

		$$=__haplo_code_leaf_from_return($2,
						 parser_param->lex.filename,
						 @1.last_line);
	}

        | INCLUDE ';'
        {
		parser_status(parser_param);
		$$=NULL;
	}

        | INCLUDE execution_arguments ';'
        {
		switch($2.n)
		{
		case 0:
			parser_status(parser_param);
			$$ = NULL;
			break;
		case 1:
			$$=__haplo_code_leaf_from_load(
				$2.first,
				parser_param,
				parser_param->lex.filename,
				@2.last_line);
			break;
		default:
			parser_error(parser_param, &@1, 2060);
			break;
		}
	}

	| QUIT ';'
	{
		$$=NULL;
		YYACCEPT;

	}
;


/**
 *
 * Error:
 */
instruction_loop
        : loop_for '(' expression ';' expression ';' expression ')' instruction
	{
		(void)$1;
		$$=__haplo_code_leaf_from_for($3, $5, $7, $9,
					      parser_param->lex.filename,
					      @1.last_line);
		parser_param->in_loop -= 1;
	}

	| loop_while '(' expression ')' instruction
	{
		(void)$1;
		$$=__haplo_code_leaf_from_while($3, $5,
						parser_param->lex.filename,
						@1.last_line);
		parser_param->in_loop -= 1;
	}
;


loop_for
	: FOR
	{
		parser_param->in_loop += 1;
		$$ = NULL;
	}
;

loop_while
	: WHILE
	{ 
		parser_param->in_loop += 1;
		$$ = NULL;
	}
;



/*-----------------------------------------------------------------------------
                                  B L O C K 
-----------------------------------------------------------------------------*/

/**
 *
 * Error: x23x
 */
block
	: block_begin input expression_or_empty block_end
	{
		(void)$1;
		
		if ($3)
		{
			parser_error(parser_param, &@3, 2100);
		}
		$$=$4;
	}
;


/**
 *
 * Error: none
 */
block_begin
	: '{'
	{
		/*
		 * Pushing a new local context
		 */
		slink_t	*s=__haplo_slink_new(__haplo_precode_new());
		s->next=parser_param->lex.context;
		parser_param->lex.context=s;

		$$=NULL;
	} 
;


/**
 *
 * Error: none
 */
block_end
	: '}'
	{
		/*
		 * Poping the context and create the leaf.
		 */
		slink_t	*save=parser_param->lex.context;
		$$=__haplo_code_leaf_from_block(
			__haplo_code_from_precode(CURRENT_PRECODE),
			parser_param->lex.filename,
			@1.last_line);
		__haplo_precode_free(CURRENT_PRECODE);
		parser_param->lex.context=parser_param->lex.context->next;
		HAPLO_FREE(save);
	}
;


/*-----------------------------------------------------------------------------
                             E X P R E S S I O N 
-----------------------------------------------------------------------------*/

/**
 *
 * Error: none
 */
expression_or_empty
	: /* empty */
	{
		$$=NULL;
	}

	| expression
;


/**
 *
 * Error: none
 */
expression
	: expression_assignment
	| expression_binary
	| expression_execution
	| expression_get
	| expression_object
	| expression_unary
	| function_definition
	| hash_definition
        | list_definition
	| expression_invalid
	{
		parser_param->lex.flags &= 
			~(PARSER_LEX_ERROR | PARSER_LEX_ERROR_MSG);

		$$=$1;
	}
;


/**
 *
 * Error: x33x
 */
expression_assignment /* x33x */
        : expression_get '=' expression
	{
		if ($1)
			__haplo_code_hash_set($1, $3);
		else {
			parser_error(parser_param, &@1, 2220);
			$$=$3;
		}
	}

	| assignment_lvalue '=' expression
	{
		if ($1)
			$$=__haplo_code_leaf_from_assign($3, $1,
						 parser_param->lex.filename,
						 @2.last_line);
		else
		{
			parser_error(parser_param, &@1, 2221);
			$$=$3;
		}
	}
;


/**
 *
 * Error: x32x
 */
assignment_lvalue
        :  REFERENCE
	| STRING
	{
		reference_t	*ref;

		if (parser_param->lex.context)
			ref=__haplo_object_ref_new(CURRENT_DB,$1);
		else
			ref=__haplo_object_ref_new(ROOT_DB, $1);
		ref->instances += 1;
		$$=ref;
	}	

	| FUNCTION
	{
		reference_t	*ref;
		
		parser_error(parser_param, &@1, 2230,
			     $1->name);

		
		if (parser_param->lex.context)
			ref=__haplo_object_ref_new(
				CURRENT_DB,
				haplo_strdup($1->name));
		else
			ref=__haplo_object_ref_new(
				ROOT_DB,
				haplo_strdup($1->name));
		
		ref->instances += 1;
		$$=ref;
	}

	| VALUE
	{
		OBJECT_REF($1); /* to make __haplo_object_free() to work */
		__haplo_object_free($1);
		$$=NULL;
	}

	| operator
	{
		$$=NULL;
	}

        | keyword
        {
		HAPLO_FREE($1);
		$$=NULL;
	}
;


/**
 *
 * Error: none
 */
assignment_rvalue_invalid
        : /* empty */ { $$=NULL; }
        | keyword     { HAPLO_FREE($1); $$=NULL; }
        | FUNCTION    { $$=NULL; }
        | operator    { $$=NULL; }
;


/**
 *
 * Error: none
 */
expression_binary
	:expression '+' expression
	{
		$$=__haplo_code_leaf_from_binary("+", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '-' expression
	{
		$$=__haplo_code_leaf_from_binary("-", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '*' expression
	{
		$$=__haplo_code_leaf_from_binary("*", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '/' expression
	{
		$$=__haplo_code_leaf_from_binary("/", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '^' expression
	{
		$$=__haplo_code_leaf_from_binary("^", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '|' expression
	{
		$$=__haplo_code_leaf_from_binary("|", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '&' expression
	{
		$$=__haplo_code_leaf_from_binary("&", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression EQUAL expression
	{
		$$=__haplo_code_leaf_from_binary("=", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '<' expression
	{
		$$=__haplo_code_leaf_from_binary("<", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression LE expression
	{
		$$=__haplo_code_leaf_from_binary("<=", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '>' expression
	{
		$$=__haplo_code_leaf_from_binary(">", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression GE expression
	{
		$$=__haplo_code_leaf_from_binary(">=", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}

	| expression '~' expression
	{
		$$=__haplo_code_leaf_from_binary("~", $1, $3,
						 parser_param->lex.filename,
						 @2.last_line);
	}
;


/**
 *
 * Error: none
 */
expression_execution
	: expression execution_arguments
	{
		/* AUTOLOAD */
		if ((parser_param->autoload) &&
		    ($1) &&
		    ($1->action == LEAF_ACTION_OBJECT) &&
		    ($1->arg.object->type == OBJECT_STRING))
		{
			reference_t *ref;
			
			/* might throw error 2300 */
			ref=parser_autoload(&@1, $1->arg.object->content,
					    parser_param);
			if (ref)
			{
				leaf_t *leaf;

				leaf=__haplo_code_leaf_from_ref(
					ref,
					parser_param->lex.filename,
					@1.last_line);

				$$=__haplo_code_leaf_from_execute(
					leaf, &$2, 
					parser_param->lex.filename,
					@1.last_line);
					
			} else {
				parser_error(parser_param, &@1, 2301,
					     $1->arg.object->content);

				__haplo_code_leaf_free($2.first);
				$$=NULL;
			}

			__haplo_code_leaf_free($1);

		} else
			$$=__haplo_code_leaf_from_execute(
				$1, &$2, 
				parser_param->lex.filename,
				@1.last_line);
	}

	| FUNCTION execution_arguments
	{
		$$=__haplo_code_leaf_from_function($1,
						   &$2,
						   parser_param->lex.filename,
						   @1.last_line);
	}
;


/**
 *
 * Error: none
 */
execution_arguments
	: '(' ')'
	{
		branch_t null={NULL, NULL, 0};
		$$=null;
	}
				
	| '(' execution_arguments_content ')'
	{
		$$=$2;
	}
;


/**
 *
 * Error: x42x */
execution_arguments_content
	: execution_arguments_atom
	{
		if ($1)
		{
			$$.first=$1;
			$$.last=$1;
			$$.n=1;
		} else {
			$$.n=0;
		}
	}

	| execution_arguments_content ',' execution_arguments_atom_or_empty
	{
		leaf_t	*arg;
		
		if ( ! $3)
		{
			parser_error(parser_param, &@2, 2320);
			arg=__haplo_code_leaf_from_object(
				__haplo_object_from_double(0.0),
				parser_param->lex.filename,
				@1.last_line);

		}
		else
			arg=$3;
		
		$$=$1;
		$$.last->brother=arg;
		$$.last=arg;
		$$.n++;
	}
;


/**
 *
 * Error: none
 */
execution_arguments_atom_or_empty
	: /* empty */
	{
		$$=NULL;
	}

	| execution_arguments_atom
;



/**
 *
 * Error: none
 */
execution_arguments_atom
	: expression

	| keyword
	{
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_string($1),
			parser_param->lex.filename,
			@1.last_line);
	}

	| FUNCTION
	{
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_string(
				haplo_strdup($1->name)),
			parser_param->lex.filename,
			@1.last_line);
	}

	| operator
	{
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_string(haplo_strdup($1)),
			parser_param->lex.filename,
			@1.last_line);
	}
;


/**
 *
 * Error: none
 */
expression_get

	: VALUE ARROW hash_member_name
	{
		leaf_t *hash;
		
		hash=__haplo_code_leaf_from_object($1,
						   parser_param->lex.filename,
						   @1.last_line);
		
		$$=__haplo_code_leaf_from_arrow(hash, $3,
						parser_param->lex.filename,
						@3.last_line);
	}

	| REFERENCE ARROW hash_member_name
	{
		leaf_t *hash;
		
		hash=__haplo_code_leaf_from_ref($1,
						parser_param->lex.filename,
						@1.last_line);
		
		$$=__haplo_code_leaf_from_arrow(hash, $3,
						parser_param->lex.filename,
						@3.last_line);
	}

	| expression_get ARROW hash_member_name
	{
		$$=__haplo_code_leaf_from_arrow($1, $3,
						parser_param->lex.filename,
						@3.last_line);
	}

	| '(' expression ')' ARROW hash_member_name
	{
		$$=__haplo_code_leaf_from_arrow($2, $5,
						parser_param->lex.filename,
						@5.last_line);

	}

	| STRING ARROW hash_member_name
	{
		/* Fix error number */
		parser_error(parser_param, &@1, 2370,
			     $1);
		__haplo_code_leaf_free($3);
		HAPLO_FREE($1);
		$$=NULL;
	}
;


/**
 *
 * Error: x40x
 */
expression_object
	: VALUE
	{

		$$=__haplo_code_leaf_from_object($1,
						 parser_param->lex.filename,
						 @1.last_line);
	}

	| REFERENCE
	{
		$$=__haplo_code_leaf_from_ref($1,
					      parser_param->lex.filename,
					      @1.last_line);
	}
	
	| STRING
	{
		if (! parser_param->autoload)
			parser_error(parser_param, &@1, 1380,
				     $1);
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_string($1),
			parser_param->lex.filename,
			@1.last_line);
	}
;


/**
 *
 * Error: none
 */
expression_unary
	: '!' expression
	{
		$$=__haplo_code_leaf_from_unary("!", $2,
						parser_param->lex.filename,
						@1.last_line);
	}

	| '+'  %prec UNARY  expression
	{
		$$=__haplo_code_leaf_from_unary("++", $2,
						parser_param->lex.filename,
						@1.last_line);
	}

	| '-'  %prec UNARY  expression
	{
		$$=__haplo_code_leaf_from_unary("--", $2,
						parser_param->lex.filename,
						@1.last_line);
	}

	| '(' %prec UNARY expression ')'
	{
		$$=$2;
	}
;


/**
 *
 * Error: x31x
 */
expression_invalid
	: expression_invalid_atom expression_invalid_atom
	{	
		parser_error(parser_param, &@1, 2260);
		parser_instruction(parser_param->lex.context,
			    (parser_param->lex.flags & PARSER_LEX_VERBOSE) ||
			    (parser_param->lex.input == INPUT_USER),
			    $1);
		$$=$2;
	}

        | '(' ')'
        {
		parser_error(parser_param, &@2, 2261);
		$$=NULL;
		
        }


	| expression_invalid expression_invalid_atom
	{
		parser_error(parser_param, &@1, 2262);
		parser_instruction(
			parser_param->lex.context,
			(parser_param->lex.flags & PARSER_LEX_VERBOSE) ||
			(parser_param->lex.input == INPUT_USER),
			$1);
		$$=$2;
	}
;


/**
 *
 * Error: none
 */
expression_invalid_atom
	: expression_object
	| function_definition
;


/*-----------------------------------------------------------------------------
                               F U N C T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 * Error: none
 */
function_definition
	: function_definition_begin input function_definition_end
	{
		(void)$1;
		
		$$=$3;
		
		parser_param->in_function -= 1;
	}	
;


/**
 *
 * Error: none
 */
function_definition_begin
	: DEFINE_FUNCTION '(' function_arguments ')' '{'
	{
		/*
		 * Pushing local context
		 */
		unsigned int	n, size;

		slink_t	*s=__haplo_slink_new(__haplo_precode_new());
		s->next=parser_param->lex.context;
		parser_param->lex.context=s;
		size=__haplo_slink_length($3);
		CURRENT_PRECODE->n=size;
		if (size)
		{
			HAPLO_ALLOC(CURRENT_PRECODE->args, size);
			for(n=0, s=$3; s;  s=s->next, n++)
			{
				/*
				 * args store arguments in reverse order.
				 */
				CURRENT_PRECODE->args[size-n-1]=
					__haplo_object_ref_new(
						CURRENT_DB,
						(char *)s->data);
			}

		}
		else
			CURRENT_PRECODE->args=NULL;
		
		__haplo_slink_free($3);

		parser_param->in_function += 1;

		$$=NULL;
	}
;


/**
 *
 * Error: none
 */
function_definition_end
	: '}'
	{
		/*
		 * Poping context and create leaf
		 */
		slink_t	*save=parser_param->lex.context;
		
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_type(
				OBJECT_CODE,
				__haplo_code_from_precode(CURRENT_PRECODE)),
			parser_param->lex.filename,
			@1.last_line);
		__haplo_precode_free(CURRENT_PRECODE);
		parser_param->lex.context=parser_param->lex.context->next;
		HAPLO_FREE(save);
	}
;


/**
 *
 * Error: x43x
 */
function_arguments
	: function_arguments_atom
	{
		if ($1)
			$$=__haplo_slink_new($1);
		else
			$$=NULL;
	}

	| function_arguments ',' function_arguments_atom
	{
		if ($3)
			$$=__haplo_slink_prepend($1, $3);
		else
			parser_error(parser_param, &@2, 2430);
			
	}
;


/**
 *
 * Error: none
 */
function_arguments_atom
	: /* nothing */
	{
		$$=NULL;
	}

	| FUNCTION
	{
		parser_error(parser_param, &@1, 1440,
			     $1->name);
		$$=haplo_strdup($1->name);
	}
	| REFERENCE
	{
		/*
		 * we don't use this ref to create a leaf_t, so we
		 * must decrease instance counter
		 */
		$1->instances -= 1;
		$$=haplo_strdup($1->name);
	}

	| STRING
	| keyword
	{
		parser_error(parser_param, &@1, 2441,
			     $1);
		HAPLO_FREE($1);
		$$=NULL;
	}
;


/*-----------------------------------------------------------------------------
                                   H A S H 
-----------------------------------------------------------------------------*/

hash_definition /* x41x */
        : '[' ']'
        {
		$$=__haplo_code_leaf_from_object(
			__haplo_hash_new(),
			parser_param->lex.filename,
			@1.last_line);
        }


	| '[' hash_content ']'
	{
		$$=$2;
	}
;


/**
 *
 * Error: none
 */
hash_member_name
	: VALUE
	{
		$$=__haplo_code_leaf_from_object($1,
						 parser_param->lex.filename,
						 @1.last_line);
	}

	| FUNCTION
	{
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_string(
				haplo_strdup($1->name)),
			parser_param->lex.filename,
			@1.last_line);
	}

	| REFERENCE
	{
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_string(
				haplo_strdup($1->name)),
			parser_param->lex.filename,
			@1.last_line);
		$1->instances -= 1;
		
	}	

	| keyword
	{
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_string($1),
			parser_param->lex.filename,
			@1.last_line);
	}

	| STRING
	{
		$$=__haplo_code_leaf_from_object(
			__haplo_object_from_string($1),
			parser_param->lex.filename,
			@1.last_line);
	}

	| '(' expression ')'
	{
		$$=$2;
	}
;


/**
 *
 * Error: none
 */
hash_content
	: hash_member_name ':' expression
	{
		$$ = __haplo_code_leaf_from_hash($1, $3,
						 parser_param->lex.filename,
						 @1.last_line);
	}

	| hash_content ',' hash_member_name ':' expression
	{
		__haplo_code_add_hash_entry($1, $3, $5);
		$$=$1;
	}
;




/*-----------------------------------------------------------------------------
                                   L I S T 
-----------------------------------------------------------------------------*/


/**
 *
 * Error: none
 */
list_definition
	: '[' list_content ']'
	{
		$$=$2;
	}
;


/**
 *
 * Error: none
 */
list_content
	: expression
	{
		$$=__haplo_code_leaf_from_list($1,
					       parser_param->lex.filename,
					       @1.last_line);
	}
	
	| list_content ',' expression
	{
		__haplo_code_add_child($1, $3);
		$$=$1;
	}
;


/*-----------------------------------------------------------------------------
                                   M I S C 
-----------------------------------------------------------------------------*/

/**
 *
 * Error: none
 */
keyword : BREAK           { $$=haplo_strdup("break");    }
        | CONTINUE        { $$=haplo_strdup("continue"); }
        | ELSE            { $$=haplo_strdup("else");     }
        | FOR             { $$=haplo_strdup("for");      }
        | FREE            { $$=haplo_strdup("free");     }
        | DEFINE_FUNCTION { $$=haplo_strdup("function"); }
        | IF              { $$=haplo_strdup("if");       }
        | INCLUDE         { $$=haplo_strdup("include");  }
        | QUIT            { $$=haplo_strdup("quit");     }
        | RETURN          { $$=haplo_strdup("return");   }
        | WHILE           { $$=haplo_strdup("while");    }
;


/**
 *
 * Error: none
 */
operator
	: '+'	{ $$ = "+";  }
	| '-'	{ $$ = "-";  }
	| '*'	{ $$ = "*";  }
	| '/'	{ $$ = "/";  }
	| '^'	{ $$ = "^";  }
	| '|'	{ $$ = "|";  }
	| '&'	{ $$ = "&";  }
	| EQUAL	{ $$ = "=";  }
	| '<'	{ $$ = "<";  }
	| LE	{ $$ = "<="; }
	| '>'	{ $$ = ">";  }
	| GE	{ $$ = ">="; }
	| '~'	{ $$ = "~";  }
	| '!'	{ $$ = "!";  }
;

	
%%

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/



/**
 *
 * @param context
 * @param verbose
 * @param instruction
 */
static void parser_instruction(slink_t *context, int verbose, leaf_t *instruction)
{
	if (instruction)
	{
		if (context)
			__haplo_code_add_leaf(context->data, instruction);
		else
		{
			object_t *result;

			result=__haplo_code_leaf_execute(instruction);

			if (verbose && result)
				__haplo_result_display(result);
			
			__haplo_object_free(result);
			
			__haplo_code_leaf_free(instruction);
		}
	}
	
	return;
}


/**
 *
 * @param method
 * @param input
 */
void __haplo_parser_method_set(int method, input_t input)
{
	parser_input_method[method]=input;
	return;
}


/**
 *
 * @param path
 *
 * @return 0
 */
int haplo_parser_path_add(const char *path)
{
	char	*full_path=NULL;
	int	len;
	
	if (path[0] == '/')
	{
		len=strlen(path);
		HAPLO_ALLOC(full_path, len+1);
		strcpy(full_path, path);
	}
	else
	{
		char	here[PATH_MAX];

		getcwd(here, PATH_MAX);
		here[PATH_MAX-1]=0;
		
		len=strlen(here)+strlen(path)+1;
		HAPLO_ALLOC(full_path, len+1);
		strcpy(full_path, here);
		strcat(full_path, "/");
		strcat(full_path, path);
	}
	if ((len > 1) && (full_path[len-1] == '/'))
		full_path[len-1]=0;
	
	parser_search_path=__haplo_slink_prepend(
		parser_search_path, full_path);

	return(0);
}


/**
 *
 */
void __haplo_parser_fini(void)
{
	__haplo_slink_free_f(parser_search_path, SLINK_FUNC(HAPLO_FREE_FUNC));
	__haplo_slink_free_f(parser_files,       SLINK_FUNC(HAPLO_FREE_FUNC));

	return;
}


/**
 *
 * @param haplo_param
 */
void __haplo_parser_init(haplo_param_t *haplo_param)
{
	parser_param_t	*param;

	HAPLO_ALLOC(param, 1);

	if (isatty(STDIN_FILENO)) 
	{
		param->lex.input=INPUT_USER;
		param->lex.flags=0;
	}
	else
	{
		param->lex.input=INPUT_FILE;
		param->lex.flags=PARSER_LEX_VERBOSE;
	}

	param->autoload=haplo_param->autoload;
	param->depth=0;
	param->in_loop = 0;
	param->in_function = 0;
	param->lex.buffer=NULL;
	param->lex.length=0;
	param->lex.position=0;
	param->lex.line=0;
	param->lex.history_line=1;
	
	param->lex.context=NULL;		/* no context at begining */

	HAPLO_ALLOC(param->lex.db, OBJECT_HASH_TABLE_SIZE);
	__haplo_object_db_init(param->lex.db);


	haplo_param->parser_param=param;
	
	return;
}


/**
 *
 * @param haplo_param
 * 
 * @return 0 if there's no error. non nul value otherwise
 */
int haplo_main(haplo_param_t *haplo_param)
{
#define PARSE_PARAM	((parser_param_t *)haplo_param->parser_param)
	int		status=0;

	if (haplo_param->filename)
	{
		const char *error=parser_file_check(haplo_param->filename);
		
		if (! error)
		{
			int	fd;
			fd=open(haplo_param->filename, O_RDONLY);
			PARSE_PARAM->prev_stdin_fileno=
				parser_redirect_stdin(fd);
			PARSE_PARAM->lex.filename=haplo_param->filename;
			PARSE_PARAM->lex.input=INPUT_FILE;
		}
		else
		{
			haplo_error(_("Cannot execute `%s': %s"),
				    haplo_param->filename, error);

			HAPLO_FREE(PARSE_PARAM->lex.db);
			HAPLO_FREE(PARSE_PARAM);

			return(-1);
		}
	}
	else
	{
		PARSE_PARAM->prev_stdin_fileno=STDIN_FILENO;
		PARSE_PARAM->lex.filename="(stdin)";
	}
	
	if (haplo_param->init_filename)
	{
		const char *error=__haplo_parser_load(
			haplo_param->init_filename,
			PARSE_PARAM);

		if (error)
			haplo_warning(_("Cannot load initilization file (%s)"),
				      haplo_param->init_filename);
	}

	status=yyparse(PARSE_PARAM);

	if (parser_input_method[PARSE_PARAM->lex.input].freeline &&
	    PARSE_PARAM->lex.buffer)
	{
		__haplo_slink_free_f(
			PARSE_PARAM->lex.buffer,
			(void (*)(void *))
			parser_input_method[
				PARSE_PARAM->lex.input].freeline);
	}

	__haplo_object_db_free_root(PARSE_PARAM->lex.db);


	HAPLO_FREE(PARSE_PARAM->lex.db);
	HAPLO_FREE(PARSE_PARAM);
	/*
	 * We cannot use PARSE_PARAM to assign something in the next line.
	 * SGI Workshop wouldn't accept it. So revert to macro definition
	 * without the cast.
	 */
	haplo_param->parser_param=NULL;
	
	return(status);
#undef PARSE_PARAM
}


/**
 * Display search pathes for script
 */
static void parser_status(const parser_param_t *param)
{
	char here[PATH_MAX];
	slink_t *p;
	

	haplo_bordered(_("Interpreter status"));
	
	/* Variables */
	__haplo_object_list_display(param->lex.db);
	

	/* Search pathes */

	haplo_underlined("Search pathes");

	getcwd(here, PATH_MAX);
	printf("  - Current directory (%s)\n", here);

	for(p=parser_search_path; p; p=p->next)
		printf("  - %s\n", (const char *)p->data);

	printf("\n");
	
	return;
}


/**
 *
 * @param filename
 * @param parser_param
 *
 * @return error string
 */
const char * __haplo_parser_load(const char *filename,
				 const parser_param_t *parser_param)
{
	parser_param_t	*newparam;
	char		*file;	
	const char	*error=NULL;
	
	HAPLO_ALLOC(newparam, 1);
		
	newparam->lex.input        = INPUT_FILE;
	newparam->lex.buffer       = NULL;
	newparam->lex.length       = 0;
	newparam->lex.position     = 0;
	newparam->lex.flags        = 0;
	newparam->lex.line         = 0;
	newparam->lex.history_line = 1;
	newparam->depth            = parser_param->depth+1;
	newparam->in_loop          = 0;
	newparam->in_function      = 0;
	
	

	if (newparam->depth > PARSER_DEPTH_MAX)
	{
		HAPLO_FREE(newparam);
		return "";
	}
	
	/*
	 * we're sharing namespaces!
	 */
	newparam->lex.context=parser_param->lex.context;
	newparam->lex.db=parser_param->lex.db;

	
	file=parser_file_search(filename);
	
	error = parser_file_check(file);
	
	if (! error)
	{
		int	fd;
		fd=open(file, O_RDONLY);

		newparam->prev_stdin_fileno=parser_redirect_stdin(fd);
		close(fd);
		newparam->lex.filename=file;
	} else {
		HAPLO_FREE(newparam);
		HAPLO_FREE(file);
		return(error);
	}

	/* Store filename in slink */
	parser_files=__haplo_slink_prepend(parser_files, file);
	

	/*
	 * Call a new parser!
	 */
	yyparse(newparam);

	
	if (parser_input_method[newparam->lex.input].freeline &&
	    newparam->lex.buffer)
	{
		__haplo_slink_free_f(
			newparam->lex.buffer,
			(void (*)(void *))
			parser_input_method[
				newparam->lex.input].freeline);
	}

	
	parser_redirect_stdin(newparam->prev_stdin_fileno);


	HAPLO_FREE(newparam);

	return(NULL);
}


/**
 *
 * @param s
 * @param begin
 * @param end
 * @param old
 */
static void parser_error_line(const char *s, int begin, int end, int old)
{
	int i=0;
	
	/* Skip prompt */
	fputs("   ", stdout);
	puts(s);

	if (old)
	{
		/* Skip prompt */
		fputs(" --", stdout);
		i=0;
	}
	else
	{
		/* Skip prompt */
		fputs("   ", stdout);

		for(i=0; i<begin; i++)
		{
			if (s[i] == '\t')
				putchar('\t');
			else
				putchar(' ');
		}
		i=begin;
	}
	
	for(; i<end; i++)
		putchar('^');

	return;
}


/**
 *
 * @param param
 * @param llocp
 * @param n
 */
static void parser_error(const parser_param_t *param, 
			 const struct yyltype *llocp,
			 int n, ...)
{
	va_list	args;
	int i;
	slink_t	*slink=param->lex.buffer;
	const char *color;

	va_start(args, n);
	color=__haplo_error(param->lex.filename, llocp->last_line, NULL,
			    n, args);
	va_end(args);

	if (color)
	{
		__haplo_colors_set(color);

		if (param->lex.history_line <= llocp->last_line)
		{
			for(i=llocp->last_line; i<param->lex.line; i++)
				slink=slink->next;

			parser_error_line(slink->data, llocp->first_column, 
					  llocp->last_column,
					  llocp->first_line!=llocp->last_line);
		}
		__haplo_colors_reset();
		putchar('\n');
	}
	
	return;
}


/**
 * Unhandled error
 *
 * @param locp
 * @param parser_param
 * @param s
 */
static void yyerror(YYLTYPE *locp, parser_param_t *parser_param, const char *s)
{
	parser_param->lex.flags |= PARSER_LEX_ERROR | PARSER_LEX_ERROR_MSG;

	parser_error(parser_param, locp, 2000, s);
	
	return;
}


/**
 *
 * @param lex_param
 *
 * @return 0 if everything is ok. -1 otherwise
 */
static int parser_lex_getline(lex_param_t *lex_param)
{
#define FLAGS		(lex_param->flags)
#define GETLINE		(parser_input_method[lex_param->input].getline)
#define FREELINE	(parser_input_method[lex_param->input].freeline)
#define LEN		(lex_param->length)
#define POS		(lex_param->position)

	
	while(POS >= LEN)
	{
		char	*line;
		
		if (FREELINE && (FLAGS & PARSER_LEX_FREE))
		{
			__haplo_slink_free_f(lex_param->buffer,
					     (void(*)(void *))FREELINE);
			lex_param->buffer=NULL;
			FLAGS &= ~PARSER_LEX_FREE;
			lex_param->history_line=lex_param->line+1;
		}
			
		if (FLAGS & PARSER_LEX_ERROR_MSG)
		{
			haplo_error(_("Input will be ignored until next `;'"));
			FLAGS &= ~PARSER_LEX_ERROR_MSG;
		}

		if (lex_param->input == INPUT_USER)
		{
#ifdef HAVE_READLINE
			completion_slink=lex_param->context;
			completion_base=lex_param->db;
#endif
			line=(*GETLINE)(
				(FLAGS & PARSER_LEX_ERROR)?"## ":">> ");
		}
		else
			line=(*GETLINE)("");

		lex_param->line += 1;
		
		if (!line)
			return(-1);

		lex_param->buffer=__haplo_slink_prepend(lex_param->buffer,
							line);

		if (FLAGS & PARSER_LEX_VERBOSE)
		{
			fputs((FLAGS & PARSER_LEX_ERROR)?"## ":">> ",
			      stdout);
			puts(line);
		}
		
		LEN=strlen(line);
		POS=0;
	}
	
	return(0);
#undef FLAGS
#undef GETLINE
#undef FREELINE
#undef POS
#undef LEN
}


/**
 *
 *
 */
static reference_t * parser_autoload(struct yyltype *llocp,
				     const char *symbol, parser_param_t *param)
{
	char *filename;
	reference_t *ref=NULL;

	HAPLO_ALLOC(filename, strlen(symbol) + 5);
	strcpy(filename, symbol);
	strcat(filename, ".hpl");

	if (strcmp(filename, param->lex.filename) != 0)
	{
		if (access(filename, F_OK)==0)
		{
			__haplo_parser_load(filename, param);
			ref=parser_object_get(param, symbol);

			if (!ref)
				parser_error(param, llocp, 1230,
					     filename, symbol);
		}
	}

	HAPLO_FREE(filename);

	return(ref);
}


/**
 *
 * @param lvalp
 * @param llocp
 * @param param
 *
 * @return token type
 */
static int parser_lex_id(YYSTYPE *lvalp, struct yyltype *llocp,
			 parser_param_t *param)
{
	static const struct { const char *keyword; int value; } table[]=
		{{	"break",	BREAK},
		 {	"continue", 	CONTINUE},
		 {	"else",		ELSE},
		 {	"for",		FOR},
		 {	"clear",	FREE},
		 {	"function",	DEFINE_FUNCTION},
		 {	"if",		IF},
		 {	"include",	INCLUDE},	 
		 {	"quit",		QUIT},
		 {	"return",	RETURN},
		 {	"while",	WHILE},
		 {	NULL,		0}};
	int i=0;
	char *symbuf;
	int length=PARSER_TOKEN_LENGTH;

#define BUF		((char *)param->lex.buffer->data)
#define POS		(param->lex.position)
	
	HAPLO_ALLOC(symbuf, length+1);

	llocp->first_column=POS;
	
	do
	{
		if (i == length)
		{
			length *= 2;
			HAPLO_RE_ALLOC(symbuf, length+1);
		}
		symbuf[i++] = BUF[POS++];
	}
	while (isalnum ((int)BUF[POS]) || BUF[POS] == '_');
	llocp->last_column=POS;
	symbuf[i] = '\0';

	/* keyword? */
	for (i=0; table[i].keyword; i++)
	{
		if (strcmp(symbuf, table[i].keyword) == 0)
		{
			HAPLO_FREE(symbuf);
			return(table[i].value);
		}
	}
			
	/* Variable or constant? */
	lvalp->ref=parser_object_get(param,
				     symbuf);
	if (lvalp->ref)
	{
		HAPLO_FREE(symbuf);
		return(REFERENCE);
	}				
	
	/*  Function ? */
	lvalp->function=__haplo_func_get(symbuf);
	if (lvalp->function)
	{
		HAPLO_FREE(symbuf);
		return(FUNCTION);
	}

	/* c'est donc une chaine */
	lvalp->string=symbuf;

	return(STRING);
#undef BUF
#undef POS
}


/**
 * Handle string token
 *
 * @param lvalp
 * @param llocp
 * @param param
 * 
 * @return OBJECT
 */
static int parser_lex_string(YYSTYPE *lvalp, struct yyltype *llocp,
			     parser_param_t *param)
{
	int i=0;
	int length=PARSER_TOKEN_LENGTH;
	char *stringbuf;

#define BUF		((char *)param->lex.buffer->data)
#define POS		(param->lex.position)

	HAPLO_ALLOC(stringbuf, length+1);

	llocp->first_column=POS;
	POS++;

	while(BUF[POS] != '"')
	{
		if (BUF[POS] == '\0')
		{
			llocp->last_column=POS;
			parser_error(param, llocp, 1800);
			POS--; /* emultate the `"' */
			break;
		}
		
		if (i == length)
		{
			length *= 2;
			HAPLO_RE_ALLOC(stringbuf, length+1);
		}
		stringbuf[i++] = BUF[POS++];
	}
	stringbuf[i] = '\0';
	POS++; /* skip `"' */
	llocp->last_column=POS;
	lvalp->object=__haplo_object_from_string(stringbuf);

#undef BUF
#undef POS
	return(VALUE);
}


/**
 * Handle unary and binary operator tokens
 *
 * @param llocp
 * @param param
 *
 * @return token type
 */
static int parser_lex_single(struct yyltype *llocp, parser_param_t *param)
{
#define BUF		((char *)param->lex.buffer->data)
#define POS		(param->lex.position)

	llocp->first_column=POS;
	switch(BUF[POS])
	{
	case '>':
		if (BUF[POS+1] == '=')
		{
			POS+=2;
			llocp->last_column=POS;
			return(GE);
		}
		break;
	case '<':
		if (BUF[POS+1] == '=')
		{
			POS+=2;
			llocp->last_column=POS;
			return(LE);
		}
		break;
	case '=':
		if (BUF[POS+1] == '=')
		{
			POS+=2;
			llocp->last_column=POS;
			return(EQUAL);
		}
		break;
	case '-':
		if (BUF[POS+1] == '>')
		{
			POS+=2;
			llocp->last_column=POS;	
			return(ARROW);
		}
		break;
	}
	llocp->last_column=POS+1;
	return(BUF[POS++]);

#undef BUF
#undef POS
}


/**
 * Lexer function
 *
 * @param lvalp
 * @param llocp
 * @param param
 * 
 * @return token type
 */
static int __haplo_parser_lex(YYSTYPE *lvalp, struct yyltype *llocp,
			      parser_param_t *param)
{
	static const char single[]=":;,+-/*<>{}[]()=|&!^~";
	int token=END;

#define BUF		((char *)param->lex.buffer->data)
#define POS		(param->lex.position)
#define LEN		(param->lex.length)

	while(1)
	{
		if (parser_lex_getline(&(param->lex)))
		{ /* EOF */
			param->lex.line -= 1; /* not really read */
			llocp->last_line=param->lex.line;
			llocp->first_line=param->lex.line;
			llocp->first_column=POS;
			llocp->last_column=POS+1;
			break;
		}

		llocp->first_line=param->lex.line;
		llocp->last_line=param->lex.line;

		
		
		if (isspace((int)BUF[POS]))
		{
			POS++;
			continue;
		}
		
		if ((!isascii((int)BUF[POS])) || iscntrl((int)BUF[POS]))
		{
			llocp->first_column=POS;
			llocp->last_column=POS+1;
			if (isprint((int)BUF[POS]))
				parser_error(param, llocp, 1801,
					     BUF[POS]);
			else
				parser_error(param, llocp, 1802,
					     (unsigned char)BUF[POS]);
			POS++;
			continue;
		}
			
		/*
		 * COMMENT
		 */
		if (BUF[POS] == '#')
		{
			POS=LEN;	/* skip to the end of line */
			continue;
		}
		
		/*
		 * FLOAT
		 */
		if ((BUF[POS] == '.') || isdigit((int)BUF[POS]))
		{
			double		num;
			char *end;
			
			llocp->first_column=POS;
			num=strtod(BUF+POS, &end);
			if (end == BUF+POS)
			{
				POS++;
				llocp->last_column=POS;
				parser_error(param, llocp, 1803);
				continue;
			}

			POS=(int)(end-BUF);
			llocp->last_column=POS;
			
			lvalp->object=__haplo_object_from_double(num);
			token=VALUE;
			break;
		}
		
		/*
		 * IDENTTIFIER
		 */
		if (isalpha ((int)BUF[POS]))
		{
			token=parser_lex_id(lvalp, llocp, param);
			break;
		}
		
		
		/*
		 * STRING
		 */
		if (BUF[POS] == '"')
		{
			token=parser_lex_string(lvalp, llocp, param);
			break;
		}
	
		/*
		 * Single char
		 */
		if (strchr(single, (int)BUF[POS]))
		{
			token=parser_lex_single(llocp, param);
			break;
		}
		else
		{
			llocp->first_column=POS;
			llocp->last_column=POS+1;
			parser_error(param, llocp, 1804,
				     BUF[POS]);
			POS++;
		}
	}
#undef LEN
#undef BUF
#undef POS

	return(token);

}


/**
 * Handle redirection
 *
 * @param from
 * 
 * @return previous stdin fd
 */
static int parser_redirect_stdin(int from)
{
	int	old=STDIN_FILENO;

	if (from != STDIN_FILENO)
	{		
		old=dup(STDIN_FILENO);
		fflush(stdin); /* flush cache ! */
		if (dup2(from, STDIN_FILENO)<0)
		{
			close(old);
			haplo_error("Impossible de redirigerle flux (%s)",
				    strerror(errno));
			return(STDIN_FILENO);
		}
		close(from);
	}
	return(old);
}
	
	
/**
 * Check if file is a text file
 *
 * @param filename
 *
 * @return 0 if yes, -1 if not
 */
static const char *parser_file_check(const char *filename)
{
	int		fd;
	signed char	buffer[PARSER_HEADER_SIZE];
	ssize_t		len,
			i;

	fd=open(filename, O_RDONLY);
	if(fd < 0)
		return(strerror(errno));
		
	len=read(fd, buffer, PARSER_HEADER_SIZE);
	for(i=0; i<len; i++)
	{
		int	c=buffer[i] & 0x7f;
		
		if ((c<32) && (c != '\n') && (c != '\t'))
		{
			close(fd);
			return(_("this file isn't a text file."));
		}
	}
	
	close(fd);

	return(NULL);
}


/**
 *
 * @param filename
 * 
 * @return newly allocated buffer containing the fullpath
 */
static char * parser_file_search(const char *filename)
{
	slink_t		*p;
	struct stat	buf;

	if (stat(filename, &buf) == 0)
		return(haplo_strdup(filename));

	for(p=parser_search_path; p; p=p->next)
	{
		char		*path;

		HAPLO_ALLOC(path, strlen((char *)p->data)+strlen(filename)+2);
		strcpy(path,(char *)p->data);
		strcat(path, "/");
		strcat(path, filename);

		if (stat(path, &buf) == 0)
			return(path);
		
		HAPLO_FREE(path);
	}

	return(haplo_strdup(filename));
}


/**
 * Get the reference if current of above contexts
 *
 * @param param
 * @param name
 * 
 * @return the reference
 */
static reference_t *parser_object_get(const parser_param_t *param,
				      const char *name)
{
	slink_t		*c;	/* contexts */

	for(c=param->lex.context; c; c=c->next)
	{
		reference_t	*ref;
		ref=__haplo_object_ref_get(PRECODE(c->data)->db, name);
		if (ref)
		{
			return(ref);
		}
	}
	
	return(__haplo_object_ref_get(param->lex.db, name));
}


/**
 * handle completion
 *
 * @param beginning
 * @param
 *
 * @return proposed completion
 */
#ifdef HAVE_READLINE
char *__haplo_parser_completion(const char *beginning, int seq)
{
	static enum 
		{
			WHAT_FUNCTION,
			WHAT_OBJECT,
			WHAT_BASE
		} 			what;
	char				*s=NULL;
	
	if (seq==0)
		what=WHAT_FUNCTION;

	switch(what)
	{
	case WHAT_FUNCTION:
		s=__haplo_func_completion(beginning, seq);
		if (s)
			return(s);
		what=WHAT_OBJECT;
		seq=0;
		
	case WHAT_OBJECT:
		if (completion_slink)
		{
			haplo_info("WHAT=OBJECT");
			while (!
			       (s=__haplo_object_completion(
				       PRECODE(completion_slink->data)->db,
				       beginning, seq)))
			{
				completion_slink=completion_slink->next;
				seq=0;
				if (!completion_slink)
					break;

			}
			if (s)
			{
				haplo_warning("s=%s", s);
				return(s);
			}
		}
		what=WHAT_BASE;
		seq=0;
		
	case WHAT_BASE:
		s=__haplo_object_completion(completion_base, beginning, seq);
	}
	return(s);
	
}
#endif /* HAVE_READLINE */

