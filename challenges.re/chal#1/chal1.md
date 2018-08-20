## Reverse Engineering Challenge #1 

Question: 
> Tags: .
What does this code do? The function has 4 arguments and it is compiled by GCC for Linux x64 ABI (i.e., arguments are passed in registers).

	<f>:
	   0:                   mov    r8,rdi
	   3:                   push   rbx
	   4:                   mov    rdi,rsi
	   7:                   mov    rbx,rdx
	   a:                   mov    rsi,r8
	   d:                   xor    rdx,rdx

	begin:
	  10:                   lods   rax,QWORD PTR ds:[rsi]
	  12:                   div    rbx
	  15:                   stos   QWORD PTR es:[rdi],rax
	  17:                   loop   begin
	  19:                   pop    rbx
	  1a:                   mov    rax,rdx
	  1d:                   ret

## Solution: 

1. First thing to note is this code is compiled for **Linux x64** ABI. This means, there is a specific manner in which arguments are passed to a function. 

2. In Linux x64 ABI, Arguments are passed in this manner: 
* First 6 arguments in the registers **rdi**, **rsi**, **rdx**, **rcx**, **r8** and **r9** in order and if there are more arguments, the rest of them are pushed onto the stack. 

3. The question says, there are 4 arguments passed. So, they should be present in **rdi**, **rsi**, **rdx** and **rcx** . 

4. Before going for deep analysis, let us glance through the function. 

* 4 Arguments. 
* **begin** is a label. 
* **lods** (Load String) is used which means **rsi** (second argument of the  function) should be an address. 
* **stos** (Store String) is used which means **rdi** (third argument of the function) should be an address.  
* There is a **loop** instruction, which means **rcx** is the counter variable for that loop. 
* Content in rdx is being moved to rax at the end, which means it is the return value. 

### Deep Analysis: 

f:
* Lines **0** , **4** and **a** show that contents of **rsi** and **rdi** are swapped. 
* Third argument - **rdx** is moved to **rbx**. 
* Then, **rdx** is set to 0 (xor rdx, rdx)
* **rcx** is untouched . 

begin: 

* **lods rax, qword ptr ds:[rsi]** : 
	* **lods** stands for Load String. Here qword is used. So, 8 bytes from address ds:[rsi] is copied / loaded into rax. 
	* After loading the value, the value of **rsi** is incremented by 8 . 

* **div rbx** : 
	* This instruction does unsigned division. This instruction is equivalent to 
		
			rdx = rax % rbx;	Remainder
			rax = rax / rbx;	Quotient
	
* **stos qword ptr es:[rdi], rax** : 
	* **stos** stands for Store String. Content in rax is stored into Address es:[rdi] . 
	* So, the Quotient of the division is stored into es:[rdi] . 
	* After storing into the string, **rdi** is incremented by 8. 

* **loop begin** : This is equivalent to 
		
		dec rcx
		cmp rcx, 0
		jnz begin
	
* So, this will loop till rcx touches 0. Then the content of rdx is copied into rax and **ret** is executed => Content of **rdx** is the return value. 

### What could this piece of code be: 

* This part took the most time to figure out. 
* Overview: There are 2 pointers. 8 bytes of data is taken from pointer, divided by a divisor and the quotient is loaded into memory of the second pointer. Then, both pointers are incremented. 
* This could possibly be some operation on large numbers that are stored as strings. 
* This is the C function f() which does the **same job** as the given assembly code but does not give the same assembly code when compiled. 

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
		
* This is the [Link](./chal1.c) to the fully working C program. You can download it and play around with it.

----------------------------------

**NOTE** : 

1. The question says the given assembly code is obtained by compiling using GCC. But, I have never seen GCC using such complex instructions like lods and stos. Generally, simple instructions are used by the compiler, although it depends on how the compiler is configured. 




