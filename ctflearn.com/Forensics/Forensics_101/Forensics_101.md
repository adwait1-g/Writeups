### Problem : Forensics 101
Question: 

>Think the flag is somewhere in there.. https://mega.nz/#!6h8whABI!wbg60PARf4u6E6juuvK9-aDRe_bgEL937VO01EImM7c


How did I capture the flag?

1. We get a **.jpg** file. I have renamed it to Forensics_101.jpg .

2. The first thing I did was to get all the strings present in it and store it in another file **strings.txt** . 

		$ strings Forensics_101.jpg > strings.txt

3. The flag was present inside strings.txt , which means flag was simply stored as a string in the .jpg file . 

### Flag: flag{wow!_data_is_cool}


