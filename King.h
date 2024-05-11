#pragma once
#include "ChessPiece.h"
using namespace std;

class King : public ChessPiece {
private:
	bool castle;
public:
	King() {
		ch = L"\u2654";
		activePiece = true;
		validDirections = { true, true, true };
		range = 1;
		name = "king";
		texture.loadFromFile("Textures/w_king.png");
		value = 0;
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