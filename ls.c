#include "header.h"

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
