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
/** Connect player to dealer
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
/** Set up a server socket listening for players to connect
 * 
 * Caller provides:
 *      a valid PORT number.
 * We return:
 *      a valid communication socket that connects to the player (> 0),
 *      -1 if socket creation failed, setsockopt failed, bind failed, or listen failed.
*/
int dealer_init(const int PORT);


#endif // __NETWORK_H