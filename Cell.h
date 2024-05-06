#pragma once
#include <string>
#include "ChessPiece.h"

using namespace std;

const wstring RESET = L"\033[0m";
const wstring RED = L"\033[31m";
const wstring GREEN = L"\033[32m";
const wstring YELLOW = L"\033[33m";
const wstring BLUE = L"\033[34m";
const wstring MAGENTA = L"\033[35m";

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

	void movePiece(Cell& other) {
		other.piece = piece;
		piece = ChessPiece();
	}

	wstring getStr() {
		wstring ch;
		if (isCursor) {
			ch = L"x";
		}
		else {
			ch = piece.getChar();
		}

		if (isSelected) {
			ch = YELLOW + ch + RESET;
		}

		return ch;
	}
};