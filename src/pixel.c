#include "../includes/pixel.h"
#include "../includes/f_manip.h"
#include <stdio.h>

/**
 * Compute convoluted pixels for a 3x3 gaussian blur
 * Operations here should be verified as valid in the functions that
 * call this one.
 */
static inline uint8_t
compute_gauss3_conv_px(struct pixel** plate, const int i, const int j, const int kern[3][3]);

struct pixel**
new_px_array(const int h, const int w) {
    struct pixel** arr = (struct pixel**)
	malloc((unsigned long) h * sizeof(struct pixel*));
    if(!arr) {
	fprintf(stderr, "Error while allocating memory for new pixel array\n");
	return NULL;
    }

    // Allocate for every line
    for (int i = 0; i < h; i++) {
	arr[i] = (struct pixel*)
	    malloc((unsigned long) w * sizeof(struct pixel));
	if(!arr[i]) {
	    fprintf(stderr, "Error while allocating memory for new pixel array\n");

	    // Free every line before the failed allocation, free the parent
	    // as well
	    for (int j = i - 1; j > -1; j--) {
		free(arr[j]);
		arr[j] = NULL;
	    }

	    free(arr);

	    return (arr = NULL);
	}
    }

    return arr;
}

void
free_px_array(struct pixel** px_arr, const int h) {
    for (int i = 0; i < h; i++) {
	free(px_arr[i]);
	px_arr[i] = NULL;
    }

    free(px_arr);
    px_arr = NULL;
}

void
grayscale_px(struct pixel* px) {
    double y = GSCALE_R_COEFF * px->r + GSCALE_G_COEFF * px->g + GSCALE_B_COEFF * px->b;
    px->r = px->g = px->b = (uint8_t) round(y);
}

struct pixel**
gaussian_blur_3(struct pixel** plate, const int h, const int w) {
    // Gaussian integer convolution kernel
    const int GB_3_INT_KER[3][3] = {{1,2,1}, {2,4,2}, {1,2,1}};

    // Create new image
    struct pixel** new_plate = new_px_array(h, w);
    if(!new_plate)
	return NULL;

    // Copy the border pixels
    for (int i = 0; i < h; i++) {
	new_plate[i][0] = plate[i][0];
	new_plate[i][w - 1] = plate[i][w - 1];
    }
    for (int j = 1; j < w - 1; j++) {
	new_plate[0][j] = plate[0][j];
	new_plate[h - 1][j] = plate[h - 1][j];
    }

    // Calculate all convoluted pixels, store in b_plate
    uint8_t conv_px = 0;
    for (int i = 1; i < h - 1; i++) {
	for (int j = 1; j < w - 1; j++) {
	    conv_px = compute_gauss3_conv_px(plate, i, j, GB_3_INT_KER);
	    new_plate[i][j].r = new_plate[i][j].g = new_plate[i][j].b = conv_px;
	}
    }

    return new_plate;
}

static inline uint8_t
compute_gauss3_conv_px(struct pixel** plate, int i, int j, const int kern[3][3]) {
    double conv_px_d = (GB3_FACTOR) *
	(plate[i - 1][j - 1].r * kern[0][0] + plate[i - 1][j].r * kern[0][1]
	 + plate[i - 1][j + 1].r * kern[0][2] +
	 plate[i][j - 1].r * kern[1][0] + plate[i][j].r * kern[1][1] +
	 plate[i][j + 1].r * kern[1][2] +
	 plate[i + 1][j - 1].r * kern[2][0] + plate[i + 1][j].r * kern[2][1] +
	 plate[i + 1][j + 1].r * kern[2][2]);

    return ((uint8_t) round(conv_px_d));
}
