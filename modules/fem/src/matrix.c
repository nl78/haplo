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

#include <float.h>
#include <math.h>

#include <haplo.h>
#include <stdio.h>

#include "matrix.h"

#define UNROLL_LOOP

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

/*
 * Global matrices
 */
fem_matrix_t *fem_matrix_new(unsigned long size);
void fem_matrix_alloc(fem_matrix_t *A);
void fem_matrix_free(fem_matrix_t *A);
fem_matrix_t *fem_matrix_copy(const fem_matrix_t *A);
void fem_matrix_display(const fem_matrix_t *A);

fem_vector_t *fem_vector_new(unsigned long size);
void fem_vector_zeros(fem_vector_t *u);
void fem_vector_free(fem_vector_t *u);
void __fem_vector_copy(fem_vector_t *v, const fem_vector_t *u);
fem_vector_t *fem_vector_copy(const fem_vector_t *u);
void fem_vector_display(const fem_vector_t *u);
void fem_vector_print(const fem_vector_t *u);

void __fem_matrix_vector(const fem_matrix_t *A, const fem_vector_t *v,
			 fem_vector_t *res);
static void matrix_ldlt_vector(fem_vector_t *b, const fem_matrix_t *A,
			  const fem_vector_t *x);
fem_vector_t *fem_matrix_vector(const fem_matrix_t *A,
				const fem_vector_t *x);
fem_matrix_t *fem_matrix_test(void);
fem_matrix_t *fem_matrix_example(const haplo_float_t *size,
				 const haplo_float_t *percent);
fem_matrix_t *fem_matrix_example_10(const haplo_float_t *size);
fem_vector_t *fem_vector_example(const haplo_float_t *size);
fem_matrix_t *fem_matrix_id(const haplo_float_t *size);
void fem_vector_set(fem_vector_t *vector, const haplo_float_t *row,
		    const haplo_float_t *value);
haplo_float_t *fem_vector_get(const fem_vector_t *vector,
			      const haplo_float_t *row);
fem_vector_t *fem_vector_test(void);
fem_vector_t *fem_vector_add(const fem_vector_t *a, const fem_vector_t *b);
fem_vector_t *fem_vector_sub(const fem_vector_t *a, const fem_vector_t *b);
fem_vector_t *fem_vector_rmul(const fem_vector_t *a, const haplo_float_t *b);
fem_vector_t *fem_vector_lmul(const haplo_float_t *b, const fem_vector_t *a);
double __fem_vector_dot_n(const double *a, const double *b, unsigned long n);
double __fem_vector_dot(const fem_vector_t *u, const fem_vector_t *v);
void __fem_vector_div(fem_vector_t *vec, double alpha);
double __fem_vector_norm(const fem_vector_t *vec);
haplo_float_t *fem_vector_norm(const fem_vector_t *vec);
haplo_float_t *fem_vector_norm_max(const fem_vector_t *vec);
void fem_vector_normalize(fem_vector_t *vec);


/*
 * Boundary Condition
 */
void fem_matrix_penalty(fem_matrix_t *matrix, const double *dof);


/*
 * Internal matrices
 */
double **__fem_matrix_new(int row, int col);
void __fem_matrix_free(double **matrix, int row);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
fem_matrix_t *fem_matrix_new(unsigned long size)
{
	fem_matrix_t	*A;
	
	HAPLO_ALLOC(A, 1);
	HAPLO_ALLOC(A->diag, size);
	
	A->size=size;
	A->coef=NULL;
	A->state=FEM_MATRIX_INIT;
	A->singularity=0;

	return(A);
}


/**
 *
 */
void fem_matrix_alloc(fem_matrix_t *A)
{
	unsigned long i;
	
	HAPLO_ALLOC(A->coef, A->diag[A->size-1]+1);
	for(i=0; i<= A->diag[A->size-1]; i++)
		A->coef[i] = 0.0;
	
	A->state=FEM_MATRIX_ALLOCATED;
	return;
}


/**
 *
 */
void fem_matrix_free(fem_matrix_t *matrix)
{
	HAPLO_FREE(matrix->coef);
	HAPLO_FREE(matrix->diag);
	HAPLO_FREE(matrix);
	return;
}


/**
 *
 */
fem_matrix_t *fem_matrix_copy(const fem_matrix_t *A)
{
	fem_matrix_t	*B;
	unsigned long	i;
	
	B=fem_matrix_new(A->size);

	for(i=0; i<A->size; ++i)
		B->diag[i]=A->diag[i];

	fem_matrix_alloc(B);
	for(i=0; i<=A->diag[A->size-1]; ++i)
		B->coef[i]=A->coef[i];

	B->singularity=A->singularity;
	
	return(B);
}


/**
 *
 */
void fem_matrix_display(const fem_matrix_t *A)
{
	double full;
	double percent;
	
	full = (((double)A->size + 1.0)*(double)A->size/2.0);
	percent = ((double)A->diag[A->size-1] + 1.0) / full * 100.0;

	printf("Matrix (%lux%lu, %.1f%%%s)",
	       A->size, A->size, percent,
	       (A->state==FEM_MATRIX_LDLT)?", LDLT":"");

	return;
}


/**
 *
 */
void fem_matrix_print(const fem_matrix_t *A)
{
	unsigned long i;

	printf("( %+.2e ", A->coef[0]);
	for(i=1; i<A->size; ++i)
		printf("          ");
	printf(")\n");

	for(i=1; i<A->size; ++i)
	{
		unsigned j;

		printf("( ");
		for(j=i-(A->diag[i]-A->diag[i-1])+1; j>0; --j)
			fputs("          ", stdout);
		
		for(j=A->diag[i-1]+1; j<=A->diag[i]; j++)
			printf("%+.2e ", A->coef[j]);
		for(j=i+1; j<A->size; ++j)
			printf("          ");
		fputs(")\n", stdout);
	}
	return;
}


/**
 *
 */
void fem_matrix_print_profile(const fem_matrix_t *A)
{
	unsigned long i;

	
	fputs("( X", stdout);
	for(i=1; i<A->size; ++i)
		fputs(" ", stdout);
	fputs(" )\n", stdout);
	
	for(i=1; i<A->size; ++i)
	{
		unsigned j;

		fputs("( ", stdout);
		for(j=i-(A->diag[i]-A->diag[i-1])+1; j>0; --j)
			fputs(" ", stdout);
		
		for(j=A->diag[i-1]+1; j<=A->diag[i]; j++)
			fputc('X', stdout);

		for(j=i+1; j<A->size; ++j)
			fputc(' ', stdout);

		fputs(" )\n", stdout);
	}

	return;
}


/**
 *
 */
void fem_matrix_save(const char *filename, fem_matrix_t *matrix)
{
	FILE *fp;
	
	fp=fopen(filename, "wt");
	if (fp)
	{
		unsigned long i;

		fprintf(fp, "%lu\n", matrix->size);
		for(i=0; i<matrix->size; i++)
			fprintf(fp, "%lu\n", matrix->diag[i]);

		for(i=0; i <= matrix->diag[matrix->size-1]; i++)
			fprintf(fp, "%e\n", matrix->coef[i]);
		
		fclose(fp);
	} else
		haplo_error("Could not open `%s'", filename);

	return;
}


/**
 *
 */
fem_matrix_t *fem_matrix_load(const char *filename)
{
	FILE *fp;
	fem_matrix_t *matrix=NULL;

	fp=fopen(filename, "rt");
	if (fp)
	{
		char line[120];
		unsigned long size;
		unsigned long k;
		unsigned long kmax;
		fpos_t off;
		
		/* on passe les commentaires */
		while(fgets(line, 120, fp))
			if (line[0] != '%')
				break;
		
		sscanf(line, "%lu %lu %lu\n", &size, &k, &kmax);
		matrix=fem_matrix_new(size);
		
		fgetpos(fp, &off);
		
		
		/* Calcul de la hauteur de ciel */
		for(k=0; k<size; k += 1)
			matrix->diag[k]=0;
		
		for(k=0; k<kmax; k += 1) 
		{
			unsigned long i;
			unsigned long j;
			double Aij;
			
			fscanf(fp, "%lu %lu %lg\n", &i, &j, &Aij);
			
			/*printf("%lu %lu %lu\n", i, i-j, matrix->diag[i-1]);*/
			if ((i-j) > matrix->diag[i-1])
				matrix->diag[i-1] = i-j;
		}
		for(k=1; k<size; k += 1)
			matrix->diag[k] += matrix->diag[k-1]+1;
		/* Allocation */
		fem_matrix_alloc(matrix);
		
		/* Lecture des données */
		fsetpos(fp, &off);
		
		for(k=0; k<kmax; k += 1) 
		{
			unsigned long i;
			unsigned long j;
			double Aij;
			
			fscanf(fp, "%lu %lu %lg\n", &i, &j, &Aij);
			/*printf("%lu/%lu %lu %lu %e\n", k, kmax, i, j, Aij);*/
#define A(i,j) 	 matrix->coef[(j)+matrix->diag[(i)]-(i)]
			A(i-1,j-1) = Aij;
#undef A
		}
	} else
		haplo_error("Could not open `%s'", filename);
	
	
	return(matrix);
}


/**
 *
 */
fem_vector_t *fem_vector_new(unsigned long size)
{
	fem_vector_t	*u;

	HAPLO_ALLOC(u, 1);
	HAPLO_ALLOC(u->coef, size);
	
	u->size=size;
	
	return(u);
}


/**
 *
 */
void fem_vector_zeros(fem_vector_t *u)
{
	unsigned long i;
	
	for(i=0; i<u->size; i++)
		u->coef[i] = 0.0;

	return;
}


/**
 *
 */
void fem_vector_free(fem_vector_t *vector)
{
	HAPLO_FREE(vector->coef);
	HAPLO_FREE(vector);
	
	return;
}


/**
 *
 */
void __fem_vector_copy(fem_vector_t *v, const fem_vector_t *u)
{
	const unsigned long size=u->size;
	unsigned long i;
	
	for(i=0; i<size; i++)
		v->coef[i] = u->coef[i];
	
	return;
}


/**
 *
 */
fem_vector_t *fem_vector_copy(const fem_vector_t *u)
{
	fem_vector_t	*v;
	
	v=fem_vector_new(u->size);
	
	__fem_vector_copy(v, u);
	
	return(v);
}


/**
 *
 */
void fem_vector_display(const fem_vector_t *u)
{
	printf("Vector (%lu)", u->size);
	return;
}


/**
 *
 */
void fem_vector_print(const fem_vector_t *u)
{
	unsigned i;
	
	for(i=0; i<u->size; ++i)
		printf("[ %+.2e ]\n", u->coef[i]);
	
	return;
}


/**
 *
 */
void fem_vector_save(const char *filename, fem_vector_t *vector)
{
	FILE *fp;
	
	fp=fopen(filename, "wt");
	if (fp)
	{
		unsigned long i;

		fprintf(fp, "%lu\n", vector->size);
		for(i=0; i<vector->size; i++)
			fprintf(fp, "%e\n", vector->coef[i]);

		fclose(fp);
	} else
		haplo_error("Could not open `%s'", filename);

	return;
}


/**
 *
 */
fem_vector_t *fem_vector(const char *filename)
{
	FILE *fp;
	fem_vector_t *vector=NULL;

	fp=fopen(filename, "rt");
	if (fp)
	{
		unsigned long size;
		unsigned long i;
		
		fscanf(fp, "%lu\n", &size);
		vector=fem_vector_new(size);
		
		for(i=0; i<size; i++)
			fscanf(fp, "%le", vector->coef+i);
	} else
		haplo_error("Could not open `%s'", filename);
	
	
	return(vector);
}


/**
 *
 */
static void matrix_ldlt_vector(fem_vector_t *b, const fem_matrix_t *A,
			       const fem_vector_t *x)
{
	unsigned long	i;
	unsigned long	j;

	for(i=0; i<x->size; i++)
		b->coef[i]=x->coef[i];

	for(i=1; i<x->size; i++)
	{
		for(j=i+1-A->diag[i]+A->diag[i-1]; j<i; j++)
			b->coef[j] += A->coef[A->diag[i]-i+j]*x->coef[i];
	}

	for(i=0; i<x->size; i++)
		b->coef[i] *= A->coef[A->diag[i]];
	
	for(i=x->size-1; i>=1; i--)
		for(j=i+1-A->diag[i]+A->diag[i-1]; j<i; j++)
			b->coef[i] += A->coef[A->diag[i]-i+j]*b->coef[j];

	return;
}


/**
 *
 */
void __fem_matrix_vector(const fem_matrix_t *A, const fem_vector_t *v,
			 fem_vector_t *res)
{
	const unsigned long N=A->size;
	unsigned long i;

	res->coef[0]=v->coef[0]*A->coef[A->diag[0]];
	for(i=1; i<N; i++)
	{
		const double vi=v->coef[i];
		double resi=vi*A->coef[A->diag[i]];
		unsigned long j;

		for(j=i+1-A->diag[i]+A->diag[i-1]; j<i; j++)
		{
			const double Aij=A->coef[A->diag[i]-i+j];

			resi         += Aij*v->coef[j];
			res->coef[j] += Aij*vi;
		}
		res->coef[i] = resi;
	}

	return;
}


/**
 *
 */
fem_vector_t *fem_matrix_vector(const fem_matrix_t *A,
				const fem_vector_t *x)
{
	fem_vector_t 	*b;
	
	if (A->size != x->size)
	{
		haplo_error("Matrix and vector mismatches.");
		return(NULL);
	}
	b=fem_vector_new(x->size);
	
	switch(A->state)
	{
	case FEM_MATRIX_INIT:
		break;
	case FEM_MATRIX_ALLOCATED:
		__fem_matrix_vector(A, x, b);
		break;
	case FEM_MATRIX_LDLT:
		haplo_info("Matrix is already factorized. "
			   "Operation will be longer.");
		matrix_ldlt_vector(b, A, x);
		break;
	}
	
	return(b);
}


/**
 *
 */
fem_matrix_t *fem_matrix_test(void)
{
	fem_matrix_t	*A;

	A=fem_matrix_new(5);
	A->diag[0]=0;
	A->diag[1]=2;
	A->diag[2]=4;
	A->diag[3]=8;
	A->diag[4]=11;

	fem_matrix_alloc(A);
	A->coef[0]=1;
	A->coef[1]=-1.3;
	A->coef[2]=3.69;
	A->coef[3]=4.8;
	A->coef[4]=15.52;
	A->coef[5]=1.7;
	A->coef[6]=-2.21;
	A->coef[7]=6;
	A->coef[8]=12.89;
	A->coef[9]=1.6;
	A->coef[10]=0.0;
	A->coef[11]=11.4;

	return(A);
}


/**
 *
 */
fem_matrix_t *fem_matrix_example(const haplo_float_t *size,
				 const haplo_float_t *percent)
{
	fem_matrix_t	*A;
	unsigned long	i;
	unsigned long	d=0;
	double		p;

	p=haplo_clamp(percent, 0.0, 1.0);
	
	A=fem_matrix_new(haplo_uint(size));

	A->diag[0]=0;
	for(i=1; i<A->size; ++i)
	{
		/* Random profile p% */
		d += 1+(unsigned long)((double)i*p /* *rand()/(RAND_MAX+1.0)*/);
	
		/* Filled matrix */
		/* d += 1+i; */
				
		A->diag[i]=d;
	}
	
	fem_matrix_alloc(A);
	for(i=0; i<=A->diag[A->size-1]; ++i)
		A->coef[i]=1.0-2.0*(double)rand()/(RAND_MAX+1.0);
	
	/* Now, make the matrix definite positive .... */
	for(i=0; i<A->size; ++i)
		A->coef[A->diag[i]] += A->size*A->size; /**
			(0.5*((double)rand()/(RAND_MAX+1.0))+0.5);*/

	
	return(A);
}


/**
 *
 */
fem_matrix_t *fem_matrix_example_10(const haplo_float_t *size)
{
	const double percent=0.1;
	
	return(fem_matrix_example(size, &percent));
}


/**
 *
 */
fem_vector_t *fem_vector_example(const haplo_float_t *size)
{
	fem_vector_t	*u;
	unsigned long	i;
	
	u=fem_vector_new(haplo_uint(size));
	for(i=0; i<u->size; ++i)
		/*u->coef[i]=5.0-((double)rand()*10.0)/(RAND_MAX+1.0);*/
		u->coef[i]=1.0;
	
	return(u);
}


/**
 *
 */
fem_matrix_t *fem_matrix_id(const haplo_float_t *size)
{
	fem_matrix_t	*A;
	unsigned long	i;

	A=fem_matrix_new(haplo_uint(size));
	A->diag[0]=0;
	for(i=1; i<A->size; ++i)
		A->diag[i]=i;
	
	fem_matrix_alloc(A);
	for(i=0; i<=A->diag[A->size-1]; ++i)
		A->coef[i]=1.0;

	return(A);
}


/**
 *
 */
fem_vector_t *fem_vector_test(void)
{
	fem_vector_t	*u;
	unsigned long	i;
	
	u=fem_vector_new(5);

	for(i=0; i<u->size; ++i)
		u->coef[i]=1.0;

	return(u);
}


/**
 *
 */
void fem_vector_set(fem_vector_t *vector, const haplo_float_t *row,
		    const haplo_float_t *value)
{
	unsigned long i;
	
	i=haplo_ulong(row);
	
	if (i == 0)
		haplo_error("Indice begin at 1");
	else	
	{
		i -= 1 ;

		if (i < vector->size)
			vector->coef[i] = *value;
		else
			haplo_error("Indice out of range");
	}

	return;
}


/**
 *
 */
haplo_float_t *fem_vector_get(const fem_vector_t *vector,
			      const haplo_float_t *row)
{
	double *value=NULL;
	unsigned long i;
	
	i=haplo_ulong(row);
	if (i==0)
		haplo_error("Indice begin at 1");	
	else
	{
		i -= 1;
		if (i<vector->size)
			value = haplo_float(vector->coef[i]);
		else
			haplo_error("Value out of range");	
	}

	return(value);
}


/**
 *
 */
fem_vector_t *fem_vector_add(const fem_vector_t *a, const fem_vector_t *b)
{
	fem_vector_t	*vec;
	
	if (a->size == b->size)
	{
		unsigned long i;
		vec=fem_vector_new(a->size);
		for(i=0; i<a->size; i++)
			vec->coef[i]=a->coef[i]+b->coef[i];
	}
	else
		vec=NULL;
	
	return(vec);
}


/**
 *
 */
fem_vector_t *fem_vector_rmul(const fem_vector_t *a, const haplo_float_t *b)
{
	fem_vector_t	*vec;
	unsigned long i;
	const double coef=*b;
	
	vec=fem_vector_new(a->size);
	for(i=0; i<a->size; i++)
		vec->coef[i]=a->coef[i] * coef;
	
	return(vec);
	
};


/**
 *
 */
fem_vector_t *fem_vector_lmul(const haplo_float_t *b, const fem_vector_t *a)
{
	fem_vector_t	*vec;
	unsigned long i;
	const double coef=*b;
	
	vec=fem_vector_new(a->size);
	for(i=0; i<a->size; i++)
		vec->coef[i]=a->coef[i] * coef;
	
	return(vec);
	
};


/**
 *
 */
fem_vector_t *fem_vector_sub(const fem_vector_t *a, const fem_vector_t *b)
{
	fem_vector_t	*vec;
	
	if (a->size == b->size)
	{
		unsigned long i;
		vec=fem_vector_new(a->size);
		for(i=0; i<a->size; i++)
			vec->coef[i]=a->coef[i]-b->coef[i];
	}
	else
		vec=NULL;
	
	return(vec);
}


/**
 *
 */
double __fem_vector_dot_n(const double *a, const double *b, unsigned long n)
{
	unsigned long i;

#ifdef UNROLL_LOOP
	const unsigned long n4=(n >> 2) << 2;
	/*
	 * On déclare 4 variables pour éviter les dépendances
	 */
	register double res1 = 0.0;
	register double res2 = 0.0;
	register double res3 = 0.0;
	register double res4 = 0.0;

	/* 
	 * La boucle est déroulée 4 fois
	 */
	for(i=0; i<n4; i+=4)
	{
		res1 += a[i]   * b[i];
		res2 += a[i+1] * b[i+1];
		res3 += a[i+2] * b[i+2];
		res4 += a[i+3] * b[i+3];
	}

	/*
	 * On traite le résidu
	 */
	switch(n-n4)
	{
	case 3: res3 += a[n4+2] * b[n4+2];
	case 2: res2 += a[n4+1] * b[n4+1];
	case 1: res1 += a[n4]   * b[n4];
	}
       
	return (res1+res2)+(res3+res4);
	
#else
	double res = 0.0;
	for(i=0; i<n; i += 1)
		res += a[i] * b[i];

	return(res);
#endif	
}


/**
 *
 * vector should have the same size!
 */

double __fem_vector_dot(const fem_vector_t *u, const fem_vector_t *v)
{
	return(__fem_vector_dot_n(u->coef, v->coef, u->size));
}


/**
 *
 */
void __fem_vector_div(fem_vector_t *vec, double alpha)
{
	const unsigned long size=vec->size;
	unsigned long i;
	
	for(i=0; i<size; i++)
		vec->coef[i] /= alpha;
	
	return;
}
	

/**
 *
 */
double __fem_vector_norm(const fem_vector_t *vec)
{
	const unsigned long size=vec->size;
	unsigned long i;
	double norm=0.0;
	
	for(i=0; i<size; i++)
	{
		const double vi=vec->coef[i];
		
		norm += vi*vi;
	}

	norm=sqrt(norm);
	
	return(norm);
}


/**
 *
 */
void fem_vector_normalize(fem_vector_t *vec)
{
	double norm=__fem_vector_norm(vec);

	__fem_vector_div(vec, norm);

	return;
}


/**
 *
 */
haplo_float_t *fem_vector_norm(const fem_vector_t *vec)
{
	return(haplo_float(__fem_vector_norm(vec)));
}


/**
 *
 */
haplo_float_t *fem_vector_norm_max(const fem_vector_t *vec)
{
	const unsigned long size=vec->size;
	unsigned long i;
	double norm=0.0;
	
	for(i=0; i<size; i++)
	{
		const double vi=fabs(vec->coef[i]);
		
		if (vi > norm)
			norm=vi;
	}
	
	return(haplo_float(norm));
}


/**
 *
 */
void fem_matrix_penalty(fem_matrix_t *matrix, const double *dof)
{
	unsigned long i;

	i=haplo_ulong(dof);
	
	if (i==0)
		haplo_error("Indice begin at 1");
	else
	{
		i -= 1;

		if (i<matrix->size)
			matrix->coef[matrix->diag[i]] *= 1.0/DBL_EPSILON;
		else
			haplo_error("This dof is not defined");
	}

	return;
}


/**
 *
 */
double __fem_matrix_epsilon(const fem_matrix_t *matrix)
{
	double epsilon=0.0;
	unsigned long i;

	for(i=0; i<matrix->size; i++)
		epsilon += pow(matrix->coef[matrix->diag[i]],2);

	epsilon = sqrt(epsilon)*DBL_EPSILON;

	return(epsilon);
}



/**
 *
 */
double *fem_matrix_size(const fem_matrix_t *matrix)
{
	return(haplo_float(matrix->size));
}


/*
 * Internal matrices
 */


/**
 * Allocate a matrix
 */
double **__fem_matrix_new(int row, int col)
{
	double **matrix;
	int i;
	
	HAPLO_ALLOC(matrix, row);
	for(i=0; i<row; i++)
	{
		int j;
		
		HAPLO_ALLOC(matrix[i], col);

		for(j=0; j<col; j++)
			matrix[i][j]=0.0;
	}
	return(matrix);
}


/**
 * Free
 */
void __fem_matrix_free(double **matrix, int row)
{
	int i;
	
	for(i=0; i<row; i++)
		HAPLO_FREE(matrix[i]);

	HAPLO_FREE(matrix);

	return;
}
