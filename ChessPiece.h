#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Constants.h"
using namespace std;

const int MAX_RANGE = 8;

enum PieceType {
	EMPTY = 0,
	PAWN = 1,
	KNIGHT = 2,
	BISHOP = 3,
	ROOK = 4,
	QUEEN = 5,
	KING = 6
};

class ChessPiece {
private:
	PieceType pieceType;
	vector<bool> validDirections; // format: {canMoveHorizontal, canMoveVertical, canMoveDiagonal}
	vector<int> takeMoves; 
	int range, side, value; // 0: white 1: black
	bool activePiece, strictMotion, strictCapture, specialTakeMoves;
	vector<int> strictMoves; // int offsets from position instead of directions where needed (ie knight)
	string name;
	sf::Texture texture;
	sf::SoundBuffer buffer;
	sf::Sound moveSound;

	// Pawn specific attributes
	int moves, lastMoveDiff, doubleMoveTurn;

	// King specific attributes
	bool kingCanCastle;

	void initializePiece() {
		switch (pieceType) {
			case PAWN:
				range = 1;
				value = 1;
				validDirections = { false, false, false };
				name = "pawn";
				strictMotion = true;
				specialTakeMoves = true;
				lastMoveDiff = 0;
				moves = 0;
				doubleMoveTurn = -1;
				takeMoves = { BOARD_WIDTH - 1, BOARD_WIDTH + 1 };
				strictMoves = { BOARD_WIDTH, 2 * BOARD_WIDTH };
				break;

			case KNIGHT:
				range = MAX_RANGE;
				value = 3;
				validDirections = { false, false, false };
				name = "knight";
				strictMotion = true;
				strictCapture = true;
				strictMoves = { -2 * BOARD_WIDTH - 1, -2 * BOARD_WIDTH + 1, -BOARD_WIDTH - 2,
					-BOARD_WIDTH + 2, BOARD_WIDTH - 2, BOARD_WIDTH + 2, 2 * BOARD_WIDTH - 1, 2 * BOARD_WIDTH + 1 };
				break;

			case BISHOP:
				range = MAX_RANGE;
				value = 3;
				validDirections = { false, false, true };
				name = "bishop";
				break;

			case ROOK:
				range = MAX_RANGE;
				value = 5;
				validDirections = { true, true, false };
				name = "rook";
				break;

			case QUEEN:
				range = MAX_RANGE;
				value = 9;
				validDirections = { true, true, true };
				name = "queen";
				break;

			case KING:
				range = 1;
				value = 0;
				validDirections = { true, true, true };
				name = "king";
				kingCanCastle = true;
				break;

			case EMPTY:
			default:
				value = 0;
				activePiece = false;
				name = "empty";
				break;
		}
	}

	void flipVector(vector<int>& v) {
		for (int& i : v) {
			i *= -1;
		}
	}

public:
	ChessPiece(PieceType type) : pieceType(type) {
		pieceType = type;
		side = 0;
		activePiece = true;
		strictMotion = false;
		strictCapture = false;
		specialTakeMoves = false;
		moves = 0;
		lastMoveDiff = 0;
		doubleMoveTurn = -1;
		kingCanCastle = false;

		initializePiece();
		loadTexture();
	}

	ChessPiece() : ChessPiece(EMPTY) {}



	void loadTexture() {
		if (activePiece) {
			ostringstream fileName;
			fileName << "Textures/" << ((side) ? "b" : "w") << "_" << name << ".png";
			texture.loadFromFile(fileName.str());
		}
	}

	void setSound(sf::SoundBuffer& buff) {
		buffer = sf::SoundBuffer(buff);
		moveSound.setBuffer(buffer);
	}

	void onMove(int moveDiff, int moveNum) {
		// called every time the piece is moved
		moveSound.play();
		kingCanCastle = false;
		if (!moves++ && isOfType(PAWN)) {
			if (moveDiff == 2 * BOARD_WIDTH) {
				doubleMoveTurn = moveNum;
			}
			strictMoves.erase(strictMoves.begin() + 1);
		}
		lastMoveDiff = moveDiff;
	}

	void switchSide() {
		side ^= 1;
		loadTexture();
		if (isOfType(PAWN)) {
			flipVector(takeMoves);
			flipVector(strictMoves);
		}
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

	bool canCastle() {
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

	bool isOfType(PieceType type) const {
		return pieceType == type;
	}

	bool canBeEnPassanted(int moveNum) const {
		// First move, moved two spaces, turn after move
		return isOfType(PAWN) && moves == 1 && lastMoveDiff == 2 * BOARD_WIDTH && moveNum == doubleMoveTurn + 1;
	}

	bool canCastle() const {
		return kingCanCastle;
	}

};