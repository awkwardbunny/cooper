#include <time.h>

int blah(){}

main(){
    struct timespec s, e;
    clock_gettime(CLOCK_REALTIME, &s);
    for(int i = 0; i < 1000000000; i++){
        blah();
    }
    clock_gettime(CLOCK_REALTIME, &e);

    double blah = (e.tv_sec - s.tv_sec) + (e.tv_nsec - s.tv_nsec)/1e9;
    printf("%1f nanoseconds per iteration\n", blah);
}
