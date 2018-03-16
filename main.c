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
	/* Going to handle how we open the Directory Stream */
	DIR *dirStream;
	/* Refers to a Directory Entry will be used to find the filename */	
	struct dirent *dirEntry;
	/* Ooening the current dir */
	dirStream = opendir(".");

	/* Checking to see if we are dealing with a file re-direct */
	if (fileOutRedir){
		fp = fopen(filePathForOut, "w");
	}

	/* If we actually have someathing in the current dir to open */
	if (dirStream) {
		/* Reading the current entry from the directory -> saving it into our struct */
		while ((dirEntry = readdir(dirStream)) != NULL) {
    			/* We dont want to show hidden files */ 
			if (dirEntry->d_name[0] != '.'){

				/* If the user wants to redirect the output to a file -> we use the d_name attribute of the struct to print the file name */		
				if (fileOutRedir){
					fprintf(fp," %s ", dirEntry->d_name);	
				}
				/* Using the d_name attribute of the struct to print the file name */
				else { 
					printf(" %s ", dirEntry->d_name);
				}

			}	
    		}
    		closedir(dirStream);
  	}
  
	if (fileOutRedir){
     		fclose(fp);
  	}
}

/* Function that handles the 'echo' command 
 * char* = the message we want to print
 * */
void echo(char *myMessage){
	
	if (fileOutRedir){
		fp = fopen(filePathForOut, "w");
	}

	/* Printing the file output */
	for (int i = 5; i < strlen(myMessage); i++){
		if (fileOutRedir){
			fprintf(fp, "%c", myMessage[i]);
		}
		else {
			printf("%c",myMessage[i]);
		}	
	}

	if (fileOutRedir){
		fclose(fp);
	}
}

/* Function that handels the 'sleep' command */
void sleepMe(char *findNumber){
	char *tempNum = (char*)malloc(strlen(findNumber) * sizeof(char));
	double timeToSleep;

	/* Cleaning up the findNumber string that we end up getting */
	int i = 6; // the index of the first decimal that we want	
	for (int indexOfNum = 0; indexOfNum < strlen(findNumber);  indexOfNum++){
		tempNum[indexOfNum] = findNumber[i];
		i++;
	}
	
	/* Converting the char* to a double */
	sscanf(tempNum, "%lf",&timeToSleep);	
	sleep(timeToSleep);
}

/* Funciton that handles the 'pwd' command */
void pwd(){
	/* Going to hold the current dir path*/
	char tempPath[1000];

	if (fileOutRedir){
		fp = fopen(filePathForOut, "w");
	}
   
	/* Grabbing the current filepath */
  	if (getcwd(tempPath, sizeof(tempPath)) != NULL){
		if (fileOutRedir){
			fprintf(fp, "%s", tempPath);
		}
		else {
       			printf("%s", tempPath);
   		}
	}

	if (fileOutRedir){
		fclose(fp);
	}
}

/* Function that removes spaces */
void removeSpaces(char *formatMe){
	/* Based on the copy, going to loop though and remove the spaces in the inputted char* */
	char *spaceRemover = formatMe;
	/* Going to be used as an incrementor for the spaceRemoveer var */
	char *formatMeCopy = formatMe;

	while (*formatMeCopy != 0){
		/* Assigning the spaceRemover string to equal the char referenced by formatMeCopy */
		*spaceRemover = *formatMeCopy++;

		/* If the current char referenced is NOT a ' ' char, we will allow the spaceRemover to increment to the next char, otherwise it will get over-wrote in the next iteration of the loop */
		if (*spaceRemover != ' '){
			spaceRemover++;
		}
	}
	/* Clears the string */
	*spaceRemover = 0;
}

/* Creates a process to run the command that the original process (soon to be parent) took the user's input for */
int runCommand(char *compareMe, int len){
	/* Initial vars to check for output redirect and cd -- since those are more easily done by the parent process */
	bool userCd = true;
	fileOutRedir = false;


	/* START to check for the output redirect '>' char */
	/* Checking to see if the user wants to redirect the file output by typing '>' */
	for (i = 0; i < len; i++){
		if (compareMe[i] == '>'){
			fileOutRedir = true;
			break;			
		}
	}

	/* If the user wants to redirect the file output, we need to split the string in terms of the '>' char  */
	if (fileOutRedir){
		/* Creating temp vars -- so we can split the string */
		char *strChunk, *str, *tofree;
		tofree = str = strdup(compareMe);  // We own str's memory now.
		/* Used to determeine if we are looking at the XXX > or YYY part of the string */
		int count = 0;

		while ((strChunk = strsep(&str, ">"))){     
			/* If we are looking at the filPath portion of the split string (or the YYY section defined above) */
			if (count == 1){
				/* Removing the spaces from the file path */
				removeSpaces(strChunk);
				/* Allocating memory to the var and copying the filepath over to the global var */
				filePathForOut = (char*)malloc(strlen(strChunk) * sizeof(char));
				strcpy(filePathForOut,strChunk);	
		       	}
			/* If we are looking at the command portion of the split string (or the XXX section defined above) */
		      	else {
			       /* Clearing the mem from the compareMe string, and need to re-size it */
			       free(compareMe);
			       /* Needs to be strlen-1 because thats the lengh that the string would be if it were getting called like this */
			       compareMe = (char*)malloc(strlen(strChunk)-1 * sizeof(char)); 

			       strcpy(compareMe, strChunk);

			       /* Adding the termination character, otherwise everything breaks */
	       		       compareMe[strlen(strChunk)-1] = '\0';		       
		       }
		       count++;
		}
		bypass = true;
		free(tofree);

		/* Checking the filePath */
		fp = (fopen(filePathForOut, "w"));
		
		if (fp == NULL){
			perror("");
			fileOutRedir = false;
		}
		else {
			fclose(fp);
		}
	}
	/* END file output redirect check */

	/* START to check for CD input */
	/* Checking to see if the user wants to cd before we make the child process */
	char *cdCompare = "cd";
	for (i = 0; i < 2; i++){
		if (compareMe[i] != cdCompare[i]){
			userCd = false;
			break;
		}	
	}

	if (userCd){
		/* Going to hold the dir we want to change to */
		char *dirToChangeTo = (char*)malloc(len-2 * sizeof(char));

		/* Just copying over the dir we want to change to, by starting on the 3rd char in the inputted string */
		int startIndex = 0;
		
		for (i = 3; i <= len; i++){
			dirToChangeTo[startIndex] = compareMe[i];
			startIndex++;
		}

		dirToChangeTo[len-2] = '\0';
		
		/* If chdir has any problems, we tell the user */
		if (chdir(dirToChangeTo) != 0){
			fprintf(stderr,"cd: %s: ",dirToChangeTo);
			perror("");
		}

		/* Freeing mem + fixing newline output */
		bypass = true;
		free(dirToChangeTo);
		return 0;
	}
	/* END check for CD input */

	/* Looking at the normal cases (if the user does not want to cd()) */
	pid = fork();
	pid_t waitPid;
	pid_t status;

	/* If we are looking at the parent process, we have it wait until its child has finished */
	if (pid > 0){
		do{
			waitPid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
		return 1;
	}
	/* If we are looking at the child process */
	else if (pid == 0){
		/* checking userinput */

		/* Checking the ls case, this is why we needed to clean up the inputted string */
		if (strcmp(compareMe, "ls") == 0){ 
			ls();	
		}
		/* Checking the 'pwd' case */
		else if (strcmp(compareMe, "pwd") == 0){
			pwd();
		}
		/* Now checking to see if the user wants to 'echo' or 'sleep' */
		else if (len > 2){

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
						break;
					}
				}
			}
			else {
				isSleep = false;
				echo(compareMe);
			}

			if (isSleep){
				sleepMe(compareMe);	
			}
		}
		/* Once the child process has finished the process */
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
	
	/* If we are looking at a child process -- we want to end it */
	if (pid == 0){
		exit(0);
	}
	/* If we are looking at the parent process, we want to print a newline and jump 'back' in the takeInput function so the user can input commands again */
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
	
		/* If the user enteres ^D input */	
		if (feof(stdin)){
			fflush(stdout);
			printf("^D\n");
			exit(0);
		}

		/* Allocating the correct amount of mem to the compare array */
		int len = strlen(userInput) - 2; 
		if (len > 0){
			compareMe = (char*)malloc(len * sizeof(char));

			for (i = 0; i <= len; i++){
				compareMe[i] = userInput[i];
			}
			
			compareMe[i] = userInput[i];
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
