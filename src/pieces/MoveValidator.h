#pragma once

#include "Board.h"
#include "Enums.h"
#include <set>

class MoveValidator {
private:
    Board* state;
    set<int> castleMoves;

    // Helper methods
    bool moveNotWrapped(int origPos, int newPos) {
        int width = state->getWidth();
        return abs(newPos % width - origPos % width) < width / 2;
    }

    // Get bounds for diagonals (either direction) for use when finding available moves
    int getDiagonalMin(int pos, bool left) {
        int width = state->getWidth();
        int height = state->getHeight();
        int loc = pos;
        int prevLoc = loc;
        while (loc >= 0 && moveNotWrapped(loc, prevLoc)) {
            prevLoc = loc;
            loc -= width + left - !left;
        }
        return loc;
    }

    int getDiagonalMax(int pos, bool left) {
        int width = state->getWidth();
        int height = state->getHeight();
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
        int width = state->getWidth();
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
        int width = state->getWidth();
        int height = state->getHeight();
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

    bool clearPathToRook(int pos, int step, int dist, PieceSide side) {
        bool clearPath = true;
        for (int i = pos + step; abs(pos - i) < dist && clearPath; i += step) {
            clearPath = !state->getCell(i).getChessPiece().isActive();
        }
        int end = pos + dist * step;
        ChessPiece& endPiece = state->getCell(end).getChessPiece();
        return clearPath && endPiece.isOnSide(side) && endPiece.getName() == "rook";
    }

    GameState checkForCheck(PieceSide sideFor, bool checkAll, ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        set<int> allMovesFor;
        GameState gameState = GameState::NONE;
        int opposingKingPos = (subPiece.isOfType(PieceType::KING) && subPieceAt != NONE_SELECTED) * subPieceAt;
        
        for (int i = 0; i < state->size(); i++) {
            ChessPiece& cur = state->getCell(i).getChessPiece();
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
            for (int i = 0; i < state->size() && noOpponentMoves; i++) {
                ChessPiece& cur = state->getCell(i).getChessPiece();
                noOpponentMoves = !cur.isOnSide(getOpposingSide(sideFor)) || getPossibleMoves(i, cur, true, subPiece, subPieceAt, removePieceFrom).empty();
            }
            gameState = (noOpponentMoves) ? ((gameState == GameState::CHECK) ? GameState::CHECKMATE : GameState::STALEMATE) : gameState;
        }
        
        return gameState;
    }

public:
    MoveValidator(Board* state) : state(state) {
        castleMoves = { NONE_SELECTED, NONE_SELECTED };
    }

    // Check if a move would result in check
    bool moveResultsInCheck(ChessPiece& movingPiece, int moveTo, int moveFrom) {
        GameState moveState = checkForCheck(getOpposingSide(movingPiece.getSide()), false, movingPiece, moveTo, moveFrom);
        return moveState == GameState::CHECK || moveState == GameState::CHECKMATE;
    }

    // Get all valid moves for a piece
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

    // Get moves for pieces with standard movement patterns
    set<int> getMoves(int pos, int range, PieceSide side, MoveDirection pattern, bool verifyLegal, ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        pair<int, int> bounds = getMovePatternBounds(pos, pattern);
        int step = getMovePatternStep(pattern);
        
        set<int> validMoves;
        int bound1 = pos, bound2 = pos;
        int rangeMax = range * step;
        int prevLoc = pos;

        while (bound1 > bounds.first && (abs(bound1 - pos) <= rangeMax)
            && (!((bound1 == subPieceAt) ? subPiece : state->getCell(bound1).getChessPiece()).isOnSide(side) || bound1 == removePieceFrom || bound1 == pos)
            && (!((prevLoc == subPieceAt) ? subPiece : state->getCell(prevLoc).getChessPiece()).isActive() || prevLoc == removePieceFrom || prevLoc == pos)) {
            prevLoc = bound1;
            bound1 -= step;
        }

        prevLoc = pos;
        while (bound2 < bounds.second && (abs(bound2 - pos) <= rangeMax)
            && (!((bound2 == subPieceAt) ? subPiece : state->getCell(bound2).getChessPiece()).isOnSide(side) || bound2 == removePieceFrom || bound2 == pos)
            && (!((prevLoc == subPieceAt) ? subPiece : state->getCell(prevLoc).getChessPiece()).isActive() || prevLoc == removePieceFrom || prevLoc == pos)) {
            prevLoc = bound2;
            bound2 += step;
        }
        for (int i = bound1 + step; i < bound2; i += step) {
            if (i != pos && !(verifyLegal && moveResultsInCheck(state->getCell(pos).getChessPiece(), i, pos))) {
                validMoves.insert(i);
            }
        }
        return validMoves;
    }

    // Get moves for pieces with special movement patterns (knights, pawns)
    set<int> getStrictMoves(int pos, ChessPiece& piece, bool verifyLegal, ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        int width = state->getWidth();
        int height = state->getHeight();
        set<int> moves;
        bool pieceInRange, continueChecking = true;
        vector<int> moveList = piece.getStrictMoves();
        
        for (int i = 0; i < moveList.size() && continueChecking; i++) {
            int newPos = pos + moveList.at(i);
            pieceInRange = newPos >= 0 && newPos < height * width;
            ChessPiece& posPiece = (newPos == subPieceAt || !pieceInRange) ? subPiece : state->getCell(newPos).getChessPiece();
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
                ChessPiece& posPiece = (newPos == subPieceAt || !pieceInRange) ? subPiece : state->getCell(newPos).getChessPiece();
                if (newPos >= 0 && newPos < height * width && posPiece.isActive()
                    && posPiece.getSide() != piece.getSide() && moveNotWrapped(pos, newPos) && (newPos != removePieceFrom) 
                    && !(verifyLegal && moveResultsInCheck(piece, newPos, pos))) {
                    moves.insert(newPos);
                }
            }
        }
        
        // En passant logic
        if (piece.isOfType(PieceType::PAWN)) {
            vector<int> neighbors = getNeighbors(pos);
            for (int i : neighbors) {
                ChessPiece& neighbor = state->getCell(i).getChessPiece();
                if (neighbor.getSide() != piece.getSide() && neighbor.canBeEnPassanted(0)) { // curMoveNum needs to be handled externally
                    int idx = i - width + 2 * width * (neighbor.getSide() == PieceSide::BLACK);
                    moves.insert(idx);
                    state->setEnPassantMove(idx);
                }
            }
        }
        
        return moves;
    }

    vector<int> getNeighbors(int pos) {
        int width = state->getWidth();
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
        ChessPiece& king = state->getCell(kingPos).getChessPiece();
        vector<int> distances{ 3, 4 };
        bool canCastle = king.canCastle();
        bool inCheck = checkForCheck(getOpposingSide(king.getSide()), false, king) == GameState::CHECK;
        castleMoves.erase(castleMoves.begin(), castleMoves.end());
        for (int i = 0; i < distances.size(); i++) {
            if (canCastle && !inCheck && clearPathToRook(kingPos, -2 * i + 1, distances.at(i), king.getSide())) {	
                castleMoves.insert(kingPos + 2 - 4*i);
            }
            else {
                castleMoves.insert(NONE_SELECTED);
            }
        }
    }

    set<int> getCastleMoves() const { return castleMoves; }

    GameState check(PieceSide sideFor, bool checkAll, ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        return checkForCheck(sideFor, checkAll, subPiece, subPieceAt, removePieceFrom);
    }

    bool shouldPromote(ChessPiece& piece, int pos) {
        int height = state->getHeight();
        // Verify that the piece is a pawn and is at the correct location for its side
        return piece.isOfType(PieceType::PAWN) && ((piece.getSide() == PieceSide::WHITE && (pos / height) == height - 1) ||
            (piece.getSide() == PieceSide::BLACK && !(pos / height)));
    }
};
