/* 
 * network.c - CS50 Blackjack Final Project 'network' module
 *
 * See network.h for more information
 * 
 * Di Luo
 * Nov. 6 2022
 * CS 50, Fall 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>       // player
#include <arpa/inet.h>  // player
#include <sys/socket.h> 
#include <sys/types.h>  // player
#include <unistd.h>     // read, write, close
#include <netinet/in.h> // dealer

/**************** functions ****************/

/**************** connectToDealer() ****************/
/* See network.h for more information */
int connectToDealer(const char* dealer_addr, const int PORT) {
    int sock = 0, player_fd;
    struct sockaddr_in serv_addr; // address of the server
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from test to binary form
    if (inet_pton(AF_INET, dealer_addr, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address/ Address not supported\n");
        return -1;
    }

    if ((player_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        fprintf(stderr, "Connection failed\n");
        return -1;
    }

    return sock;
}

/**************** setUpDealerSocket() ****************/
/* See network.h for more information */
int setUpDealerSocket(const int PORT, int* connected_socket, int* listening_socket) {
    int dealer_fd = 0;
    int new_socket = 0;
    struct sockaddr_in serv_addr;
    int opt = 1;
    int addrlen = sizeof(serv_addr);

    // Creating socket file descriptor
    if ((dealer_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }

    // Attaching socket to the PORT
    if (setsockopt(dealer_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Binding socket to the PORT
    if (bind(dealer_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        return -1;
    }

    if (listen(dealer_fd, 1) < 0) {
        perror("listen failed");
        return -1;
    }

    while (true) {
        if ((new_socket = accept(dealer_fd, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen)) != -1) {
            break;
        }
    }

    *listening_socket = dealer_fd;
    *connected_socket = new_socket;
    
    return 0;
}

/**************** readMessage() ****************/
/* See network.h for more information */
char* readMessage(const int socket) {
    char* buffer = calloc(30, sizeof(char));

    if (buffer == NULL) {
        perror("calloc failed");
        return NULL;
    }

    if (read(socket, buffer, 29) < 0) {
        perror("reading message failed");
        return NULL;
    }

    return buffer;
}

/**************** sendMessage() ****************/
/* See network.h for more information */
int sendMessage(const int socket, char* message) {
    char* buffer = calloc(30, sizeof(char));

    if (buffer == NULL) {
        perror("calloc failed");
        return -1;
    }

    strcpy(buffer, message);

    if (write(socket, buffer, strlen(buffer)) < 0) {
        perror("writing message failed");
        free(buffer);
        return -1;
    }

    free(buffer);

    return 0;
}

/**************** closeServerSocket() ****************/
/* See network.h for more information */
void closeServerSocket(const int connected_socket, const int listening_socket) {
    close(connected_socket);
    shutdown(listening_socket, SHUT_RDWR);
}

/**************** closeClientConnection() ****************/
/* See network.h for more information */
void closeClientConnection(const int socket) {
    close(socket);
}
