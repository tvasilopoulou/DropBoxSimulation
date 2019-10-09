# DropBoxSimulation
A simple exchange of files between parallel executions

READ ME

Project#: 2

Compilation command: make
Execution command: ./mirror_client -n 1 -c ./common -i ./1_input -m ./1_mirror -b 100 -l log_file1
					(./mirror_client -n 2 -c ./common -i ./2_input -m ./1_mirror -b 100 -l log_file2)

Execution commands for bash: ./create_infiles.sh 1_input 10 5 2
							 cat log_file1 log_file2 | ./get_stats.sh

1. Bash Scripts

(i). create_infiles.sh
This script starts by checking if the requested file already exists, and if so, aborts. Next, the 
given values are inspected in case of NaN values. After that, following the dfs approach as
requested, until there are no more directories to create, a new one is created in every level,
with a random alphanumerical string (8 chars). 
Following the creation of directories, using find, we locate all directories in a list, along with
their subdirectories. For every one of these, until we run out of files, to create, we continue creating
in dfs, as above. In every file, using a random amount of bytes in the range [1,128] KBs. 

(ii). get_stats.sh
This script starts by checking the command line and making sure no arguements were given. For every 
line of input (here requested input was the concatenation of every log_file) we calculate the number
of clients as the number of different ids in cat. In the same way and using simple calculations we 
determine all other values requested.

2. C Script
The program starts by checking and allocating space for the arguements. Next, the signal handlers are set. 
The signal handlers, consist of Ctrl-C(SIGINT) and Ctrl-\(SIGQUIT) and (SIGALARM) which in this case 
is the requested (SIGUSR). In the first 2 cases, the mirror directory and the id file of the common
directory are deleted. The alarm signal does not change other than letting the client know their time
quantum is over. From there on, the execution goes on to create the requested folders, check that the 
input directories are already present and that no mirror directories exist before this step. Inside the
common directory a file with the client's id is created and their process id is written inside. Now the 
client opens their logfile to write and a list of all the files the input directory contains (and therefore
need to be copied to the mirror directory) is created recursively.

From this point forward, the implementation of the communication protocol begins. First a watch is placed
onto the common direcotory in case of new clients connection to the system. Once this is done is time for
the parent process to fork 2 children processes and create the named pipes. Every child has a quantum of 30
seconds to process any data they need. For every file that exists in the list and is passed through the pipe
on the write end, a specific message concerning its name(aka path) and its size is printed in the clients 
logfile. In case an error occurs(that isn't of type "File exists") the child unlinks the fifo and exits to
 the parent process(instead of sending the SIGUSR). The second child process behaves accordingly.

Once control has reached the parent process, if the flag (once transfer is complete, a child process increments
this) is one the system lets the user know  that the transfer is completed. If the flag has not been incremented,
the parent has 3 tries to get it right until they exit with -1 return value. 

In case a client exits and their id file is removed from the common directory, a new child is forked to remove
their id_mirror/id directory. 

Before exiting, any open file descriptors are closed, any allocated memory is freed, the pipes are unlinked,
the list of files is deleted and any still running processes are killed (last part could be omitted without 
problem.)
