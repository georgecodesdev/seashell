#include "header.h"

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
