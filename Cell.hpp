#ifndef CELL_H
#define CELL_H

#include <string>

class Cell {
public:
	Cell() : row(-1), column(-1) {}
    void setRow(int cellRow);
    void setColumn(int cellColumn);
	int getRow() const;
	int getColumn() const;
    std::string toString();
private:
	int row, column;	
};

#endif
