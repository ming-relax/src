#if !defined (_LOG_H_)
#define _LOG_H_ 
#include <stdio.h>
#define LOG_RANGE_CNT 3
#define LOG_FILE_CNT 64

typedef struct{
	int start[LOG_RANGE_CNT];
	int end[LOG_RANGE_CNT];
}log_range_t;/*closed range*/

extern log_range_t log_range[];
extern long long log_bits;



#define __DEBUG__(namefmt, ...)\
do{\
	if(line_in_log_range(__LINE__, LOG_ID))\
		printf("%s:%d:%s():  ", __FILE__, __LINE__, __func__);\
		printf(namefmt"\n", ## __VA_ARGS__);\
}while(0)

extern int line_in_log_range(int line, int id);

#endif

