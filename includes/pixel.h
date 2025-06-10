/// @file pixel.h
#ifndef INCLUDES_PIXEL_H
#define INCLUDES_PIXEL_H

#include <stdint.h>
#include <stdlib.h>

// Grayscale coefficients to transform pixels to grayscale versions
#define GSCALE_R_COEFF	0.2126
#define GSCALE_G_COEFF	0.7152
#define GSCALE_B_COEFF	0.0722

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
