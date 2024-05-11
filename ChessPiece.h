#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
using namespace std;

const int MAX_RANGE = 8;

class ChessPiece {
protected:
	wstring ch;
	vector<bool> validDirections; // format: {canMoveHorizontal, canMoveVertical, canMoveDiagonal}
	vector<int> takeMoves; 
	int range, side, value; // 0: white 1: black
	bool activePiece, strictMotion, strictCapture, specialTakeMoves;
	vector<int> strictMoves; // int offsets from position instead of directions where needed (ie knight)
	string name;
	sf::Texture texture;

public:
	ChessPiece() {
		ch = L"-";
		side = 0;
		range = 0;
		value = 0;
		activePiece = false;
		strictMotion = false;
		strictCapture = false;
		specialTakeMoves = false;
	}
	wstring getChar() {
		return ch;
	}

	void loadTexture() {
		if (activePiece) {
			ostringstream fileName;
			fileName << "Textures/" << ((side) ? "b" : "w") << "_" << name << ".png";
			cout << "Loading " << fileName.str() << endl;
			texture.loadFromFile(fileName.str());
		}
	}

	virtual void onMove() {
		// called every time the piece is moved
	}

	virtual void switchSide() {
		side ^= 1;
		loadTexture();
	}

	int getSide() const {
		return side;
	}

	void onCapture() {
		return;
	}

	int getRange() const {
		return range;
	}

	vector<bool> getValidDirections() const {
		return validDirections;
	}

	bool isActive() const {
		return activePiece;
	}

	bool isOnSide(int s) const {
		return activePiece && side == s;
	}

	vector<int> getStrictMoves() const {
		return strictMoves;
	}

	bool useStrictMotion() const {
		return strictMotion;
	}

	bool canTakeStrictly() const {
		return strictCapture;
	}

	bool hasSpecialTakeMoves() const {
		return specialTakeMoves;
	}

	virtual bool isKing() {
		return false;
	}

	virtual bool canCastle() {
		return false;
	}

	vector<int> getTakeMoves() const {
		return takeMoves;
	}

	sf::Texture getTexture() const {
		return texture;
	}

	int getValue() const {
		return value;
	}

	string getName() {
		return name;
	}

};