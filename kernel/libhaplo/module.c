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
 *              C h a r g e m e n t   d e s   M O DU L E S 
 *              ------------------------------------------
 *
 * Il s'agit de charger une bibliotèques dynamique.
 * Le nom du fichier correspondant est de la forme libXXX.so ou libXXX.sl selon
 * l'implementation.
 * Une fois le dlopen(3) ou shl_load() effectué on évalue le symbole XXX_init.
 *
 */

#include <haplo/config.h>

#include <stdio.h>
#include <string.h>

#include "builtin.h"
#include "error.h"
#include "func.h"
#include "memory.h"
#include "module.h"
#include "slink.h"
#include "utils.h"


#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
#	ifdef HAVE_DIRECT_H
#		include <direct.h>
#	endif
#	ifdef HAVE_IO_H
#		include <io.h>
#	endif
#	ifdef HAVE_DL_H
#		include <dl.h>
#	endif /* HAVE_DL_H */
#	ifdef HAVE_DLFCN_H
#		include <dlfcn.h>
#	endif /* HAVE_DLFCN_H */
#	include <limits.h>
#	include <sys/stat.h>
#	include <sys/types.h>
#	ifdef HAVE_UNISTD_H
#		include <unistd.h>
#	endif
#	ifndef PATH_MAX
#		define PATH_MAX	1024
#	endif /* PATH_MAX */
#	ifndef HAVE_DLERROR
#		include <errno.h>
#		define dlerror()	strerror(errno)
#	endif /* HAVE_DLERROR */
#	ifndef RTLD_NOW
#		define RTLD_NOW	0
#	endif /* RTLD_NOW */
#	ifdef HAVE_WINDOWS_H
#		include <Windows.h>
#	endif
#endif /* HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE */


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
static slink_t		*module_list=NULL;
static slink_t		*module_path=NULL;
#endif /* */


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

int haplo_module_path_add(const char *path);
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
static module_handle_t module_library_load(const char *path, int flag);
static int module_library_unload(module_handle_t handle);
module_handle_t *  __haplo_module_library_load(const char *path);
void *__haplo_module_symbol_get(module_handle_t handle, const char *symbol);
static module_t * module_already_loaded(const char *name);
static char *module_full_path(const char *dir, const char *name);
static int module_load_magic(const char *magic);
static int module_load_check(int major, int minor);
static int module_load_depend(const char *const* depend);
static int module_load_init(module_t *module);
static int module_load(module_t *module, const char *dir);
int __haplo_module_load_options(const char *name, const char *options);
int __haplo_module_load(const char *name);
static void module_status_loaded(module_t *module);
static void module_status_path(const char *path);
void __haplo_module_status(void);
static void module_unload(module_t *module);
void __haplo_module_unload_all(void);
void __haplo_module_reload(const char *name);
#endif /* HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE */


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Add a path in module searchpath
 *
 * @param path
 */
int haplo_module_path_add(const char *path)
{
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
	char *full_path=NULL;
	int len;
	
	if (path[0] != '\0')
	{
		if (path[0] == '/')
		{
			full_path=haplo_strdup(path);
			len=strlen(full_path);
		} else {
			char here[PATH_MAX];

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
	} else
		full_path=haplo_strdup(path);
	
	module_path=__haplo_slink_prepend(module_path, full_path);
#else	/* HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_NONE */
	(void)path;	/* avoid gcc to complain... */
#endif /* HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE */	

	return(0);
}

/**
 *
 */
haplo_boolean_t *__haplo_module_isloaded(const char *name)
{
	haplo_boolean_t *status=NULL;


#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
	if (module_already_loaded(name))
		status = haplo_boolean(HAPLO_TRUE);
	else
#endif
		status = haplo_boolean(HAPLO_FALSE);
	
	return(status);
}


#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
/*
 * the rest of the module support is not needed if the arch does not support
 * loadable library mecanism.
 */


/**
 * dlopen() wrapper
 *
 * @param path is the fully qualified path of the object we're trying to load
 */

static module_handle_t module_library_load(const char *path, int flag)
{
	module_handle_t handle=NULL;

	
#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_DL

	if (flag)
		handle=dlopen(path, RTLD_LAZY);
	else
		handle=dlopen(path, RTLD_NOW | RTLD_GLOBAL);	

#	endif
#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_DLD
	handle=shl_load(path,
			BIND_IMMEDIATE |
			BIND_NONFATAL |
			BIND_VERBOSE,
			0);
#	endif
#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_WIN32
	handle=LoadLibrary(path);
#	endif

	if (!handle)
	{
		/*
		 * Display diagnostic only if a object was found or searched
		 * in LD_LIBRAY_PATH
		 */
		if ((path[0] != '/') || (access(path, F_OK) == 0))
			haplo_error(_("Failed in loading: %s"), dlerror());
	}

	return(handle);
}


/**
 * dlclose() wrapper
 * 
 * @param handle
 */
static int module_library_unload(module_handle_t handle)
{
	int status=0;

#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_DL
	if (handle)
		status=dlclose(handle);
#	endif
#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_DLD
	status=shl_unload(handle);
#	endif

#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_WIN32
	status=!(FreeLibrary(handle));
#	endif

	return(status);
}


/**
 * Haplo interface for loading module
 *
 * @param path
 */
module_handle_t * __haplo_module_library_load(const char *path)
{
	module_handle_t	*handle;
	
	HAPLO_ALLOC(handle, 1);
	*handle=module_library_load(path, 0);
	
	if (! *handle)
	{	
		HAPLO_FREE(handle);
		handle=NULL;
	}

	return(handle);
}


/**
 * Resolve a symbol in a module
 *
 * @param handle
 * @param symbol
 */
void *__haplo_module_symbol_get(module_handle_t handle, const char *symbol)
{
	void *addr=NULL;

	if (symbol && symbol[0])
	{

#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_DL
		addr = dlsym(handle, symbol);
#	endif

#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_DLD
		shl_findsym(&handle, symbol, TYPE_UNDEFINED, (void *)&addr);
#	endif 


#	if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_WIN32
		addr = GetProcAddress(handle,symbol);
#	endif 

	}
	
	return(addr);
}


/**
 * check if a module is already loaded
 *
 * @param name
 */
static module_t * module_already_loaded(const char *name)
{
	slink_t	*s;
	module_t *module=NULL;
	
	for(s=module_list; s; s=s->next)
	{
		if (strcmp(MODULE(s->data)->name, name)==0)
		{
			module=MODULE(s->data);
			break;
		}
	}
	return(module);
}


/**
 * try to resolve to fully qualified path from a module name
 *
 * @param dir
 * @param name
 */
static char * module_full_path(const char *dir, const char *name)
{
	char *path;
	
	if (dir[0] != '\0')
	{
		HAPLO_ALLOC(path,
			    strlen(dir)+
			    strlen(name)+
			    strlen(HAPLO_MODULE_PREFIX)+
			    strlen(HAPLO_MODULE_SUFFIX)+2);
		strcpy(path, dir);
		strcat(path, "/" HAPLO_MODULE_PREFIX);
		strcat(path, name);
		strcat(path, HAPLO_MODULE_SUFFIX);
	}
	else
	{
		HAPLO_ALLOC(path,
			    strlen(name)+
			    strlen(HAPLO_MODULE_PREFIX)+
			    strlen(HAPLO_MODULE_SUFFIX)+1);
		strcpy(path, HAPLO_MODULE_PREFIX);
		strcat(path, name);
		strcat(path, HAPLO_MODULE_SUFFIX);
	}
	
	return(path);
}


/**
 *
 */
static int module_load_magic(const char *magic)
{
	int status=-1;
	
	if ((magic[0]==HAPLO_MAG0) &&
	    (magic[1]==HAPLO_MAG1) &&
	    (magic[2]==HAPLO_MAG2) &&
	    (magic[3]==HAPLO_MAG3) &&
	    (magic[4]==HAPLO_MAG4))
		status=0;

	return(status);
}


/** 
 *
 */
static int module_load_check(int major, int minor)
{
	int status=0;
	
	if (major == HAPLO_MAJOR)
	{
		if (minor > HAPLO_MINOR)
		{
			haplo_error(
				"Haplo-kernel is outdated (required minor %d)",
				minor);
			status=-1;
		}
	} else if (major < HAPLO_MAJOR)
	{
		haplo_warning("This module is rather old.");
	} else {
		haplo_error("Haplo-kernel is outdated (required major %d)",
			    major);
		status=-1;
	}

	return(status);
}


/**
 *
 */
static int module_load_depend(const char *const* depend)
{
	int	i;

	for(i=0; depend[i]!=NULL; i++)
	{
		if (!module_already_loaded(depend[i]))
		{
			if (__haplo_module_load(depend[i])<0)
			{
				haplo_error("Dependencies not resolved.");
				return(-1);
			}
		}
	}

	return(0);
}


/**
 *
 */
static int module_load_init(module_t *module)
{
	haplo_module_t *hmodule;
	int status=-1;
	char *symbol;

	HAPLO_ALLOC(symbol, 
		    strlen(module->name) + sizeof(HAPLO_MODULE_SYMBOL)+1);

	strcpy(symbol, module->name);
	strcat(symbol, HAPLO_MODULE_SYMBOL);
	hmodule=__haplo_module_symbol_get(module->handle, symbol);


	if (hmodule && (module_load_magic(hmodule->magic) == 0))
	{
		if (module_load_check(hmodule->required_major,
				      hmodule->required_minor) == 0)
		{	
			if (module_load_depend(hmodule->depend))
			{	
				HAPLO_FREE(symbol);
				return(-1);
			}
			

			/*
			 * Now reload this module with check on _all_ symbols
			 */
			module_library_unload(module->handle);
			module->handle=module_library_load(module->path, 0);
			if (!module->handle)
			{
				HAPLO_FREE(symbol);
				return(-1); /* is there unresolved? */
			}
			
			/* remap "module" symbol */
			hmodule=__haplo_module_symbol_get(
				module->handle, symbol);
			HAPLO_FREE(symbol);
			if (!module)
				return(-1); /* someone change this lib */
			
			if (hmodule->init)
			{
				int status;

				status = (*hmodule->init)(module->options);

				if (status)
				{
					haplo_error(_("Failed in initializing module `%s'."), module->name);
					return(-1);
				}
			}
			module->types=__haplo_object_type_register(
				hmodule->types, module);
			
			module->functions=__haplo_func_register(
				hmodule->functions, module);
			module->hmodule=hmodule;
			status=0;
		} else
			HAPLO_FREE(symbol);	
		
	} else {
		HAPLO_FREE(symbol);
		haplo_error(_("Library `%s' (%s) is not a haplo-module."),
			    module->name, module->path);
	}
	
	return(status);
}


/**
 *
 * @return  0  OK
 *          1 not found. not loaded. but keep trying.
 *         -1 not loaded because of error. Stop loading loop.
 */
static int module_load(module_t *module, const char *dir)
{
	module_handle_t handle;
	char *path;
	int status=1;

	path=module_full_path(dir, module->name);

	/* Loading */
	handle=module_library_load(path, 1);
	
	if (handle)
	{
		module->handle=handle;
		module->path=path;

		/* Init */
		if (module_load_init(module) == 0)
			status=0;
		else {
			module_library_unload(module->handle);
			HAPLO_FREE(module->name);
			if (module->options)
				HAPLO_FREE(module->options);
			HAPLO_FREE(path);
			HAPLO_FREE(module);
			status=-1;
		}
	} else {
		if (access(path, F_OK) == 0)
		{
			HAPLO_FREE(module);
			status=-1;
		}
		
		HAPLO_FREE(path);
	}

	return(status);
}


/**
 * haplo interface: use()
 *
 * @param name
 */
int __haplo_module_load_options(const char *name, const char *options)
{
	slink_t		*p;
	module_t	*module;
	char		here[PATH_MAX];
	

	if (module_already_loaded(name))
	{
		haplo_info(_("Module `%s' is already loaded."), name);
		return(1);
	}

	HAPLO_ALLOC(module, 1);	
	module->name = haplo_strdup(name);
	if (options)
		module->options=haplo_strdup(options);
	else
		module->options=NULL;

	/*
	 * First: try current working directory
	 */
	getcwd(here, PATH_MAX);
	
	switch(module_load(module, here))
	{
	case 0:
		module_list=__haplo_slink_prepend(module_list, module);
		return(0);
	case -1:
		return(-1);
	case 1:
		break;
	}


	/*
	 * Second: try `module_path'...
	 */
	for(p=module_path; p; p=p->next)
	{
		switch(module_load(module, (char *)p->data))
		
		{
		case 0:
			module_list=__haplo_slink_prepend(module_list,
							  module);
			return(0);
		case -1:
			return(-1);
		case 1:
			/* still trying */
			break;
		}
	}


	/*
	 * Then try LD_LIBRARY_PATH
	 */
	if (module_load(module, "") == 0)
	{
		module_list=__haplo_slink_prepend(module_list,
						  module);
		return(0);
	}

	HAPLO_FREE(module->name);
	HAPLO_FREE(module);
	
	haplo_error(_("Failed in loading module `%s'."), name);

	return(-1);
}


/**
 * Loading with no options
 */
int __haplo_module_load(const char *name)
{
	return __haplo_module_load_options(name, NULL);
}


/**
 *
 */
static void module_status_loaded(module_t *module)
{
	const char **dep;
#define NA(x)	(x)?x:"n/a"
	
	printf("  - %s (%s)\n", module->name, module->path);
	printf("    | Title            : %s\n", NA(module->hmodule->title));
	printf("    | Version          : %s\n", NA(module->hmodule->version));
	printf("    | Author           : %s\n", NA(module->hmodule->author));
	printf("    | defined types    : %u\n", module->types);
	printf("    | defined functions: %u\n", module->functions);
	if (module->options)
		printf("    | loading options  : %s\n", module->options);
	printf("    | Requires         : haplo-%d.%d",
	       module->hmodule->required_major,
	       module->hmodule->required_minor);
	for(dep=module->hmodule->depend; *dep; dep += 1)
		printf(", %s", *dep);
	printf("\n\n");

#undef NA

	return;
}


/**
 *
 */
static void module_status_path(const char *path)
{
	printf("  - %s\n", path);
	
	return;
}


/**
 *
 */
void __haplo_module_status(void)
{
	char here[PATH_MAX];
	
	getcwd(here, PATH_MAX);
	
	haplo_bordered("Module status");

	haplo_underlined("loaded");
	__haplo_slink_loop(module_list, SLINK_FUNC(module_status_loaded));
	printf("\n");

	haplo_underlined("Search pathes");
	printf("  - Current directory (%s)\n", here);
	__haplo_slink_loop(module_path, SLINK_FUNC(module_status_path));
	printf("  - Default system pathes\n\n");

	return;
}


/**
 * 
 */
static void module_unload(module_t *module)
{
	int status=0;

	if (module->hmodule->fini)	
		status = (*module->hmodule->fini)(module->options);

	/*
	 * some module does not support dlclosing: they use atexit()
	 * and, when haplo exits, they crash. It is the case
	 * for xhaplo module.
	 */
	if ((status == 0) && module_library_unload(module->handle))
	{
		haplo_error(_("Failure in unloading `%s': %s"),
			    module->name,
			    dlerror());
	}
	if (module->options)
		HAPLO_FREE(module->options);
	HAPLO_FREE(module->path);
	HAPLO_FREE(module->name);
	HAPLO_FREE(module);
	
	return;
}


/**
 *
 */
void __haplo_module_unload_all(void)
{
	__haplo_slink_free_f(module_list, SLINK_FUNC(module_unload));
	__haplo_slink_free_f(module_path, SLINK_FUNC(HAPLO_FREE_FUNC));

	return;
}

#endif /* HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE */
