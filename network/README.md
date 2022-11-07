# CS50 Blackjack Final Project Network module

This directory holds `network` module used by `dealer` and `player` in the CS50 Blackjack Final Project.

For background knowledge, see [Networking and Socket Programming](https://www.cs.dartmouth.edu/~tjp/cs50/reading/sockets/).

## Usage

To build `network.a`, run `make`.

To clean up, run `make clean`.

To test network module, run `make test_dealer` in a terminal and run `make test_player` in another terminal. 

To test network module, run `make valgrind_dealer` in a terminal and run `make valgrind_player` in another terminal. 

## Control flow

The network module is implemented in one file `network.c`, with six functions.

### 1. connectToDealer



### 2. setUpDealerSocket



### 3. readMessage



### 4. sendMessage



### 5. closeServerSocket



### 6. closeClientConnection



## Function prototypes

### network

```c
int connectToDealer(const char* server_addr, const int PORT);
int setUpDealerSocket(const int PORT, int* connected_socket, int* listening_socket);
char* readMessage(const int socket);
int sendMessage(const int socket, char* message);
void closeServerSocket(const int connectedSocket, const int listeningSocket);
void closeClientConnection(const int socket);
```

## Testing

Two testing files `networktest_dealer.c` and `networktest_player.c` are implemented to simulate dealer and player respectively. 

The executable of `networktest_dealer.c` is `dealer` and that of `networktest_player.c` is `player`. Two executables .

## Acknowledgement

[Networking and Socket Programming](https://www.cs.dartmouth.edu/~tjp/cs50/reading/sockets/)
[Client Demo Program](http://www.cs.dartmouth.edu/~tjp/cs50/project/client.c)
[Server Demo Program](http://www.cs.dartmouth.edu/~tjp/cs50/project/server.c)
[Socket Programming in C/C++](https://www.geeksforgeeks.org/socket-programming-cc/)
[TCP client/server example](https://cs.dartmouth.edu/~tjp/cs50/examples/client-server/)
