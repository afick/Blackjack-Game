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
#include <string.h>
#include <unistd.h>
#include "../network/network.h"
#include <mem.h>
#include <file.h>
#include "../cards/cards.h"
#include <bag.h>

/***** Global Variables ****/

// This defines the dimensions of 
#define max_player_points 20
#define max_dealer_points 11
#define number_actions 2

float Q[max_player_points][max_dealer_points][number_actions] = {0};
int Q_count[max_player_points][max_dealer_points][number_actions] = {0};

// loadQTables loads the q tables
void loadQTables() {
	FILE* qfile = fopen("data/qtable", "r");
	FILE* qcountfile = fopen("data/qtablecount", "r");

#ifdef TRAIN
	printf("test of flag");
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
					mem_free(currline);
				}

				if ((currline = file_readLine(qcountfile))) {
					Q_count[i][j][k] = atoi(currline);
					mem_free(currline);
				}
			}
		}
	}

	fclose(qfile);
	fclose(qcountfile);
}

// saveQTables saves the tables to disk
void saveQTables() {
	FILE* qfile = fopen("data/qtable", "w");
	FILE* qcountfile = fopen("data/qtablecount", "w");

	mem_assert(qfile, "Error openin Q table in saving function");
	mem_assert(qcountfile, "Error opening Q table in saving function");

	for (int i = 0; i < max_player_points; i++) {
		for (int j = 0; j < max_dealer_points; j++) {
			for (int k = 0; k < number_actions; k++) {
				fprintf(qfile, "%f\n", Q[i][j][k]);
				fprintf(qcountfile, "%d\n", Q_count[i][j][k]);
			}
		}
	}

	fclose(qfile);
	fclose(qcountfile);
}

#ifdef TRAIN
// rewardSaver saves the reward to the bag
void rewardSaver(void* arg, void* item) {
	int* reward = arg;
	int* round = item;
	round[3] = *reward;
} 

// roundbagSaver saves the round matrix to the table
void roundbagSaver(void* arg, void* item) {
	int* round = item;
	int player_points = round[0];
	int dealer_points = round[1];
	int action = round[2];
	int reward = round[3];
	Q_count[player_points][dealer_points][action] += 1;
	Q[player_points][dealer_points][action] += (1/(float)Q_count[player_points][dealer_points][action]) * (reward - Q[player_points][dealer_points][action]);
}
#endif

// play method begins playing a game
void play(char* player_name, char* ip_address, int port) {
	// Begin networking set up	
	int socket = connectToDealer(ip_address, port);
	if (socket <= 0) exit(99); 

	// Joinning game
	char joinmessage[30];
	sprintf(joinmessage, "JOIN %s", player_name);
	
	if (sendMessage(socket, joinmessage) == -1) exit(99);	
	
	// Recieve Begin message
	char* beginMessage = readMessage(socket);
	if (beginMessage == NULL) {
		sleep(2);
		if ((beginMessage = readMessage(socket)) == NULL) exit(99);
	}

	if (strcmp(beginMessage, "BEGIN")) {
		fprintf(stderr, "%s\n", "Unexpected Begin message");
		exit(99);
	}
	
	while (!strcmp(beginMessage,"BEGIN")) {
		mem_free(beginMessage);

#ifdef TRAIN
		// Set up new round bag
		bag_t* roundbag = mem_assert(bag_new(), "Round bag not created in play function in TRAIN mode");
#endif

		// Setting up hands
		hand_t* phand = mem_assert(newHand(), "Unable to create hand in play function");
		hand_t* dhand = mem_assert(newHand(), "Unable to create hand in play function");
	
		// Getting player hand
		char* cardm = readMessage(socket);
		if (cardm == NULL) {
			sleep(2);
			if ((cardm = readMessage(socket)) == NULL) exit(99);
		}	
	
		card_t* card = mem_assert(newPlayerCard(cardm), "New card was not created in play function");
		addToHand(phand, card);
		mem_free(cardm);
	
		cardm = readMessage(socket);
		if (cardm == NULL) {
			sleep(2);
			if ((cardm = readMessage(socket)) == NULL) exit(99);
		}
	
		card = mem_assert(newPlayerCard(cardm), "New card was not created in play function");
		addToHand(phand, card);
		mem_free(cardm);
	
		// Getting dealer hand
		cardm = readMessage(socket);
		if (cardm == NULL) {
			sleep(2);
			if ((cardm = readMessage(socket)) == NULL) exit(99);
		}
	
		card = mem_assert(newPlayerCard(cardm), "New card was not created in play function");
		addToHand(dhand, card);
		mem_free(cardm);
	
		// Begin playing game
		char* decm = readMessage(socket);
		if (decm == NULL) {
			sleep(2);
			if ((decm = readMessage(socket)) == NULL) exit(99);
		}
	
		if (strcmp(decm, "DECISION")) {
			fprintf(stderr, "%s\n", "Unexpected decision message");
			exit(99);
		} 
	
		mem_free(decm);
	
		char* dec = mem_malloc_assert(sizeof(char)*6, "dec");
		int decnum;
	
		do {
			// Deciding next move
#ifdef TRAIN
			if ((rand() % 2)) {
				dec = "HIT";
				decnum = 0;
			} else {
				dec = "STAND";
				decnum = 1;
			}
#else
			int ppoints = getHandScore(phand);
			int dpoints = getHandScore(dhand);
	
			float hit_chance = Q[ppoints][dpoints][0];
			float stand_chance = Q[ppoints][dpoints][1];
	
			if (hit_chance > stand_chance) {
				dec = "HIT";
				decnum = 0;
			} else {
				dec = "STAND";
				decnum = 1;
			}
#endif
			if (sendMessage(socket, dec) == -1) {
				sleep(2);
				if (sendMessage(socket, dec) == -1) {
					fprintf(stderr, "%s\n", "Sending message to dealer with move was not possible");
					exit(99);
				}
			}

#ifdef TRAIN
			// Recording round
			int* round = mem_calloc_assert(4, sizeof(int), "Unable to create round recording in play function in TRAIN mode");
			round[0] = getHandScore(phand);
			round[1] = getHandScore(dhand);
			round[2] = decnum;
			bag_insert(roundbag, round);
#endif
	
			if (!strcmp(dec, "STAND")) break;
	
			// Get card
			cardm = readMessage(socket);
			if (cardm == NULL) {
				sleep(2);
				if ((cardm = readMessage(socket)) == NULL) exit(99);
			}
	
			card = mem_assert(newPlayerCard(cardm), "New card was not created in play function");
			addToHand(phand, card);
			mem_free(cardm);
			
			// Verify Decision Message
			decm = readMessage(socket);
			if (decm == NULL) {
				sleep(2);
				if ((decm = readMessage(socket)) == NULL) exit(99);
			}
	
			if (strcmp(decm, "DECISION")) {
				fprintf(stderr, "%s\n", "Unexpected decision message");
				exit(99);
			}
			mem_free(decm);
			mem_free(dec);
		} while (1);
		
		// Getting match result
		char* result = readMessage(socket);
		if (result == NULL) {
			sleep(10);
			if ((result = readMessage(socket)) == NULL) exit(99);
		}
	
		printf("Match Result: %s\n", result);
	
#ifdef TRAIN
		// Saving reward integer
		int reward;
		if (!strcmp(result, "WIN")) {
			reward = 1;
		} else if (!strcmp(result, "LOOSE")) {
			reward = -1;
		} else if (!strcmp(result, "BUST")) {
			reward = -1;
		} else if (!strcmp(result, "PUSH")) {
			reward = 0;
		} else {
			fprintf(stderr, "%s\n", "Unexpected reward obtained in play function");
			exit(99);
		}

#endif
		mem_free(result);
	
		// Freeing memory
		deleteHand(phand);
		deleteHand(dhand);
		
		beginMessage = readMessage(socket);	
		if (beginMessage == NULL) {
			sleep(2);
			if ((beginMessage = readMessage(socket)) == NULL) exit(99);
		}

#ifdef TRAIN
		// Record Reward
		bag_iterate(roundbag, &reward, rewardSaver);

		// Record match
		bag_iterate(roundbag, NULL, roundbagSaver);

		// Freeing round bag
		bag_delete(roundbag, mem_free);
#endif
	}

	if (strcmp(beginMessage, "QUIT")) {
		fprintf(stderr, "%s\n", "Strange Message Received");
		exit(0);
	}

	mem_free(beginMessage);

	// Beaking connection
	closeClientConnection(socket);
}	

int main(int argc, char* argv[]) {
	
	//Ensure argument integrity
	if (argc != 4) {
		fprintf(stderr, "%s\n", "Not enough arguments. Usage: player <player name> <server’s IP address> <PORT number>");
		return 1;
	}

	if (atoi(argv[3]) <= 0) {
		fprintf(stderr, "%s\n", "PORT passed is invalid");
		return 1;
	}

	//Extracting variables
	char* player_name = argv[1];
	char* ip_address = argv[2];
	int port = atoi(argv[3]);

	loadQTables();

	play(player_name, ip_address, port);

	saveQTables();
}
