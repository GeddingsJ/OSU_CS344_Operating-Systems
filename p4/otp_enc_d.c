#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>

void error(const char *msg){ perror(msg); exit(1); }

int main(int argc, char *argv[]){

	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;

	//PREPARE CONNECTIONS
	if(argc < 2){ fprintf(stderr, "USAGE: %s port\n", argv[0]); exit (1); }	
	memset((char *)&serverAddress, '\0', sizeof(serverAddress));
	portNumber = atoi(argv[1]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//SET YOUR SOCKETS
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if(listenSocketFD < 0) error("ERROR: opening socks\n");

	//LISTEN 
	if(bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		error("ERROR: on binding\n");
	listen(listenSocketFD, 5); //receive up to 5 connections

	//ACCEPT DATA UNLESS FULL
	int pidChild[10]; //Array for storing child processes
	int numChild = 0; //Active child
	int childExitMethod = 0; //Storing exit values

	pid_t parentPid = getpid(); //Get parent session ID
	pid_t spawnPid = -5; //Initialize spawnPid

	while(1){
		//ACCEPT CLIENT 
		sizeOfClientInfo = sizeof(clientAddress);
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
		if(establishedConnectionFD < 0) error("ERROR: on accept\n");
	
		//TAKE CLIENT INFO
		//--
		char finalBuffer[150000];
		memset(finalBuffer, '\0', sizeof(finalBuffer));
		char buffer[1000];
		while(strstr(finalBuffer, "$$") == NULL){
			memset(buffer, '\0', sizeof(buffer));
			charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer) - 1, 0);

			if(charsRead < 0) error("ERROR: reading from socket\n");
			strcat(finalBuffer, buffer);
		}
		int loc = strstr(finalBuffer, "$$") - finalBuffer;
		finalBuffer[loc] = '\0';
		//

		spawnPid = fork();
		
		switch(spawnPid){		
		case -1: //IF SWITCH CASE FAILS
			perror("Case -1 Error\n");
			exit(1);
			break;

		case 0:{ //CHILD PROCESS GOES HERE
			char *token;
			char *plaintext;
			char *key;
			int tick = 0;
			int plainSize = 0;
		
			token = strtok(finalBuffer, "@");
			while(token != NULL){ //While token is not null
				if(tick == 0){ //If plaintext portion
					plaintext = token;
				}else if(tick == 1){ //If key portion
					key = (char *)malloc(sizeof(token + 1));
					key = token;
					strcat(key, "\n");
				}
				token = strtok(NULL, "@\n"); //Move token on
				tick++;
			}
			plainSize = strlen(plaintext); //Size of data to encrypt
		
			//ENCRYPT THE GIVEN TEXT
			char pTemp = '\0'; //Single given char value
			char kTemp = '\0';
			char fTemp = '\0';
			int pVal = 0; //Char ASCII value
			int kVal = 0; 
			int lapC = 0;
			int sum = 0;
		
			char final[150000]; 
			memset(final, '\0', sizeof(final));
		
			while(plainSize != 0){ //While there is still text to work with
				pTemp = plaintext[lapC]; //Acquire single char from array
				kTemp = key[lapC];
			
				pVal = pTemp; 
				kVal = kTemp;
				pVal = pVal - 64; //Remove ASCII's prior characters
				kVal = kVal - 64;
				if(pTemp == '\n'){ //If newline keep it
					sum = 10;
				}else if(kTemp == '\n'){
					sum = 10;
				}else{ //Else convert
					if(pTemp == 32){ //If <space> set to 27
						pVal = 27;
					}
					if(kTemp == 32){
						kVal = 27;
					}
					sum = pVal + kVal; //Add char value with key's char value
					if(sum > 27){ //If value is greater than 27
						sum -= 27; //Loop back around
					}
					if(sum == 27){
						sum = 32; //Set value to <space>
					}else{
						sum += 64; //Once done set to ASCII equivalent value
					}
				
				}
			
				fTemp = (char) sum; //Convert back to char
				final[lapC] = fTemp; //Store char into array
					
				lapC++;
				plainSize--;
			}
			//SEND SUCCESS MSG TO CLIENT
			strcat(final, "$$");
			int tock = 0;
			while(tock < strlen(final)){
				charsRead = send(establishedConnectionFD, final, strlen(final), 0); //Send data back
				if(charsRead < 0) error("ERROR: writing to socket\n");
				tock += charsRead;
			}
			
			break;
		}
		default: //Store child process
			pidChild[numChild] = spawnPid;
			numChild++;
			
			break;
		}

		waitpid(spawnPid, &childExitMethod, 0); //Wait for child process completion
		close(establishedConnectionFD);
	}
	close(listenSocketFD);
	
	return 0;
}
