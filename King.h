#pragma once
#include "ChessPiece.h"
using namespace std;

class King : public ChessPiece {
public:
	King() {
		ch = L"\u2654";
		activePiece = true;
		validDirections = { true, true, true };
		range = 1;
		name = "king";
		texture.loadFromFile("Textures/w_king.png");
		value = 0;
	}

	void switchSide() override {
		ChessPiece::switchSide();
		ostringstream fileName;
		fileName << "Textures/" << ((side) ? "b" : "w") << "_king.png";
		texture.loadFromFile(fileName.str());
	}
};