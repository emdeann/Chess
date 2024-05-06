#pragma once
#include <string>
#include "ChessPiece.h"

using namespace std;

class Cell {
private:
	bool isCursor, isSelected;
	ChessPiece piece;
public:
	Cell() {
		isCursor = false;
		isSelected = false;
	}
	void toggleSelected() {
		isSelected = !isSelected;
	}

	void toggleCursor() {
		isCursor = !isCursor;
	}

	void setChessPiece(ChessPiece& p) {
		piece = p;
	}

	ChessPiece& getChessPiece() {
		return piece;
	}

	wchar_t getChar() {
		wchar_t ch;
		if (isCursor) {
			ch = 'x';
		}
		else {
			ch = piece.getChar();
		}

		return ch;
	}
};