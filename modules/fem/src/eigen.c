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
 *                         E I G E N   S O L V E R 
 *                         =======================
 *
 */ 

#include <haplo.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "eigen.h"
#include "matrix.h"
#include "solver.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static eigen_t *eigen_new(unsigned long req);
static void eigen_ortho_sub(fem_vector_t *u, double a, fem_vector_t *v);
static void eigen_ortho(eigen_t *eigen, fem_vector_t *u, const fem_vector_t *mu,
			unsigned long n);
static fem_vector_t *eigen_vector_initial(unsigned long size);
static int eigen_solve_one(eigen_t *eigen, fem_matrix_t *K, const fem_matrix_t *M,
			   unsigned long n);
static eigen_t *eigen_solve(fem_matrix_t *K, const fem_matrix_t *M, unsigned long req);
fem_eigen_t *fem_eigen_new(fem_matrix_t *K, const fem_matrix_t *M,
			   const haplo_float_t *req);
void fem_eigen_free(fem_eigen_t *eigen);
void fem_eigen_display(const fem_eigen_t *eigen);
haplo_float_t *fem_eigen_getf(const fem_eigen_t *eigen,
			      const haplo_float_t *i);
fem_vector_t *fem_eigen_getv(const fem_eigen_t *eigen, const haplo_float_t *i);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
static eigen_t *eigen_new(unsigned long req)
{
	eigen_t *eigen;
	unsigned long i;

	HAPLO_ALLOC(eigen, 1);
	
	if (req==0)
	{
		eigen->size=0;
		req=1;
	} else
		eigen->size=req;

	HAPLO_ALLOC(eigen->value,  req);
	HAPLO_ALLOC(eigen->vector, req);

	for(i=0; i<req; i += 1)
		eigen->vector[i] = NULL;

	return(eigen);
}


/**
 *
 */
static void eigen_ortho_sub(fem_vector_t *u, double a, fem_vector_t *v)
{
	const unsigned long size=u->size;
	unsigned long i;
	
	for(i=0; i<size; i++)
		u->coef[i] -= a*v->coef[i];
	
	return;
}


/**
 *
 */
static void eigen_ortho(eigen_t *eigen, fem_vector_t *u, const fem_vector_t *mu,
		   unsigned long n)
{
	unsigned long i;
	
	for(i=0; i<n; i++)
		eigen_ortho_sub(u,
				__fem_vector_dot(mu, eigen->vector[i]),
				eigen->vector[i]);

	return;
}


/**
 *
 */
static fem_vector_t *eigen_vector_initial(unsigned long size)
{
	fem_vector_t *u;
	unsigned long i;
	
	u=fem_vector_new(size);

	for(i=0; i<size; i++)
	{
		const double r=rand();
		u->coef[i]=(r / RAND_MAX) - 0.5;
	}

	fem_vector_normalize(u);

	return(u);
}

	
/**
 *
 */
static int eigen_solve_one(eigen_t *eigen, fem_matrix_t *K, const fem_matrix_t *M,
			   unsigned long n)
{
	fem_vector_t *u;
	fem_vector_t *f;
	fem_vector_t *mu;
	double l1=0.0;
	double l=1.0;
	double a=0.0;
	unsigned long iter=0;

	u=eigen_vector_initial(K->size);
	
	f=fem_vector_new(K->size);
	mu=fem_vector_new(K->size);

	__fem_solver_factorize(K);
	
	__fem_matrix_vector(M, u, f);
	
	while((fabs(l1-l)/l) > DBL_EPSILON)
	{
		/*
		 * [k][U] = [F]
                 * [F']   = [M][U]
		 * a      = [U].[F']
                 * l      = [U].[F]/a
		 * [F]=1/sqrt(a)*[F']
		 */
		iter += 1;
		l=l1;
		
		__fem_vector_copy(u, f);

		__fem_solver_invert(K, u);

		__fem_matrix_vector(M, u, mu);
		eigen_ortho(eigen, u, mu, n);

		l1=__fem_vector_dot(u, f);
		
		__fem_matrix_vector(M, u, f);
		a=__fem_vector_dot(u, f);
		
		l1 /= a;
		
		__fem_vector_div(f, sqrt(a));		
	}
	
	__fem_vector_div(u, sqrt(a));
	
	eigen->vector[n]=u;
	eigen->value[n]=l1;

	fem_vector_free(mu);
	fem_vector_free(f);
	

	haplo_info("Iterations: %4lu, lambda = %.2f", iter, l1);

	return(0);
}


/**
 *
 */
static eigen_t *eigen_solve(fem_matrix_t *K, const fem_matrix_t *M, unsigned long req)
{
	eigen_t *eigen;
	unsigned long i;
	
	eigen=eigen_new(req);

	for(i=0; i<req; i++)
		eigen_solve_one(eigen, K, M, i);
	
	return(eigen);
}

	
/**
 *
 */	
fem_eigen_t *fem_eigen_new(fem_matrix_t *K,
			   const fem_matrix_t *M,
			   const haplo_float_t *req)
{
	eigen_t *eigen=NULL;

	if (M->size == K->size)
	{
		unsigned long requested;

		requested=haplo_ulong(req);
		if (requested == 0)
		{
			haplo_warning("Solve at least one eigen value");
			requested=1;
		}
		if (requested > K->size)
		{
			haplo_warning("Cannot solve more than %lu "
				      "eigen values", K->size);
			requested=K->size;
		}

		eigen=eigen_solve(K, M, requested);
	} else 
		haplo_error("Matrice's sizes missmatch");

	return(eigen);
}



/**
 *
 */
void fem_eigen_free(fem_eigen_t *eigen)
{
	unsigned long i;

	for(i=0; i<eigen->size; i += 1)
		if (eigen->vector[i])
			fem_vector_free(eigen->vector[i]);

	HAPLO_FREE(eigen->value);
	HAPLO_FREE(eigen->vector);
	HAPLO_FREE(eigen);

	return;
}


/**
 *
 */
void fem_eigen_display(const fem_eigen_t *eigen)
{
	printf("Eigen base (%lu vector%s)",
	       eigen->size, (eigen->size>1)?"s":"");

	return;
}


/**
 *
 */
haplo_float_t *fem_eigen_getf(const fem_eigen_t *eigen,
			      const haplo_float_t *i)
{
	double *f=NULL;
	unsigned long j=haplo_ulong(i);

	if ((j>0) && (j <= eigen->size))
		f = haplo_float(eigen->value[j-1]);
        else 
		haplo_error("Index exceeds dimension of eigen base");

	return(f);
}


/**
 *
 */
fem_vector_t *fem_eigen_getv(const fem_eigen_t *eigen,
			     const haplo_float_t *i)
{
	fem_vector_t *v=NULL;
	unsigned long j=haplo_ulong(i);

	if ((j>0) && (j <= eigen->size))
		v = fem_vector_copy(eigen->vector[j-1]);
        else 
		haplo_error("Index exceeds dimension of eigen base");

	return(v);
}

