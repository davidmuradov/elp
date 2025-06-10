/// @file f_manip.h
#ifndef INCLUDES_F_MANIP_H
#define INCLUDES_F_MANIP_H

#include <stdio.h>
#include <string.h>

#include "pixel.h"

// PPM file, binary format
#define P6_HEADER	"P6\n"
#define ERR_NOT_P6	"Error: file is not a .ppm file of binary format (P6 FORMAT)\n"

#define BASE_10		10

/** \fn int ft_read(char* pathname)
 *
 * Read a file.
 */
struct pixel** f_read(char* pathname, int* height, int* width);
#endif
