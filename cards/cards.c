/* 
 * cards.c - blackjack 'cards' module
 *
 * see cards.h for more information.
 *
 * Alex Fick, Final Project
 * CS 50, Fall 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/bag.h"
#include "../utils/mem.h"
#include "cards.h"
#include <time.h>


/**************** file-local global variables ****************/
/* none */
enum suits{Hearts = 1, Diamonds = 2, Clubs = 3, Spades = 4};
enum nums{Ace = 1, Two = 2, Three = 3, Four = 4, Five = 5, Six = 6,      // number on card
             Seven = 7, Eight = 8, Nine = 9, Ten = 10, Jack = 11, Queen = 12, King = 13};

/**************** local types ****************/

/**************** global types ****************/
typedef struct card {
    enum suits suit;        // suit of card    
    enum nums number;
    int val;                // blackjack value of card
} card_t;

typedef struct hand {
    bag_t* cards;
    int score;
} hand_t;

typedef bag_t deck_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see bag.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static card_t* newCard(int id);
static void findAces(void* arg, void* item);
void cardDelete(void* item);
void swap (int *a, int *b); 

/**************** newDeck() ****************/
/* see cards.h for description */
deck_t* newDeck(void)
{
    deck_t* deck = bag_new();
    int sizeDeck = 52;
    if (deck != NULL) {
        // create array of 52 numbers
        int cardArr[sizeDeck];
        for (int i = 0; i < sizeDeck; i++) {
            cardArr[i] = i+1;
        }
        // shuffle the array
        srand(time(NULL));
        for (int i = sizeDeck-1; i > 0; i--)
        {
            // Pick a random index from 0 to i
            int j = rand() % (i+1);
    
            // Swap arr[i] with the element at random index
            swap(&cardArr[i], &cardArr[j]);
        }
        // Create the cards and insert by card ID
        for (int n = 0; n < sizeDeck; n++) {
            card_t* card = newCard(cardArr[n]);
            if (card != NULL) {
                bag_insert(deck, card);
            }
        }
    }
    return deck;
}

/**************** swap() ****************/
/* 
 * Local function for swapping the value of two array ints
 */
void swap (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**************** newCard() ****************/
/* 
 * local function for creating a card that gets added to a deck
 */
card_t* newCard(int id)
{
    if (id > 0) {
        card_t* card = mem_malloc(sizeof(card_t));
        if (card != NULL) {
            int suit = (id-1)/13;
            int num = (id-1)%13+1;
            card->suit = suit;
            card->number = num;
            if (num == 1) card->val = 11;
            else if (num > 10) card->val = 10;
            else card->val = num;
            
        } else {
            return NULL;
        }
    } 
    return NULL;
}

/**************** newPLayerCard() ****************/
/* see cards.h for description */
card_t* newPlayerCard(char* cardString) {
    if (cardString != NULL) {
        static char const * suitsArr[] = { "Hearts", "Diamonds",
                                        "Clubs", "Spades" };
        static char const * numArr[] = {"Ace", "Two", "Three", "Four", 
        "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"};
        
        char* number = mem_malloc(sizeof(char)*6); // size of largest number
        char* suit = mem_malloc(sizeof(char)*9); // Size of largest suit

        sscanf(cardString, "CARD %s of %s", number, suit);
        
        enum nums numEnum;
        enum suits suitEnum;

        for (int i = 0; i < King; i++) {
            if (!strcmp(numArr[i], number)) {
                numEnum = i + 1;
            }
        }
        for (int i = 0; i < Spades + 1; i++) {
            if (!strcmp(suitsArr[i], suit)) {
                suitEnum = i + 1;
            }
        }
        card_t* card = mem_malloc(sizeof(card_t));
        if (card != NULL) {
            card->suit = suitEnum;
            card->number = numEnum;
            if (card->number == 1) card->val = 11;
            else if (card->number > 10) card->val = 10;
            else card->val = card->number;
            return card;
        } else {
            return NULL;
        }

    }
    return NULL;
}

/**************** newHand() ****************/
/* see cards.h for description */
hand_t* newHand(void)
{
  hand_t* hand = mem_malloc(sizeof(hand_t));

  if (hand == NULL) {
    return NULL;              // error allocating hand
  } else {
    // initialize contents of bag structure
    bag_t* bagHand = bag_new();
    if (bagHand != NULL) {
        hand->cards = bagHand;
        hand->score = 0;
    }
    return hand;
  }
}

/**************** pullCard() ****************/
/* see cards.h for description */
card_t* pullCard(deck_t* deck)
{
  if (deck == NULL) {
    return NULL;              // error allocating bag
  } else {
    bag_t* bag = deck;
    card_t* card = bag_extract(bag);
    return card;
  }
}

/**************** addToHand() ****************/
/* see cards.h for description */
void addToHand(hand_t* hand, card_t* card)
{
  if (hand != NULL && card != NULL) {
    bag_t* bag = hand->cards;
    int val = card->val;
    if (bag != NULL) {
        bag_insert(bag, card);
        int newScore = hand->score + val;
        if (newScore > 21) {
            bag_iterate(bag, &newScore, findAces);
        }
        hand->score = newScore;
    }
  }
}

/**************** findAces() ****************/
/* Bag iterate function for adjusting value of aces
 * when score is greater than 21
 */
static void findAces(void* arg, void* item) {
    card_t* card = item;
    int* score = arg;
    if (card != NULL) {
        if (card->val == 11 && *score > 21) {
            card->val = 1;
            *score -= 10; 
        }
    }
}

/**************** getHandScore() ****************/
/* see cards.h for description */
int getHandScore(hand_t* hand)
{
  if (hand != NULL ) {
    return hand->score;
  } 
  return 0;
}

/**************** deleteDeck() ****************/
/* see cards.h for description */
void deleteDeck(deck_t* deck)
{
    if (deck != NULL) {
        bag_delete(deck, cardDelete);
    }
}

/**************** deleteDeck() ****************/
/* see cards.h for description */
void deleteHand(hand_t* hand)
{
    if (hand != NULL) {
        bag_delete(hand->cards, cardDelete);
    }
    mem_free(hand);
}

/**************** cardDelete() ****************/
/* 
 * local item function for deleting a card from a bag
 */
void cardDelete(void* item)
{
    card_t* card = item;
    if (card != NULL) {
        mem_free(card);  
    }
}