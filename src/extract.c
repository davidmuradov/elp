#include "../includes/extract.h"

char*
extract_plate(struct pixel** plate, const int h, const int w) {

    // Turn image to grayscale version
    for (int i = 0; i < h; i++) {
	for (int j = 0; j < w; j++) {
	    grayscale_px(&(plate[i][j]));
	}
    }
    f_write("plgray.ppm", plate, h, w);

    // Use gaussian blur on image
    gaussian_blur_3(plate, h, w);
    f_write("plgauss.ppm", plate, h, w);

    return "returned";
}
