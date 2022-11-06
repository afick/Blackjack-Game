/*
 *
 * player.c - Executes the player in the blackjack game
 *
 * usage: player <player name> <server’s IP address> <PORT number>
 * Juan Suarez, Project Team 12, CS 50 Fall 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

/***** Global Variables ****/

// This defines the dimensions of 
int max_player_points = 20;
int max_dealer_points = 11;
int number_actions = 2;

float Q[max_player_points][max_dealer_points][number_actions] = {0};
int Q_count[max_player_points][max_dealer_points][number_actions] = {0};

// loadQTables loads the q tables
void loadQTables() {
	FILE* qfile = fopen("data/qtable", "r");
	FILE* qcountfile = fopen("data/qtablecount", "r");

#ifdef TRAIN
	// If this is the first iteration, there might not be a table
	if (!qfile) {
		if (qcountfile) fclose(qcountfile);
		return;	
	}

	if (!qcountfile) {
		if (qfile) fclose(qfile);
		return;
	}
	
#else
	// There has to be a set of q tables. Otherwise the algorithm won't work
	mem_assert(qfile, "Error opening q table");
	mem_assert(qcountfile, "Error opening q count table");
#endif
	char* currline;

	for (int i = 0; i < max_player_points; i++) {
		for (int j = 0; j < max_dealer_points; j++) {
			for (int k = 0; k < number_actions; k++) {
				if ((currline = file_readLine(qfile))) {
					Q[i][j][k] = atoi(currline);
					free(currline);
				}

				if ((currline = file_readLine(qcountfile))) {
					Q_count[i][j][k] = atoi(currline);
					free(qcountfile);
				}
			}
		}
	}

	fclose(qfile);
	fclose(qcountfile);
}

// saveQTables saves the tables to disk
void saveQTables() {
	FILE* qfile = fopen("data/qtable", "w+");
	FILE* qcountfile = fopen("data/qtablecount", "w+");

	mem_assert(qfile, "Error openin Q table in saving function");
	mem_assert(qcountfile, "Error opening Q table in saving function");

	for (int i = 0; i < max_player_points; i++) {
		for (int j = 0; j < max_dealer_points; j++) {
			for (int k = 0; k < number_actions; k++) {
				fprintf(qfile, "%s\n", Q[i][j][k]);
				fprintf(qcountfile, "%s\n", Q_count[i][j][k]);
			}
		}
	}

	fclose(qfile);
	fclose(qcountfile);
}

// updateQTables updates the table with a new value based on a game
void updateQTables(int player_points, int dealer_points, int action, int reward) {
	Q_count[player_points][dealer_points][action] += 1;
	Q[player_points][dealer_points][action] += (1/(float)Q_count[player_points][dealer_points][action]) * (reward - Q[player_points][dealer_points][action]);
}

int main(int argc, char* argv[]) {
	
	//Ensure argument integrity
	if (argc != 4) {
		fprintf(stderr, "%s\n", "Not enough arguments. Usage: player <player name> <server’s IP address> <PORT number>");
		return 1;
	}

	//Extracting variables
	char* player_name = argv[2];
	char* ip_address = argv[3];
	char* port = argv[4];

	loadQTables();

#ifdef TRAIN
	train(player_name, ip_address, port);
#else
	play(player_name, ip_address, port);
#endif
	saveQTables();
}
