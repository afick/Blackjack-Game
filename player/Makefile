# Makefile for 'player' module
#
# Final Project
# CS 50, Fall 2022

# utils macro
U = ../utils
# common directory macro

OBJS = player.o ../cards/cards.o ../network/network.o
LIBS = $U/utils.a

# uncomment to train the player
TRAIN=-DTRAIN

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$U -fsanitize=address
TCFLAGS = -Wall -pedantic -std=c11 -ggdb $(TRAIN) -I$U  -fsanitize=address
CC = gcc
MAKE = make

.PHONY: all test clean

all: player

player: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

train: $(OBJS) $(LIBS)
	$(CC) $(TCFLAGS) $^ $(LIBS) -o $@

player.o: $U/bag.h $U/mem.h $U/file.h ../cards/cards.h ../network/network.h 

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f player
	rm -f train
	rm -f core
	rm -f $(OBJS)
