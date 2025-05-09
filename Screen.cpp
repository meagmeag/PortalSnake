#include "Screen.hpp"

Screen::Screen(int numRows, int numColumns) {
    this->numRows = numRows;
    this->numColumns = numColumns;
	screen.resize(numRows);
	for (int row = 0; row < numRows; row++) {
		screen.at(row).resize(numColumns);
	}
    freePool.resize((numRows - 2) * (numColumns - 2)); // top and bottom and left and right cells not used
    fpLastIndex = freePool.size();
    
    // variables used for printing
    numMaxDigitsScreen = 1;
    int screenLargestNum = freePool.size() - 1;
    while (screenLargestNum > 0) {
        numMaxDigitsScreen++;
        screenLargestNum /= 10;
    }
    numMaxDigitsFP = 2; // starts at 1 for comma
    int largestRowNum = numRows - 1;
    while (largestRowNum > 0) {
        numMaxDigitsFP++;
        largestRowNum /= 10;

    }
    int largestColNum = numColumns - 1;
    while (largestColNum > 0) {
        numMaxDigitsFP++;
        largestColNum /= 10;
    }
    
    initializeScreen();
}

void Screen::printScreen(int wormHeadRow, int wormHeadCol) {
    std::cout << std::setw(numMaxDigitsScreen + 1) << ""; // to line column numbers up
    for (int colNum = 0; colNum < numColumns; colNum++) { // print col numbers
        std::cout << std::setw(numMaxDigitsScreen + 1) << colNum;
    }
    std::cout << std::endl;
    for (int numRow = 0; numRow < screen.size(); numRow++) { // loop through each row
        std::cout << std::setw(numMaxDigitsScreen + 1) << numRow; // print current
        for (int numCol = 0; numCol < screen.at(0).size(); numCol++) {
            if (numRow == wormHeadRow && numCol == wormHeadCol) {
                std::cout << std::setw(numMaxDigitsScreen + 1) << "@";
            }
            else {
                std::cout << std::setw(numMaxDigitsScreen + 1) << screen.at(numRow).at(numCol);
            }
        }
        std::cout << std::endl;
    }
}

void Screen::printFreePool() {
    for (int i = 0; i < fpLastIndex; i++) {
        std::cout << std::setw(numMaxDigitsFP + 5) << std::left << std::to_string(i) + ": " + freePool.at(i).toString() + " ";
        if ((i + 1) % 10 == 0 && (i + 1) != fpLastIndex) { // new line every ten cells (unless no more cells to print)
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void Screen::printAll(int wormHeadRow, int wormHeadCol) {
    std::cout << "SCREEN" << std::endl;
    printScreen(wormHeadRow, wormHeadCol);
    std::cout << "FREE-POOL" << std::endl;
    printFreePool();
}

void Screen::initializeScreen() {
    // top and bottom rows
    for (int col = 0; col < numColumns; col++) {
        screen.at(0).at(col) = -1;
        screen.at(numRows - 1).at(col) = -1;
    }

    // middle
    int freePoolIndex = 0;
    for (int row = 1; row < numRows - 1; row++) {
        screen.at(row).at(0) = screen.at(row).at(numColumns - 1) = -1;
        for (int col = 1; col < numColumns - 1; col++) {
            freePool.at(freePoolIndex).setRow(row);
            freePool.at(freePoolIndex).setColumn(col);
            screen.at(row).at(col) = freePoolIndex;
            freePoolIndex++;
        }
    }
}

void Screen::makeFree(int row, int column) {
    if (screen.at(row).at(column) != -1) { // already free
        return;
    }

    screen.at(row).at(column) = fpLastIndex;
    freePool.at(fpLastIndex).setRow(row);
    freePool.at(fpLastIndex).setColumn(column);
    fpLastIndex++;
}

void Screen::makeOccupied(int row, int column) {
    int cellToOccupyIdx = screen.at(row).at(column);
    if (cellToOccupyIdx == -1) { // already occupied
        return;
    }

    Cell endFP = freePool.at(fpLastIndex - 1); // last unoccupied cell
    // move values of last unoccupied cell to the index of the cell to make occupied
    screen.at(endFP.getRow()).at(endFP.getColumn()) = cellToOccupyIdx;
    freePool.at(cellToOccupyIdx).setRow(endFP.getRow());
    freePool.at(cellToOccupyIdx).setColumn(endFP.getColumn());
    screen.at(row).at(column) = -1;
    fpLastIndex--;
}

bool Screen::isFree(int row, int column) const {
    return screen.at(row).at(column) != -1;
}

Cell Screen::getRandFreeCell() {
     return freePool.at(rand() % fpLastIndex); // available cells are at indexes [0, fpLastIndex - 1]
}
