
# include <cstdlib>
# include <iostream>
# include <fstream>
# include <math.h>
# include "mpi.h"
using namespace std;

int main(int argc, char *argv[])

{
  int id;
  int p, k, m;
  int rc;
  int matSize;
  int destr, destl, sourcer, sourcel;
  double wtime;
  
  MPI_Status Stat;
  MPI::Init(argc, argv); //  Initialize MPI.
  p = MPI::COMM_WORLD.Get_size(); //  Get the number of processes.
  id = MPI::COMM_WORLD.Get_rank(); //  Get the individual process ID.
  
  matSize = atoi(argv[1]); //mat
  k = atoi(argv[2]);
  m = atoi(argv[3]);
  
  //cout << "N = " << matSize << "\n" << "k = " << k << "\n" << "m = " << m << "\n";
  
  int q = matSize*matSize;
  int rowNum = matSize/p;
  
  if (id == 0) {
	
	destr = (id + 1);
	sourcer = destr;
	
	ifstream myfile("test.txt");
	int board[matSize][matSize];
	char *data;
	
	//read 0s and 1s from file into int array, file assumed to contain matSize^2 ammount of bits
	cout << "Process id: " << id << "\n\n" << "The mat: FIRST\n";
	
	for(int i = 0; i < matSize; i++) { //read first rowSize rows into upRows, the 0th processes 
		myfile.read(data,matSize);
		for(int j = 0; j < matSize; j++) {
				board[i][j] = data[j] - '0';
				cout << board[i][j];
		}
	}
	
	cout << "\n\n";
    
	for(int l = 1; l < p; l++) { //for each process send a pointer to that row
		
		if(l == p-1) {
			rc = MPI_Send(&(board[l*rowNum][0]), matSize*((matSize/p) + (matSize%p)), MPI_INT, l, 0, MPI_COMM_WORLD) ; 
		} else {
			rc = MPI_Send(&(board[l*rowNum][0]), matSize*((matSize/p)), MPI_INT, l, 0, MPI_COMM_WORLD) ; //0 is initializer tag
		}	
	}
    
    //send rightmost collumn to id++
    //receive leftmost collumn of id++
    //rc = MPI_Recv(&inmsg, 1, MPI_CHAR, id += 1, tag, MPI_COMM_WORLD);
    int count = 0;
    int numRows = (matSize/p);
	int numCols = matSize;
	int boardCopy[numRows][numCols];
	int recBottom[numCols];

	while(count < k) {

		if(count > 0 && count % m == 0) {
			cout << "\n\n";
			for(int y = 0; y < matSize; y++) {
				for(int z = 0; z < matSize; z++) {
					cout << board[y][z];
				}
				cout << "\n";
			}
			cout << "\n";
		}

		rc = MPI_Recv(&recBottom, numCols, MPI_INT, sourcer, 0, MPI_COMM_WORLD, &Stat);
		rc = MPI_Send(&board[numRows-1][0], numCols, MPI_INT, destr, 0, MPI_COMM_WORLD);

		for(int r = 0; r < numRows; r++) {
			for(int c = 0; c < numCols; c++) {
				int numAlive = 0;
				if(r == 0) { //TOP, THERE ARE NO ROWS ABOVE THE TOP
					if(numRows > 1) { //MORE THAN 1 ROW
						if(c == 0) { //LEFTMOST POSITION, TOP LEFT CORNER, CHECK EAST, SOUTHEAST, SOUTH
							numAlive += (board[r][c+1] + board[r+1][c+1] + board[r+1][c]);
						} else if(c == numCols-1) { //RIGHTMOST POSITION, TOP RIGHT CORNER, CHECK WEST, SOUTHWEST, SOUTH
							numAlive += (board[r][c-1] + board[r+1][c-1] + board[r+1][c]);
						} else { //THE REST, CHECK EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST 
							numAlive += (board[r][c+1] + board[r+1][c+1] + board[r+1][c] + board[r+1][c-1] + board[r][c-1]);
						}
					} else { //ONLY 1 ROW, TOP = BOTTOM
						if(c == 0) { //LEFTMOST POSITION, CHECK EAST, CHECK EAST OF REC, CHECK REC
							numAlive += (board[r][c+1] + recBottom[c+1] + recBottom[c]);
						} else if(c == numCols-1) { //RIGHTMOST POSITION, CHECK WEST, WEST OF REC, REC
							numAlive += (board[r][c-1] + recBottom[c-1] + recBottom[c]);
						} else { //THE REST, CHECK EAST, REC EAST, REC, REC WEST, WEST
							numAlive += (board[r][c+1] + recBottom[c+1] + recBottom[c] + recBottom[c-1] + board[r][c-1]);
						}
					}

				} else if(r == numRows-1) { //BOTTOM, MORE THAN ONE ROW AUTOMATICALLY, NEED ROW BELOW, TOP EXISTS
					if(c == 0) { //LEFTMOST POSITION, CHECK EAST, EAST REC, REC, NORTH, NORTHEAST 
						numAlive += (board[r][c+1] + recBottom[c+1] + recBottom[c] + board[r-1][c] + board[r-1][c+1]);
					} else if(c == numCols-1) { //RIGHTMOST POSITION, CHECK WEST, WEST OF REC, REC, NORTH, NORTHWEST
						numAlive += (board[r][c-1] + recBottom[c-1] + recBottom[c] + board[r-1][c] + board[r-1][c-1]);
					} else { //THE REST, CHECK EAST, REC EAST, REC, REC WEST, WEST, NORTHWEST, NORTH, NORTHEAST
						numAlive += (board[r][c+1] + recBottom[c+1] + recBottom[c] + recBottom[c-1] + board[r][c-1] + board[r-1][c-1] + board[r-1][c] + board[r-1][c+1]);
					}
				} else { //AVERGAE CASE, NEED NO ROWS
					if(c == 0) { //LEFTMOST POSITION
						numAlive += (board[r-1][c] + board[r-1][c+1] + board[r][c+1] + board[r+1][c+1] + board[r+1][c]);
					} else if(c == numCols-1) { //RIGHTMOST POSITION
						numAlive += (board[r-1][c] + board[r-1][c-1] + board[r][c-1] + board[r+1][c-1] + board[r+1][c]);
					} else { //THE REST
						numAlive += (board[r-1][c-1] + board[r-1][c] + board[r-1][c+1] + board[r][c+1] + board[r+1][c+1] + board[r+1][c] + board[r+1][c-1] + board[r][c-1]);
					}
				}

				if(board[r][c] == 0) {
					if(numAlive >= 3) {
						boardCopy[r][c] = 1;
					}
				} else {
					if(numAlive >= 4 || numAlive <= 2) {
						boardCopy[r][c] = 0;
					}
				}

			}//end inner for
		}//end outer for

		for(int w = 0; w < numRows; w++) {
			for(int x = 0; x < numCols; x++) {
				board[w][x] = boardCopy[w][x];
			}
		}

		count++;

	}//end while
    
  }
  
  else if (id == p-1) { //THE BOTTOM
	  
	  int numRows = (matSize/p) + (matSize%p);
	  int numCols = matSize;
	  int recData[numRows][numCols];
	  int recDataCopy[numRows][numCols];
	  int recTop[numCols];
	  int count = 0;
	  
	  destl = (id - 1);
	  sourcel = destl;
	  
	  rc = MPI_Recv(&recData, matSize*((matSize/p) + (matSize%p)), MPI_INT, 0, 0, MPI_COMM_WORLD, &Stat);
	  
	  
	  cout << "Process id: " << id << "\n\n" << "The mat: LAST\n";
	  
	  for(int i = 0; i < numRows; i++) {
		  for(int j = 0; j < numCols; j++) {
				cout << recData[i][j];
		  }
	  }
	  
	  cout << "\n\n";
		  
	  
	  //send leftmost collumn to id--
	  //receive rightmost collumn of id--
	  while(count < k) {

		rc = MPI_Send(&recData[0][0], numCols, MPI_INT, destl, 0, MPI_COMM_WORLD);
		rc = MPI_Recv(&recTop, numCols, MPI_INT, sourcel, 0, MPI_COMM_WORLD, &Stat);

		for(int r = 0; r < numRows; r++) {
			for(int c = 0; c < numCols; c++) {
				int numAlive = 0;
				if(r == 0) { //TOP OF THE BOTTOM
					if(numRows > 1) { //MORE THAN 1 ROW
						if(c == 0) { 
							numAlive += (recData[r][c+1] + recData[r+1][c+1] + recData[r+1][c] + recTop[c]  + recTop[c+1]);
						} else if(c == numCols-1) { 
							numAlive += (recData[r][c-1] + recData[r+1][c-1] + recData[r+1][c] + recTop[c]  + recTop[c-1]);
						} else { 
							numAlive += (recData[r][c+1] + recData[r+1][c+1] + recData[r+1][c] + recData[r+1][c-1] + recData[r][c-1] + recTop[c+1] + recTop[c]  + recTop[c-1]);
						}
					} else { //ONLY 1 ROW, TOP = BOTTOM
						if(c == 0) { 
							numAlive += (recData[r][c+1] + recTop[c+1] + recTop[c]);
						} else if(c == numCols-1) { 
							numAlive += (recData[r][c-1] + recTop[c-1] + recTop[c]);
						} else { 
							numAlive += (recData[r][c+1] + recTop[c+1] + recTop[c] + recTop[c-1] + recData[r][c-1]);
						}
					}

				} else if(r == numRows-1) { //BOTTOM, 
					if(c == 0) { 
						numAlive += (recData[r][c+1] + recData[r-1][c] + recData[r-1][c+1]);
					} else if(c == numCols-1) { 
						numAlive += (recData[r][c-1] + recData[r-1][c] + recData[r-1][c-1]);
					} else { 
						numAlive += (recData[r][c+1] + recData[r][c-1] + recData[r-1][c-1] + recData[r-1][c] + recData[r-1][c+1]);
					}
				} else { //AVERAGE CASE
					if(c == 0) { //LEFTMOST POSITION
						numAlive += (recData[r-1][c] + recData[r-1][c+1] + recData[r][c+1] + recData[r+1][c+1] + recData[r+1][c]);
					} else if(c == numCols-1) { //RIGHTMOST POSITION
						numAlive += (recData[r-1][c] + recData[r-1][c-1] + recData[r][c-1] + recData[r+1][c-1] + recData[r+1][c]);
					} else { //THE REST
						numAlive += (recData[r-1][c-1] + recData[r-1][c] + recData[r-1][c+1] + recData[r][c+1] + recData[r+1][c+1] + recData[r+1][c] + recData[r+1][c-1] + recData[r][c-1]);
					}
				}

				if(recData[r][c] == 0) {
					if(numAlive >= 3) {
						recDataCopy[r][c] = 1;
					}
				} else {
					if(numAlive >= 4 || numAlive <= 2) {
						recDataCopy[r][c] = 0;
					}
				}

			}//end inner for
		}//end outer for

		for(int w = 0; w < numRows; w++) {
			for(int x = 0; x < numCols; x++) {
				recData[w][x] = recDataCopy[w][x];
			}
		}

		count++;

	}//end while
  }
  
  else {
	  
	int numRows = (matSize/p);
	int numCols = matSize;
	int recData[numRows][numCols];
	int recDataCopy[numRows][numCols];
	int recTop[numCols];
	int recBottom[numCols];
	int count = 0;
	  
	destl = (id - 1);
	destr = (id + 1);
	sourcel = destl;
	sourcer = destr;
	
	rc = MPI_Recv(&recData, matSize*(matSize/p), MPI_INT, 0, 0, MPI_COMM_WORLD, &Stat);
	
	cout << "Process id: " << id << "\n\n" << "The mat: AVERAGE\n";
	  
	for(int i = 0; i < numRows; i++) {
		for(int j = 0; j < numCols; j++) {
			cout << recData[i][j];
        }
	}
	  
	cout << "\n\n";
	
	//send leftmost collumn to id--
	//send rightmost collumn to id++
	//receive leftmost collumn of id++
	//receive rightmost collumn of id--
	while(count < k) {
		
		rc = MPI_Send(&recData[0][0], numCols, MPI_INT, destl, 0, MPI_COMM_WORLD);
		rc = MPI_Recv(&recTop, numCols, MPI_INT, sourcer, 0, MPI_COMM_WORLD, &Stat);
		rc = MPI_Recv(&recBottom, numCols, MPI_INT, sourcel, 0, MPI_COMM_WORLD, &Stat);
		rc = MPI_Send(&recData[numRows-1][0], numCols, MPI_INT, destr, 0, MPI_COMM_WORLD);

		for(int r = 0; r < numRows; r++) {
			for(int c = 0; c < numCols; c++) {
				int numAlive = 0;
				if(r == 0) { //TOP 
					if(numRows > 1) { //MORE THAN 1 ROW
						if(c == 0) { 
							numAlive += (recData[r][c+1] + recData[r+1][c+1] + recData[r+1][c] + recTop[c]  + recTop[c+1]);
						} else if(c == numCols-1) { 
							numAlive += (recData[r][c-1] + recData[r+1][c-1] + recData[r+1][c] + recTop[c]  + recTop[c-1]);
						} else { 
							numAlive += (recData[r][c+1] + recData[r+1][c+1] + recData[r+1][c] + recData[r+1][c-1] + recData[r][c-1] + recTop[c+1] + recTop[c]  + recTop[c-1]);
						}
					} else { //ONLY 1 ROW, TOP = BOTTOM
						if(c == 0) { 
							numAlive += (recData[r][c+1] + recTop[c+1] + recTop[c] + recBottom[c+1] + recBottom[c]);
						} else if(c == numCols-1) { 
							numAlive += (recData[r][c-1] + recTop[c-1] + recTop[c] + recBottom[c-1] + recBottom[c]);
						} else { 
							numAlive += (recData[r][c+1] + recTop[c+1] + recTop[c] + recTop[c-1] + recData[r][c-1] + recBottom[c+1] + recBottom[c] + recBottom[c-1]);
						}
					}

				} else if(r == numRows-1) { //BOTTOM, 
					if(c == 0) { 
						numAlive += (recData[r][c+1] + recData[r-1][c] + recData[r-1][c+1] + recBottom[c] + recBottom[c+1]);
					} else if(c == numCols-1) { 
						numAlive += (recData[r][c-1] + recData[r-1][c] + recData[r-1][c-1] + recBottom[c] + recBottom[c-1]);
					} else { 
						numAlive += (recData[r][c+1] + recData[r][c-1] + recData[r-1][c-1] + recData[r-1][c] + recData[r-1][c+1] + recBottom[c+1] + recBottom[c]  + recBottom[c-1]);
					}
				} else { //AVERAGE CASE
					if(c == 0) { //LEFTMOST POSITION
						numAlive += (recData[r-1][c] + recData[r-1][c+1] + recData[r][c+1] + recData[r+1][c+1] + recData[r+1][c]);
					} else if(c == numCols-1) { //RIGHTMOST POSITION
						numAlive += (recData[r-1][c] + recData[r-1][c-1] + recData[r][c-1] + recData[r+1][c-1] + recData[r+1][c]);
					} else { //THE REST
						numAlive += (recData[r-1][c-1] + recData[r-1][c] + recData[r-1][c+1] + recData[r][c+1] + recData[r+1][c+1] + recData[r+1][c] + recData[r+1][c-1] + recData[r][c-1]);
					}
				}

				if(recData[r][c] == 0) {
					if(numAlive >= 3) {
						recDataCopy[r][c] = 1;
					}
				} else {
					if(numAlive >= 4 || numAlive <= 2) {
						recDataCopy[r][c] = 0;
					}
				}

			}//end inner for
		}//end outer for

		for(int w = 0; w < numRows; w++) {
			for(int x = 0; x < numCols; x++) {
				recData[w][x] = recDataCopy[w][x];
			}
		}

		count++;

	}//end while
	
  } 

  MPI::Finalize();
  return 0;
}

