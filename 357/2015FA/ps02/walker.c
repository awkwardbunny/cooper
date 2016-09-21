#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv){
	char *path = argv[1]; //For now
	DIR *dirp;
	struct dirent *de;
	if(!(dirp = opendir(path))){
		printf("Can not open directory %s: %s\n", path, strerror(errno));
		return -1;
	}
	while(de = readdir(dirp)){
		printf("%s\n", de->d_name);
	}
	closedir(dirp);
}
