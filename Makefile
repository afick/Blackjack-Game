# Makefile for CS50 Blackjack final project
# CS 50, Fall 2022

U = utils
.PHONY: all clean

############## default: make all libs and programs ##########
all: 
	make -C utils
	make -C player
	make -C dealer
	make -C cards
	make -C network


############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C utils clean
	make -C player clean
	make -C dealer clean
	make -C cards clean
	make -C network clean
