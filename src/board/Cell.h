#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "../pieces/ChessPiece.h"
#include "../pieces/ChessPieceBuilder.h"

using namespace std;

class Cell : public sf::Drawable {
private:
	bool isCursor, isSelected, isHighlighted;
	sf::Color highlightColor;
	sf::Color defaultColor;
	sf::Color selectedColor;
	ChessPiece piece;
	sf::RectangleShape cellRect;
public:
	Cell() {
		piece = ChessPieceFactory::createPiece(PieceType::EMPTY);
		isCursor = false;
		isSelected = false;
		isHighlighted = false;
		highlightColor = sf::Color::Green;
		selectedColor = sf::Color::Yellow;
		cellRect.setOutlineColor(sf::Color::Black);
		cellRect.setOutlineThickness(1);
	}
	void toggleSelected() {
		isSelected = !isSelected;
		cellRect.setFillColor((isSelected) ? selectedColor : defaultColor);
	}

	void setChessPiece(ChessPiece& p) {
		piece = p;
	}

	ChessPiece& getChessPiece() {
		return piece;
	}

	void movePiece(Cell& other) {
		other.piece = piece;
		piece = ChessPieceFactory::createPiece(PieceType::EMPTY);
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

	sf::RectangleShape& getRect() {
		return cellRect;
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(cellRect);
		if (piece.isActive()) {
			sf::Sprite pieceSprite;
			sf::Texture texture = piece.getTexture();
			pieceSprite.setTexture(texture);
			pieceSprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
			pieceSprite.setScale(sf::Vector2f(DEFAULT_ITEM_SIZE, DEFAULT_ITEM_SIZE));
			sf::Vector2f rectPos = cellRect.getPosition();
			pieceSprite.setPosition(rectPos.x + CELL_WIDTH/2, rectPos.y + CELL_WIDTH/2);
			target.draw(pieceSprite);
		}
	}
};