#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <libgen.h>

void prompt();
int process(char *);

int main(int argc, char **argv){
    FILE *fp_in;

    if(argc == 1){
        fp_in = stdin;
    }else if(!(fp_in = fopen(argv[1], "r"))){
        fprintf(stderr, "Error opening file \'%s\': %s\n", argv[1], strerror(errno)); return -1;
    }

    if(fp_in == stdin)
        prompt();

    int ret = 0;

    char cmd_line[1024];
    while(fgets(cmd_line, 1024, fp_in)!=NULL){    //what to do if larger than 1024?
        if(cmd_line[0] != '#' && cmd_line[0] != '\n'){
            ret += process(cmd_line);
        }

        if(fp_in == stdin)
            prompt();
    }

    //Don't think I need this since it can only be success (if eof) or EINVAL
/*
    if(read < 0){
        fprintf(stderr, "Error getline(): %s\n", strerror(errno));
    }
*/

    return ret; //returns number of errors
}

void prompt(){
    char cwd[1024];
    if(!getcwd(cwd, 1024))
        fprintf(stderr, "Error getting current directory: %s\n", strerror(errno));
    printf("[%s]$ ", basename(cwd));
}

int process(char *line){
    char line_bak[strlen(line)]; // dont need +1 because getting rid of \n
    strncpy(line_bak, line, strlen(line));
    line_bak[strlen(line) -1] = 0;

    char *argv_new[1024];
    int argc_new = 0;

    char *fn_rIn = NULL;
    char *fn_rOut = NULL;
    char *fn_rOut_A = NULL;
    char *fn_rErr = NULL;
    char *fn_rErr_A = NULL;

    char *tok = strtok(line, " \t\r\n");
    while(tok){

        // Parse redirects
        if(tok[0] == '<'){
            fn_rIn = tok + 1;
        }else if(tok[0] == '>'){
            if(tok[1] == '>'){
                fn_rOut_A = tok + 2;
            }else{
                fn_rOut = tok + 1;
            }
        }else if(tok[0] == '2' && tok[1] == '>'){
            if(tok[2] == '>'){
                fn_rErr_A = tok + 3;
            }else{
                fn_rErr = tok + 2;
            }
        }else{
            if(argc_new < 1023)
                argv_new[argc_new++] = tok;
            else
                fprintf(stderr, "Error, too many arguments... Truncating to 1023 arguments.\n");
        }

        tok = strtok(NULL, " \t\r\n");
    }

    //needed to prevent passing anything extra
    argv_new[argc_new] = NULL;

    struct timeval s_time;
    struct timeval e_time;
    if(gettimeofday(&s_time, NULL) < 0){
        fprintf(stderr, "Error getting start time: %s\n", strerror(errno));
        exit(1);
    }

    //command 'cd' has to be implemented separately
    //http://unix.stackexchange.com/questions/38808/why-is-cd-not-a-program
    if(!strcmp(argv_new[0], "cd")){
        if(argc_new == 1){
            fprintf(stderr, "Error changing directory! Please provide an argument\n");
            return 1;
        }
        if(chdir(argv_new[1]) < 0){
            fprintf(stderr, "Error changing directory: %s\n", strerror(errno));
            return 1;
        }

        return 0;
    }

    pid_t pid = fork();
    if(pid == -1){
        fprintf(stderr, "Error forking: %s\n", strerror(errno));
        return 1;
    }else if(!pid){
        int fd_tmp;

            if(fn_rIn){
//                fprintf(stderr, "STDIN redirected from %s\n", fn_rIn);
                if((fd_tmp = open(fn_rIn, O_RDONLY | 0666)) > 0){
                    if(dup2(fd_tmp, 0) < 0){
                        fprintf(stderr, "Error redirecting input from %s: %s\n", fn_rIn, strerror(errno));
                        _exit(1);
                    }
                }else{
                    fprintf(stderr, "Error opening input file %s: %s\n", fn_rIn, strerror(errno));
                    _exit(1);
                }
            }

            if(fn_rOut){
//                fprintf(stderr, "STDOUT redirected to %s\n", fn_rOut);
                if((fd_tmp = open(fn_rOut, O_RDWR | O_TRUNC | O_CREAT, 0666)) > 0){
                    if(dup2(fd_tmp, 1) < 0){
                        fprintf(stderr, "Error redirecting output to %s: %s\n", fn_rOut, strerror(errno));
                        _exit(1);
                    }
                }else{
                    fprintf(stderr, "Error opening output file %s: %s\n", fn_rOut, strerror(errno));
                    _exit(1);
                }
            }else if(fn_rOut_A){
//                fprintf(stderr, "STDOUTa redirected to %s\n", fn_rOut_A);
                if((fd_tmp = open(fn_rOut_A, O_RDWR | O_APPEND | O_CREAT, 0666)) > 0){
                    if(dup2(fd_tmp, 1) < 0){
                        fprintf(stderr, "Error redirecting output to %s: %s\n", fn_rOut_A, strerror(errno));
                        _exit(1);
                    }
                }else{
                    fprintf(stderr, "Error opening output file %s: %s\n", fn_rOut_A, strerror(errno));
                    _exit(1);
                }
            }

            if(fn_rErr){
/* GODDAMNIT I was trying to fix a bug where redirected STDERR was empty only to realize 2 weeks later that there was nothing producing the errors */
//                fprintf(stderr, "STDERR redirected to %s\n", fn_rErr);
                if((fd_tmp = open(fn_rErr, O_RDWR | O_TRUNC | O_CREAT, 0666)) > 0){
                    if(dup2(fd_tmp, 2) < 0){
                        fprintf(stderr, "Error redirecting errors to %s: %s\n", fn_rErr, strerror(errno));
                        _exit(1);
                    }
                }else{
                    fprintf(stderr, "Error opening output error file %s: %s\n", fn_rErr, strerror(errno));
                    _exit(1);
                }
            }else if(fn_rErr_A){
//                fprintf(stderr, "STDERRa redirected to %s\n", fn_rErr_A);
                if((fd_tmp = open(fn_rErr_A, O_RDWR | O_APPEND | O_CREAT, 0666)) > 0){
                    if(dup2(fd_tmp, 2) < 0){
                        fprintf(stderr, "Error redirecting errors to %s: %s\n", fn_rErr_A, strerror(errno));
                        _exit(1);
                    }
                }else{
                    fprintf(stderr, "Error opening output error file %s: %s\n", fn_rErr_A, strerror(errno));
                    _exit(1);
                }
            }

            if(execvp(argv_new[0], argv_new) < 0){
                fprintf(stderr, "Error from execvp() on %s: %s\n", argv_new[0], strerror(errno));
                _exit(1);
            }
    }else{
        int wstatus = 0;
        struct timeval dTime;
        struct rusage ru;
        if(wait4(pid, &wstatus, 0, &ru) < 0){
            fprintf(stderr, "Error wait3 on pid %d: %s\n", pid, strerror(errno));
            return 1;
        }else{
            //command done
            if(gettimeofday(&e_time, NULL) < 0){
                fprintf(stderr, "Error getting end time: %s\n", strerror(errno));
                exit(1);
            }

            timersub(&e_time, &s_time, &dTime);
            fprintf(stderr, "Command \'%s\' returned %d\n", line_bak, WEXITSTATUS(wstatus));
            fprintf(stderr, "Real time:\t%01d.%03ds\n", dTime.tv_sec, dTime.tv_usec);
            fprintf(stderr, "System time:\t%01d.%03ds\n", ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
            fprintf(stderr, "User time:\t%01d.%03ds\n", ru.ru_utime.tv_sec, ru.ru_utime.tv_usec);

            if(WEXITSTATUS(wstatus)) return 1;
        }
    }

    return 0;
}
