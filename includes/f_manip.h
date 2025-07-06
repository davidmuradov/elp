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

/** \fn int f_read(char* pathname)
 *
 * Read a file.
 */
void f_read(char* pathname, struct t_image* image);

/** \fn int f_write(char* pathname, const int height, const int width)
 *
 * Write a struct t_image* into a P6 formatted .ppm file.
 * Return 1 on success, 0 otherwise.
 */
int f_write(char* pathname, struct t_gsimage* image);
#endif
