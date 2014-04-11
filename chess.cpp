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

using namespace std;

char board[65];
string moves;
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

void move(char m[4]){
    int a, b, c, d;
    //printf("Move(): move: %s\n", m);
    //printf("m[0]: %d, m[1]: %d, m[2]: %d, m[3]: %d\n", m[0], m[1], m[2], m[3]);
    //sscanf(m, "%c%d%c%d", &a, &b, &c, &d);
    //printf("a: %d, b: %d, c: %d, d: %d\n", a, b, c, d);
    int src = (8 - m[1] + '0') * 8 + (m[0] - 'a');
    int dest = (8 - m[3] + '0') * 8 + (m[2] - 'a');
    //printf("src: %d, dest: %d\n", src, dest);
    move(src, dest);
}

void *get_moves(void *){
    char in[20];
    while(1){
        printf("Enter move: ");
        fgets(in, 20, stdin);
        int a, b, c, d;
        sscanf(in, "%c%d%c%d", &a, &b, &c, &d);
        printf("a: %d, b: %d, c: %d, d: %d\n", a, b, c, d);
        int src = (8 - b) * 8 + (a - 'a');
        int dest = (8 - d) * 8 + (c - 'a');
        printf("src: %d, dest: %d\n", src, dest);
        move(src, dest);
        printf("board: %s\n", board);
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
        char buf[200];
        FILE *in = fdopen(pin_cout[0], "r"); 
        FILE *out = fdopen(pout_cin[1], "w");
        FILE *file = fopen("out.txt", "w");

        fprintf(out, "uci\n");
        fflush(out);

        do{
            fgets(buf, 200, in);
            fprintf(file, "%s", buf);
            fflush(file);
        }while(strcmp(buf, "uciok\n"));

        fprintf(out, "isready\n");
        fflush(out);

        fgets(buf, 200, in);
        fprintf(file, "%s", buf);
        if(strcmp(buf, "readyok\n")){
            fprintf(stderr, "Engine not ready\n");
            return NULL;
        }

        fprintf(out, "ucinewgame\n");
        fprintf(out, "go movetime 10\n");
        fflush(out); 
        //fscanf(in, "bestmove %5s", buf);
        do{
            do{
                fgets(buf, 200, in);
                fprintf(file, "%s", buf);
            }while(strncmp(buf, "bestmove", 8));
            sscanf(buf, "bestmove %5s", buf);
            //printf("Got here\n");
            moves.append(buf);
            moves.append(" ");
            printf("move: %s\n", buf);
            move(buf);
            printf("moves: %s\n", moves.c_str());
            fprintf(out, "position startpos moves %s\n", moves.c_str());
            fprintf(out, "go movetime 10\n");
            fflush(out);
            sleep(1);
        }while(strcmp(buf, "(none"));
        FILE *game = fopen("last_game.txt", "w");
        fprintf(game, "%s", moves.c_str());
        fclose(game);
        /*
        printf("move\n");
            moves.append(buf, 5);
            move(buf);
            fprintf(out, "position startpos moves %s\n", moves.c_str());
            fprintf(out, "go movetime 10\n");
            fflush(out);
            do{
                fgets(buf, 200, in);
                fprintf(file, "%s", buf);
            }while(strncmp(buf, "bestmove", 8));
            sscanf(buf, "bestmove %5s", buf);
            printf("move: %s\n", buf);
            printf("moves: %s\n", moves.c_str());
        }
        */
        //printf("Engine ready\n");
    }else{ // child
        fprintf(stderr, "child\n");
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
        fprintf(stderr, "child\n");
        char buf[100];
        FILE *out = fopen("childout.txt", "w");
        /*
           while(fgets(buf, 100, stdin)){
           fprintf(out, "%s", buf);
           fflush(out);
           }
           */ 
        execl("./engines/stockfish", "./engines/stockfish", NULL);
        fprintf(out, "UH OH\n");
        fclose(out);
    }
}
