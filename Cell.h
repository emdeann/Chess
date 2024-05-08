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
	wstring highlightColor;
	ChessPiece piece;
	sf::RectangleShape cellRect;
public:
	Cell() {
		isCursor = false;
		isSelected = false;
		isHighlighted = false;
		highlightColor = GREEN;
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

	void toggleHighlight(wstring color = GREEN) {
		highlightColor = color;
		isHighlighted = !isHighlighted;
	}

	wstring getStr() {
		wstring ch;
		if (isCursor) {
			ch = L"x";
		}
		else {
			ch = piece.getChar();
		}

		if (isSelected) {
			ch = YELLOW + ch + RESET;
		}
		else if (isHighlighted) {
			ch = highlightColor + ch + RESET;
		}
		else if (piece.getSide()) {
			ch = BLUE + ch + RESET;
		}

		return ch;
	}

	void setColor(sf::Color c) {
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
			sf::Sprite pieceSprite = piece.getSprite();
			sf::Vector2f rectPos = cellRect.getPosition();
			pieceSprite.setPosition(rectPos.x + CELL_WIDTH/2, rectPos.y + CELL_WIDTH/2);
			target.draw(pieceSprite);
		}
	}
};