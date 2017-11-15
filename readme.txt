By Damjan Markovic 100823871

note: The program divides the board by rows, but it is also incomplete. It successfully divides the board, passes it to the appropriate processes and prints out the board every m generations as well as communicating successfully between the processes but the updating of the board isn’t properly implemented.

To run: 

make sure main.cpp is compiled as main and stored on all machines as specified in the host file

run the following command, where hostile is the file containing all the machines

alter the test.txt as need be to specify an initial input

mpirun -np 4 --hostfile hostfile main N k m


