#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "chess.h"
#include <map>

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
void render_board(SDL_Renderer *);
void init_icons(SDL_Renderer *);
void resize(int, int, SDL_Window *);

int main(){
    if(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG != IMG_INIT_PNG){
        err_msg("IMG_INIT");
        return 1;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING)){
        //std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        err_msg("SDL_Init");
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("TBD", 100, 100, current_width, current_height,
            SDL_WINDOW_SHOWN);
    if (!win){
        //std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        err_msg("CreateWindow");
        return 1;
    }
    SDL_Renderer *board = SDL_CreateRenderer(win, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!board){
        //std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        err_msg("CreateRenderer");
        return 1;
    }
    init_board();
    init_icons(board);
    set_board();
    render_board(board);
    int quit = 0;
    SDL_Event e;
    while(!quit){
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_WINDOWEVENT:
                    printf("Window, %d\n", e.window.event);
                    printf("Window resize: %d\n", SDL_WINDOWEVENT_RESIZED);
                    switch(e.window.event){
                        case SDL_WINDOWEVENT_RESIZED:
                            resize(e.window.data1, e.window.data2, win);
                            init_board();
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
        SDL_RenderPresent(board);
    } 
    SDL_DestroyRenderer(board);
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

void init_icons(SDL_Renderer *ren){
    icons['r'] = load_texture("pieces/black/rook.png", ren);
    icons['n'] = load_texture("pieces/black/knight.png", ren);
    icons['b'] = load_texture("pieces/black/bishop.png", ren);
    icons['p'] = load_texture("pieces/black/pawn.png", ren);
    icons['q'] = load_texture("pieces/black/queen.png", ren);
    icons['k'] = load_texture("pieces/black/king.png", ren);
    icons['R'] = load_texture("pieces/white/rook.png", ren);
    icons['N'] = load_texture("pieces/white/knight.png", ren);
    icons['B'] = load_texture("pieces/white/bishop.png", ren);
    icons['P'] = load_texture("pieces/white/pawn.png", ren);
    icons['Q'] = load_texture("pieces/white/queen.png", ren);
    icons['K'] = load_texture("pieces/white/king.png", ren);
}

void resize(int w, int h, SDL_Window* win){
    current_width = w;
    current_height = h;
    SDL_SetWindowSize(win, w, h);
}

void render_board(SDL_Renderer *ren){
    SDL_SetRenderDrawColor(ren, 127, 127, 127, 255);
    SDL_RenderClear(ren);
    for(int i = 0; i < 64; i++){
        if((i / 8 + i % 8) & 1){
            SDL_SetRenderDrawColor(ren, 139, 69, 19, 255);
        }
        else
            SDL_SetRenderDrawColor(ren, 245, 222, 179, 255);
        SDL_RenderFillRect(ren, tiles + i); // square
        if(board[i] != '-')
            render_texture(icons[board[i]], ren, tiles[i].x, tiles[i].y, tiles[i].w, tiles[i].h); // piece
    }
}
