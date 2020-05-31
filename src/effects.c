#include <stdio.h>
#include "effects.h"


void heatmap(unsigned int width, unsigned int height, char *target, float *heat)
{
    // find min, max
    float min = heat[0 + (0 * width)];
    float max = heat[0 + (0 * width)];
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (heat[i + (j * width)] < min) {
                min = heat[i + (j * width)];
            }
            if (heat[i + (j * width)] > max) {
                max = heat[i + (j * width)];
            }
        }
    }
    // apply
    float rt;
    char r, g, b;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            rt = 2. * ((heat[i + (j * width)] - min) / (max - min));
            b = (0 >= 255 * (1. - rt)) ? 0 : (char)(255 * (1. - rt));
            r = (0 >= 255 * (rt - 1.)) ? 0 : (char)(255 * (rt - 1.));
            g = 255 - b - r;
            
            target[(4 * i) + (4 * j * width)  + 0] = r;
            target[(4 * i) + (4 * j * width)  + 1] = g;
            target[(4 * i) + (4 * j * width)  + 2] = b;
            target[(4 * i) + (4 * j * width)  + 3] = 255;
        }
    }
}

void horizontal_seam(unsigned int width, unsigned int height, char *target, int *designated)
{
    for (int i = 0; i < width; i++) {
        target[(4 * i) + (4 * designated[i] * width)  + 0] = 255;
        target[(4 * i) + (4 * designated[i] * width)  + 1] = 69;
        target[(4 * i) + (4 * designated[i] * width)  + 2] = 0;
    }
}

void vertical_seam(unsigned int width, unsigned int height, char *target, int *designated)
{
    for (int j = 0; j < height; j++) {
        target[(4 * designated[j]) + (4 * j * width)  + 0] = 255;
        target[(4 * designated[j]) + (4 * j * width)  + 1] = 69;
        target[(4 * designated[j]) + (4 * j * width)  + 2] = 0;
    }
}




