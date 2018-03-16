#include "header.h"

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
