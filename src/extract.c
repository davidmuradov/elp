#include "../includes/extract.h"
#include "pixel.h"

char*
extract_plate(struct pixel** plate, const int h, const int w) {

    // Turn image to grayscale version
    for (int i = 0; i < h; i++) {
	for (int j = 0; j < w; j++) {
	    grayscale_px(&(plate[i][j]));
	}
    }
    f_write("screenshots/plgray.ppm", plate, h, w);

    // Use gaussian blur on image
    struct pixel** gauss_plate = gaussian_blur_3(plate, h, w);
    if(!gauss_plate) {
	free_px_array(plate, h);
	exit(EXIT_FAILURE);
    }
    f_write("screenshots/plgauss.ppm", gauss_plate, h, w);

    // Use Sobel operator on image
    struct pixel** sobel_plate = sobel(gauss_plate, h, w);
    if(!sobel_plate) {
	free_px_array(plate, h);
	free_px_array(gauss_plate, h);
	exit(EXIT_FAILURE);
    }
    f_write("screenshots/plsobel.ppm", sobel_plate, h, w);

    // Use simple thresholding
    struct pixel** t_plate = threshold(gauss_plate, h, w);
    if(!t_plate) {
	free_px_array(plate, h);
	free_px_array(gauss_plate, h);
	free_px_array(sobel_plate, h);
	exit(EXIT_FAILURE);
    }
    f_write("screenshots/plthreshold.ppm", t_plate, h, w);

    // Use dilation
    struct pixel** d_plate = morph_dilation(t_plate, h, w);
    if(!d_plate) {
	free_px_array(plate, h);
	free_px_array(gauss_plate, h);
	free_px_array(sobel_plate, h);
	free_px_array(t_plate, h);
	exit(EXIT_FAILURE);
    }
    f_write("screenshots/pldilate.ppm", d_plate, h, w);

    // Use erosion
    struct pixel** e_plate = morph_erosion(d_plate, h, w);
    if(!e_plate) {
	free_px_array(plate, h);
	free_px_array(gauss_plate, h);
	free_px_array(sobel_plate, h);
	free_px_array(t_plate, h);
	free_px_array(d_plate, h);
	exit(EXIT_FAILURE);
    }
    f_write("screenshots/plerode.ppm", e_plate, h, w);

    // At this point, plate is morphologicaly closed

    free_px_array(plate, h);
    free_px_array(gauss_plate, h);
    free_px_array(sobel_plate, h);
    free_px_array(t_plate, h);
    free_px_array(d_plate, h);
    free_px_array(e_plate, h);

    return "returned";
}
