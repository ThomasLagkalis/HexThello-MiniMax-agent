#ifndef MINIMAX_H
#define MINIMAX_H

#include "board.h"


#define MAX_DEPTH 4

Move getBestMove(Position pos, int player, int alpheBeta);

#endif


