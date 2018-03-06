#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h> 

/* TO implement in the 'basic version'
 * pwd
 */

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
	printf("\n");
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
       		printf("%s\n", temp);
   	}
}

int main(){
	char *userInput;
	char *compareMe;
	int bufSize = 100, i = 0;

	userInput = (char *)malloc(bufSize * sizeof(char));

	while (true){
		printf("# ");

		if (fgets(userInput,bufSize,stdin) == NULL){
			printf("^D\n");
			fflush(stdout);
			break;			
		}

		/* Allocating the correct amount of mem to the compare array */
		int len = strlen(userInput) - 2; //for some reason the strlen doesnt actually get the correct num chars -- idk why
		compareMe = (char*)malloc(len * sizeof(char));
		
		/* IDK if there is a better way of doing this, because it looks not efficient */
		for (i = 0; i <= len; i++){
			compareMe[i] = userInput[i];
		}
		
		/* comparing what the user typed vs what we are looking for */
		if (strcmp(compareMe, "ls") == 0){ //if the user wants to list the files in the dir
			ls();
			printf("\n");	
		}
		else if (len == 2){ //if the user inputted 'pwd'
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

			/* De-allocating the mem for the compare arrays  -- since we dont need them anymore */
			//TODO need to work on that
		}
		free(compareMe);	
	}
}



