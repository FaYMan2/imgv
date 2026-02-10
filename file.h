#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stdio.h>

typedef enum {
    IMAGE_UNKNOWN = 0,
    PPM
} IMAGE_FORMATS;

typedef struct {
    uint8_t r, g, b, a;
} RGBA;

typedef struct {
    int width;
    int height;
    int max_color_value;
    RGBA* pixels;    
} PPM6_IMAGE;

IMAGE_FORMATS get_file_format(const char* file_path);
PPM6_IMAGE* read_ppm6_image(FILE* file);
void free_ppm6_image(PPM6_IMAGE* img);

#endif
