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
	bool activePiece = true;

public:
	ChessPiece() {
		ch = L"-";
		side = 0;
		range = 0;
		activePiece = false;
	}
	wstring getChar() {
		return ch;
	}

	void switchSide() {
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
};