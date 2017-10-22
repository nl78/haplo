/*
 * Copyright (C) 2005 Nicolas LAURENT
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

#include <haplo/config.h>

#include <stdarg.h>
#include <stdio.h>


#include "bt.h"
#include "error.h"
#include "utils.h"

/*
 *
 * XXXX
 * ||||
 * |||`-
 * |||
 * ||`-- level: 
 * ||
 * |`--- Kind of error : - 0: General
 * |                     - 1: input  
 * |                     - 2: stmt
 * |                     - 3: exp
 * |                     - 4: object/list
 * |                     - 5: lex
 * |
 * `---- Severity level: - 0: INFO
 *                       - 1: Parser Warning
 *                       - 2: Parser Error
 *                       - 3: Runtime Error
 */

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static error_message_t error_table[]={
	{  500, "" /* General info from module */                            },
	{  501, N_("Execution interrupted due to user request."             )},	
	/* WARNING */
	{ 1000, "" /* General warning from module */                         },
	{ 1020, N_("Missing `;' at the end of input."                       )},
	{ 1230, N_("File `%s' should define function `%s'"                  )},
	{ 1061, N_("`break' statement not within loop."                     )},
	{ 1062, N_("`continue' statement not within loop."                  )},
	{ 1063, N_("`return' statement not within function definition."     )},
	{ 1064, N_("`return' statement not within function definition."     )},
	{ 1380, N_("Variable `%s' undefined. Making a string."              )},
	{ 1440, N_("Shadowing function `%s'."                               )},
	{ 1800, N_("Missing closing `\"'."                                  )},
	{ 1801, N_("Char `%c' is ignored."                                  )},
	{ 1802, N_("Char `0x%x' is ignored."                                )},
	{ 1803, N_("Unexpected symbol"                                      )},
	{ 1804, N_("Symbol `%c' is simply ignored."                         )},

	/* ERROR */
	{ 2000, N_("Unrecoverable parse error (%s). If you think this\n"
		   "  should not have happened, please submit a bug report "
		   "to <bugs@haplo.info>."                                  )},
	{ 2010, N_("Syntax error before this point."                        )},
	{ 2011, N_("Syntax error at end of input."                          )},
	{ 2012, N_("Syntax error before this point."                        )},
	{ 2021, N_("Incorrect rvalue in assignment."                        )},
	{ 2030, N_("Cannot clear this expression."                          )},
	{ 2031, N_("This keyword is not allowed infunction"                 )},
	{ 2060, N_("`include' keyword accepts only one argument"            )},
	{ 2100, N_("Missing `;' after this point."
		   "Last instruction ignored."                              )},
	{ 2220, N_("Incorrect lvalue in assignment."                        )},
	{ 2221, N_("Incorrect lvalue in assignment."                        )},
	{ 2230, N_("Shadowing function `%s'."                               )},
	{ 2260, N_("Missing `;' after this point."                          )},
	{ 2261, N_("Empty expression are not allowed."                      )},
	{ 2262, N_("Missing `;' after this point."                          )},
	{ 2301, N_("Undefined function `%s'."                               )},
	{ 2320, N_("Expecting expression after `,'. Insert `0.0'"           )},
	{ 2370, N_("Hashtable `%s' is not defined."                         )},
	{ 2430, N_("Expecting parameter name."                              )},
	{ 2441, N_("Keyword are not allowed as arameters."                  )},

	/* RUNTIME ERROR */
	{ 3000, "" /* General error from module */                           },
	{ 3050, N_("Object of type <%s> used as <boolean> "
		   "in if statement."                                       )},
	{ 3060, N_("Object of type <%s> used as <string> "
		   "in `include()'."                                        )},
	{ 3061, N_("Too many neested `include()'."                          )},
	{ 3062, N_("Cannot include file `%s': %s"                           )},
	{ 3070, N_("Using object of type `%s' as "
		   "loop condition"                                         )},
	{ 3220, N_("Dangling reference detected."                           )},
	{ 3221, N_("Cannot assign undefined value  to `%s'."                )},
	{ 3222, N_("Assignment impossible: try to apply `->' on "
		   " objet of type <%s>"                                    )},
	{ 3300, N_("Execution of this code needs %u parameter%s."           )},
	{ 3301, N_("Try to use object of type <%s> as a function."          )},
	{ 3302, N_("No instance of overloaded function `%s' "
		   "matches the argument list."                             )},
	{ 3303, N_("Execution of `%s' failed"                               )},
        { 3304, N_("Indice of vector should be single number"               )},
	{ 3305, N_("Parameter #%lu of function `%s' should be named."       )},
	{ 3306, N_("Too many recursive calls. Virtual memory exhausted."    )},
	
	{ 3310, N_("Evaluation of argument #%hu does not "
		   "produce any valid result."                              )},
	{ 3370, N_("Applying `->' on object of type `%s'"                   )},
	{ 3380, N_("Variable `%s' is undefined."                            )},
	{ 3500, N_("Member `%s' does not exist."                            )},
	{ 3501, N_("Try to use an objet of type <%s> as member name."       )},
	{ 3502, N_("Utilisation d'un objet de type `%s' nom "
		   "membre d'une table"                                     )},
	{ 3600, N_("Index out of bounds."                                   )},
	{ 3601, N_("Try to use an objet of type <%s> as indice of a vector.")},
	/* End of errors */
        { 4000, "" /* Fatal error */                                         },
	{    0, N_("Undefined error"                                        )}
};

static error_prefix_t	error_prefix={ NULL, NULL, 0, 0, NULL };


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static error_message_t * error_format(unsigned int n);
static void error_type(int n, const char **color, const char **type);
const char * __haplo_error(const char *filename, unsigned int line,
			   const char *function, int n, va_list ap);

void haplo_error_off(int n);
static void prefix(int type, ...);
void haplo_fatal(const char *s, ...);
void haplo_warning(const char *s, ...);
void haplo_error(const char *s, ...);
void haplo_info(const char *s, ...);
void haplo_debug(const char *s, ...);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
static error_message_t * error_format(unsigned int n)
{
	error_message_t *error;

	for(error=error_table; error->n; error += 1)
		if (error->n == n)
			break;

	return(error);
}


/**
 *
 */
static void error_type(int n, const char **color, const char **type)
{
	if (n<1000)
	{
		*type="INFO";
		*color=COLOR_INFO;
	}
	else if (n<2000)
	{
		*type="WARNING";
		*color=COLOR_WARNING;
	}
	else if (n<4000)
	{
		*type="ERROR";
		*color=COLOR_ERROR;
	}
	else
	{
		*type="FATAL";
		*color=COLOR_FATAL;
	}


	return;
}


/**
 *
 */
const char * __haplo_error(const char *filename, unsigned int line,
			   const char *function, int n, va_list ap)
{
	const error_message_t *error;
	const char *color=NULL;
	
	error=error_format(n);

	if (error->format)
	{
		const char *type;

		error_type(n, &color, &type);
		
		__haplo_colors_set(color);
		
		if (filename)
		{
			printf("\n%s(%d) File = %s, Line = %d",
			       type, n, filename, line);

			if (function)
				printf(", Function = %s\n", function);
			else	
				putchar('\n');
		}
				
		if (error->format[0])
		{
			fputs("  ", stdout);
			vprintf(error->format, ap);
	
			__haplo_colors_reset();
		}		
	}

	return(color);
}


/**
 *
 */
void haplo_error_off(int n)
{
	error_message_t *error;

	error = error_format(n);
	
	if (error->n)
		error->format = NULL;
	
	return;
}



/**
 *
 */
error_prefix_t __haplo_error_prefix_set(const error_prefix_t *error)
{
	error_prefix_t prev=error_prefix;

	if (error_prefix.filename && error_prefix.thrown)
		putchar('\n');
	
	error_prefix        = *error;
	error_prefix.thrown = 0;
	error_prefix.color  = NULL;

	return(prev);
}

/**
 *
 */
error_prefix_t __haplo_error_prefix_get(void)
{
	return error_prefix;
}



/**
 *
 */
static void prefix(int type, ...)
{
	if (error_prefix.thrown != type)
	{
		va_list	args;

		va_start(args, type);
		if (error_prefix.thrown)
			putchar('\n');
		
		error_prefix.color=__haplo_error(error_prefix.filename,
						 error_prefix.line,
						 error_prefix.function,
						 type, args);
		va_end(args);
		error_prefix.thrown = type;
	}
	
	if (error_prefix.color)
		__haplo_colors_set(error_prefix.color);
	
	if (error_prefix.filename)
		fputs("  ", stdout);
	return;
}


/**
 *
 */
void haplo_fatal(const char *s, ...)
{
	va_list	ap;
	
	prefix(4000);

	fputs("Internal failure detected.\n", stdout);
	fputs("  Please submit a bug report to <bugs@haplo.info>.\n", stdout);
	fputs("  Error message: ", stdout);


	va_start(ap, s);
	vprintf(s, ap);
	va_end(ap);
	__haplo_colors_reset();

#ifdef ENABLE_BACKTRACE
	__haplo_bt();
#endif /* BT */

	puts(_("End."));
	exit(EXIT_FAILURE);

	/* never reached */
	return;
}


/**
 *
 */
void haplo_error(const char *s, ...)
{
	va_list	ap;

	prefix(3000);

	va_start(ap, s);
     	vprintf(s, ap);
	va_end(ap);

	__haplo_colors_reset();
	
	return;
}


/**
 *
 */
void haplo_warning(const char *s, ...)
{
	va_list	ap;

	prefix(1000);
	
	va_start(ap, s);
	vprintf(s, ap);     
	va_end(ap);

	__haplo_colors_reset();
	
	return;
}


/**
 *
 */
void haplo_info(const char *s, ...)
{
	va_list	ap;
	
	prefix(500);
	va_start(ap, s);
	vprintf(s, ap);     
	va_end(ap);

	__haplo_colors_reset();

	return;
}


/**
 *
 */
void haplo_debug(const char *s, ...)
{
	va_list	ap;	

	if (error_prefix.function)
	{
		__haplo_colors_set(COLOR_CYAN);
		printf("%s: ", error_prefix.function);
	}

	__haplo_colors_set(COLOR_DEBUG);
	va_start(ap, s);
	vprintf(s, ap);
	va_end(ap);

	__haplo_colors_reset();
	
	return;
}

