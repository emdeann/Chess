#pragma once

#include "constants/Constants.h"
#include "board/Board.h"
#include "moves/MoveExecutor.h"
#include "moves/MoveValidator.h"
#include "../ui/SoundManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <set>

class BoardManager : public sf::Drawable {
private:
    Board board;
    MoveValidator validator;
    MoveExecutor executor;
    int selected;
    set<int> currentValidMoves;

public:
    BoardManager(int h, int w) : board(h, w) {
        selected = NONE_SELECTED;
    }

    GameState selectTile(int pos, int moveNum, SoundManager& sounds) {
        int turn = moveNum % 2;
        PieceSide activeSide = turn == 0 ? PieceSide::WHITE : PieceSide::BLACK;
        executor.setCurrentMoveNumber(moveNum);
        GameState turnState = GameState::NO_TURN;
        
        if (selected == NONE_SELECTED) {
            const ChessPiece& curPiece = board.getCell(pos).getChessPiece();
            if (curPiece.isActive() && curPiece.getSide() == activeSide) {
                selected = pos;
                currentValidMoves = validator.getPossibleMoves(board, selected, curPiece, true, curPiece);
                board.highlightValidMoves(currentValidMoves, curPiece.getSide());
                board.toggleCellSelected(selected);
            }
        } else {
            if (selected != pos && currentValidMoves.find(pos) != currentValidMoves.end()) {
                sounds.playMoveSound();
                turnState = executor.executeMove(board, validator, selected, pos);
            }
            
            board.toggleCellSelected(selected);
            selected = NONE_SELECTED;
            board.highlightValidMoves(currentValidMoves);
            currentValidMoves.clear();
        }
        
        return turnState;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        board.draw(target, states);
    }
    
    bool isDoPromotion() const {
        return executor.isDoPromotion();
    }
    
    PieceSide getPromotionSide() {
        return executor.getPromotionSide(board);
    }
    
    void setPromotedPiece(Cell& cell) {
        executor.setPromotedPiece(board, cell);
    }

    const vector<int>& getScores() const {
        return board.getScores();
    }
};
