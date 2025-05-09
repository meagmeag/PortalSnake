#ifndef SCREEN_H
#define SCREEN_H

#include "Cell.hpp"
#include <vector>
#include <iostream>
#include <iomanip>

class Screen {
public:
    Screen(int numRows, int numColumns);
    void printScreen(int wormHeadRow, int wormHeadCol);
    void printFreePool();
    void printAll(int wormHeadRow, int wormHeadCol);
    void makeFree(int row, int column);
    void makeOccupied(int row, int column);
    bool isFree(int row, int column) const;
    Cell getRandFreeCell();
private:
    void initializeScreen();
    int numRows, numColumns;
    std::vector<std::vector<int>> screen;
    std::vector<Cell> freePool;
    int fpLastIndex;

    // variables for set width during printing
    int numMaxDigitsScreen;
    int numMaxDigitsFP;

};

#endif
