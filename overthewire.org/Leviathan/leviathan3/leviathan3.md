## Level3: leviathan3

Objective: Find the password to enter level4 **leviathan4**.

### This is how I found it:

1. There is a setuid executable **level3** present. It is setuid to **leviathan4**. The basic route is clear. Escalate privileges, and then read the contents of **/etc/leviathan_pass/leviathan4**. 
		
		leviathan3@leviathan:~$ ls
		level3
		leviathan3@leviathan:~$ ls -l
		total 12
		-r-sr-x--- 1 leviathan4 leviathan3 10488 May 10 18:27 level3
		leviathan3@leviathan:~$ 


2. Let us run **level3** and see what it does.
			
		leviathan3@leviathan:~$ ./level3
		Enter the password> qwerty
		bzzzzzzzzap. WRONG
		leviathan3@leviathan:~$ 

3. It is asking for a password, which it verifies in some manner. If the password is wrong, it prints **bzzzzzzzzap. WRONG**. 

4. Let us run this with **ltrace** and checkout the internal working of **level3**.
				
		leviathan3@leviathan:~$ ltrace ./level3
		__libc_start_main(0x565557b4, 1, 0xffffd744, 0x56555870 <unfinished ...>
		strcmp("h0no33", "kakaka")                       = -1
		printf("Enter the password> ")                   = 20
		fgets(Enter the password> qwerty
		"qwerty\n", 256, 0xf7fc55a0)               = 0xffffd550
		strcmp("qwerty\n", "snlprintf\n")                = -1
		puts("bzzzzzzzzap. WRONG"bzzzzzzzzap. WRONG
		)                       = 19
		+++ exited (status 0) +++
		leviathan3@leviathan:~$ 

* First, it executes **strcmp("h0no33", "kakaka")**. And it seems it's result is not being elsewhere. For now let us ignore it and let us focus on the password part.

* It asks for a password using **fgets** function: One thing to note about this function is, it appends a **\n** or **newline** character at the end of string entered. Here the buffer length = 256. If you enter a string of 256 characters, it takes in only the first 255 characters and appends a '\n' to it.

* Look at the next **strcmp**
			
		strcmp("qwerty\n", "snlprintf\n")
	
	* The entered string is compared with the string: **snlprintf**.  This is how the password is being checked. 
	
* The obvious thing to do is, run it with input string = **snlprintf**.

		leviathan3@leviathan:~$ ltrace ./level3
		__libc_start_main(0x565557b4, 1, 0xffffd744, 0x56555870 <unfinished ...>
		strcmp("h0no33", "kakaka")                       = -1
		printf("Enter the password> ")                   = 20
		fgets(Enter the password> snlprintf
		"snlprintf\n", 256, 0xf7fc55a0)            = 0xffffd550
		strcmp("snlprintf\n", "snlprintf\n")             = 0
		puts("[You've got shell]!"[You've got shell]!
		)                      = 20
		geteuid()                                        = 12003
		geteuid()                                        = 12003
		setreuid(12003, 12003)                           = 0
		system("/bin/sh"$ 
		$ 
		$ whoami
		leviathan3
		$ 
* There you go! and we got what we wanted, a shell!

* But when running it with ltrace, privilege is not escalated. 

* Let us run it normally and get that password.

		leviathan3@leviathan:~$ ./level3 
		Enter the password> snlprintf
		[You've got shell]!
		$ 
		$ id
		uid=12004(leviathan4) gid=12003(leviathan3) groups=12003(leviathan3)
		$ 
		$ cat /etc/leviathan_pass/leviathan4
		vuH0coox6m
		$ 
And **GAME OVER!**

### Password for leviathan4: vuH0coox6m


