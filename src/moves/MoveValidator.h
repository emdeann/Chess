#pragma once

#include "../board/Board.h"
#include "../constants/Enums.h"
#include <set>

class MoveValidator {
private:
    set<int> castleMoves;

    // Helper methods
    bool moveNotWrapped(const Board& board, int origPos, int newPos) {
        int width = board.getWidth();
        return abs(newPos % width - origPos % width) < width / 2;
    }

    // Get bounds for diagonals (either direction) for use when finding available moves
    int getDiagonalMin(const Board& board, int pos, bool left) {
        int width = board.getWidth();
        int height = board.getHeight();
        int loc = pos;
        int prevLoc = loc;
        while (loc >= 0 && moveNotWrapped(board, loc, prevLoc)) {
            prevLoc = loc;
            loc -= width + left - !left;
        }
        return loc;
    }

    int getDiagonalMax(const Board& board, int pos, bool left) {
        int width = board.getWidth();
        int height = board.getHeight();
        int loc = pos;
        int prevLoc = loc;
        while (loc < width * height && moveNotWrapped(board, loc, prevLoc)) {
            prevLoc = loc;
            loc += width + left - !left;
        }
        return loc;
    }

    PieceSide getOpposingSide(PieceSide side) {
        return (side == PieceSide::WHITE) ? PieceSide::BLACK : PieceSide::WHITE;
    }

    // New helper method to convert MovePattern to step value
    int getMovePatternStep(const Board& board, MoveDirection pattern) const {
        int width = board.getWidth();
        switch (pattern) {
            case MoveDirection::HORIZONTAL: return 1;
            case MoveDirection::VERTICAL: return width;
            case MoveDirection::DIAGONAL_LEFT: return width + 1;
            case MoveDirection::DIAGONAL_RIGHT: return width - 1;
            default: return 0;
        }
    }

    pair<int, int> getMovePatternBounds(const Board& board, int pos, MoveDirection pattern) {
        int width = board.getWidth();
        int height = board.getHeight();
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
                min = getDiagonalMin(board, pos, true);
                max = getDiagonalMax(board, pos, true);
                break;
            case MoveDirection::DIAGONAL_RIGHT:
                min = getDiagonalMin(board, pos, false);
                max = getDiagonalMax(board, pos, false);
                break;
        }
        return {min, max};
    }

    bool clearPathToRook(const Board& board, int pos, int step, int dist, PieceSide side) {
        bool clearPath = true;
        for (int i = pos + step; abs(pos - i) < dist && clearPath; i += step) {
            clearPath = !board.getCell(i).getChessPiece().isActive();
        }
        int end = pos + dist * step;
        const ChessPiece& endPiece = board.getCell(end).getChessPiece();
        return clearPath && endPiece.isOnSide(side) && endPiece.getName() == "rook";
    }

    GameState checkForCheck(const Board& board, PieceSide sideFor, bool checkAll, const ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        set<int> allMovesFor;
        GameState gameState = GameState::NONE;
        int opposingKingPos = (subPiece.isOfType(PieceType::KING) && subPieceAt != NONE_SELECTED) * subPieceAt;
        
        for (int i = 0; i < board.size(); i++) {
            const ChessPiece& cur = board.getCell(i).getChessPiece();
            if (cur.isOnSide(sideFor) && i != subPieceAt) {
                set<int> curMoves = getPossibleMoves(board, i, cur, false, subPiece, subPieceAt, removePieceFrom);
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
            for (int i = 0; i < board.size() && noOpponentMoves; i++) {
                const ChessPiece& cur = board.getCell(i).getChessPiece();
                noOpponentMoves = !cur.isOnSide(getOpposingSide(sideFor)) || getPossibleMoves(board, i, cur, true, subPiece, subPieceAt, removePieceFrom).empty();
            }
            gameState = (noOpponentMoves) ? ((gameState == GameState::CHECK) ? GameState::CHECKMATE : GameState::STALEMATE) : gameState;
        }
        
        return gameState;
    }

public:
    MoveValidator() {
        castleMoves = { NONE_SELECTED, NONE_SELECTED };
    }

    // Check if a move would result in check
    bool moveResultsInCheck(const Board& board, const ChessPiece& movingPiece, int moveTo, int moveFrom) {
        GameState moveState = checkForCheck(board, getOpposingSide(movingPiece.getSide()), false, movingPiece, moveTo, moveFrom);
        return moveState == GameState::CHECK || moveState == GameState::CHECKMATE;
    }

    // Get all valid moves for a piece
    set<int> getPossibleMoves(const Board& board, int pos, const ChessPiece& piece, bool verifyLegal, const ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        set<int> allMoves;
        if (piece.useStrictMotion()) {
            allMoves = getStrictMoves(board, pos, piece, verifyLegal, subPiece, subPieceAt, removePieceFrom);
        }
        else {
            vector<bool> permissions = piece.getValidDirections();
            if (permissions.back()) {
                permissions.push_back(true);
            }
            vector<MoveDirection> possibleMoves = { MoveDirection::HORIZONTAL, MoveDirection::VERTICAL, MoveDirection::DIAGONAL_LEFT, MoveDirection::DIAGONAL_RIGHT };
            for (int i = 0; i < permissions.size(); i++) {
                if (permissions.at(i)) {
                    set<int> cur = getMoves(board, pos, piece.getRange(), piece.getSide(), possibleMoves.at(i), verifyLegal, subPiece, subPieceAt, removePieceFrom);
                    allMoves.insert(cur.begin(), cur.end());
                }
            }
        }
        if (piece.isOfType(PieceType::KING) && verifyLegal) {
            setCastleMoves(board, pos);
            for (int i : castleMoves) {
                if (i != NONE_SELECTED) {
                    allMoves.insert(i);
                }
            }
        }
        return allMoves;
    }

    // Get moves for pieces with standard movement patterns
    set<int> getMoves(const Board& board, int pos, int range, PieceSide side, MoveDirection pattern, bool verifyLegal, const ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        pair<int, int> bounds = getMovePatternBounds(board, pos, pattern);
        int step = getMovePatternStep(board, pattern);
        
        set<int> validMoves;
        int bound1 = pos, bound2 = pos;
        int rangeMax = range * step;
        int prevLoc = pos;

        while (bound1 > bounds.first && (abs(bound1 - pos) <= rangeMax)
            && (!((bound1 == subPieceAt) ? subPiece : board.getCell(bound1).getChessPiece()).isOnSide(side) || bound1 == removePieceFrom || bound1 == pos)
            && (!((prevLoc == subPieceAt) ? subPiece : board.getCell(prevLoc).getChessPiece()).isActive() || prevLoc == removePieceFrom || prevLoc == pos)) {
            prevLoc = bound1;
            bound1 -= step;
        }

        prevLoc = pos;
        while (bound2 < bounds.second && (abs(bound2 - pos) <= rangeMax)
            && (!((bound2 == subPieceAt) ? subPiece : board.getCell(bound2).getChessPiece()).isOnSide(side) || bound2 == removePieceFrom || bound2 == pos)
            && (!((prevLoc == subPieceAt) ? subPiece : board.getCell(prevLoc).getChessPiece()).isActive() || prevLoc == removePieceFrom || prevLoc == pos)) {
            prevLoc = bound2;
            bound2 += step;
        }
        for (int i = bound1 + step; i < bound2; i += step) {
            if (i != pos && !(verifyLegal && moveResultsInCheck(board, board.getCell(pos).getChessPiece(), i, pos))) {
                validMoves.insert(i);
            }
        }
        return validMoves;
    }

    // Get moves for pieces with special movement patterns (knights, pawns)
    set<int> getStrictMoves(const Board& board, int pos, const ChessPiece& piece, bool verifyLegal, const ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        int width = board.getWidth();
        int height = board.getHeight();
        set<int> moves;
        bool pieceInRange, continueChecking = true;
        vector<int> moveList = piece.getStrictMoves();
        
        for (int i = 0; i < moveList.size() && continueChecking; i++) {
            int newPos = pos + moveList.at(i);
            pieceInRange = newPos >= 0 && newPos < height * width;
            const ChessPiece& posPiece = (newPos == subPieceAt || !pieceInRange) ? subPiece : board.getCell(newPos).getChessPiece();
            if (pieceInRange && (newPos == removePieceFrom || !posPiece.isActive() 
                || (posPiece.getSide() != piece.getSide() && piece.canTakeStrictly()))
                && moveNotWrapped(board, pos, newPos) && !(verifyLegal && moveResultsInCheck(board, piece, newPos, pos))) {
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
                const ChessPiece& posPiece = (newPos == subPieceAt || !pieceInRange) ? subPiece : board.getCell(newPos).getChessPiece();
                if (newPos >= 0 && newPos < height * width && posPiece.isActive()
                    && posPiece.getSide() != piece.getSide() && moveNotWrapped(board, pos, newPos) && (newPos != removePieceFrom)
                    && !(verifyLegal && moveResultsInCheck(board, piece, newPos, pos))) {
                    moves.insert(newPos);
                }
            }
        }
        
        // TODO: Re-add en-passant in a different location
        
        return moves;
    }

    vector<int> getNeighbors(const Board& board, int pos) {
        int width = board.getWidth();
        vector<int> neighbors;
        for (int i = 0; i < 2; i++) {
            int adj = pos - 1 + 2 * i;
            if ((!i && pos % width != 0) || (i && pos % width != width - 1)) {
                neighbors.push_back(adj);
            }
        }
        return neighbors;
    }

    void setCastleMoves(const Board& board, int kingPos) {
        const ChessPiece& king = board.getCell(kingPos).getChessPiece();
        vector<int> distances{ 3, 4 };
        bool canCastle = king.canCastle();
        bool inCheck = checkForCheck(board, getOpposingSide(king.getSide()), false, king) == GameState::CHECK;
        castleMoves.erase(castleMoves.begin(), castleMoves.end());
        for (int i = 0; i < distances.size(); i++) {
            if (canCastle && !inCheck && clearPathToRook(board, kingPos, -2 * i + 1, distances.at(i), king.getSide())) {	
                castleMoves.insert(kingPos + 2 - 4*i);
            }
            else {
                castleMoves.insert(NONE_SELECTED);
            }
        }
    }

    set<int> getCastleMoves() const { return castleMoves; }

    GameState check(const Board& board, PieceSide sideFor, bool checkAll, const ChessPiece& subPiece, int subPieceAt = NONE_SELECTED, int removePieceFrom = NONE_SELECTED) {
        return checkForCheck(board, sideFor, checkAll, subPiece, subPieceAt, removePieceFrom);
    }

    bool shouldPromote(const Board& board, const ChessPiece& piece, int pos) {
        int height = board.getHeight();
        // Verify that the piece is a pawn and is at the correct location for its side
        return piece.isOfType(PieceType::PAWN) && ((piece.getSide() == PieceSide::WHITE && (pos / height) == height - 1) ||
            (piece.getSide() == PieceSide::BLACK && !(pos / height)));
    }
};
