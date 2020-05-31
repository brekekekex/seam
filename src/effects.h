#ifndef EFFECTS_H
#define EFFECT_H

void heatmap(unsigned int width, unsigned int height, char *target, float *heat);
void horizontal_seam(unsigned int width, unsigned int height, char *target, int *designated);
void vertical_seam(unsigned int width, unsigned int height, char *target, int *designated);

#endif