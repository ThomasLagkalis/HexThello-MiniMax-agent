#include "minimax.h"
#include "move.h"
#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INF 100000

// A helper struct to store a move and its static evaluation.
typedef struct {
    Move move;
    int eval;
} MoveScore;

// Comparison function for maximizing player (descending order).
int cmpMax(const void *a, const void *b) {
    MoveScore *ma = (MoveScore *)a;
    MoveScore *mb = (MoveScore *)b;
    return mb->eval - ma->eval;  // high eval first
}

// Comparison function for minimizing player (ascending order).
int cmpMin(const void *a, const void *b) {
    MoveScore *ma = (MoveScore *)a;
    MoveScore *mb = (MoveScore *)b;
    return ma->eval - mb->eval;  // low eval first
}

int isFrontier(Position pos, int x, int y) {
    // If the tile itself is empty, it's not a frontier piece.
    if (pos.board[x][y] == EMPTY) return 0;

    // Define the 6 possible directions in a hexagonal grid
    int dx[] = {-1, -1, 0, 0, 1, 1};
    int dy[] = {-1, 0, -1, 1, 0, 1};

    for (int d = 0; d < 6; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];

        // Ensure we're inside the board limits
        if (nx >= 0 && nx < ARRAY_BOARD_SIZE && ny >= 0 && ny < ARRAY_BOARD_SIZE) {
            if (pos.board[nx][ny] == EMPTY) {
                return 1; // It's a frontier piece
            }
        }
    }
    return 0; // Not a frontier piece
}

int evaluate(Position pos){
    int pieces_diff = 0, valid_moves = 0, border_control = 0, frontier_pieces = 0, val = 0;
    Move cur_move;
    for (int i = 0; i < ARRAY_BOARD_SIZE; i++){
        // Check pieces controlling the border of the board.
        if (pos.board[0][i] == WHITE || pos.board[i][0] == WHITE ||
            pos.board[ARRAY_BOARD_SIZE-1][i] == WHITE || pos.board[i][ARRAY_BOARD_SIZE-1] == WHITE){
            border_control++;
        }
        else if (pos.board[0][i] == BLACK || pos.board[i][0] == BLACK ||
                 pos.board[ARRAY_BOARD_SIZE-1][i] == BLACK || pos.board[i][ARRAY_BOARD_SIZE-1] == BLACK){
            border_control--;
        }
        for (int j = 0; j < ARRAY_BOARD_SIZE; j++){
            cur_move.tile[0] = i;
            cur_move.tile[1] = j;

            // Compute the difference in pieces for each player.
            if (pos.board[i][j] == WHITE) {
                frontier_pieces += isFrontier(pos, i, j);
                pieces_diff++;
            }
            else if (pos.board[i][j] == BLACK){
                frontier_pieces -= isFrontier(pos, i, j);
                pieces_diff--;
            }

            // Check number of valid moves for each player.
            cur_move.color = WHITE;
            if (isLegalMove(&pos, &cur_move)) valid_moves++;
            cur_move.color = BLACK;
            if (isLegalMove(&pos, &cur_move)) valid_moves--;
        }
    }
    // Assign weights to each heuristic.
    val = 10 * pieces_diff + 8 * valid_moves + 3 * border_control - 2 * frontier_pieces;
    return val;
}


int minimax(Position pos, int depth, int is_max, int alpha_beta, int alpha, int beta) {
    // Terminal condition: maximum depth or no legal moves for the current player.
    if (depth == MAX_DEPTH || !canMove(&pos, !is_max))
        return evaluate(pos);

    int best = is_max ? -INF : INF;
    int score;
    Move cur_move;
    Position new_pos;

    // First, gather all legal moves for this node.
    // We allocate an array to hold moves. Worst case: all squares are legal.
    MoveScore moves[ARRAY_BOARD_SIZE * ARRAY_BOARD_SIZE];
    int moveCount = 0;

    for (int i = 0; i < ARRAY_BOARD_SIZE; i++) {
        for (int j = 0; j < ARRAY_BOARD_SIZE; j++) {
            if (pos.board[i][j] == EMPTY) {
                cur_move.tile[0] = i;
                cur_move.tile[1] = j;
                cur_move.color = is_max ? WHITE : BLACK;
                if (isLegalMove(&pos, &cur_move)) {
                    // Create a copy of pos and apply the move.
                    memcpy(&new_pos, &pos, sizeof(Position));
                    doMove(&new_pos, &cur_move);
                    // Store the move and its evaluation (static ordering).
                    moves[moveCount].move = cur_move;
                    moves[moveCount].eval = evaluate(new_pos);
                    moveCount++;
                }
            }
        }
    }

    // Sort moves so that the best ones are expanded first.
    if (is_max)
        qsort(moves, moveCount, sizeof(MoveScore), cmpMax);
    else
        qsort(moves, moveCount, sizeof(MoveScore), cmpMin);

    // Now, iterate over the sorted moves.
    for (int m = 0; m < moveCount; m++) {
        cur_move = moves[m].move;
        memcpy(&new_pos, &pos, sizeof(Position));
        doMove(&new_pos, &cur_move);

        score = minimax(new_pos, depth + 1, !is_max, alpha_beta, alpha, beta);

        if (is_max) {
            if (score > best)
                best = score;
            if (alpha_beta && best >= beta)
                return best; // Beta cutoff.
            if (best > alpha)
                alpha = best;
        } else {
            if (score < best)
                best = score;
            if (alpha_beta && best <= alpha)
                return best; // Alpha cutoff.
            if (best < beta)
                beta = best;
        }
    }

    return best;
}

Move getBestMove(Position pos, int player, int alphaBeta) {
    Move bestMove;
    bestMove.tile[0] = NULL_MOVE;
    bestMove.tile[1] = NULL_MOVE;
    bestMove.color = player;

    int best = (player == WHITE) ? -INF : INF;
    int score;
    Move cur_move;
    Position new_pos;

    // If no legal moves, return the NULL_MOVE.
    if (!canMove(&pos, player))
        return bestMove;

    // Iterate over all legal moves (you could also apply move ordering here too)
    for (int i = 0; i < ARRAY_BOARD_SIZE; i++) {
        for (int j = 0; j < ARRAY_BOARD_SIZE; j++) {
            cur_move.tile[0] = i;
            cur_move.tile[1] = j;
            cur_move.color = player;
            if (pos.board[i][j] == EMPTY && isLegalMove(&pos, &cur_move)) {
                memcpy(&new_pos, &pos, sizeof(Position));
                doMove(&new_pos, &cur_move); // Simulate move

                if (alphaBeta)
                    score = minimax(new_pos, 0, player == WHITE, TRUE, -INF, INF);
                else
                    score = minimax(new_pos, 0, player == WHITE, FALSE, -INF, INF);

                if ((player == WHITE && score > best) || (player == BLACK && score < best)) {
                    best = score;
                    bestMove.tile[0] = cur_move.tile[0];
                    bestMove.tile[1] = cur_move.tile[1];
                }
            }
        }
    }
    return bestMove;
}

