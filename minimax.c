#include "minimax.h"
#include "move.h"
#include "global.h"
#include <stdio.h>


int isFrontier(Position pos, int x, int y) {
	/*
	 * Helper function for evaluation determines if in current state (pos)
	 * cell (x,y) is a frontier cell i.e. there is an empty neighbor cell 
	 * empty, so a piece in this cell could be captured, therefore is a 
	 * liability.
	 */
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
	/* 
	 *	Evaluation function of current state (pos). 
	 *	
	 *	f(state) = W_diff * (num_of_white - num_of_black) + W_validMoves * (valid_moves_white - valid_moves_black)
	 *				- W_frontier * (num_of_frontier_white - num_of_frontier_black)
	 *
	 *	Evaluation heuristics: 
	 *		+ Numner of pieces of each player (the more the better)
	 *		+ Numbera of valid moves of each player (the more the better)
	 *		+ Number of pieces near the border (these pieces can't be captured, so the more the better)
	 *		- Number of pieces that could be captured (frontier pieces, these are liability, the less the better() 
	 *	
	 *	returns an int (f(state)).
	 */
	//printf("[DEBUG] Evaluation.\n");
	int pieces_diff = 0, valid_moves = 0, border_control = 0, frontier_pieces = 0, val=0;
	Move cur_move;
	for (int i =0; i<ARRAY_BOARD_SIZE; i++){
		// Check pieces controling the border of the board.
		if (pos.board[0][i] == WHITE || pos.board[i][0] == WHITE ||
			pos.board[ARRAY_BOARD_SIZE-1][i] == WHITE || pos.board[i][ARRAY_BOARD_SIZE-1] == WHITE){
			border_control++;
		}
		else if (pos.board[0][i] == BLACK || pos.board[i][0] == BLACK ||
			pos.board[ARRAY_BOARD_SIZE-1][i] == BLACK || pos.board[i][ARRAY_BOARD_SIZE-1] == BLACK){
			
			border_control--;
		}
		for (int j =0; j<ARRAY_BOARD_SIZE; j++){
			cur_move.tile[0] = i;
			cur_move.tile[1] = j;

			// Compute the difference in pieces of each playes.
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
	// Assign weight to each heuristic.
	val = 10 * pieces_diff + 8 * valid_moves + 3 * border_control - 2 * frontier_pieces;
	//printf("\nEval.: %d", val);
	return val;
	
}


int minimax(Position pos, int depth, int is_max){
	// If we reached the maximux depth return the evaluation of the board.
	if (depth == MAX_DEPTH || !canMove(&pos, !is_max)) return evaluate(pos);

	int best = is_max ? -1000000 : 1000000;
	int score;
	Move cur_move;
	Position new_pos;
	for (int i =0; i<ARRAY_BOARD_SIZE; i++){
		for (int j =0; j<ARRAY_BOARD_SIZE; j++){
			cur_move.tile[0] = i;
			cur_move.tile[1] = j;
			cur_move.color = !is_max;
			if (pos.board[i][j] == EMPTY && isLegalMove(&pos, &cur_move)){
				doMove(&new_pos, &cur_move); // Simulate move

				score = minimax(new_pos, depth+1, !is_max);	

				// Undo the move
				new_pos = pos; 				

				if (is_max){
					if(score>best) best = score;
				}
                else{
					if (score<best) best = score;
				}
			}
		}
	}
	
	return best;
}


int minimax_alphabeta(Position pos, int depth, int is_max, int alpha, int beta) {
    // If we reached the maximum depth or there are no legal moves, evaluate the board.
    if (depth == MAX_DEPTH || !canMove(&pos, !is_max))
        return evaluate(pos);

    int best = is_max ? -1000000 : 1000000;
    int score;
    Move cur_move;
    Position new_pos;

    // Iterate over the board positions
    for (int i = 0; i < ARRAY_BOARD_SIZE; i++) {
        for (int j = 0; j < ARRAY_BOARD_SIZE; j++) {
            cur_move.tile[0] = i;
            cur_move.tile[1] = j;
			cur_move.color = !is_max;
            // Check if the cell is empty and the move is legal
            if (pos.board[i][j] == EMPTY && isLegalMove(&pos, &cur_move)) {
                doMove(&new_pos, &cur_move); // Simulate move

                // Recurse with the new position, increased depth and flipped player flag.
                score = minimax_alphabeta(new_pos, depth + 1, !is_max, alpha, beta);

                // Undo the move by restoring the position.
                new_pos = pos;

                if (is_max) {
                    // Maximizer: select the maximum score
                    if (score > best)
                        best = score;
                    if (best > alpha)
                        alpha = best;

                    // Beta cut-off: if alpha is at least beta, prune the branch.
                    if (alpha >= beta)
                        return best;  // Prune: no need to search further in this branch
                } else {
                    // Minimizer: select the minimum score
                    if (score < best)
                        best = score;
                    if (best < beta)
                        beta = best;

                    // Alpha cut-off: if alpha is at least beta, prune the branch.
                    if (alpha >= beta)
                        return best;  // Prune: no need to search further in this branch
                }
            }
        }
    }

    return best;
}



Move getBestMove(Position pos, int player, int alphaBeta){
	Move bestMove;
	bestMove.tile[0] = NULL_MOVE;
	bestMove.tile[1] = NULL_MOVE;
	bestMove.color = player;
	
	int best = (player==WHITE) ? -1000000 : 1000000;
	int score;
	Move cur_move;
	Position new_pos;

	if (!canMove(&pos, player)) return bestMove; // Returns -1 -1 move which corresponds to NULL_MOVE.
	
	for (int i =0; i<ARRAY_BOARD_SIZE; i++){
		for (int j =0; j<ARRAY_BOARD_SIZE; j++){
			cur_move.tile[0] = i;
			cur_move.tile[1] = j;
			cur_move.color = player;
			if (pos.board[i][j] == EMPTY && isLegalMove(&pos, &cur_move)){
				doMove(&new_pos, &cur_move); // Simulate move
				
				if (alphaBeta)
					score = minimax_alphabeta(new_pos, 0, player==WHITE, -100000, 100000);
				else 
					score = minimax(new_pos, 0, player==WHITE);	

				// Undo the move
				new_pos = pos; 				
				//printf("DEBUG 1, score: %d, best score: %d, player: %d, Best Move: (%d, %d)\n", score, best, player, bestMove.tile[0], bestMove.tile[1]);
				if ((player == WHITE && score > best) || (player == BLACK && score < best)) {
                    //printf("DEBUG 2");
					best = score;
                    bestMove.tile[0] = cur_move.tile[0];
                    bestMove.tile[1] = cur_move.tile[1];
                }
			}
		}
	}
	return bestMove;
	
}
