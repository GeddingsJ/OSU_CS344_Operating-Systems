#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main( void){
	pid_t spawnpid = fork();
	switch(spawnpid){
		case -1: exit(1); break;
		case 0: exit(0); break;
		default: break;
	}
	printf("XYZZY\n");
	return 0;
}
