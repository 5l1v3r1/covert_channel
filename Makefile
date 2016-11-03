CC=gcc
CFLAGS=-c -Wall -pedantic
LDFLAGS=

all:	covert

covert: covert.o
		${CC} ${LDFLAGS} covert.o -o covert
		rm *.o

covert.o: covert.c
		${CC} ${CFLAGS} covert.c

clean:
		rm -rf *.o covert

run:
	./covert
