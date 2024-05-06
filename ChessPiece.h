#pragma once
#include <vector>

using namespace std;
class ChessPiece {
private:
	char ch;
	vector<int> validMoves;
	vector<int> standardMoves; 
public:
	ChessPiece() {

	}
	char getChar() {
		return ch;
	}

	virtual void onCapture();
};