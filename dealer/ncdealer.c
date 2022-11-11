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
#include <ncurses.h>


#define PORT 8092    // server port number 
#define NUMGAMES 10   // number of games to be played

// headers
void getNewCard(deck_t* deck, hand_t* hand, char* type, int connected_socket, bool send, int* row);
char* findResult(int playerHandScore, int dealerHandScore, int connected_socket);


int main(int argc, char* argv[]) {
    // init screen and sets up screen
    initscr();
	// Set up server socket for player
    int connected_socket;
    int listening_socket;
    bool bust;
    srand(time(0)); // set the seed for deck creation

    // check if dealer is connected with player
    if (setUpDealerSocket(PORT, &connected_socket, &listening_socket) == -1) {
        fprintf(stderr, "Failed to set up server socket\n");
        exit(1);
    }
    printw("Blackjack Dealer is connected with Player\n Connected Socket: %d\n Listening socket: %d\n\n", connected_socket, listening_socket);
    
    // refreshes the screen
    refresh();
    

    char* joinMessage = readMessage(connected_socket);
    char* name;
    if (strncmp(joinMessage,"JOIN ", strlen("JOIN ")) == 0) {
        name = mem_malloc(sizeof(char)*(strlen(joinMessage)-4));
        sscanf(joinMessage, "JOIN %s", name);
        mvprintw(10, 40, "%s, welcome to BlackJack! Press any key to continue.\n", name);
        refresh();
        getch();
        clear();   
        refresh();
    } else {
        printw("didn't receive JOIN message, received %s\n", joinMessage);
        refresh();
        endwin();
        exit(1);
    }
    free(joinMessage);
    joinMessage = NULL;

    // for each game:
    for(int i = 0; i < NUMGAMES; i++) {
        bust = false;
        // init screen and sets up screen
        initscr();
        // create new, shuffled deck
        deck_t* deck = newDeck();
        if (sendMessage(connected_socket, "BEGIN") == -1) {
            printw("sending BEGIN failed\n");
            refresh();
        } else {
            mvprintw(0, 0, "Dealer's Hand\n");
            mvprintw(0, 40, "Begin Game %d!\n", i+1);
            mvprintw(0, 80, "%s's Hand\n", name);
            refresh();
        }
        int row = 2; // keep track of which row you are on on the window

        // deal cards to the player, sending messages 
        // with card suit and rank as a string, like "Seven of Hearts"
        
        // create playerHand and add two cards
        hand_t* playerHand = newHand();
        getNewCard(deck, playerHand, "CARD", connected_socket, true, &row);
        getNewCard(deck, playerHand, "CARD", connected_socket, true, &row);

        // create dealer hand and add two cards
        hand_t* dealerHand = newHand();
        getNewCard(deck, dealerHand, "DEALER", connected_socket, true, &row);
        getNewCard(deck, dealerHand, "DEALER", connected_socket, false, &row);

    
        if (sendMessage(connected_socket, "DECISION") == -1) {
            printw("sending DECISION failed\n");
            refresh();
        } 
        

        // Let the player decide to hit until they are content
        char* result = readMessage(connected_socket);
        if (result != NULL) {
            if (strncmp(result,"HIT", strlen("HIT")) == 0) {
                mvprintw(row, 40, "%s HITS\n", name);  
                refresh();
            } else if (strncmp(result,"STAND", strlen("STAND")) == 0) {
                mvprintw(row, 40, "%s STANDS\n", name);  
                refresh();
            } else {
                mvprintw(row, 40, "Didn't receive HIT or STAND, received %s\n", result);
                refresh();
            }
            row++;
            while(strncmp(result, "HIT", strlen("HIT")) == 0) {
                getNewCard(deck, playerHand, "CARD", connected_socket, true, &row);
                if(getHandScore(playerHand) > 21) {
                    bust = true;
                    break;
                }

                if (sendMessage(connected_socket, "DECISION") == -1) {
                    printw("sending DECISION failed\n");
                    refresh();
                } 

                free(result);
                result = NULL;
                result = readMessage(connected_socket);
                if (strncmp(result,"HIT", strlen("HIT")) == 0) {
                    mvprintw(row, 40, "%s HITS\n", name);  
                    refresh();
                } else if (strncmp(result,"STAND", strlen("STAND")) == 0) {
                    mvprintw(row, 40, "%s STANDS\n", name);  
                    refresh();
                } else {
                    printw("didn't receive HIT or STAND, received %s\n", result);
                    refresh();
                }
                row += 1;
            }
            free(result);
        } else continue;
        
        // deal the dealer until the hand total is 
        while (!bust && getHandScore(dealerHand) < 17) {
            getNewCard(deck, dealerHand, "DEALER", connected_socket, false, &row);
        }


        // Calculate game's result and send message to client
        char* finalResult = findResult(getHandScore(playerHand), getHandScore(dealerHand), connected_socket);
        if (sendMessage(connected_socket, finalResult) == -1) {
            printw("sending %s failed\n", finalResult);
            refresh();
        } else {
            mvprintw(row, 40, "%s's Score: %d, Dealer's Score: %d\n", name, getHandScore(playerHand), getHandScore(dealerHand));
            row += 1;
            mvprintw(row, 40, "%s's result: %s\n", name, finalResult);
            refresh();
        }

        // Reset and play again
        deleteHand(playerHand);
        deleteHand(dealerHand);
        deleteDeck(deck);

        printw("---------------\n Click any key to continue to next game\n");
        refresh();
        getch();
        clear();   
        refresh();
    }
    endwin();
    mem_free(name);
    // when finished, send a QUIT message to the client
    sendMessage(connected_socket, "QUIT");
    closeServerSocket(connected_socket, listening_socket);
    return 0;
}

// helper function to get a card and perform the associated actions
void getNewCard(deck_t* deck, hand_t* hand, char* type, int connected_socket, bool send, int* row) {
        card_t* card = pullCard(deck);
        addToHand(hand, card);
        char* message = cardToString(type, card);
        if (send) {
            if (sendMessage(connected_socket, message) == -1) printw("Couldn't send %s\n", message);
            if(strcmp(type, "DEALER") == 0) mvprintw(*row, 0, "%s\n", message);
            else mvprintw(*row, 80, "%s\n", message);
            *row += 1;
        } else {
            mvprintw(*row, 0, "%s (Facedown)\n", message);
            *row += 1;
        }
        refresh();
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