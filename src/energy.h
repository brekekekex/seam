#ifndef ENERGY_H
#define ENERGY_H

void sobel(unsigned char *rgba, unsigned int width, unsigned int height, float *energy_matrix);
float *energy_transpose(unsigned int width, unsigned int height, float *source);

#endif