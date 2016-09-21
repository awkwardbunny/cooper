#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include "two-a.h"

int main(int argc, char **argv){
	char buffer[12];
//	char b2[12];

///	unsigned int i = 2147000000;
///	do{
///		printf("%d: %s\n", i, my_itoa(i, buffer, 10));
//		if(strcmp(my_itoa(i, buffer, 10), (b2)))
//			break;
///	}while(++i);

//	if(i)
//		printf("Error\n");
//	else
//		printf("WORKS!\n");

	printf("printf: %d\t my_itoa: %s\n", 1232, my_itoa(1232, buffer, 10));
	printf("printf: %d\t my_itoa: %s\n", 21308, my_itoa(21308, buffer, 10));
	printf("printf: %x\t my_itoa: %s\n", 12345, my_itoa(12345, buffer, 16));
	printf("printf: %x\t my_itoa: %s\n", 98327, my_itoa(98327, buffer, 16));
	printf("printf: %o\t my_itoa: %s\n", 92945, my_itoa(92945, buffer, 8));
	printf("printf: %o\t my_itoa: %s\n", 3785, my_itoa(3785, buffer, 8));
	printf("printf: %d\t my_itoa: %s\n", -6123, my_itoa(-6123, buffer, 10));

}
