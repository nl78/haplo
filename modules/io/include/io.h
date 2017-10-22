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

#ifndef __HAPLO_IO_H__
#define __HAPLO_IO_H__

#ifdef HAPLO_IO_MODULE  /* Used to build module */
#	include <haplo/config.h>
#	define HAPLO_MODULE_TITLE		"IO layer"
#	define HAPLO_MODULE_AUTHOR		"Nicolas LAURENT"
#	define HAPLO_MODULE_REQUIRED_MAJOR	HAPLO_MAJOR
#	define HAPLO_MODULE_REQUIRED_MINOR	HAPLO_MINOR
#endif

#include <io/block.h>
#include <io/data.h>
#include <io/decode.h>
#include <io/encode.h>
#include <io/entity.h>
#include <io/error.h>
#include <io/file.h>
#include <io/fileslice.h>
#include <io/filter.h>
#include <io/gzindex.h>
#include <io/gzio.h>
#include <io/parser.h>
#include <io/parser_UNV.h>
#include <io/value.h>

#endif /* __HAPLO_IO_H__ */
