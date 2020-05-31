#ifndef CARVER_H
#define CARVER_H

void seam_carve(unsigned int width, unsigned int height, float *energy_matrix, int *targets);
//void carve_vertical(unsigned int width, unsigned int height, char *target, char *source, int *seam);
//void carve_horizontal(unsigned int width, unsigned int height, char *target, char *source, int *seam);
unsigned char *execute_carve_horizontal(unsigned int s_width, unsigned int s_height, char *source, int *targets);
unsigned char *execute_carve_vertical(unsigned int s_width, unsigned int s_height, char *source, int *targets);
#endif