#include <stdio.h>

char ar[10];

int main(){

	//initialize array from 20 to 29
	int i = 0;
	while(i < 10)
		// http://en.cppreference.com/w/cpp/language/eval_order
		ar[i++] = 19+i;

	//assign pointer
	char *ptr = &ar[3];

	printf("###############################################\n");

	printf("Address of 0th element in ar: %#010x\n", &ar[0]);
	printf("Address of 3rd element is ar: %#010x\n", &ar[3]);
	printf("Address of ptr: %#010x\n", &ptr);
	printf("Address held in ptr: %#010x\n", ptr);
	printf("Content of ar[3]: %d\n", ar[3]);
	printf("Content of address held in ptr: %d\n", *ptr);

	//pointers converted to void* to get length in bytes
	int size = ((void *)&ar[3] - (void *)&ar[0])/(3-0);
	printf("Size of char is: %d\n", size);

	printf("###############################################\n");
}
