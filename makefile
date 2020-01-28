# This makefile is intended to build the Smart Pointers solution
# Written by Valentyn Faychuk at SoftServe on 28.01.2020
# email:
# 		faitchouk.valentyn@gmail.com

SHELL = /bin/sh
$(shell mkdir -p build)

ODIR = build
# Add here all the include directories
IDIR = .
# Add here all the sources in the project
SOURCES = main smart-ptr
#A Add here all the libs, needed by the project
CLIBS = -lpthread -lrt

CC = gcc
CFLAGS = -g -I$(IDIR)

.PHONY: all clean smartptr

all: smartptr

smartptr: setup
	$(CC) $(CFLAGS) -o $(ODIR)/smart-ptr $(shell find $(ODIR)/ -type f -name \*.o) $(CLIBS)

setup:
	for SOURCE in $(SOURCES) ; do \
		$(CC) -c $$SOURCE.c $(CFLAGS) -o $(ODIR)/$$SOURCE.o ; \
	done

clean:
	rm -f $(ODIR)/*.o $(ODIR)/smart-ptr

