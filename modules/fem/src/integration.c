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
 * Numerical integration schemes.
 */

#include <stdlib.h>

#include "integration.h"


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

/*
 * NONE
 */
static const fem_integration_t integration_none = {
	0,
	NULL,
	NULL
};


/*
 * L1
 */
#define L1_X1 0.0
static const double l1_x1[]={
	L1_X1
};
static const double *l1_x[]={
	l1_x1
};
static const double l1_w[]={
	2.0
};
static const fem_integration_t integration_l1 = {
	1,
	l1_x,
	l1_w
};


/*
 * L2
 */
#define L2_X1	-0.577350269189626
#define L2_X2	 0.577350269189626
static const double l2_x1[]={
	L2_X1
};
static const double l2_x2[]={
	L2_X2
};
static const double *l2_x[]={
	l2_x1,
	l2_x2
};
static const double l2_w[]={
	1.0,
	1.0
};
static const fem_integration_t integration_l2 = {
	2,
	l2_x,
	l2_w
};


/*
 * L3
 */
#define L3_X1	 0.0
#define L3_X2	-0.774596669241483
#define L3_X3	 0.774596669241483
static const double l3_x1[]={
	L3_X1
};
static const double l3_x2[]={
	L3_X2
};
static const double l3_x3[]={
	L3_X3
};
static const double *l3_x[]={
	l3_x1,
	l3_x2,
	l3_x3
};
static const double l3_w[]={
	0.888888888888889,
	0.555555555555556,
	0.555555555555556
};
static const fem_integration_t integration_l3 = {
	3,
	l3_x,
	l3_w
};


/*
 * L4
 */
#define L4_X1	-0.339981043584856
#define L4_X2	 0.339981043584856
#define L4_X3	-0.861136311594053
#define L4_X4	 0.861136311594053
static const double l4_x1[]={
	L4_X1
};
static const double l4_x2[]={
	L4_X2
};
static const double l4_x3[]={
	L4_X3
};
static const double l4_x4[]={
	L4_X4
};
static const double *l4_x[]={
	l4_x1,
	l4_x2,
	l4_x3,
	l4_x4
};
static const double l4_w[]={
	0.652145154862546,
	0.652145154862546,
	0.347854845137454,
	0.347854845137454
};
static const fem_integration_t integration_l4 = {
	4,
	l4_x,
	l4_w
};


/*
 * L5
 */
#define L5_X1	 0.0
#define L5_X2	-0.538469310105683
#define L5_X3	 0.538469310105683
#define L5_X4	-0.906179845938664
#define L5_X5	 0.906179845938664
static const double l5_x1[]={
	L5_X1
};
static const double l5_x2[]={
	L5_X2
};
static const double l5_x3[]={
	L5_X3
};
static const double l5_x4[]={
	L5_X4
};
static const double l5_x5[]={
	L5_X5
};
static const double *l5_x[]={
	l5_x1,
	l5_x2,
	l5_x3,
	l5_x4,
	l5_x5
};
static const double l5_w[]={
	0.568888888888889,
	0.478628670499366,
	0.478628670499366,
	0.236926885056189,
	0.236926885056189
};
static const fem_integration_t integration_l5 = {
	5,
	l5_x,
	l5_w
};


/*
 * L6
 */
#define L6_X1	-0.238619186083197
#define L6_X2	 0.238619186083197
#define L6_X3	-0.661209386466265
#define L6_X4	 0.661209386466265
#define L6_X5	-0.932469514203152
#define L6_X6	 0.932469514203152
static const double l6_x1[]={
	L6_X1
};
static const double l6_x2[]={
	L6_X2
};
static const double l6_x3[]={
	L6_X3
};
static const double l6_x4[]={
	L6_X4
};
static const double l6_x5[]={
	L6_X5
};
static const double l6_x6[]={
	L6_X6
};
static const double *l6_x[]={
	l6_x1,
	l6_x2,
	l6_x3,
	l6_x4,
	l6_x5,
	l6_x6
};
static const double l6_w[]={
	0.467913934572691,
	0.467913934572691,
	0.360761573048139,
	0.360761573048139,
	0.171324492379170,
	0.171324492379170
};
static const fem_integration_t integration_l6 = {
	6,
	l6_x,
	l6_w
};


/*
 * L7
 */
#define L7_X1	 0.0 
#define L7_X2	-0.405845151377397
#define L7_X3	 0.405845151377397
#define L7_X4   -0.741531185599394
#define L7_X5	 0.741531185599394
#define L7_X6	-0.949107912342759
#define L7_X7	 0.949107912342759
static const double l7_x1[]={
	L7_X1
};
static const double l7_x2[]={
	L7_X2
};
static const double l7_x3[]={
	L7_X3
};
static const double l7_x4[]={
	L7_X4
};
static const double l7_x5[]={
	L7_X5
};
static const double l7_x6[]={
	L7_X6
};
static const double l7_x7[]={
	L7_X7
};
static const double *l7_x[]={
	l7_x1,
	l7_x2,
	l7_x3,
	l7_x4,
	l7_x5,
	l7_x6,
	l7_x7
};
static const double l7_w[]={
	0.417959183673469,
	0.381830050505119,
	0.381830050505119,
	0.279705391489277,
	0.279705391489277,
	0.129484966168870,
	0.129484966168870
};
static const fem_integration_t integration_l7 = {
	7,
	l7_x,
	l7_w
};


/*
 * T1
 */
#define T1_X1	0.333333333333333, 0.333333333333333
static const double t1_x1[]={
	T1_X1
};
static const double *t1_x[]={
	t1_x1
};
static const double t1_w[]={
	0.5
};
static const fem_integration_t integration_t1 = {
	1,
	t1_x,
	t1_w
};


/*
 * T3
 */
#define T3_X1	0.5, 0.5
#define T3_X2	0.0, 0.5
#define T3_X3	0.5, 0.0
static const double t3_x1[]={
	T3_X1
};
static const double t3_x2[]={
	T3_X2
};
static const double t3_x3[]={
	T3_X3
};
static const double *t3_x[]={
	t3_x1,
	t3_x2,
	t3_x3
};
static const double t3_w[]={
	0.166666666666667,
	0.166666666666667,
	0.166666666666667	
};
static const fem_integration_t integration_t3 = {
	3,
	t3_x,
	t3_w
};

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/
const fem_integration_t *__fem_integration(fem_integration_scheme_t scheme);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
const fem_integration_t *fem_integration(fem_integration_scheme_t scheme)
{
	const fem_integration_t *result=NULL;
	
	switch(scheme)
	{
	case FEM_INTEGRATION_INVALID:
	case FEM_INTEGRATION_NONE:
		result=&integration_none;
		break;
	case FEM_INTEGRATION_L1:
		result=&integration_l1;
		break;
	case FEM_INTEGRATION_L2:
		result=&integration_l2;
		break;
	case FEM_INTEGRATION_L3:
		result=&integration_l3;
		break;
	case FEM_INTEGRATION_L4:
		result=&integration_l4;
		break;
	case FEM_INTEGRATION_L5:
		result=&integration_l5;
		break;
	case FEM_INTEGRATION_L6:
		result=&integration_l6;
		break;
	case FEM_INTEGRATION_L7:
		result=&integration_l7;
		break;
	case FEM_INTEGRATION_T1:
		result=&integration_t1;
		break;
		
	case FEM_INTEGRATION_T3:
		result=&integration_t3;
		break;
	}
	
	return(result);
}
