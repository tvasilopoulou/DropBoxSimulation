#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
	/*if(access(filename, F_OK) != -1)remove(filename);
	error=0;
	if(!stat(commonDir, &s))error=rmdir(commonDir);
	if(error){
		printf("Directory cannot be removed.\n");
	}
	error=0;
	if(!stat(mirrorDir, &s))error=rmdir(mirrorDir);
	if(error){
		printf("Directory cannot be removed.\n");
	}*/
	return 0;
}

/*
	int error=0;
	struct stat s;
	char * iddir=malloc(sizeof(char)*(strlen(mirrorDir) +1));
	strncpy(iddir, mirrorDir, strlen(mirrorDir) +1);
	iddir=concat(iddir, "/");
	iddir=concat(iddir, id);
	createDirectories(inputDir, mirrorDir, commonDir, iddir);

							//Creation of directories
//	int error=stat(inputDir, &s);		//inputDir should alredy exist
//	if(error) {
//		perror("inputDir");
//		exit(-1);
//	}
//	error=mkdir(mirrorDir, PERMS);		//since we need to create the file if it does not exist, mkdir is faster (1 less use of stat())
//	if(error){
//		printf("Directory already exists.\n");
//		exit(-1);
//	}
//	error=mkdir(commonDir, PERMS);		//since we need to create the file if it does not exist, mkdir is faster (1 less use of stat())
//	
//	error=mkdir(iddir, PERMS);
//	printf("Directories created.\n");
	
	filename=concat(id, ".id");		//fix filename with path
	char * tempcommonDir=concat(commonDir, "/");
	filename=concat(tempcommonDir, filename);
	printf("filename %s\n", filename);
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
    char * name=malloc(sizeof(itoa(getpid(), name)+1));
    strncpy(name, itoa(getpid(), name), strlen(itoa(getpid(), name))+1);
	fputs(name, file);		//add process id in file and close
	fclose(file);
	FILE * logfile=fopen(logFile, "w");		//open new file to write
	if (!logfile){
		printf("Unable to create file.\n");
        exit(-1);
	}

	int fd, i=0;						//https://www.thegeekstuff.com/2010/04/inotify-c-program-example/
	fd = inotify_init ();
	if (fd < 0)perror ("inotify_init");

	int wd;   
	wd = inotify_add_watch(fd,commonDir, IN_CREATE | IN_DELETE);
	if (wd < 0)perror ("inotify_add_watch");
	char buffer[EVENT_BUF_LEN];
	int length=read(fd, buffer, buffSize );
	if(length<0)perror("read");
	//int pids[2];
	char * fifo1, * fifo2;
	int flag=0;
	int pida, pidb;
	fileNode * head= malloc(sizeof(fileNode));
	head=createFileNode(head);
	listFilesRecursively(inputDir, head);
	fileNode * temp=head->next;
	while ( i < length ) {
		struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ]; 
		if ( event->len ) {			//in here creation of pipe!
			if ( event->mask & IN_CREATE ){
				if ( !(event->mask & IN_ISDIR) ) printf( "New file %s created.\n", event->name );
				int fds[2];
				fifo1=malloc(sizeof(char)*strlen(commonDir));
				fifo1=concat(commonDir, "/");
				fifo1=concat(fifo1, "id1_to_id2.fifo");
				fifo2=concat(commonDir, "/");
				fifo2=concat(fifo2, "id2_to_id1.fifo");
				mkfifo(fifo1, 0666);
				error=stat(fifo1, &s);
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
				errno=0;
				if (!(pida=fork())){						//make this into functions!!!
					signal(SIGALRM, alarmHandler);
					alarm(30);
					fds[0]=open(fifo1, O_WRONLY|O_NONBLOCK);
					write(fds[0], temp, sizeof(temp));
					close(fds[0]);
					while(temp!=NULL){
						if(strlen(temp->info->fileName)<2){
							temp=temp->next;
							continue;
						}
						char * message="File ";
						message=concat(message, temp->info->fileName);
						message=concat(message, " passed, ");
						char * size=malloc(sizeof(itoa(sizeof(temp), size)+1));
						strncpy(size, itoa(sizeof(temp), size), strlen(itoa(sizeof(temp), size))+1);
						message=concat(message, size);
						message=concat(message, " bytes read\n");
						char buf[100];
						while(fgets(buf, 100, logfile)!=NULL){
							if(strstr(buf, message)==0)continue;
						}
						fputs(message, logfile);
						char * path=malloc(sizeof(char)*(strlen((temp->info->fileName)+10)+1));
						strncpy(path, (temp->info->fileName)+10, strlen((temp->info->fileName)+10)+1);
						path[strlen(path)-3]='\0';
						char * new=malloc(sizeof(char)*(strlen(iddir)+1));
						strncpy(new, iddir, strlen(iddir)+1);
						new=concat(new, "/");
						char * token=strtok((temp->info->fileName)+10, "/");
						while(token!=NULL){
							if(strlen(token)>3) {new=concat(new, token); new=concat(new, "/"); error=mkdir(new, PERMS);}
							else break;
							token=strtok(NULL, "/");
						}
						printf("old is %s\n", temp->info->fileName);
						new=concat(new, token);
						printf("new path %s\n", new );
						free(temp->info->fileName);
						temp->info->fileName=malloc(sizeof(char)*(strlen(new)+1));
						strncpy(temp->info->fileName, new, strlen(new)+1);
						usleep(10);
						if(errno && errno!= 9 && errno!=17){
							printf("Error #%d:", errno );
							perror(NULL);
							if(unlink(fifo1)) perror("unlink fifo1:");
							if(unlink(fifo2)) perror("unlink fifo2:");
							exit(-1);	//return and send signal to parent process
						}
						temp=temp->next;
						free(path);
						free(new);

					}
					flag=1;
				}
				
				temp=head->next;
				if (!(pidb=fork())){
					fds[1]=open(fifo2, O_RDONLY);
					read(fds[1], temp, sizeof(temp));
					close(fds[1]);
					temp=temp->next;
				}
				if (pida==-1 || pidb==-1){
					perror("fork");
				}
				else if(pida>0 || pidb>0){		//if back to parent, all files transfered 
					if(flag==1){printf("All files have successfully been transfered to the mirror directory.\n");} 
				}
			} 
			else if ( event->mask & IN_DELETE ) {
				if ( !(event->mask & IN_ISDIR) ) printf( "File %s deleted.\n", event->name );
			}
	    }
    	i += EVENT_SIZE + event->len;
    }

	close( fd );

	
	usleep(100);
	fclose(logfile);
//	deleteFileLinkedList(head);
//	if(fifo1) unlink(fifo1);
//	if(fifo2) unlink(fifo2);
//	free(fifo1);
//	free(fifo2);
//	wait(NULL);
//	printf("Mirror client exiting normally...Exit.\n");
//	kill(0, SIGKILL);
	return 0;
}*/