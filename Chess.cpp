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

int main() {
    sf::RenderWindow window(sf::VideoMode(768, 512), "Chess", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    Board board(BOARD_HEIGHT, BOARD_WIDTH);
    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.draw(board);
        window.display();
    }
}