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

const int CELL_WIDTH = 64;
const float DEFAULT_ITEM_SIZE = 0.375;

const int MAX_RANGE = 8;

const std::string ASSET_PATH = "assets/";
const std::string TEXTURE_PATH = ASSET_PATH + "/textures/";
const std::string AUDIO_PATH = ASSET_PATH + "/sounds/";
const std::string FONT_PATH = ASSET_PATH + "/fonts/";

enum class PieceType {
	EMPTY = 0,
	PAWN = 1,
	KNIGHT = 2,
	BISHOP = 3,
	ROOK = 4,
	QUEEN = 5,
	KING = 6
};

enum class PieceSide {
    NONE,
    WHITE,
    BLACK
};

enum class GameState { 
	NONE, 
	CHECK,
	CHECKMATE, 
	STALEMATE, 
	NO_TURN 
};


enum class WindowState { 
	START, 
	GAME, 
	END 
};