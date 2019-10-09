#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

typedef struct fileInfo{
	short nameLen;
	char * fileName;
	int fileLen;
	FILE * file;
}fileInfo;

typedef struct fileNode{
	fileInfo * info;
	struct fileNode * next;
}fileNode;

fileInfo * initFileInfo(fileInfo * );
fileInfo * addFileInfo(fileInfo * , short , char * , int , FILE * );
fileNode * createFileNode(fileNode * );
void fileNodeInsert(fileNode * , short , char * , int , FILE * );
int countDocLines(FILE * );
void listFilesRecursively(char *, fileNode *);
void deleteFileLinkedList(fileNode * );
void createMessage(fileNode * , FILE * );
void transferFiles(fileNode * , char * );
