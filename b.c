#include <stdio.h>

int flag;
#define STUB(ret, func, ...)\
	typeof(func(__VA_ARGS__)) stub_##func\
	{\
		typeof(func) rc = ret;\
		if(!flag){\
			rc = func(__VA_ARGS__);\
		}\
		rc;\
	}

int func1(int i, int j)
{
	return i>j;
}

STUB(0, func1, i, int )

int main()
{
	int ret;
	flag = 0;
	ret = stub_func1(0, func1, 5, 10);	
	flag = 1;
}
