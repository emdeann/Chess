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

const int NONE_SELECTED = -1;

const vector<ChessPiece> BACK_ROW = {Rook(), Knight(), Bishop(), Queen(), King(), Bishop(), Knight(), Rook()};

class Board {
private:
	int height, width, cursorPos;
	vector<Cell> brd;
	int selected;

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
		selected = -1;
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
				wcout << c.getStr() << " ";
			}
			wcout << endl;
		}
		if (!rev) {
			brd.at(cursorPos).toggleCursor();
		}
	}

	void onSpace() {
		brd.at((selected == -1) ? cursorPos : selected).toggleSelected();
		if (selected == NONE_SELECTED) {
			selected = cursorPos;
			
		}
		else if (cursorPos == selected) {
			selected = NONE_SELECTED;
		}
		else {
			brd.at(selected).movePiece(brd.at(cursorPos));
			selected = NONE_SELECTED;
		}
		
	}

	void moveCursor(int x, int y) {
		int newLoc = (cursorPos + (x - width * y)) % (width * height);
		newLoc = (newLoc > 0) ? newLoc : 0;
		cursorPos = newLoc;
	}
};