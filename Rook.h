#pragma once
#include "ChessPiece.h"
using namespace std;

class Rook : public ChessPiece {
public:
	Rook() {
		ch = L'\u2656';
	}
};