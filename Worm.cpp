#include "Worm.hpp"

Worm::Worm(int numCells) {
    segments.resize(numCells);
    nextHeadIndex = tailIndex = 0;
}

Cell Worm::getHead() const {
    return segments.at((nextHeadIndex - 1) % segments.size());
}

Cell Worm::getTail() const {
    return segments.at(tailIndex);
}

void Worm::addHead(int row, int column) {
    segments.at(nextHeadIndex).setRow(row);
    segments.at(nextHeadIndex).setColumn(column);
    nextHeadIndex = (nextHeadIndex + 1) % segments.size();
}

Cell Worm::removeTail() {
    tailIndex = (tailIndex + 1) % segments.size();
    return segments.at((tailIndex - 1) % segments.size());
}

void Worm::print() {
    std::cout << "WORM (tail to head)\n";
    int numCellsPrinted = 0;
    for (int i = tailIndex; i != nextHeadIndex; i = (i + 1) % segments.size()) {
        std::cout << segments.at(i).toString() << "  ";
        numCellsPrinted++;
        if (numCellsPrinted % 15 == 0) {
            std::cout << std::endl;
        }
    }
}
