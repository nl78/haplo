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

#ifndef __HAPLO_PARSER_H__
#define __HAPLO_PARSER_H__

#include <haplo/parser.h>

#include "code.h"
#include "slink.h"


/*-----------------------------------------------------------------------------
                            L E X _ P A R A M _ T 
-----------------------------------------------------------------------------*/

typedef struct lex_param_s
{
	slink_t			*context; /* precode_t * */
	reference_t		**db;     /* base */
	/*
	 * Parser input
	 */
	int			input;
	slink_t			*buffer;
	unsigned int		length;
	unsigned int		position;
	unsigned int		flags;
	int			line;
	int			history_line;
	const char		*filename;
} lex_param_t;


/*-----------------------------------------------------------------------------
                          P A R S E _ P A R A M _ T 
-----------------------------------------------------------------------------*/

typedef struct parser_param_s
{
	slink_t			*precode;	/* precode_t */
	lex_param_t		lex;
	int			prev_stdin_fileno;
	int			autoload;
	int			depth;
	int			in_loop;
	int			in_function;
} parser_param_t;

#define PARSER_DEPTH_MAX	16

/*-----------------------------------------------------------------------------
                                I N P U T _ T 
-----------------------------------------------------------------------------*/

typedef struct input_s
{
	char *			(*getline)(const char *);
	void			(*freeline)(char *);
} input_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define INPUT_USER	0
#define INPUT_FILE	1
#define INPUT_NUMBER	(INPUT_FILE+1)

#define PARSER_TOKEN_LENGTH	40
#define PARSER_HEADER_SIZE	256

#define PARSER_LEX_ERROR	1
#define PARSER_LEX_ERROR_MSG	2
#define PARSER_LEX_VERBOSE	4
#define PARSER_LEX_FREE		8


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void __haplo_parser_fini(void);
extern void __haplo_parser_method_set(int method, input_t input);
extern const char * __haplo_parser_load(const char *filename,
					const parser_param_t *param);
extern void __haplo_parser_init(haplo_param_t *haplo_param);
#ifdef HAVE_READLINE
extern char *__haplo_parser_completion(const char *beginning, int seq);
#endif


#endif /* __HAPLO_PARSER_H__ */
