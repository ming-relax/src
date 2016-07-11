#include "log.h"
#include "unistd.h"
#include <string.h>
extern void f2();
extern void f3();
#define LOG_ID 1

void f1()
{
	__DEBUG__();	
}

int main()
{
	memset(log_range, 0, sizeof(log_range_t)*LOG_FILE_CNT);
	log_range[2].start[0]=0;
	log_range[2].end[0]=15;
	while(1){
		f1();
		f2();
		sleep(3);
	}
}

