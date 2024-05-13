#pragma once
#include "ChessPiece.h"
using namespace std;

class Rook : public ChessPiece {
public:
	Rook() : ChessPiece(MAX_RANGE, 5, { true, true, false }, "rook") {
	}

};