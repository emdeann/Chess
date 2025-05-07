#pragma once

const int BOARD_HEIGHT = 8;
const int BOARD_WIDTH = 8;
const int WINDOW_WIDTH = 512;
const int WINDOW_HEIGHT = 768;
const float BUTTON_SIZE = 128;
const int BUTTON_OUTLINE_WIDTH = 5;
const int BUTTON_CHARSIZE = 32;
const int TITLE_CHARSIZE = 48;

const int NONE_SELECTED = -10;
const int Y_OFFSET = 128;
const int BOARD_DIM_IN_WINDOW = 512;

enum class PieceSide {
    NONE,
    WHITE,
    BLACK
};