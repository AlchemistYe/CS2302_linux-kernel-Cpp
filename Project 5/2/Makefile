# makefile for p_c.c
#

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: p_c.o buffer.o
	$(CC) $(CFLAGS) -o p_c p_c.o buffer.o $(PTHREADS)

p_c.o: p_c.c
	$(CC) $(CFLAGS) -c p_c.c $(PTHREADS)

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c

clean:
	rm -rf *.o
	rm -rf p_c

