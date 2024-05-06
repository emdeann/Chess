#pragma once
#include "windows.h"
#include "Cell.h"
#include "ChessPiece.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "Knight.h"
#include "King.h"
#include "Bishop.h"
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

const vector<ChessPiece> BACK_ROW = {Rook(), Knight(), Bishop(), Queen(), King(), Bishop(), Knight(), Rook()};

class Board {
private:
	int height, width, cursorPos;
	vector<Cell> brd;

	void ShowConsoleCursor(bool showFlag)
	{
		//-------do not change-------
		//hides the cursor, use only once at program start
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(out, &cursorInfo);
		cursorInfo.bVisible = showFlag; // set the cursor visibility
		SetConsoleCursorInfo(out, &cursorInfo);
	}
	BOOL gotoxy(const WORD x, const WORD y) {
		//-------do not change-------
		COORD xy;
		xy.X = x;
		xy.Y = y;
		return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
	}

	void moveCursor(int value) {
		int newLoc = (cursorPos + value) % (height * width);
		newLoc = (newLoc > 0) ? newLoc : 0;
		cursorPos = newLoc;
	}
public:
	Board(int h, int w) {
		ShowConsoleCursor(false);
		height = h;
		width = w;
		brd = vector<Cell>(h * w);
		for (int i = 0; i < w; i++) {
			ChessPiece p = BACK_ROW.at(i);
			Pawn pawn;
			brd.at(i).setChessPiece(p);
			brd.at((h * w - w) + i).setChessPiece(p);
			brd.at(w + i).setChessPiece(pawn);
			brd.at((h * (w - 2) + i)).setChessPiece(pawn);
		}
	}

	void draw(bool rev) {
		if (!rev) {
			brd.at(cursorPos).toggleCursor();
		}
		gotoxy(0, 0);
		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				Cell& c = brd.at(w + h * width);
				wcout << c.getChar() << " ";
			}
			wcout << endl;
		}
		if (!rev) {
			brd.at(cursorPos).toggleCursor();
		}
	}

	void moveCursor(int x, int y) {
		int newLoc = (cursorPos + (x - width * y)) % (width * height);
		newLoc = (newLoc > 0) ? newLoc : 0;
		cursorPos = newLoc;
	}
};