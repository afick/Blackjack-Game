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
#include "bag.h"
#include "mem.h"
#include "cards.h"
#include <time.h>


/**************** file-local global variables ****************/
/* none */
enum suits{hearts, diamonds, clubs, spades};
enum nums{ace, two, three, four, five, six,      // number on card
             seven, eight, nine, ten, jack, queen, king};

/**************** local types ****************/
struct aceFinder {
    int acesFound;
    int score;
};

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


/**************** newDeck() ****************/
/* see cards.h for description */
deck_t*
newDeck(void)
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

void swap (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**************** newCard() ****************/
/* see cards.h for description */
card_t*
newCard(int id)
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
}

/**************** newHand() ****************/
/* see cards.h for description */
hand_t*
newHand(void)
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
card_t*
pullCard(deck_t* deck)
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
void
addToHand(hand_t* hand, card_t* card)
{
  if (hand != NULL && card != NULL) {
    bag_t* bag = hand->cards;
    int val = card->val;
    if (bag != NULL) {
        bag_insert(bag, card);
        int newScore = hand->score + val;
        if (newScore > 21) {
            struct aceFinder arg = {0, newScore};
            bag_iterate(bag, &arg, findAces);
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
    struct aceFinder* find = arg;
    if (card != NULL) {
        if (card->val == 11 && find->score > 21) {
            card->val == 1;
            find->acesFound = 1;
            find->score -= 10; 
        }
    }
}

/**************** getHandScore() ****************/
/* see cards.h for description */
int
getHandScore(hand_t* hand)
{
  if (hand != NULL ) {
    return hand->score;
  }
}

/**************** deleteDeck() ****************/
/* see cards.h for description */
void 
deleteDeck(deck_t* deck)
{
    if (deck != NULL) {
        bag_delete(deck, cardDelete);
    }
}

/**************** deleteDeck() ****************/
/* see cards.h for description */
void 
deleteHand(hand_t* hand)
{
    if (hand != NULL) {
        bag_delete(hand->cards, cardDelete);
    }
    mem_free(hand);
}
void cardDelete(void* item)
{
    card_t* card = item;
    if (card != NULL) {
        mem_free(card);  
    }
}