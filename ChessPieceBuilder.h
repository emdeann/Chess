#pragma once
#include "ChessPiece.h"
#include <unordered_map>
#include <memory>

class ChessPieceRegistry;

// Fluent interface builder for ChessPiece
class ChessPieceBuilder {
private:
    ChessPiece piece;

public:
    ChessPieceBuilder() : piece(EMPTY) {}
    explicit ChessPieceBuilder(PieceType type) : piece(type) {}

    ChessPieceBuilder& type(PieceType type) {
        piece.pieceType = type;
        return *this;
    }

    ChessPieceBuilder& range(int range) {
        piece.range = range;
        return *this;
    }

    ChessPieceBuilder& value(int value) {
        piece.value = value;
        return *this;
    }

    ChessPieceBuilder& name(const std::string& name) {
        piece.name = name;
        return *this;
    }

    ChessPieceBuilder& side(int side) {
        piece.side = side;
        if (side == 1) {
            piece.switchSide();
        }
        return *this;
    }

    ChessPieceBuilder& directions(bool horizontal, bool vertical, bool diagonal) {
        piece.validDirections = { horizontal, vertical, diagonal };
        return *this;
    }

    ChessPieceBuilder& strictMotion(bool strict) {
        piece.strictMotion = strict;
        return *this;
    }

    ChessPieceBuilder& strictCapture(bool strict) {
        piece.strictCapture = strict;
        return *this;
    }

    ChessPieceBuilder& specialTakeMoves(bool special) {
        piece.specialTakeMoves = special;
        return *this;
    }

    ChessPieceBuilder& strictMoves(const std::vector<int>& moves) {
        piece.strictMoves = moves;
        return *this;
    }

    ChessPieceBuilder& takeMoves(const std::vector<int>& moves) {
        piece.takeMoves = moves;
        return *this;
    }

    ChessPieceBuilder& canCastle(bool canCastle) {
        piece.kingCanCastle = canCastle;
        return *this;
    }

    ChessPieceBuilder& active(bool active) {
        piece.activePiece = active;
        return *this;
    }

    ChessPiece build() {
        piece.loadTexture();
        return piece;
    }
};

// A registry to hold predefined piece configurations
class ChessPieceRegistry {
private:
    static std::unordered_map<PieceType, ChessPiece> pieceTemplates;

public:
    static void initialize() {
        if (!pieceTemplates.empty()) return;

        pieceTemplates[PAWN] = ChessPieceBuilder()
            .type(PAWN)
            .range(1)
            .value(1)
            .name("pawn")
            .directions(false, false, false)
            .strictMotion(true)
            .specialTakeMoves(true)
            .takeMoves({ BOARD_WIDTH - 1, BOARD_WIDTH + 1 })
            .strictMoves({ BOARD_WIDTH, 2 * BOARD_WIDTH })
            .build();

        pieceTemplates[KNIGHT] = ChessPieceBuilder()
            .type(KNIGHT)
            .range(MAX_RANGE)
            .value(3)
            .name("knight")
            .directions(false, false, false)
            .strictMotion(true)
            .strictCapture(true)
            .strictMoves({ -2 * BOARD_WIDTH - 1, -2 * BOARD_WIDTH + 1, -BOARD_WIDTH - 2,
                         -BOARD_WIDTH + 2, BOARD_WIDTH - 2, BOARD_WIDTH + 2,
                         2 * BOARD_WIDTH - 1, 2 * BOARD_WIDTH + 1 })
            .build();

        pieceTemplates[BISHOP] = ChessPieceBuilder()
            .type(BISHOP)
            .range(MAX_RANGE)
            .value(3)
            .name("bishop")
            .directions(false, false, true)
            .build();

        pieceTemplates[ROOK] = ChessPieceBuilder()
            .type(ROOK)
            .range(MAX_RANGE)
            .value(5)
            .name("rook")
            .directions(true, true, false)
            .build();

        pieceTemplates[QUEEN] = ChessPieceBuilder()
            .type(QUEEN)
            .range(MAX_RANGE)
            .value(9)
            .name("queen")
            .directions(true, true, true)
            .build();

        pieceTemplates[KING] = ChessPieceBuilder()
            .type(KING)
            .range(1)
            .value(0)
            .name("king")
            .directions(true, true, true)
            .canCastle(true)
            .build();

        pieceTemplates[EMPTY] = ChessPieceBuilder()
            .type(EMPTY)
            .value(0)
            .name("empty")
            .active(false)
            .build();
    }

    static const ChessPiece& getPieceTemplate(PieceType type) {
        if (pieceTemplates.empty()) {
            initialize();
        }
        return pieceTemplates[type];
    }

    static ChessPiece fromTemplate(PieceType type) {
        return ChessPiece(ChessPieceRegistry::getPieceTemplate(type));
    }
};

std::unordered_map<PieceType, ChessPiece> ChessPieceRegistry::pieceTemplates;


// Factory to create instances of predefined pieces
class ChessPieceFactory {
public:
    static ChessPiece createPiece(PieceType type) {
        return ChessPieceRegistry::fromTemplate(type);
    }

    static std::vector<ChessPiece> createStandardBackRow() {
        return {
            createPiece(ROOK),
            createPiece(KNIGHT),
            createPiece(BISHOP),
            createPiece(QUEEN),
            createPiece(KING),
            createPiece(BISHOP),
            createPiece(KNIGHT),
            createPiece(ROOK)
        };
    }

    static std::vector<ChessPiece> createStandardPromotionPieces() {
        return {
            createPiece(ROOK),
            createPiece(BISHOP),
            createPiece(KNIGHT),
            createPiece(QUEEN)
        };
    }
};
