/*
 * This is the C equivalent of this assembly code: https://challenges.re/1/
 * Task is to reverse engineer it and make some sense out of it. 
 * So, writing a C program to get a high-level view of assembly code. 
 * 
 * NOTE: If you compile this program, you won't get a function whose assembly code is function f(). 
 * Instead, you get a function which works just like function f(). 
 */


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

unsigned long int f(char *num, char *quotient, unsigned long int divisor, unsigned long int num_len);

int main() {

	char *num, *quotient;
	unsigned long int num_len, divisor;

	printf("\nEnter the number of digits in the number: ");
	scanf("%lu", &num_len);

	num = (char *)malloc(sizeof(char) * num_len);
	quotient = (char *)malloc(sizeof(char) * num_len);

	if(num_len % 8 == 0)
		num_len = num_len / 8;
	else
		num_len = num_len / 8 + 1;

	printf("Enter the number: ");
	scanf("%s", num);
	
	printf("Enter the divisor: ");
	scanf("%lu", &divisor);

	f(num, quotient, divisor, num_len);

	printf("\nQuotient = %s\n", quotient);

	return 0;
}


//This function f() does the same job as the given assembly code but does not compile to give the assembly code. 

unsigned long int f(char *num, char *quotient, unsigned long int divisor, unsigned long int num_len) {

	char number[8], quo[8];
	unsigned long int q, rem;

	while(num_len != 0) {

		//Equivalent to lods instruction. 
		snprintf(number, 9, "%s", num);
		num += 8;

		//Equivalent to div instruction. 
		q = atoi(number) / divisor;
		rem = atoi(number) % divisor;

		printf("DBG: number = %s, q = %lu, rem = %lu\n", number, q, rem);

		//Equivalent to stos instruction. 
		snprintf(quotient, 8, "%lu", q);
		quotient += 8;

		num_len--;
	}

	return rem;
}

