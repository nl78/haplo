/*
 * Copyright (C) 1998-2009 Nicolas LAURENT
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
 *                        L I N E A R   S O L V E R 
 *                        =========================
 *
 * This file contains implementations of 2 solvers:
 *
 * - Direct Solver: LDLT factorization with hand optimized code for x86.
 *
 * References:
 *
 * "Analyse numérique matricielle appliquée à l'art de l'ingénieur"
 *                      P. LASCAUX / R. THEODOR
 * 
 *  Tome 1 - ISBN 2-225-84122-5 - pages 218-222, 234-250
 * 
 */


#include <haplo.h>

#include <math.h>
#include <string.h>
#include <stdio.h>

#include "matrix.h"
#include "solver.h"

/*#define DEBUG_SOLVER*/


/*-----------------------------------------------------------------------------
                                S O L V E R _ S
-----------------------------------------------------------------------------*/

struct solver_s
{
	fem_matrix_t *mat;	/* IN/OUT */
	unsigned long i;	/* IN     */
	unsigned long j_max;	/* IN     */
};


/*-----------------------------------------------------------------------------
                                   M A C R O
-----------------------------------------------------------------------------*/

#define L(i,j) 	 mat->coef[j+mat->diag[i]-i]
#define D(i)   	 mat->coef[mat->diag[i]]
#define FIRST(i) ((i + mat->diag[i-1]) + (1 - mat->diag[i]))

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static int solver_check(const fem_matrix_t *matrix,
			const fem_vector_t *vector);

static void solver_line_factorize(fem_matrix_t *mat, unsigned long i,
				  unsigned long j_min,
				  unsigned long j_max);

static void solver_factorize_threaded_task(
	const struct solver_s *param);

static void solver_line_finish(fem_matrix_t *mat, unsigned long i,
			       const double epsilon);

static void solver_factorize_threaded(fem_matrix_t *mat,
				      const double epsilon,
				      int nb_threads);

static void solver_factorize_sequential(fem_matrix_t *mat, double epsilon);

void __fem_solver_factorize(fem_matrix_t *mat);

void __fem_solver_invert(const fem_matrix_t *mat, fem_vector_t *vec);

static void solver(fem_matrix_t *mat, fem_vector_t *vec, double epsilon,
		   int nb_threads);

fem_vector_t *fem_solver_threaded(fem_matrix_t *A, const fem_vector_t *b,
				  const double *nb_threads);

fem_vector_t *fem_solver(fem_matrix_t *A, const fem_vector_t *b);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 *
 */
static int solver_check(const fem_matrix_t *matrix, const fem_vector_t *vector)
{
	int status=0;
	
	if (matrix->size != vector->size)
	{
		status = -1;
		haplo_error("Matrix and vector's dimensions missmatch.");
	}

	return(status);
}


/**
 * Factorize a part of line #i. The lines 0 to j_max should be already
 * factorized.
 *
 * @param mat
 * @param i
 * @param j_min
 * @param j_max
 */
static void solver_line_factorize(fem_matrix_t *mat,
				  unsigned long i,
				  unsigned long j_min,
				  unsigned long j_max)
{
	const unsigned long first_i = FIRST(i);
	unsigned long j;
	double *Lij;

	if (first_i > j_min)
		j_min = first_i;

	Lij  = mat->coef + (j_min+1 + mat->diag[i]-i);
	for(j=j_min+1; j<j_max; j += 1)
	{
		const unsigned long first_j=FIRST(j);

		if (first_i > first_j)
			*Lij++ -= __fem_vector_dot_n(
				& L(i, first_i),
				& L(j, first_i),
				j-first_i);
		else
			*Lij++ -= __fem_vector_dot_n(
				& L(i, first_j),
				& L(j, first_j),
				j-first_j);
	}

	return;
}



/**
 * Batch factorize couple of lines.
 *
 * @param param describe what lines will be factorized.
 */
static void solver_factorize_threaded_task(const struct solver_s *param)
{
	solver_line_factorize(param->mat, param->i, 1, param->j_max);
	return;
}


/**
 * Compute final content of a line.
 *
 * @param mat is the matrix
 * @param i
 * @param epsilon is used to determine singularity
 */
static void solver_line_finish(fem_matrix_t *mat, unsigned long i,
			       const double epsilon)
{
	const unsigned long first_i = FIRST(i);
	double aii;
	unsigned long j;

	aii=D(i);
	for(j=first_i; j<i; j++)
	{
		const double aij = L(i,j);
		const double s   = aij/D(j);
		
			
		aii   -= s*aij;
		L(i,j) = s;
	}
		

	/* Test Singularity of D */
	if (fabs(aii) < epsilon)
	{
		aii = 1.0 / (epsilon*epsilon);
		haplo_warning(
			"Singularity on dof #%lu. Blocking.",
			i+1);
		mat->singularity = i+1;
	}
	D(i) = aii;

	return;
}


/**
 * LDLT factorization. Threaded version
 *
 * Algorithm:
 *
 * FOR I=2 TO N DO
 *	FOR J=2 TO I-1 DO
 *		FOR K=1 TO J-1 DO
 *			A(I,J)=A(I,J)-A(I,K)*A(J,K)
 *		DONE
 *	DONE
 * 	FOR J=1 TO I-1 DO
 *		S=A(I,J)/A(J,J)
 *		A(I,I)=A(I,I)-S*A(I,J)
 *		A(I,J)=S
 *	DONE
 * DONE
 *
 * @param mat is a pointer to a matrix
 * @param epsilon is the value used to determine singularity
 * @param n is the number of thread to use
 */
static void solver_factorize_threaded(fem_matrix_t *mat,
				      const double epsilon,
				      int nb_threads)
{
	unsigned long I;
	haplo_pool_t *pool;
	haplo_timer_t timer;
	struct solver_s *params;
	int nb_jobs = nb_threads * 2;
	

	haplo_info("THREADED LDLT (%d threads)", nb_threads);

	
	/*
	 * Allocating threads
	 */

	pool = haplo_pool_new(nb_jobs, nb_threads);

	HAPLO_ALLOC(params, nb_jobs);
	
	haplo_timer_start(&timer);


	for(I=1; I<mat->size; I += nb_jobs)
	{
		int id;
		/*
		 * Adjut number of thread
		 */
		if (I+nb_threads > mat->size)
			nb_jobs = mat->size-I;

		/*
		 * PARALLEL PART
		 */
		for(id=0; id<nb_jobs; id += 1)
		{
			params[id].mat    = mat;
			params[id].i      = I+id;
			params[id].j_max  = I;

			haplo_pool_run(pool,
				       (haplo_pool_fcn_t)solver_factorize_threaded_task,
				       params+id);
		}
		/* Wait for tasks to finish... */
		haplo_pool_wait(pool);

		/*
		 * SEQUENTIAL PART
		 */
		solver_line_finish(mat, I, epsilon);	
		for(id=1; id<nb_jobs; id += 1)
		{
			solver_line_factorize(mat, I+id, I-1, I+id);
			solver_line_finish(mat, I+id, epsilon);	
		}
	}

	haplo_timer_stop(&timer);
	haplo_timer_print(&timer);

	/*
	 * Factorization done. Clean up.
	 */
	haplo_pool_free(pool);
	HAPLO_FREE(params);
	
	mat->state=FEM_MATRIX_LDLT;

	return;
}


/**
 * LDLT factorization. Sequential version
 *
 * Algorithm:
 *
 * FOR I=2 TO N DO
 *	FOR J=2 TO I-1 DO
 *		FOR K=1 TO J-1 DO
 *			A(I,J)=A(I,J)-A(I,K)*A(J,K)
 *		DONE
 *	DONE
 * 	FOR J=1 TO I-1 DO
 *		S=A(I,J)/A(J,J)
 *		A(I,I)=A(I,I)-S*A(I,J)
 *		A(I,J)=S
 *	DONE
 * DONE
 *
 * @param mat is a pointer to a matrix
 * @param epsilon is the value used to determine singularity
 * @param n is the number of thread to use
 */
static void solver_factorize_sequential(fem_matrix_t *mat, double epsilon)
{
	unsigned long i;
	haplo_timer_t timer;

	haplo_info("SEQUENTIAL LDLT");
	haplo_timer_start(&timer);

	for(i=1; i<mat->size; i+=1)
	{
		solver_line_factorize(mat, i, 1, i);
		solver_line_finish(mat, i, epsilon);
	}

	haplo_timer_stop(&timer);
	haplo_timer_print(&timer);
	
	mat->state=FEM_MATRIX_LDLT;

	return;
}


/**
 *
 */
void __fem_solver_factorize(fem_matrix_t *mat)
{
	if (mat->state == FEM_MATRIX_ALLOCATED)
		solver_factorize_threaded(mat,
					  __fem_matrix_epsilon(mat), 2);
	
	return;
}



/**
 *
 */
void __fem_solver_invert(const fem_matrix_t *mat, fem_vector_t *vec)
{
	unsigned long i;

	for(i=1; i<mat->size; ++i)
	{	
		double s=0.0;
		const double  *aij=mat->coef+mat->diag[i-1]+1;
		unsigned long j;

		for(j=i-mat->diag[i]+mat->diag[i-1]+1; j<i; j++)
			s += (*aij++) * vec->coef[j];

		vec->coef[i] -= s;
	}
	
	for(i=0; i<mat->size; ++i)
		vec->coef[i] /= mat->coef[mat->diag[i]];
	
	for(i=mat->size-1; i>=1; --i)
	{
		const double vi=vec->coef[i];
		const double  *aij=mat->coef+mat->diag[i-1]+1;
		unsigned long j;

		for(j=i-mat->diag[i]+mat->diag[i-1]+1; j<i; j++)
			vec->coef[j] -= (*aij++) * vi;
		
	}

	return;
}


/**
 *
 */
static void solver(fem_matrix_t *mat, fem_vector_t *vec, double epsilon,
		   int nb_threads)
{


	switch(mat->state)
	{
	case FEM_MATRIX_INIT:
		haplo_fatal(
			"Call to fem_solver_solve() with non-allocated matrix"
			" (BUG?)");
		break;		

	case FEM_MATRIX_ALLOCATED:
		if (nb_threads > 1)
			solver_factorize_threaded  (mat, epsilon, nb_threads);
		else
			solver_factorize_sequential(mat, epsilon);
		break;

	case FEM_MATRIX_LDLT:
		break;
	}
	
	__fem_solver_invert(mat, vec);
	
	return;
}

/**
 *
 */
fem_vector_t *fem_solver_threaded(fem_matrix_t *A, const fem_vector_t *b,
				  const double *nb_threads)
{
	fem_vector_t *x=NULL;

	if (solver_check(A, b) == 0)
	{
		unsigned long i;
	
		x=fem_vector_new(A->size);
	
		for(i=0; i<x->size; i++)
			x->coef[i]=b->coef[i];
		
		solver(A,
		       x,
		       __fem_matrix_epsilon(A)*__fem_vector_norm(x),
		       haplo_uint(nb_threads));
	}
	
	return(x);
}


/**
 *
 */
fem_vector_t *fem_solver(fem_matrix_t *A, const fem_vector_t *b)
{
	double nb_threads=1;
	return fem_solver_threaded(A, b, &nb_threads);
}

