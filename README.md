# Mini_Project_3-Othello
### NTHU I2P(II) Mini Project 3

Othello AI based on MiniMax search with Alpha-Beta pruning

**Templete** : https://github.com/j3soon/OthelloAITemplate

### Heuristic function
The heuristic function is adaptive, which means that the weight of each value changes according to the current game stage, determined by number of empty discs.
### State value design
* **Stable edges** : Returns a value if border around 4 corners can be formed, especially a L-shape one.

* **Weight** : Gives the overall board value according to the weight table.

* **Mobility** : Returns number of next_valid_spots.

* **Parity** : If the number of empty discs is odd, cur_player is expected to play the final move, may possess slight advantage.

* **Discs differences** : Returns discs difference between player and opponent.
