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

/**************** player_connect() ****************/
/* See network.h for more information */
int player_connect(const char* dealer_addr, const int PORT) {
    int sock = 0, player_fd;
    struct sockaddr_in serv_addr; // address of the server
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Conver IPv4 and IPv6 addresses from test to binary form
    if (inet_pton(AF_INET, dealer_addr, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address/ Address not supported\n");
        return -1;
    }

    if ((player_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        fprintf(stderr, "Connection Failed\n");
        return -1;
    }

    return sock;
}

/**************** dealer_init() ****************/
/* See network.h for more information */
int dealer_init(const int PORT) {
    int dealer_fd, new_socket, valread;
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

    printf("Waiting for connection... ");
    fflush(stdout); //ensure line above prints before waiting for connection

    while (true) {
        if ((new_socket = accept(dealer_fd, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen)) != -1) {
            break;
        }
    }
    
    return new_socket;
}

