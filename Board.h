#pragma once

#include "windows.h"
#include "Cell.h"
#include "ChessPiece.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "Knight.h"
#include "King.h"
#include "Bishop.h"
#include <set>
#include <vector>
#include <iostream>
#include <iomanip>


using namespace std;

const int NONE_SELECTED = -10;
const int Y_OFFSET = 128;
const int BOARD_DIM_IN_WINDOW = 512;
const enum State {NONE, CHECK, CHECKMATE, STALEMATE};


class Board : public sf::Drawable {
private:
	int height, width;
	vector<Cell> brd;
	vector<int> scores;
	vector<vector<ChessPiece>> captures;
	int selected;
	set<int> currentValidMoves;
	vector<ChessPiece> backRow;

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


public:
	Board(int h, int w) {
		height = h;
		width = w;
		scores = vector<int>(2);
		captures = vector<vector<ChessPiece>>(2);
		brd = vector<Cell>(h * w);
		selected = NONE_SELECTED;
		backRow = { Rook(), Knight(w), Bishop(), Queen(), King(), Bishop(), Knight(w), Rook() };
		for (int i = 0; i < w; i++) {
			ChessPiece p = backRow.at(i);
			Pawn pawn(w);
			brd.at(i).setChessPiece(p);
			p.switchSide();
			brd.at((h * w - w) + i).setChessPiece(p);
			brd.at(w + i).setChessPiece(pawn);
			pawn.switchSide();
			brd.at((h * (w - 2) + i)).setChessPiece(pawn);
		}
		for (int i = 0; i < h * w; i++) {
			Cell& c = brd.at(i);
			bool whiteSquare = (i % 2 == i / h % 2);
			c.setDefaultColor((whiteSquare) ? sf::Color::White : sf::Color::Black);
			c.setSize(sf::Vector2f(CELL_WIDTH, CELL_WIDTH));
			c.setPos(sf::Vector2f(CELL_WIDTH * (i % w), CELL_WIDTH * (i / h) + Y_OFFSET));
			c.getChessPiece().loadTexture();
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
	}

	// Called when a tile is clicked on in the GUI
	bool selectTile(int pos, int move) {
		bool turnCompleted = false;
		if (selected == NONE_SELECTED) {
			ChessPiece& curPiece = brd.at(pos).getChessPiece();
			if (curPiece.isActive() && curPiece.getSide() == move) {
				selected = pos;
				currentValidMoves = getPossibleMoves(selected, curPiece, true);
				toggleMoveHighlights(curPiece.getSide());
				brd.at(selected).toggleSelected();
			}
			
		}
		else {
			// if selected is not none, then a piece is selected - attempt to move it
			if (selected != pos && currentValidMoves.find(pos) != currentValidMoves.end()) {
				ChessPiece curPiece = brd.at(selected).getChessPiece();
				ChessPiece oldPiece = brd.at(pos).getChessPiece();
				brd.at(selected).movePiece(brd.at(pos));
				if (oldPiece.isActive()) {
					scores.at(move) += oldPiece.getValue();
					captures.at(move).push_back(oldPiece);
				}
;				turnCompleted = true;
			}
			// clicking an invalid space will still cancel the move
			brd.at(selected).toggleSelected();
			selected = NONE_SELECTED;
			toggleMoveHighlights();
			currentValidMoves.clear();

		}

		return turnCompleted;

		
	}


	void toggleMoveHighlights(int side = -1) {
		for (int i : currentValidMoves) {
  			sf::Color color = (!brd.at(i).getChessPiece().isActive() || brd.at(i).getChessPiece().getSide() == side) ? sf::Color::Green : sf::Color::Red;
			brd.at(i).toggleHighlight(color);
		}
	}

	// Determine if a side has check, checkmate, or stalemate | Args with default arguments are used when verifying legality of potential moves
	State check(int sideFor, bool checkAll, ChessPiece subPiece = ChessPiece(), int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
		set<int> allMovesFor;
		State gameState = NONE;
		int opposingKingPos = (subPiece.getName() == "king") * subPieceAt;
		for (int i = 0; i < brd.size(); i++) {
			ChessPiece cur = brd.at(i).getChessPiece();
			if (cur.isOnSide(sideFor) && i != subPieceAt) {
				set<int> curMoves = getPossibleMoves(i, cur, false, subPiece, subPieceAt, removePieceFrom);
				allMovesFor.insert(curMoves.begin(), curMoves.end());
			}
			else if (!(subPiece.getName() == "king") && cur.getName() == "king") {
				opposingKingPos = i;
			}
		}
		if (allMovesFor.find(opposingKingPos) != allMovesFor.end()) {
			gameState = CHECK;
		}
		if (checkAll) {
			bool noOpponentMoves = true;
			for (int i = 0; i < brd.size() && noOpponentMoves; i++) {
				ChessPiece cur = brd.at(i).getChessPiece();
				noOpponentMoves = !cur.isOnSide(sideFor ^ 1) || getPossibleMoves(i, cur, true, subPiece, subPieceAt, removePieceFrom).empty();
			}
			gameState = (noOpponentMoves) ? ((gameState == CHECK) ? CHECKMATE : STALEMATE) : gameState;
		}
		return gameState;

	}

	// Highest level method to get possible moves of any piece
	set<int> getPossibleMoves(int pos, ChessPiece& piece, bool verifyLegal, ChessPiece subPiece = ChessPiece(), int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
		set<int> allMoves;
		if (piece.useStrictMotion()) {
			allMoves = getStrictMoves(pos, piece, verifyLegal, subPiece, subPieceAt, removePieceFrom);
		}
		else {
			vector<bool> permissions = piece.getValidDirections();
			if (permissions.back()) {
				permissions.push_back(true);
			}
			string possibleMoves = "hvlr";
			for (int i = 0; i < permissions.size(); i++) {
				if (permissions.at(i)) {
					set<int> cur = getMoves(pos, piece.getRange(), piece.getSide(), possibleMoves.at(i), verifyLegal, subPiece, subPieceAt, removePieceFrom);
					allMoves.insert(cur.begin(), cur.end());
				}
			}
		}
		return allMoves;
	}

	/* Gets move from a standard piece(defined directions and movement range)
	 Last four args are used in testing potential moves: the piece at subPieceAt is treated as subPiece, and the piece at removePieceFrom is treated as an empty space
	 Effectively treats the board as if subPiece is at subPieceAt instead of removePieceFrom, without actually modifying the game board */
	set<int> getMoves(int pos, int range, int side, char method, bool verifyLegal, ChessPiece subPiece = ChessPiece(), int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
		int bound1 = pos, bound2 = pos, step, min, max, rangeMax, prevLoc;
		set<int> validMoves;
		switch (method) {
		case 'h':
			min = pos - pos % width - 1;
			max = (pos - pos % width) + width;
			step = 1;
			break;
		case 'v':
			min = pos - width * (pos / height + 1);
			max = width * (height + 1) - (width - pos % width);
			step = width;
			break;
		case 'l': // left diagonal
			min = getDiagonalMin(pos, true);
			max = getDiagonalMax(pos, true);
			step = width + 1;
			break;
		case 'r':
			min = getDiagonalMin(pos, false);
			max = getDiagonalMax(pos, false);
			step = width - 1;
			break;
		}
		rangeMax = range * step;
		prevLoc = pos;
		while (bound1 > min && (abs(bound1 - pos) <= rangeMax)
			&& (!((bound1 == subPieceAt) ? subPiece : brd.at(bound1).getChessPiece()).isOnSide(side) || bound1 == removePieceFrom || bound1 == pos)
			&& (!((prevLoc == subPieceAt) ? subPiece : brd.at(prevLoc).getChessPiece()).isActive() || prevLoc == removePieceFrom || prevLoc == pos)) {
			prevLoc = bound1;
			bound1 -= step;
		}

		prevLoc = pos;
		while (bound2 < max && (abs(bound2 - pos) <= rangeMax)
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
	bool moveResultsInCheck(ChessPiece movingPiece, int moveTo, int moveFrom) {
		return check(movingPiece.getSide() ^ 1, false, movingPiece, moveTo, moveFrom);
	}

	// Used for pieces with different movement patterns (pawns, knights)
	set<int> getStrictMoves(int pos, ChessPiece piece, bool verifyLegal, ChessPiece subPiece = ChessPiece(), int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
		set<int> moves;
		bool pieceInRange;
		vector<int> moveList = piece.getStrictMoves();
		for (int i : moveList) {
			int newPos = pos + i;
			pieceInRange = newPos >= 0 && newPos < height * width;
			ChessPiece posPiece = (newPos == subPieceAt || !pieceInRange) ? subPiece : brd.at(newPos).getChessPiece();
			if (pieceInRange && (newPos == removePieceFrom || !posPiece.isActive() 
				|| (posPiece.getSide() != piece.getSide() && piece.canTakeStrictly()))
				&& moveNotWrapped(pos, newPos) && !(verifyLegal && moveResultsInCheck(piece, newPos, pos))) {
				moves.insert(newPos);
			}
		}
		if (piece.hasSpecialTakeMoves()) {
			vector<int> takeMoves = piece.getTakeMoves();
			for (int i : takeMoves) {
				int newPos = pos + i;
				pieceInRange = newPos >= 0 && newPos < height * width;
				ChessPiece posPiece = (newPos == subPieceAt || !pieceInRange) ? subPiece : brd.at(newPos).getChessPiece();
				if (newPos >= 0 && newPos < height * width && posPiece.isActive()
					&& posPiece.getSide() != piece.getSide() && moveNotWrapped(pos, newPos) && (newPos != removePieceFrom) 
					&& !(verifyLegal && moveResultsInCheck(piece, newPos, pos))) {
					moves.insert(newPos);
				}
			}
		}
		return moves;
	}

};