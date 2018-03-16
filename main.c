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
#include <setjmp.h>


//TODO need to write the actual data to a file

pid_t pid = 1; 
sigjmp_buf ctrlc_buf;	

bool runningProcess = false;
bool bypass = false;
char *userInput;
char *compareMe;
char *dateTime;
size_t bufSize = 100;
int i = 0;
bool fileOutRedir;
char *filePathForOut;
FILE *fp;

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

	if (fileOutRedir){
		fp = fopen(filePathForOut, "w");
		
		if (fp == NULL){ //need to handle this input || want to create a new file based on the
			printf("here I need to have to make a new file or someathing \n");			
		}
	}

	/* If we actually have someathing in the current dir to open */
	if (dirStream) {
		while ((dir = readdir(dirStream)) != NULL) {
    			/* We dont want to show hidden files */ 
			if (dir->d_name[0] != '.'){

				/* If the user wants to redirect the output to a file */		
				if (fileOutRedir){
					/* Need to figure out how to write the output to a file*/
					fprintf(fp," %s ", dir->d_name);	
				}
				else {
					printf(" %s ", dir->d_name);
				}

			}	
    		}
    		closedir(dirStream);
  	}
  
	if (fileOutRedir){
     		fclose(fp);
  	}
}

/* Function that handles the 'echo' command */
//TODO need to print to a file
void echo(char *myMessage){
	
	if (fileOutRedir){
		fp = fopen(filePathForOut, "w");
	}

	for (int i = 5; i < strlen(myMessage); i++){
		printf("%c",myMessage[i]);
	}
}

/* Function that handels the 'sleep' command */
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
//TODO need to print to a file
void pwd(){
	/* Going to hold the actual info for the file path */
	char temp[1000];
   
	/* Grabbing the current filepath -- if we  */
  	if (getcwd(temp, sizeof(temp)) != NULL){
       		printf("%s", temp);
   	}
}

/* Function that removes spaces -- NEEDS testing */
void RemoveSpaces(char *source){
	char *i = source;
	char *j = source;

	while (*j != 0){
		*i = *j++;
		if (*i != ' '){
			i++;
		}
	}
	*i = 0;
}

/* Creates a process to run the command that the original process (soon to be parent) took the user's input for */
int runCommand(char *compareMe, int len){
	bool userCd = true;
	fileOutRedir = false;

	/* Checking to see if the user wants to redirect the file output by typing '>' */
	for (i = 0; i < len; i++){
		if (compareMe[i] == '>'){
			fileOutRedir = true;
			break;			
		}
	}

	/* If the user wants to redir the file, we want to remove the  */
	if (fileOutRedir){
		char *token, *str, *tofree;

		tofree = str = strdup(compareMe);  // We own str's memory now.
		int count = 0;
		while ((token = strsep(&str, ">"))){
		       if (count == 1){
			       	printf("This should be the file path: %s\n",token);
				/* Removing the spaces from the file path */
				RemoveSpaces(token);
				/* Allocating memory to the var and copying the filepath over to the global var */
				filePathForOut = (char*)malloc(strlen(token) * sizeof(char));
				strcpy(filePathForOut,token);	
		       }
		       else {
			       /* Clearing the mem from the compareMe string, and need to re-size it */
			       free(compareMe);
			       /* Needs to be strlen-1 because thats the lengh that the string would be if it were getting called like this */
			       compareMe = (char*)malloc(strlen(token)-1 * sizeof(char)); 

			       printf("\nThis should be the rest of the text: %s\n",token);
			       strcpy(compareMe, token);
	       		       compareMe[strlen(token)-1] = '\0';		       
		       }
		       count++;
		}
		//TODO at some point I could add funtionality to the up and down arrows, possibly by writing all the commands I am outputting to a file, then reading that file or someathing 
		//then @ the end of the progrm I could close the file
		free(tofree);
		printf("\n\nthe file path \n%s\n",filePathForOut);
		printf("The actual data \n%s\n\n",compareMe);
		
	}

	/* Checking to see if the user wants to cd before we make the child process */
	char *cdCompare = "cd";
	for (i = 0; i < 2; i++){
		if (compareMe[i] != cdCompare[i]){
			userCd = false;
			break;
		}	
	}

	if (userCd){
		//todo need to figure out why this is not wokring --  probs has 
		/* For some reason this works, but doing the exact same with the manual copy breaks */
		char *dirToChangeTo = (char*)malloc(len-2 * sizeof(char));


		int startIndex = 0;
		
		for (i = 3; i <= len; i++){
			dirToChangeTo[startIndex] = compareMe[i];
			startIndex++;
		}

		dirToChangeTo[len-2] = '\0';
		
		if (chdir(dirToChangeTo) != 0){
			fprintf(stderr,"cd: %s: ",dirToChangeTo);
			perror("");
		}

		bypass = true;
		return 0;
	}

	pid = fork();
	pid_t waitPid;
	pid_t status;

	if (pid > 0){
		do{
			waitPid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
		return 1;
	}
	else if (pid == 0){		
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

/* Function to over-ride ^C input to make it act like the normal shell */
void overrideCtrlC(){

	signal(SIGINT, overrideCtrlC);
	
	if (pid == 0){
		exit(0);
	}
	else {
		printf("\n");
		siglongjmp(ctrlc_buf, 1);
	}	
	
}

/* Print time/date in the command line */
void printStats(){
	time_t rawtime;
  	struct tm * timeinfo;

	dateTime = (char *)malloc(40 * sizeof(char));

	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
  	strftime(dateTime,40,"%d/%m %H:%M", timeinfo);

	printf ("[%s]", dateTime );	
	printf("# ");
	free(dateTime);
}

/* Take input from the user, 'cleanes' up the input to call the 'runcommand' function */
void takeInput(){		
	userInput = (char *)malloc(bufSize * sizeof(char));

	while (true){
		
		while (sigsetjmp(ctrlc_buf, 1) != 0 );
		bypass = false;

		printStats();
		getline(&userInput,&bufSize,stdin);
		
		if (feof(stdin)){
			fflush(stdout);
			printf("^D\n");
			exit(0);
		}

		/* Allocating the correct amount of mem to the compare array */
		int len = strlen(userInput) - 2; //for some reason the strlen doesnt actually get the correct num chars -- idk why
		if (len > 0){
			compareMe = (char*)malloc(len * sizeof(char));

			/* TODO need to figure oiut  */
			for (i = 0; i <= len; i++){
				compareMe[i] = userInput[i];
			}
			compareMe[i] = userInput[i];

			/* Somehow this is OK -- I have no idea why  */
			compareMe[i]= '\0';
		
			runningProcess = true;
			runCommand(compareMe,len);
			free(compareMe);
			
			if (!bypass){
				printf("\n");
			}

			fflush(stdout);
		}
	}
}

int main(){
	signal(SIGINT, overrideCtrlC);
	takeInput();
}
