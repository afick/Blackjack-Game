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
#define max_player_points 21
#define max_dealer_points 11
#define number_actions 2

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

	for (int i = 4; i < max_player_points; i++) {
		for (int j = 2; j < max_dealer_points; j++) {
			for (int k = 0; k < number_actions; k++) {
				if ((currline = file_readLine(qfile))) {
					sscanf(currline, "%f", &Q[i][j][k]);
					mem_free(currline);
				}

				if ((currline = file_readLine(qcountfile))) {
					sscanf(currline, "%d", &Q_count[i][j][k]);
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

	for (int i = 4; i < max_player_points; i++) {
		for (int j = 2; j < max_dealer_points; j++) {
			for (int k = 0; k < number_actions; k++) {
				fprintf(qfile, "%f\n", Q[i][j][k]);
				fprintf(qcountfile, "%d\n", Q_count[i][j][k]);
			}
		}
	}

	fclose(qfile);
	fclose(qcountfile);
}


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
	Q_count[player_points-1][dealer_points-1][action] += 1; // Throws invalid read in a couple cases
	Q[player_points-1][dealer_points-1][action] += (1/(float)Q_count[player_points-1][dealer_points-1][action]) * (reward - Q[player_points-1][dealer_points-1][action]);
}


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
		mem_free(beginMessage);
		exit(99);
	}
	
	while (!strcmp(beginMessage,"BEGIN")) {
		mem_free(beginMessage);

		// Set up new round bag
		bag_t* roundbag = mem_assert(bag_new(), "Round bag not created in play function in TRAIN mode");

		// Setting up hands
		hand_t* phand = mem_assert(newHand(), "Unable to create hand in play function");
		hand_t* dhand = mem_assert(newHand(), "Unable to create hand in play function");
	
		// Getting player hand
		char* cardm = readMessage(socket);
		if (cardm == NULL) {
			sleep(2);
			if ((cardm = readMessage(socket)) == NULL) exit(99);
		}	
		printf("card string is: %s\n", cardm);
		card_t* card = mem_assert(newPlayerCard(cardm), "New card was not created in play function");
		addToHand(phand, card);
		mem_free(cardm);
	
		cardm = readMessage(socket);
		if (cardm == NULL) {
			sleep(2);
			if ((cardm = readMessage(socket)) == NULL) exit(99);
		}
		printf("card string is: %s\n", cardm);
		card = mem_assert(newPlayerCard(cardm), "New card was not created in play function");
		addToHand(phand, card);
		mem_free(cardm);
	
		// Getting dealer hand
		cardm = readMessage(socket);
		if (cardm == NULL) {
			sleep(2);
			if ((cardm = readMessage(socket)) == NULL) exit(99);
		}
		printf("card string is: %s\n", cardm);
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
			fprintf(stderr, "Unexpected decision message: %s\n", decm);
			exit(99);
		} 
	
		// mem_free(decm);
	
		char* dec = mem_calloc_assert(6, sizeof(char), "Message for dec not created");
		int decnum = -1;
	
		do {
			int ppoints = getHandScore(phand);
			if (ppoints > 20) {
				strcpy(dec, "STAND");
				decnum = 1;
			} else {
				// Deciding next move
	#ifdef TRAIN
				if ((rand() % 2)) {
					strcpy(dec, "HIT");
					decnum = 0;
				} else {
					strcpy(dec, "STAND");
					decnum = 1;
				}
	#else
				
				int dpoints = getHandScore(dhand);

				float hit_chance = Q[ppoints-1][dpoints-1][0];
				float stand_chance = Q[ppoints-1][dpoints-1][1];
		
				if (hit_chance >= stand_chance) {
					strcpy(dec, "HIT");
					decnum = 0;
				} else {
					strcpy(dec, "STAND");
					decnum = 1;
				}
	#endif
			}
			if (sendMessage(socket, dec) == -1) {
				sleep(2);
				if (sendMessage(socket, dec) == -1) {
					fprintf(stderr, "%s\n", "Sending message to dealer with move was not possible");
					exit(99);
				}
			}

			// Recording round
			int* round = mem_calloc_assert(5, sizeof(int), "Unable to create round recording in play function in TRAIN mode");

			round[0] = getHandScore(phand);
			round[1] = getHandScore(dhand);
			// printf("dealer hand val: %d", getHandScore(dhand));
			round[2] = decnum;
			bag_insert(roundbag, round);
	
			if (!strcmp(dec, "STAND")) break;
	
			// Get card
			cardm = readMessage(socket);
			if (cardm == NULL) {
				sleep(2);
				if ((cardm = readMessage(socket)) == NULL) exit(99);
			}

			printf("card string is: %s\n", cardm);
			card = mem_assert(newPlayerCard(cardm), "New card was not created in play function");
			addToHand(phand, card);
			mem_free(cardm);
			
			// Verify Decision Message
			mem_free(decm);
			decm = readMessage(socket);
			if (decm == NULL) {
				sleep(2);
				if ((decm = readMessage(socket)) == NULL) exit(99);
			}
	
			if (!strcmp(decm, "RESULT BUST")) {
				mem_free(dec);
				break;
			} 

			if (strcmp(decm, "DECISION")) {
				fprintf(stderr, "Unexpected decision message: %s\n", decm);
				exit(99);
			}
			
		} while (1);
		
		char* result;
		if (!strcmp(decm, "RESULT BUST")) {
			result = decm;
		}
		else {
			result = readMessage(socket);
			mem_free(decm);
			mem_free(dec);
		}
		// mem_free(dec);
		// Getting match result
		
		
		
		if (result == NULL) {
			sleep(10);
			if ((result = readMessage(socket)) == NULL) exit(99);
		} 
	
		printf("Match Result: %s\n", result);
	
		// Saving reward integer
		int reward;
		if (!strcmp(result, "RESULT WIN")) {
			reward = 1;
		} else if (!strcmp(result, "RESULT LOOSE")) {
			reward = -1;
		} else if (!strcmp(result, "RESULT BUST")) {
			reward = -1;
		} else if (!strcmp(result, "RESULT PUSH")) {
			reward = 0;
		} else {
			fprintf(stderr, "%s\n", "Unexpected reward obtained in play function");
			exit(99);
		}

		mem_free(result);
	
		// Freeing memory
		deleteHand(phand);
		deleteHand(dhand);
		
		beginMessage = readMessage(socket);	
		if (beginMessage == NULL) {
			sleep(2);
			if ((beginMessage = readMessage(socket)) == NULL) exit(99);
		}
		

		// Record Reward
		bag_iterate(roundbag, &reward, rewardSaver);

		// Record match
		bag_iterate(roundbag, NULL, roundbagSaver);

		// Freeing round bag
		bag_delete(roundbag, mem_free);

	}

	if (strcmp(beginMessage, "QUIT")) {
		fprintf(stderr, "%s\n", "Strange Message Received");
		mem_free(beginMessage);
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
