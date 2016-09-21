#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv){

// Load default values
	int in_fd = STDIN_FILENO;
    int out_fd = STDOUT_FILENO;
	int buf_size = 512;
    int arglength = 1; // Number of arguments that are not input sources

// Parse options
    char opt;
    while((opt = getopt(argc, argv, "b:o:")) != -1){
        if(opt == 'o'){
			out_fd = open(optarg, O_RDWR|O_CREAT|O_TRUNC, 0644);
			if(out_fd < 0){
                fprintf(stderr, "Failed to open file (%s) for writing: %s\n", optarg, strerror(errno));
				exit(-1);
			}

            arglength += 2; // There are now two more arguments
        }else if(opt == 'b'){
            buf_size = atoi(optarg);
            if(buf_size < 1){
                fprintf(stderr, "Invalid argument: buffer size must be greater than 0\n");
                exit(-1);
            }

            arglength += 2; // There are now two more arguments
        }else{
// Getopt already produced its own errors
            exit(-1);
        }
    }

	char* buf = malloc(buf_size);

// Loop inputs files
    int i = !(argc == arglength);   // If there are no input sources (# of arguments == argv[0] and optional arguments), start from 0
    char *a;

	for(; i < argc; i++){
        if(i == 0){                 // If start from 0, read from stdin
            a = "-";
        }else{
            a = argv[i];
        }

// If "-o" or "-b", skip this argument and the next
        if(!strncmp(a, "-o", 2) || !strncmp(a, "-b", 2)){
            i++;
            continue;
        }
            
// Open file
        if(!strncmp(a, "-", 1)){
            in_fd = STDIN_FILENO;
        }else{  
            in_fd = open(a, O_RDONLY);
            if(in_fd < 0){
                fprintf(stderr, "Could not open file (%s) for reading: %s\n", a, strerror(errno));
                exit(-1);
            }
        }

// Read and Write
        int w = 0, r = read(in_fd, buf, buf_size);
        if(r < 0){
            fprintf(stderr, "Error occurred reading from file (%s): %s\n", a, strerror(errno));
            exit(-1);
        }
        
        while(r){
            w = write(out_fd, buf, r);
            if(w != r){                 // Write failed! (Partial write)
                r -= w;                 // Calculate how much left
                w = write(out_fd, buf+w, r); // Try again from new offset
                
                if(w != r){
                    fprintf(stderr, "Error occurred writing to file (%s): %s\n", a, strerror(errno));
                    exit(-1);
                }
            }

            r = read(in_fd, buf, buf_size);
            if(r < 0){
                fprintf(stderr, "Error occurred reading from file (%s): %s\n", a, strerror(errno));
                exit(-1);
            }
        }
        
        close(in_fd);
	}

	free(buf);
    close(out_fd);
}
