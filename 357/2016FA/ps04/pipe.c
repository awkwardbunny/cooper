#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>

int main (int argc, char **argv){
    int count = 0;
    int fd[2];

    if((fd[0] = open("/tmp/in", O_CREAT | O_TRUNC | O_RDONLY, 0666)) < 0){
        fprintf(stderr, "Error opening file '/tmp/in': %s\n", strerror(errno));
        exit(-1);
    }

    if((fd[1] = open("/tmp/out", O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0){
        fprintf(stderr, "Error opening file '/tmp/out': %s\n", strerror(errno));
        exit(-1);
    }

    if(pipe2(fd, O_NONBLOCK) < 0){
        fprintf(stderr, "Error creating pipe: %s\n", strerror(errno));
        exit(-1);
    }

    while(write(fd[1], " ", 1) > 0)
        count++;

    fprintf(stdout, "write() ERROR: ");
    if(errno == EAGAIN)
        printf("errno == EAGAIN and %d bytes written to pipe\n", count);
    else
        printf("errno != EAGAIN; strerror: %s\n", strerror(errno));
}
