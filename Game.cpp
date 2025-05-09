//
// Created by meagh on 11/21/2024.
//

#include "Game.hpp"

const int SEC_DELAY = 0;
const int USEC_DELAY = 800000;
static char lastc = ' ';

Game::Game(int numRows, int numColumns) : screen(numRows, numColumns), worm((numRows - 2) * (numColumns - 2)) {
	score = segmentsToAddToWorm = 0;
	userInput = '?';	
	this->numRows = numRows;
	this->numColumns = numColumns;
	NUM_ASTERISKS = 2 * numColumns + 2 * numRows - 4; // minus the 4 corners, which are counted twice
	COLOR_LEN = NUM_ASTERISKS / (NUM_RBOW_COLORS * 2);
	startupScreen();
}

void Game::play() {
	while (userInput != 'Q') {
		userInput = getChar(); // getChar() checks input is valid
		moveWorm(); // moveWorm() checks for worm collisions
		moveRainbow(true);
		refresh();
	}
	// if this line is reached, user pressed Q
	terminateScreen();
	gameOver(false); // worm did not die
}

void Game::print() {
	std::cout << std::endl;
	screen.printAll(worm.getHead().getRow(), worm.getHead().getColumn());
	worm.print();
	std::cout << "\n\n";
}

void Game::moveWorm() {
	int nextHeadRow = worm.getHead().getRow();
	int nextHeadColumn = worm.getHead().getColumn();
	if (userInput == 'W' || userInput == 'K') { // going up a row
		nextHeadRow -= 1;
	}
	else if (userInput == 'S' || userInput == 'J') { // going down a row
		nextHeadRow += 1;
	}
	else if (userInput == 'A' || userInput == 'H') { // going left a column
		nextHeadColumn -= 1;
	}
	else if (userInput == 'D' || userInput == 'L') { // going right a column
		nextHeadColumn += 1;
	}
	else {
		return;
	}

	// hit a wall or the worm
	if (!screen.isFree(nextHeadRow, nextHeadColumn)
			&& !(nextHeadRow == worm.getTail().getRow() && nextHeadColumn == worm.getTail().getColumn() && segmentsToAddToWorm == 0)) {
		// ^ if the snake is about to hit its tail and no new segment is going to spawn,
		// that is fine because the tail is going to move out of the way

		if (nextHeadRow != 0 && nextHeadRow != numRows - 1
				&& nextHeadColumn != 0 && nextHeadColumn != numColumns - 1) {
			// snake hit self = game over
			terminateScreen();
			gameOver(true); // worm died
		}

		int nextPos = getCompAsterisk(posToAsterisk(nextHeadRow + 1, nextHeadColumn));
		getPos(nextPos, nextHeadRow, nextHeadColumn);
		if (nextHeadRow == numRows) {
			nextHeadRow = numRows - 2;
		}
		if (nextHeadColumn == 0) {
			nextHeadColumn++;
		}
		else if (nextHeadColumn >= numColumns - 1) {
			nextHeadColumn = numColumns - 2;
		}
	}

	// move worm 
	mvaddch(worm.getHead().getRow() + 1, worm.getHead().getColumn(), 'o'); // previous head position is now body segment
	if (segmentsToAddToWorm > 0) { // segments to be added to worm - do not move tail 
		segmentsToAddToWorm--;
	}
	else { // no segments to add - move tail
		Cell oldTail = worm.removeTail();
		screen.makeFree(oldTail.getRow(), oldTail.getColumn());
		mvaddch(oldTail.getRow() + 1, oldTail.getColumn(), 32); // 32 for space
	}
	worm.addHead(nextHeadRow, nextHeadColumn);
	mvaddch(nextHeadRow + 1, nextHeadColumn, '@');
	screen.makeOccupied(nextHeadRow, nextHeadColumn);

	// ate munchie?
	if (nextHeadRow == munchieLoc.getRow() && nextHeadColumn == munchieLoc.getColumn()) {
		score += munchieValue;
		move(0, scoreNumCol);
		addstr(&(std::to_string(score)[0]));
		segmentsToAddToWorm += munchieValue; 
		newMunchie();
	}
}

void Game::startupScreen( void )
{
	initscr();	 /* activate the curses */
	curs_set(0);/* do not display the cursor */
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

	// game details
	int minScoreCol = numColumns - 11;
	int maxWormCol = 3;
	while ((maxWormCol + 12) >= minScoreCol) { // ensure game title and score don't overlap
		if (maxWormCol > 0) {
			maxWormCol--;
		}	    
		minScoreCol++;
	}
	scoreNumCol = minScoreCol + 7; 
	move(0, maxWormCol);
	addstr("Portal Snake");
	move (0, minScoreCol);
	addstr("Score  0");

	// rainbow outline	
	int row = 1;
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

	// add worm head and munchie
	int spawnRow = numRows / 2;
	int spawnCol = numColumns / 2;
	worm.addHead(spawnRow, spawnCol);
	screen.makeOccupied(spawnRow, spawnCol);
	newMunchie();
	mvaddch(spawnRow + 1, spawnCol, '@');
	mvaddch(munchieLoc.getRow() + 1, munchieLoc.getColumn(), munchieValue + 48);

	// Q to quit
	int pressQCol = (numColumns - 15)  / 2; 
	if (pressQCol < 0) {
		pressQCol = 0;
	}
	move(numRows + 1, pressQCol);
	attron(COLOR_PAIR(1));
	attron(A_BOLD);
	addstr("PRESS Q TO QUIT");
	attroff(COLOR_PAIR(1));
	attroff(A_BOLD);

	refresh();
}

void Game::terminateScreen( void )
{
	mvcur( 0, COLS - 1, LINES - 1, 0 );
	clear();
	refresh();
	endwin();
}

char Game::getChar( )
{
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
		if ( read(0, &c, 1) < 0)
			getCharFatalError();
		c = std::toupper(c); // comparisons in uppercase
		if (isValidInput(c)) {
			lastc = c;
		}
	}
	return( lastc );
}

bool Game::isValidInput(char c) {
	return ( c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'Q'
			|| c == 'H' || c == 'J' || c == 'K' || c == 'L');
}

void Game::getCharFatalError( void )
{
	signal( SIGINT, SIG_IGN );
	mvcur( 0, COLS - 1, LINES - 1, 0 );
	endwin();
	exit( 1 );
}

void Game::gameOver(bool wormDied) {
	if (wormDied) {
		std::cout << "The worm die because it hit something!\n";
	}
	else {
		std::cout << "Game ended by user.\n";
	}
	int scoreAreaPercent =(score * 100) / ((numRows - 2) * (numColumns - 2));
	std::cout << "Final score was: " << score << std::endl
		<< "(" << scoreAreaPercent << "% of game play area.)\n"; 
	if (score == 0) {
		std::cout << "Did you even try?\n";
	}
	else if (scoreAreaPercent < 10) {
		std::cout << "You could do better...\n";
	}
	else if (scoreAreaPercent < 20) {
		std::cout << "You did ok I guess.\n";
	}
	else if (scoreAreaPercent < 30) {
		std::cout << "You did alright.\n";
	}
	else if (scoreAreaPercent < 50) {
		std::cout << "You did awesome!\n";
	}
	else {
		std::cout << "You are a worm god!\n";
	}
	exit(0);     
}

void Game::newMunchie() {
	munchieLoc = screen.getRandFreeCell();
	munchieValue = rand() % 9 + 1;
	mvaddch(munchieLoc.getRow() + 1, munchieLoc.getColumn(), munchieValue + 48);
}

int Game::mod(int a, int b) {
	if (a > -1) {
		return a % b;
	}
	int div = -b;
	while (div > a) {
		div -= b;
	}
	return a - div;
}

int Game::max(int a, int b) {
	if (a > b)
		return a;
	else
		return b;
}

int Game::getCompAsterisk(int asteriskNum) {
	return mod(asteriskNum + NUM_RBOW_COLORS * COLOR_LEN, NUM_ASTERISKS);
}

void Game::getPos(int asteriskNum, int &row, int &col) {
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
	else if (asteriskNum < numColumns) { // section 1
		row = 1;
		col = asteriskNum;
	}
	else if (asteriskNum < numColumns + numRows - 1) { // section 2
		row = 2 + asteriskNum - numColumns; // 1 + asteriskNum + 1 - numColumns
		col = numColumns - 1;
	}
	else if (asteriskNum < numColumns + numRows - 1 + numColumns - 1) { // section 3
		row = numRows;
		col = 2 * numColumns + numRows - asteriskNum - 3; // numColumns - 1 - (asteriskNum + 1 - (numColumns + numRows - 1))
	}
	else if (asteriskNum < NUM_ASTERISKS) { // section 4
		row =  2 * numRows + 2 * numColumns - asteriskNum - 3; // numRows - (asteriskNum + 1 - (numColumns +  numRows - 1 + numColumns - 1))
		col = 0;
	}
	else {
		terminateScreen();
		std::cout << "ERROR: asteriskNum passed to getPos() invalid - " << asteriskNum << "/" 
			<< NUM_ASTERISKS << ".\n";
		exit(1);
	}
}

int Game::posToAsterisk(int row, int col) {
	// follows same "sections" as above
	if ( row < 1 || row > numRows || col < 0 || col >= numColumns) {
		terminateScreen();
		std::cout << "ERROR: out of bounds in posToAsterisk()\n"
			<< "row: " << row << "/" << numRows << std::endl
			<< "column: " << col << "/" << numColumns - 1 << std::endl;
		exit(1);
	}

	if (row == 1) { // section 1
		return col;
	}
	else if (row == numRows) { // section 3
		return 2 * numColumns + numRows - col - 3;
	}

	if (col == 0) { // section 4
		return 2 * numRows + 2 * numColumns - row - 3;
	}
	else if (col == numColumns - 1) { // section 2
		return row + numColumns - 2;
	}

	terminateScreen();
	std::cout << "ERROR: unknown issue in posToAsterisk().\n"
			<< "row: " << row << "/" << numRows << std::endl
			<< "column: " << col << "/" << numColumns - 1 << std::endl;
	exit(1);
}

void Game::moveRainbow(bool clockWise) {
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
}

