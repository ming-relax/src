#include <stdio.h>
#include "enable_log.h"
void f1()
{
	__DEBUG__("debug enabled in %s", __func__);
}
#include "disable_log.h"
void f2()
{
	__DEBUG__("debug enabled in %s", __func__);
}
int main()
{
	f1();
	f2();
}
