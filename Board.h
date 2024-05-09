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

	bool moveNotWrapped(int origPos, int newPos) {
		return abs(newPos % width - origPos % width) < width / 2;
	}

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
		for (int i = 0; i < brd.size(); i++) {
			target.draw(brd.at(i));
		}
		int mid = BOARD_DIM_IN_WINDOW / 2;
		for (int i = 0; i < captures.size(); i++) {
			vector<ChessPiece> cur = captures.at(i);
			for (int j = 0; j < cur.size(); j++) {
				ChessPiece piece = cur.at(j);
				sf::Sprite pieceSprite;
				sf::Texture texture = piece.getTexture();
				pieceSprite.setTexture(texture);
				pieceSprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
				pieceSprite.setScale(sf::Vector2f(DEFAULT_ITEM_SIZE * 0.7, DEFAULT_ITEM_SIZE * 0.7));
				pieceSprite.setPosition(mid + CELL_WIDTH/2 + Y_OFFSET/4 * (j % 7), Y_OFFSET/4 + (BOARD_DIM_IN_WINDOW + Y_OFFSET) * i + Y_OFFSET/4 * (j/7));
				target.draw(pieceSprite);
			}
		}
	}

	bool selectTile(int pos, int move) {
		bool turnCompleted = false;
		if (selected == NONE_SELECTED) {
			ChessPiece& curPiece = brd.at(pos).getChessPiece();
			if (curPiece.isActive() && curPiece.getSide() == move) {
				selected = pos;
				currentValidMoves = getPossibleMoves(selected, curPiece);
				toggleMoveHighlights(curPiece.getSide());
				brd.at(selected).toggleSelected();
			}
			
		}
		else {
			if (selected != pos && currentValidMoves.find(pos) != currentValidMoves.end()) {
				ChessPiece curPiece = brd.at(selected).getChessPiece();
				ChessPiece oldPiece = brd.at(pos).getChessPiece();
				brd.at(selected).movePiece(brd.at(pos));
				if (oldPiece.isActive()) {
					scores.at(move) += oldPiece.getValue();
					captures.at(move).push_back(oldPiece);
				}
				cout << "Check for " << curPiece.getSide() << boolalpha << ": " << check(curPiece.getSide()) << endl;
;				turnCompleted = true;
			}
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
												
	set<int> getPossibleMoves(int pos, ChessPiece& piece, ChessPiece subPiece = ChessPiece(), int repl = NONE_SELECTED, int rem = NONE_SELECTED, bool verifyLegal = true) {
		set<int> allMoves;
		if (piece.useStrictMotion()) {
			allMoves = getStrictMoves(pos, piece, verifyLegal, subPiece, repl, rem);
		}
		else {
			vector<bool> permissions = piece.getValidDirections();
			if (permissions.back()) {
				permissions.push_back(true);
			}
			string possibleMoves = "hvlr";
			for (int i = 0; i < permissions.size(); i++) {
				if (permissions.at(i)) {
					set<int> cur = getMoves(pos, piece.getRange(), piece.getSide(), possibleMoves.at(i), verifyLegal, subPiece, repl, rem);
					allMoves.insert(cur.begin(), cur.end());
				}
			}
		}
		return allMoves;
	}


	bool check(int sideFor, ChessPiece subPiece = ChessPiece(), int repl = NONE_SELECTED, int rem = NONE_SELECTED) {
		set<int> allMovesFor;
		int opposingKingPos = (subPiece.getName() == "king") * repl;
		for (int i = 0; i < brd.size(); i++) {
			ChessPiece cur = brd.at(i).getChessPiece();
			if (cur.isOnSide(sideFor)) {
				set<int> curMoves = getPossibleMoves(i, cur, subPiece, repl, rem, false);
				allMovesFor.insert(curMoves.begin(), curMoves.end());
			}
			else if (!(subPiece.getName() == "king") && cur.getName() == "king") {
				opposingKingPos = i;
			}
		}
		return allMovesFor.find(opposingKingPos) != allMovesFor.end();

	}

	set<int> getMoves(int pos, int range, int side, char method, bool verifyLegal, ChessPiece subPiece = ChessPiece(), int repl = NONE_SELECTED, int rem = NONE_SELECTED) {
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
			&& (!((bound1 == repl) ? subPiece : brd.at(bound1).getChessPiece()).isOnSide(side) || bound1 == rem || bound1 == pos)
			&& (!((prevLoc == repl) ? subPiece : brd.at(prevLoc).getChessPiece()).isActive() || prevLoc == rem || prevLoc == pos)) {
			prevLoc = bound1;
			bound1 -= step;
		}

		prevLoc = pos;
		while (bound2 < max && (abs(bound2 - pos) <= rangeMax)
			&& (!((bound2 == repl) ? subPiece : brd.at(bound2).getChessPiece()).isOnSide(side) || bound2 == rem || bound2 == pos)
			&& (!((prevLoc == repl) ? subPiece : brd.at(prevLoc).getChessPiece()).isActive() || prevLoc == rem || prevLoc == pos)) {
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

	bool moveResultsInCheck(ChessPiece movingPiece, int moveTo, int moveFrom) {
		return check(movingPiece.getSide() ^ 1, movingPiece, moveTo, moveFrom);
	}

	set<int> getStrictMoves(int pos, ChessPiece piece, bool verifyLegal = true, ChessPiece subPiece = ChessPiece(), int repl = NONE_SELECTED, int rem = NONE_SELECTED) {
		set<int> moves;
		bool pieceInRange;
		vector<int> moveList = piece.getStrictMoves();
		for (int i : moveList) {
			int newPos = pos + i;
			pieceInRange = newPos >= 0 && newPos < height * width;
			ChessPiece posPiece = (newPos == repl || !pieceInRange) ? subPiece : brd.at(newPos).getChessPiece();
			if (pieceInRange && (newPos == rem || !posPiece.isActive() 
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
				ChessPiece posPiece = (newPos == repl || !pieceInRange) ? subPiece : brd.at(newPos).getChessPiece();
				if (newPos >= 0 && newPos < height * width && posPiece.isActive()
					&& posPiece.getSide() != piece.getSide() && moveNotWrapped(pos, newPos) && (newPos != rem) 
					&& !(verifyLegal && moveResultsInCheck(piece, newPos, pos))) {
					moves.insert(newPos);
				}
			}
		}
		return moves;
	}

	bool checkMate(int sideFor) {

	}

};