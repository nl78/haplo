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


#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef HAVE_SYS_IOCTL_H
#	include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif
#ifdef ENABLE_BACKTRACE
#	include "bt.h"
#endif
#include "error.h"
#include "memory.h"
#include "utils.h"


#ifdef HAPLO_DEBUG_MEMORY
#	include "bt.h"
#endif /* HAPLO_DEBUG_MEMORY */



/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static int		colors=HAPLO_COLORS_NONE;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

/*
 * Display Stuff.
 */
void __haplo_colors_policy(int colors);
int __haplo_colors_set(const char *color);
void __haplo_colors_reset(void);
const char *__haplo_prefix(const char *s);
void __haplo_result_display(const object_t *object);
static int get_width(void);
void haplo_centered(const char *s, ...);
void haplo__bordered(const char *s, ...);
void haplo_underlined(const char *s, ...);

/*
 * numerical operation
 */
double haplo_clamp(const haplo_float_t *in, double min,
		   double max);
int haplo_int(const haplo_float_t *in);
unsigned int haplo_uint(const haplo_float_t *in);
unsigned short haplo_ushort(const haplo_float_t *in);
unsigned long haplo_ulong(const haplo_float_t *in);

/*
 * string operation
 */
char *haplo_strdup(const char *s);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/



/*
 * Display Stuff.
 */

/**
 *
 */
void __haplo_colors_policy(int c)
{
	colors = c;
	return;
}


/**
 *
 */
int __haplo_colors_set(const char *color)
{
	int ret=1;
	
	if (colors & HAPLO_COLORS_ANSI)
	{
		if (colors & HAPLO_COLORS_FLASH)
			fputs(COLOR_FLASH, stdout);
		fputs(color, stdout);
		ret=0;
	}
	return(ret);
}


/**
 *
 */
void __haplo_colors_reset(void)
{
	if (colors)
		puts(COLOR_RESET);
	else
		fputc('\n', stdout);

	return;
}




/**
 *
 */
void __haplo_result_display(const object_t *object)
{
	if (object)
	{
		__haplo_colors_set(COLOR_RESULT);

		fputs("   ", stdout);
		__haplo_object_display(object);
	
		__haplo_colors_reset();
	}
	
	return;
}


/**
 *
 */
static int get_width(void)
{
	int width=DEFAULT_SCREEN_WIDTH;
#ifdef TIOCGWINSZ
	struct winsize win;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, (char *)&win) == 0)
		width = win.ws_col;
	else
	{
#endif /* TIOCGWINSZ*/
		char *txt;
		
		txt=getenv("COLUMNS");
 		if (txt)
			width=atoi(txt);
#ifdef  TIOCGWINSZ
	}
#endif /* TIOCGWINSZ*/
	return(width);
}


/**
 *
 */
void haplo_centered(const char *s, ...)
{
	va_list	ap;
	char	*title;
	const int width=get_width();
	int len;
	int i;
	int mid;	
	
	title=HAPLO_MALLOC(width+1);

	va_start(ap, s);
	len=vsnprintf(title, width+1, s, ap);
	va_end(ap);

	mid=(width-len)/2;
	
	for(i=0; i<mid; i++)
		putchar(' ');
	puts(title);

	putchar('\n');

	HAPLO_FREE(title);

	return;
}


/**
 *
 */
void haplo_underlined(const char *s, ...)
{
	va_list	ap;
	char	*title;
	const int width=get_width();
	int len;
	int i;
	int mid;

	title=HAPLO_MALLOC(width+1);

	va_start(ap, s);
	len=vsnprintf(title, width+1, s, ap);
	va_end(ap);

	mid=(width-len)/2;
	
	for(i=0; i<mid; i++)
		putchar(' ');
	puts(title);
	for(i=0; i<mid; i++)
		putchar(' ');
	for(i=0; i<len; i++)
		putchar('-');

	putchar('\n');
	putchar('\n');

	HAPLO_FREE(title);
     
	return;
}


/**
 *
 */
void haplo_bordered(const char *s, ...)
{
	va_list	ap;
	char	*title;
	const int width=get_width();
	int len;
	int i;
	int mid;	

	title=HAPLO_MALLOC(width+1);

	va_start(ap, s);
	len=vsnprintf(title, width+1, s, ap);
	va_end(ap);

	mid=(width-len)/2;

	for(i=0; i<mid-2; i++)
		putchar(' ');
	
	putchar('+');
	for(i=0; i<len+2; i++)
		putchar('-');
	putchar('+');
	putchar('\n');
	for(i=0; i<mid-2; i++)
		putchar(' ');
	putchar('|');
	putchar(' ');
	fputs(title, stdout);
	puts(" |");
	for(i=0; i<mid-2; i++)
		putchar(' ');
	putchar('+');
	for(i=0; i<len+2; i++)
		putchar('-');
	puts("+");

	__haplo_colors_reset();
	putchar('\n');

	HAPLO_FREE(title);

	return;
}


/**
 *
 */
double haplo_clamp(const haplo_float_t *in,
		   double min,
		   double max)
{
	double out;

	if (*in > max)
	{
		out=max;
		haplo_info(_("Overflow: `%.8g' is rounded to `%.8g'"),
			     *in, out);
	} else if (*in < min)
	{
		out=min;
		haplo_info(_("Underflow: `%.8g' is rounded to `%.8g'"),
			     *in, out);
	} else {
		out=*in;
	}
	
	return(out);
}


/**
 *
 */
int haplo_int(const haplo_float_t *in)
{
	int	out;
	
	if (*in < (double)INT_MIN)
	{
		out=INT_MIN;
		haplo_info(_("Underflow: `%.8g' is rounded to `%u'"), *in,
			   out);
	}
	else if (*in > (double)INT_MAX)
	{
		out=INT_MAX;
		haplo_info(_("Overflow: `%.8g' is rounded to `%u'"), *in, out);
	}
	else
	{
		out=(int)(*in);
		if (*in != out)
			haplo_info(_("`%.8g' is rounded to `%u'"), *in, out);
	}

	return(out);
}


/**
 *
 */
unsigned int haplo_uint(const haplo_float_t *in)
{
	unsigned int	out;

	if (*in < 0)
	{
		out=0;
		haplo_info(_("Underflow: `%e' is rounded to `%u'"), *in, out);
	}
	else if (*in > (double)UINT_MAX)
	{
		out=UINT_MAX;
		haplo_info(_("Overflow: `%e' is rounded to `%u'"), *in, out);
	}
	else
	{
		out=(unsigned int)(*in);
		if (*in != out)
			haplo_info(_("`%e' is rounded to `%u'"), *in, out);
	}

	return(out);
}


/**
 *
 */
unsigned short haplo_ushort(const haplo_float_t *in)
{
	unsigned short	out;

	if (*in < 0)
	{
		out=0;
		haplo_info(_("Underflow: `%e' is rounded to `%u'"), *in, out);
	}
	else if (*in > (double)USHRT_MAX)
	{
		out=USHRT_MAX;
		haplo_info(_("Overflow: `%e' is rounded to `%u'"), *in, out);
	}
	else
	{
		out=(unsigned short)(*in);
		if (*in != out)
			haplo_info(_("`%e' is rounded to `%u'"), *in, out);
	}

	return(out);
}


/**
 *
 */
unsigned long haplo_ulong(const haplo_float_t *in)
{
	unsigned long	out;

	if (*in < 0)
	{
		out=0;
		haplo_info(_("Underflow: `%e' is rounded to `%lu'"), *in, out);
	}
	else if (*in > (double)ULONG_MAX)
	{
		out=ULONG_MAX;
		haplo_info(_("Overflow: `%e' is rounded to `%lu'"), *in, out);
	}
	else
	{
		out=(unsigned long)(*in);
		if (*in != out)
			haplo_info(_("`%e' is rounded to `%lu'"), *in, out);
	}

	return(out);
}


/**
 *
 */
char *haplo_strdup(const char *s)
{
	char	*copy;
	copy=HAPLO_MALLOC(strlen(s)+1);
	strcpy(copy, s);
	
	return(copy);
}


