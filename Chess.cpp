#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cctype>
#include <thread>
#include <chrono>
#include "windows.h"
#include "Board.h"
using namespace std;

const int BOARD_HEIGHT = 8;
const int BOARD_WIDTH = 8;

int main() {
	bool game_running = true;

	Board myBoard(BOARD_HEIGHT, BOARD_WIDTH); //width, height, %of bombs (max 30% of total
	system("cls");
	do {

		myBoard.draw(false);
		this_thread::sleep_for(80ms); //wait for key
		if (GetAsyncKeyState(VK_DOWN)) {
			myBoard.moveCursor(0, -1);
		}
		else if (GetAsyncKeyState(VK_UP)) {
			myBoard.moveCursor(0, 1);
		}
		else if (GetAsyncKeyState(VK_RIGHT)) {
			myBoard.moveCursor(1, 0);
		}
		else if (GetAsyncKeyState(VK_LEFT)) {
			myBoard.moveCursor(-1, 0);
		}
		else if (GetAsyncKeyState(VK_SPACE)) {
		}
		else if (GetAsyncKeyState(VK_ESCAPE)) { //....TODO : Add case for pressing letter M for marking bombs
			game_running = false;
		}

	} while (game_running);
	system("cls");
}