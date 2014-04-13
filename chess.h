#include <pthread.h>

extern char board[65];
extern pthread_mutex_t board_m;
extern pthread_mutex_t to_move_m;
void set_board();
int move(int, int, char);
int move(char move[4]);
void *get_moves(void *);
void *load_engine(void *);
