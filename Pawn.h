#pragma once
#include "ChessPiece.h"
using namespace std;

class Pawn : public ChessPiece {
private:
	bool firstMove;

	void flipVector(vector<int>& v) {
		for (int& i : v) {
			i *= -1;
		}
	}
public:
	Pawn(int boardWidth) {
		ch = L"\u2659";
		name = "pawn";
		activePiece = true;
		strictMotion = true;
		specialTakeMoves = true;
		firstMove = true;
		takeMoves = { boardWidth - 1, boardWidth + 1 };
		strictMoves = { boardWidth, 2 * boardWidth};
		value = 1;
	}

	void onMove() override {
		if (firstMove) {
			strictMoves.erase(strictMoves.begin() + 1);
			firstMove = false;
		}
	}

	void switchSide() override {
		ChessPiece::switchSide();
		flipVector(takeMoves);
		flipVector(strictMoves);
	}

	bool isPawn() const override {
		return true;
	}

};