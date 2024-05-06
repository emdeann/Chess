#pragma once
#include <vector>

using namespace std;
class ChessPiece {
protected:
	wchar_t ch;
	vector<int> validMoves;
	vector<int> standardMoves; 
public:
	ChessPiece() {
		ch = L'-';
	}
	wchar_t getChar() {
		return ch;
	}

	void onCapture() {
		return;
	}
};