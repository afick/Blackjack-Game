# CS50 Final Project

This was the final project for CS50, Software Design and Implementation. We were tasked with implemented both the dealer and player of a blackjack game, which could communicate remotely through TCP/IP messages. It also incorporates a reinforcement learning approach to training the player to play automatically, so there is no user input involved in this game. See [Design.MD](DESIGN.md) and [Implementation.MD](IMPLEMENTATION.md) for detailed descriptions of each of the four modules. I completed the cards module fully by myself, and also the majority of the dealer, and well as a large portion of the player, and all of the ncdealer implementation for the NCurses interface.

See the course web site for [detailed description of the project](https://www.cs.dartmouth.edu/~tjp/cs50/project/index.html)

### Compilation

To compile, simply call `make`.

### Usage

First, run the dealer with:
```
dealer/ncdealer <number of games> <port>` 
```

Then, run the player with the following command:

```
player/player <player_name> <ip_address> <port_number>
```


### Notes
#### Training (# of Games)
In our training, we were primarily limited by time. We also understood that the value of a specific index in the Q table wouldn't matter in playing beyond being greater or less than the opposite action for the same state. We saw that even just after 5,000 tests, the two states would not flip in terms of average reward. Since we had the ability to, however, we decided to run an approximately 1,000,000 extra games overnight. With the run time running <5 games per second, for the 7 hours, it probably got through around 100,000-125,000 games. Combined with other training done before it, we trainied around a total of 150,000 games probably. We hope this would eliminate any fluctuations between preference of one action over the other. Training was done on babylon1 server, so that is the IP address being used.

#### NCurses

We did implement a version of the game in ncurses. It nicely lays out the process of the execution of the dealer: it opens with a welcome message, then the user can press any key to continue, which will bring the user to the first game, and then the user can press any key to continue through the games, until the number of games declared have been played. 

To use the ncurses version of dealer, please navigate to the dealer directory, run `make`, then use the command line syntax `./ncdealer <number of games> <port>` to run the program. 

The logic and implementation is almost exactly the same as the normal dealer program.

#### Potential Flaws

Occassionally, when running a large load of games (>500) at once, we have seen connection issues pop up. These are very rare, and are likely due to a network instability issue that is out of our control. It gets handled with the message "reading message failed: Connection reset by peer" and the player is able to keep going with a new game. 