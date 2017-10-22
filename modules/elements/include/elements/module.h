#ifndef __ELEMENTS_ELEMENTS_MODULE_H__
#define __ELEMENTS_ELEMENTS_MODULE_H__

#include <haplo/module.h>

#ifdef HAPLO_ELEMENTS_MODULE
#	define __elements_function(x)	__haplo_function(x)
#       define __elements_type_display	__haplo_type_display
#       define __elements_type_copy		__haplo_type_copy
#       define __elements_type_free		__haplo_type_free
#	define __elements_protected		__haplo_protected
#else
#	define __elements_function(x)	
#       define __elements_type_display
#       define __elements_type_copy
#       define __elements_type_free
#	define __elements_protected
#endif

#endif /* __ELEMENTS_ELEMENTS_MODULE_H__  */
