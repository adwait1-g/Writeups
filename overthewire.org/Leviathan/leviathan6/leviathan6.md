## Level6: leviathan6

Objective: Find the password to enter level7 **leviathan7**.

### This is how I found it:

1. There is a setuid executable **leviathan6** in the home directory. I ran it to see what it does.

		leviathan6@leviathan:~$ ./leviathan6
		usage: ./leviathan6 <4 digit code>
		leviathan6@leviathan:~$ 

2. It mostly takes in a particular 4-digit number to execute commands at escalated privileges. The first think which came to mind is **brute forcing**. There are only **9000** 4-digit numbers. A simple script would do it in no time. This is a python script I wrote to brute force it.
		
		leviathan6@leviathan:~$ cat /tmp/brute_force.py
		#!/usr/bin/python

		import os

		i = 1000

		while i <= 9999 :
	    print i
	    exec_string = "~/leviathan6 " + str(i)
	    os.system(exec_string)
	    i = i + 1

		leviathan6@leviathan:~$ 

* It is a simple script which starts checking from **1000** and goes till **9999**. And for every 4-digit number, it executes the executable to check if that is the 4-digit code the program is looking for. 

* Got the shell for **7123**.

		7119
		Wrong
		7120
		Wrong
		7121
		Wrong
		7122
		Wrong
		7123
		$ 
		$ 
		$ id   
		uid=12007(leviathan7) gid=12006(leviathan6) groups=12006(leviathan6)
		$ 
		$ cat /etc/leviathan_pass/leviathan7
		ahy7MaeBo9
		$ 

And **GAME OVER!**

### Password for leviathan7: ahy7MaeBo9

--------------

**PS** :

1. Brute forcing is a good method for smaller ranges. Nowadays, even 32-bit numbers can be bruteforced easily. 

2. For this question, there is another definitive and elegant method to find out that 4-digit code. 

* This is the ltrace output. 
		
		leviathan6@leviathan:~$ ltrace ./leviathan6 1234
		__libc_start_main(0x565556c0, 2, 0xffffd724, 0x56555780 <unfinished ...>
		atoi(0xffffd864, 0xffffd724, 0xf7ffcd00, 0x565556d8) = 1234
		puts("Wrong"Wrong
		)                                    = 6
		+++ exited (status 0) +++
		leviathan6@leviathan:~$ 

* **atoi** is being used. So, I guess that this number we enter and the actual passcode are being compared. 

* I took a look at the disassembly of **main** function just to check if the 4-digit code was **hardcoded** in the program. This is the disassembly of main.

		000006c0 <main>:
		 6c0:	8d 4c 24 04          	lea    ecx,[esp+0x4]
		 6c4:	83 e4 f0             	and    esp,0xfffffff0
		 6c7:	ff 71 fc             	push   DWORD PTR [ecx-0x4]
		 6ca:	55                   	push   ebp
		 6cb:	89 e5                	mov    ebp,esp
		 6cd:	56                   	push   esi
		 6ce:	53                   	push   ebx
		 6cf:	51                   	push   ecx
		 6d0:	83 ec 1c             	sub    esp,0x1c
		 6d3:	e8 b8 fe ff ff       	call   590 <__x86.get_pc_thunk.bx>
		 6d8:	81 c3 28 19 00 00    	add    ebx,0x1928
		 6de:	89 c8                	mov    eax,ecx
		 6e0:	c7 45 e4 d3 1b 00 00 	mov    DWORD PTR [ebp-0x1c],0x1bd3
		 6e7:	83 38 02             	cmp    DWORD PTR [eax],0x2
		 6ea:	74 22                	je     70e <main+0x4e>
		 6ec:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
		 6ef:	8b 00                	mov    eax,DWORD PTR [eax]
		 6f1:	83 ec 08             	sub    esp,0x8
		 6f4:	50                   	push   eax
		 6f5:	8d 83 00 e8 ff ff    	lea    eax,[ebx-0x1800]
		 6fb:	50                   	push   eax
		 6fc:	e8 bf fd ff ff       	call   4c0 <printf@plt>
		 701:	83 c4 10             	add    esp,0x10
		 704:	83 ec 0c             	sub    esp,0xc
		 707:	6a ff                	push   0xffffffff
		 709:	e8 f2 fd ff ff       	call   500 <exit@plt>
		 70e:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
		 711:	83 c0 04             	add    eax,0x4
		 714:	8b 00                	mov    eax,DWORD PTR [eax]
		 716:	83 ec 0c             	sub    esp,0xc
		 719:	50                   	push   eax
		 71a:	e8 11 fe ff ff       	call   530 <atoi@plt>
		 71f:	83 c4 10             	add    esp,0x10
		 722:	3b 45 e4             	cmp    eax,DWORD PTR [ebp-0x1c]
		 725:	75 2d                	jne    754 <main+0x94>
		 727:	e8 a4 fd ff ff       	call   4d0 <geteuid@plt>
		 72c:	89 c6                	mov    esi,eax
		 72e:	e8 9d fd ff ff       	call   4d0 <geteuid@plt>
		 733:	83 ec 08             	sub    esp,0x8
		 736:	56                   	push   esi
		 737:	50                   	push   eax
		 738:	e8 d3 fd ff ff       	call   510 <setreuid@plt>
		 73d:	83 c4 10             	add    esp,0x10
		 740:	83 ec 0c             	sub    esp,0xc
		 743:	8d 83 1a e8 ff ff    	lea    eax,[ebx-0x17e6]
		 749:	50                   	push   eax
		 74a:	e8 a1 fd ff ff       	call   4f0 <system@plt>
		 74f:	83 c4 10             	add    esp,0x10
		 752:	eb 12                	jmp    766 <main+0xa6>
		 754:	83 ec 0c             	sub    esp,0xc
		 757:	8d 83 22 e8 ff ff    	lea    eax,[ebx-0x17de]
		 75d:	50                   	push   eax
		 75e:	e8 7d fd ff ff       	call   4e0 <puts@plt>
		 763:	83 c4 10             	add    esp,0x10
		 766:	b8 00 00 00 00       	mov    eax,0x0
		 76b:	8d 65 f4             	lea    esp,[ebp-0xc]
		 76e:	59                   	pop    ecx
		 76f:	5b                   	pop    ebx
		 770:	5e                   	pop    esi
		 771:	5d                   	pop    ebp
		 772:	8d 61 fc             	lea    esp,[ecx-0x4]
		 775:	c3                   	ret    
 
 * I first spotted the **atoi** function, and checked if there was a **cmp** to compare it's output with actual passcode. Take a look at address **0x722**.
				
		71a:	e8 11 fe ff ff       	call   530 <atoi@plt>
		71f:	83 c4 10             	add    esp,0x10
		722:	3b 45 e4             	cmp    eax,DWORD PTR [ebp-0x1c]

* The return value of **atoi** is stored in **eax**. That is being checked against some value present at **$ebp - 0x1c**. 

* I checked the disassembly for **$ebp - 0x1c** and spotted this.  
		 
		6e0:	c7 45 e4 d3 1b 00 00 	mov    DWORD PTR [ebp-0x1c],0x1bd3

* This is straight forwarded **hardcoding**. **0x1bd3** is the hexadecimal of **7123**. 

And we got it!

2. Very rarely, passcodes are hardcoded. They are sometimes dynamically generated by a function. If the number of digits in the passcode is high, I would prefer to reverse engineer that function to get the passcode generating mechanism than simply bruteforcing. 

3. Depending on the situation, we have to decide to use bruteforcing / reverse engineering. 
  
