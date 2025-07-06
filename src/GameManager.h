#pragma once

#include "ui/UIManager.h"
#include "ui/SoundManager.h"
#include "board/BoardManager.h"
#include "constants/Constants.h"

class GameManager {
private:
    int move = 0;
    PieceSide winnerSide = PieceSide::NONE;

    bool holderPiecesSet = false, winSoundPlayed = false;
    GameState gameState = GameState::NONE;

    BoardManager board;
    UIManager uiManager;
    SoundManager soundManager;

    bool inBoardRange(int x, int y) {
        return x >= 0 && x < BOARD_DIM_IN_WINDOW && y >= 0 && y < BOARD_DIM_IN_WINDOW;
    }
public:
    GameManager(): board(BOARD_WIDTH, BOARD_HEIGHT) {}

    UIManager& getUI() {
        return uiManager;
    }

    SoundManager& getSound() {
        return soundManager;
    }

    BoardManager& getBoard() {
        return board;
    }

    template<typename... Ts>
    void handleEvents(Ts &&... handlers) {
        uiManager.getWindow().handleEvents(handlers...);
    }

    void updateScreen() {
        uiManager.update(board, gameState, winnerSide);
    }

    void tryStartGame(const sf::Event::MouseButtonPressed& clickEvent) {
        if (uiManager.startClicked(clickEvent)) {
            soundManager.playStartSounds();
        }
    }

    void tryRestartGame(const sf::Event::MouseButtonPressed& clickEvent) {
        if (uiManager.startClicked(clickEvent)) {
            board = BoardManager(BOARD_WIDTH, BOARD_HEIGHT);
            uiManager = UIManager();
            soundManager = SoundManager();
            soundManager.playStartSounds();
        }
    }

    void runTurn(const sf::Event::MouseButtonPressed& event) {
        bool promote = board.isDoPromotion();
        if (promote && !holderPiecesSet) {
            uiManager.setPlaceHolderPieces(board.getPromotionSide());
            holderPiecesSet = true;
        }

        sf::Vector2i mousePos = event.position;
        int yAdj = mousePos.y - Y_OFFSET;
        int  promotionSelection =  uiManager.getPromotionSelection(event);
        if (promotionSelection != NONE_SELECTED) {
            board.setPromotedPiece(uiManager.getPromotionCells().at(promotionSelection));
            holderPiecesSet = false;
        }
        else if (inBoardRange(mousePos.x, yAdj)) {
            int boardPos = (mousePos.x / CELL_WIDTH) + (yAdj / CELL_WIDTH) * BOARD_WIDTH;
            GameState curState = board.selectTile(boardPos, move, soundManager);
            if (curState != GameState::NO_TURN) {
                move += 1;
                gameState = curState;
                if (gameState == GameState::CHECKMATE) {
                    soundManager.playWinSound();
                    winnerSide = (move - 1) % 2 == 0 ? PieceSide::WHITE : PieceSide::BLACK;
                }
            }
        }
    }
};