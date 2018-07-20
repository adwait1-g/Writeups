#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main() {

	char *str = "&7!LKEN*CMP.VNN";
	char str1[30];
	strcpy(str1, str);

	for(char ch = 32; ch <= 122; ch++) {

		for(int i = 0; i < strlen(str1); i++) {
			str1[i] = str1[i] ^ ch;
		}

		printf("\nch = %d, str1 = %s", ch , str1);
		strcpy(str1, str);
	}

	return 0;
}
