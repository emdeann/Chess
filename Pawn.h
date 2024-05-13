#pragma once
#include "ChessPiece.h"
using namespace std;

class Pawn : public ChessPiece {
private:
	int moves, lastMoveDiff, boardWidth, doubleMoveTurn;

	void flipVector(vector<int>& v) {
		for (int& i : v) {
			i *= -1;
		}
	}
public:
	Pawn(int w) : ChessPiece(1, 1, { false, false, false }, "pawn") {
		boardWidth = w;
		strictMotion = true;
		specialTakeMoves = true;
		lastMoveDiff = 0;
		moves = 0;
		doubleMoveTurn = -1;
		takeMoves = { w - 1, w + 1 };
		strictMoves = { w, 2 * w};
	}

	void onMove(int moveDiff, int moveNum) override {
		ChessPiece::onMove(moveDiff, moveNum);
		if (!moves++) {
			if (moveDiff == 2 * boardWidth) {
				doubleMoveTurn = moveNum;
			}
			strictMoves.erase(strictMoves.begin() + 1);
		}
		lastMoveDiff = moveDiff;
	}

	void switchSide() override {
		ChessPiece::switchSide();
		flipVector(takeMoves);
		flipVector(strictMoves);
	}

	bool canBeEnPassanted(int moveNum) override {
		// First move, moved two spaces, turn after move
		return moves == 1 && lastMoveDiff == 2 * boardWidth && moveNum == doubleMoveTurn + 1;
	}

	bool isPawn() const override {
		return true;
	}

};