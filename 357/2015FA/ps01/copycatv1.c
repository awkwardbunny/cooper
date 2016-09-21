#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void print_error(int err, char* msg, char* filename){
	fprintf(stderr, "%s\n%s: %s\n", strerror(err), msg, filename);
}

int main(int argc, char** argv){

	int buff_size = BUFFER_SIZE;
	int in_fd = 0;
	int out_fd = 1;

	char* buffer = NULL;

	char opt;
	while((opt = getopt(argc, argv, "b:o:")) != -1){
		if(opt == 'o'){
			out_fd = open(optarg, O_RDWR|O_CREAT|O_TRUNC, 0644);
			if(out_fd < 0){
				print_error(errno, "Failed to open file for writing", optarg);
				exit(-1);
			}
			//printf("Output file specified: %s\n", optarg);
		}else if(opt == 'b'){
			buff_size = atoi(optarg);
		}else{
			fprintf(stderr, "Unknown option: %c\n", opt);
		}
	}

	printf("OUTPUT: %d\t BUFFER: %d\n", out_fd, buff_size);

	buffer = malloc(buff_size * sizeof(char));

	int count = 1;
	while(count < argc){
		char* ar = argv[count];
		if(!strcmp(ar, "-o") && !strcmp(ar, "-b")){
			in_fd = open(ar, O_RDONLY);
			if(in_fd < 0){
				fprintf(stderr, "Could not open file for reading: %s\n%s\n", ar, strerror(errno));
				exit(-1);
			}else
				printf("File opened: %s\n", ar);

			int r = read(in_fd, buffer, buff_size);
			if(r <= 0){
				fprintf(stderr, "Error occurred reading from file: %s\n%s\n", ar, strerror(errno));
				exit(-1);
			}

			while(r == buff_size){
				write(out_fd, buffer, buff_size);
				r = read(in_fd, buffer, buff_size);
			}

			write(out_fd, buffer, r);
			close(in_fd);
		}
		count++;	
	}

	free(buffer);

	if(out_fd)
		close(out_fd);
}
