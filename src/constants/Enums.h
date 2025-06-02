#pragma once
#include <SFML/Graphics.hpp>

// Move existing enums from Constants.h
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

enum class MoveDirection {
    HORIZONTAL,
    VERTICAL,
    DIAGONAL_LEFT,
    DIAGONAL_RIGHT
};