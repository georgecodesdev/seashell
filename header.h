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

pid_t pid = 1; 
sigjmp_buf ctrlc_buf;	

bool runningProcess = false;
bool bypass = false;
char *userInput;
char *compareMe;
char *dateTime;
size_t bufSize = 100;
int i = 0;
bool fileOutRedir;
char *filePathForOut;
FILE *fp;

void ls();
void echo(char *);
void sleepMe(char *);
void pwd();
int runCommand(char *, int);
void overrideCtrlC();
void takeInput();
void printStats();
