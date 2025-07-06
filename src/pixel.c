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
    struct pixel** arr = malloc((unsigned long) h * sizeof(struct pixel*));
    if(!arr) {
	fprintf(stderr, "Error while allocating memory for new pixel array\n");
	return NULL;
    }

    // Allocate for every line
    for (int i = 0; i < h; i++) {
	arr[i] = malloc((unsigned long) w * sizeof(struct pixel));
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

struct gspixel**
new_gspx_array(const int h, const int w) {
    struct gspixel** arr = malloc((unsigned long) h * sizeof(struct gspixel*));
    if(!arr) {
	fprintf(stderr, "Error while allocating memory for new pixel array\n");
	return NULL;
    }

    // Allocate for every line
    for (int i = 0; i < h; i++) {
	arr[i] = malloc((unsigned long) w * sizeof(struct gspixel));
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
free_gspx_array(struct gspixel** px_arr, const int h) {
    for (int i = 0; i < h; i++) {
	free(px_arr[i]);
	px_arr[i] = NULL;
    }

    free(px_arr);
    px_arr = NULL;
}

void
grayscale_filter(struct t_image* im_src, struct t_gsimage* im_dst) {
    im_dst->h = im_src->h;
    im_dst->w = im_src->w;

    struct gspixel** im_dst_im = new_gspx_array(im_dst->h, im_dst->w);
    if(!im_dst_im)
	return;

    im_dst->im = im_dst_im;

    double y_lin;

    for(int i = 0; i < im_src->h; i++) {
	for (int j = 0; j < im_src->w; j++) {
	    y_lin = GSCALE_R_COEFF * im_src->im[i][j].r
		+ GSCALE_G_COEFF * im_src->im[i][j].g
		+ GSCALE_B_COEFF * im_src->im[i][j].b;
	    im_dst->im[i][j].gspx = im_dst->im[i][j].gspx = im_dst->im[i][j].gspx = (int) round(y_lin);
	}
    }
}

void
gaussian_blur3_filter(struct t_gsimage* im_src, struct t_gsimage* im_dst) {
    im_dst->h = im_src->h;
    im_dst->w = im_src->w;

    // Inter. array for padding
    struct gspixel** i_arr = new_gspx_array(im_src->h + 2, im_src->w + 2);

    // Inter. array for first pass
    struct gspixel** p1 = new_gspx_array(im_src->h, im_src->w + 2);

    // Copy corners
    i_arr[0][0] = im_src->im[0][0];
    i_arr[0][im_src->w + 1] = im_src->im[0][im_src->w - 1];
    i_arr[im_src->h + 1][0] = im_src->im[im_src->h - 1][0];
    i_arr[im_src->h + 1][im_src->w + 1] = im_src->im[im_src->h - 1][im_src->w - 1];


    // Upper-lower border copy
    for (int j = 1; j < im_src->w + 1; j++) {
	i_arr[0][j] = im_src->im[0][j - 1];
	i_arr[im_src->h + 1][j] = im_src->im[im_src->h - 1][j - 1];
    }

    // Left-right border copy
    for (int i = 1; i < im_src->h + 1; i++) {
	i_arr[i][0] = im_src->im[i - 1][0];
	i_arr[i][im_src->w + 1] = im_src->im[i - 1][im_src->w - 1];
    }

    // Copy rest of picture
    for (int i = 1; i < im_src->h + 1; i++)
	for (int j = 1; j < im_src->w + 1; j++)
	    i_arr[i][j] = im_src->im[i - 1][j - 1];

    // First pass
    for (int i = 1; i < im_src->h + 1; i++) {
	for (int j = 0; j < im_src->w + 2; j++) {
	    p1[i - 1][j].gspx = i_arr[i - 1][j].gspx + 2 * i_arr[i][j].gspx + i_arr[i + 1][j].gspx;
	}
    }

    free_gspx_array(i_arr, im_src->h + 2);
    
    // Create copy of image array
    struct gspixel** im_dst_im = new_gspx_array(im_dst->h, im_dst->w);
    if(!im_dst_im)
	return;

    im_dst->im = im_dst_im;

    // Second pass
    for (int i = 0; i < im_dst->h; i++) {
	for (int j = 0; j < im_dst->w; j++) {
	    im_dst->im[i][j].gspx = GB3_FACTOR * (p1[i][j].gspx + 2 * p1[i][j + 1].gspx + p1[i][j + 2].gspx);
	}
    }

    free_gspx_array(p1, im_src->h);
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

void
dilation_morphological(struct t_image* im_src, struct t_image* im_dst) {
    im_dst->h = im_src->h;
    im_dst->w = im_src->w;

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

    // Dilation
    for (int i = 1; i < im_src->h - 1; i++) {
	for (int j = 1; j < im_src->w - 1; j++) {
	    if(im_src->im[i][j].r) {
		im_dst->im[i][j].r = im_dst->im[i][j].g = im_dst->im[i][j].b
		    = dilates(im_src->im, i, j);
	    }
	    else
		im_dst->im[i][j].r = im_dst->im[i][j].g = im_dst->im[i][j].b = im_src->im[i][j].r;
	}
    }
}

void
erosion_morphological(struct t_image* im_src, struct t_image* im_dst) {
    im_dst->h = im_src->h;
    im_dst->w = im_src->w;

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

    // Erosion
    for (int i = 1; i < im_src->h - 1; i++) {
	for (int j = 1; j < im_src->w - 1; j++) {
	    if(!im_src->im[i][j].r) {
		im_dst->im[i][j].r = im_dst->im[i][j].g = im_dst->im[i][j].b
		    = erodes(im_src->im, i, j);
	    }
	    else
		im_dst->im[i][j].r = im_dst->im[i][j].g = im_dst->im[i][j].b = im_src->im[i][j].r;
	}
    }
}

void
isolate_number(struct t_image* im_src, struct t_image* im_dst) {

    int tbp; // Total black pixels on x axis per line
    int px_buf = 5; // 5 pixels of buffer
    double h_rat = 0;
    double w_rat = 0;
    
    // Dimensions for the new plate
    int min_h = 0;
    int max_h = im_src->h;
    int min_w = 0;
    int max_w = im_src->w;

    for (int i = 0; i < im_src->h; i++) {
	tbp = 0;
	for (int j = 0; j < im_src->w; j++) {
	    if(!im_src->im[i][j].r)
		tbp++;
	}

	h_rat = ((double) i) / im_src->h;
	w_rat = ((double) tbp) / im_src->w;
	// 0.3221 for letters ratio, 0.22135 for height ratio
	if(w_rat > 0.3211 && (h_rat > 0.22135 && h_rat < 0.2864)) {
	    min_h = i - px_buf;
	    i = (int) (0.664 * im_src->h);
	}
	// 0.035248 for letters ratio, 0.359 for height ratio
	else if(w_rat < 0.035248 && h_rat > 0.359) {
	    max_h = i + px_buf;
	    break;
	}
    }

    // Create new image
    int new_h = max_h - min_h + 1;

    struct t_image im_inter;
    im_inter.h = new_h;
    im_inter.w = im_src->w;

    struct pixel** im_dst_im_inter = new_px_array(new_h, im_src->w);
    if(!im_dst_im_inter)
	return;

    im_inter.im = im_dst_im_inter;

    // Copy the pixels
    for (int i = min_h; i < max_h + 1; i++) {
	for (int j = 0; j < im_src->w; j++) {
	    im_inter.im[i - min_h][j] = im_src->im[i][j];
	}
    }

    // Using the new image to isolate the plate horizontally
    double v_rat = 0;
    double d_rat = 0;
    for (int j = 0; j < im_src->w; j++) {
	tbp = 0;
	for (int i = 0; i < im_inter.h; i++) {
	    if(!im_inter.im[i][j].r)
		tbp++;
	}

	v_rat = ((double) tbp) / im_inter.h;
	d_rat = ((double) j) / im_src->w;
	if(v_rat > 0.03 && (d_rat > 0.03911 && d_rat < 0.13038)) {
	    min_w = j - px_buf;
	    j = (int) (0.855 * im_src->w);
	}
	// 0.035248 for letters ratio, 0.359 for height ratio
	else if(v_rat < 0.03 && d_rat > 0.88657) {
	    max_w = j + px_buf;
	    break;
	}
    }

    // Create new image
    int new_w = max_w - min_w + 1;

    im_dst->h = im_inter.h;
    im_dst->w = new_w;
    struct pixel** im_dst_im = new_px_array(im_inter.h, new_w);

    if(!im_dst_im)
	return;

    im_dst->im = im_dst_im;

    // Copy the pixels
    for (int j = min_w; j < max_w + 1; j++) {
	for (int i = 0; i < im_inter.h; i++) {
	    im_dst->im[i][j - min_w] = im_inter.im[i][j];
	}
    }

    // Free inter. plate
    free_px_array(im_inter.im, im_inter.h);

    // i hate this function
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
