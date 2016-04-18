#
#	Makefile
#
#	@author Joey Comeau
#
#	Notes:
#		$@ - the filename of the target
#		$^ - the names of all the dependencies separated by spaces, but 
#			with duplicate names removed
#		-c - flag says to generate the object file
#		-g - flag adds debugging information to the executable file

CC = gcc
CFLAGS = -Wall -g

all: client server

server:
	$(CC) $(CFLAGS) -o $@ assign1_server.c

client:
	$(CC) $(CFLAGS) -o $@ assign1_client.c

clean:
	rm -r server client *.dSYM

