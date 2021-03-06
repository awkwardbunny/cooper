#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char** argv){
	long lng = 136792;

	char* ptr = &lng;

	int i;
	long sum = 0;

	for (i = 0; i < sizeof(lng); i++){
		unsigned char c = *(ptr+i); //equivalent to ptr[i], but in bytes
		                            //it won't matter which i use since ptr is of char*
		long worth = (long)c << (8*i); //multiply by 256^i for correct value of digit
		//sum += worth; //accumulate
		printf("Content @ %#08x in hex: %x\tin decimal: %d\t worth: %d*16^%d = %ld\n", ptr+i, c, c, c, i, worth);
	}
	//printf("The sum is: %ld\n", sum);
}
