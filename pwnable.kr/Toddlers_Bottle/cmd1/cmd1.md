### To pwn: cmd1

**Question** :
> Mommy! what is PATH environment in Linux?
ssh cmd1@pwnable.kr -p2222 (pw:guest)

### How did I pwn it?

1. There is an executable **cmd1** along with it's sourcecode **cmd1.c** and the flag file.
2. Understanding the sourcecode.
		
		#include <string.h>

		int filter(char* cmd){
			int r=0;
			r += strstr(cmd, "flag")!=0;
			r += strstr(cmd, "sh")!=0;
			r += strstr(cmd, "tmp")!=0;
		return r;
		}
		int main(int argc, char* argv[], char** envp){
			putenv("PATH=/thankyouverymuch");
			if(filter(argv[1])) return 0;
			system( argv[1] );
			return 0;
		}

* In the **main** function, the environment variable **PATH** is changed to **/thankyouverymuch** . Let us check what the old value of **PATH** was. 
	
		cmd1@ubuntu:~$ echo $PATH
		/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games
		cmd1@ubuntu:~$ 

* Let us understand what the  value means. There are many bash commands that we use day in and day out. Some most used ones are **cd** , **ls** , **cat** , **mkdir** and many more. What are all these commands?
 
	* These commands are nothing but normal executables programmed to do a certain job. **mkdir** is needed to create a new directory , **ls** is used to list the contents of a specified directory and so on. 
	
	* Where do you think these commands are stored? - They are stored in various places like **/bin** directory, **/usr/local/bin** and many other places. 
	
	* All these directories where common commands are stored as a part of the **PATH** environment variable.
	
	* If you want to find the locations where these commands' executables are stored, you can use the **which** command. Here are a few examples. 
		
			cmd1@ubuntu:~$ which ls
			/bin/ls
			cmd1@ubuntu:~$ which cat
			/bin/cat
			cmd1@ubuntu:~$ which which
			/usr/bin/which
			cmd1@ubuntu:~$
	
	* You can try out on whichever command you want. 

* Coming back to **PATH** env variable, it is a **concatenation** of all these common paths / locations where these commands are stored. Have a look at **PATH** variable once again and inspect every location present. So, when you use **mkdir** command, the shell searches for **mkdir** in all the paths specified in the **PATH** variable. 

* To understand this better, let us take an example. There is **cmd1** in **~** directory. This is how execute it. 

		cmd1@ubuntu:~$ ./cmd1
		Segmentation fault
	
	* Do not worry about the seg-fault for now.
	
	* You use **./** before **cmd1** to execute it. Let us not use **./** and try to execute it. Let us see what happens. 
	
			cmd1@ubuntu:~$ cmd1
			cmd1: command not found

	* Now, the shell returns an error saying it is not able to find the command named **cmd1** . The shell by default searches for **cmd1** in all locations specified in **PATH** variable. As there is not cmd1 in any of those paths, we get an error.
	
	* **.** stands for **present directory** . When you execute **./cmd1** , you are telling the shell to search for an executable named **cmd1** in the **present directory** . So, shell searches for it, finds it and executes it. 
	
	* If you don't want to use **./** everytime to execute a program in the current directory, you can **add** current directory to the **PATH** variable in the following manner.  
	
			cmd1@ubuntu:~$ PATH=$PATH:.
			cmd1@ubuntu:~$ cmd1
			Segmentation fault
			cmd1@ubuntu:~$ echo $PATH
			/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:.
			cmd1@ubuntu:~$
			
	* Look at the changed **PATH** variable. It has **.** (or current directory) in it. So, when you just do **cmd1** , the shell searches for all locations in **PATH** . As the current directory is present in **PATH** , shell gets **cmd1** and executes it. 

* I hope you have got some clarity on **PATH** environment variables. Let us talk about what exactly an env-variable is at the end. 

* Coming back to the question, **putenv** is used to change an environment variable. In this program in main function, **putenv** is changing **PATH** variable to **/thankyouverymuch** . 

* After it is changed, a user-defined function **filter** is called with **argv[1]** as argument. If **filter** return a **non-zero** value, then our program with return. Else, it will execute **system(argv[1])** . 
* Let us now take a look at **filter** function. 

		int filter(char* cmd){
        int r=0;
        r += strstr(cmd, "flag")!=0;
        r += strstr(cmd, "sh")!=0;
        r += strstr(cmd, "tmp")!=0;
        return r;
		}

	* **strstr** is a function which is used to find the position of a sub-string in a string. The position of second argument is searched in the first argument. It returns NULL if sub-string is not found, else it return the position. Take a look at it's manpage. 
	
	* What **filter** does is, it is looking for sub-strings **flag** , **sh** , **tmp** in the string **cmd** . If **none** of them is found, r will be 0 and 0 will be returned. Else, some positive value will be returned. 
	
	* If **filter** returns a non-zero value, Then the **main** function returns without executing **system(argv[1])** . So, we have to construct **argv[1]** in such a way that it does not have **flag** , **sh** and **tmp** , but we have to get the flag. 
	
	* Also remember that **PATH** has been changed. So, you cannot just use **cat** or **ls** etc., You have to specify them like **/bin/cat** , **/bin/ls** and so on. 
	* This is a simple payload I constructed for **argv[1]** to get the flag. 
		
			cmd1@ubuntu:~$ ./cmd1 "/bin/cat *"

	* Look at the payload. It does not have **flag** , **tmp** or **sh** . So, system() is executed. 
	
	* Here, I had to use **/bin/cat** . ** * ** is a wild-card character. Shell expands ** * ** to everything that is there in the current directory. So, **/bin/cat * ** expands to **/bin/cat cmd cmd1 flag** . 
	
	* So, everything including what we want - the flag is printed on the terminal. This is what I got. You get a lot of stuff. I have shown only the flag. 

			mommy now I get what PATH environment is for :)
			cmd1@ubuntu:~$

### FLAG: mommy now I get what PATH environment is for :)

--------------------

PS: 

1. To read more about **putenv** , **strstr** or any C Library Function, you it's manpage. It has good details. Some even have proper examples which help us understand how to use them. 

2.  Let us understand what environment variables are and why are they used. 

* As the name suggests, these variables make up the environment. To get a list of all environment variables, you can use **printenv** . 

		

	    $ printenv
	    XDG_VTNR=7
	    XDG_SESSION_ID=c2
	    XDG_GREETER_DATA_DIR=/var/lib/lightdm-data/adwi
	    CLUTTER_IM_MODULE=xim
	    SESSION=ubuntu
	    GPG_AGENT_INFO=/home/adwi/.gnupg/S.gpg-agent:0:1
	    FLAG=flag
	    TERM=xterm-256color
	    VTE_VERSION=4205
	    XDG_MENU_PREFIX=gnome-
	    SHELL=/bin/bash
	    QT_LINUX_ACCESSIBILITY_ALWAYS_ON=1
	    WINDOWID=71303178
	    OLDPWD=/home/adwi/Desktop/CTF-Writeups/pwnable.kr/Toddlers_Bottle/cmd1
	    UPSTART_SESSION=unix:abstract=/com/ubuntu/upstart-session/1000/2094
	    GNOME_KEYRING_CONTROL=
	    GTK_MODULES=gail:atk-bridge:unity-gtk-module
	    USER=adwi
	    LS_COLORS=rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:mi=00:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arc=01;31:*.arj=01;31:*.taz=01;31:*.lha=01;31:*.lz4=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.tzo=01;31:*.t7z=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.dz=01;31:*.gz=01;31:*.lrz=01;31:*.lz=01;31:*.lzo=01;31:*.xz=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=01;31:*.tbz2=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.war=01;31:*.ear=01;31:*.sar=01;31:*.rar=01;31:*.alz=01;31:*.ace=01;31:*.zoo=01;31:*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.cab=01;31:*.jpg=01;35:*.jpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:*.webm=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;35:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:*.ogv=01;35:*.ogx=01;35:*.aac=00;36:*.au=00;36:*.flac=00;36:*.m4a=00;36:*.mid=00;36:*.midi=00;36:*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=00;36:*.ra=00;36:*.wav=00;36:*.oga=00;36:*.opus=00;36:*.spx=00;36:*.xspf=00;36:
	    QT_ACCESSIBILITY=1
	    UNITY_HAS_3D_SUPPORT=true
	    XDG_SESSION_PATH=/org/freedesktop/DisplayManager/Session0
	    XDG_SEAT_PATH=/org/freedesktop/DisplayManager/Seat0
	    SSH_AUTH_SOCK=/run/user/1000/keyring/ssh
	    DEFAULTS_PATH=/usr/share/gconf/ubuntu.default.path
	    SESSION_MANAGER=local/adwi:@/tmp/.ICE-unix/2358,unix/adwi:/tmp/.ICE-unix/2358
	    XDG_CONFIG_DIRS=/etc/xdg/xdg-ubuntu:/usr/share/upstart/xdg:/etc/xdg
	    UNITY_DEFAULT_PROFILE=unity
	    DESKTOP_SESSION=ubuntu
	    PATH=/home/adwi/bin:/home/adwi/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/home/adwi/android_studio_bin/
	    QT_IM_MODULE=ibus
	    QT_QPA_PLATFORMTHEME=appmenu-qt5
	    XDG_SESSION_TYPE=x11
	    PWD=/home/adwi/Desktop/CTF-Writeups/pwnable.kr/Toddlers_Bottle/cmd1/code_samples
	    JOB=unity-settings-daemon
	    XMODIFIERS=@im=ibus
	    GNOME_KEYRING_PID=
	    LANG=en_IN
	    GDM_LANG=en_US
	    MANDATORY_PATH=/usr/share/gconf/ubuntu.mandatory.path
	    COMPIZ_CONFIG_PROFILE=ubuntu
	    IM_CONFIG_PHASE=1
	    GDMSESSION=ubuntu
	    SESSIONTYPE=gnome-session
	    GTK2_MODULES=overlay-scrollbar
	    SHLVL=1
	    HOME=/home/adwi
	    XDG_SEAT=seat0
	    LANGUAGE=en_IN:en
	    GNOME_DESKTOP_SESSION_ID=this-is-deprecated
	    UPSTART_INSTANCE=
	    UPSTART_EVENTS=xsession started
	    XDG_SESSION_DESKTOP=ubuntu
	    LOGNAME=adwi
	    COMPIZ_BIN_PATH=/usr/bin/
	    DBUS_SESSION_BUS_ADDRESS=unix:abstract=/tmp/dbus-QnzvBql2V0
	    XDG_DATA_DIRS=/usr/share/ubuntu:/usr/share/gnome:/usr/local/share:/usr/share:/var/lib/snapd/desktop
	    QT4_IM_MODULE=xim
	    LESSOPEN=| /usr/bin/lesspipe %s
	    INSTANCE=
	    UPSTART_JOB=unity7
	    XDG_RUNTIME_DIR=/run/user/1000
	    DISPLAY=:0
	    XDG_CURRENT_DESKTOP=Unity
	    GTK_IM_MODULE=ibus
	    LESSCLOSE=/usr/bin/lesspipe %s %s
	    XAUTHORITY=/home/adwi/.Xauthority
	    _=/usr/bin/printenv
	   $ 

* Some variables you might be familiar with are **SHELL** , **HOME** , **PWD** , **PATH** . 

* When you run a program from a shell with this environment(or with this set of environment variables) , The list is **inherited** by the new program getting executed. Even it will have the same set of variables. 

3. I found this method of mine very crude, but it was successful. There could be better, beautiful ways to pwn it! 

