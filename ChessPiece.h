#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace std;

const int MAX_RANGE = 8;

class ChessPiece {
protected:
	vector<bool> validDirections; // format: {canMoveHorizontal, canMoveVertical, canMoveDiagonal}
	vector<int> takeMoves; 
	int range, side, value; // 0: white 1: black
	bool activePiece, strictMotion, strictCapture, specialTakeMoves;
	vector<int> strictMoves; // int offsets from position instead of directions where needed (ie knight)
	string name;
	sf::Texture texture;
	sf::Sound moveSound;
	sf::SoundBuffer buffer;

public:
	ChessPiece() {
		side = 0;
		range = 0;
		value = 0;
		activePiece = false;
		strictMotion = false;
		strictCapture = false;
		specialTakeMoves = false;
	}

	ChessPiece(int rng, int val, vector<bool> perms, string n) {
		range = rng;
		value = val;
		validDirections = perms;
		name = n;
		side = 0;
		activePiece = true;

		if (!buffer.loadFromFile("move.mp3")) {
			throw invalid_argument("bad file");
		}
		moveSound.setBuffer(buffer);
	}



	void loadTexture() {
		if (activePiece) {
			ostringstream fileName;
			fileName << "Textures/" << ((side) ? "b" : "w") << "_" << name << ".png";
			cout << "Loading " << fileName.str() << endl;
			texture.loadFromFile(fileName.str());
		}
	}

	virtual void onMove(int moveDiff, int moveNum) {
		// called every time the piece is moved
		moveSound.play();
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

	string getName() const {
		return name;
	}

	virtual bool isPawn() const {
		return false;
	}

	virtual bool canBeEnPassanted(int moveNum) {
		return false;
	}

};