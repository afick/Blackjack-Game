# Makefile for CS50 Blackjack Final Project network module
# 
# Di Luo
# Nov. 6 2022
# CS 50, Fall 2022

# object files, and the target library
OBJS = network.o
LIBS = 

# $^ refers to “the dependencies listed in this rule”
# $@ refers to “the target of this rule”
# add -DAPPEST for functional tracking report
# add -DMEMTEST for memory tracking report
# (and run `make clean; make` whenever you change this)
FLAGS = # -DAPPTEST # -DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -I$L
CC = gcc
MAKE = make

.PHONY: test valgrind clean

########### network ##################

network: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# network source dependencies
network.o: network.h

# clean up after our compilation
clean:
	rm -f core
	rm -f *.a *~ *.o *.out