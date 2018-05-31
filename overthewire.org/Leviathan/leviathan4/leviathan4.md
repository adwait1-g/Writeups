## Level5: leviathan5

Objective: Find the password to enter level5 **leviathan5**.

### This is how I found it:

1. `$ ls -al` showed that there is a directory named **.trash**. 

		leviathan4@leviathan:~$ ls -al
		total 24
		drwxr-xr-x  3 root root       4096 May 10 18:27 .
		drwxr-xr-x 10 root root       4096 May 10 18:27 ..
		-rw-r--r--  1 root root        220 May 15  2017 .bash_logout
		-rw-r--r--  1 root root       3526 May 15  2017 .bashrc
		-rw-r--r--  1 root root        675 May 15  2017 .profile
		dr-xr-x---  2 root leviathan4 4096 May 10 18:27 .trash

2. And there it was, **bin** the setuid executable. I ran it to see what it does.

		leviathan4@leviathan:~/.trash$ ls
		bin
		leviathan4@leviathan:~/.trash$ ./bin
		01010100 01101001 01110100 01101000 00110100 01100011 01101111 01101011 01100101 01101001 00001010 
		leviathan4@leviathan:~/.trash$ 

3. It prints 11 **8-bit** binary numbers. I could identify the last one easily. **00001010** is **0xa**, the newline character(**\n**). Looking at the other numbers, they are all binary values of **ascii** characters, because the **MSB** is 0 . **Note** that ascii encoding took only 7 bits, but we had 1 byte / 8 bits as the basic memory unit. That is why ascii is made 8-bits by placing a 0 for the MSB bit.

4. I wrote a short python script which directly gives the string, when the binary numbers are converted to their respective ascii characters and joined together.
		
		#!/usr/bin/python

		bin_str = '01010100 01101001 01110100 01101000 00110100 01100011 01101111 01101011 01100101 01101001 00001010'

		bin_list = bin_str.split()
		char_list = []

		i = 0

		while i < len(bin_list) :
			char_list.append(chr(int(bin_list[i], 2)))
	        i = i + 1

		final_string = ''.join(str(char) for char in char_list)

		print final_string

* And the final string you get is **Tith4cokei**. There is also a **\n** at the end. 

* This looks like a password. So, I tried logging in with this password and **GAME OVER!**.

		leviathan5@leviathan:~$ 
		leviathan5@leviathan:~$ 

### Password for leviathan5: Tith4cokei

------------

**PS**:

1. I ran **bin** with ltrace and this is what I got.
		
		leviathan4@leviathan:~/.trash$ ltrace ./bin
		__libc_start_main(0x56555640, 1, 0xffffd724, 0x56555750 <unfinished ...>
		fopen("/etc/leviathan_pass/leviathan5", "r")     = 0
		+++ exited (status 255) +++
		leviathan4@leviathan:~/.trash$ 

* **bin** tries to open **/etc/leviathan_pass/leviathan5** but as you can see, fopen returned a **0** or a **NULL** Pointer. 

2. The **bin** is not as simple as it seems. Thought we have got the password, there are a few things to be investigated about **bin**. 
		
