#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>

//#define _DEBUG

jmp_buf UGH; // Wise words of Fred Fontaine

int bgrep(char *patt, char **fns, int f_n, int c, int c_set);
int bstrcmp(char *p, char *pp, int n);
void sigbus_handler();
void print_context(char *bstr, int c, char *search, int n, int n_buf);

int main(int argc, char **argv){

    if(signal(SIGBUS, sigbus_handler) == SIG_ERR){
        fprintf(stderr, "%s: signal() call failed: %s\n", argv[0], strerror(errno));
        exit(-1);
    }

    char *fn = NULL;
    char *patt = NULL;
    int context = 0;
    int c_set = 0;
    int size = -1;

    int args = 1;

    int c;
    opterr = 0;
    while((c = getopt(argc, argv, "c:p:")) != -1){
        switch(c){
            case 'c':
                //Context = atoi(optarg); // NO error detection; I guess I could do isdigit, but nah
                if((sscanf(optarg, "%d", &context) != 1) || context < 0){
                    fprintf(stderr, "%s: Context must be a non-negative integer\n", argv[0]);
                    exit(-1);
                }
                args += 2;
                c_set = 1;
                break;
            case 'p':
                fn = optarg;
                args += 2;
                break;
            case '?':
                if(optopt == 'c' || optopt == 'p')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if(isprint(optopt))
                    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                exit(-1);
            default:
                //shouldn't be here?
                fprintf(stderr, "Unknown error?!?!?!?!???????");
                exit(-1);
        }
    }

    int pa = 0;
    int files_n = 0;
    char *files[argc-args];
    for(int i = 1; i < argc; i++){
        char *s_curr = argv[i];
        if(s_curr[0] == '-'){
            i++;
            continue;
        }

        if(!pa && !fn){
            patt = s_curr;
            pa++;
        }else files[files_n++] = s_curr;
    }

#ifdef _DEBUG
    if(fn){
        fprintf(stderr, "Input is from file: %s\n", fn);
    }else if(patt){
        fprintf(stderr, "Input is from cmdline: %s\n", patt);
    }else{
        fprintf(stderr, "%s Error: No pattern specified\n", argv[0]);
        exit(-1);
    }

    fprintf(stderr, "Context: %d\n", context);

    for(int i = 0; i < files_n; i++){
        fprintf(stderr, "File %d: %s\n", i, files[i]);
    }
    if(!files_n) fprintf(stderr, "Read from <stdin>\n");
#endif

    if(fn){
        int fd_patt;
        int size;

        if((fd_patt = open(fn, O_RDONLY)) < 0){
            fprintf(stderr, "%s: Cannot open %s for reading: %s\n", argv[0], fn, strerror(errno));
            exit(-1);
        }

        if((size = lseek(fd_patt, 0, SEEK_END)) < 0){
            fprintf(stderr, "%s: Cannot lseek(), failed on %s wtf: %s\n", argv[0], fn, strerror(errno));
            exit(-1);
        }

        if((patt = mmap(NULL, size, PROT_READ, MAP_SHARED, fd_patt, 0)) == MAP_FAILED){
            fprintf(stderr, "%s: mmap() failed on %s: %s\n", argv[0], fn, strerror(errno));
            exit(-1);
        }
    }

    return bgrep(patt, files, files_n, context, c_set);
}

int bgrep(char *patt, char **fns, int f_n, int c, int c_set){
    int found = 0;
    int sysfail = 0;
    int read_from_stdin = 0;
    int fd;
    int size;
    char *search;

    if(!f_n){
        f_n = 1;
        read_from_stdin = 1;
        fns[0] = "<stdin>";
    }

    for(int i = 0; i < f_n; i++){
        if(!read_from_stdin && (fd = open(fns[i], O_RDONLY, 0444)) < 0){
           fprintf(stderr, "Error: Cannot open file \'%s\' for reading: %s\n", fns[i], strerror(errno));
           sysfail = 1;
           continue;
        }else if(read_from_stdin){
            fd = STDIN_FILENO;
        }

        if((size = lseek(fd, 0, SEEK_END)) < 0){
            fprintf(stderr, "Error: Cannot lseek(), failed on %s wtf: %s\n", fns[i], strerror(errno));
           sysfail = 1;
           close(fd);
           continue;
        }

        /*
        struct stat st;
        if(fstat(fd, &st) < 0){
            fprintf(stderr, "Error: fstat() failed on %s: %s\n", fns[i], strerror(errno));
            return -1;
        }
        */

        if((search = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED){
            fprintf(stderr, "Error: Cannot mmap \'%s\': %s\n", fns[i], strerror(errno));
            sysfail = 1;
            close(fd);
            continue;
        }

#ifdef _DEBUG
        printf("Pattern: %s\n", patt);
        printf("Search: %s\n", search);
#endif
        if(setjmp(UGH)){
            close(fd);
            sysfail = 1;
            fprintf(stderr, "AHH SIGBUS RECEIVED!!! DID IT SHRINK?!\n");
            continue;
        }

        int n_patt = strlen(patt);
        char *pointer_pointer = search;
        while(pointer_pointer <= search+size-n_patt){
            if(pointer_pointer[0] != patt[0]){
                pointer_pointer++;
                continue;
            }

            found = bstrcmp(patt, pointer_pointer, n_patt);
            if(found){
                printf("%s:%ld", fns[i], pointer_pointer-search);
                if(c_set)
                    print_context(pointer_pointer, c, search, n_patt, size);
                printf("\n");
            }

            pointer_pointer++;
        }

        close(fd);
        
    }

    if(sysfail) return -1;
    return !found;
}

int bstrcmp(char *p, char *pp, int n){
    for(n--;n;n--){
        if(pp[n] != p[n])
            return 0;
    }
    
    return 1;
}

void sigbus_handler(){
    longjmp(UGH, 1337);
}

/*
 * bstr   is the matched string pointer
 * c      is the context number
 * search is the beginning of file content pointer (used to calculate things)
 * n      is the length of match string (or pattern string)
 * n_buf  is the total size of file (used to calculate things)
 */
void print_context(char *bstr, int c, char *search, int n, int n_buf){
    char *start = bstr-c; //new place to start printing
    long int nn = n+c+c; //new print length

    if(start < search){
        nn -= ((long int)search+(long int)c-(long int)bstr);
        start = search;
    }

    if(start+nn > search+n_buf){
        nn = search+n_buf-start;
    }

    printf("\t");
    for(int i = 0; i < nn; i++){
        if(isprint(start[i]))
            printf("%c", start[i]);
        else
            printf(".");
    }

    printf("\t");
    for(int i = 0; i < nn; i++){
        printf("%02x ", (unsigned char)start[i]);
    }
}















