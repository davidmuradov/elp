/// @file pixel.h
#ifndef INCLUDES_PIXEL_H
#define INCLUDES_PIXEL_H

#include <stdint.h>
#include <stdlib.h>

struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/**
 * Free memory for a 2D array of pixels (or a plate image)
 */
void free_px_array(struct pixel** px_arr, const int h);

#endif
