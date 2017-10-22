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

#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif
#include <haplo/config.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "haplo.h"

#include "hgetopt.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static char *optname(const char *opt, int len);
int hgetopt(haplo_param_t *param, const option_t *options,
	    int argc, char *const argv[], int passas);
int usage(const option_t *options);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/

/**
 * Return truncated copy of opt
 *
 * @param opt
 * @param len
 * @return copy of opt truncated at len char
 */
static char *optname(const char *opt, int len)
{
	char	*name;
	
	name=haplo_strdup(opt);
	name[len]='\0';
	
	return(name);	
}


/**
 * Parse options
 *
 * @param param 
 * @param options contains understoop options
 * @param argc number of command line arguments
 * @param argv command line argument
 * @param pass is 1 or 2
 */
int hgetopt(haplo_param_t *param, const option_t *options,
	    int argc, char *const argv[], int pass)
{
	int	i=1;
	int	status=0;
	
	while(i<argc)
	{
		int	j=0;
		char	*optarg=NULL;
		size_t	len;
		int	ok=-1;
		int	incr=1;
		int	optstatus=0;

		if (argv[i][0] != '-')
			break;
		if (strcmp(argv[i], "--")==0)
		{
			i++;
			break;	
		}

		len=strlen(argv[i]);
		
		if (isupper((int)argv[i][1]))
		{
			if (len>2)
			{
				len=2;
				optarg=argv[i]+len;
			}
		} else {
			size_t l;
			
			for(l=2; l<len-1; l++)
			{
				if (argv[i][l]=='=')
				{
					len=l;
					optarg=argv[i]+len+1;
					break;
				}
			}
		}

		while(options[j].name)
		{
			if ((strlen(options[j].name) == len) &&
			    (strncmp(argv[i], options[j].name, len) == 0))
			{
				ok=j;
				break;
			}
			j++;
		}
		if (ok<0)
		{

			fprintf(stderr, _("Option `%s' is unknown.\n"),
				optname(argv[i], len));
			status=-1;
			i++;
			continue;
			
		}

		if ((options[ok].type==OPTARG) || (options[ok].type==PARAM))
		{
			if (optarg == NULL)
			{
				if ( (i+1 < argc) && (argv[i+1][0] != '-'))
				{
					optarg=argv[i+1];
					incr=2;
				}
				else
				{
					fprintf(stderr,
						_("Option `%s' expected "
						"value.\n"),
						optname(argv[i], len));
					status=-1;
					i++;
					continue;
				}
			}
		}
		else
		{
			if (optarg)
			{
				fprintf(stderr, 
					_("Option `%s' didn't expect "
					"value.\n"),
					optname(argv[i], len));
				status=-1;
				i++;
				continue;
			}
		}
		

		switch(options[ok].type)
		{
		case NONE:
			if (pass == 2)
				optstatus=(*options[ok].action.none)();
			break;

		case OPTIONS:
			if (pass == 2)
				optstatus=(*options[ok].action.options)
					(options);
			break;

		case OPTARG:
			if (pass == 2)
			optstatus=(*options[ok].action.optarg)(optarg);
			break;

		case PARAM:
			if (pass == 1)
				optstatus=(*options[ok].action.param)
					(param, optarg);
			break;
			
		case PARAMONLY:
			if (pass == 1)
				optstatus=
					(*options[ok].action.paramonly)(param);
			break;

		case ARGS:
			if (pass == 1)
				optstatus=(*options[ok].action.args)
					(argc, argv);
			break;
		}
		i += incr;

		if ((status == 0) || (optstatus == -1))
			status=optstatus;
	}

	if (status<0)
	{
		fputs("\nTry to type `haplo -help' to get some help.\n",
		      stderr);
		exit(EXIT_FAILURE);
	}
	if (status>0)
	{
		putchar('\n');
		exit(EXIT_SUCCESS);
	}
	
	if (i<argc)
		return(i);
	else
		return(-1);
}


/**
 * Display usage message
 *
 * @param options
 */
int usage(const option_t *options)
{
	int	i=0;

	fputs(_("`haplo' is a finite element analysis software. \n\n"),stderr);
	fputs(_("Usage: haplo [options] [filename]\n\n"), stderr);
	fputs(_("Options: \n"), stderr);
	while(options[i].name)
	{
	
		switch(options[i].type)
		{
		case OPTARG:
		case PARAM:
			if (isupper((int)options[i].name[1]))
			{
				char	name[20];
				snprintf(name, 20, "%s<optarg>",
					 options[i].name);
				printf("  %-20s %s\n", name,
				       _(options[i].description));
			}
			else
			{
				char	name[20];
				snprintf(name, 20, "%s=<arg>",
					 options[i].name);
				printf("  %-20s %s\n", name,
				       _(options[i].description));
			}
			break;

		case PARAMONLY:
		case NONE:
		case OPTIONS:
		case ARGS:
			printf("  %-20s %s\n", options[i].name,
			       _(options[i].description));

			break;
		}

		i++;
	}
	fputs(_("\nExemples:\n"), stderr);
	fputs(_("   haplo               "
		"Launch interactive haplo shell.\n"), stderr);
	fputs(_("   haplo file.hpl      "
		"Interpret `file.hpl' then exit.\n"), stderr);
	fputs(_("\nReport bugs to <nl@haplo.info>."), stderr);
	return(1);
}
