#pragma once

#include "Board.h"
#include <SFML/Graphics.hpp>
#include <set>

class BoardRenderer : public sf::Drawable {
private:
    Board* state;
    vector<sf::Text> scoreText;
    
public:
    BoardRenderer(Board* state, sf::Font& textFont) : state(state) {
        
        // Initialize score text
        for (int i = 0; i < 2; i++) {
            sf::Text t(textFont);
            t.setCharacterSize(24);
            t.setString("Score: 0");
            sf::FloatRect textRect = t.getLocalBounds();
            t.setOrigin({ textRect.position.x + textRect.size.x / 2.0f,
                textRect.position.y + textRect.size.y / 2.0f });
            t.setPosition(sf::Vector2f(BOARD_DIM_IN_WINDOW / 4.f, Y_OFFSET / 2.f + (BOARD_DIM_IN_WINDOW + Y_OFFSET) * i));
            scoreText.push_back(t);
        }
    }
    
    void updateScoreText(int side, int score) {
        ostringstream text;
        text << "Score: " << score;
        scoreText.at(side).setString(text.str());
    }
    
    void highlightValidMoves(const set<int>& moves, PieceSide side = PieceSide::NONE) {
        for (int i : moves) {
            sf::Color color = (!state->getCell(i).getChessPiece().isActive() || 
                              state->getCell(i).getChessPiece().getSide() == side) ? 
                              sf::Color::Green : sf::Color::Red;
            state->getCell(i).toggleHighlight(color);
        }
    }
    
    void toggleCellSelected(int pos) {
        state->getCell(pos).toggleSelected();
    }
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        int mid = BOARD_DIM_IN_WINDOW / 2;
        int piecesInRow = 7;
        float capSize = DEFAULT_ITEM_SIZE * 0.7;
        int betweenCaptures = Y_OFFSET / 4;
        
        // Draw the board cells and pieces
        const vector<Cell>& cells = state->getCells();
        for (int i = 0; i < cells.size(); i++) {
            target.draw(cells.at(i));
        }
        
        // Draw captured pieces above/below board
        const vector<vector<ChessPiece>>& captures = state->getCaptures();
        for (int i = 0; i < captures.size(); i++) {
            vector<ChessPiece> cur = captures.at(i);
            for (int j = 0; j < cur.size(); j++) {
                ChessPiece piece = cur.at(j);
                sf::Sprite pieceSprite(piece.getTexture());
                pieceSprite.setOrigin({ static_cast<float>(pieceSprite.getTexture().getSize().x) / 2, static_cast<float>(pieceSprite.getTexture().getSize().y) / 2 });
                pieceSprite.setScale({ capSize, capSize });
                pieceSprite.setPosition({ mid + static_cast<float>(CELL_WIDTH) / 4 + betweenCaptures * (j % piecesInRow), betweenCaptures + (BOARD_DIM_IN_WINDOW + Y_OFFSET) * i + betweenCaptures * (static_cast<float>(j) / piecesInRow) });
                target.draw(pieceSprite);
            }
        }
        
        // Draw scores
        for (int i = 0; i < scoreText.size(); i++) {
            target.draw(scoreText.at(i));
        }
    }
};
