#
# In order to execute this "Makefile" just type "make"
#

OBJS 	= main.o functions.o fileLinkedList.o
SOURCE	= main.c functions.c fileLinkedList.c
HEADER  = header.h fileLinkedList.h
OUT  	= mirror_client
CC	= gcc
FLAGS   = -c -g -Wall
flags=$(shell gpgme-config --libs --cflags)
#-g -c -pedantic -ansi  -Wall
# -g option enables debugging mode
# -c flag generates object code for separate files

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

fileLinkedList.o: fileLinkedList.c
	$(CC) $(FLAGS) fileLinkedList.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

gpgme1: gpgme1.c
	$(CC) -D_FILE_OFFSET_BITS=64 -g ${flags} gpgme1.c -o $@

# clean house
clean:
	rm -f $(OBJS) $(OUT) gpgme*
