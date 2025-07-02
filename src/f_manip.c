#include "../includes/f_manip.h"
#include "../includes/pixel.h"

void f_read(char* pathname, struct t_image* image) {

    FILE* f = fopen(pathname, "rb");
    if(!f) {
	fprintf(stderr, "Error: Could not load image file\n");
	return;
    }

    // Read header of .ppm file, verify if it's P6
    char buf[4];
    fgets(buf, sizeof(buf), f);
    if(strcmp(buf, P6_HEADER)) {
	fprintf(stderr, ERR_NOT_P6);
	fclose(f);
	return;
    }

    image->w = 0; // Width of picture
    image->h = 0; // Height of picture
    
    char dimensions[14]; // This allows for a max resolution of 999999x999999 px
    fgets(dimensions, sizeof(dimensions), f); // Read dimensions line in file

    char* tok_w = strtok(dimensions, " ");
    char* tok_h = strtok(NULL, "\n");

    // Convert tok_w, tok_h to integers
    // strtol can return error codes through errno from errno.h.
    // They should be checked (TODO)
    char* end;
    image->w = (int) strtol(tok_w, &end, BASE_10);
    image->h = (int) strtol(tok_h, &end, BASE_10);

    // Allocate memory for 2D array converted picture
    // array would be of form IMAGE[height][width]
    struct pixel** v1_plate = new_px_array(image->h, image->w);

    // Verify max color value (we want 255 for now,
    // but we could change that later)
    char max_comp[5];
    fgets(max_comp, sizeof(max_comp), f);
    int m_comp = (int) strtol(max_comp, &end, BASE_10);
    if(m_comp != 0xFF) {
	fprintf(stderr, "Error: max pixel component value is not 255\n");
	fclose(f);
	free_px_array(v1_plate, image->h);
	return;
    }

    // Read RGB color for every pixel in image and store in v1_plate
    for (int i = 0; i < image->h; i++) {
	for (int j = 0; j < image->w; j++) {
	    fread(&(v1_plate[i][j].r), 1, 1, f);
	    fread(&(v1_plate[i][j].g), 1, 1, f);
	    fread(&(v1_plate[i][j].b), 1, 1, f);
	}
    }

    // Processing done, close the file
    fclose(f);

    image->im = v1_plate;
}

int f_write(char* pathname, struct pixel** arr, const int height, const int width) {
    FILE* f = fopen(pathname, "wb");
    if (!f) {
	perror("Failed to open file for writing: %s");
	return 0;
    }

    // Write the PPM header
    fprintf(f, "P6\n%d %d\n255\n", width, height);

    // Write the binary pixel data
    for (int i = 0; i < height; ++i) {
	for (int j = 0; j < width; ++j) {
	    fwrite(&arr[i][j], sizeof(struct pixel), 1, f);
	}
    }

    fclose(f);
    return 1;
}
