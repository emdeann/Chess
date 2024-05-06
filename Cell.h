#pragma once
#include <string>
#include "ChessPiece.h"

using namespace std;

class Cell {
private:
	bool isCursor, isSelected;
	ChessPiece* piece;
public:
	Cell() {
		isCursor = false;
		isSelected = false;
		piece = nullptr;
	}
	void toggleSelected() {
		isSelected = !isSelected;
	}

	void toggleCursor() {
		isCursor = !isCursor;
	}

	char getChar() {
		char ch;
		if (piece != nullptr) {
			ch = piece->getChar();
		}
		else if (isCursor) {
			ch = 'x';
		}
		else {
			ch = '|';
		}

		return ch;
	}
};