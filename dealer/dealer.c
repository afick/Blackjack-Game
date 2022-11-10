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
#include <string.h>
#include "../utils/mem.h"
#include "../utils/bag.h"
#include "../cards/cards.h"
#include "../network/network.h"
#include <time.h>


#define PORT 8092    // server port number 
#define NUMGAMES 3   // number of games to be played

// headers
void getNewCard(deck_t* deck, hand_t* hand, char* type, int connected_socket, bool send);
char* findResult(int playerHandScore, int dealerHandScore, int connected_socket);


int main(int argc, char* argv[]) {
	// Set up server socket for player
    int connected_socket;
    int listening_socket;
    bool bust;
    srand(time(0)); // set the seed for deck creation

    // check if dealer is connected with player
    if (setUpDealerSocket(PORT, &connected_socket, &listening_socket) == -1) {
        printf("Failed to set up server socket\n");
        exit(1);
    }
    printf("dealer is connected with player\n connected socket: %d\n listening socket: %d\n", connected_socket, listening_socket);

    char* joinMessage = readMessage(connected_socket);
    char* name;
        if (strncmp(joinMessage,"JOIN ", strlen("JOIN ")) == 0) {
            printf("dealer: received %s from player\n", joinMessage); 
            name = mem_malloc(sizeof(char)*(strlen(joinMessage)-4));
            sscanf(joinMessage, "JOIN %s", name);
        } else {
            printf("didn't receive JOIN message, received %s\n", joinMessage);
        }
        free(joinMessage);
        joinMessage = NULL;

    // for each game:
    for(int i = 0; i < NUMGAMES; i++) {
        bust = false;
        // create new, shuffled deck
        deck_t* deck = newDeck();
        if (sendMessage(connected_socket, "BEGIN") == -1) {
            printf("sending BEGIN failed\n");
        } else {
            printf("dealer: sent BEGIN\n");
        }


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

    
        if (sendMessage(connected_socket, "DECISION") == -1) {
            printf("sending DECISION failed\n");
        } else {
            printf("dealer: sent DECISION\n");
            fflush(stdout);
        }
        

        // Let the player decide to hit until they are content
        char* result = readMessage(connected_socket);
        if (result != NULL) {
            if (strncmp(result,"HIT", strlen("HIT")) == 0) {
                printf("dealer: received HIT from player\n");  
            } else if (strncmp(result,"STAND", strlen("STAND")) == 0) {
                printf("dealer: received STAND from player\n");  
            } else {
                printf("didn't receive HIT or STAND, received %s\n", result);
            }
            while(strncmp(result, "HIT", strlen("HIT")) == 0) {
                getNewCard(deck, playerHand, "CARD", connected_socket, true);
                if(getHandScore(playerHand) > 21) {
                    bust = true;
                    break;
                }

                if (sendMessage(connected_socket, "DECISION") == -1) {
                    printf("sending DECISION failed\n");
                } else {
                    printf("dealer: sent DECISION\n");
                }

                free(result);
                result = NULL;
                result = readMessage(connected_socket);
                if (strncmp(result,"HIT", strlen("HIT")) == 0) {
                    printf("dealer: received HIT from player\n");  
                } else if (strncmp(result,"STAND", strlen("STAND")) == 0) {
                    printf("dealer: received STAND from player\n");  
                } else {
                    printf("didn't receive HIT or STAND, received %s\n", result);
                }
            }
            free(result);
        } else continue;
        
        
        

        // deal the dealer until the hand total is 
        while (!bust && getHandScore(dealerHand) < 17) {
            getNewCard(deck, dealerHand, "DEALER", connected_socket, false);
        }


        // Calculate game's result and send message to client
        char* finalResult = findResult(getHandScore(playerHand), getHandScore(dealerHand), connected_socket);
        if (sendMessage(connected_socket, finalResult) == -1) {
            printf("sending %s failed\n", finalResult);
        } else {
            printf("dealer: sent %s\n", finalResult);
            printf("%s's result: %s\n", name, finalResult);
        }

        // Reset and play again
        deleteHand(playerHand);
        deleteHand(dealerHand);
        deleteDeck(deck);

        printf("---------------\n");
    }
    mem_free(name);
    // when finished, send a QUIT message to the client
    sendMessage(connected_socket, "QUIT");
    closeServerSocket(connected_socket, listening_socket);
    exit(0);
}

// helper function to get a card and perform the associated actions
void getNewCard(deck_t* deck, hand_t* hand, char* type, int connected_socket, bool send) {
        card_t* card = pullCard(deck);
        addToHand(hand, card);
        char* message = cardToString(type, card);
        if (send) {
            if (sendMessage(connected_socket, message) == -1) {
                printf("sending %s failed\n", message);
            } else {
                printf("dealer: sent %s\n", message);
            }
            
        } 
        mem_free(message);
}

// find the result of the game and message the player
char* findResult(int playerHandScore, int dealerHandScore, int connected_socket) {
    if (playerHandScore > 21) { // player bust condition
        return "RESULT BUST";
    } else if (dealerHandScore > 21 || playerHandScore > dealerHandScore) { // player win
        return "RESULT WIN";
    } else if (playerHandScore < dealerHandScore) { // player loss
        return "RESULT LOOSE";
    } else if (playerHandScore == dealerHandScore) { // tie condition
        return "RESULT PUSH";
    }
    return "FAILURE";
}