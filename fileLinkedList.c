#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fileLinkedList.h"
#define PERMS 0777

fileInfo * initFileInfo(fileInfo * info){		//initialise namelen with non-0 value to get last node  correctly
	info->nameLen=-1;
	info->fileName="\0";
	info->fileLen=0;
	info->file=NULL;
	return info;
}

fileInfo * addFileInfo(fileInfo * info, short nameLen, char * fileName, int fileLen, FILE * file){	//give correct values
	info->nameLen=nameLen;																			//to struct variables
	info->fileName=malloc(sizeof(char)*(strlen(fileName) +1));
	strncpy(info->fileName, fileName, strlen(fileName)+1);
	info->fileLen=fileLen;
	info->file=file;
	return info;
}


fileNode * createFileNode(fileNode * node){		//initialise list
	fileInfo * info=malloc(sizeof(fileInfo));
	info=initFileInfo(info);
	node->next=NULL;
	return node;
}

void fileNodeInsert(fileNode * head, short nameLen, char * fileName, int fileLen, FILE * file){		//insert new node to list
	if(head==NULL)return;
	fileNode * temp=head;
	while(temp->next!=NULL)temp=temp->next;
	temp->next=malloc(sizeof(fileNode));
	temp->next=createFileNode(temp->next);
	temp=temp->next;
	temp->info=malloc(sizeof(fileInfo));
	temp->info=addFileInfo(temp->info, nameLen, fileName, fileLen, file);
}

int countDocLines(FILE * file){		//last year's project, count lines by counting characters and incrementing on every \n
	int lines=0;
	char c;
	while(!feof(file)){
		c=fgetc(file);
		if (c=='\n') lines++;
	}
	return lines;
}

void listFilesRecursively(char *basePath, fileNode * head){			//https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
	struct dirent *dp;
	DIR *dir = opendir(basePath);									//open directory
	char * filename=malloc(sizeof(char)*(strlen(basePath) +1));
	char * path=malloc(sizeof(char)*(strlen(basePath) +1));
	if (!dir) return;						//if directory is empty, backtrack
	while ((dp = readdir(dir)) != NULL){	//while contents unread
	    if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){		//ignore directories . and ..
	    	if (dp->d_type != DT_DIR ){											//if content item=file
	    		strncpy(filename, basePath, strlen(basePath)+1);				//initialise filename as full-path directory name
	    		filename=concat(filename, "/");	
	    		filename=concat(filename, dp->d_name);							//add filename in the end (f_n)
	    		FILE * f=fopen(filename, "r");		//open it to read
	    		if (!f){
					printf("Unable to read file.\n");
			        exit(-1);
				}
				int lines=countDocLines(f);			//count lines
				fclose(f);
				fileNodeInsert(head, strlen(filename), filename, lines, f);		//insert file in list of files
	    	}
			strncpy(path, basePath, strlen(basePath)+1);						//now in directory, continue one level and fix dir name
			path=concat(path, "/");
			path=concat(path, dp->d_name);
			listFilesRecursively(path, head);									//recursion forward: one step deeper, (inside every directory)
	    }
	}
	fileNodeInsert(head, 0, "\0", 0, NULL);										//after adding every file in list, add one with nameLen=00
	free(path);
	free(filename);
	closedir(dir);
	}




void deleteFileLinkedList(fileNode * head){				//delete whole list and everything it contains
	fileNode * current=malloc(sizeof(fileNode));
	fileNode * next=malloc(sizeof(fileNode));
	current=head;
	while (current != NULL){
		next = current->next;
		free(current);
		current = next;
	}
	head=NULL;
}


void createMessage(fileNode * temp, FILE * logfile){	//create input for logfile
	int flag=0;
	char * message="File ";				//logfile input
	message=concat(message, temp->info->fileName);		//"File <whole path name/filename> passed, <sizeof(struct)> bytes read."
	message=concat(message, " passed, ");
	char * size=malloc(sizeof(itoa(sizeof(temp), size)+1));
	strncpy(size, itoa(sizeof(temp), size), strlen(itoa(sizeof(temp), size))+1);
	message=concat(message, size);
	message=concat(message, " bytes read\n");
	char buf[100];
	while(fgets(buf, 100, logfile)!=NULL){			//avoid duplicates in logfile
		printf("okokokok\n");
		if(strstr(buf, message))flag=1;
	}
	if(flag==0){									//if no duplicates, add line to file
		fputs(message, logfile);
	}
	free(size);
}


void transferFiles(fileNode * temp, char * iddir){		//from ./n_input to ./n_mirror
	char * path=malloc(sizeof(char)*(strlen((temp->info->fileName)+10)+1));			//exclude the ./n_input part of path
	strncpy(path, (temp->info->fileName)+10, strlen((temp->info->fileName)+10)+1);
	path[strlen(path)-3]='\0';		//exclude the /f_n part (filename)
	if(path[strlen(path)-1]=='/')path[strlen(path)-1]='\0';	//if 10<files<99 exclude one more char
	char * new=malloc(sizeof(char)*(strlen(iddir)+1));		//create new path by concatenating above iddir + leftover + filename
	strncpy(new, iddir, strlen(iddir)+1);
	new=concat(new, "/");
	char * token=strtok((temp->info->fileName)+10, "/");	//tokenize by /
	while(token!=NULL){
		if(strlen(token)>3) {new=concat(new, token); new=concat(new, "/"); mkdir(new, PERMS);} 	//distinguish filename and dirname
		else break;
		token=strtok(NULL, "/");
	}
	new=concat(new, token);
	free(temp->info->fileName);
	temp->info->fileName=malloc(sizeof(char)*(strlen(new)+1));
	strncpy(temp->info->fileName, new, strlen(new)+1);			//change path of file in list
	FILE * tf=fopen(new, "w");						//copy file in current position
	fclose(tf);
	free(path);
	free(new);
}