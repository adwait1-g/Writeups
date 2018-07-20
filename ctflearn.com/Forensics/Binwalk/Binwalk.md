### Problem: Binwalk

Question: 
> https://mega.nz/#!4UEnAZKT!-deNdQJxsQS8bTSMxeUOtpEclCI-zpK7tbJiKV0tXYY

How did I capture the flag?

1. You get a **jpeg** file from the above link. Used a tool called **foremost** to solve this question. 

2. Process the jpeg file using foremost. 

		$ foremost PurpleThing.jpeg 
		Processing: PurpleThing.jpeg
		|*|
		$ ls
		output  PurpleThing.jpeg
		$
3. Let us see what the **output** directory has. 

		$ ls
		audit.txt  png
		$ 
		
4. This is what the **audit.txt** has. 

		Foremost version 1.5.7 by Jesse Kornblum, Kris Kendall, and Nick Mikus
		Audit File

		Foremost started at Fri Jul 20 10:38:48 2018
		Invocation: foremost PurpleThing.jpeg 
		Output directory: /home/adwi/Desktop/CTF-Writeups/ctflearn.com/Forensics/Binwalk/output
		Configuration file: /etc/foremost.conf
		------------------------------------------------------------------
		File: PurpleThing.jpeg
		Start: Fri Jul 20 10:38:48 2018
		Length: 160 KB (164802 bytes)
		 
		Num	 Name (bs=512)	       Size	 File Offset	 Comment 

		0:	00000000.png 	     149 KB 	          0 	  (780 x 720)
		1:	00000299.png 	      11 KB 	     153493 	  (802 x 118)
		Finish: Fri Jul 20 10:38:48 2018

		2 FILES EXTRACTED
			
		png:= 2
		------------------------------------------------------------------

		Foremost finished at Fri Jul 20 10:38:48 2018

* It tells that there are 2 **png** files. Let us see what the 2 file have.
		
		output/png$ ls
		00000000.png  00000299.png
		output/png$

6.  Here you go, the flag is in **00000299.png** . 
![capture the flag](./output/png/00000299.png)

### Flag: ABCTF{b1nw4lk_is_us3ful}

 


