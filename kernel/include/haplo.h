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
 * @mainpage
 * @image html haplo.png
 * 
 * @section  Un logiciel de mod�lisation m�canique par �l�ments finis
 *
 * Le projet Haplo vise � l'�criture d'un code de calcul de m�canique
 *
 *
 * @section Principales fonctionnalit�s
 *
 * Le but de ce projet �tant, avant tout, de comprendre un code de calcul,
 * il a �t� �crit � partir de rien (from scratch). A ce stade de d�veloppement,
 * les principaux algorithmes d�velopp�s sont (dans l'ordre historique) :
 *
 *  - M�canique �lastique en petites d�formations
 *  - Stockage de matrices en mode "ligne de ciel"
 *  - Solveur direct de type LDLT
 *  - Solveur it�ratif de type gradient conjugu� pr�conditionn�
 *  - Renum�rotation des noeuds
 *  - Prise en compte de conditions aux limites par dualisation
 *
 */

#ifndef __HAPLO_HAPLO_H__
#	define __HAPLO_HAPLO_H__

#	include <haplo/builtin.h>
#	include <haplo/code.h>
#	include <haplo/config.h>
#	ifdef __cplusplus
#		include <haplo/cpp.h>
#	endif /* __cplusplus */
#	include <haplo/des.h>
#	include <haplo/error.h>
#	include <haplo/func.h>
#	include <haplo/hash_str.h>
#	include <haplo/hash_ul.h>
#	include <haplo/init.h>
#	include <haplo/list.h>
#	include <haplo/memory.h>
#	include <haplo/module.h>
#	include <haplo/object.h>
#	include <haplo/olist.h>
#	include <haplo/parser.h>
#	include <haplo/pool.h>
#	include <haplo/timer.h>
#	include <haplo/utils.h>

#endif /* __HAPLO_HAPLO_H__ */
