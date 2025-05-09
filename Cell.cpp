//
// Created by meagh on 11/21/2024.
//


#include "Cell.hpp"

int Cell::getRow() const {
    return row;
}
int Cell::getColumn() const {
    return column;
}

void Cell::setRow(int cellRow) {
    row = cellRow;
}
void Cell::setColumn(int cellColumn) {
    column = cellColumn;
}

std::string Cell::toString() {
    return std::to_string(row) + "," + std::to_string(column);
}
