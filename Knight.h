#pragma once
#include "ChessPiece.h"
using namespace std;

class Knight : public ChessPiece {
public:
	Knight(int boardWidth) {
		ch = L"\u2658";
		strictMotion = true;
		activePiece = true;
		strictCapture = true;
		strictMoves = {-2 * boardWidth - 1, -2* boardWidth + 1, -boardWidth - 2, -boardWidth + 2, boardWidth - 2, boardWidth + 2, 2 * boardWidth - 1, 2 * boardWidth + 1 };
	}
};