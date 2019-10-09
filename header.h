#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern char * commonDir, * inputDir, * mirrorDir, * filename;


char * concat(char *, char *);
char* itoa(int , char * );
void catchInterrupt ( int );
void  alarmHandler(int);
void setSignalHandlers();
void createDirectories( char * );
void createIdFile(char * , int );
int remove_directory(char *);
void createFifos(char * ,char *  ,int );