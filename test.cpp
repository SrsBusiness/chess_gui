#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 40;

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

int main(int argc, char **argv){
    if(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG != IMG_INIT_PNG){
        err_msg("IMG_INIT");
        return 1;
    }
    
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        //std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        err_msg("SDL_Init");
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);
    if (!win){
        //std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        err_msg("CreateWindow");
        return 1;
    }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren){
        //std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        err_msg("CreateRenderer");
        return 1;
    }
    
    SDL_Texture *background = load_texture("background.png", ren);
    SDL_Texture *image = load_texture("image.png", ren);
    if(!((long)background & (long)image)){
        err_msg("Load Images");
        return 1;
    }
    SDL_RenderClear(ren);
    int w, h;
    SDL_QueryTexture(background, NULL, NULL, &w, &h);
    /*for(int i = 0; i < 4; i++){
        render_texture(background, ren, (i & 1) * w, ((i >> 1) & 1) * h);
    }*/
    int tile_x = SCREEN_WIDTH / TILE_SIZE;
    int tile_y = SCREEN_HEIGHT / TILE_SIZE;

    for(int i = 0; i < tile_x * tile_y; i++){
        int x = i % tile_x;
        int y = i / tile_x;
        render_texture(background, ren, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    }
   
    int iW, iH;
    SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
    int x = SCREEN_WIDTH / 2 - iW / 2;
    int y = SCREEN_HEIGHT / 2 - iH / 2;
    render_texture(image, ren, x, y);

    SDL_RenderPresent(ren);
    SDL_Delay(2000);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();






}   
