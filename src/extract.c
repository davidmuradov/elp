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
    /*
    f_write("screenshots/plgauss.ppm", &gauss_image);

    // Use Sobel operator on image
    struct t_image sobel_image;
    sobel_filter(&gauss_image, &sobel_image);
    f_write("screenshots/plsobel.ppm", &sobel_image);

    // Use simple thresholding
    struct t_image t_image;
    threshold(&gauss_image, &t_image);
    f_write("screenshots/plthreshold.ppm", &t_image);

    // Use dilation
    struct t_image dilated_image;
    dilation_morphological(&t_image, &dilated_image);
    f_write("screenshots/pldilate.ppm", &dilated_image);

    // Use erosion
    struct t_image eroded_image;
    erosion_morphological(&dilated_image, &eroded_image);
    f_write("screenshots/plerode.ppm", &eroded_image);

    // At this point, plate is morphologicaly closed
    // Isolate the plate vertically and horizontally (very basic version)
    struct t_image isolated_image;
    isolate_number(&eroded_image, &isolated_image);
    f_write("screenshots/plisolate.ppm", &isolated_image);

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
