
#define __DEBUG__(namefmt, ...) \
do{\
        printf(namefmt"\n", ## __VA_ARGS__);\
}while(0);


