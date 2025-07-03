#include "../includes/pixel.h"
#include "../includes/f_manip.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Compute convoluted pixel for a given kernel. Optionnaly send a constant.
 * Operations here should be verified as valid in the functions that
 * call this one.
 */
static inline int
compute_gen_conv_px(struct pixel** plate, const int i, const int j,
	const int kern[3][3], const double factor);

/**
 * Determines if a pixel should be dilated. Scan for black pixels in a box of
 * size d_sz x d_sz. Maybe at some point we could choose a shape for the 
 * structuring element. Returns 0x00 if pixel should turn dark, 0xFF otherwise.
 */
static inline uint8_t
dilates(struct pixel** plate, const int i, const int j);

/**
 * Determines if a pixel should be eroded. Scan for white pixels in a box of
 * size d_sz x d_sz. Maybe at some point we could choose a shape for the 
 * structuring element. Returns 0xFF if pixel should turn dark, 0x00 otherwise.
 */
static inline uint8_t
erodes(struct pixel** plate, const int i, const int j);

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
grayscale_filter(struct t_image* image) {
    double y_lin;

    for(int i = 0; i < image->h; i++) {
	for (int j = 0; j < image->w; j++) {
	    y_lin = GSCALE_R_COEFF * image->im[i][j].r
		+ GSCALE_G_COEFF * image->im[i][j].g
		+ GSCALE_B_COEFF * image->im[i][j].b;
	    image->im[i][j].r = image->im[i][j].g = image->im[i][j].b = (uint8_t) round(y_lin);
	}
    }
}

void
gaussian_blur3_filter(struct t_image* im_src, struct t_image* im_dst) {
    im_dst->h = im_src->h;
    im_dst->w = im_src->w;

    // Gaussian integer convolution kernel
    const int GB_3_INT_KER[3][3] = {{1,2,1}, {2,4,2}, {1,2,1}};

    // Create new image
    struct pixel** im_dst_im = new_px_array(im_dst->h, im_dst->w);
    if(!im_dst_im)
	return;

    im_dst->im = im_dst_im;

    // Copy the border pixels
    for (int i = 0; i < im_dst->h; i++) {
	im_dst->im[i][0] = im_src->im[i][0];
	im_dst->im[i][im_dst->w - 1] = im_src->im[i][im_dst->w - 1];
    }
    for (int j = 1; j < im_dst->w - 1; j++) {
	im_dst->im[0][j] = im_src->im[0][j];
	im_dst->im[im_dst->h - 1][j] = im_src->im[im_dst->h - 1][j];
    }

    // Calculate all convoluted pixels, store in new_plate
    uint8_t conv_px = 0;
    for (int i = 1; i < im_dst->h - 1; i++) {
	for (int j = 1; j < im_dst->w - 1; j++) {
	    conv_px = (uint8_t) compute_gen_conv_px(im_src->im, i, j, GB_3_INT_KER, GB3_FACTOR);
	    im_dst->im[i][j].r = im_dst->im[i][j].g = im_dst->im[i][j].b = conv_px;
	}
    }
}

void
sobel_filter(struct t_image* im_src, struct t_image* im_dst) {
    im_dst->h = im_src->h;
    im_dst->w = im_src->w;

    // Sobel convolution kernels
    const int SOBEL_GX[3][3] = {{-1,0,1}, {-2,0,2}, {-1,0,1}};
    const int SOBEL_GY[3][3] = {{-1,-2,-1}, {0,0,0}, {1,2,1}};

    // Create new image
    struct pixel** im_dst_im = new_px_array(im_dst->h, im_dst->w);
    if(!im_dst_im)
	return;

    im_dst->im = im_dst_im;

    // Copy the border pixels
    for (int i = 0; i < im_dst->h; i++) {
	im_dst->im[i][0] = im_src->im[i][0];
	im_dst->im[i][im_dst->w - 1] = im_src->im[i][im_dst->w - 1];
    }
    for (int j = 1; j < im_dst->w - 1; j++) {
	im_dst->im[0][j] = im_src->im[0][j];
	im_dst->im[im_dst->h - 1][j] = im_src->im[im_dst->h - 1][j];
    }

    // Calculate all convoluted pixels, calculate magnitude, store in new_plate
    int px_gx = 0;
    int px_gy = 0;
    uint8_t conv_px = 0;
    for (int i = 1; i < im_dst->h - 1; i++) {
	for (int j = 1; j < im_dst->w - 1; j++) {
	    px_gx = compute_gen_conv_px(im_src->im, i, j, SOBEL_GX, SOBEL_FACTOR);
	    px_gy = compute_gen_conv_px(im_src->im, i, j, SOBEL_GY, SOBEL_FACTOR);
	    conv_px = (uint8_t) sqrt(px_gx * px_gx + px_gy * px_gy);
	    im_dst->im[i][j].r = im_dst->im[i][j].g = im_dst->im[i][j].b = conv_px;
	}
    }
}

void
threshold(struct t_image* im_src, struct t_image* im_dst) {
    im_dst->h = im_src->h;
    im_dst->w = im_src->w;

    // Create new image
    struct pixel** im_dst_im = new_px_array(im_dst->h, im_dst->w);
    if(!im_dst_im)
	return;

    im_dst->im = im_dst_im;

    // Simple thresholding
    for (int i = 0; i < im_src->h; i++) {
	for (int j = 0; j < im_src->w; j++) {
	    im_dst->im[i][j].r = im_dst->im[i][j].g = im_dst->im[i][j].b
		= (im_src->im[i][j].r < THRESH_S) ? (0x00):(0xFF);
	}
    }
}

struct pixel**
morph_dilation(struct pixel** plate, const int h, const int w) {
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

    // Dilation
    for (int i = 1; i < h - 1; i++) {
	for (int j = 1; j < w - 1; j++) {
	    if(plate[i][j].r) {
		new_plate[i][j].r = new_plate[i][j].g = new_plate[i][j].b
		    = dilates(plate, i, j);
	    }
	    else
		new_plate[i][j].r = new_plate[i][j].g = new_plate[i][j].b = plate[i][j].r;
	}
    }

    return new_plate;
}

struct pixel**
morph_erosion(struct pixel** plate, const int h, const int w) {
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

    // Erosion
    for (int i = 1; i < h - 1; i++) {
	for (int j = 1; j < w - 1; j++) {
	    if(!plate[i][j].r) {
		new_plate[i][j].r = new_plate[i][j].g = new_plate[i][j].b
		    = erodes(plate, i, j);
	    }
	    else
		new_plate[i][j].r = new_plate[i][j].g = new_plate[i][j].b = plate[i][j].r;
	}
    }

    return new_plate;
}

struct pixel**
isolate_number(struct pixel** plate, int* h, int* w) {

    int tbp; // Total black pixels on x axis per line
    int px_buf = 5; // 5 pixels of buffer
    double h_rat = 0;
    double w_rat = 0;
    
    // Dimensions for the new plate
    int min_h = 0;
    int max_h = 0;
    int min_w = 0;
    int max_w = 0;

    for (int i = 0; i < *h; i++) {
	tbp = 0;
	for (int j = 0; j < *w; j++) {
	    if(!plate[i][j].r)
		tbp++;
	}

	h_rat = ((double) i) / *h;
	w_rat = ((double) tbp) / *w;
	// 0.3221 for letters ratio, 0.22135 for height ratio
	if(w_rat > 0.3211 && (h_rat > 0.22135 && h_rat < 0.2864)) {
	    min_h = i - px_buf;
	    i = (int) (0.664 * *h);
	}
	// 0.035248 for letters ratio, 0.359 for height ratio
	else if(w_rat < 0.035248 && h_rat > 0.359) {
	    max_h = i + px_buf;
	    break;
	}
    }

    // Create new image
    int new_h = max_h - min_h + 1;
    struct pixel** new_plate = new_px_array(new_h, *w);
    *h = new_h;
    if(!new_plate)
	return NULL;

    // Copy the pixels
    for (int i = min_h; i < max_h + 1; i++) {
	for (int j = 0; j < *w; j++) {
	    new_plate[i - min_h][j] = plate[i][j];
	}
    }

    // Using the new image to isolate the plate horizontally
    double v_rat = 0;
    double d_rat = 0;
    for (int j = 0; j < *w; j++) {
	tbp = 0;
	for (int i = 0; i < *h; i++) {
	    if(!new_plate[i][j].r)
		tbp++;
	}

	v_rat = ((double) tbp) / *h;
	d_rat = ((double) j) / *w;
	if(v_rat > 0.03 && (d_rat > 0.03911 && d_rat < 0.13038)) {
	    min_w = j - px_buf;
	    j = (int) (0.855 * *w);
	}
	// 0.035248 for letters ratio, 0.359 for height ratio
	else if(v_rat < 0.03 && d_rat > 0.88657) {
	    max_w = j + px_buf;
	    break;
	}
    }

    // Create new image
    int new_w = max_w - min_w + 1;
    struct pixel** newest_plate = new_px_array(*h, new_w);
    *w = new_w;
    if(!new_plate)
	return NULL;

    // Copy the pixels
    for (int j = min_w; j < max_w + 1; j++) {
	for (int i = 0; i < *h; i++) {
	    newest_plate[i][j - min_w] = new_plate[i][j];
	}
    }

    // Free inter. plate
    free_px_array(new_plate, *h);

    return newest_plate;
}

static inline int
compute_gen_conv_px(struct pixel** plate, int i, int j,
	const int kern[3][3], const double factor) {
    double conv_px_d = factor *
	(plate[i - 1][j - 1].r * kern[2][2] + plate[i - 1][j].r * kern[2][1]
	 + plate[i - 1][j + 1].r * kern[2][0] +
	 plate[i][j - 1].r * kern[1][2] + plate[i][j].r * kern[1][1] +
	 plate[i][j + 1].r * kern[1][0] +
	 plate[i + 1][j - 1].r * kern[0][2] + plate[i + 1][j].r * kern[0][1] +
	 plate[i + 1][j + 1].r * kern[0][0]);

    return (int) conv_px_d;
}

static inline uint8_t
dilates(struct pixel** plate, const int i, const int j) {
    for (int k = i - 1; k < i + 2; k++) {
	for (int l = j - 1; l < j + 2; l++) {
	    if(!plate[k][l].r)
		return 0x00;
	}
    }

    return 0xFF;
}

static inline uint8_t
erodes(struct pixel** plate, const int i, const int j) {
    for (int k = i - 1; k < i + 2; k++) {
	for (int l = j - 1; l < j + 2; l++) {
	    if(plate[k][l].r)
		return 0xFF;
	}
    }

    return 0x00;
}
