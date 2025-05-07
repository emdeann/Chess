#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Constants.h"
using namespace std;

const int MAX_RANGE = 8;

enum class PieceType {
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
	int range, value;
	PieceSide side;
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

	void flipVector(vector<int>& v) {
		for (int& i : v) {
			i *= -1;
		}
	}

	friend class ChessPieceBuilder;
	friend class ChessPieceRegistry;

public:
	ChessPiece(PieceType type) {
		pieceType = type;
		side = PieceSide::WHITE;
		activePiece = true;
		strictMotion = false;
		strictCapture = false;
		specialTakeMoves = false;
		moves = 0;
		lastMoveDiff = 0;
		doubleMoveTurn = -1;
		kingCanCastle = false;
		range = 0;
		value = 0;
		name = "";
	}

	ChessPiece() : ChessPiece(PieceType::EMPTY) {}

	void loadTexture() {
		if (activePiece) {
			ostringstream fileName;
			fileName << "Textures/" << ((side == PieceSide::BLACK) ? "b" : "w") << "_" << name << ".png";
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
		if (!moves++ && isOfType(PieceType::PAWN)) {
			if (moveDiff == 2 * BOARD_WIDTH) {
				doubleMoveTurn = moveNum;
			}
			strictMoves.erase(strictMoves.begin() + 1);
		}
		lastMoveDiff = moveDiff;
	}

	void switchSide() {
		side = (side == PieceSide::WHITE) ? PieceSide::BLACK : PieceSide::WHITE;
		loadTexture();
		if (isOfType(PieceType::PAWN)) {
			flipVector(takeMoves);
			flipVector(strictMoves);
		}
	}

	PieceSide getSide() const {
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

	bool isOnSide(PieceSide s) const {
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
		return isOfType(PieceType::PAWN) && moves == 1 && lastMoveDiff == 2 * BOARD_WIDTH && moveNum == doubleMoveTurn + 1;
	}

	bool canCastle() const {
		return kingCanCastle;
	}
};