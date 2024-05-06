#pragma once
#include <vector>
#include <string>
using namespace std;
class ChessPiece {
protected:
	wstring ch;
	vector<int> validMoves;
	vector<int> standardMoves; 
public:
	ChessPiece() {
		ch = L"-";
	}
	wstring getChar() {
		return ch;
	}

	void onCapture() {
		return;
	}
};