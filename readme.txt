By Damjan Markovic 100823871

"The goal of this assignment is to write a parallel MPI program for the Game of Life on a processor cluster (the cluster described by Andrew Pullin in class). The Game of Life is a board game. The board consist of N x N cells (N rows, N columns), each having value 1 or 0 depending on whether or not it contains an "organism"; see example below. Every cell on the board has eight neighbours (the boundary cells have imaginary neighbours outside the boundary with value 0). Initially, some of the cells hold organisms. The cell values then change in synchronous steps according to the following rules: 

Every organism with two or three neighbouring organisms survives for the next generation.
Every organism with four or more neighbours dies from overpopulation.
Every organism with one or no neighbour dies from isolation.
Every empty cell adjacent to exactly three occupied neighbour cells will give birth to a new organism.
For more information, check out the Game of Life Wikipedia page (http://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) and try out this Game of Life applet: http://www.bitstorm.org/gameoflife/. "

note: The program divides the board by rows, but it is also incomplete. It successfully divides the board, passes it to the appropriate processes and prints out the board every m generations as well as communicating successfully between the processes but the updating of the board isn’t properly implemented.

To run: 

make sure main.cpp is compiled as main and stored on all machines as specified in the host file

run the following command, where hostile is the file containing all the machines

alter the test.txt as need be to specify an initial input

mpirun -np 4 --hostfile hostfile main N k m


