//Program 2 - geddingj.adventure.c
//Author: Jacob Geddings
//Navigate the most recently made files to reach the 'end' file
//upon completion have ur total steps and rooms navigated reported back.
//In addition to this when stating the time you will receive the current time in game
//as well as a file of it stored in the used subdirectory.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int main(void){

	char* fd = "geddingj.rooms."; //front-end to correctly accessing newest subdirectory
	char currentDir[100]; //arbitrary quantity assigned to currentDir placeholder

	memset(currentDir, '\0', sizeof(currentDir)); //prepare currentDir by cleaning it's memory with nulls
	getcwd(currentDir, sizeof(currentDir)); //getcwd will find and store the current directory into currentDir

	DIR *d; //Will be used to read directories
	struct dirent *dp; //Directory pointer for content
	struct stat *buffer; //Pointer for buffer
	
	buffer = malloc(sizeof(struct stat)); //Prepare buffer by allocating memory
	dp = malloc(sizeof(struct dirent)); //Same goes for dp

	time_t lastModified;  //for use in determining most recent subdirectory

	d = opendir(currentDir); //d now holds current active directory
	if(d != NULL){ //if there are potential directories to read
		while(dp = readdir(d)){ //Read directories
			if(strstr(dp->d_name,fd) != NULL){ //If current dp isn't null
				stat(dp->d_name, buffer); //Retrieve information on directory
				lastModified = buffer->st_mtime; //Search for it's last modified date
				chdir(dp->d_name); //Change directory to new one
			}
		}
	}

	FILE* fp; //for accessing files
	char* roomHolder[8][8]; //2d array for storing relevent information about rooms
	time_t t; //variable to acquire time
	int victoryLap[64]; //int array for storing where the user has been
	int moves = 0; //int storing number of room change requests
	int size;
	int victory = 0; //marker to determine if user has won

	memset(roomHolder, '\0', sizeof(roomHolder[0][0]) * 8 * 8); //clean 2d array with null pointers

	fp = fopen("tick", "r"); //open tick file with read permissions

	char* line = NULL; //char pointer for getline
	char* token; //token for string tokenizer
	size_t len = 0; //for determining length of line
	ssize_t read; //storage for getline
	int i = 1; //incrementer
	int conCount = 0; //total number of connections for given file

	while((read = getline(&line, &len, fp)) != -1){
		token = strtok(line, ":"); //cut everything before and including the ":"
		strcpy(token, strtok(NULL, "\n")); //cut everything past the new line delimiter
		char* var1 = strdup(token); //create char pointer to store a duplicate of the token
		roomHolder[1][i] = var1; //store variable within 2d array
		i++; //increment for location in array
	}
	fclose(fp); //close access to file	

	fp = fopen("tock", "r"); //repeat above portion for all files
	i = 1;

	while((read = getline(&line, &len, fp)) != -1){
		token = strtok(line, ":");
		strcpy(token, strtok(NULL, "\n"));
		char* var1 = strdup(token);
		roomHolder[2][i] = var1;
		i++;
		
	}
	fclose(fp);

	fp = fopen("high", "r");
	i = 1;

	while((read = getline(&line, &len, fp)) != -1){
		token = strtok(line, ":");
		strcpy(token, strtok(NULL, "\n"));
		char* var1 = strdup(token);
		roomHolder[3][i] = var1;
		i++;
	}
	fclose(fp);

	fp = fopen("down", "r");
	i = 1;
	
	while((read = getline(&line, &len, fp)) != -1){
		token = strtok(line, ":");
		strcpy(token, strtok(NULL, "\n"));
		char* var1 = strdup(token);
		roomHolder[4][i] = var1;
		i++;
	}
	fclose(fp);

	fp = fopen("duck", "r");
	i = 1;

	while((read = getline(&line, &len, fp)) != -1){
		token = strtok(line, ":");
		strcpy(token, strtok(NULL, "\n"));
		char* var1 = strdup(token);
		roomHolder[5][i] = var1;
		i++;
	}
	fclose(fp);

	fp = fopen("beav", "r");
	i = 1;

	while((read = getline(&line, &len, fp)) != -1){
		token = strtok(line, ":");
		strcpy(token, strtok(NULL, "\n"));
		char* var1 = strdup(token);
		roomHolder[6][i] = var1;
		i++;
	}
	fclose(fp);

	fp = fopen("flee", "r");
	i = 1;

	while((read = getline(&line, &len, fp)) != -1){
		token = strtok(line, ":");
		strcpy(token, strtok(NULL, "\n"));
		char* var1 = strdup(token);
		roomHolder[7][i] = var1;
		i++;
	}
	fclose(fp);

	printf("CURRENT LOCATION: %s\n", roomHolder[1][1]); //Starting prompt
	printf("POSSIBLE CONNECTIONS: %s, %s, %s.\n", roomHolder[1][2], roomHolder[1][3], roomHolder[1][4]); //Second line to starting prompt

	while(victory == 0){ //while victory has not been secured
		printf("WHERE TO? >"); //ask user for direction
		
		char input[6]; //prep for answer

		fgets(input, sizeof(input), stdin); //receive answer and store it
		printf("\n"); //give space after entry
	
		if(strcmp(input, "tick\n") == 0){ //compare input with given room files, if found update map with relevent location
			printf("CURRENT LOCATION: %s\n", roomHolder[1][1]);
			printf("POSSIBLE CONNECTIONS: %s, %s, %s.\n", roomHolder[1][2], roomHolder[1][3], roomHolder[1][4]);
			victoryLap[moves] = 1; //store moves made
			moves++; //store steps made

		}else if(strcmp(input, "tock\n") == 0){
			printf("CURRENT LOCATION: %s\n", roomHolder[2][1]);
			printf("POSSIBLE CONNECTIONS: %s, %s, %s.\n", roomHolder[2][2], roomHolder[2][3], roomHolder[2][4]);
			victoryLap[moves] = 2;
			moves++;
	
		}else if(strcmp(input, "high\n") == 0){
			printf("CURRENT LOCATION: %s\n", roomHolder[3][1]);
			printf("POSSIBLE CONNECTIONS: %s, %s, %s.\n", roomHolder[3][2], roomHolder[3][3], roomHolder[3][4]);
			victoryLap[moves] = 3;
			moves++;
	
		}else if(strcmp(input, "down\n") == 0){
			printf("CURRENT LOCATION: %s\n", roomHolder[4][1]);
			printf("POSSIBLE CONNECTIONS: %s, %s, %s, %s.\n", roomHolder[4][2], roomHolder[4][3], roomHolder[4][4], roomHolder[4][5]);
			victoryLap[moves] = 4;
			moves++;
	
		}else if(strcmp(input, "duck\n") == 0){
			printf("CURRENT LOCATION: %s\n", roomHolder[5][1]);
			printf("POSSIBLE CONNECTIONS: %s, %s, %s, %s.\n", roomHolder[5][2], roomHolder[5][3], roomHolder[5][4], roomHolder[5][5]);
			victoryLap[moves] = 5;
			moves++;
	
		}else if(strcmp(input, "beav\n") == 0){
			printf("CURRENT LOCATION: %s\n", roomHolder[6][1]);
			printf("POSSIBLE CONNECTIONS: %s, %s, %s, %s.\n", roomHolder[6][2], roomHolder[6][3], roomHolder[6][4], roomHolder[6][5]);
			victoryLap[moves] = 6;
			moves++;
	
		}else if(strcmp(input, "flee\n") == 0){
			printf("CURRENT LOCATION: %s\n", roomHolder[7][1]);
			printf("CONGRATULATIONS! \n"); //Final room met
			victoryLap[moves] = 7;
			moves++;
			victory = 1; //break out of while loop

			printf("PATH TO VICTORY TOOK %d STEPS AND WENT THROUGH \n", moves); //present total number of moves
			int j = 0;
			for(j = 0; j != moves; j++){ //for loop to print rooms covered
				if(victoryLap[j] == 1){
					printf("tick\n");
				}else if(victoryLap[j] == 2){
					printf("tock\n");
				}else if(victoryLap[j] == 3){
					printf("high\n");
				}else if(victoryLap[j] == 4){
					printf("down\n");
				}else if(victoryLap[j] == 5){
					printf("duck\n");
				}else if(victoryLap[j] == 6){
					printf("beav\n");
				}else if(victoryLap[j] == 7){
					printf("flee\n");
				}else{
				}
			}
		}else if(strcmp(input, "time\n") == 0){
			FILE* timeFile; //create variable for new time file
			t = time(NULL); //get time
			printf(ctime(&t)); //print time to user in game
			timeFile = fopen("currentTime.txt", "w+"); //store time in file within subdirectory
			fprintf(timeFile, ctime(&t)); //print time into newly made file
			fclose(timeFile); //close access to file

		}else{
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n"); //bad entries are discarded

		}
	}

	return 0;
}
