#include <stdio.h>
#include <stdlib.h>
#include "two-a.h"

int main(int argc, char **argv){
	char buffer[12];
//	char b2[12];

	unsigned int i = 2147000000;
	do{
//		printf("%d: %s\n", i, my_itoa(i, buffer, 10));
		if(atoi(my_itoa(i, buffer, 10)) != i)
			break;
	}while(++i);

	if(i)
		printf("Error\n");
	else
		printf("WORKS!\n");
}
