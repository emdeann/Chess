#pragma once
#include "ChessPiece.h"
using namespace std;

class Bishop : public ChessPiece {
public:
	Bishop() : ChessPiece(MAX_RANGE, 3, { false, true, true }, "bishop") {
	}
};