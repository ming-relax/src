#include "log.h"

log_range_t log_range[LOG_FILE_CNT];
long long log_bits;

inline int line_in_log_range(int line, int id)
{
        int in_range = 0;
        int i;
	if( log_bits & (1<<id))
		return 0;
        for(i = 0;  i< LOG_RANGE_CNT; i ++){
                if ((__LINE__ >= log_range[id].start[i]) && (__LINE__ <= log_range[id].end[i]))
                in_range = 1;
        }
        return in_range;
}
