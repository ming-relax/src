#include <stdio.h>
int main()
{
	char c;
	for (c = 'A';  c <= 'Z'; c ++){
		printf("%c, %d\n", c, c);
	}
	for (c = 'a';  c <= 'z'; c ++){
		printf("%c, %d\n", c, c);
	}
	for (c = '0';  c <= '9'; c ++){
		printf("%c, %d\n", c, c);
	}
	return 0;
}
