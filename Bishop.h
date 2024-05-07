#pragma once
#include "ChessPiece.h"
using namespace std;

class Bishop : public ChessPiece {
public:
	Bishop() {
		ch = L"\u2657";
		validDirections = { false, false, true };
		range = MAX_RANGE;
		activePiece = true;
	}
};