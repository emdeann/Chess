#pragma once

#include "windows.h"
#include "Cell.h"
#include "ChessPiece.h"
#include "ChessPieceBuilder.h"
#include "Constants.h"
#include "Enums.h"
#include <set>
#include <vector>
#include <iostream>
#include <iomanip>


using namespace std;

const vector<ChessPiece> standardBackRow = ChessPieceFactory::createStandardBackRow();


class Board : public sf::Drawable {
private:
    int height, width, curMoveNum, enPassantMove;
    vector<Cell> brd;
    vector<int> scores;
    vector<sf::Text> scoreText;
    vector<vector<ChessPiece>> captures;
    int selected;
    set<int> currentValidMoves, castleMoves;
    sf::Sound* moveSound;
    bool doPromotion;
    int promotionPos, promotionSide;

    // Helpers

    // Verify that a move does not wrap to the other side of the board display
    bool moveNotWrapped(int origPos, int newPos) {
        return abs(newPos % width - origPos % width) < width / 2;
    }


    // Get bounds for diagonals (either direction) for use when finding available moves
    int getDiagonalMin(int pos, bool left) {
        int loc = pos;
        int prevLoc = loc;
        while (loc >= 0 && moveNotWrapped(loc, prevLoc)) {
            prevLoc = loc;
            loc -= width + left - !left;
        }
        return loc;
    }

    int getDiagonalMax(int pos, bool left) {
        int loc = pos;
        int prevLoc = loc;
        while (loc < width * height && moveNotWrapped(loc, prevLoc)) {
            prevLoc = loc;
            loc += width + left - !left;
        }
        return loc;
    }

    PieceSide getOpposingSide(PieceSide side) {
        return (side == PieceSide::WHITE) ? PieceSide::BLACK : PieceSide::WHITE;
    }

    // New helper method to convert MovePattern to step value
    int getMovePatternStep(MoveDirection pattern) const {
        switch (pattern) {
            case MoveDirection::HORIZONTAL: return 1;
            case MoveDirection::VERTICAL: return width;
            case MoveDirection::DIAGONAL_LEFT: return width + 1;
            case MoveDirection::DIAGONAL_RIGHT: return width - 1;
            default: return 0;
        }
    }

    // New helper to get move pattern bounds
    pair<int, int> getMovePatternBounds(int pos, MoveDirection pattern) {
        int min, max;
        switch (pattern) {
            case MoveDirection::HORIZONTAL:
                min = pos - pos % width - 1;
                max = (pos - pos % width) + width;
                break;
            case MoveDirection::VERTICAL:
                min = pos - width * (pos / height + 1);
                max = width * (height + 1) - (width - pos % width);
                break;
            case MoveDirection::DIAGONAL_LEFT:
                min = getDiagonalMin(pos, true);
                max = getDiagonalMax(pos, true);
                break;
            case MoveDirection::DIAGONAL_RIGHT:
                min = getDiagonalMin(pos, false);
                max = getDiagonalMax(pos, false);
                break;
        }
        return {min, max};
    }

public:
    Board(int h, int w, sf::Sound& sound, sf::Font& textFont) {
        height = h;
        width = w;
        curMoveNum = 0;
        scores = vector<int>(2);
        scoreText = vector<sf::Text>(2);
        captures = vector<vector<ChessPiece>>(2);
        brd = vector<Cell>(h * w);
        castleMoves = { NONE_SELECTED, NONE_SELECTED };
        selected = NONE_SELECTED;
        promotionPos = NONE_SELECTED, promotionSide = -1;
        enPassantMove = NONE_SELECTED;
        doPromotion = false;
        moveSound = &sound;
        for (int j = 0; j < 2; j++) {
            vector<ChessPiece> backRow = standardBackRow;
            for (int i = 0; i < w; i++) {
                ChessPiece& backPiece = backRow.at(i);
                ChessPiece pawn = ChessPieceFactory::createPiece(PieceType::PAWN);
                backPiece.setSound(moveSound);  
                pawn.setSound(moveSound);
                if (j) {
                    backPiece.switchSide();
                    pawn.switchSide();
                }
                brd.at(i + j * (h * w - w)).setChessPiece(backPiece);
                brd.at(w + i + (h * (w - 3)) * j).setChessPiece(pawn);
            }
        }
        
        for (int i = 0; i < h * w; i++) {
            Cell& c = brd.at(i);
            bool whiteSquare = (i % 2 == i / h % 2);
			c.setDefaultColor((whiteSquare) ? sf::Color::White : sf::Color::Black);
            c.setSize(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
            c.setPos(sf::Vector2f(CELL_WIDTH * (i % w), CELL_WIDTH * (i / h) + Y_OFFSET));
        }

        for (int i = 0; i < scoreText.size(); i++) {
            sf::Text& t = scoreText.at(i);
            t.setCharacterSize(24);
            t.setFont(textFont);
            t.setString("Score: 0");
            sf::FloatRect textRect = t.getLocalBounds();
            t.setOrigin(textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f);
            t.setPosition(sf::Vector2f(BOARD_DIM_IN_WINDOW / 4.f, Y_OFFSET / 2.f + (BOARD_DIM_IN_WINDOW + Y_OFFSET) * i));
		
        }
		
    }


    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        int mid = BOARD_DIM_IN_WINDOW / 2;
        int piecesInRow = 7;
        double capSize = DEFAULT_ITEM_SIZE * 0.7;
        int betweenCaptures = Y_OFFSET / 4;

        // Pass to draw function in cells to draw active squares & pieces
        for (int i = 0; i < brd.size(); i++) {
            target.draw(brd.at(i));
        }

        // Draw captured pieces above/below board
		
        for (int i = 0; i < captures.size(); i++) {
            vector<ChessPiece> cur = captures.at(i);
            for (int j = 0; j < cur.size(); j++) {
                ChessPiece piece = cur.at(j);
                sf::Sprite pieceSprite;
                sf::Texture texture = piece.getTexture();
                pieceSprite.setTexture(texture);
                pieceSprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
                pieceSprite.setScale(sf::Vector2f(capSize, capSize));
                pieceSprite.setPosition(mid + CELL_WIDTH/4 + betweenCaptures * (j % piecesInRow), betweenCaptures + (BOARD_DIM_IN_WINDOW + Y_OFFSET) * i + betweenCaptures * (j/piecesInRow));
                target.draw(pieceSprite);
            }
        }
        for (int i = 0; i < scores.size(); i++) {
            target.draw(scoreText.at(i));
        }
    }

	// Called when a tile is clicked on in the GUI
	GameState selectTile(int pos, int moveNum) {
		int turn = moveNum % 2;
		PieceSide activeSide = turn == 0 ? PieceSide::WHITE : PieceSide::BLACK;
		curMoveNum = moveNum;
		GameState turnState = GameState::NO_TURN;
		if (selected == NONE_SELECTED) {
			ChessPiece& curPiece = brd.at(pos).getChessPiece();
			if (curPiece.isActive() && curPiece.getSide() == activeSide) {
				selected = pos;
				currentValidMoves = getPossibleMoves(selected, curPiece, true, curPiece);
				toggleMoveHighlights(curPiece.getSide());
				brd.at(selected).toggleSelected();
			}
			
		}
		else {
			// if selected is not none, then a piece is selected - attempt to move it
			if (selected != pos && currentValidMoves.find(pos) != currentValidMoves.end()) {
				ChessPiece oldPiece = brd.at(pos).getChessPiece();
				ChessPiece& selectedPiece = brd.at(selected).getChessPiece();
				selectedPiece.onMove(abs(selected - pos), moveNum);
				brd.at(selected).movePiece(brd.at(pos));
				if (castleMoves.find(pos) != castleMoves.end()) {
					bool rookRight = pos > selected;
					int step = (rookRight) ? -1 : 1, rookPos = (rookRight) ? selected + 3 : selected - 4 ;
					brd.at(rookPos).movePiece(brd.at(pos + step));
				}
				else if (pos == enPassantMove) {
					int newIdx = pos + width - 2 * width * (getOpposingSide(brd.at(pos).getChessPiece().getSide()) == PieceSide::BLACK);
					oldPiece = brd.at(newIdx).getChessPiece();
					brd.at(selected).movePiece(brd.at(newIdx));
					enPassantMove = NONE_SELECTED;
		}
				if (oldPiece.isActive()) {
					scores.at(turn) += oldPiece.getValue();
					ostringstream text;
					text << "Score: " << scores.at(turn);
					scoreText.at(turn).setString(text.str());
					captures.at(turn).push_back(oldPiece);
				}
				turnState = check(brd.at(pos).getChessPiece().getSide(), true, oldPiece);
				doPromotion = shouldPromote(selectedPiece, pos);
				promotionPos = (doPromotion) ? pos : NONE_SELECTED;
		
			}
			// clicking an invalid space will still cancel the move 
			brd.at(selected).toggleSelected(); 
			selected = NONE_SELECTED;
			toggleMoveHighlights();
			currentValidMoves.clear();

		}

		return turnState;

	}

	bool shouldPromote(ChessPiece& piece, int pos) {
		// Verify that the piece is a pawn and is at the correct location for its side
		return piece.isOfType(PieceType::PAWN) && ((piece.getSide() == PieceSide::WHITE && (pos / height) == height - 1) ||
			(piece.getSide() == PieceSide::BLACK && !(pos / height)));
	}


	void toggleMoveHighlights(PieceSide side = PieceSide::NONE) {
		for (int i : currentValidMoves) {
  			sf::Color color = (!brd.at(i).getChessPiece().isActive() || brd.at(i).getChessPiece().getSide() == side) ? sf::Color::Green : sf::Color::Red;
			brd.at(i).toggleHighlight(color);
		}
	}

	// Determine if a side has check, checkmate, or stalemate | Args with default arguments are used when verifying legality of potential moves
	GameState check(PieceSide sideFor, bool checkAll, ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
		set<int> allMovesFor;
		GameState gameState = GameState::NONE;
		int opposingKingPos = (subPiece.isOfType(PieceType::KING) && subPieceAt != NONE_SELECTED) * subPieceAt;
		for (int i = 0; i < brd.size(); i++) {
			ChessPiece& cur = brd.at(i).getChessPiece();
			if (cur.isOnSide(sideFor) && i != subPieceAt) {
				set<int> curMoves = getPossibleMoves(i, cur, false, subPiece, subPieceAt, removePieceFrom);
				allMovesFor.insert(curMoves.begin(), curMoves.end());
			}
			else if (!(subPiece.isOfType(PieceType::KING) && subPieceAt != NONE_SELECTED) && cur.isOfType(PieceType::KING)) {
				opposingKingPos = i;
			}
		}
		if (allMovesFor.find(opposingKingPos) != allMovesFor.end()) {
			gameState = GameState::CHECK;
		}
		if (checkAll) {
			bool noOpponentMoves = true;
			for (int i = 0; i < brd.size() && noOpponentMoves; i++) {
				ChessPiece& cur = brd.at(i).getChessPiece();
				noOpponentMoves = !cur.isOnSide(getOpposingSide(sideFor)) || getPossibleMoves(i, cur, true, subPiece, subPieceAt, removePieceFrom).empty();
			}
			gameState = (noOpponentMoves) ? ((gameState == GameState::CHECK) ? GameState::CHECKMATE : GameState::STALEMATE) : gameState;
		}
		return gameState;

	}

	// Highest level method to get possible moves of any piece
	set<int> getPossibleMoves(int pos, ChessPiece& piece, bool verifyLegal, ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
		set<int> allMoves;
		if (piece.useStrictMotion()) {
			allMoves = getStrictMoves(pos, piece, verifyLegal, subPiece, subPieceAt, removePieceFrom);
		}
		else {
			vector<bool> permissions = piece.getValidDirections();
			if (permissions.back()) {
				permissions.push_back(true);
			}
			vector<MoveDirection> possibleMoves = { MoveDirection::HORIZONTAL, MoveDirection::VERTICAL, MoveDirection::DIAGONAL_LEFT, MoveDirection::DIAGONAL_RIGHT };
			for (int i = 0; i < permissions.size(); i++) {
				if (permissions.at(i)) {
					set<int> cur = getMoves(pos, piece.getRange(), piece.getSide(), possibleMoves.at(i), verifyLegal, subPiece, subPieceAt, removePieceFrom);
					allMoves.insert(cur.begin(), cur.end());
				}
			}
		}
		if (piece.isOfType(PieceType::KING) && verifyLegal) {
			setCastleMoves(pos);
			for (int i : castleMoves) {
				if (i != NONE_SELECTED) {
					allMoves.insert(i);
				}
			}
		}
		return allMoves;
	}

	vector<int> getNeighbors(int pos) {
		vector<int> neighbors;
		for (int i = 0; i < 2; i++) {
			int adj = pos - 1 + 2 * i;
			if ((!i && pos % width != 0) || (i && pos % width != width - 1)) {
				neighbors.push_back(adj);
			}
		}
		return neighbors;
	}

	void setCastleMoves(int kingPos) {
		ChessPiece& king = brd.at(kingPos).getChessPiece();
		vector<int> distances{ 3, 4 };
		bool canCastle = king.canCastle();
		bool inCheck = check(getOpposingSide(king.getSide()), false, king) == GameState::CHECK;
		castleMoves.erase(castleMoves.begin(), castleMoves.end());
		for (int i = 0; i < distances.size(); i++) {
			if (canCastle && !inCheck && clearPathToRook(kingPos, -2 * i + 1, distances.at(i), king.getSide())) {	
				castleMoves.insert( kingPos + 2 - 4*i);
			}
			else {
				castleMoves.insert(NONE_SELECTED);
			}
		}
	}

	bool clearPathToRook(int pos, int step, int dist, PieceSide side) {
		bool clearPath = true;
		for (int i = pos + step; abs(pos - i) < dist && clearPath; i += step) {
			clearPath = !brd.at(i).getChessPiece().isActive();
		}
		int end = pos + dist * step;
		ChessPiece& endPiece = brd.at(end).getChessPiece();
		return clearPath && endPiece.isOnSide(side) && endPiece.getName() == "rook";
	}

    /* Gets move from a standard piece(defined directions and movement range)
     Last four args are used in testing potential moves: the piece at subPieceAt is treated as subPiece, and the piece at removePieceFrom is treated as an empty space
     Effectively treats the board as if subPiece is at subPieceAt instead of removePieceFrom, without actually modifying the game board */
    set<int> getMoves(int pos, int range, PieceSide side, MoveDirection pattern, bool verifyLegal, ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        pair<int, int> bounds = getMovePatternBounds(pos, pattern);
        int step = getMovePatternStep(pattern);
        
        set<int> validMoves;
        int bound1 = pos, bound2 = pos;
        int rangeMax = range * step;
        int prevLoc = pos;

        while (bound1 > bounds.first && (abs(bound1 - pos) <= rangeMax)
            && (!((bound1 == subPieceAt) ? subPiece : brd.at(bound1).getChessPiece()).isOnSide(side) || bound1 == removePieceFrom || bound1 == pos)
            && (!((prevLoc == subPieceAt) ? subPiece : brd.at(prevLoc).getChessPiece()).isActive() || prevLoc == removePieceFrom || prevLoc == pos)) {
            prevLoc = bound1;
            bound1 -= step;
        }

        prevLoc = pos;
        while (bound2 < bounds.second && (abs(bound2 - pos) <= rangeMax)
            && (!((bound2 == subPieceAt) ? subPiece : brd.at(bound2).getChessPiece()).isOnSide(side) || bound2 == removePieceFrom || bound2 == pos)
            && (!((prevLoc == subPieceAt) ? subPiece : brd.at(prevLoc).getChessPiece()).isActive() || prevLoc == removePieceFrom || prevLoc == pos)) {
            prevLoc = bound2;
            bound2 += step;
        }
        for (int i = bound1 + step; i < bound2; i += step) {
            if (i != pos && !(verifyLegal && moveResultsInCheck(brd.at(pos).getChessPiece(), i, pos))) {
                validMoves.insert(i);
            }
        }
        return validMoves;
    }

	// Method just makes this check more readable
	bool moveResultsInCheck(ChessPiece& movingPiece, int moveTo, int moveFrom) {
		GameState moveState = check(getOpposingSide(movingPiece.getSide()), false, movingPiece, moveTo, moveFrom);
		return moveState == GameState::CHECK || moveState == GameState::CHECKMATE;
	}

	// Used for pieces with different movement patterns (pawns, knights)
	set<int> getStrictMoves(int pos, ChessPiece& piece, bool verifyLegal, ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
		set<int> moves;
		bool pieceInRange, continueChecking = true;
		vector<int> moveList = piece.getStrictMoves();
		for (int i = 0; i < moveList.size() && continueChecking; i++) {
			int newPos = pos + moveList.at(i);
			pieceInRange = newPos >= 0 && newPos < height * width;
			ChessPiece& posPiece = (newPos == subPieceAt || !pieceInRange) ? subPiece : brd.at(newPos).getChessPiece();
			if (pieceInRange && (newPos == removePieceFrom || !posPiece.isActive() 
				|| (posPiece.getSide() != piece.getSide() && piece.canTakeStrictly()))
				&& moveNotWrapped(pos, newPos) && !(verifyLegal && moveResultsInCheck(piece, newPos, pos))) {
				moves.insert(newPos);
			}
			else {
				continueChecking = !piece.isOfType(PieceType::PAWN);
			}
		}
		if (piece.hasSpecialTakeMoves()) {
			vector<int> takeMoves = piece.getTakeMoves();
			for (int i : takeMoves) {
				int newPos = pos + i;
				pieceInRange = newPos >= 0 && newPos < height * width;
				ChessPiece& posPiece = (newPos == subPieceAt || !pieceInRange) ? subPiece : brd.at(newPos).getChessPiece();
				if (newPos >= 0 && newPos < height * width && posPiece.isActive()
					&& posPiece.getSide() != piece.getSide() && moveNotWrapped(pos, newPos) && (newPos != removePieceFrom) 
					&& !(verifyLegal && moveResultsInCheck(piece, newPos, pos))) {
					moves.insert(newPos);
				}
			}
		}
		if (piece.isOfType(PieceType::PAWN)) {
			vector<int> neighbors = getNeighbors(pos);
			for (int i : neighbors) {
				ChessPiece& neighbor = brd.at(i).getChessPiece();
				if (neighbor.getSide() != piece.getSide() && neighbor.canBeEnPassanted(curMoveNum)) {
					int idx = i - width + 2 * width * (neighbor.getSide() == PieceSide::BLACK);
					moves.insert(idx);
					enPassantMove = idx;
				}
			}
		}
		return moves;
	}

    bool isDoPromotion() const {  
        return doPromotion;
    }
    PieceSide getPromotionSide() {
        return brd.at(promotionPos).getChessPiece().getSide();
    }

    void setPromotedPiece(Cell& cell) {
        cell.getChessPiece().setSound(moveSound);
        cell.movePiece(brd.at(promotionPos));
        doPromotion = false;
        promotionPos = NONE_SELECTED;
    }
};