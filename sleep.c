#include "header.h"

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
