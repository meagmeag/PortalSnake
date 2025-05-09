#include <iostream>
#include "Game.hpp"


int main (int argc, char* argv[]) {
	// ensure program run with valid arguments
	if (argc != 3 ) {
		std::cout << "usage: " << argv[0] << " numRows numColumns\n"
			<< "where each is an estimate of your desired playing area\n";
		exit(1);
	}
	int numRows = atoi(argv[1]);
	int numColumns = atoi(argv[2]);
	if (numRows < 10 || numColumns < 10) {
		std::cout << "usage:" << argv[0] << " numRows numColumns\n"
			<< "numRows and numColumns must be >= 10\n";
		exit(1);	
	}

	// resize columns if needed
	if (numColumns % 10 != 2) { // numColumns is multiple of 10 + 2
		numColumns -= numColumns % 10;   // (could have been rows, but resizing columns
		numColumns += 2;    // is a safer bet since there are likely more
	}
	if (numRows % 10 != 0) { // numRows is a multiple of 10
		numRows -= numRows % 10;
	}
	if (argc != 3) {
		std::cout << "usage: " << argv[0] << " numRows numColumns\n";
		exit(1);
	}
	
	
    	srand(time(NULL));
    	Game wormGame(numRows, numColumns);
    	wormGame.play(); // game loop in Game.cpp
	return 0;
}
