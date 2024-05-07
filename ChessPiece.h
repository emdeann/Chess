#pragma once
#include <vector>
#include <string>
using namespace std;

const int MAX_RANGE = 8;
class ChessPiece {
protected:
	wstring ch;
	vector<bool> validDirections; // format: {canMoveHorizontal, canMoveVertical, canMoveDiagonal}
	vector<int> takeMoves; 
	int range, side; // 0: white 1: black
	bool activePiece, strictMotion, strictCapture, specialTakeMoves;
	vector<int> strictMoves; // int offsets from position instead of directions where needed (ie knight)

public:
	ChessPiece() {
		ch = L"-";
		side = 0;
		range = 0;
		activePiece = false;
		strictMotion = false;
		strictCapture = false;
		specialTakeMoves = false;
	}
	wstring getChar() {
		return ch;
	}

	virtual void switchSide() {
		side = ~side;
	}

	int getSide() {
		return side;
	}

	void onCapture() {
		return;
	}

	int getRange() {
		return range;
	}

	vector<bool> getValidDirections() {
		return validDirections;
	}

	bool isActive() {
		return activePiece;
	}

	vector<int> getStrictMoves() {
		return strictMoves;
	}

	bool useStrictMotion() {
		return strictMotion;
	}

	bool canTakeStrictly() {
		return strictCapture;
	}

	bool hasSpecialTakeMoves() {
		return specialTakeMoves;
	}

	vector<int> getTakeMoves() {
		return takeMoves;
	}
};