#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include "file.h"

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

    printf("Image loaded\n");
    printf("W=%d H=%d Max=%d\n",PIMG->width, PIMG->height, PIMG->max_color_value);

    RGBA px = PIMG->pixels[0];
    printf("First pixel: %d %d %d\n", px.r, px.g, px.b);
    // if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    //     printf("SDL Init Failed: %s\n", SDL_GetError());
    //     return 1;
    // }

    // SDL_Window* window = SDL_CreateWindow(
    //     "Image Viewer",
    //     SDL_WINDOWPOS_CENTERED,
    //     SDL_WINDOWPOS_CENTERED,
    //     400, 400,
    //     SDL_WINDOW_SHOWN
    // );

    // SDL_Renderer* renderer = SDL_CreateRenderer(
    //     window,
    //     -1,
    //     SDL_RENDERER_ACCELERATED
    // );
    
    // SDL_Event e;
    // bool running = true;

    // while (running) {
    //     while (SDL_PollEvent(&e)) {
    //         if (e.type == SDL_QUIT)
    //             running = false;
    //     }   
    //     SDL_RenderClear(renderer);
    //     SDL_RenderPresent(renderer);
    // }

    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit();
    return 0;
}
