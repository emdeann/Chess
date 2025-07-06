#pragma once

#include "../constants/Constants.h"
#include "../constants/Enums.h"
#include "../board/Cell.h"
#include "../board/BoardManager.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


#include <sstream>
#include <vector>
#include <format>

class UIManager {
private:
	sf::RenderWindow window;
	WindowState windowState;

	std::ostringstream titleStr;
	sf::Font textFont;
	sf::Text titleText, buttonText;
    vector<sf::Text> scoreText;

	sf::RectangleShape replayButton;

    vector<Cell> promotionCells;
    vector<ChessPiece> standardPromotionPieces;

    void textSetup(sf::Text& txt, string s, sf::RenderWindow& window) {
        txt.setCharacterSize(TITLE_CHARSIZE);
        txt.setString(s);
        sf::FloatRect textRect = txt.getLocalBounds();
        txt.setOrigin({ textRect.position.x + textRect.size.x / 2.0f,
                    textRect.position.y + textRect.size.y / 2.0f });
        txt.setPosition({ window.getSize().x / 2.f, window.getSize().y / 4.f });
    }

	void setupButton(sf::RectangleShape& rect, sf::Vector2f size, sf::Vector2f pos, sf::Color fillColor, int outline, sf::Text& buttonText) {
		rect.setSize(size);
		rect.setPosition(pos);
		rect.setFillColor(fillColor);
		rect.setOutlineThickness(outline);
		buttonText.setOrigin(buttonText.getGlobalBounds().size / 2.f + buttonText.getLocalBounds().position);
		buttonText.setPosition(rect.getPosition() + (rect.getSize() / 2.f));
	}

    void displayTitleText(const GameState& gameState, const PieceSide& winnerSide = PieceSide::NONE) {
        ostringstream titleStr;
        string text;
        window.clear(sf::Color::Black);
        switch (gameState) {
        case GameState::NONE:
        case GameState::NO_TURN:
        case GameState::CHECK:
            text = "2-P Chess!";
            break;
        case GameState::CHECKMATE:
            titleStr << "Checkmate" << endl << ((winnerSide == PieceSide::BLACK) ? "Black" : "White") << " Wins!";
            text = titleStr.str();
            break;
        case GameState::STALEMATE:
            text = "Stalemate :(";
            break;
        }
        textSetup(titleText, text, window);
        window.draw(titleText);
    }

    bool rectContainsMouse(const sf::FloatRect& rect, const sf::Event::MouseButtonPressed& event) {
        return rect.contains(static_cast<sf::Vector2f>(event.position));
    }

public:
	UIManager() : window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Chess", sf::Style::Titlebar | sf::Style::Close), titleText(textFont), buttonText(textFont), promotionCells(4) {
		windowState = WindowState::START;
		textFont.openFromFile(FONT_PATH + "zig.ttf");
		window.setVerticalSyncEnabled(true);
		buttonText.setCharacterSize(BUTTON_CHARSIZE);
		buttonText.setString("Start Game");
		setupButton(replayButton, sf::Vector2f(window.getSize().x * 0.75f, BUTTON_SIZE),
			sf::Vector2f(window.getSize().x / 8.f, window.getSize().y / 2.f), sf::Color::Black, BUTTON_OUTLINE_WIDTH, buttonText);

        standardPromotionPieces = ChessPieceFactory::createStandardPromotionPieces();

        setPlaceHolderPieces();
        for (int i = 0; i < promotionCells.size(); i++) {
            Cell& cur = promotionCells.at(i);
            cur.setSize(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
            cur.setDefaultColor(sf::Color::Cyan);
        }

        for (int i = 0; i < 2; i++) {
            sf::Text t(textFont);
            t.setCharacterSize(24);
            t.setString("Score: 0");
            sf::FloatRect textRect = t.getLocalBounds();
            t.setOrigin({ textRect.position.x + textRect.size.x / 2.0f,
                textRect.position.y + textRect.size.y / 2.0f });
            t.setPosition(sf::Vector2f(BOARD_DIM_IN_WINDOW / 4.f, Y_OFFSET / 2.f + (BOARD_DIM_IN_WINDOW + Y_OFFSET) * i));
            scoreText.push_back(t);
        }
	}

    void setPlaceHolderPieces(PieceSide promoSide = PieceSide::NONE) {
        vector<ChessPiece> promotionPieces = standardPromotionPieces;
        for (int i = 0; i < promotionCells.size(); i++) {
            Cell& cur = promotionCells.at(i);
            if (promoSide == PieceSide::BLACK) {
                promotionPieces.at(i).switchSide();
            }
            cur.setChessPiece(promotionPieces.at(i));
            cur.setPos(sf::Vector2f(WINDOW_WIDTH / 2 + CELL_WIDTH * (i - 2), CELL_WIDTH + (BOARD_DIM_IN_WINDOW + CELL_WIDTH) * (promoSide == PieceSide::WHITE)));
        }
    }

    int getPromotionSelection(const sf::Event::MouseButtonPressed& event) {
        int selectedPos = NONE_SELECTED;
        for (int i = 0; i < promotionCells.size() && selectedPos == NONE_SELECTED; i++) {
            if (rectContainsMouse(promotionCells.at(i).getRect().getGlobalBounds(), event)) {
                selectedPos = i;
            }
        }
        return selectedPos;
    }

    vector<Cell>& getPromotionCells() {
        return promotionCells;
    }

	bool windowIsOpen() {
		return window.isOpen();
	}

    void closeWindow() {
        window.close();
    }

    void updateScoreText(int side, int score) {
        scoreText.at(side).setString(std::format("Score: {}", score));
    }

    sf::RenderWindow& getWindow() {
        return window;
    }
    
    WindowState getWindowState() const {
        return windowState;
    }

	void update(const BoardManager& board, const GameState& gameState, const PieceSide& winnerSide = PieceSide::NONE) {
        switch (windowState) {
        case WindowState::START:
            displayTitleText(gameState);
            window.draw(replayButton);
            window.draw(buttonText);
            break;
        case WindowState::GAME:
            switch (gameState) {
            case GameState::CHECK:
            case GameState::NONE:
                window.clear((gameState == GameState::CHECK) ? sf::Color::Red : sf::Color::Black);
                window.draw(board);
                if (board.isDoPromotion()) {
                    for (Cell& c : promotionCells) {
                        window.draw(c);
                    }
                }
                break;
            case GameState::CHECKMATE:
            case GameState::STALEMATE:
                buttonText.setString("Play Again?");
                windowState = WindowState::END;
                break;
            }
            break;
        case WindowState::END:
            displayTitleText(gameState, winnerSide);
            window.draw(replayButton);
            window.draw(buttonText);
        }

        for (sf::Text& score : scoreText) {
            window.draw(score);
        }

        window.display();
	}

    bool startClicked(const sf::Event::MouseButtonPressed& clickEvent) {
        if (rectContainsMouse(replayButton.getGlobalBounds(), clickEvent)) {
            windowState = WindowState::GAME;
            return true;
        }
        return false;
    }
};