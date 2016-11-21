#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int sigusr_count, sigrt_count;

void test_signals(int s1, int s2){
    int parent = getpid();

    for(int i = 0; i < 50; i++){
        int pid = fork();
        if(pid == -1){
            fprintf(stderr, "Error forking: %s\n", strerror(errno));
            exit(-1);
        }else if(!pid){
            for(int j = 0; j < 100; j++){
                kill(parent, s1);
                kill(parent, s2);
            }
            exit(0);
        }
    }
    
    errno = 0;
    while(errno != ECHILD){
        wait(NULL);
    }

    return;
}

void sig_handler(int signum){
    if(signum == SIGUSR1)
        sigusr_count++;
    else
        sigrt_count++;
}

int main(int argc, char **argv){
    sigusr_count = 0;
    sigrt_count = 0;

    signal(SIGUSR1, sig_handler);
    signal(SIGRTMAX, sig_handler);

    test_signals(SIGUSR1, SIGRTMAX);
//    raise(SIGUSR1);

    printf("Conventional Linux signal \"SIGUSR1\" sent 50 times from each of 100 processes\nReceived %d times\n", sigusr_count);
    printf("Real-time signal number %d sent 50 times from each of 100 processes\nReceived %d times\n", SIGRTMAX, sigrt_count);
}
