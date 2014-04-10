#include "chess.h"
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "gui.h"

char board[65];

pthread_mutex_t board_m; // mutex on board (the char array);

void set_board(){
    pthread_mutex_lock(&board_m); 
    strncpy(board, "rnbqkbnrpppppppp--------------------------------PPPPPPPPRNBQKBNR", 65);
    pthread_mutex_unlock(&board_m); 
}


void move(int src, int dest){
    if(src < 0 || src > 64 || dest < 0 || dest > 64)
        fprintf(stderr, "Move out of Range\n");
    else{
        pthread_mutex_lock(&board_m); 
        board[dest] = board[src];
        board[src] = '-';
        pthread_mutex_unlock(&board_m); 
    }
}

void *get_moves(void *){
    char in[20];
    while(1){
        printf("Enter move: ");
        gets(in);
        int src = (8 - in[1] + '0') * 8 + (in[0] - 'a');
        int dest = (8 - in[3] + '0') * 8 + (in[2] - 'a');
        printf("src: %d, dest: %d\n", src, dest);
        move(src, dest);
        printf("board: %s\n", board);
    }
}
