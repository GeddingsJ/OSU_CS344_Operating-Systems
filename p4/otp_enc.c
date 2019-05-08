#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>

void error(const char *msg){ perror(msg); exit(1); } //Function to report problems

int main(int argc, char *argv[]){

	//CHECK CHAR COUNT OF PLAINTEXT AND KEYGEN
	FILE *fp;
	char plainCh, keyCh;
	int plainInt = 0;
	int keyInt = 0;
	int size;
	
	//RETRIEVE FIRST FILE
	fp = fopen(argv[1], "r"); 
	if(fp){
		while((plainCh = getc(fp)) != EOF){
			if(plainCh != ' ' && plainCh != '\n'){
				plainInt++;
			}
		}
	}else{
		error("Bad plaintext entry.");
	}
	
	//RETRIEVE SECOND FILE
	fp = fopen(argv[2], "r");
	if(fp){
		while((keyCh = getc(fp)) != EOF){
			if(keyCh != ' ' && keyCh != '\n'){
				keyInt++;
			}
		}
	}
	fclose(fp);
	
	//CHECK IF KEY IS TOO SHORT
	if(keyInt < plainInt){ 
		error("Key shorter than plaintext");
		exit(1);
	}

	//COMBINE FILE CONTENTS
	char *first;
	char *second;
	char *middle = "@@\n";
	char final[150000];
	size_t sizeT;

	first = (char *)malloc(plainInt);
	second = (char *)malloc(keyInt);

	memset(first, '\n', sizeof(first));
	memset(second, '\n', sizeof(second));

	fp = fopen(argv[1], "r"); //select file
	sizeT = plainInt;
	getline(&first, &sizeT, fp); //get file contents
	fclose(fp);

	fp = fopen(argv[2], "r"); //select file
	sizeT = keyInt;
	getline(&second, &sizeT, fp); //get file contents
	fclose(fp);

	int illegalCheck = 0;
	for(illegalCheck = 0; illegalCheck < strlen(first); illegalCheck++){
		int tem = (int)first[illegalCheck];
		if((tem < 65) || (tem > 90)){
			if((tem == 32) || (tem == 10)){

			}else{
				fprintf(stderr, "ERROR: Illegal characters submitted\n");
				exit(1);
			}
		}
	}

	memset(final, '\n', sizeof(final));
	snprintf(final, sizeof(final), "%s@@\n%s", first, second);
	
	//CONSTRUCT SERVER CONNECTION
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];

	if(argc < 3){ fprintf(stderr, "USAGE: %s hostname port\n", argv[0]); exit(0); }

	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	portNumber = atoi(argv[3]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverHostInfo = gethostbyname("localhost");
	
	if(serverHostInfo == NULL){ fprintf(stderr, "CLIENT: ERROR, no host\n"); exit(0); }

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	//SOCKET CREATION
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if(socketFD < 0) error("CLIENT: ERROR opening socket\n");

	//CONNECTION CREATION
	if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		error("CLIENT: ERROR connecting\n");

	//SEND DATA TO SERVER
	strcat(final, "$$");
	
	int tock = 0;
	while(tock < strlen(final)){
		charsWritten = send(socketFD, final, strlen(final), 0);
		if(charsWritten < 0) error("CLIENT: ERROR writing to socks\n");
		if(charsWritten < strlen(final)) printf("CLIENT: WARNING: Not all data written to socks!\n");

		tock += charsWritten;
	}

	//

	//int checkSend = -5;
	//do{
	//	ioctl(socketFD, TIOCOUTQ, &checkSend);
	//}while(checkSend > 0);
	//if(checkSend < 0) error("ioctl error occured");

	//RECEIVE DATA FROM SERVER
	//
	char ending[1000];
	memset(final, '\0', sizeof(final));
	while(strstr(final, "$$") == NULL){
		memset(ending, '\0', sizeof(ending));
		charsRead = recv(socketFD, ending, sizeof(ending) - 1, 0);
	
		if(charsRead == -1) {
			error("CLIENT: Server data failed to go through");
			exit(1);
		}
	
		strcat(final, ending);
	}
	int tick = strstr(final, "$$") - final;
	final[tick] = '\0';
	
	//
	printf("%s", final);
	
	close(socketFD);
	return 0;	
}
