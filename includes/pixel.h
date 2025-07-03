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

// Sobel operator smoothing factor
#define SOBEL_FACTOR	(1./4)

#define THRESH_S	(0x80)

struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct t_image {
    int h;
    int w;
    struct pixel** im;
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
 * Convert an image to a grayscale version. The grayscaled image is obtained
 * using grayscale coefficients.
 */
void
grayscale_filter(struct t_image* image);

/**
 * Applies a 3x3 gaussian blur to an image. In this version, the outer edge
 * of the picture will not be modified. Returns a new struct pixel** which
 * is the gaussian blurred version of the image received as parameter.
 */
void
gaussian_blur3_filter(struct t_image* im_src, struct t_image* im_dst);

/**
 * Applies the sobel operator to an image. Outer edge pixels are ignored.
 * Returns a new struct pixel** which is the sobel variant of the image received
 * as parameter
 */
void
sobel_filter(struct t_image* im_src, struct t_image* im_dst);

/**
 * Applies a constant threshold for binerization. Dark colors turn pure black
 * and light colors turn pure white.
 */
void
threshold(struct t_image* im_src, struct t_image* im_dst);

/**
 * Applies morphological dilation on a binary image. The function attempts to
 * dilate dark pixels based on the size of the structuring element
 * (the dilation kernel). Returns a dilated image.
 */
void
dilation_morphological(struct t_image* im_src, struct t_image* im_dst);

/**
 * Applies morphological erosion on a binary image. The function attempts to
 * erode dark pixels based on the size of the structuring element
 * (the erosion kernel). Returns an eroded image.
 */
void
erosion_morphological(struct t_image* im_src, struct t_image* im_dst);

/**
 * Isolates the plate number for Optical Character Recognition (OCR). This is
 * a very basic implementation, will probably need to be updated in the future
 * for reliability. Returns a struct pixel** containing the isolated number.
 */
void
isolate_number(struct t_image* im_src, struct t_image* im_dst);

#endif
