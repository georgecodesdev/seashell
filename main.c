#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* TO implement in the 'basic version'
 * ls 
 * pwd
 * echo
 * sleep
 *quit program on ^D
 */

int main(int argc, char *argv){
	bool run = true;
	char *userInput;
	int bufSize = 100;
		
	char *checkMe = "ls";
	userInput = (char *)malloc(bufSize * sizeof(char));
	
	while (run){
		printf("# ");
		
		if (fgets(userInput,bufSize,stdin) == NULL){
			printf("\n");
			break;
		}

		int len = strlen(userInput) - 2; //for some reason the strlen doesnt actually get the correct num chars -- idk why
		printf("\n\nWhat does this do %c\n\n",userInput[len]);
		
		if (strcmp(userInput, checkMe) == 0){
			printf("You just entered: %s",userInput);
			printf("\n");
		}
		
	}
}
