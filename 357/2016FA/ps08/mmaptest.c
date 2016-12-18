#include <stdio.h>
#include <sys/mman.h>

int main(){
    void *normal = mmap(NULL, 65536, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    printf("Start: %p, end %p, minus: %p\n", normal, normal+65536, normal-65536);
    void *down = mmap(NULL, 65536, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_GROWSDOWN, -1, 0);
    printf("Start: %p, end %p, minus: %p down\n", down, down+65536, down-65536);
    down = mmap(NULL, 65536, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    printf("Start: %p, end %p, minus: %p down\n", down, down+65536, down-65536);
    normal = mmap(NULL, 65536, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    printf("Start: %p, end %p, minus: %p\n", normal, normal+65536, normal-65536);
    while(1);
}
