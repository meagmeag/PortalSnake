//
// Created by meagh on 11/21/2024.
//

#ifndef PROJECT5_GAME_HPP
#define PROJECT5_GAME_HPP

#include "Screen.hpp"
#include "Worm.hpp"
#include <iostream>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sgtty.h>
#include <curses.h>
#include <signal.h>

class Game {
	public:
		Game(int numRows, int numColumns);
		void play();
	private:
		int numRows, numColumns;
		Screen screen;

		Worm worm;
		void moveWorm();

		int score;
		int scoreNumCol;

		void newMunchie();
		Cell munchieLoc;
		int munchieValue;
		int segmentsToAddToWorm;

		char userInput;
		bool isValidInput(char c);

		void print(); // used for debug outside of curses

		void getCharFatalError( void );
		char getChar();
		void startupScreen( void );
		void terminateScreen( void );
		void gameOver(bool wormDied);


		/* rainbow */ 
		const int NUM_RBOW_COLORS = 5;
		int COLOR_LEN;
		void moveRainbow(bool clockWise);

		/* calculations for drawing */
		int NUM_ASTERISKS;
		int offset = 0; // offset of start of red from 0th asterisk
		int mod(int a, int b);
		int max(int a, int b);
		void getPos(int asteriskNum, int &row, int &col);
		int posToAsterisk(int row, int col);
		int getCompAsterisk(int asteriskNum);
};


#endif //PROJECT5_GAME_HPP
