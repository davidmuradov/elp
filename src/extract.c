#include "../includes/extract.h"
#include "pixel.h"

char*
extract_plate(struct t_image* image) {

    // Turn image to grayscale version
    grayscale_filter(image);
    f_write("screenshots/plgray.ppm", image);

    // Use gaussian blur on image
    struct t_image gauss_image;
    gaussian_blur3_filter(image, &gauss_image);
    f_write("screenshots/plgauss.ppm", &gauss_image);

    // Use Sobel operator on image
    struct t_image sobel_image;
    sobel_filter(&gauss_image, &sobel_image);
    f_write("screenshots/plsobel.ppm", &sobel_image);

    /*
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
    // Isolate the plate vertically and horizontally (very basic version)
    struct pixel** iso_plate = isolate_number(e_plate, &h, &w);
    if(!iso_plate) {
	free_px_array(plate, h);
	free_px_array(gauss_plate, h);
	free_px_array(sobel_plate, h);
	free_px_array(t_plate, h);
	free_px_array(d_plate, h);
	free_px_array(e_plate, h);
	exit(EXIT_FAILURE);
    }
    f_write("screenshots/plisolate.ppm", iso_plate, h, w);

    free_px_array(plate, h);
    free_px_array(gauss_plate, h);
    free_px_array(sobel_plate, h);
    free_px_array(t_plate, h);
    free_px_array(d_plate, h);
    free_px_array(e_plate, h);
    free_px_array(iso_plate, h);
    */

    return "returned";
}
