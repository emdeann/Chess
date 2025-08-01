#pragma once

#include "../board/Board.h"
#include "MoveValidator.h"
#include <set>

class MoveExecutor {
private:
    Board* state;
    MoveValidator* moveValidator;
    sf::Sound* moveSound;
    bool doPromotion;
    int promotionPos;
    int curMoveNum;

public:
    MoveExecutor(Board* state, MoveValidator* validator, sf::Sound& sound) 
        : state(state), moveValidator(validator), moveSound(&sound) {
        doPromotion = false;
        promotionPos = NONE_SELECTED;
        curMoveNum = 0;
    }

    void setCurrentMoveNumber(int moveNum) {
        curMoveNum = moveNum;
    }

    GameState executeMove(int from, int to) {
        ChessPiece oldPiece = state->getCell(to).getChessPiece();
        ChessPiece& selectedPiece = state->getCell(from).getChessPiece();
        
        // Update piece state
        selectedPiece.onMove(abs(from - to), curMoveNum);
        
        // Move the piece on the board
        state->movePiece(from, to);
        
        // Check if this is a castling move
        set<int> castleMoves = moveValidator->getCastleMoves();
        if (castleMoves.find(to) != castleMoves.end()) {
            bool rookRight = to > from;
            int step = (rookRight) ? -1 : 1;
            int rookPos = (rookRight) ? from + 3 : from - 4;
            state->movePiece(rookPos, to + step);
        }
        // Check if this is an en passant move
        else if (to == state->getEnPassantMove()) {
            int width = state->getWidth();
            PieceSide opposingSide = (state->getCell(to).getChessPiece().getSide() == PieceSide::WHITE) ? 
                                     PieceSide::BLACK : PieceSide::WHITE;
            int newIdx = to + width - 2 * width * (opposingSide == PieceSide::BLACK);
            oldPiece = state->getCell(newIdx).getChessPiece();
            state->movePiece(from, newIdx);
            state->setEnPassantMove(NONE_SELECTED);
        }
        
        // Update score and captures for the turn
        int turn = curMoveNum % 2;
        if (oldPiece.isActive()) {
            state->addScore(turn, oldPiece.getValue());
            state->addCapture(turn, oldPiece);
        }
        
        // Check game state (check, checkmate, etc)
        GameState turnState = moveValidator->check(state->getCell(to).getChessPiece().getSide(), true, oldPiece);
        
        // Check if pawn promotion is needed
        doPromotion = moveValidator->shouldPromote(selectedPiece, to);
        promotionPos = (doPromotion) ? to : NONE_SELECTED;
        
        return turnState;
    }
    
    void setPromotedPiece(Cell& cell) {
        cell.getChessPiece().setSound(moveSound);
        cell.movePiece(state->getCell(promotionPos));
        doPromotion = false;
        promotionPos = NONE_SELECTED;
    }
    
    bool isDoPromotion() const { return doPromotion; }
    
    PieceSide getPromotionSide() {
        return state->getCell(promotionPos).getChessPiece().getSide();
    }
    
    int getPromotionPos() const { return promotionPos; }
};
