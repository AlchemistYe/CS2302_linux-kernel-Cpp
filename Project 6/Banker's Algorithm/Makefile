# makefile for banker.c
#

CC=gcc
CFLAGS=-Wall

all: banker.o
	$(CC) $(CFLAGS) -o banker banker.o 

banker.o: banker.c
	$(CC) $(CFLAGS) -c banker.c 

clean:
	rm -rf *.o
	rm -rf banker

