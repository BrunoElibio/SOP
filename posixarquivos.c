#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char const *argv[]) {
    struct stat stats2;
    struct stat stats1;
    char *buffer;
    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

    if(stat(argv[2], &stats2) == -1) exit(1);
    if(stat(argv[1], &stats1) == -1) exit(1);

    int tamanho = stats1.st_blksize;
    int tamanho2 = stats1.st_size;

    buffer = malloc(sizeof(char)*tamanho);
    if(tamanho2<tamanho){
	read(fd1, buffer, tamanho2);
	write(fd2, buffer, tamanho2);
    }else{
	    while(read(fd1, buffer, tamanho) != 0){
		write(fd2, buffer, tamanho);
	}		
	
    }

  return 0;
}

