# Makefile for CS50 Blackjack Final Project network module
# 
# Di Luo
# Nov. 6 2022
# CS 50, Fall 2022

# object files, and the target library
OBJS = network.o
LIB = network.a

# $^ refers to “the dependencies listed in this rule”
# $@ refers to “the target of this rule”
# add -DAPPEST for functional tracking report
# add -DMEMTEST for memory tracking report
# (and run `make clean; make` whenever you change this)
FLAGS = # -DAPPTEST # -DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

.PHONY: test valgrind clean

########### network ##################

$(LIB): $(OBJS)
	ar cr $(LIB) $^

# network source dependencies
network.o: network.h

# test network module with networktest_dealer and networktest_player
test: $(LIB)
	$(CC) $(CFLAGS) networktest_dealer.c $(LIB) -o dealer
	$(CC) $(CFLAGS) networktest_player.c $(LIB) -o player

# clean up after our compilation
clean:
	rm -f core
	rm -f *.a *~ *.o *.out network