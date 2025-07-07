#pragma once

#include "../board/Board.h"
#include "MoveValidator.h"
#include <set>

class MoveExecutor {
private:
    bool doPromotion;
    int promotionPos;
    int curMoveNum;

public:
    MoveExecutor() {
        doPromotion = false;
        promotionPos = NONE_SELECTED;
        curMoveNum = 0;
    }

    void setCurrentMoveNumber(int moveNum) {
        curMoveNum = moveNum;
    }

    GameState executeMove(Board& board, MoveValidator& moveValidator, int from, int to) {
        ChessPiece oldPiece = board.getCell(to).getChessPiece();
        ChessPiece& selectedPiece = board.getCell(from).getChessPiece();
        
        // Update piece state
        selectedPiece.onMove(abs(from - to), curMoveNum);
        
        // Move the piece on the board
        board.movePiece(from, to);
        
        // Check if this is a castling move
        set<int> castleMoves = moveValidator.getCastleMoves();
        if (castleMoves.find(to) != castleMoves.end()) {
            bool rookRight = to > from;
            int step = (rookRight) ? -1 : 1;
            int rookPos = (rookRight) ? from + 3 : from - 4;
            board.movePiece(rookPos, to + step);
        }
        // Check if this is an en passant move
        else if (to == board.getEnPassantMove()) {
            int width = board.getWidth();
            PieceSide opposingSide = (board.getCell(to).getChessPiece().getSide() == PieceSide::WHITE) ? 
                                     PieceSide::BLACK : PieceSide::WHITE;
            int newIdx = to + width - 2 * width * (opposingSide == PieceSide::BLACK);
            oldPiece = board.getCell(newIdx).getChessPiece();
            board.movePiece(from, newIdx);
            board.setEnPassantMove(NONE_SELECTED);
        }
        
        // Update score and captures for the turn
        int turn = curMoveNum % 2;
        if (oldPiece.isActive()) {
            board.addScore(turn, oldPiece.getValue());
            board.addCapture(turn, oldPiece);
        }
        
        // Check game state (check, checkmate, etc)
        GameState turnState = moveValidator.check(board, board.getCell(to).getChessPiece().getSide(), true, oldPiece);
        
        // Check if pawn promotion is needed
        doPromotion = moveValidator.shouldPromote(board, selectedPiece, to);
        promotionPos = (doPromotion) ? to : NONE_SELECTED;
        
        return turnState;
    }
    
    void setPromotedPiece(Board& board, Cell& cell) {
        cell.movePiece(board.getCell(promotionPos));
        doPromotion = false;
        promotionPos = NONE_SELECTED;
    }
    
    bool isDoPromotion() const { return doPromotion; }
    
    PieceSide getPromotionSide(Board& board) {
        return board.getCell(promotionPos).getChessPiece().getSide();
    }
    
    int getPromotionPos() const { return promotionPos; }
};
