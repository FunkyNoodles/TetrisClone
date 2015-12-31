#pragma once
class Coord
{
public:
	Coord() {}
	Coord(int column, int row) {
		this->column = column;
		this->row = row;
	}
	int getColumn() {
		return column;
	}
	int getRow() {
		return row;
	}
	void setColumn(int column) {
		this->column = column;
	}
	void setRow(int row) {
		this->row = row;
	}
private:
	int column, row;
};