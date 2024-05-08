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
	int range, side; // 0: white 1: black
	bool activePiece, strictMotion, strictCapture, specialTakeMoves;
	vector<int> strictMoves; // int offsets from position instead of directions where needed (ie knight)
	string name;
	sf::Texture texture;
	sf::Sprite sprite;

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

	void loadTexture() {
		if (activePiece) {
			ostringstream fileName;
			fileName << "Textures/" << ((side) ? "b" : "w") << "_" << name << ".png";
			cout << "Loading " << fileName.str() << endl;
			texture.loadFromFile(fileName.str());
			sprite.setTexture(texture);
			sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
			sprite.setScale(sf::Vector2f(0.375f, 0.375f));
		}
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

	vector<int> getTakeMoves() const {
		return takeMoves;
	}

	sf::Sprite getSprite() const {
		return sprite;
	}
};