#ifdef __APPLE__
    #include <SDL.h>
#else
    #include<SDL2/SDL.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include "file.h"


#define MAX_ZOOM 10.f
#define MIN_ZOOM 0.1f

typedef struct {
    float zoom;
    int offset_x;
    int offset_y;
} Image_State;

float max_f(float a,float b){
    return a > b ? a : b;
}

void update_dst_rect(SDL_Rect* rect, SDL_Window* window,int imgW, int imgH, Image_State state){
    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);
    rect->w = (int)(imgW * state.zoom);
    rect->h = (int)(imgH * state.zoom);
    rect->x = (winW - rect->w) / 2 + state.offset_x;
    rect->y = (winH - rect->h) / 2 + state.offset_y;
}


void handle_keypress(SDL_Keycode key, Image_State* state){
    if(key == SDLK_EQUALS) state->zoom *= 1.1f;
    if(key == SDLK_MINUS) state->zoom *= 0.9f;
    if(key == SDLK_LEFT) state->offset_x -= 10;
    if(key == SDLK_RIGHT) state->offset_x += 10;
    if(key == SDLK_UP) state->offset_y -= 10;
    if(key == SDLK_DOWN) state->offset_y += 10;
    if(key == SDLK_0) {
        state->zoom = 1.f;
        state->offset_x = 0;
        state->offset_y = 0;
    };
}

int main(int argc, char* argv[]) {
    if(argc <= 1) {
        fprintf(stderr, "File not specified\n");
        exit(1);
    }

    IMAGE_FORMATS format = get_file_format(argv[1]);
    if(format == IMAGE_UNKNOWN){
        fprintf(stderr, "Unsupported file format\n");
        exit(1);
    }

    printf("File format: %d\n", format);
    PPM6_IMAGE* PIMG = NULL;
    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", argv[1]);
        exit(1);
    }

    PIMG = read_ppm6_image(file);
    if (!PIMG) {
        fprintf(stderr, "Failed to read image data\n");
        fclose(file);
        exit(1);
    }
    fclose(file);
    printf("Image loaded\n");
    printf("W=%d H=%d Max=%d\n",PIMG->width, PIMG->height, PIMG->max_color_value);

    RGBA px = PIMG->pixels[0];
    printf("First pixel: %d %d %d\n", px.r, px.g, px.b);
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL Init Failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Image Viewer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        PIMG->width,
        PIMG->height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    SDL_Texture* pTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        PIMG->width,
        PIMG->height
    );

    if(SDL_UpdateTexture(pTexture, NULL, PIMG->pixels, PIMG->width * sizeof(RGBA)) != 0) {
        printf("SDL Init Failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Event e;
    bool running = true;

    Image_State state = {
        .zoom = 1.f,
        .offset_x = 0,
        .offset_y = 0
    };

    SDL_Rect dstRect;
    dstRect.h = PIMG -> height;
    dstRect.w = PIMG -> width;
    dstRect.x = 0;
    dstRect.y = 0;

    while(running){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT)running = false;

            if(e.type == SDL_KEYDOWN) {
                handle_keypress(e.key.keysym.sym, &state);
            }
        }

        // CLAMP ZOOM
        if(state.zoom <= MIN_ZOOM) state.zoom = MIN_ZOOM;
        if(state.zoom >= MAX_ZOOM) state.zoom = MAX_ZOOM;

        update_dst_rect(&dstRect, window, PIMG->width, PIMG->height, state);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, pTexture, NULL, &dstRect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(pTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free_ppm6_image(PIMG);

    return 0;
}
