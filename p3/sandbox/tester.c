#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){

	int sourceFD, targetFD, result;

	if(argc != 3){
		printf("FAILED\n");
		exit(1);
	}

	sourceFD = open(argv[1], O_RDONLY);
	if(sourceFD == -1){
		perror("source open()"); 
		exit(1);
	}
	printf("sourceFD == %d\n", targetFD);

	targetFD = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(targetFD == -1){
		perror("target open()");
		exit(1);
	}
	printf("targetFD == %d\n", targetFD);

	result = dup2(sourceFD, 0);
	if(result == -1){ perror("source dup2()"); exit(2); }
	
	result = dup2(targetFD, 1);
	if(result == -1){ perror("target dup2()"); exit(2); }

	execlp("sort", "sort", NULL);

	return (3);
}
