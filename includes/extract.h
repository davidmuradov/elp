/// @file extract.h
#ifndef INCLUDES_EXTRACT_H
#define INCLUDES_EXTRACT_H

#include "pixel.h"
#include "f_manip.h"

/**
 * Main function. Extract the plate number and return it as string.
 */
char*
extract_plate(struct t_image* image);

#endif
