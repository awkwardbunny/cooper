#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv){

	int in_fd = 0;
	int out_fd = 1;

	int buf_size = 1024;
	char* buf;
	
	int files = 0;
	char* filenames[argc];

	for(int i = 1; i < argc; i++){
		char* arg = argv[i];
		
		if(!strncmp(arg, "-o", 2)){
			arg = argv[++i];
			out_fd = open(arg, O_WRONLY|O_CREAT|O_TRUNC, 0644);

			/**** ERROR CHECKING ****/
		}else if(!strncmp(arg, "-b", 2)){
			arg = argv[++i];

			/**** validation ****/

			buf_size = atoi(arg);
		}else{
			filenames[files++] = arg;
		}
	}

	/** DEBUG **/
	for(int a = 0; a < files; a++){
		printf("%s ", filenames[a]);
	}
	printf("\n");

	printf("Output file descriptor: %d\n", out_fd);
	printf("Buffer size in bytes: %d\n", buf_size);
	/** END DEBUG **/

	buf = malloc(buf_size * sizeof(char));

	for(int f = 0; f < files; f++){
		in_fd = open(filenames[f], O_RDONLY);

		/** ERROR CHECKING **/
		if(in_fd < 0){
			fprintf(stderr, "Could not open file \'%s\' for reading:\n%s", filenames[f], strerror(errno));
			return errno;
		}

		int c = 0;
		while(c = read(in_fd, buf, 1024)){
			write(out_fd, buf, c);
		}

		close(out_fd);
	}

	if(!files){
		int c = 0;
		while(c = read(0, buf, 1024)){
			write(out_fd, buf, c);
		}
	}

	free(buf);
}
