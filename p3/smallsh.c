//Program 3: smallsh.c
//Author: Jacob Geddings
//Purpose: Simulate the basic workings of a shell by using built-in commands as well as exec()
//    alongside signal interruption and redirection to acheive this goal.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>

int mode = 0;

void catchSIGINT(int signo){ //catch for SIGINT with the sigaction struct
	signal(SIGINT, catchSIGINT);
}

void catchSIGUSR2(int signo){ //catch for SIGTSTP with the sigaction struct
	char* enter = "Entering foreground only mode: \n"; //Notifications for entering and exiting foreground mode
	char* exit = "Exiting foreground only mode: \n";
	if(mode == 0){ //If presently NOT in foregound only mode
		write(STDOUT_FILENO, enter ,32); //Send prompt to terminal
		mode = 1; //Set to indicate it's mode has changed
	}else{
		write(STDOUT_FILENO, exit, 31);
		mode = 0;
	}
}

int main( void ){
	char *userInput[2048]; //Array for storing tokenized input
	char *userRequest = NULL; //for storing getline() input
	
	int tester; //for testing/tracking purposes
	int pidChild[10]; //Array for storing child PID's
	int numChild = 0; //Tracker for number of children
	
	size_t size = 256; 
	char *token;
	int tick = 0; //Loop tracker
	int childExitMethod = 0; //For storing how child process ended
	int i = 0; //Loop tracker
	int j = 0; //Loop tracker
	
	pid_t parentPid = getpid(); //Get current PID for parent(shell)
	pid_t spawnPid = -5; //For storing child PID on creation '-5' to clearly see changes
	int status = 0; //Tracking what the status of process is
	int statSig = 0; //Tracking last signal kill number
	
	userRequest = (char *) malloc(size); //Prepare for getline() input
	memset(userInput, '\n', sizeof(userInput)); //wipe contents
	memset(pidChild, '\n', sizeof(pidChild)); //wipe contents

	struct sigaction SIGINT_action = {0}, SIGUSR2_action = {0}, ignore_action = {0}; //struct for handling sigaction members
	SIGINT_action.sa_handler = catchSIGINT; //For redirect
	sigfillset(&SIGINT_action.sa_mask); //Protect parent from signal
	SIGINT_action.sa_flags = 0; 
	
	SIGUSR2_action.sa_handler = catchSIGUSR2; //Intended for handling SIGTSTP (CTRL + Z)
	sigfillset(&SIGUSR2_action.sa_mask);
	SIGUSR2_action.sa_flags = 0;
	
	ignore_action.sa_handler = SIG_IGN; //Final layer of protection in preserving shell

	sigaction(SIGINT, &ignore_action, NULL); //Catch for SIGINT
	sigaction(SIGTSTP, &SIGUSR2_action, NULL); //Catch for SIGTSTP

	while(tick == 0){ //Only way to flip this is through exit command
		printf(":"); //Indicator for user input
		fflush(stdout); //Ensure contents made it through to user
		int numCom = 0; //Track number of individual words submitted
		int bg = 0; //Track if background process was requested
		int sourceFD, targetFD, result; //For handling dup2() and exec()
		memset(userInput, '\n', sizeof(userInput)); //memset for each loop
		memset(userRequest, '\n', sizeof(userRequest));

		fflush(stdin); //ensure input is cleaned for user
		clearerr(stdin); //prevent bad results in the event that getline get's interrupted
		getline(&userRequest, &size, stdin); //take user input in
		clearerr(stdin); //check again to prevent bad results if interrupted

		token = strtok(userRequest, " \n"); //tokenizer established
		while(token != NULL){ //run until completion
			userInput[numCom++] = token; //store token contents into array
			token = strtok(NULL, " \n"); //take next word, note space delimeter
		}

		if(numCom == 0){ //if empty submission just skip and try again
		
		}else{
			char* parPid; //Parent PID will be stored here 
			parPid = malloc(100); //Allocate memory to it
			snprintf(parPid, 100, "%d", parentPid); //Convert parentPid into a char*

			for(j = 0; j < numCom; j++){ //loop through each submitted word
				if(strcmp(userInput[j], "$$") == 0){ //if $$ is found
					strcpy(userInput[j], parPid); //replace with parent PID
				}
			}

			if(strcmp(userInput[numCom - 1], "&") == 0){ //If background process request is present
				bg = 1; //Declare it as a background
				if(mode != 0){ //Unless we're in foreground only mode
					bg = 0; //If that is the case revert back to normal
				}
				strcpy(userInput[numCom - 1], "\n"); //Regardless, remove the '&' off the back
				numCom--; //Reduce number of words
			}

			if(strcmp(userInput[0], "cd") == 0){ //if request to cd
				char* home;
				home = getenv("HOME"); //determine home environment 

				if(numCom <= 1){ //if just 'cd' is submitted
					chdir(home); //send them HOME
				}else{
					chdir(userInput[1]); //else go to the requested path
				}

			}else if(strcmp(userInput[0], "status") == 0){ //if status is requested
				if(status == 0){ //report the exit value that was last set
					printf("exit status 0\n");
					fflush(stdout);
				}else if(status == 1){
					printf("exit status 1\n");
					fflush(stdout);
				}else if(status == 2){ //if last death was by signal
					printf("terminated by signal %d\n", statSig); //say what the singal number was
					fflush(stdout);
				}

			}else if(strcmp(userInput[0], "exit") == 0){ //if exit is requested
				tick = 1; //make this the final loop

			}else if(strncmp(userInput[0], "#", 1) == 0){ //if comment is requested
				for(i = 0; i < numCom; i++){ //loop through each word 
					printf("%s", userInput[i]); //print the word 
					fflush(stdout);
					printf(" "); //add a space between them to be legible
					fflush(stdout);
				}
				printf("\n");
				fflush(stdout);
	
			}else if(strcmp(userInput[0], "\n") == 0){ //if bogus entry disregard and try again

			}else{	
				spawnPid = fork(); //create new child process
				switch (spawnPid){ //divert the path between parent and child
					case -1: //case for an error -- close it down
						perror("Case -1: Error\n");
						fflush(stdout);
						exit(1);
						break;
					
					case 0: //case for child
						SIGINT_action.sa_handler = SIG_DFL; //restore signal commands against children
						sigaction(SIGINT, &SIGINT_action, NULL); //now vulnerable to SIGINT

						if(numCom > 1){	//if there is more than one word
							if(strcmp(userInput[1], "<") == 0){ //if second word is an '<'
								sourceFD = open(userInput[2], O_RDONLY); //set source file to third word and make it read only
								fcntl(sourceFD, F_SETFD, FD_CLOEXEC); //safety net for cleanup
								result = dup2(sourceFD, 0); //set sourceFD as stdin
								if(result == -1) { perror("source dup2()"); fflush(stdout); status = 1; exit(1); } //error handling
								if(numCom >= 2){ //if there are more than three words
									targetFD = open(userInput[4], O_WRONLY | O_CREAT | O_TRUNC, 0644); //establish target file
									fcntl(targetFD, F_SETFD, FD_CLOEXEC);
									result = dup2(targetFD, 1); //set target as stdout

									execlp(userInput[0], userInput[0], NULL); //run exec now that redirection is complete		
									perror("Exec() error [1]"); //if something goes wrong
									fflush(stdout);
									if(result == -1) { perror("target dup2()\n"); fflush(stdout); status = 1; exit(1); } //additional error check
								}else{
									execlp(userInput[0], userInput[0], NULL); //if only source and no target is present, execute this way
									perror("Exec() error [2]");
									fflush(stdout);
									if(result == -1) { perror("source dup2()"); fflush(stdout); status = 1; exit(1); }
								}
							}else if(strcmp(userInput[1], ">") == 0){ //if second word is '>'
								targetFD = open(userInput[2], O_WRONLY | O_CREAT | O_TRUNC, 0644); //establish second word as destination
								fcntl(targetFD, F_SETFD, FD_CLOEXEC);
								result = dup2(targetFD, 1);
								if(result == -1) { perror("target dup2()\n"); fflush(stdout); status = 1; exit(1); }

								execlp(userInput[0], userInput[0], NULL); //execute
								perror("Exec() error [3]");
								fflush(stdout);
							}else{
								userInput[numCom] = NULL; //if unkown additional command
								if(execvp(userInput[0], userInput) < 0){ //send it through here
									perror("Exec() error [4]");
									fflush(stdout);
									status = 1;
								}
							}
						}else{ //if single word command -- execute here
							userInput[numCom] = NULL;
							if(execvp(userInput[0], userInput) < 0){
								perror("Exec() error [5]");
								fflush(stdout);
								status = 1;
							}
						}
						exit(0);
						tick = 1;
						break;
					
					default:
						if(bg != 0){ //if background process was requested
							printf("Background pid is %d\n", spawnPid); //state child PID
						
							pidChild[numChild] = spawnPid; //store child PID into an array
							numChild++;
							status = 0; //status is reassigned
						}						
						break;
					
				}
				if(bg == 0){ //if foreground process
					waitpid(spawnPid, &childExitMethod, 0); //wait until it's completed then parent can continue
					if(WIFEXITED(childExitMethod)){
						//normal termination
					}else{
						printf("PID %d has died: signal that terminated is %d\n", spawnPid, childExitMethod); //if signal terminated
						status = 2; //set warnings
						statSig = 2;
					}
				}

				for(i = 0; i < numChild; i++){ //search through active children
					spawnPid = waitpid(pidChild[i], &childExitMethod, WNOHANG); //check if they're done but don't wait for them
					
					if(spawnPid != 0){ 
						if(spawnPid == -1){ //if child already died
						}else{
							if(WIFEXITED(childExitMethod)){ //normal terminated
								printf("PID %d has died: exit value is %d\n", spawnPid, childExitMethod);
							}else{ //signal terminated
								printf("PID %d has died: signal that terminated is %d\n", spawnPid, childExitMethod);
								
							}
						}
					}
				}
			}
		}
	}

	return 0;
}
