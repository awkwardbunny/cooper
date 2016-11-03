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
    while(fgets(cmd_line, 1024, fp_in)){    //what to do if larger than 1024?
        if(cmd_line[0] != '#' && cmd_line[0] != '\n'){
            ret += process(cmd_line);
        }
        if(fp_in == stdin)
            prompt();
    }

    //Don't think I need this since it can only be success (>0) or NULL (if eof or error)
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

        printf("Token: %s\n", tok);
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

    int wstatus;

    pid_t pid = fork();
    if(pid == -1){
        fprintf(stderr, "Error forking: %s\n", strerror(errno));
        return 1;
    }else if(pid == 0){

            //TODO: Implement redirects
            if(fn_rIn){
                fprintf(stderr, "STDIN redirected from %s\n", fn_rIn);
            }

            if(fn_rOut){
                fprintf(stderr, "STDOUT redirected to %s\n", fn_rOut);
            }else if(fn_rOut_A){
                fprintf(stderr, "STDOUTa redirected to %s\n", fn_rOut_A);
            }

            if(fn_rErr){
                fprintf(stderr, "STDERR redirected to %s\n", fn_rErr);
            }else if(fn_rErr_A){
                fprintf(stderr, "STDERRa redirected to %s\n", fn_rErr_A);
            }

            if(execvp(argv_new[0], argv_new) < 0){
                fprintf(stderr, "Error from execvp() on %s: %s\n", argv_new[0], strerror(errno));
                exit(1);
            }
    }else{
   //     sleep(1);
//        waitpid(pid, &wstatus, 0);
/*            if(wait3(&wstatus, 0, &r_usage) > 0){
                //command done
                if(gettimeofday(&e_time, NULL) < 0){
                    fprintf(stderr, "Error getting end time: %s\n", strerror(errno));
                    exit(1);
                }

                //TODO: print times

                if(wstatus) return 1;
            }else{
                fprintf(stderr, "Error wait3 on pid %d: %s\n", pid, strerror(errno));
                return 1;
            }
 */   }

    return 0;
}
