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


#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "dof.h"


/*-----------------------------------------------------------------------------
                        G L O B A L   V A R I A B L E 
-----------------------------------------------------------------------------*/

/*
 * A warning at the next line means a serious problem!
 */
static const char *dof_name[FEM_DOF_MAX+1][2]={
	{ "DX",           "FX" },
	{ "DY",           "FY" },
	{ "DZ",           "FZ" },
	{ "DRX",          "MX" },
	{ "DRY",          "MY" },
	{ "DRZ",          "MZ" },
	{ "[Unamed DOF]", "P" } /* Last entry is reserved */
};


/*-----------------------------------------------------------------------------
                               P R O T O T Y P E S
-----------------------------------------------------------------------------*/

fem_dofset_t fem_dof_dofset(const char *s);
static const char *dof_name_get(fem_dofset_t dofnb, int type);
const char *fem_dof_name(fem_dofset_t dofset);
const char *fem_dof_load_name(fem_dofset_t dofset);
static fem_dofset_t dof_nb(const char *name, int type);
fem_dofset_t fem_dof_nb(const char *name);
fem_dofset_t fem_dof_load_nb(const char *name);


/*-----------------------------------------------------------------------------
                        I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/

/**
 *
 */
fem_dofset_t fem_dof_dofset(const char *s)
{	
	fem_dofset_t dofset=0;
	size_t len;
	
	do
	{
		const char *off;
		int i;
		
		off=strchr(s, ':');
		
		if (off)
		{
			off++;
			len=(size_t)(off-s)-1;
		} else
			len=strlen(s);

		for(i=0; dof_name[i][0] != NULL; i++)
			if (strncmp(s, dof_name[i][0], len) == 0)
			{
				dofset |= ( 1 << i);
				break;
			}
		s=off;
	} while(s);

	return(dofset);
}


/**
 *
 */
static const char *dof_name_get(fem_dofset_t dofnb, int type)
{
	if (dofnb > FEM_DOF_MAX)
		dofnb=FEM_DOF_MAX;

	
	return(dof_name[dofnb][type]);
}


/**
 *
 */
const char *fem_dof_name(fem_dofset_t dofnb)
{
	return dof_name_get(dofnb, 0);
}



/**
 *
 */
const char *fem_dof_load_name(fem_dofset_t dofnb)
{
	return dof_name_get(dofnb, 1);
}


/**
 *
 */
static fem_dofset_t dof_nb(const char *name, int type)
{
	fem_dofset_t dofset=FEM_DOF_MAX;
	fem_dofset_t i;
	
	for(i=0; dof_name[i][type] != NULL; i++)
		if (strcmp(name, dof_name[i][type]) == 0)
		{
			dofset = i;
			break;
		}
	
	return(dofset);
}


/**
 *
 */
fem_dofset_t fem_dof_nb(const char *name)
{
	return dof_nb(name, 0);
}


/**
 *
 */
fem_dofset_t fem_dof_load_nb(const char *name)
{
	return dof_nb(name, 1);
}
