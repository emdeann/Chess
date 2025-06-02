#pragma once

#include "Constants.h"
#include "Board.h"
#include "MoveValidator.h"
#include "BoardRenderer.h"
#include "MoveExecutor.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <set>

class GameManager : public sf::Drawable {
private:
    Board state;
    MoveValidator validator;
    BoardRenderer renderer;
    MoveExecutor executor;
    int selected;
    set<int> currentValidMoves;

public:
    GameManager(int h, int w, sf::Sound& sound, sf::Font& textFont) 
        : state(h, w, sound), 
          validator(&state), 
          renderer(&state, textFont), 
          executor(&state, &validator, sound) {
        selected = NONE_SELECTED;
    }

    // Called when a tile is clicked on in the GUI
    GameState selectTile(int pos, int moveNum) {
        int turn = moveNum % 2;
        PieceSide activeSide = turn == 0 ? PieceSide::WHITE : PieceSide::BLACK;
        executor.setCurrentMoveNumber(moveNum);
        GameState turnState = GameState::NO_TURN;
        
        if (selected == NONE_SELECTED) {
            // Try to select a piece
            ChessPiece& curPiece = state.getCell(pos).getChessPiece();
            if (curPiece.isActive() && curPiece.getSide() == activeSide) {
                selected = pos;
                currentValidMoves = validator.getPossibleMoves(selected, curPiece, true, curPiece);
                renderer.highlightValidMoves(currentValidMoves, curPiece.getSide());
                renderer.toggleCellSelected(selected);
            }
        } else {
            // Attempt to move the selected piece
            if (selected != pos && currentValidMoves.find(pos) != currentValidMoves.end()) {
                turnState = executor.executeMove(selected, pos);
                
                // Update score display
                const vector<int>& scores = state.getScores();
                renderer.updateScoreText(turn, scores.at(turn));
            }
            
            // Deselect the piece regardless of move validity
            renderer.toggleCellSelected(selected);
            selected = NONE_SELECTED;
            renderer.highlightValidMoves(currentValidMoves); // This will toggle off the highlights
            currentValidMoves.clear();
        }
        
        return turnState;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        renderer.draw(target, states);
    }
    
    bool isDoPromotion() const {
        return executor.isDoPromotion();
    }
    
    PieceSide getPromotionSide() {
        return executor.getPromotionSide();
    }
    
    void setPromotedPiece(Cell& cell) {
        executor.setPromotedPiece(cell);
    }
};
