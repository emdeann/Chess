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

const int BOARD_DIM_IN_WINDOW = 512;

bool inBoardRange(int x, int y) {
    return x >= 0 && x < BOARD_DIM_IN_WINDOW && y >= 0 && y < BOARD_DIM_IN_WINDOW;
}

int main() {
    int move = 0;
    sf::RenderWindow window(sf::VideoMode(512, 512), "Chess", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    Board board(BOARD_HEIGHT, BOARD_WIDTH);
    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (inBoardRange(event.mouseButton.x, event.mouseButton.y)) {
                    int boardPos = (event.mouseButton.x / CELL_WIDTH) + (event.mouseButton.y / CELL_WIDTH) * BOARD_WIDTH;
                    cout << boardPos << endl;
                    if (board.selectTile(boardPos, move)) {
                        move ^= 1;
                    }
                }
           
                

            }
        }
        window.draw(board);
        window.display();
    }
}

