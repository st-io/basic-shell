# Usage:
# make					# create shell binary
# make clean		# remove shell binary and objects

.PHONY: clean

CC = gcc

mysh: mysh.o mysh-functions.o
	cc -o mysh mysh.o mysh-functions.o

mysh.o: mysh.c mysh.h 
	cc -c mysh.c

mysh-functions.o:
	cc -c mysh-functions.c

clean:
	rm -f mysh mysh.o mysh-functions.o
			
