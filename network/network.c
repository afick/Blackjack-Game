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
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <unistd.h>     // read, write, close

/**************** functions ****************/

/**************** player_connect() ****************/
/* See network.h for more information */
int player_connect(const char* dealer_addr, const int PORT) {
    int sock = 0, player_fd;
    struct sockaddr_in serv_addr; // address of the server
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Conver IPv4 and IPv6 addresses from test to binary form
    if (inet_pton(AF_INET, dealer_addr, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((player_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        fprintf(stderr, "\nConnection Failed \n");
        return -1;
    }

    return sock;
}

