/// @file pixel.h
#ifndef INCLUDES_PIXEL_H
#define INCLUDES_PIXEL_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// Grayscale coefficients to transform pixels to grayscale versions
#define GSCALE_R_COEFF	0.2126
#define GSCALE_G_COEFF	0.7152
#define GSCALE_B_COEFF	0.0722

// Gaussian integer blur factor
#define GB3_FACTOR	(1./16)

struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/**
 * Create a new 2D array of pixels. The color values are random, so this array
 * should be initialized later in the program.
 */
struct pixel**
new_px_array(const int h, const int w);

/**
 * Free memory for a 2D array of pixels (or a plate image)
 */
void
free_px_array(struct pixel** px_arr, const int h);

/**
 * Convert a pixel to a grayscale version. The grayscaled pixel is obtained
 * using grayscale coefficients.
 */
void
grayscale_px(struct pixel* px);

/**
 * Applies a 3x3 gaussian blur to an image. In this version, the outer edge
 * of the picture will not be modified. The original image gets deallocated
 * and the new version of the image is stored in the new_plate. If allocation
 * fails, nothing happens.
 */
void
gaussian_blur_3(struct pixel** plate, struct pixel** new_plate,
	const int h, const int w);

#endif
