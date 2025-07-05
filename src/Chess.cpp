#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cctype>
#include <thread>
#include <chrono>
#define NOMINMAX
#include "windows.h"
#include "GameManager.h"
#include "pieces/ChessPieceBuilder.h"
#include "constants/Constants.h"
#include "constants/Enums.h"
#include "fcntl.h"
#include "io.h"
using namespace std;

const vector<ChessPiece> standardPromotionPieces = ChessPieceFactory::createStandardPromotionPieces();

void loadSound(sf::SoundBuffer& buffer, string fileName) {
    buffer.loadFromFile(AUDIO_PATH + fileName);
}

bool leftMousePressed(std::optional<sf::Event>& event) {
    return event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left;
}

bool rectContainsMouse(sf::FloatRect& rect, std::optional<sf::Event>& event) {
    return rect.contains(static_cast<sf::Vector2f>(event->getIf<sf::Event::MouseButtonPressed>()->position));
}

bool inBoardRange(int x, int y) {
    return x >= 0 && x < BOARD_DIM_IN_WINDOW && y >= 0 && y < BOARD_DIM_IN_WINDOW;
}

void textSetup(sf::Text& txt, string s, sf::RenderWindow& window) {
    txt.setCharacterSize(TITLE_CHARSIZE);
    txt.setString(s);
    sf::FloatRect textRect = txt.getLocalBounds();
    txt.setOrigin({ textRect.position.x + textRect.size.x / 2.0f,
                textRect.position.y + textRect.size.y / 2.0f });
    txt.setPosition({ window.getSize().x / 2.f, window.getSize().y / 4.f });
}

void setPlaceHolderPieces(vector<Cell>& v, PieceSide promoSide = PieceSide::NONE) {
    vector<ChessPiece> promotionPieces = standardPromotionPieces;
    for (int i = 0; i < promotionPieces.size(); i++) {
        Cell& cur = v.at(i);
        if (promoSide == PieceSide::BLACK) {
            promotionPieces.at(i).switchSide();
        }
        cur.setChessPiece(promotionPieces.at(i));
        cur.setPos(sf::Vector2f(WINDOW_WIDTH / 2 + CELL_WIDTH * (i - 2), CELL_WIDTH + (BOARD_DIM_IN_WINDOW + CELL_WIDTH) * (promoSide == PieceSide::WHITE)));
    }
}

void runGame(std::optional<sf::Event>& event, GameState& gameState, int& winnerSide, int& move, sf::RenderWindow& window, 
    GameManager& board, ostringstream& titleStr, sf::Text& titleText, WindowState& windowState, sf::Text& buttonText, 
    vector<Cell>& promoCells, bool& holderPiecesSet) {
    bool promote = board.isDoPromotion();
    if (promote && !holderPiecesSet) {
        setPlaceHolderPieces(promoCells, board.getPromotionSide());
        holderPiecesSet = true;
    }
    if (leftMousePressed(event)) {
        sf::Vector2i mousePos = event->getIf <sf::Event::MouseButtonPressed>()->position;
        int yAdj = mousePos.y - Y_OFFSET;
        int selectedPos = NONE_SELECTED;
        if (promote) {
            for (int i = 0; i < promoCells.size() && selectedPos == NONE_SELECTED; i++) {
                if (rectContainsMouse(promoCells.at(i).getRect().getGlobalBounds(), event)) {
                    selectedPos = i;
                }
            }
            if (selectedPos != NONE_SELECTED) {
                board.setPromotedPiece(promoCells.at(selectedPos));
                holderPiecesSet = false;
            }
        }
        else if (inBoardRange(mousePos.x, yAdj)) {
            int boardPos = (mousePos.x / CELL_WIDTH) + (yAdj / CELL_WIDTH) * BOARD_WIDTH;
            GameState curState = board.selectTile(boardPos, move);
            if (curState != GameState::NO_TURN) {
                move += 1;
                gameState = curState;
                if (gameState == GameState::CHECKMATE) {
                    winnerSide = (move - 1) % 2;
                }
            }
        }
    }
    switch (gameState) {
    case GameState::CHECK:
    case GameState::NONE:
        window.clear((gameState == GameState::CHECK) ? sf::Color::Red : sf::Color::Black);
        window.draw(board);
        if (promote) {
            for (Cell& c : promoCells) {
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
}

void displayTitleText(sf::RenderWindow& window, GameState& gameState, sf::Text& titleText, int winnerSide = -1) {
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
        titleStr << "Checkmate" << endl << ((winnerSide) ? "Black" : "White") << " Wins!";
        text = titleStr.str();
        break;
    case GameState::STALEMATE:
        text = "Stalemate :(";
        break;
    }
    textSetup(titleText, text, window);
    window.draw(titleText);
}

void resetGame(GameManager& board, int& move, int& winnerSide, GameState& gameState, WindowState& windowState, sf::Sound& sound, sf::Font& font) {
    board = GameManager(BOARD_WIDTH, BOARD_HEIGHT, sound, font);
    move = 0;
    winnerSide = -1;
    gameState = GameState::NONE;
    windowState = WindowState::GAME;
}

void setupButton(sf::RectangleShape& rect, sf::Vector2f size, sf::Vector2f pos, sf::Color fillColor, int outline, sf::Text& buttonText) {
    rect.setSize(size);
    rect.setPosition(pos);
    rect.setFillColor(fillColor);
    rect.setOutlineThickness(outline);
    buttonText.setOrigin(buttonText.getGlobalBounds().size / 2.f + buttonText.getLocalBounds().position);
    buttonText.setPosition(rect.getPosition() + (rect.getSize() / 2.f));
}

int main() {
    int move = 0, winnerSide = -1;
    vector<Cell> promotionCells(4);
    bool holderPiecesSet = false, winSoundPlayed = false;
    WindowState windowState = WindowState::START;
    sf::SoundBuffer moveSoundBuffer, winSoundBuffer, selectSoundBuffer;
    GameState gameState = GameState::NONE;
    sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Chess", sf::Style::Titlebar | sf::Style::Close);
    sf::Font font;
    sf::Text titleText(font);
    sf::Text buttonText(font);
    sf::RectangleShape replayButton;
    ostringstream titleStr;
    sf::Music music;
    music.openFromFile(AUDIO_PATH + "music.mp3");
    music.setLooping(true);
    music.setVolume(5);
    loadSound(moveSoundBuffer, "move.mp3");
    loadSound(selectSoundBuffer, "select.wav");
    loadSound(winSoundBuffer, "win.wav");
    sf::Sound moveSound(moveSoundBuffer);
    sf::Sound winSound(winSoundBuffer);
    sf::Sound selectSound(selectSoundBuffer);
    font.openFromFile(FONT_PATH + "zig.ttf");
    window.setVerticalSyncEnabled(true);
    GameManager board(BOARD_HEIGHT, BOARD_WIDTH, moveSound, font);
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
    while (std::optional event = window.pollEvent()) {

        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        switch (windowState) {
            case WindowState::START:
                displayTitleText(window, gameState, titleText, winnerSide);
                window.draw(replayButton);
                window.draw(buttonText);
                if (leftMousePressed(event)) {
                    if (rectContainsMouse(replayButton.getGlobalBounds(), event)) {
                        windowState = WindowState::GAME;
                        selectSound.play();
                        music.play();
                    }
                }
                break;
            case WindowState::GAME:
                runGame(event, gameState, winnerSide, move, window, board, titleStr, titleText, 
                    windowState, buttonText, promotionCells, holderPiecesSet);
                break;
            case WindowState::END:
                displayTitleText(window, gameState, titleText, winnerSide);
                if (!winSoundPlayed) {
                    winSound.play();
                    music.stop();
                    winSoundPlayed = true;
                }
                window.draw(replayButton);
                window.draw(buttonText);
                if (leftMousePressed(event)) {
                    if (rectContainsMouse(replayButton.getGlobalBounds(), event)) {
                        resetGame(board, move, winnerSide, gameState, windowState, moveSound, font);
                        selectSound.play();
                        music.play();
                        winSoundPlayed = false;
                    }
                }
                break;
        }
        window.display();
    }

}

