# CS50 Blackjack Final Project Player module
## Implementation Specification

In this document we reference the [Requirements Specification](../REQUIREMENTS.md) and [Design Specification](../DESIGN.md) and focus on the implementation-specific decisions.
Here we focus on the core subset of topics:

-  Data structures/descripts
-  Descriptions
-  Pseudo code for each of the functions
-  Detailed function prototypes and their parameters

## Data structures/descriptions 

We leverage the data structures provided by cards; otherwise, no major data structures are used.
### Usage

The *dealer* module is defined and implemented in `dealer.c`, and exports the following functions:

```c
void getNewCard(deck_t* deck, hand_t* hand, char* type, int connectedSocket, bool send);
char* findResult(int playerHandScore, int dealerHandScore, int connectedSocket);
static void parseArgs(const int argc, char* argv[], int* games, int* port);
int main(int argc, char* argv[]);
```

### Implementation

The dealer begins in `main` by calling `parseArgs`, to verify that the required arguments have been provided accurately. It then 

### Assumptions

No assumptions beyond those that are clear from the spec.


### Files

REPLACE

* `Makefile` - compilation procedure
* `cards.h` - the interface
* `cards.c` - the implementation

### Compilation

To compile, simply `make`.

