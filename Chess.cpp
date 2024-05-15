#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cctype>
#include <thread>
#include <chrono>
#define NOMINMAX
#include "windows.h"
#include "Board.h"
#include "fcntl.h"
#include "io.h"
using namespace std;

const int BOARD_HEIGHT = 8;
const int BOARD_WIDTH = 8;
const int WINDOW_WIDTH = 512;
const int WINDOW_HEIGHT = 768;
const float BUTTON_SIZE = 128;
const int BUTTON_OUTLINE_WIDTH = 5;
const int BUTTON_CHARSIZE = 32;
const int TITLE_CHARSIZE = 48;
enum WindowState {START, GAME, END};

bool inBoardRange(int x, int y) {
    return x >= 0 && x < BOARD_DIM_IN_WINDOW && y >= 0 && y < BOARD_DIM_IN_WINDOW;
}

void textSetup(sf::Text& txt, string s, sf::RenderWindow& window) {
    txt.setCharacterSize(TITLE_CHARSIZE);
    txt.setString(s);
    sf::FloatRect textRect = txt.getLocalBounds();
    txt.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    txt.setPosition(window.getSize().x / 2.f, window.getSize().y / 4.f);
}

void setPlaceHolderPieces(vector<Cell>& v, int promoSide = 0) {
    vector<ChessPiece*> promotionPieces = { new Rook, new Bishop, new Knight(BOARD_WIDTH), new Queen };
    for (int i = 0; i < promotionPieces.size(); i++) {
        Cell& cur = v.at(i);
        if (promoSide) {
            promotionPieces.at(i)->switchSide();
        }
        cur.setChessPiece(promotionPieces.at(i));
        cur.setPos(sf::Vector2f(WINDOW_WIDTH / 2 + CELL_WIDTH * (i - 2), CELL_WIDTH + (BOARD_DIM_IN_WINDOW + CELL_WIDTH) * !promoSide));
    }
}

void runGame(sf::Event& event, GameState& gameState, int& winnerSide, int& move, sf::RenderWindow& window, 
    Board& board, ostringstream& titleStr, sf::Text& titleText, WindowState& windowState, sf::Text& buttonText, 
    vector<Cell>& promoCells, bool& holderPiecesSet) {
    bool promote = board.isDoPromotion();
    if (promote && !holderPiecesSet) {
        setPlaceHolderPieces(promoCells, board.getPromotionSide());
        holderPiecesSet = true;
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        int yAdj = event.mouseButton.y - Y_OFFSET;
        int selectedPos = NONE_SELECTED;
        if (promote) {
            for (int i = 0; i < promoCells.size() && selectedPos == NONE_SELECTED; i++) {
                if (promoCells.at(i).getRect().getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    selectedPos = i;
                }
            }
            if (selectedPos != NONE_SELECTED) {
                board.setPromotedPiece(promoCells.at(selectedPos));
                holderPiecesSet = false;
            }
        }
        else if (inBoardRange(event.mouseButton.x, yAdj)) {
            int boardPos = (event.mouseButton.x / CELL_WIDTH) + (yAdj / CELL_WIDTH) * BOARD_WIDTH;
            GameState curState = board.selectTile(boardPos, move);
            if (curState != NO_TURN) {
                move += 1;
                gameState = curState;
                if (gameState == CHECKMATE) {
                    winnerSide = (move - 1) % 2;
                }
            }
        }
    }
    switch (gameState) {
    case CHECK:
    case NONE:
        window.clear((gameState == CHECK) ? sf::Color::Red : sf::Color::Black);
        window.draw(board);
        if (promote) {
            for (Cell& c : promoCells) {
                window.draw(c);
            }
        }
        break;
    case CHECKMATE:
    case STALEMATE:
        buttonText.setString("Play Again?");
        windowState = END;
        break;
    }
}

void displayTitleText(sf::RenderWindow& window, GameState& gameState, sf::Text& titleText, int winnerSide = -1) {
    ostringstream titleStr;
    string text;
    window.clear(sf::Color::Black);
    switch (gameState) {
    case NONE:
    case NO_TURN:
    case CHECK:
        text = "2-P Chess!";
        break;
    case CHECKMATE:
        titleStr << "Checkmate" << endl << ((winnerSide) ? "Black" : "White") << " Wins!";
        text = titleStr.str();
        break;
    case STALEMATE:
        text = "Stalemate :(";
        break;
    }
    textSetup(titleText, text, window);
    window.draw(titleText);
}

void resetGame(Board& board, int& move, int& winnerSide, GameState& gameState, WindowState& windowState) {
    board = Board(BOARD_WIDTH, BOARD_HEIGHT);
    move = 0;
    winnerSide = -1;
    gameState = NONE;
    windowState = GAME;
}

void setupButton(sf::RectangleShape& rect, sf::Vector2f size, sf::Vector2f pos, sf::Color fillColor, int outline, sf::Text& buttonText) {
    rect.setSize(size);
    rect.setPosition(pos);
    rect.setFillColor(fillColor);
    rect.setOutlineThickness(outline);
    buttonText.setOrigin(buttonText.getGlobalBounds().getSize() / 2.f + buttonText.getLocalBounds().getPosition());
    buttonText.setPosition(rect.getPosition() + (rect.getSize() / 2.f));
}

int main() {
    int move = 0, winnerSide = -1;
    vector<Cell> promotionCells(4);
    bool holderPiecesSet = false;
    WindowState windowState = START;
    GameState gameState = NONE;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chess", sf::Style::Titlebar | sf::Style::Close);
    sf::Font font;
    sf::Text titleText;
    sf::Text buttonText;
    sf::RectangleShape replayButton;
    ostringstream titleStr;
    font.loadFromFile("zig.ttf");
    titleText.setFont(font);
    buttonText.setFont(font);
    window.setVerticalSyncEnabled(true);
    Board board(BOARD_HEIGHT, BOARD_WIDTH);
    buttonText.setCharacterSize(BUTTON_CHARSIZE);
    buttonText.setString("Start Game");
    setupButton(replayButton, sf::Vector2f(window.getSize().x * 0.75f, BUTTON_SIZE),
        sf::Vector2f(window.getSize().x / 8.f, window.getSize().y / 2.f), sf::Color::Black, BUTTON_OUTLINE_WIDTH, buttonText);
    setPlaceHolderPieces(promotionCells);
    for (int i = 0; i < promotionCells.size(); i++) {
        Cell& cur = promotionCells.at(i);
        cur.setSize(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
        cur.setDefaultColor(sf::Color::Cyan);
    }

    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
            switch (windowState) {
            case START:
                displayTitleText(window, gameState, titleText, winnerSide);
                window.draw(replayButton);
                window.draw(buttonText);
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (replayButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        windowState = GAME;
                    }
                }
                break;
            case GAME:
                runGame(event, gameState, winnerSide, move, window, board, titleStr, titleText, 
                    windowState, buttonText, promotionCells, holderPiecesSet);
                break;
            case END:
                displayTitleText(window, gameState, titleText, winnerSide);
                window.draw(replayButton);
                window.draw(buttonText);
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (replayButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        board = Board(BOARD_WIDTH, BOARD_HEIGHT);
                        resetGame(board, move, winnerSide, gameState, windowState);
                    }
                }
            }
            window.display();
        }
    }

}

