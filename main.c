#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h> 
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

/* TO implement in the 'basic version'
 * pwd
 */


pid_t pid = 1; 
bool runningProcess = false;
bool bypass = false;
char *userInput;
char *compareMe;
char *dateTime;
int bufSize = 100, i = 0;


void ls();
void echo(char *);
void sleepMe(char *);
void pwd();
int runCommand(char *, int);
void overrideCtrlC();
void takeInput();
void printStats();


/* Function that handles the functionality of the 'ls' command */
void ls(){
  /* Going to handle the opening of the files */
  DIR *dirStream;
  struct dirent *dir;
  dirStream = opendir(".");
  /* If we actually have someathing in the current dir to open */
  if (dirStream) {
    while ((dir = readdir(dirStream)) != NULL) {
    	/* We dont want to show hidden files */ 
	if (dir->d_name[0] != '.'){
		 printf(" %s ", dir->d_name);
    	}
    }
    closedir(dirStream);
  }	
}
/* Function that handles the 'echo' command */
void echo(char *myMessage){
	for (int i = 4; i < strlen(myMessage); i++){
		printf("%c",myMessage[i]);
	}
}

void sleepMe(char *findNumber){
	char *tempNum = (char*)malloc(strlen(findNumber) * sizeof(char));
	double timeToSleep;

	int i = 6; // the index of the first decimal that we want	
	for (int indexOfNum = 0; indexOfNum < strlen(findNumber);  indexOfNum++){
		tempNum[indexOfNum] = findNumber[i];
		i++;
	}

	sscanf(tempNum, "%lf",&timeToSleep);	
	sleep(timeToSleep);
}
/* Funciton that handles the 'pwd' command */
void pwd(){
	/* Going to hold the actual info for the file path */
	char temp[1000];
   
	/* Grabbing the current filepath -- if we  */
  	if (getcwd(temp, sizeof(temp)) != NULL){
       		printf("%s", temp);
   	}
}
/* Creates a process to run the command that the original process (soon to be parent) took the user's input for */
int runCommand(char *compareMe, int len){
	pid = fork();

	pid_t waitPid;
	pid_t status;
	
	int i;

	if (pid > 0){
		do{
			waitPid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
		return 1;
	}
	else if (pid == 0){
		/*TODO work on getting the string to split based on the space, so I can take in different flags  */	
		
		/* comparing what the user typed vs what we are looking for */
		if (strcmp(compareMe, "ls") == 0){ //if the user wants to list the files in the dir
			ls();	
		}
		else if (strcmp(compareMe, "pwd") == 0){ //if the user inputted 'pwd'
			pwd();
		}
		else if (len > 2){ //we know at this point it could either be echo or sleep (because they both contain stuff after
			/* Simple compare arrays  */
			char *echoCompare = "echo";
			char *sleepCompare = "sleep";
			bool isSleep = true, isEcho = true;

			/* Going to check to see if we are looking at an 'echo' command */
			for (i = 0; i < 4; i++){
				if (compareMe[i] != echoCompare[i]){
					isEcho = false;
					break;	
				}	
			}
				
			/* If we know the user didnt want to use the echo command, we gotta check for the other possibility "sleep" */
			if (!isEcho){
				for (i = 0; i < 5; i++){
					if(compareMe[i] != sleepCompare[i]){
						isSleep = false;
					}
				}
			}
			else { // we need to rememeber to reset the isSleep
				isSleep = false;
				echo(compareMe);
			}

			if (isSleep){
				sleepMe(compareMe);	
			}
		}
		exit(0);	
		return 0;
	}
	else {
		perror("ERROR: Process did not create correctly!!!.\n");
		exit(0);
		return -1;
	}		
}
/*   */
void overrideCtrlC(){
	char c;
	while ((c = getchar()) != '\n' && c != EOF) { }

	signal (SIGINT, overrideCtrlC);

	if (!runningProcess){	
			free(userInput);
			free(compareMe);
			free(dateTime);

			printf("Look I am doing stuff now\n");
			userInput = (char *)malloc(bufSize * sizeof(char));	
			bypass = true;
			
			printf("This is getting called now?\n");
		}
		else {
			if (pid == 0){
				exit(0);
			}
			else {
				printf("is this correctly being called?");
				if (runningProcess == true){
					printf("\n");
				}
			}
		}
}

void printStats(){
	time_t rawtime;
  	struct tm * timeinfo;

	dateTime = (char *)malloc(40 * sizeof(char));

	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
  	strftime(dateTime,40,"%d/%m %H:%M", timeinfo);

	printf ("[%s]", dateTime );	
	printf("# ");
}

void takeInput(){		
	userInput = (char *)malloc(bufSize * sizeof(char));

	while (true){
		runningProcess = false;
		
		if (!bypass){
			printf("\nbypass is false\n");
			printStats();
			
			if (fgets(userInput,bufSize,stdin) == NULL && !bypass){	
					printf("^D\n");
					fflush(stdout);
					exit(0);			
			}
			else {
				for (int i = 0; i < 100; i++){

				}
			}
			
			/* Allocating the correct amount of mem to the compare array */
			int len = strlen(userInput) - 2; //for some reason the strlen doesnt actually get the correct num chars -- idk why
			compareMe = (char*)malloc(len * sizeof(char));
		
			/* TODO need to figure oiut  */
			for (i = 0; i <= len; i++){
				compareMe[i] = userInput[i];
			}

			/* Somehow this is OK -- I have no idea why  */
			compareMe[i]= '\0';

			runningProcess = true;
			runCommand(compareMe,len);
			free(compareMe);

			printf("\n");
		}
		else {
			fflush(stdout);
			bypass = false;
		}
	}
}

int main(){

	signal(SIGINT, overrideCtrlC);
	takeInput();
}



