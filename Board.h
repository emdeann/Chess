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
#include <set>
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
	set<int> currentValidMoves;

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
			p.switchSide();
			brd.at((h * w - w) + i).setChessPiece(p);
			brd.at(w + i).setChessPiece(pawn);
			pawn.switchSide();
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
		if (selected == NONE_SELECTED) {
			ChessPiece& curPiece = brd.at(cursorPos).getChessPiece();
			if (curPiece.isActive()) {
				selected = cursorPos;
				currentValidMoves = getPossibleMoves(selected, curPiece.getRange(), curPiece.getValidDirections());
				toggleMoveHighlights();
				brd.at(selected).toggleSelected();
			}
			
		}
		else {
			if (currentValidMoves.find(cursorPos) != currentValidMoves.end()) {
				brd.at(selected).movePiece(brd.at(cursorPos));
				brd.at(selected).toggleSelected();
				selected = NONE_SELECTED;
				toggleMoveHighlights();
				currentValidMoves.clear();
			}

		}
		
	}

	void toggleMoveHighlights() {
		for (int i : currentValidMoves) {
			brd.at(i).toggleHighlight();
		}
	}

	void moveCursor(int x, int y) {
		int newLoc = (cursorPos + (x - width * y)) % (width * height);
		newLoc = (newLoc > 0) ? newLoc : 0;
		cursorPos = newLoc;
	}
												// Horizontal, Vertical, Diagonal
	set<int> getPossibleMoves(int pos, int range, vector<bool> permissions) {
		set<int> allMoves;
		vector<set<int>> possibleMoves = { getMoves(pos, range, 'h'), getMoves(pos, range, 'v') };
		for (int i = 0; i < possibleMoves.size(); i++) {
			if (permissions.at(i)) {
				set<int> cur = possibleMoves.at(i);
				allMoves.insert(cur.begin(), cur.end());
			}
		}
		return allMoves;
	}

	set<int> getStraightMoves(int pos, int range, bool horizontal = true) {
		int bound1 = pos, bound2 = pos, step = horizontal + !horizontal * width;
		int min = (horizontal) ? (pos - pos % width - 1) : (pos - width * (pos / height));
		int max = (horizontal) ? ((pos - pos % width) + width) : (width * height - (width - pos % width));
		set<int> validMoves;
		while (bound1 != min && (-(bound1 - pos) <= (horizontal ? range : range * width))
					&& (!brd.at(bound1).getChessPiece().isActive() || bound1 == pos)) {
			bound1 -= step;
		}

		while (bound2 != max && (bound2 - pos <= (horizontal ? range : range * width))
					&& (!brd.at(bound2).getChessPiece().isActive() || bound2 == pos)) {
			bound2 += step;
		}
		for (int i = bound1 + step; i < bound2; i += step) {
			validMoves.insert(i);
		}
		return validMoves;
	}

	set<int> getMoves(int pos, int range, char method) {
		int bound1 = pos, bound2 = pos, step, min, max, rangeMax;
		switch (method) {
		case 'h':
			min = pos - pos % width - 1;
			max = (pos - pos % width) + width;
			step = 1;
			rangeMax = range;
			break;
		case 'v':
			min = pos - width * (pos / height + 1);
			max = width * (height + 1) - (width - pos % width);
			step = width;
			rangeMax = range * width;
			break;
		case 'd':
			step = width + 1;
			rangeMax = range * (width + 1);
		}
		set<int> validMoves;
		while (bound1 > min && (-(bound1 - pos) <= rangeMax)
			&& (!brd.at(bound1).getChessPiece().isActive() || bound1 == pos)) {
			bound1 -= step;
		}

		while (bound2 < max && (bound2 - pos <= rangeMax)
			&& (!brd.at(bound2).getChessPiece().isActive() || bound2 == pos)) {
			bound2 += step;
		}
		for (int i = bound1 + step; i < bound2; i += step) {
			validMoves.insert(i);
		}
		return validMoves;
	}

};