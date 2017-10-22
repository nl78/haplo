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

#ifndef __HAPLO_FEM_H__
#define __HAPLO_FEM_H__



#ifdef HAPLO_FEM_MODULE  /* Used to build module */
#	include <haplo/config.h>
#	define HAPLO_MODULE_TITLE		"F.E.M"
#	define HAPLO_MODULE_AUTHOR		"Nicolas LAURENT"
#	define HAPLO_MODULE_REQUIRED_MAJOR	HAPLO_MAJOR
#	define HAPLO_MODULE_REQUIRED_MINOR	HAPLO_MINOR
#endif

#include <fem/analysis.h>
#include <fem/bc.h>
#include <fem/dof.h>
#include <fem/cara.h>
#include <fem/eigen.h>
#include <fem/element.h>
#include <fem/geom.h>
#include <fem/group.h>
#include <fem/integration.h>
#include <fem/load.h>
#include <fem/material.h>
#include <fem/matrix.h>
#include <fem/meshing.h>
#include <fem/model.h>
#include <fem/numbering.h>
#include <fem/post.h>
#include <fem/problem.h>
#include <fem/property.h>
#include <fem/renum.h>
#include <fem/solver.h>
#include <fem/vectorized.h>

#endif /* __HAPLO_FEM_H__ */
