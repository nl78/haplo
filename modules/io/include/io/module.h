/*
 * Copyright (C) 2007 Nicolas LAURENT
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

#ifndef __HAPLO_IO_MODULE_H__
#define __HAPLO_IO_MODULE_H__

#include <haplo/module.h>

#ifdef HAPLO_IO_MODULE
#	define __io_function(x)		__haplo_function(x)
#       define __io_type_display	__haplo_type_display
#       define __io_type_copy		__haplo_type_copy
#       define __io_type_free		__haplo_type_free
#	define __io_protected		__haplo_protected
#else
#	define __io_function(x)	
#       define __io_type_display
#       define __io_type_copy
#       define __io_type_free
#	define __io_protected
#endif

#endif /* __HAPLO_IO_MODULE_H__  */
