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
	}
};