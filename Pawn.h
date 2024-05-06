#pragma once
#include "ChessPiece.h"
using namespace std;

class Pawn : public ChessPiece {
public:
	Pawn() {
		ch = L'\u2659';
	}
};