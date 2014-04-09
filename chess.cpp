#include "chess.h"
#include <string.h>
#include <stdio.h>

char board[65];

void set_board();

void set_board(){
    strncpy(board, "rnbqkbnrpppppppp--------------------------------PPPPPPPPRNBQKBNR", 65);
}
