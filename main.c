#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <wait.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include "fileLinkedList.h"
#include "header.h"
#define PERMS 0777
#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 *(EVENT_SIZE + 16))

/*   ./mirror_client -n 1 -c ./common -i ./1_input -m ./1_mirror -b 100 -l log_file1		*/
/*   ./mirror_client -n 2 -c ./common -i ./2_input -m ./2_mirror -b 100 -l log_file2		*/

char * commonDir, * inputDir, * mirrorDir, * filename;		//global

int main(int argc, char const *argv[]){
	if (argc!=13)
	{
		printf("Please try entering Dropbox again. Number of arguments was invalid.\n");
		exit(-1);
	}
    //check arguements in pairs
	if ((strcmp(argv[1],"-n") && strcmp(argv[3],"-n") && strcmp(argv[5],"-n") && strcmp(argv[7],"-n") && strcmp(argv[9],"-n") && strcmp(argv[11],"-n"))
	 || (strcmp(argv[1],"-c") && strcmp(argv[3],"-c") && strcmp(argv[5],"-c") && strcmp(argv[7],"-c") && strcmp(argv[9],"-c") && strcmp(argv[11],"-c"))
	 || (strcmp(argv[1],"-i") && strcmp(argv[3],"-i") && strcmp(argv[5],"-i") && strcmp(argv[7],"-i") && strcmp(argv[9],"-i") && strcmp(argv[11],"-i"))
	 || (strcmp(argv[1],"-m") && strcmp(argv[3],"-m") && strcmp(argv[5],"-m") && strcmp(argv[7],"-m") && strcmp(argv[9],"-m") && strcmp(argv[11],"-m"))
	 || (strcmp(argv[1],"-l") && strcmp(argv[3],"-l") && strcmp(argv[5],"-l") && strcmp(argv[7],"-l") && strcmp(argv[9],"-l") && strcmp(argv[11],"-l"))
	 || (strcmp(argv[1],"-b") && strcmp(argv[3],"-b") && strcmp(argv[5],"-b") && strcmp(argv[7],"-b") && strcmp(argv[9],"-b") && strcmp(argv[11],"-b")))
	{
		printf("Please try entering the Bitcoin Network again. Arguments given were either in the wrong order, or incorrect.\n");
		exit(-2);
	}
	int buffSize;
	char *id, * logFile;
/*-n*/
	//set variables assigned from input
	if (!(strcmp(argv[1],"-n"))){id=malloc(sizeof(argv[2]+1));strncpy(id, argv[2], strlen(argv[2])+1);} 
	else if (!(strcmp(argv[3],"-n"))){id=malloc(sizeof(argv[4]+1));strncpy(id, argv[4], strlen(argv[4])+1);} 
	else if (!(strcmp(argv[5],"-n"))){id=malloc(sizeof(argv[6]+1));strncpy(id, argv[6], strlen(argv[6])+1);} 
	else if (!(strcmp(argv[7],"-n"))){id=malloc(sizeof(argv[8]+1));strncpy(id, argv[8], strlen(argv[8])+1);} 		
	else if (!(strcmp(argv[9],"-n"))){id=malloc(sizeof(argv[10]+1));strncpy(id, argv[10], strlen(argv[10])+1);} 
	else if (!(strcmp(argv[11],"-n"))){id=malloc(sizeof(argv[12]+1));strncpy(id, argv[12], strlen(argv[12])+1);} 

/*-c*/
	if (!(strcmp(argv[1],"-c"))){commonDir=malloc(sizeof(argv[2]+1));strncpy(commonDir, argv[2], strlen(argv[2])+1);}
	else if (!(strcmp(argv[3],"-c"))) {commonDir=malloc(sizeof(argv[4]+1));strncpy(commonDir, argv[4], strlen(argv[4])+1);}
	else if (!(strcmp(argv[5],"-c"))) {commonDir=malloc(sizeof(argv[6]+1));strncpy(commonDir, argv[6], strlen(argv[6])+1);}
	else if (!(strcmp(argv[7],"-c"))) {commonDir=malloc(sizeof(argv[8]+1));strncpy(commonDir, argv[8], strlen(argv[8])+1);}
	else if (!(strcmp(argv[9],"-c"))) {commonDir=malloc(sizeof(argv[10]+1));strncpy(commonDir, argv[10], strlen(argv[10])+1);}
	else if (!(strcmp(argv[11],"-c"))) {commonDir=malloc(sizeof(argv[12]+1));strncpy(commonDir, argv[12], strlen(argv[12])+1);}
/*-i*/
	if (!(strcmp(argv[1],"-i"))){inputDir=malloc(sizeof(argv[2]+1));strncpy(inputDir, argv[2], strlen(argv[2])+1);}
	else if (!(strcmp(argv[3],"-i"))) {inputDir=malloc(sizeof(argv[4]+1));strncpy(inputDir, argv[4], strlen(argv[4])+1);}
	else if (!(strcmp(argv[5],"-i"))) {inputDir=malloc(sizeof(argv[6]+1));strncpy(inputDir, argv[6], strlen(argv[6])+1);}
	else if (!(strcmp(argv[7],"-i"))) {inputDir=malloc(sizeof(argv[8]+1));strncpy(inputDir, argv[8], strlen(argv[8])+1);}
	else if (!(strcmp(argv[9],"-i"))) {inputDir=malloc(sizeof(argv[10]+1));strncpy(inputDir, argv[10], strlen(argv[10])+1);}
	else if (!(strcmp(argv[11],"-i"))) {inputDir=malloc(sizeof(argv[12]+1));strncpy(inputDir, argv[12], strlen(argv[12])+1);}
/*-m*/
	if (!(strcmp(argv[1],"-m"))){mirrorDir=malloc(sizeof(argv[2]+1));strncpy(mirrorDir, argv[2], strlen(argv[2])+1);}
	else if (!(strcmp(argv[3],"-m"))) {mirrorDir=malloc(sizeof(argv[4]+1));strncpy(mirrorDir, argv[4], strlen(argv[4])+1);}
	else if (!(strcmp(argv[5],"-m"))) {mirrorDir=malloc(sizeof(argv[6]+1));strncpy(mirrorDir, argv[6], strlen(argv[6])+1);}
	else if (!(strcmp(argv[7],"-m"))) {mirrorDir=malloc(sizeof(argv[8]+1));strncpy(mirrorDir, argv[8], strlen(argv[8])+1);}
	else if (!(strcmp(argv[9],"-m"))) {mirrorDir=malloc(sizeof(argv[10]+1));strncpy(mirrorDir, argv[10], strlen(argv[10])+1);}
	else if (!(strcmp(argv[11],"-m"))) {mirrorDir=malloc(sizeof(argv[12]+1));strncpy(mirrorDir, argv[12], strlen(argv[12])+1);}
/*-l*/
	if (!(strcmp(argv[1],"-l"))){logFile=malloc(sizeof(argv[2]+1));strncpy(logFile, argv[2], strlen(argv[2])+1);}
	else if (!(strcmp(argv[3],"-l"))) {logFile=malloc(sizeof(argv[4]+1));strncpy(logFile, argv[4], strlen(argv[4])+1);}
	else if (!(strcmp(argv[5],"-l"))) {logFile=malloc(sizeof(argv[6]+1));strncpy(logFile, argv[6], strlen(argv[6])+1);}
	else if (!(strcmp(argv[7],"-l"))) {logFile=malloc(sizeof(argv[8]+1));strncpy(logFile, argv[8], strlen(argv[8])+1);}
	else if (!(strcmp(argv[9],"-l"))) {logFile=malloc(sizeof(argv[10]+1));strncpy(logFile, argv[10], strlen(argv[10])+1);}
	else if (!(strcmp(argv[11],"-l"))) {logFile=malloc(sizeof(argv[12]+1));strncpy(logFile, argv[12], strlen(argv[12])+1);}
/*-b*/
	if (!(strcmp(argv[1],"-b"))) buffSize=atoi(argv[2]);
	else if (!(strcmp(argv[3],"-b"))) buffSize=atoi(argv[4]);
	else if (!(strcmp(argv[5],"-b"))) buffSize=atoi(argv[6]);
	else if (!(strcmp(argv[7],"-b"))) buffSize=atoi(argv[8]);
	else if (!(strcmp(argv[9],"-b"))) buffSize=atoi(argv[10]);	
	else if (!(strcmp(argv[11],"-b"))) buffSize=atoi(argv[12]);	
	/*END OF ARGUMENT CHECK*/
	/*----------------------------------------------------------------------------------------------------------------------------------*/
	setSignalHandlers();		/*Ctrl-C and Ctrl-\*/
	/*----------------------------------------------------------------------------------------------------------------------------------*/
	//struct stat s;						//Creation of directories
	int error=0;
	char * iddir=malloc(sizeof(char)*(strlen(mirrorDir) +1)); //create commonDir, mirrorDir, mirrorDir/iddir
	strncpy(iddir, mirrorDir, strlen(mirrorDir) +1);
	iddir=concat(iddir, "/");
	iddir=concat(iddir, id);
	createDirectories(iddir);				
	/*----------------------------------------------------------------------------------------------------------------------------------*/
	createIdFile(id, getpid());				//create commonDir/id.id
	/*----------------------------------------------------------------------------------------------------------------------------------*/
	FILE * logfile=fopen(logFile, "w");		//open logfile to write
	if (!logfile){
		printf("Unable to create file.\n");
        exit(-1);
	}
	/*----------------------------------------------------------------------------------------------------------------------------------*/
	int fd, i=0;						//https://www.thegeekstuff.com/2010/04/inotify-c-program-example/
	fd = inotify_init ();
	if (fd < 0)perror ("inotify_init");
	int errorCount=0;
	int wd;   
	wd = inotify_add_watch(fd,commonDir, IN_CREATE | IN_DELETE);		//add watch for new client
	if (wd < 0)perror ("inotify_add_watch");
	char buffer[EVENT_BUF_LEN];
	int length=read(fd, buffer, buffSize );
	if(length<0)perror("read");
	char * fifo1, * fifo2;			//named pipes
	int flag=0;
	int pida, pidb;						//for child processes
	fileNode * head= malloc(sizeof(fileNode));		//head of list of files
	head=createFileNode(head);
	listFilesRecursively(inputDir, head);		//create list of files
	fileNode * temp=head->next;
	while ( i < length ) {
		struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ]; 
		if ( event->len ) {			//in here creation of pipe!
			if ( event->mask & IN_CREATE ){
				//fflush(stdin);
				if ( !(event->mask & IN_ISDIR) );
				int fds[2];
				fifo1=concat(commonDir, "/");				//create fifos in common directory
				fifo1=concat(fifo1, "id1_to_id2.fifo");
				fifo2=concat(commonDir, "/");
				fifo2=concat(fifo2, "id2_to_id1.fifo");
				createFifos(fifo1, fifo2, error);

				errno=0;
				if (!(pida=fork())){			//fork() the first child process, fd[0]:write
					signal(SIGALRM, alarmHandler);
					alarm(30);
					fds[0]=open(fifo1, O_WRONLY|O_NONBLOCK);	//open fifo to write
					write(fds[0], temp, sizeof(temp));			//write node into pipe and close
					close(fds[0]);
					while(temp!=NULL){
						if(strlen(temp->info->fileName)<2){		//this is head node, where fileName="\0"
							temp=temp->next;
							continue;
						}
						createMessage(temp, logfile);			//pass message into logfile and move file into mirror dir
						transferFiles(temp, iddir);
						usleep(10);
						if(errno && errno!= 9 && errno!=17){		//obviously omit errors like File exists, we only create new files
							printf("Error #%d:", errno );			//print error number and error message
							perror(NULL);							
							if(unlink(fifo1)) perror("unlink fifo1:");//unlink fifos and exit into parent process
							if(unlink(fifo2)) perror("unlink fifo2:");
							exit(-1);	//return and send signal to parent process
						}
						temp=temp->next;
					}
					flag=1;											//once all nodes passed, mark flag 1 to let parent know transfer is complete
				}
				
				temp=head->next;									//for read end go back to head of list
				if (!(pidb=fork())){
					fds[1]=open(fifo2, O_RDONLY);
					read(fds[1], temp, sizeof(temp));				//set alarm for 30 secs
					signal(SIGALRM, alarmHandler);
					alarm(30);
					if(errno && errno!= 9 && errno!=17){		//obviously omit errors like File exists, we only create new files
						printf("Error #%d:", errno );
						perror(NULL);
						if(unlink(fifo1)) perror("unlink fifo1:");
						if(unlink(fifo2)) perror("unlink fifo2:");		//add counter<=3
						exit(-1);	//return and send signal to parent process
					}
					close(fds[1]);
					while(temp!=NULL){						//no need to transfer files so commented out?
						//transferFiles(temp, iddir);
						temp=temp->next;
					}
					flag=2;									//in case we want to transfer from child 2 also, safety measure
				}
				if (pida==-1 || pidb==-1){					//fork() error
					perror("fork");
				}
				else if(pida>0 || pidb>0){		//if back to parent, all files transfered 
					usleep(10);					//flag=1 || flag=2 are both correct
					if(flag>=1){printf("All files have successfully been transfered to the mirror directory.\n");} 
					else{						//3 tries to get it right, if errorCount>3 exit parent as well
						errorCount++;
						if(errorCount>3){printf("File transfer caused too many errors. Exiting...\n"); exit(-1);}
					}
				}
			} 
			else if ( event->mask & IN_DELETE ) {		//if directory item is deleted
				if ( !(event->mask & IN_ISDIR) ){
					int pidc;
					if (!(pidc=fork())){					//fork() new child to delete mirror/id directory
						remove_directory(iddir);
					}
				}
			}
	    }
    	i += EVENT_SIZE + event->len;
    }

	close( fd );

	/*----------------------------------------------------------------------------------------------------------------------------------*/
	usleep(100);					//free allocated memory, unlink and delete pipes, delete list of file, kill any hanging processes
	fclose(logfile);				//works fine without kill as well
	deleteFileLinkedList(head);
	if(fifo1) unlink(fifo1);
	if(fifo2) unlink(fifo2);
	free(fifo1);
	free(fifo2);
	free(id);
	free(commonDir);
	free(inputDir);
	free(mirrorDir);
	free(iddir);
	free(logFile);
	wait(NULL);
	printf("Mirror client exiting normally...Exit.\n");
	kill(0, SIGKILL);
	return 0;
}