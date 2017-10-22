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

#include "extensions.h"
#include <haplo/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_READLINE_READLINE_H
#	include <readline/readline.h>
#	include <readline/history.h>
#endif
#ifdef HAVE_READLINE_H
#	include <readline.h>
#	include <history.h>
#endif
#include "haplo/init.h"
#include "haplo/module.h"

#include "bt.h"
#include "builtin.h"
#include "code.h"
#include "interruptible.h"
#include "func.h"
#include "memory.h"
#include "object.h"
#include "parser.h"
#include "module.h"
#include "slink.h"
#include "utils.h"

#define BUFFER_SZ	256


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

char *haplo_param_path(const char *dir);
void haplo_init(haplo_param_t *param);
void haplo_fini(void);
void haplo_param_default(haplo_param_t *param);
#if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_NONE
static void use(const char *s);
#endif /* HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_NONE */
static char *haplo_gets(const char *prompt);
#ifdef HAVE_READLINE
static char *haplo_readline(const char *prompt);
#endif


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/




/**
 * Compute the home direcy of haplo
 *
 * @param dir is direcroy concatened to home directory
 * @return the computed path
 */
char *haplo_param_path(const char *dir)
{
	size_t	len;
	char	*base;
	char	*path;
	
	
	base=getenv(HAPLO_HOME_VAR);
	if (!base)
		base=HAPLO_HOME_DEFAULT;

	len=strlen(base)+strlen(dir)+strlen(HAPLO_SEPARATOR)+1;
	
	HAPLO_ALLOC(path, len);
	
	strcpy(path, base);
	strcat(path, HAPLO_SEPARATOR);
	strcat(path, dir);
	
	return(path);
}


/**
 * Initialize libhaplo
 *
 * @param param is the parameters of initialization
 */
void haplo_init(haplo_param_t *param)
{
	input_t		input; /* for parser */
	char		*path;

	__haplo_interruptible_init();
	__haplo_parser_init(param);
	__haplo_slink_init();
		
	__haplo_colors_policy(param->colors & COLORS_AVAILABLE);

	input.getline=param->user_getline;
	input.freeline=param->user_freeline;
	__haplo_parser_method_set(INPUT_USER, input);

	input.getline=param->file_getline;
	input.freeline=param->file_freeline;
	__haplo_parser_method_set(INPUT_FILE, input);


	srand(time(NULL));
	
	__haplo_func_init();
	__haplo_object_init();
	__haplo_code_init();
	__haplo_builtin_init();


	path=haplo_param_path(HAPLO_MODULE_DIR);
	haplo_module_path_add(path);
	HAPLO_FREE(path);
	
	path=haplo_param_path(HAPLO_SCRIPTS_DIR);
	haplo_parser_path_add(path);
	HAPLO_FREE(path);

#ifdef HAVE_READLINE
#if 0
	using_history();
	stifle_history(512);
	rl_completion_entry_function=__haplo_parser_completion;
#endif
#endif

	return;
}


/**
 * Free allocated buffers
 */
void haplo_fini(void)
{
	__haplo_object_type_free();
	__haplo_object_fini();
	__haplo_code_fini();
	__haplo_func_fini();
	__haplo_builtin_fini();
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
	__haplo_module_unload_all();
#endif /* HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE */
	__haplo_parser_fini();
	__haplo_slink_fini();

	return;
}


/**
 * get default parameters
 *
 * @param param
 */
void haplo_param_default(haplo_param_t *param)
{
	param->colors=0;
#ifdef __linux
	param->colors=HAPLO_COLORS_ANSI | HAPLO_COLORS_FLASH;
#endif /* linux */
#if (defined __sgi) || (defined __sun)
	param->colors=HAPLO_COLORS_ANSI;
#endif /* sgi */
	param->autoload=0;
	param->banner=1;
#ifdef HAVE_READLINE
	param->user_getline=haplo_readline;
	param->user_freeline=(void (*)(char *))free;
#else
	param->user_getline=haplo_gets;
	param->user_freeline=(void (*)(char *))HAPLO_FREE_FUNC;
#endif
	param->file_getline=haplo_gets;
	param->file_freeline=(void (*)(char *))HAPLO_FREE_FUNC;
	param->init_filename="haplo-init.hpl";
	param->filename=NULL;
	
	return;
}


/**
* Local implementation of use() used when modules are not available
*
* @param s is the name of module which should be loaded
*/
#if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_NONE
static void use(const char *s)
{
	(void)s;	/* avoid gcc to complain... */
	puts(_("La fonction use() n'est pas disponible dans cette "
	       "configuration."));
	return;
}
#endif /* HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_NONE */


/**
 * local implementation of gets(2)
 *
 * @param prompt
 * @return a newly allocated buffer 
 */
static char *haplo_gets(const char *prompt)
{
	int	length=BUFFER_SZ, i;
	char	*buf;

	fputs(prompt, stdout); fflush(stdout);

	HAPLO_ALLOC(buf, BUFFER_SZ);
	
	
	i=0;
	do
	{
		int	next;
		
		if (i == length)
		{
			length *= 2;
			HAPLO_RE_ALLOC(buf, length+1);
		}
		next=getchar();
		if (next == EOF)
		{
			HAPLO_FREE(buf);
			return(NULL);
		}
		buf[i++] = (char)next;
	} while(buf[i-1] != '\n');
	buf[i-1]='\0';
	
	return(buf);
}


/**
 * nicer gets() implementation
 *
 * @param prompt
 * @return a newly allocated buffer
 */
#ifdef HAVE_READLINE
static char *haplo_readline(const char *prompt)
{
	char	*line;
	
	line=readline(prompt);
	if (line && (strlen(line)>1))
		add_history(line);
	
	return(line);
}
#endif

