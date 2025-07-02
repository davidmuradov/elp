/// @file main.c
#include <getopt.h>
#include <stdio.h>

#include "../includes/extract.h"
#include "f_manip.h"
#include "pixel.h"

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

    // Read image, write to t_image structure
    struct t_image plate;
    f_read(argv[optind], &plate);

    char* p_number = extract_plate(&plate);

    return EXIT_SUCCESS;
}

static void print_help(void) {
    printf("elp (Extract Licence Plate). Usage:\n");
}
