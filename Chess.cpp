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

bool inBoardRange(int x, int y) {
    return x >= 0 && x < BOARD_DIM_IN_WINDOW && y >= 0 && y < BOARD_DIM_IN_WINDOW;
}

void textSetup(sf::Text txt, string s, sf::RenderWindow& window) {
    txt.setCharacterSize(48);
    sf::FloatRect textRect = txt.getLocalBounds();
    txt.setString(s);
    txt.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    txt.setPosition(window.getSize().x / 2.f, window.getSize().y / 4.f);
}

int main() {
    int move = 0, winnerSide;
    State gameState = NONE;
    sf::RenderWindow window(sf::VideoMode(512, 768), "Chess", sf::Style::Titlebar | sf::Style::Close);
    sf::Font font;
    sf::Text titleText;
    ostringstream titleStr;
    font.loadFromFile("zig.ttf");
    titleText.setFont(font);
    window.setVerticalSyncEnabled(true);
    Board board(BOARD_HEIGHT, BOARD_WIDTH);
    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int yAdj = event.mouseButton.y - Y_OFFSET;
                if (inBoardRange(event.mouseButton.x, yAdj)) {
                    int boardPos = (event.mouseButton.x / CELL_WIDTH) + (yAdj / CELL_WIDTH) * BOARD_WIDTH;
                    State curState = board.selectTile(boardPos, move);
                    if (curState != NO_TURN) {
                        move += 1;
                        gameState = curState;
                        if (gameState == CHECKMATE) {
                            winnerSide = (move - 1) % 2;
                        }
                    }
                }
            }
        }
        switch (gameState) {
        case NONE:
        case CHECK:
            window.draw(board);
            break;
        case CHECKMATE:
            window.clear();
            titleStr << "Checkmate" << endl << ((winnerSide) ? "Black" : "White") << " Wins!";
            textSetup(titleText, titleStr.str(), window);
            window.draw(titleText);
            break;
        case STALEMATE:
            window.clear();
            textSetup(titleText, "Stalemate :(", window);
            window.draw(titleText);
            break;
        }
        window.display();
    }

}

