#include <time.h>

struct timespec s, e;
main(){
    clock_gettime(CLOCK_REALTIME, &s);
    for(int i = 0; i < 100000000; i++){
        getuid();
    }
    clock_gettime(CLOCK_REALTIME, &e);

    double blah = (e.tv_sec - s.tv_sec) + (e.tv_nsec - s.tv_nsec)/1e8;
    printf("%f nanoseconds per iteration\n", blah);
}
