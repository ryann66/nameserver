CC=gcc
CXX=g++
CFLAGS=
DFLAGS=-g -Og -DDEBUG
BFLAGS=-O2
LFLAGS=
LIBS=

debug: override CFLAGS+=$(DFLAGS)
debug: all

build: override CFLAGS+=$(BFLAGS)
build: all

all: server

server: server.o register.o
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

register.o: register.c register.h server.h
	$(CC) $(CFLAGS) -c -o $@ $<

server.o: server.c server.h register.h utils.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -f *.o server
