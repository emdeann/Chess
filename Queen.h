#pragma once
#include "ChessPiece.h"
using namespace std;

class Queen : public ChessPiece {
public:
	Queen() : ChessPiece(MAX_RANGE, 9, { true, true, true }, "queen") {

	}
};