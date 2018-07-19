#include <stdio.h>
#include <string.h>

int filter(char* cmd){
        int r=0;
        r += strstr(cmd, "flag")!=0;
        r += strstr(cmd, "sh")!=0;
        r += strstr(cmd, "tmp")!=0;

	printf("r = %d\n", r);
        return r;
}
int main(int argc, char* argv[], char** envp){
        putenv("PATH=/thankyouverymuch");
	system("echo $PATH");
	printf("argv[1] = %s\n", argv[1]);
        if(filter(argv[1])) return 0;
        system( argv[1] );
        return 0;
}

