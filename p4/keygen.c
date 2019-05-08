#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main( int argc, char *argv[]){

	int randVal;
	int arrSize = atoi(argv[1]);
	char arrAlpha[arrSize + 1];
	char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int i = arrSize;
	int j = 0;

	srand(time(NULL));
	memset(arrAlpha, '\n', sizeof(arrAlpha));

	while(i != 0){
		randVal = rand() % 27 + 0; //get random number between 1 - 27
		
		char temp = alphabet[randVal];
		arrAlpha[j] = temp;

		j++;
		i--;
	}
	
	arrAlpha[j] = '\n';

	printf("%s", arrAlpha);

	return 0;
}


