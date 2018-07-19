## Level0: behemoth0

Objective: Get the password to enter **behemoth1**.

### How did I get it?

1. I directly went to **/behemoth** directory, where I found 8 setuid executables and another file. Now, I have to exploit **behemoth0**, and be able to read the contents in **/etc/behemoth_pass/behemoth1** .

		behemoth1@behemoth:/behemoth$ ls -l behemoth0
		-r-sr-x--- 1 behemoth1 behemoth0 6700 May 10 22:07 behemoth0
		behemoth1@behemoth:/behemoth$ ls -l /etc/behemoth_pass/behemoth1
		-r-------- 1 behemoth1 behemoth1 11 May 10 22:06 /etc/behemoth_pass/behemoth1
		behemoth1@behemoth:/behemoth$ 

2. I ran **behemoth0** to see what it does.

		behemoth1@behemoth:/behemoth$ ./behemoth0
		Password: qwerty
		Access denied..
		behemoth1@behemoth:/behemoth$ 

* It takes in a password, checks / verifies it against something. If the wrong password is entered, it outputs **Access denied..**. 

3. Let us run it using **ltrace** and understand the working of **behemoth0**.

		behemoth1@behemoth:/behemoth$ ltrace ./behemoth0
		__libc_start_main(0x56555740, 1, 0xffffd734, 0x56555860 <unfinished ...>
		printf("Password: ")                             = 10
		__isoc99_scanf(0x5655592c, 0xffffd617, 0xf7fe3100, 0x56555758Password: qwerty
		) = 1
		strlen("OK^GSYBEX^Y")                            = 11
		strcmp("qwerty", "eatmyshorts")                  = 1
		puts("Access denied.."Access denied..
		)                          = 16
		+++ exited (status 0) +++
		behemoth1@behemoth:/behemoth$ 

* It asks for the password, and compares it with the string **eatmyshorts**. 

4. Let us give **eatmyshorts** as input and see what happens.

		behemoth1@behemoth:/behemoth$ ./behemoth0 
		Password: eatmyshorts
		Access granted..
		$ 
		$ id
		uid=13001(behemoth1) gid=13000(behemoth0) groups=13000(behemoth0)
		$ 
		$ cat /etc/behemoth_pass/behemoth1
		aesebootiv
		$ 

And **GAME OVER!**

### Password for behemoth1: aesebootiv

------------------

**PS**:

1. I was curious what that **strlen()** is doing there. I saw the disassembly of **behemoth0** and the program was not as simple as it looked above. So, I loaded it in gdb and ran it.
			
		behemoth1@behemoth:/behemoth$ gdb -q behemoth0
		Reading symbols from behemoth0...(no debugging symbols found)...done.
		(gdb) source /usr/local/peda/peda.py
		gdb-peda$ b main
		Breakpoint 1 at 0x750
		gdb-peda$ run


* I found the string used in **strlen** : Just before executing <main+51>
		
		gdb-peda$ x/s $ebp - 0x30
		0xffffd638:	"OK^GSYBEX^Y"
		gdb-peda$ 

* I also got to know that there were other strings stored / hardcoded in the program. 
		gdb-peda$ x/s 0x565558e0
		0x565558e0:	"unixisbetterthanwindows"
		gdb-peda$
		gdb-peda$ x/s 0x565558f8
		0x565558f8:	"followthewhiterabbit"
		gdb-peda$ 
		gdb-peda$ x/s 0x5655590d
		0x5655590d:	"pacmanishighoncrack"
		gdb-peda$ 
		 
* After taking in the user's input, it does not directly check with **eatmyshorts**. It first checks the length of `OK^GSYBEX^Y`. It's length = **11**.

* After that, a function named **memfrob** in this manner.
		
		memfrob(str, str_length);//str = "OK^GSYBEX^Y", str_length = 11

	* This is the **disassembly** of **memfrob** :
			
			Dump of assembler code for function memfrob:
		   0x56555710 <+0>:	push   ebp
		   0x56555711 <+1>:	mov    ebp,esp
		   0x56555713 <+3>:	call   0x5655584d <__x86.get_pc_thunk.ax>
		   0x56555718 <+8>:	add    eax,0x14c0
		   0x5655571d <+13>:	jmp    0x56555730 <memfrob+32>
		   0x5655571f <+15>:	mov    eax,DWORD PTR [ebp+0x8]
		   0x56555722 <+18>:	lea    edx,[eax+0x1]
		   0x56555725 <+21>:	mov    DWORD PTR [ebp+0x8],edx
		   0x56555728 <+24>:	movzx  edx,BYTE PTR [eax]
		   0x5655572b <+27>:	xor    edx,0x2a
		   0x5655572e <+30>:	mov    BYTE PTR [eax],dl
		   0x56555730 <+32>:	mov    eax,DWORD PTR [ebp+0xc]
		   0x56555733 <+35>:	lea    edx,[eax-0x1]
		   0x56555736 <+38>:	mov    DWORD PTR [ebp+0xc],edx
		   0x56555739 <+41>:	test   eax,eax
		   0x5655573b <+43>:	jg     0x5655571f <memfrob+15>
		   0x5655573d <+45>:	nop
		   0x5655573e <+46>:	pop    ebp
		   0x5655573f <+47>:	ret    
		End of assembler dump.
			
	* When the function is observed instruction by instruction, we come to know that **eatmyshorts** is generated from `OK^GSYBEX^Y` , when each of it's characters is **xored** with **0x2a**. Roughly, the function when written in C would look like this:
	
			void memfrob(char str, int str_length) {
				
				for(int i = 0; i <= str_length; i++) {
					str[i] = str[i] ^ 0x2a
				}
			}
	*  After control returned back to main, I verified that the  `OK^GSYBEX^Y` has got converted to **eatmyshorts**. 
	
			gdb-peda$ x/s 0xffffd638     
			0xffffd638:	"eatmyshorts"
			gdb-peda$ 

	* If you go up and see, you can verify that **0xffffd638** is the address of `OK^GSYBEX^Y` also. 

* The next part, we have seen it. Using **strcmp**, the string we entered is compared with the string that just got generated(**eatmyshorts**). 

2. I thought of going through it's disassembly when I did not find the string **eatmyshorts** in the executable. That is, it was not hardcoded anywhere. But, to confuse us, there were other strings which looked like the passwords. 
		
		unixisbetterthanwindows
		followthewhiterabbit
		pacmanishighoncrack

* There was no **eatmyshorts**. That is the reason I wanted to dig deep into the program to find what is actually happening. 

3. Just like an exercise, I have reverse engineered **behemoth0** completely and written a C program - **behemoth0.c**

		#include<stdio.h>
		#include<stdlib.h>
		#include<string.h>

		void memfrob(char *pass_gen_string, int pass_gen_string_length);

		int main() {
	
			char input_string[64];
			char pass_gen_string[] = "OK^GSYBEX^Y";
			char *str1 = "unixisbetterthanwindows";
			char *str2 = "followthewhiterabbit";
			char *str3 = "pacmanishighoncrack";

			int pass_gen_string_length;

			printf("Password: ");
			scanf("%64s", input_string);

			pass_gen_string_length = strlen(pass_gen_string);

			memfrob(pass_gen_string, pass_gen_string_length);

			if(strcmp(input_string, pass_gen_string) != 0) {
				printf("Access denied..\n");
				return 0;
		}

			printf("Access granted..\n");
			setreuid(geteuid(), geteuid());
			system("/bin/sh");

			return 0;
		}

		void memfrob(char *pass_gen_string, int pass_gen_string_length) {

			char *ptr;
			while(pass_gen_string_length-- > 0) {

				ptr = pass_gen_string;
				pass_gen_string++;
				(*ptr) = (*ptr) ^ 0x2a;
			}
		}
 

	 
