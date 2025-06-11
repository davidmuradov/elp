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

    free_px_array(plate, h);
    free_px_array(gauss_plate, h);

    return "returned";
}
