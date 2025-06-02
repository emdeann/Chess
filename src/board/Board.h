#pragma once

#include "Cell.h"
#include "Constants.h"
#include "Enums.h"
#include <vector>

using namespace std;

class Board {
private:
    int height, width;
    vector<Cell> cells;
    int enPassantMove;
    vector<int> scores;
    vector<vector<ChessPiece>> captures;

public:
    Board(int h, int w, sf::Sound& sound) {
        height = h;
        width = w;
        enPassantMove = NONE_SELECTED;
        scores = vector<int>(2);
        captures = vector<vector<ChessPiece>>(2);
        cells = vector<Cell>(h * w);
        
        initializePieces(sound);
    }

    void initializePieces(sf::Sound& sound) {
        vector<ChessPiece> standardBackRow = ChessPieceFactory::createStandardBackRow();
        
        for (int j = 0; j < 2; j++) {
            vector<ChessPiece> backRow = standardBackRow;
            for (int i = 0; i < width; i++) {
                ChessPiece& backPiece = backRow.at(i);
                ChessPiece pawn = ChessPieceFactory::createPiece(PieceType::PAWN);
                backPiece.setSound(&sound);  
                pawn.setSound(&sound);
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
