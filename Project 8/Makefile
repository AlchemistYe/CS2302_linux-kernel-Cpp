# makefile for virtual memory manager

CC=gcc
CFLAGS=-Wall

all: virtualMemory.o checker.o
	$(CC) $(CFLAGS) -o virtualMemory virtualMemory.o
	$(CC) $(CFLAGS) -o checker checker.o

virtualMemory.o: virtualMemory.c
	$(CC) $(CFLAGS) -c virtualMemory.c

checker.o: checker.c
	$(CC) $(CFLAGS) -c checker.c

clean:
	rm -rf *.o
	rm -rf virtualMemory
	rm -rf checker

