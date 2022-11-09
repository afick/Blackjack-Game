/*
 *
 * dealer.c - Executes the dealer in the blackjack game
 *
 * usage: dealer
 * Jake Olson, Project Team 12, CS 50 Fall 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utils/mem.h"
#include "../utils/bag.h"
#include "../cards/cards.h"
#include "../network/network.h"


#define PORT 8092    // server port number 
#define NUMGAMES 3   // number of games to be played


int main(int argc, char* argv[]) {
	// Set up server socket for player
    int connected_socket;
    int listening_socket;
    char* buffer;
    bool bust;

    // check if dealer is connected with player
    if (setUpDealerSocket(PORT, &connected_socket, &listening_socket) == -1) {
        printf("Failed to set up server socket\n");
        exit(1);
    }

    // for each game:
    for(int i = 0; i < 3; i++) {
        bust = false;
        // create new, shuffled deck
        deck_t* deck = newDeck();
        sendMessage(connected_socket, "BEGIN");


        // deal cards to the player, sending messages 
        // with card suit and rank as a string, like "Seven of Hearts"
        
        // create playerHand and add two cards
        hand_t* playerHand = newHand();
        getNewCard(deck, playerHand, "CARD", connected_socket, true);
        getNewCard(deck, playerHand, "CARD", connected_socket, true);


        // create dealer hand and add two cards
        hand_t* dealerHand = newHand();
        getNewCard(deck, dealerHand, "DEALER", connected_socket, true);
        getNewCard(deck, dealerHand, "DEALER", connected_socket, false);

    
        sendMessage(listening_socket, "DECISION");
        

        // Let the player decide to hit until they are content
        char* result = readMessage(listening_socket);
        while(strcmp(result, "HIT") == 0) {
            getNewCard(deck, playerHand, "CARD", connected_socket, true);
            if(getHandScore(playerHand) > 21) {
                bust = true;
                free(result);
                break;
            }
            sendMessage(listening_socket, "DECISION");
            free(result);
            result = readMessage(listening_socket);
        }

        // deal the dealer until the hand total is 
        while (!bust && getHandScore(dealerHand) < 17) {
            getNewCard(deck, dealerHand, "DEALER", connected_socket, false);
        }


        // Calculate game's result and send message to client
        findResult(getHandScore(playerHand), getHandScore(dealerHand), listening_socket);

        // Reset and play again
        deleteHand(playerHand);
        deleteHand(dealerHand);
        deleteDeck(deck);

    }
    // when finished, send a QUIT message to the client
    sendMessage(connected_socket, "QUIT");
}

// helper function to get a card and perform the associated actions
static void getNewCard(deck_t* deck, hand_t* hand, char* type, int connected_socket, bool send) {
        card_t* card = pullCard(deck);
        addToHand(hand, card);
        if (send) {
            char* message = cardToString(type, card);
            sendMessage(connected_socket, message);
            mem_free(message);
        } 
}

// find the result of the game and message the player
void findResult(int playerHandScore, int dealerHandScore, int listening_socket) {
    if (playerHandScore > 21) { // player bust condition
        sendMessage(listening_socket, "RESULT BUST");
    } else if (dealerHandScore > 21 || playerHandScore > dealerHandScore) { // player win
        sendMessage(listening_socket, "RESULT WIN");
    } else if (playerHandScore < dealerHandScore) { // player loss
        sendMessage(listening_socket, "RESULT LOSS");
    } else if (playerHandScore == dealerHandScore) { // tie condition
        sendMessage(listening_socket, "RESULT PUSH");
    }
}