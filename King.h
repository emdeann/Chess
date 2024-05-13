#pragma once
#include "ChessPiece.h"
using namespace std;

class King : public ChessPiece {
private:
	bool castle;
public:
	King() : ChessPiece(1, 0, { true, true, true }, "king") {	
		castle = true;
	}

	bool isKing() override {
		return true;
	}

	void onMove(int moveDiff, int moveNum) override {
		castle = false;
	}

	bool canCastle() override {
		return castle;
	}

};