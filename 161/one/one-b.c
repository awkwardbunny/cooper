#include <stdio.h>

long ar[10];

int main(){

	//initialize array
	int i = 0;
	while(i < 10)
		// http://en.cppreference.com/w/cpp/language/eval_order
		ar[i++] = 30+i;

	long *ptr = &ar[3];

	printf("###############################################\n");

	printf("Address of 0th element in ar: %#010x\n", &ar[0]);
	printf("Address of 3rd element is ar: %#010x\n", &ar[3]);
	printf("Address of ptr: %#010x\n", &ptr);
	printf("Address held in ptr: %#010x\n", ptr);
	printf("Content of ar[3]: %d\n", ar[3]);
	printf("Content of address held in ptr: %d\n", *ptr);

	//convert to void* to get size in bytes
	int size = ((void *)&ar[3] - (void *)&ar[0])/(3-0);
	printf("Size of long is: %d bytes\n", size);

	printf("###############################################\n");
}
