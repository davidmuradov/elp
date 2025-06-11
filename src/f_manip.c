#include "../includes/f_manip.h"
#include "../includes/pixel.h"

struct pixel** f_read(char* pathname, int* height, int* width) {

    FILE* f = fopen(pathname, "rb");
    if(!f) {
	fprintf(stderr, "Error: Could not load image file\n");
	return NULL;
    }

    // Read header of .ppm file, verify if it's P6
    char buf[4];
    fgets(buf, sizeof(buf), f);
    if(strcmp(buf, P6_HEADER)) {
	fprintf(stderr, ERR_NOT_P6);
	fclose(f);
	return NULL;
    }

    int w = 0; // Width of picture
    int h = 0; // Height of picture
    
    char dimensions[14]; // This allows for a max resolution of 999999x999999 px
    fgets(dimensions, sizeof(dimensions), f); // Read dimensions line in file

    char* tok_w = strtok(dimensions, " ");
    char* tok_h = strtok(NULL, "\n");

    // Convert tok_w, tok_h to integers
    // strtol can return error codes through errno from errno.h.
    // They should be checked (TODO)
    char* end;
    w = (int) strtol(tok_w, &end, BASE_10);
    h = (int) strtol(tok_h, &end, BASE_10);

    // Allocate memory for 2D array converted picture
    // array would be of form IMAGE[height][width]
    struct pixel** v1_plate = new_px_array(h, w);

    // Verify max color value (we want 255 for now,
    // but we could change that later)
    char max_comp[5];
    fgets(max_comp, sizeof(max_comp), f);
    int m_comp = (int) strtol(max_comp, &end, BASE_10);
    if(m_comp != 0xFF) {
	fprintf(stderr, "Error: max pixel component value is not 255\n");
	fclose(f);
	free_px_array(v1_plate, h);
	return NULL;
    }

    // Read RGB color for every pixel in image and store in v1_plate
    for (int i = 0; i < h; i++) {
	for (int j = 0; j < w; j++) {
	    fread(&(v1_plate[i][j].r), 1, 1, f);
	    fread(&(v1_plate[i][j].g), 1, 1, f);
	    fread(&(v1_plate[i][j].b), 1, 1, f);
	}
    }

    // Processing done, close the file
    fclose(f);

    // Set parameters to image size values
    *height = h;
    *width = w;

    return v1_plate;
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
