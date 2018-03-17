#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h> 
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <ctype.h>

extern pid_t pid; 
extern sigjmp_buf ctrlc_buf;	

extern bool runningProcess;
extern bool bypass;
extern char *userInput;
extern char *compareMe;
extern char *dateTime;
extern size_t bufSize;
extern int i;
extern bool fileOutRedir;
extern char *filePathForOut;
extern FILE *fp;

void ls();
void echo(char *);
void sleepMe(char *);
void pwd();
int runCommand(char *, int);
void overrideCtrlC();
void takeInput();
void printStats();
void removeSpaces(char *);
int main();
