#include "header.h"

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
