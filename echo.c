#include "header.h"

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
