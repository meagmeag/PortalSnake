/* curses and timing */
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sgtty.h>
#include <curses.h>
#include <signal.h>

#include <iostream>

/* timing */
const int SEC_DELAY = 0;
const int USEC_DELAY = 800000;
static char lastc = ' ';

/* curses control */
void terminateScreen();

/* input */
void getCharFatalError();
char getChar();

/* rainbow */
int NUM_ROWS;
int NUM_COLUMNS;
const int NUM_RBOW_COLORS = 5;
int COLOR_LEN;
void moveRainbow(bool clockWise);

/* calculations for drawing */
int NUM_ASTERISKS;
int offset = 0; // offset of start of red from 0th asterisk
int mod(int a, int b);
int max(int a, int b);
void getPos(int asteriskNum, int &row, int &col);

int main(int argc, char* argv[]) {
	// ensure program run with valid arguments
	if (argc != 3 ) {
		std::cout << "usage: " << argv[0] << " numRows numColumns\n"
			<< "where each is an estimate of your desired playing area\n";
		exit(1);
	}
	NUM_ROWS = atoi(argv[1]);
	NUM_COLUMNS = atoi(argv[2]);
	if (NUM_ROWS < 10 || NUM_COLUMNS < 10) {
		std::cout << "usage:" << argv[0] << " numRows numColumns\n"
			<< "numRows and numColumns must be >= 10\n";
		exit(1);	
	}

	// resize columns if needed
	if (NUM_COLUMNS % 10 != 2) { // numColumns is multiple of 10 + 2
		NUM_COLUMNS -= NUM_COLUMNS % 10;   // (could have been rows, but resizing columns
		NUM_COLUMNS += 2;    // is a safer bet since there are likely more
	}
	if (NUM_ROWS % 10 != 0) { // numRows is a multiple of 10
		NUM_ROWS -= NUM_ROWS % 10;
	}
	// why? the two numbers must add to a multiple of 10 + 2
	// for the number of asterisks to be divisible by NUM_RBOW_COLORS * 2
	NUM_ASTERISKS = 2 * NUM_COLUMNS + 2 * NUM_ROWS - 4; // minus the 4 corners, which are counted twice
	COLOR_LEN = NUM_ASTERISKS / (NUM_RBOW_COLORS * 2);
	
	// start screen
	initscr();	 /* activate the curses */
	curs_set(0);	 /* do not display the cursor */
	clear();	 /* clear the screen that curses provides */
	noecho();	 /* prevent the input chars to be echoed to the screen */
	cbreak();	 /* change the stty so that characters are delivered to the
			    program as they are typed--no need to hit the return key!*/
	
	// initialize colors
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);

	// draw screen:
	// Q to quit
	move(NUM_ROWS, max((NUM_COLUMNS - 15) / 2, 0));
	attron(COLOR_PAIR(1));
	attron(A_BOLD);
	addstr(&"PRESS Q TO QUIT"[0]);
	attroff(COLOR_PAIR(1));
	attroff(A_BOLD);
	// rainbow
	int row = 0;
	int col = 0;
	int currAsterisk = 0;
	for (int i = 0; i < NUM_RBOW_COLORS * 2; i++) {
		attron(COLOR_PAIR(i % NUM_RBOW_COLORS + 1));
		for (int j = 0; j < COLOR_LEN; j++) {
			if (currAsterisk >= NUM_ASTERISKS) {
				std::cout << "ERROR\n";
			}
			getPos(currAsterisk, row, col);
			mvaddch(row, col, '*');
			currAsterisk++;
		}
		attroff(COLOR_PAIR(i % NUM_RBOW_COLORS + 1));
	}
	refresh();
	
	while (lastc != 'Q'){
		moveRainbow(true);
		/*if (lastc == 'A') {
			moveRainbow(false);
		}
		else if (lastc == 'D') {
			moveRainbow(true);
		}*/
		getChar();
	}
	terminateScreen();
	return 0;
}

int mod(int a, int b) {
	if (a > -1) {
		return a % b;
	}
	int div = -b;
	while (div > a) {
		div -= b;
	}
	return a - div;
}

int max(int a, int b) {
	if (a > b)
		return a;
	else
		return b;
}

void getPos(int asteriskNum, int &row, int &col) {
	/*******************************************
	 *	The following is based on
	 *	splitting the bounding asterisks
	 *	into sections 1, 2, 3, & 4.
	 *
	 *	111111
	 *	******
	 *     4*    *2
	 *     4*    *2
	 *	******2
	 *      33333
	 *
	 *      In sections 1 & 3, the row is
	 *      known. In 2 & 4. The column is
	 *      known. To calculate the unknown
	 *      row or column, subtract the
	 *      number of remaining asterisks
	 *      from the "last" row or column
	 *      in the secton.
	 *      [lastRow/Col - numAsterisksLeft]
	 *
	 *      The number of asterisks remaining
	 *      is [asteriskNum + 1 - asterisks in
	 *      previous sections], the latter being
	 *      the RHS of the < in previous if 
	 *      statement.
	 ********************************************/
	if (asteriskNum < 0) {
		terminateScreen();
		std::cout << "ERROR: asteriskNum passed to getPos() invalid - less than 0.\n";
		exit(1);
	}
	else if (asteriskNum < NUM_COLUMNS) { // section 1
		row = 0;
		col = asteriskNum;
	}
	else if (asteriskNum < NUM_COLUMNS + NUM_ROWS - 1) { // section 2
		row = asteriskNum + 1 - NUM_COLUMNS;
		col = NUM_COLUMNS - 1;
	}
	else if (asteriskNum < NUM_COLUMNS + NUM_ROWS - 1 + NUM_COLUMNS - 1) { // section 3
		row = NUM_ROWS - 1;
		col = NUM_COLUMNS - 1 - (asteriskNum + 1 - (NUM_COLUMNS + NUM_ROWS - 1));
	}
	else if (asteriskNum < NUM_ASTERISKS) { // section 4
		row = NUM_ROWS - 1 - (asteriskNum + 1 - (NUM_COLUMNS +  NUM_ROWS - 1 + NUM_COLUMNS - 1));
		col = 0;
	}
	else {
		terminateScreen();
		std::cout << "ERROR: asteriskNum passed to getPos() invalid - greater than or equal to number of asterisks.\n";
		exit(1);
	}
}

void moveRainbow(bool clockWise) {
	if (clockWise) {
		for (int i = 0; i < NUM_RBOW_COLORS; i++) {
			attron(COLOR_PAIR(i + 1));
			// first section of color
			int asteriskNum = mod(offset + i * COLOR_LEN + COLOR_LEN, NUM_ASTERISKS);
			int row, col;
			getPos(asteriskNum, row, col);
			mvaddch(row, col, '*');
			// second section of color
			asteriskNum = mod(offset + COLOR_LEN * NUM_RBOW_COLORS + i * COLOR_LEN + COLOR_LEN, NUM_ASTERISKS);
			getPos(asteriskNum, row, col);
			mvaddch(row, col, '*');
			// color done
			attroff(COLOR_PAIR(i + 1));
		}
		offset++; // shifted all color sections one space foward
		offset = mod(offset, NUM_ASTERISKS);
	}
	else {
		// implement me?
	}
	refresh();
}

void terminateScreen() {
	mvcur(0, COLS - 1, LINES - 1, 0);
	clear();
	curs_set(1);
	refresh();
	endwin();
}

void getCharFatalError() {
	signal(SIGINT, SIG_IGN);
	mvcur(0, COLS - 1, LINES - 1, 0);
	endwin();
	exit(1);
}

char getChar() {
	char c;
	struct timeval tvec;
	fd_set rfds;

	tvec.tv_sec = SEC_DELAY;
	tvec.tv_usec = USEC_DELAY;
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	
	int retval = select(1, &rfds, NULL, NULL, &tvec);

	if ( retval < 0 )
		getCharFatalError();

     	if ( retval ) {
        	 if (read(0, &c, 1) < 0)
			 getCharFatalError();
	 	else 
	 		lastc = std::toupper(c);
     	}
     return( lastc );
}
