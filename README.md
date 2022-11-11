# CS50 Final Project
## Team 12

The assignment is in a [public repo](https://github.com/cs50-2022-fall/project).
Do not clone that repo; view it on GitHub.  One person from your team should accept the assignment from GitHub classroom.
Watch if there for any commits that may represent updates to the assignment or specs.

See the course web site for [detailed description of the project](https://www.cs.dartmouth.edu/~tjp/cs50/project/index.html)

Add here any assumptions you made while writing the solution, or any ways in which you know your implementation fails to work.

### Notes
#### NCurses

We did implement a version of the game in ncurses. It nicely lays out the process of the execution of the dealer: it opens with a welcome message, then the user can press any key to continue, which will bring the user to the first game, and then the user can press any key to continue through the games, until the number of games declared have been played. 

To use the ncurses version of dealer, please navigate to the dealer directory, run `make`, then use the command line syntax `./ncdealer <number of games> <port>` to run the program. 

The logic and implementation is almost exactly the same as the normal dealer program.

#### Potential Flaws

Occassionally, when running a large load of games (>500) at once, we have seen connection issues pop up. These are very rare, and are likely due to a network instability issue that is out of our control. It gets handled and the player is able to keep going with a new game. 