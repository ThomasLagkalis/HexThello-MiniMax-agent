#include "minimax.h"
#include "move.h"
#include "global.h"


int get_actions(Position * pos ){
	if (canMove(pos, pos->turn)){
		return TRUE;
	}else {
		return FALSE;
	}
}
