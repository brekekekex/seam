#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "seam_carver.h"

#define NONVIABLE 9999999999.

float CLIJ(int i, int j, unsigned int width, unsigned int height, float *energy_matrix)
{
    return fabs(energy_matrix[i + ((j + 1) * width)] - energy_matrix[i + ((j - 1) * width)]) + 
    fabs(energy_matrix[(i - 1) + (j * width)] - energy_matrix[i + (j * width)]);
}

float CUIJ(int i, int j, unsigned int width, unsigned int height, float *energy_matrix)
{
    return fabs(energy_matrix[i + ((j + 1) * width)] - energy_matrix[i + ((j - 1) * width)]);
}

float CRIJ(int i, int j, unsigned int width, unsigned int height, float *energy_matrix)
{
 
    return fabs(energy_matrix[i + ((j + 1) * width)] - energy_matrix[i + ((j - 1) * width)]) + 
    fabs(energy_matrix[(i - 1) + (j * width)] - energy_matrix[i + ((j + 1) * width)]);
}

void seam_carve(unsigned int width, unsigned int height, float *energy_matrix, int *targets)
{
    float a, b, c;

    float *cost_matrix = malloc(sizeof(float) * width * height);
    assert(cost_matrix != NULL);

    // initialize left edge
    cost_matrix[0 + (0 * width)] = NONVIABLE;
    cost_matrix[0 + ((height - 1) * width)] = NONVIABLE;
    for (int j = 1; j < height - 1; j++) {
        cost_matrix[0 + (j * width)] = 100; // seed edge
    }
    // compute accumulation matrix to right edge (guard top and bottom edges)
    for (int i = 1; i < width; i++) {
        for (int j = 0; j < height; j++) {
            a = (j == 0 || j == (height - 1)) ? NONVIABLE : cost_matrix[(i-1) + ((j - 1) * width)] + CLIJ(i, j, width, height, energy_matrix);
            b = (j == 0 || j == (height - 1)) ? NONVIABLE : cost_matrix[(i-1) + (j * width)] + CUIJ(i, j, width, height, energy_matrix);
            c = (j == 0 || j == (height - 1)) ? NONVIABLE : cost_matrix[(i-1) + ((j + 1) * width)] + CRIJ(i, j, width, height, energy_matrix);
            cost_matrix[i + (j * width)] = fminf(a, fminf(b, c));
        }
    }
    // backtrace and identify targets
    // first find exit pixel (in last col)
    float min_val = cost_matrix[(width - 1) + (1 * width)];
    int min_ind = 1;
    for (int j = 1; j < (height - 1); j++) {
        if (cost_matrix[(width - 1) + (j * width)] < min_val) {
            min_val = cost_matrix[(width - 1) + (j * width)];
            min_ind = j;
        }
    }
    
    // add pixel to target buffer
    targets[width - 1] = min_ind;
    
    // trace
    float D, S, U;
    for (int i = width - 2; i != 0; i--) {
        S = cost_matrix[(i - 1) + ((min_ind) * width)]; 
        D = cost_matrix[(i - 1) + ((min_ind + 1) * width)];
        U = cost_matrix[(i - 1) + ((min_ind - 1) * width)]; 
        if (S <= D && S <= U) {
            targets[i] = min_ind;
        } else if (D < U && D < S) {
            min_ind++;
            targets[i] = min_ind;
        } else if (U < D && U < S) {
            min_ind--;
            targets[i] = min_ind;
        } else {
            targets[i] = min_ind;
        }
    }
    targets[0] = targets[1]; 
}


void carve_vertical(unsigned int width, unsigned int height, char *target, char *source, int *seam)
{
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < seam[j]; i++) {
            target[(4 * i) + (4 * j * (width - 1)) + 0] = source[(4 * i) + (4 * j * width) + 0] ;
            target[(4 * i) + (4 * j * (width - 1)) + 1] = source[(4 * i) + (4 * j * width) + 1] ; 
            target[(4 * i) + (4 * j * (width - 1)) + 2] = source[(4 * i) + (4 * j * width) + 2] ; 
            target[(4 * i) + (4 * j * (width - 1)) + 3] = source[(4 * i) + (4 * j * width) + 3] ;  
        }
        for (int i = seam[j] + 1; i < width; i++) {
            target[(4 * (i - 1)) + (4 * j * (width - 1)) + 0] = source[(4 * i) + (4 * j * width) + 0] ;
            target[(4 * (i - 1)) + (4 * j * (width - 1)) + 1] = source[(4 * i) + (4 * j * width) + 1] ; 
            target[(4 * (i - 1)) + (4 * j * (width - 1)) + 2] = source[(4 * i) + (4 * j * width) + 2] ; 
            target[(4 * (i - 1)) + (4 * j * (width - 1)) + 3] = source[(4 * i) + (4 * j * width) + 3] ;  
        }
    }
}

void carve_horizontal(unsigned int width, unsigned int height, char *target, char *source, int *seam)
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < seam[i]; j++) {
            target[(4 * i) + (4 * j * (width - 0)) + 0] = source[(4 * i) + (4 * j * width) + 0] ;
            target[(4 * i) + (4 * j * (width - 0)) + 1] = source[(4 * i) + (4 * j * width) + 1] ; 
            target[(4 * i) + (4 * j * (width - 0)) + 2] = source[(4 * i) + (4 * j * width) + 2] ; 
            target[(4 * i) + (4 * j * (width - 0)) + 3] = source[(4 * i) + (4 * j * width) + 3] ;  
        }

        for (int j = seam[i] + 1; j < height; j++) {
            target[(4 * (i - 0)) + (4 * (j - 1) * (width - 0)) + 0] = source[(4 * i) + (4 * j * width) + 0] ;
            target[(4 * (i - 0)) + (4 * (j - 1) * (width - 0)) + 1] = source[(4 * i) + (4 * j * width) + 1] ; 
            target[(4 * (i - 0)) + (4 * (j - 1) * (width - 0)) + 2] = source[(4 * i) + (4 * j * width) + 2] ; 
            target[(4 * (i - 0)) + (4 * (j - 1) * (width - 0)) + 3] = source[(4 * i) + (4 * j * width) + 3] ;  
        }
        
    }
}

unsigned char *execute_carve_horizontal(unsigned int s_width, unsigned int s_height, char *source, int *targets)
{
    unsigned char *resized = malloc(sizeof(char) * 4 * s_width * (s_height - 1));
    assert(resized != NULL);

    for (int i = 0; i < s_width; i++) {
        int j = 0;
        for (; j < targets[i]; j++) {
            resized[(4 * i) + (4 * j * s_width) + 0] = source[(4 * i) + (4 * j * s_width) + 0];
            resized[(4 * i) + (4 * j * s_width) + 1] = source[(4 * i) + (4 * j * s_width) + 1];
            resized[(4 * i) + (4 * j * s_width) + 2] = source[(4 * i) + (4 * j * s_width) + 2];
            resized[(4 * i) + (4 * j * s_width) + 3] = source[(4 * i) + (4 * j * s_width) + 3];
        }
        j++;
        for (; j < s_height; j++) {
            resized[(4 * i) + (4 * (j - 1) * s_width) + 0] = source[(4 * i) + (4 * j * s_width) + 0];
            resized[(4 * i) + (4 * (j - 1) * s_width) + 1] = source[(4 * i) + (4 * j * s_width) + 1];
            resized[(4 * i) + (4 * (j - 1) * s_width) + 2] = source[(4 * i) + (4 * j * s_width) + 2];
            resized[(4 * i) + (4 * (j - 1) * s_width) + 3] = source[(4 * i) + (4 * j * s_width) + 3];
        }
    }
    return resized;
}

unsigned char *execute_carve_vertical(unsigned int s_width, unsigned int s_height, char *source, int *targets)
{
    unsigned char *resized = malloc(sizeof(char) * 4 * (s_width - 1) * s_height);
    assert(resized != NULL);

    for (int j = 0; j < s_height; j++) {
        int i = 0;
        for (; i < targets[j]; i++) {
            resized[(4 * i) + (4 * j * (s_width - 1)) + 0] = source[(4 * i) + (4 * j * s_width) + 0];
            resized[(4 * i) + (4 * j * (s_width - 1)) + 1] = source[(4 * i) + (4 * j * s_width) + 1];
            resized[(4 * i) + (4 * j * (s_width - 1)) + 2] = source[(4 * i) + (4 * j * s_width) + 2];
            resized[(4 * i) + (4 * j * (s_width - 1)) + 3] = source[(4 * i) + (4 * j * s_width) + 3];
        }
        i++;
        for (; i < s_width; i++) {
            resized[(4 * (i - 1)) + (4 * j * (s_width - 1)) + 0] = source[(4 * i) + (4 * j * s_width) + 0];
            resized[(4 * (i - 1)) + (4 * j * (s_width - 1)) + 1] = source[(4 * i) + (4 * j * s_width) + 1];
            resized[(4 * (i - 1)) + (4 * j * (s_width - 1)) + 2] = source[(4 * i) + (4 * j * s_width) + 2];
            resized[(4 * (i - 1)) + (4 * j * (s_width - 1)) + 3] = source[(4 * i) + (4 * j * s_width) + 3];
        }
    }
    return resized;
}
