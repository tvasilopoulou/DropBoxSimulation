#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <dirent.h>
#include "header.h"
#define PERMS 0777



char * concat(char *s1, char *s2){			//https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char * result = malloc(len1 + len2 + 1); 		//stick s2 in the end of s1 (like strncat but without causing a segmentation fault)
    if(!result){
		printf("Something related to memory allocation went wrong.\n");
		exit(-1);
    }
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    return result;
}

char* itoa(int i, char * b){			//https://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
	char const digit[] = "0123456789";		//covert integer to string
	char* p = b;
	if(i<0){
		*p++ = '-';
		i *= -1;
	}
	int shifter = i;
	do{ //Move to where representation ends
		++p;
		shifter = shifter/10;
	}while(shifter);
	*p = '\0';
	do{ //Move back, inserting digits as u go
		*--p = digit[i%10];
		i = i/10;
	}while(i);
	return b;
}

void catchInterrupt ( int signo ) {
	printf ( "\nCatching : signo =% d \n" , signo ) ;
	int error=0;
	struct stat s;
	if(!stat(mirrorDir, &s))error=rmdir(mirrorDir);
	if(error){
		printf("Directory cannot be removed.\n");
	}
	if(access(filename, F_OK) != -1)remove(filename);
}

void alarmHandler(int sig){					//reinitialise alarm signal
	signal(SIGALRM, SIG_IGN);
	printf("Time quantum over! Please return.\n");
	alarm(30);								//time quantum=30sec
	signal(SIGALRM, alarmHandler);
	exit(-1);								//exit for parent
}

void setSignalHandlers(void){
	static struct sigaction act;				//in case of interrupt or quit
	act.sa_handler = catchInterrupt ;			//call handler
	sigfillset(&( act.sa_mask )) ;
	sigaction( SIGINT , &act , NULL ) ;			//signals for interrupts
	sigaction( SIGQUIT , &act , NULL ) ;
	signal(SIGALRM, alarmHandler);				//SIGUSR
}

void createDirectories(char * iddir){
	struct stat s;						//Creation of directories
	int error=stat(inputDir, &s);		//inputDir should alredy exist
	if(error) {
		perror("inputDir");
		exit(-1);
	}
	error=mkdir(mirrorDir, PERMS);		//since we need to create the file if it does not exist, mkdir is faster (1 less use of stat())
	if(error){
		printf("Directory already exists.\n");
		exit(-1);
	}
	error=mkdir(commonDir, PERMS);		//since we need to create the file if it does not exist, mkdir is faster (1 less use of stat())
	error=mkdir(iddir, PERMS);
	printf("Directories created.\n");
}

void createIdFile(char * id, int pid){
	filename=concat(id, ".id");		//fix filename with path
	char * tempcommonDir=concat(commonDir, "/");			//in common directory
	filename=concat(tempcommonDir, filename);
	FILE * file=fopen(filename, "w");		//open new file to write
	if (!file){
		printf("Unable to create file.\n");
        exit(-1);
	}
	fseek(file, 0, SEEK_END);			//count bytes until the end
    if(ftell(file)!=0){					//if more than 0, something already exists
    	printf("Different client working on this particular file right now.\n");
    	exit(-1);
    }
    char * name=malloc(sizeof(itoa(pid, name)+1));
    strncpy(name, itoa(pid, name), strlen(itoa(pid, name))+1);
	fputs(name, file);		//add process id in file and close
	free(name);
	fclose(file);
}

void createFifos(char * fifo1,char * fifo2,int error){		//create named pipes in common directory
	struct stat s;
	fifo1=concat(commonDir, "/");
	fifo1=concat(fifo1, "id1_to_id2.fifo");
	fifo2=concat(commonDir, "/");
	fifo2=concat(fifo2, "id2_to_id1.fifo");
	mkfifo(fifo1, 0666);
	error=stat(fifo1, &s);			//if pipes exist, exit
	if(error) {
		perror("fifo1");
		exit(-1);
	}
	mkfifo(fifo2, 0666);	//later open and write/read
	error=stat(fifo2, &s);
	if(error) {
		perror("fifo2");
		exit(-1);
	}
}

int remove_directory(char *path){		//https://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c/42978529
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;
	if (d){
		struct dirent *dir;
		r = 0;
		while (!r && (dir=readdir(d))){			//while contents still there
			int r2 = -1;
			char *buf;
			size_t len;
			if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) continue;	//ignore content directories . and ..
			len = path_len + strlen(dir->d_name) + 2; 
			buf = malloc(len);
			if (buf){
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, dir->d_name);
				if (!stat(buf, &statbuf)){
					if (S_ISDIR(statbuf.st_mode)){r2 = remove_directory(buf);}
					else{r2 = unlink(buf);}		//unlink and remove
				}
				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}
	if (!r) r = rmdir(path);
	return r;
}