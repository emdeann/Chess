#pragma once
#include "ChessPiece.h"
using namespace std;

class Knight : public ChessPiece {
public:
	Knight() {
		ch = L'\u2658';
	}
};