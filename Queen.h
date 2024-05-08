#pragma once
#include "ChessPiece.h"
using namespace std;

class Queen : public ChessPiece {
public:
	Queen() {
		ch = L"\u2655";
		name = "queen";
		activePiece = true;
		validDirections = { true, true, true};
		range = MAX_RANGE;
		value = 9;
	}
};