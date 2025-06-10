#include "../includes/pixel.h"
#include <stdlib.h>

void free_px_array(struct pixel** px_arr, const int h) {
    for (int i = 0; i < h; i++) {
	free(px_arr[i]);
	px_arr[i] = NULL;
    }

    free(px_arr);
    px_arr = NULL;
}
