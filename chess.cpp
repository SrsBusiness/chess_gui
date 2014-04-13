#include "chess.h"
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "gui.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string>
#include <stdlib.h>

#define WHITE 0x20
#define LEVEL 20

using namespace std;

char board[65];
string moves;
pthread_mutex_t board_m; // mutex on board (the char array);

int to_move = 0;
pthread_mutex_t to_move_m;

void set_board(){
    pthread_mutex_lock(&board_m); 
    strncpy(board, "rnbqkbnrpppppppp--------------------------------PPPPPPPPRNBQKBNR", 65);
    pthread_mutex_unlock(&board_m); 
}


int move(int src, int dest, char promote){
    if(src < 0 || src > 64 || dest < 0 || dest > 64){
        fprintf(stderr, "Move out of Range\n");
        return -1;
    }
    if(board[src] == '-'){
        fprintf(stderr, "Moving nonexistant piece\n");
        return -1;
    }
    else{
        pthread_mutex_lock(&board_m); 
        if(board[src] == 'K'){
            if(src == 60 && dest == 62){
                board[61] = board[63];
                board[63] = '-';
            }else if(src == 60 && dest == 58){
                board[59] = board[56];
                board[56] = '-';
            }

        }
        if(board[src] == 'k'){
            if(src == 4 && dest == 6){
                board[5] = board[7];
                board[7] = '-';
            }else if(src == 4 && dest == 2){
                board[3] = board[0];
                board[0] = '-';
            }
        }
        board[dest] = board[src];
        if(promote){
            if(board[dest] & WHITE){
                board[dest] = promote;
            }else
                board[dest] = promote ^ WHITE;
        }
        board[src] = '-';
        pthread_mutex_unlock(&board_m);
        return 0;
    }
}

int move(char m[5]){
    //printf("move: %s", m);
    int a, b, c, d;
    //printf("Move(): move: %s\n", m);
    //printf("m[0]: %d, m[1]: %d, m[2]: %d, m[3]: %d\n", m[0], m[1], m[2], m[3]);
    //sscanf(m, "%c%d%c%d", &a, &b, &c, &d);
    //printf("a: %d, b: %d, c: %d, d: %d\n", a, b, c, d);
    int src = (8 - m[1] + '0') * 8 + (m[0] - 'a');
    int dest = (8 - m[3] + '0') * 8 + (m[2] - 'a');
    char promote = 0;
    if(m[4] == 'q' || m[4] == 'b' || m[4] == 'n' || m[4] == 'r')
        promote = m[4];
    //printf("promote: %d, move[4]: %d\n", promote, m[4]);
    //printf("src: %d, dest: %d\n", src, dest);
    return move(src, dest, promote);
}

void *get_moves(void *){
    char in[20];
    while(1){
        pthread_mutex_lock(&to_move_m);
        if(!to_move){ 
            printf("Enter move: ");
            fgets(in, 20, stdin);
            strtok(in, "\n");
            //printf("in %s\n", in);
            //printf("got string\n");
            if(!strncmp(in, "\n", 1)){
                //printf("Passed\n");
                to_move = 1;
            }else{
                if(!move(in)){
                    //printf("herp\n");
                    moves.append(in);
                    moves.append(" ");
                    to_move = 1;
                }
                else
                    printf("Illegal Move\n");
                //printf("t0_move %d\n", to_move);
                //fgets(in, 5, stdin);
            }
            //printf("exit, to_move: %d\n", to_move);
        }
        pthread_mutex_unlock(&to_move_m);
        /*
           int a, b, c, d;
           sscanf(in, "%c%d%c%d", &a, &b, &c, &d);
           printf("a: %d, b: %d, c: %d, d: %d\n", a, b, c, d);
           int src = (8 - b) * 8 + (a - 'a');
           int dest = (8 - d) * 8 + (c - 'a');
           printf("src: %d, dest: %d\n", src, dest);
           move(src, dest);
           printf("board: %s\n", board);
           */
    }
}

void *load_engine(void *){
    int pin_cout[2]; // parent reads, child writes
    int pout_cin[2]; // parent writes, child reads
    if(pipe(pin_cout)){
        fprintf(stderr, "Failed to create pipe\n");
        return NULL;
    }
    if(pipe(pout_cin)){
        fprintf(stderr, "Failed to create pipe\n");
        return NULL;
    }
    int pid = fork();
    if(pid < 0){
        fprintf(stderr, "Fork Failed\n");
        return NULL;
    }else if (pid){ //parent
        close(pin_cout[1]);
        close(pout_cin[0]);
        //fcntl(pout_cin[1], F_SETFD, O_FSYNC);
        char buf[512];
        FILE *in = fdopen(pin_cout[0], "r"); 
        FILE *out = fdopen(pout_cin[1], "w");
        //FILE *file = fopen("out.txt", "w");

        fprintf(out, "uci\n");
        fflush(out);

        do{
            fgets(buf, 512, in);
            //fprintf(file, "%s", buf);
            //fflush(file);
        }while(strcmp(buf, "uciok\n"));

        fprintf(out, "isready\n");
        fflush(out);

        fgets(buf, 512, in);
        //fprintf(file, "%s", buf);
        if(strcmp(buf, "readyok\n")){
            fprintf(stderr, "Engine not ready\n");
            return NULL;
        }
        fprintf(out, "setoption name Hash value 4096\n");
        fprintf(out, "setoption name Threads value 8\n");
        fprintf(out, "ucinewgame\n");
        /* 
           fprintf(out, "go infinite\n");
           fflush(out); 
           sleep(3);
           fprintf(out, "stop\n");
           fflush(out); 
           */ 
        //fscanf(in, "bestmove %5s", buf);
        FILE *game = fopen("last_game.txt", "w");
        do{
            //fprintf(stderr, "to_move: %d\n", to_move);
            pthread_mutex_lock(&to_move_m);
            if(1){
                //fprintf(stderr, "engine's move\n");
                //printf("moves: %s\n", moves.c_str());
                fprintf(out, "position startpos moves %s\n", moves.c_str());
                //fprintf(out, "go nodes 4294967296\n");
                fprintf(out, "go nodes %d\n", 1 << LEVEL);
                //fprintf(out, "go\n");
                fflush(out);
                //sleep(3);
                /* 
                fprintf(out, "stop\n");
                fflush(out);
                */ 
                do{
                    fgets(buf, 512, in);
                    //fprintf(file, "%s", buf);
                }while(strncmp(buf, "bestmove", 8));
                //printf("buf: %s", buf);
                sscanf(buf, "bestmove %s ", buf);
                //printf("Got here\n");
                //printf("stripped %s\n", buf);
                fprintf(game, "%s ", buf);
                fflush(game);
                moves.append(buf);
                moves.append(" ");
                //printf("move: %s\n", buf);
                move(buf);
                //printf("moves: %s\n", moves.c_str());
                to_move = 0;
                //fprintf(stderr, "end engine's move\n");
            }
            pthread_mutex_unlock(&to_move_m);
        }while(strcmp(buf, "(none)"));
        //fprintf(game, "%s", moves.c_str());
        fclose(game);
    }else{ // child
        //fprintf(stderr, "child\n");
        close(pin_cout[0]);
        close(pout_cin[1]);
        fclose(stdout);       
        if(dup2(pin_cout[1], 1) == -1){
            fprintf(stderr, "pin_cout failed to copy\n");
            perror("error is");
        }
        stdout = fdopen(1, "w");
        fclose(stdin);
        if(dup2(pout_cin[0], 0) == -1){
            fprintf(stderr, "pout_cin failed to copy\n");
            perror("error is");
        }
        stdin = fdopen(0, "r");
        //fprintf(stderr, "child\n");
        char buf[100];
        //FILE *out = fopen("childout.txt", "w");
        /*
           while(fgets(buf, 100, stdin)){
           fprintf(out, "%s", buf);
           fflush(out);
           }
           */ 
        execl("./engines/stockfish", "./engines/stockfish", NULL);
        //fprintf(out, "UH OH\n");
        //fclose(out);
    }
}
