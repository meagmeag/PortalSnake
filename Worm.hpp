#ifndef WORM_H
#define WORM_H

#include "Cell.hpp"
#include <vector>
#include <iostream>
#include <iomanip>

class Worm {
public:
    Worm(int numCells);
    Cell getHead() const;
    Cell getTail() const;
    void addHead(int row, int column);
    Cell removeTail();
    void print();
private:
    std::vector<Cell> segments;
    int nextHeadIndex, tailIndex;
};

#endif
