#include <pthread.h>

extern char board[65];
extern pthread_mutex_t board_m;
void set_board();
void move(int, int);
void move(char move[4]);
void *get_moves(void *);
void *load_engine(void *);
