#pragma once

#include "Cell.h"
#include "../constants/Constants.h"
#include "../constants/Enums.h"
#include <vector>
#include <set>

using namespace std;

class Board {
private:
    int height, width;
    vector<Cell> cells;
    int enPassantMove;
    vector<int> scores;
    vector<vector<ChessPiece>> captures;

public:
    Board(int h, int w) {
        height = h;
        width = w;
        enPassantMove = NONE_SELECTED;
        scores = vector<int>(2);
        captures = vector<vector<ChessPiece>>(2);
        cells = vector<Cell>(h * w);
        
        initializePieces();
    }

    void initializePieces() {
        vector<ChessPiece> standardBackRow = ChessPieceFactory::createStandardBackRow();
        
        for (int j = 0; j < 2; j++) {
            vector<ChessPiece> backRow = standardBackRow;
            for (int i = 0; i < width; i++) {
                ChessPiece& backPiece = backRow.at(i);
                ChessPiece pawn = ChessPieceFactory::createPiece(PieceType::PAWN);
                if (j) {
                    backPiece.switchSide();
                    pawn.switchSide();
                }
                cells.at(i + j * (height * width - width)).setChessPiece(backPiece);
                cells.at(width + i + (height * (width - 3)) * j).setChessPiece(pawn);
            }
        }
        
        // Setup cells
        for (int i = 0; i < height * width; i++) {
            Cell& c = cells.at(i);
            bool whiteSquare = (i % 2 == i / height % 2);
            c.setDefaultColor((whiteSquare) ? sf::Color::White : sf::Color::Black);
            c.setSize(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
            c.setPos(sf::Vector2f(CELL_WIDTH * (i % width), CELL_WIDTH * (i / height) + Y_OFFSET));
        }
    }

    void highlightValidMoves(const set<int>& moves, PieceSide side = PieceSide::NONE) {
        for (int i : moves) {
            sf::Color color = (!cells.at(i).getChessPiece().isActive() ||
                cells.at(i).getChessPiece().getSide() == side) ?
                sf::Color::Green : sf::Color::Red;
            cells.at(i).toggleHighlight(color);
        }
    }

    void toggleCellSelected(int pos) {
        cells.at(pos).toggleSelected();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        int mid = BOARD_DIM_IN_WINDOW / 2;
        int piecesInRow = 7;
        float capSize = DEFAULT_ITEM_SIZE * 0.7;
        int betweenCaptures = Y_OFFSET / 4;

        for (int i = 0; i < cells.size(); i++) {
            target.draw(cells.at(i));
        }

        // Draw captured pieces above/below board
        const vector<vector<ChessPiece>>& captures = getCaptures();
        for (int i = 0; i < captures.size(); i++) {
            vector<ChessPiece> cur = captures.at(i);
            for (int j = 0; j < cur.size(); j++) {
                ChessPiece piece = cur.at(j);
                sf::Sprite pieceSprite(piece.getTexture());
                pieceSprite.setOrigin({ static_cast<float>(pieceSprite.getTexture().getSize().x) / 2, static_cast<float>(pieceSprite.getTexture().getSize().y) / 2 });
                pieceSprite.setScale({ capSize, capSize });
                pieceSprite.setPosition({ mid + static_cast<float>(CELL_WIDTH) / 4 + betweenCaptures * (j % piecesInRow), static_cast<float>(betweenCaptures + (BOARD_DIM_IN_WINDOW + Y_OFFSET) * i + betweenCaptures * (j / piecesInRow)) });
                target.draw(pieceSprite);
            }
        }
    }

    const Cell& getCell(int pos) const { return cells.at(pos); }
    Cell& getCell(int pos) { return cells.at(pos); }
    const vector<Cell>& getCells() const { return cells; }
    int getHeight() const { return height; }
    int getWidth() const { return width; }
    int getEnPassantMove() const { return enPassantMove; }
    const vector<int>& getScores() const { return scores; }
    const vector<vector<ChessPiece>>& getCaptures() const { return captures; }
    
    void setEnPassantMove(int move) { enPassantMove = move; }
    void addScore(int side, int value) { scores.at(side) += value; }
    void addCapture(int side, const ChessPiece& piece) { captures.at(side).push_back(piece); }
    
    int size() const { return cells.size(); }
    
    void movePiece(int from, int to) {
        cells.at(from).movePiece(cells.at(to));
    }
};
