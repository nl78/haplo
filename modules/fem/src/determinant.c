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
 * NOT USED.
 */

#include <float.h>
#include <math.h>

#include <haplo.h>

#include "determinant.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static double determinant_swap(double **mat, unsigned short dim,
			       unsigned short offset);
double __fem_determinant(double **mat, unsigned short dim);


/*-----------------------------------------------------------------------------
                        I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/

/**
 * @param mat
 * @param dim
 * @param offset
 *
 * @return A[offset][offset]
 */
static double determinant_swap(double **mat, unsigned short dim,
			       unsigned short offset)
{
	if (fabs(mat[offset][offset]) < DBL_EPSILON)
	{
		unsigned short i;

		for(i=offset+1; i<dim; i++)
		{
			if (fabs(mat[i][offset]) >= DBL_EPSILON)
			{
				/*
				 * Swap line `i' and line `offset'
				 */
				double	*tmp;

				tmp=mat[offset];
				mat[offset]=mat[i];
				mat[i]=tmp;
				break;
			}
		}
	}

	return(mat[offset][offset]);
}


/**
 * @param mat
 * @param dim
 *
 * @return the determinant of mat
 *
 * @warning mat will be altered
 */
double __fem_determinant(double **mat, unsigned short dim)
{
	double det=1.0;
	unsigned short i;


	for(i=0; i<dim; i++)
	{
		
		const double Aii=determinant_swap(mat, dim, i);
		unsigned short j;

		if (fabs(Aii) < DBL_EPSILON)
		{
			det=0.0;
			break;
		}

		det *= Aii;
		
		/*
		 * Do elimination
		 */
		for(j=i+1; j<dim; j++)
		{
			const double pivot=mat[j][i]/Aii;

			unsigned short k;
			
			for(k=i; k<dim; k++)
				mat[j][k] -= pivot*mat[i][k];
		}
	}

	return(fabs(det));
}
