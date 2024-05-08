#pragma once
#include "ChessPiece.h"
using namespace std;

class Pawn : public ChessPiece {
private:
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
		takeMoves = { boardWidth - 1, boardWidth + 1 };
		strictMoves = { boardWidth};
		range = 1;
	}

	void switchSide() override {
		ChessPiece::switchSide();
		flipVector(takeMoves);
		flipVector(strictMoves);
	}
};