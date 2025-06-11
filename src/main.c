/// @file main.c
#include <getopt.h>
#include <stdio.h>

#include "../includes/extract.h"

#define OPTIONS_LIST	"h?"

static void print_help(void);

/** \brief A brief description for the main function.
 * Some random description
 */
int main(int argc, char *argv[]) {

    int opt = 0;

    while((opt = getopt(argc, argv, OPTIONS_LIST)) != -1) {
	switch (opt) {
	    case 'h':
		print_help();
		exit(EXIT_SUCCESS);
	    case '?':
		fprintf(stderr, "Try 'elp -h' for more information.\n");
		exit(EXIT_SUCCESS);
	}
    }

    int v1_plate_h = 0; // Number of pixels of height of image to analyze
    int v1_plate_w = 0; // Number of pixels of width of image to analyze

    // Convert initial picture to 2d array of struct pixel
    struct pixel** v1_plate = f_read(argv[optind], &v1_plate_h, &v1_plate_w);
    if(!v1_plate) {
	exit(EXIT_FAILURE);
    }

    extract_plate(v1_plate, v1_plate_h, v1_plate_w);

    //free_px_array(&v1_plate, v1_plate_h);

    return EXIT_SUCCESS;
}

static void print_help(void) {
    printf("elp (Extract Licence Plate). Usage:\n");
}
