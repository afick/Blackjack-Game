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

all: dealer ncdealer
dealer: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

ncdealer: ncdealer.o ../cards/cards.o ../network/network.o $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -lncurses -o $@

dealertest_player:$(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

test: 
	bash -v testing.sh &> testing.out 

# test dealer with test_dealer and test_player
# run 'make test_dealer' first, then run 'make test_player'
#test_dealer: dealer
#	./dealer > testing_dealer.out
#
#test_player: dealertest_player
#	./player > testing_player.out

dealer.o: $U/bag.h $U/mem.h ../cards/cards.h ../network/network.h 
../network/network.o: ../network/network.h 
../cards/cards.o: ../cards/cards.h


clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f dealer
	rm -f ncdealer
	rm -f testing.out
	rm -f dealer.out
	rm -f data/*
	rm -f core
	rm -f $(OBJS)
