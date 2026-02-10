#include "file.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool check_ppm6_format(FILE* file) {
    char magic[3];
    if (fread(magic, 1, 3, file) != 3)
        return false;
    rewind(file);
    return magic[0] == 'P' && magic[1] == '6';
}

void free_ppm6_image(PPM6_IMAGE* img) {
    if (img) {
        free(img->pixels);
        free(img);
    }
}

PPM6_IMAGE* read_ppm6_image(FILE* file){
    if(!check_ppm6_format(file))return NULL;
    PPM6_IMAGE* image = malloc(sizeof(PPM6_IMAGE));
    if (!image) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    } 
    
    fscanf(file, "%2s", (char[3]){0}); // Skip magic number
    fscanf(file, "%d %d", &image->width, &image->height);
    fscanf(file, "%d", &image->max_color_value);
    fgetc(file); // Skip newline 

    if (image->max_color_value > 255) {
        fprintf(stderr, "Unsupported max color value: %d\n", image->max_color_value);
        free_ppm6_image(image);
        return NULL;
    }

    size_t pixel_count = (size_t)image->height * image->width;
    size_t image_size = pixel_count * 3;
    uint8_t* RGB_pixels = malloc(image_size);
    if (!RGB_pixels) {
        fprintf(stderr, "Memory al`location failed\n");
        free_ppm6_image(image);
        return NULL;
    }
    
    if (fread(RGB_pixels, 1, image_size, file) != image_size) {
        fprintf(stderr, "Failed to read pixel data\n");
        free(RGB_pixels);
        free_ppm6_image(image);
        return NULL;
    }

    image->pixels = malloc(image-> width * image-> height * sizeof(RGBA));
    if(!image->pixels) {
        fprintf(stderr, "Memory allocation failed\n");
        free(RGB_pixels);
        free_ppm6_image(image);
        return NULL;
    }

    for(size_t i = 0; i < image->width * image->height; i++) {
        image->pixels[i].r = RGB_pixels[i*3];
        image->pixels[i].g = RGB_pixels[i*3 + 1];
        image->pixels[i].b = RGB_pixels[i*3 + 2];
        image->pixels[i].a = 255; // Set alpha to fully opaque
    }
    free(RGB_pixels);
    return image;
}

IMAGE_FORMATS get_file_format(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Could not open file: %s\n", file_path);
        return IMAGE_UNKNOWN; 
    }

    if(check_ppm6_format(file)) {
        fclose(file);
        return PPM;
    }
    fclose(file);
    return IMAGE_UNKNOWN;
}

    