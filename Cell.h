#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "ChessPiece.h"

using namespace std;

const wstring RESET = L"\033[0m";
const wstring BLACK = L"\033[30m";
const wstring RED = L"\033[31m";
const wstring GREEN = L"\033[32m";
const wstring YELLOW = L"\033[33m";
const wstring BLUE = L"\033[34m";
const wstring MAGENTA = L"\033[35m";

const int CELL_WIDTH = 64;

class Cell : public sf::Drawable {
private:
	bool isCursor, isSelected, isHighlighted;
	sf::Color highlightColor;
	sf::Color defaultColor;
	ChessPiece piece;
	sf::RectangleShape cellRect;
public:
	Cell() {
		isCursor = false;
		isSelected = false;
		isHighlighted = false;
		highlightColor = sf::Color::Green;
	}
	void toggleSelected() {
		isSelected = !isSelected;
	}

	void toggleCursor() {
		isCursor = !isCursor;
	}

	void setChessPiece(ChessPiece& p) {
		piece = p;
	}

	ChessPiece& getChessPiece() {
		return piece;
	}

	void movePiece(Cell& other) {
		other.piece = piece;
		piece = ChessPiece();
	}

	void toggleHighlight(sf::Color color = sf::Color::Green) {
		highlightColor = color;
		isHighlighted = !isHighlighted;
		cellRect.setFillColor((isHighlighted) ? highlightColor : defaultColor);
	}

	void setDefaultColor(sf::Color c) {
		defaultColor = c;
		cellRect.setFillColor(c);
	}

	void setSize(sf::Vector2f vec) {
		cellRect.setSize(vec);
	}

	void setPos(sf::Vector2f vec) {
		cellRect.setPosition(vec);
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(cellRect);
		if (piece.isActive()) {
			sf::Sprite pieceSprite;
			sf::Texture texture = piece.getTexture();
			pieceSprite.setTexture(texture);
			pieceSprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
			pieceSprite.setScale(sf::Vector2f(0.375f, 0.375f));
			sf::Vector2f rectPos = cellRect.getPosition();
			pieceSprite.setPosition(rectPos.x + CELL_WIDTH/2, rectPos.y + CELL_WIDTH/2);
			target.draw(pieceSprite);
		}
	}
};