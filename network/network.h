/* 
 * network.h - header file for 'network' module
 *
 * Handle:
 * Server set up/tear down
 * Socket connection/close
 * Message passing from dealer to player, and player to dealer
 * 
 * Di Luo
 * Nov. 6 2022
 * CS 50, Fall 2022
 */

#ifndef __NETWORK_H
#define __NETWORK_H

#include <stdio.h>

/**************** functions ****************/

/**************** player_connect() ****************/
/** Connect player to dealer.
 * 
 * Caller provides:
 *      a valid string for server's IP address,
 *      a valid PORT number.
 * We return:
 *      a valid communication socket (> 0),
 *      -1 if socket creation error, invalid address, address not supported, or connection failed.
*/
int player_connect(const char* server_addr, const int PORT);

/**************** dealer_init() ****************/
/** Set up a dealer socket listening for players to connect.
 * 
 * Caller provides:
 *      a valid PORT number.
 * We return:
 *      a valid communication socket that connects to the player (> 0),
 *      -1 if socket creation failed, setsockopt failed, bind failed, or listen failed.
*/
int dealer_init(const int PORT);

/**************** read_message() ****************/
/** Player/dealer reads a message from dealer/player.
 * 
 * Caller provides:
 *      a valid socket.
 * We return:
 *      a string of message from dealer/player,
 *      NULL if calloc failed or reading message failed.
 * Caller is responsible for:
 *      later free the memory allocated for the string.
*/
char* read_message(const int socket);

/**************** player_send() ****************/
/** Player/dealer sends a message to dealer/player.
 * 
 * Caller provides:
 *      a valid socket,
 *      a valid string of message to dealer/player.
 * We return:
 *      false if calloc failed or writing message failed,
 *      trun if message is written to the socket.
*/
void write_message(const int socket, char* message);

#endif // __NETWORK_H