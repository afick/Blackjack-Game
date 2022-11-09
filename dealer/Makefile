# Makefile for 'dealer' module
#
# Final Project
# CS 50, Fall 2022

# utils macro
U = ../utils
# common directory macro

OBJS = dealer.o ../cards/cards.o ../network/network.o
LIBS = $U/utils.a

# comment the following to turn off verbose memory logging
# TESTING=-DTESTING
TRAIN=-DTRAIN

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TRAIN) -I$U
CC = gcc
MAKE = make

.PHONY: all test clean

all: dealer
dealer: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@


test: dealer
	./dealer

dealer.o: $U/bag.h $U/mem.h ../cards/cards.h ../network/network.h 



clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f dealer
	rm -f data/*
	rm -f core
	rm -f $(OBJS)
