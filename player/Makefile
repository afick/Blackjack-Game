# Makefile for 'player' module
#
# Final Project
# CS 50, Fall 2022

# utils macro
U = ../utils
# common directory macro

OBJS = player.o ../cards/cards.o ../network/network.o
LIBS = $U/utils.a

# comment the following to turn off verbose memory logging
# TESTING=-DTESTING
TRAIN=-DTRAIN

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TRAIN) -I$U -fsanitize=address
CC = gcc
MAKE = make

.PHONY: all test clean

all: player
player: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@


test: player
	./player Name 129.170.64.104 8092

player.o: $U/bag.h $U/mem.h $U/file.h ../cards/cards.h ../network/network.h 



clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f player
	rm -f data/*
	rm -f core
	rm -f $(OBJS)
