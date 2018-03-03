#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/* TO implement in the 'basic version'
 * pwd
 * echo
 * sleep
 *quit program on ^D
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


int main(){
	bool run = true;
	char *userInput;
	char *compareMe;
	int bufSize = 100;

	userInput = (char *)malloc(bufSize * sizeof(char));
	
	while (run){
		printf("# ");
		
		if (fgets(userInput,bufSize,stdin) == NULL){
			printf("\n");
			break;
		}
		
		/* Allocating the correct amount of mem to the compare array */
		int len = strlen(userInput) - 2; //for some reason the strlen doesnt actually get the correct num chars -- idk why
		compareMe = (char*)malloc(len * sizeof(char));
		
		/* IDK if there is a better way of doing this, because it looks not efficient */
		for (int i = 0; i <= len; i++){
			compareMe[i] = userInput[i];
		}
		
		/* comparing what the user typed vs what we are looking for */
		if (strcmp(compareMe, "ls") == 0){ //if the user wants to list the files in the dir
			ls();
			printf("\n");	
		}
		else if (len == 2){ //if the user inputted 'pwd'

		}
		else if (len > 2){ //if the user did not
			printf("This is what you entered: %s\n",compareMe);
		}
		free(compareMe);	
	}
}



