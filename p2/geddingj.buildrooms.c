//Program 2 - geddingj.buildrooms.c
//Author: Jacob Geddings 
//Create new subdirectory and populate with files to be used as a game for geddingj.adventure.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void){

	int bufferSize = 20; 
	int pid = getpid(); //Store a PID for appending to subdirectory
	FILE* roomFile;

	char* fileName = malloc(bufferSize); //Prepare place in memory for fileName
	char* prefix = "geddingj.rooms."; //prefix for subdirectory set

	srand(pid);
	int dice;

	snprintf(fileName, bufferSize, "%s%d", prefix, pid); //full filename created for use as subdirectory

	struct stat st = {0};	//Starting up stat 

	if(stat(fileName, &st) == -1){ //If filename doesn't already exist
		mkdir(fileName, 0755); //create the new subdirectory with r/w/e
		chdir(fileName); //change active directory to newly created subdirectory

		FILE* file_pointer; //pointer to new file being made
		file_pointer = fopen("tick", "w+"); //creates empty file for read/write
		fprintf(file_pointer, "ROOM NAME: tick\n"); //first line to be written into file
		fprintf(file_pointer, "CONNECTION 1: tock\n"); //second line, etc.
		fprintf(file_pointer, "CONNECTION 2: high\n");
		fprintf(file_pointer, "CONNECTION 3: down\n");
		fprintf(file_pointer, "ROOM TYPE: START_ROOM\n");
		fclose(file_pointer); //close access to 'tick' file

		file_pointer = fopen("tock", "w+"); //repeat above step for all active maps
		fprintf(file_pointer, "ROOM NAME: tock\n");
		fprintf(file_pointer, "CONNECTION 1: tick\n");
		fprintf(file_pointer, "CONNECTION 2: high\n");
		fprintf(file_pointer, "CONNECTION 3: duck\n");
		fprintf(file_pointer, "ROOM TYPE: MID_ROOM\n");
		fclose(file_pointer);
		
		file_pointer = fopen("high", "w+");
		fprintf(file_pointer, "ROOM NAME: high\n");
		fprintf(file_pointer, "CONNECTION 1: tick\n");
		fprintf(file_pointer, "CONNECTION 2: tock\n");
		fprintf(file_pointer, "CONNECTION 3: beav\n");
		fprintf(file_pointer, "ROOM TYPE: MID_ROOM\n");
		fclose(file_pointer);

		file_pointer = fopen("down", "w+");
		fprintf(file_pointer, "ROOM NAME: down\n");
		fprintf(file_pointer, "CONNECTION 1: tick\n");
		fprintf(file_pointer, "CONNECTION 2: beav\n");
		fprintf(file_pointer, "CONNECTION 3: flee\n");
		fprintf(file_pointer, "CONNECTION 4: duck\n");
		fprintf(file_pointer, "ROOM TYPE: MID_ROOM\n");
		fclose(file_pointer);

		file_pointer = fopen("duck", "w+");
		fprintf(file_pointer, "ROOM NAME: duck\n");
		fprintf(file_pointer, "CONNECTION 1: tock\n");
		fprintf(file_pointer, "CONNECTION 2: beav\n");
		fprintf(file_pointer, "CONNECTION 3: down\n");
		fprintf(file_pointer, "CONNECTION 4: flee\n");
		fprintf(file_pointer, "ROOM TYPE: MID_ROOM\n");
		fclose(file_pointer);

		file_pointer = fopen("beav", "w+");
		fprintf(file_pointer, "ROOM NAME: beav\n");
		fprintf(file_pointer, "CONNECTION 1: high\n");
		fprintf(file_pointer, "CONNECTION 2: down\n");
		fprintf(file_pointer, "CONNECTION 3: duck\n");
		fprintf(file_pointer, "CONNECTION 4: flee\n");
		fprintf(file_pointer, "ROOM TYPE: MID_ROOM\n");
		fclose(file_pointer);

		file_pointer = fopen("flee", "w+");
		fprintf(file_pointer, "ROOM NAME: flee\n");
		fprintf(file_pointer, "CONNECTION 1: beav\n");
		fprintf(file_pointer, "CONNECTION 2: duck\n");
		fprintf(file_pointer, "CONNECTION 3: down\n");
		fprintf(file_pointer, "ROOM TYPE: END_ROOM\n"); 
		fclose(file_pointer);
		
	}	
	
	return 0;
}
