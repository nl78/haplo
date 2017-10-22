#include <stdio.h>

void proto(const char *x)
{
	printf("__haplo_function(%s)\n", x);
	printf("extern fem_vector_t * fem_vectorized_%s"
	       "(const fem_vector_t *arg);\n\n", x);
	
	return;
}


int main(void)
{
	const char *funclist[]={
		"acos",
		"asin",
		"atan",
		"cos",
		"cosh",
		"exp",
		"log",
		"log10",
		"sin",
		"sinh",
		"sqrt",
		"tan",
		"tanh",
		"fabs",
		"acosh",
		"asinh",
		"atanh",
		"cbrt",
		"erf",
		"erfc",
		"expm1",
		"j0",
		"j1",
		"lgamma",
		"log1p",
		"logb",
		"y0",
		"y1",
		NULL
	};
	int i;
	
	for(i=0; funclist[i]; i+=1)
		proto(funclist[i]);
	
	return 0;
}

