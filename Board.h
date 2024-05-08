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

const int NONE_SELECTED = -1;
const int Y_OFFSET = 128;


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
		while (loc > 0 && moveNotWrapped(loc, prevLoc)) {
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
			bool whiteSquare = (i % 2 == 0 == i / h % 2);
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
	}

	bool selectTile(int pos, int move) {
		bool turnCompleted = false;
		if (selected == NONE_SELECTED) {
			ChessPiece& curPiece = brd.at(pos).getChessPiece();
			if (curPiece.isActive() && curPiece.getSide() == move) {
				selected = pos;
				currentValidMoves = (curPiece.useStrictMotion()) ? getStrictMoves(selected, curPiece) 
					: getPossibleMoves(selected, curPiece);
				toggleMoveHighlights(curPiece.getSide());
				brd.at(selected).toggleSelected();
			}
			
		}
		else {
			if (selected != pos && currentValidMoves.find(pos) != currentValidMoves.end()) {
				ChessPiece oldPiece = brd.at(pos).getChessPiece();
				brd.at(selected).movePiece(brd.at(pos));
				if (oldPiece.isActive()) {
					scores.at(move) += oldPiece.getValue();
					captures.at(move).push_back(oldPiece);
				}
				turnCompleted = true;
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
												
	set<int> getPossibleMoves(int pos, ChessPiece& piece) {
		set<int> allMoves;
		vector<bool> permissions = piece.getValidDirections();
		if (permissions.back()) {
			permissions.push_back(true);
		}
		string possibleMoves = "hvlr";
		for (int i = 0; i < permissions.size(); i++) {
			if (permissions.at(i)) {
				set<int> cur = getMoves(pos, piece.getRange(), piece.getSide(), possibleMoves.at(i));
				allMoves.insert(cur.begin(), cur.end());
			}
		}
		return allMoves;
	}

	set<int> getMoves(int pos, int range, int side, char method) {
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
			&& (!brd.at(bound1).getChessPiece().isOnSide(side) || bound1 == pos) 
			&& (!brd.at(prevLoc).getChessPiece().isActive() || prevLoc == pos)) {
			prevLoc = bound1;
			bound1 -= step;
		}

		prevLoc = pos;
		while (bound2 < max && (abs(bound2 - pos) <= rangeMax)
			&& (!brd.at(bound2).getChessPiece().isOnSide(side) || bound2 == pos)
			&& (!brd.at(prevLoc).getChessPiece().isActive() || prevLoc == pos)) {
			prevLoc = bound2;
			bound2 += step;
		}
		for (int i = bound1 + step; i < bound2; i += step) {
			if (i != pos) {
				validMoves.insert(i);
			}
		}
		return validMoves;
	}

	set<int> getStrictMoves(int pos, ChessPiece& piece) {
		set<int> moves;
		vector<int> moveList = piece.getStrictMoves();
		for (int i : moveList) {
			int newPos = pos + i;
			if (newPos >= 0 && newPos < height * width && (!brd.at(newPos).getChessPiece().isActive() 
				|| (brd.at(newPos).getChessPiece().getSide() != piece.getSide() && piece.canTakeStrictly()))
				&& moveNotWrapped(pos, newPos)) {
				moves.insert(newPos);
			}
		}
		if (piece.hasSpecialTakeMoves()) {
			vector<int> takeMoves = piece.getTakeMoves();
			for (int i : takeMoves) {
				int nPos = pos + i;
				if (nPos >= 0 && nPos < height * width && brd.at(nPos).getChessPiece().isActive()
					&& brd.at(nPos).getChessPiece().getSide() != piece.getSide() && moveNotWrapped(pos, nPos)) {
					moves.insert(nPos);
				}
			}
		}
		return moves;
	}

};