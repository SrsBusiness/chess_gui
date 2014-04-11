#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "chess.h"
#include <map>
#include "gui.h"

using namespace std;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

map<char, SDL_Texture *> icons;

int current_width = SCREEN_WIDTH;
int current_height = SCREEN_HEIGHT;
SDL_Rect tiles[64];

void err_msg(const char *);
SDL_Texture *load_texture(const char *, SDL_Renderer *);
void render_texture(SDL_Texture *, SDL_Renderer *, int, int);
void render_texture(SDL_Texture *, SDL_Renderer *, int, int, int, int);
void init_board();
void render_board();
void init_icons();
void resize(int, int);

SDL_Window *win;
SDL_Renderer *board_r;

int quit;

int main(){
    pthread_mutex_init(&board_m, NULL);
    if(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG != IMG_INIT_PNG){
        err_msg("IMG_INIT");
        return 1;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING)){
        //std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        err_msg("SDL_Init");
        return 1;
    }
    win = SDL_CreateWindow("TBD", 100, 100, current_width, current_height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!win){
        //std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        err_msg("CreateWindow");
        return 1;
    }
    board_r = SDL_CreateRenderer(win, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!board){
        //std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        err_msg("CreateRenderer");
        return 1;
    }
    init_board();
    init_icons();
    set_board();
    render_board();
    quit = 0;
    SDL_Event e;
    pthread_t io;
    //pthread_create(&io, NULL, load_engine, NULL); 
    pthread_create(&io, NULL, load_engine, NULL); 
    while(!quit){
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_WINDOWEVENT:
                    //printf("Window, %d\n", e.window.event);
                    //printf("Window resize: %d\n", SDL_WINDOWEVENT_RESIZED);
                    switch(e.window.event){
                        case SDL_WINDOWEVENT_RESIZED:
                            resize(e.window.data1, e.window.data2);
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
        render_board();
        SDL_RenderPresent(board_r);
        //printf("Presented\n");
    } 
    SDL_DestroyRenderer(board_r);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void err_msg(const char *msg){
    fprintf(stderr, "%s\n", msg);
}

SDL_Texture *load_texture(const char *file, SDL_Renderer *ren){
    SDL_Texture *texture = IMG_LoadTexture(ren, file);
    if(!texture)
        err_msg("LoadTexture");
    return texture;
}

void render_texture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

void render_texture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h){
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

void init_board(){
    int board_size = (current_height > current_width ? current_width : current_height) - 20;
    //int index = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            int x = board_size / 8 * j + 10;
            int y = board_size / 8 * i + 10;
            tiles[i * 8 + j] = (SDL_Rect){x, y, board_size / 8, board_size / 8};
        }
    }
}

void init_icons(){
    icons['r'] = load_texture("pieces/black/rook.png", board_r);
    icons['n'] = load_texture("pieces/black/knight.png", board_r);
    icons['b'] = load_texture("pieces/black/bishop.png", board_r);
    icons['p'] = load_texture("pieces/black/pawn.png", board_r);
    icons['q'] = load_texture("pieces/black/queen.png", board_r);
    icons['k'] = load_texture("pieces/black/king.png", board_r);
    icons['R'] = load_texture("pieces/white/rook.png", board_r);
    icons['N'] = load_texture("pieces/white/knight.png", board_r);
    icons['B'] = load_texture("pieces/white/bishop.png", board_r);
    icons['P'] = load_texture("pieces/white/pawn.png", board_r);
    icons['Q'] = load_texture("pieces/white/queen.png", board_r);
    icons['K'] = load_texture("pieces/white/king.png", board_r);
}

void resize(int w, int h){
    current_width = w;
    current_height = h;
    SDL_SetWindowSize(win, w, h);
    SDL_RenderClear(board_r);
    init_board();
    render_board();
}

void render_board(){
    SDL_SetRenderDrawColor(board_r, 127, 127, 127, 255);
    SDL_RenderClear(board_r);
    pthread_mutex_lock(&board_m);
    for(int i = 0; i < 64; i++){
        if((i / 8 + i % 8) & 1){
            SDL_SetRenderDrawColor(board_r, 139, 69, 19, 255);
        }
        else
            SDL_SetRenderDrawColor(board_r, 245, 222, 179, 255);
        SDL_RenderFillRect(board_r, tiles + i); // square
        if(board[i] != '-')
            render_texture(icons[board[i]], board_r, tiles[i].x, tiles[i].y, tiles[i].w, tiles[i].h); // piece
    }
    pthread_mutex_unlock(&board_m);
}
