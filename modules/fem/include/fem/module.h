#ifndef __HAPLO_FEM_MODULE_H__
#define __HAPLO_FEM_MODULE_H__

#include <haplo/module.h>

#ifdef HAPLO_FEM_MODULE
#	define __fem_function(x)	__haplo_function(x)
#       define __fem_type_display	__haplo_type_display
#       define __fem_type_copy		__haplo_type_copy
#       define __fem_type_free		__haplo_type_free
#	define __fem_protected		__haplo_protected
#else
#	define __fem_function(x)	
#       define __fem_type_display
#       define __fem_type_copy
#       define __fem_type_free
#	define __fem_protected
#endif

#endif /* __HAPLO_FEM_MODULE_H__  */
