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
		if (strcmp(compareMe, "ls") == 0){
			printf("You just entered: %s",userInput);
		}

		free(compareMe);	
		printf("\n");
	}
}
