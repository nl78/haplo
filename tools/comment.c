#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define WIDTH	79

int main(int argc, char **argv)
{
	int i;
	int c;
	

	if (argc<2)
	{
		printf("USAGE: %s <comment>\n", argv[0]);
		return -1;
	}
	
	printf("/*");
	for(i=0; i<WIDTH-2; i++)
		printf("-");
	printf("\n");

	c=(WIDTH-strlen(argv[1])*2) / 2;
	
	for(i=0; i<c; i++)
		printf(" ");
	for(i=0; i<strlen(argv[1]); i++)
		printf("%c ", toupper(argv[1][i]));

	printf("\n");
	for(i=0; i<WIDTH-2; i++)
		printf("-");
	printf("*/\n");
	
	
}
