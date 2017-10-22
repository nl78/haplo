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

/**
 * @file
 *
 * Haplo executable entry point.
 * - initialize libhaplo
 * - parse command line
 * - display some hello/goodbye messages
 */


#include <haplo/config.h>

#include <errno.h>
#ifdef HAVE_LOCALE_H
#	include <locale.h>
#endif
#include <stdio.h>
#include <string.h>
#ifdef HAVE_SYS_RESOURCE_H
#	include <sys/resource.h>
#endif /* HAVE_SYS_RESOURCE_H */
#include <sys/stat.h>
#ifdef HAVE_SYS_TIME_H
#	include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */
#include <sys/types.h>
#include <time.h>
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_LIBREADLINE
#	include <readline/readline.h>
#	include <readline/history.h>
#endif /* HAVE_LIBREADLINE */

#include <haplo.h>

#include "hgetopt.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

void do_work(haplo_param_t *param);
int main(int argc, char *const argv[]);
static int version(void);
static int config(void);
static int cflags(void);
static int libs(void);
static int noinit(haplo_param_t *param);
static int autoload(haplo_param_t *param);
static int nobanner(haplo_param_t *param);
static int setcolors(haplo_param_t *param, const char *optarg);
#ifdef SIGXCPU
static int limit_cpu(const char *value);
#endif
static int limit_mem(const char *value);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 *
 */
void do_work(haplo_param_t *param)
{
	haplo_timer_t timer;
	
	int status=EXIT_SUCCESS;


	haplo_timer_start(&timer);
	
	/* MAIN LOOP */
	haplo_main(param);

	/* EXIT CLEANLY */
	haplo_fini();


	haplo_timer_stop(&timer);
	
	if (param->banner)
	{
		haplo_timer_print(&timer);

		status = (HAPLO_MEMORY_USAGE())?(EXIT_FAILURE):(EXIT_SUCCESS);
		

		haplo_info(_("End of processing."));
	}
	
	exit(status);

	/* Never reached */
	return;
}


/**
 * Entry point
 *
 * @param argc
 * @param argv
 */
int main(int argc, char *const argv[])
{
#ifdef ENABLE_NLS
	char *path;
#endif
	int		ind;
	haplo_param_t	param;
	option_t	options[]={
		{ "-I" , N_("Add <optarg> to scripts search-path."),
		  OPTARG, OPT_F(haplo_parser_path_add)},
		{ "-L" , N_("Add <optarg> to modules search-path."),
		  OPTARG, OPT_F(haplo_module_path_add) },
		{ "-cflags", N_("Print compilation flags and exit."),
		  NONE, OPT_F(cflags) },
		{ "-colors", N_("Set <arg> as colors scheme."),
		  PARAM, OPT_F(setcolors) },
		{ "-config", N_("Print configuration informations and exit."),
		  NONE, OPT_F(config) },
#ifdef SIGXCPU
		{ "-cpu", N_("Limit CPU usage to <optarg> seconds."),
		  OPTARG, OPT_F(limit_cpu) },
#endif
		{ "-help", N_("Print this message."),
		  OPTIONS, OPT_F(usage) },
		{ "-libs", N_("Print linkage flags and exit."),
		  NONE, OPT_F(libs) },
		{ "-mem", N_("Limit memory usage to <optarg> Mb."),
		  OPTARG, OPT_F(limit_mem) },
		{ "-nobanner", N_("No welcome message."),
		  PARAMONLY, OPT_F(nobanner) },
		{ "-noinit", N_("Do not load init script."),
		  PARAMONLY, OPT_F(noinit) },
		{ "-autoload", N_("Do load automatically files."),
		  PARAMONLY, OPT_F(autoload) },
		{ "-version", N_("Print version informations and exit."),
		  NONE, OPT_F(version) },
		{ NULL, NULL, NONE,  OPT_F(NULL) }
		
	};

#ifdef ENABLE_NLS
	path=haplo_param_path("share/locale");
	bindtextdomain(PACKAGE, path);
	textdomain (PACKAGE);
	HAPLO_FREE(path);
	if (setlocale(LC_ALL, "") == NULL)
		haplo_error("Locale are not supported in this environement");
	setlocale(LC_NUMERIC, "C");
#endif

	haplo_param_default(&param);
	
	hgetopt(&param, options, argc, argv, 1);
	
	haplo_init(&param);

	ind=hgetopt(&param, options, argc, argv, 2);

	if (ind > 0)
		param.filename=argv[ind];

	do_work(&param);
	/* never reached */

	return(0);	/* avoid warning on non-gcc compiler */
}


/**
 * Display version message
 *
 * @return 1
 */
static int version(void)
{
	printf("Haplo %d.%d\n", HAPLO_MAJOR, HAPLO_MINOR);

	puts("\nCopyright (C) 1998-2007 Nicolas LAURENT");
	puts("This is free software; see the sources for copying conditions.");
	puts("There is NO warranty; not even for MERCHANTABILITY or "
	     "FITNESS FOR A PARTICULAR");
	fputs("PURPOSE.", stdout);
	
	
	return(1);
}


/**
 * Display config message
 *
 * @return 1
 */
static int config(void)
{
	char *path;
	
	printf(_("Version............: %d.%d\n"),
	       HAPLO_MAJOR, HAPLO_MINOR);
	printf(_("Revision...........: %s\n"), HAPLO_SCM);
	printf(_("Build..............: #%s (%s) [%s]\n"),
	       __DATE__, __TIME__, HAPLO_BUILD);
	printf(_("OS.................: %s\n"), HAPLO_HOST_OS);
	printf(_("Modules support....: "));
	switch(HAPLO_MODULE_IMPL)
	{
	case HAPLO_MODULE_IMPL_NONE:
		puts(_("None"));
		break;
	case HAPLO_MODULE_IMPL_DL:
		puts(_("DL style"));
		break;
	case HAPLO_MODULE_IMPL_DLD:
		puts(_("DLD style"));
		break;
	case HAPLO_MODULE_IMPL_WIN32:
		puts(_("WIN32 style"));
		break;
	default:
		puts(_("Unknown"));
	}
	printf(_("i18n...............: "));
#ifdef ENABLE_NLS
	printf(_("yes\n"));
	path=haplo_param_path("share/locale");
	printf(_("i18n data directory: %s\n"), path);
	HAPLO_FREE(path);
#else
	printf("no\n");
#endif /* ENABLE_NLS */
	printf(_("Options............: %s\n"), HAPLO_OPTIONS);
	path=haplo_param_path(HAPLO_MODULE_DIR);
	printf(_("Module directory...: %s\n"), path);
	HAPLO_FREE(path);
	path=haplo_param_path(HAPLO_SCRIPTS_DIR);
	printf(_("Scripts directory..: %s"),	 path);
	HAPLO_FREE(path);

	return(1);
}


/**
 * Display cflags message
 *
 * @return 1
 */
static int cflags(void)
{
	char	*path;
	
	path=haplo_param_path(HAPLO_INCLUDE_DIR);
	printf("-I%s ", path);
	HAPLO_FREE(path);
	
	return(1);
}


/**
 * display libs message
 *
 * @return 1
 */
static int libs(void)
{
	char *path;
	
	path=haplo_param_path(HAPLO_MODULE_DIR);
	printf("-L%s -lhaplo -lm ", path);
	HAPLO_FREE(path);
	return(1);
}


/**
 *
 *
 */
static int noinit(haplo_param_t *param)
{
	param->init_filename=NULL;

	return(0);
}


/**
 *
 *
 */
static int autoload(haplo_param_t *param)
{
	param->autoload=1;

	return(0);
}


/**
 *
 *
 */
static int nobanner(haplo_param_t *param)
{
	param->banner=0;

	return(0);
}


/**
 * set colors schemes
 *
 * @param param
 * @param optarg
 * @return 0 or -1 in case of error
 */
static int setcolors(haplo_param_t *param, const char *optarg)
{
	if (strcmp(optarg, "ansi")==0)
	{
		param->colors = HAPLO_COLORS_ANSI;
	} else if (strcmp(optarg, "flash")==0)
	{
		param->colors = HAPLO_COLORS_ANSI | HAPLO_COLORS_FLASH;
	} else if (strcmp(optarg, "none")==0)
	{
		param->colors = HAPLO_COLORS_NONE;
	} else {
		fputs(_("Option `-colors' expected one of the "
			"following values:\n"
			"  `none'\tno colors at all\n"
			"  `ansi'\tuse simple ansi colors\n"
			"  `flash'\tuse all ansi colors\n"), stderr);
		return(-1);
	}
	
	
	return(0);
}


/**
 *
 */
#ifdef SIGXCPU
static int limit_cpu(const char *value)
{
	struct rlimit limit;

	if (getrlimit(RLIMIT_CPU, &limit) == 0)
	{
		rlim_t sec;
		
		sec=atoi(value);
		if (sec > limit.rlim_max)
			sec = limit.rlim_max;
		limit.rlim_cur=sec;
		
		setrlimit(RLIMIT_CPU, &limit);
			
	} else
		perror("getrlimit()");
	return(0);
}
#endif /* SIGXCPU */


/**
 *
 */
static int limit_mem(const char *value)
{
#ifndef WIN32
	struct rlimit limit;
	
	if (getrlimit(RLIMIT_DATA, &limit))
	{
		rlim_t bytes;
		
		bytes=atoi(value)*1024*1024;

		if (bytes > limit.rlim_max)
			bytes = limit.rlim_max;
		limit.rlim_cur=bytes;

		setrlimit(RLIMIT_DATA, &limit);
	} else
		perror("getrlimit()");
#endif	
	return(0);
}

