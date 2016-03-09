#! /usr/bin/make

# UNIX style makefile for the linux PIC downloader as found at
# http://www.workingtex.com/htpic

CC=mipsel-linux-gnu-gcc
CFLAGS= -mips32 -static
LDFLAGS=

all: picdl

clean: 
	-rm *.o picdl

picdl: downldr.o linuxio.o gethex.o
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

downldr.o: io.h gethex.h proto.h

linuxio.o: io.h proto.h

gethex.o: gethex.h
