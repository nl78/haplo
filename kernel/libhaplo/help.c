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

#ifdef HAVE_DIRECT_H
#	include <direct.h>
#endif
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#include "error.h"
#include "func.h"
#include "help.h"
#include "memory.h"
#include "utils.h"

#ifndef PATH_MAX
#	define PATH_MAX	1024
#endif /* PATH_MAX */
#define HTML_BUFFER_SZ 256



struct html
{
	FILE *out;
	const object_type_t *type;
};

enum replace_type
{
	REPLACE_FUNC,
	REPLACE_TYPE,
	REPLACE_NONE
};


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static void dump(FILE *out, const char *type, const char *sub,
		 const char *name, int n);
static void print_func_details(FILE *out, const func_t *f);
static void print_func_name(FILE *out, const func_t *f);
static void print_type_name(FILE *out, const object_type_t *f);
static void print_func_formal(FILE *out, int n);
static void print_func_arg(FILE *out, const func_t *f, int n);
static void print_func_args(FILE *out, const func_t *f);
static void print_func_arg_descr(FILE *out, const func_t *f, int n);
static void print_func_args_descr(FILE *out, const func_t *f);
static void print_func_return(FILE *out, const func_t *f);
static void print_func_prototype(FILE *out, const func_t *f);
static void print_func_prototypes(FILE *out, const func_t *f);
static void print_func_descr(FILE *out, const func_t *f);
static void print_type_descr(FILE *out, const object_type_t *t);
static void print_func_validation(FILE *out, const func_t *f);
static void print_type_validation(FILE *out, const object_type_t *t);
static void print_func_module(FILE *out, const func_t *f);
static void print_type_module(FILE *out, const object_type_t *t);
static void print_func_symbol(FILE *out, const func_t *f);
static void print_haplo_version(FILE *out);
static char *cat_func_replace(FILE *out, char *begin, const func_t *f);
static void cat_func(FILE *out, const char *file, const func_t *f);
static void htmlify(char *s);
static FILE *html_func_open(const char *name, const char *dir);
static void html_func(const func_t *l, const char *dir);
static void html_func_index_f(func_t *l, FILE *out);
static FILE *html_type_open(const char *name, const char *dir);
static void html_type(const object_type_t *l, const char *dir);
static void html_type_index(const char *dir);
void __haplo_html_html(const char *base, const char *out);





/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 *
 */
static void dump(FILE *out, const char *type, const char *sub,
		 const char *name, int n)
{
	FILE *fp;
	char dir[PATH_MAX];
	char path[PATH_MAX];
	
	strcpy(dir, name);
	htmlify(dir);
	
	if (n)
		snprintf(path, PATH_MAX, "%s/%s/%s%d", type, dir, sub, n);
	else
		snprintf(path, PATH_MAX, "%s/%s/%s", type, dir, sub);

	fp=fopen(path, "rt");
	if (fp)
	{
		char buffer[HTML_BUFFER_SZ];
		
		while(fgets(buffer, sizeof(buffer), fp))
			fputs(buffer, out);

		fclose(fp);
	}
	else
	{
		if (n)
			haplo_warning("Cannot document '%s' (%s/%s%d).",
				      name, type, sub, n);
		else
			haplo_warning("Cannot document '%s' (%s/%s).",
				      name, type, sub);

		fputs("<i>(Documentation en cours)</i>", out);
	}

	return;
}



/**
 *
 */
static void print_none(FILE *out)
{
	fputs("<i>(none)</i>", out);
	return;
}


/**
 *
 */
static void print_func_details(FILE *out, const func_t *f)
{
	const char *name=f->name;
	
	cat_func(out, "templates/function_detail.html", f);
	for(f=f->next; f; f=f->next)
		if (strcmp(f->name, name) == 0)
			cat_func(out, "templates/function_detail.html", f);
		else
			break;

	return;
}


/**
 *
 */
static void print_func_name(FILE *out, const func_t *f)
{
	fputs("<span class=\"function\">", out);
	fputs(f->name, out);
	fputs("</span>", out);

	return;
}


/**
 *
 */
static void print_type_name(FILE *out, const object_type_t *t)
{
	fputs("<span class=\"type\">", out);
	fputs(t->name, out);
	fputs("</span>", out);
	
	return;
}


/**
 *
 */
static void print_func_formal(FILE *out, int n)
{
	fprintf(out, "<span class=\"formal\">arg%d</span>", n+1);
}


/**
 *
 */
static void print_func_arg(FILE *out, const func_t *f, int n)
{
	
	fputs("<span class=\"argument\">", out);

	/* flags */
	if (f->args_flags[n] & FUNC_ARG_IN)
		fputs("<span class=\"keyword\">const </span>", out);
	if (f->args_flags[n] & FUNC_ARG_PROTECT)
		fputs("<span class=\"keyword\">protect </span>", out);

	/* type */
	fputs("<a href=\"type_", out);
	fputs(f->args[n+1]->name, out);
	fputs(".html\" class=\"type\">", out);
	fputs(f->args[n+1]->name, out);
	fputs("</a> ", out);
	print_func_formal(out, n);
	fputs("</span>", out);

	return;
}


/**
 *
 */
static void print_func_args(FILE *out, const func_t *f)
{
	fputc('(', out);
	
	if (f->n)
	{
		unsigned int i;

		print_func_arg(out,f, 0);
		
		for(i=1; i<f->n; i++)
		{
			fputs(", ", out);
			print_func_arg(out, f, i);
		}
	}
	
	fputc(')', out);
	
	return;
}


static void print_func_arg_descr(FILE *out, const func_t *f, int n)
{
	fputs("<td class=\"field\">", out);
	print_func_formal(out, n);
	fputs("</td><td class=\"value\">", out);
	
	dump(out, "functions", "arg", f->name, n+1);

	return;
}


/**
 *
 */
static void print_func_args_descr(FILE *out, const func_t *f)
{

	if (f->n > 0)
	{
		unsigned int i;

		print_func_arg_descr(out, f, 0);
		
		for(i=1; i<f->n; i++)
		{
			fputs("</tr>", out);
			print_func_arg_descr(out, f, i);
			fputs("<tr>", out);
		}
	}
	
	return;
}


/**
 *
 */
static void print_func_return(FILE *out, const func_t *f)
{
	if (f->args[0])
	{
		fputs("<a href=\"type_", out);
		fputs(f->args[0]->name, out);
		fputs(".html\" class=\"type\">", out);
		fputs(f->args[0]->name, out);
		fputs("</a>", out);
	}
	
	return;
}


/**
 *
 */
static void print_func_prototype(FILE *out, const func_t *f)
{
	fputs("<span class=\"prototype\">", out);
	
	print_func_return(out, f);
	fputc(' ', out);
	print_func_name(out, f);
	print_func_args(out, f);
		
	fputs("</span>", out);

	return;
}


/**
 *
 */
static void print_func_prototypes(FILE *out, const func_t *f)
{
	const char *name;
	
	name=f->name;
	
	print_func_prototype(out, f);
	
	for(f=f->next; f; f=f->next)
		if (strcmp(f->name, name) == 0)
		{
			fputs("<li>", out);
			print_func_prototype(out, f);
			fputs("</li>", out);
		} else
			break;

	return;
}


/**
 *
 */
static void print_func_descr(FILE *out, const func_t *f)
{
	dump(out, "functions", "descr", f->name, 0);
	
	return;
}


/**
 *
 */
static void print_type_descr(FILE *out, const object_type_t *t)
{
	dump(out, "types", "descr", t->name, 0);
	
	return;
}


/**
 *
 */
static void print_func_validation(FILE *out, const func_t *f)
{
	dump(out, "functions", "validation", f->name, 0);
	
	return;
}


/**
 *
 */
static void print_type_validation(FILE *out, const object_type_t *t)
{
	dump(out, "types", "validation", t->name, 0);
	
	return;
}


/**
 *
 */
static void print_func_module(FILE *out, const func_t *f)
{
	if (f->module)
	{
		fputs("<span class=\"module\">", out);
		fputs(f->module->name, out);
		fputs("</span>", out);
	} else
		print_none(out);
	
	return;
}


/**
 *
 */
static void print_type_module(FILE *out, const object_type_t *t)
{
	if (t->module)
	{
		fputs("<span class=\"module\">", out);
		fputs(t->module->name, out);
		fputs("</span>", out);
	} else
		print_none(out);
	
	return;
}


/**
 *
 */
static void print_func_symbol(FILE *out, const func_t *f)
{
	fputs("<span class=\"symbol\">", out);
	fputs(f->symbol, out);
	fputs("</span>", out);

	return;
}


/**
 *
 */
static void print_type_copy(FILE *out, const object_type_t *t)
{
	if (t->copy)
	{
		fputs("<span class=\"symbol\">", out);
		fputs(t->copy_symbol, out);
		fputs("</span>", out);
	} else
		print_none(out);
	

	return;
}


/**
 *
 */
static void print_type_display(FILE *out, const object_type_t *t)
{
	if (t->display)
	{
		fputs("<span class=\"symbol\">", out);
		fputs(t->display_symbol, out);
		fputs("</span>", out);
	} else
		print_none(out);
	

	return;
}


/**
 *
 */
static void print_type_free(FILE *out, const object_type_t *t)
{
	if (t->free)
	{
		fputs("<span class=\"symbol\">", out);
		fputs(t->free_symbol, out);
		fputs("</span>", out);
	} else
		print_none(out);

	return;
}


/**
 *
 */
static void html_func_index_f(func_t *l, FILE *out)
{
	char *s=haplo_strdup(l->name);
	
	htmlify(s);
	
	fputs("<li><a href=\"function_", out);
	fputs(s, out);
	fputs(".html\">", out);
	print_func_name(out, l);
	fputs("</a></li>", out);
	
	HAPLO_FREE(s);
	
	return;
}


/**
 *
 */
static void html_type_index_f(object_type_t *t, FILE *out)
{
	char *s=haplo_strdup(t->name);
	
	htmlify(s);
	
	fputs("<li><a href=\"type_", out);
	fputs(s, out);
	fputs(".html\">", out);
	print_type_name(out, t);
	fputs("</a></li>", out);
	
	HAPLO_FREE(s);
	
	return;
}




/**
 *
 */
static void print_func_index(FILE *out)
{
	__haplo_func_loop((func_loop_t)html_func_index_f, (void *)out);
	
	return;
}


/**
 *
 */
static void print_type_index(FILE *out)
{
	__haplo_object_type_loop((object_type_loop_t)html_type_index_f, (void *)out);
	
	return;
}


/**
 *
 */
static void html_type_constructors_f(func_t *f, struct html *h)
{
	if (f->args[0] == h->type)
		html_func_index_f(f, h->out);
	
	return;
}


/**
 *
 */
static void print_type_constructors(FILE *out, const object_type_t *t)
{
	struct html h;
	
	h.out=out;
	h.type=t;
	
	__haplo_func_loop((func_loop_t)html_type_constructors_f, (void *)&h);
	
	return;
}


/**
 *
 */
static void print_haplo_version(FILE *out)
{
	fprintf(out, "haplo-%d.%d", HAPLO_MAJOR, HAPLO_MINOR);
	return;
	
}


/**
 *
 */
static char *cat_func_replace(FILE *out, char *begin, const func_t *f)
{
	char *end;
	size_t l;
	
	end=strpbrk(begin, " <\t\r\n");
	
	if (end)
		l=(size_t)(end-begin);
	else
		l=strlen(begin);
	

#define IF(x)	if ((l==strlen(x)) && (strncmp(begin, x, l) == 0))
#define ELSE	else

	IF ("_name_") {
		print_func_name(out, f);
 	} ELSE IF ("_description_") {
		print_func_descr(out, f);
 	} ELSE IF ("_details_") {
		print_func_details(out, f);
	} ELSE IF ("_index_") {
		print_func_index(out);
	} ELSE IF ("_prototype_") {
		print_func_prototype(out, f);
	} ELSE IF ("_prototypes_") {
		print_func_prototypes(out, f);
	} ELSE IF ("_arguments_") {
		print_func_args_descr(out, f);
	} ELSE IF ("_symbol_") {
		print_func_symbol(out, f);
	} ELSE IF ("_module_") {
		print_func_module(out, f);
	} ELSE IF ("_validation_") {		
		print_func_validation(out, f);
	} ELSE IF ("_version_") {
		print_haplo_version(out);
	} ELSE {
		char c='\0';

		if (end)
		{
			c=*end;
			*end = '\0';
		}
		
		fputs(begin, out);
		if (end)
			*end = c;
	}
	
#undef IF
#undef ELSE

	return(end);
}


/**
 *
 */
static char *cat_type_replace(FILE *out, char *begin, const object_type_t *t)
{
	char *end;
	size_t l;
	
	end=strpbrk(begin, " <\t\r\n");
	
	if (end)
		l=(size_t)(end-begin);
	else
		l=strlen(begin);
	

#define IF(x)	if ((l==strlen(x)) && (strncmp(begin, x, l) == 0))
#define ELSE	else

	IF ("_version_") {
		print_haplo_version(out);
	} ELSE IF ("_constructors_") {
		print_type_constructors(out, t);
	} ELSE IF ("_copy_") {
		print_type_copy(out, t);
	} ELSE IF ("_display_") {
		print_type_display(out, t);
	} ELSE IF ("_description_") {
		print_type_descr(out, t);
	} ELSE IF ("_index_") {
		print_type_index(out);
	} ELSE IF ("_free_") {
		print_type_free(out, t);
	} ELSE IF ("_module_") {
		print_type_module(out, t);
	} ELSE IF ("_name_") {
		print_type_name(out, t);
	} ELSE IF ("_validation_") {		
		print_type_validation(out, t);
	} ELSE {
		char c='\0';

		if (end)
		{
			c=*end;
			*end = '\0';
		}
		
		fputs(begin, out);
		if (end)
			*end = c;
	}
	
#undef IF
#undef ELSE

	return(end);
}


/**
 *
 */
static void cat(FILE *out, const char *file, enum replace_type type, 
		const void *f)
{
	FILE *fp;
	
	fp=fopen(file, "rt");
	
	if (fp)
	{
		char buffer[HTML_BUFFER_SZ];
		
		while(fgets(buffer, sizeof(buffer), fp))
		{
			char *begin;
			char *off=buffer;
			
			while(off && (begin=strchr(off, '_')))
			{
				*begin='\0';
				fputs(off, out);
				*begin='_';
				
				switch(type)
				{
				case REPLACE_FUNC:
					off=cat_func_replace(out, begin, f);
					break;
				case REPLACE_TYPE:
					off=cat_type_replace(out, begin, f);
					break;
				case REPLACE_NONE:
					break;
				}
			}
			if (off)
				fputs(off, out);
		}
		
		fclose(fp);

	} else
		haplo_error("Cannot open `%s'", file);
	
	return;
}


static void cat_func(FILE *out, const char *file, const func_t *f)
{
	cat(out, file, REPLACE_FUNC, f);

	return;
}


static void cat_type(FILE *out, const char *file, const object_type_t *t)
{
	cat(out, file, REPLACE_TYPE, t);

	return;
}


/**
 *
 */
static void htmlify(char *s)
{
	size_t i;

	const char sub[]="!&*+-/<>=^|~";
	
	for(i=0; i<strlen(s); i++)
	{
		size_t j;
		
		for(j=0; j<sizeof(sub); j++)	
			if (s[i] == sub[j])
				s[i] = 'A' + j;
	}
	
	return;
}


/**
 *
 */
static FILE *html_func_open(const char *name, const char *dir)
{
	FILE *fp;
	char *filename;
	char *f;
	
	f=haplo_strdup(name);
	htmlify(f);
	
	HAPLO_ALLOC(filename, strlen(name)+strlen(dir)+
		    strlen("/function_.html")+1);
	
	strcpy(filename, dir);
	strcat(filename, "/function_");
	strcat(filename, f);
	strcat(filename, ".html");

	fp=fopen(filename, "wt");
	if (!fp)
		haplo_error("Cannot open `%s': %s", filename, strerror(errno));
	
	HAPLO_FREE(filename);
	HAPLO_FREE(f);
	
	return(fp);
}


/**
 *
 */
static void html_func(const func_t *l, const char *dir)
{
	FILE *out;
	
	out=html_func_open(l->name, dir);
	
	if (out)
	{
		cat_func(out, "templates/function.html", l);
		fclose(out);
	}
	
	return;
	
}


/**
 *
 */
static void html_func_index(const char *dir)
{
	FILE *out;
	
	out=html_func_open("_index", dir);
	
	if (out)
	{
		cat_func(out, "templates/function_index.html", NULL);
		fclose(out);
	}
	
	return;
}


/**
 *
 */
static FILE *html_type_open(const char *name, const char *dir)
{
	FILE *fp;
	char *filename;
	
	HAPLO_ALLOC(filename, strlen(name)+strlen(dir)+
		    strlen("/type_.html")+1);
	
	strcpy(filename, dir);
	strcat(filename, "/type_");
	strcat(filename, name);
	strcat(filename, ".html");

	fp=fopen(filename, "wt");
	if (!fp)
		haplo_error("Cannot open `%s': %s", filename, strerror(errno));
	
	HAPLO_FREE(filename);
	
	return(fp);
}




/**
 *
 */
static void html_type(const object_type_t *l, const char *dir)
{
	FILE *out;
	
	out=html_type_open(l->name, dir);
	
	if (out)
	{
		cat_type(out, "templates/type.html", l);
		fclose(out);
	}
	
	return;
	
}


/**
 *
 */
static void html_type_index(const char *dir)
{
	FILE *out;
	
	out=html_type_open("_index", dir);
	
	if (out)
	{
		cat_type(out, "templates/type_index.html", NULL);
		fclose(out);
	}
	
	return;
}


/**
 *
 */
void __haplo_html(const char *base, const char *out)
{
	char here[PATH_MAX];
	
	getcwd(here, PATH_MAX);
	
	if (chdir(base) < 0)
	{
		haplo_error("Cannot open template directory '%s': %s",
			    base, strerror(errno));
	}
	else
	{
		char path[PATH_MAX];
		
		if (out[0] != '/')
			snprintf(path, PATH_MAX, "%s/%s", here, out);
		else
			strncpy(path, out, PATH_MAX);
		
		__haplo_func_loop((func_loop_t)html_func, path);
		html_func_index(path);

		__haplo_object_type_loop((object_type_loop_t)html_type, path);
		html_type_index(path);

		chdir(here);
	}
	
	return;
}


/**
 *
 */
static int help_cat(const char *filename)
{
	int status=-1;
	FILE *fp;
	
	fp=fopen(filename, "rt");
	
	if (fp)
	{
		char buffer[HTML_BUFFER_SZ];
		
		haplo_bordered("%s", filename);

		while(fgets(buffer, sizeof(buffer), fp))
			fputs(buffer, stdout);

		fclose(fp);
		fputc('\n', stdout);
		status=0;
	}

	return(status);
}


/**
 *
 */
static void help_func(const func_t *func)
{
	const char *title;
	
	if (func->flags & FUNC_FLAG_OPERATOR_BINARY)
		title = _("Binary operator `%s'");
	else if (func->flags & FUNC_FLAG_OPERATOR_UNARY)
		title = _("Unary operator `%s'");
	else
		title = _("Function `%s'");

	haplo_bordered(title, func->name);
	
	printf("SYNOPSIS\n\n");
	__haplo_func_prototypes(func);

	printf("\n\nDESCRIPTION\n\n");

	printf("\n\n");

	return;
}


/**
 * Cat first comment block of a file
 */
static int help_code(const char *filename)
{
	int success=0;
	FILE *fp;

	fp=fopen(filename, "rt");
	if (fp)
	{
		int incomment=0;
		char buffer[HTML_BUFFER_SZ];

		while(fgets(buffer, sizeof(buffer), fp))
		{
			if ((incomment == 1) && (buffer[0] != '#'))
				break;

			if ((!incomment) && (buffer[0] == '#'))
				incomment = 1;


			if (incomment)
				fputs(buffer+1, stdout);

		}

		if (incomment)
			success=1;

		fclose(fp);
	}

	return(success);
}


/**
 *
 */
void __haplo_help_code(const code_t *code)
{
	int found=0;

	if (code->entry)
	{
		const char *filename=code->entry->filename;

		haplo_bordered("Interpreted function");

		printf("SYNOPSIS\n\n");
		printf("\tDefined in          : %s\n", filename);
		printf("\tNumber of arguments : %u\n", code->args);
		printf("\tNumber of operations: %u\n", code->ops);
		printf("\tNumber of local var.: %lu\n", code->n);
		printf("\n\nDESCRIPTION\n\n");

		if (filename && help_code(filename))
			found=1;
	}

	if (!found)
		haplo_error(_("There's no help message."));

	
	return;
}


/**
 *
 */
void __haplo_help(const char *symbol)
{
	func_t *func;
		
	func=__haplo_func_get(symbol);
	if (func)
		help_func(func);
	else if (help_cat(symbol))
		haplo_error(_("There's no help message associated with `%s'."),
			    symbol);

	return;
}

/**
 *
 */
void __haplo_help_banner(void)
{
	help_cat("help");
	__haplo_func_list_display();
	return;
}
