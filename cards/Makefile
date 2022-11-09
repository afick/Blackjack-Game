# Makefile for 'cards' module
#
# Alex Fick, Final Project
# CS 50, Fall 2022

U = ../utils

OBJS = cards.o cardtest.o
LIBS = $U/utils.a

# comment the following to turn off verbose memory logging
TESTING=-DTESTING

CCFLAGS = -Wall -pedantic -std=c11 -ggdb 
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) 
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

cards: $(OBJS)
	$(CC) $(CCFLAGS) $^ $(LIBS) -o $@

cardtest: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

cardtest.o: cards.h 
cards.o: ../utils/bag.h ../utils/mem.h cards.h
../utils/bag.o:  ../utils/bag.h
../utils/mem.o: ../utils/mem.h

test: cardtest
	valgrind --leak-check=full ./cardtest

.PHONY: test valgrind clean


clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f cards
	rm -f core
