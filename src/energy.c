#include <stdio.h>
#include <math.h>
#include <assert.h>

#define BEYOND 255

void sobel(unsigned char *rgba, unsigned int width, unsigned int height, float *energy_matrix)
{
    // window

    float r1, g1, b1, a1;   float r2, g2, b2, a2;   float r3, g3, b3, a3;
    
    
    
    float r4, g4, b4, a4;   float r5, g5, b5, a5;   float r6, g6, b6, a6;
    
    
    
    float r7, g7, b7, a7;   float r8, g8, b8, a8;   float r9, g9, b9, a9;

    // S_x

    int Sx1 = 1;  int Sx2 = 0;  int Sx3 = -1;

    int Sx4 = 2;  int Sx5 = 0;  int Sx6 = -2;
    
    int Sx7 = 1;  int Sx8 = 0;  int Sx9 = -1;
    
    // S_y

    int Sy1 = 1;  int Sy2 = 2;  int Sy3 = 1;

    int Sy4 = 0;  int Sy5 = 0;  int Sy6 = 0;
    
    int Sy7 = -1;  int Sy8 = -2;  int Sy9 = -1;

    // grad
    float rDx, gDx, bDx, aDx;
    float rDy, gDy, bDy, aDy;
    float rD, gD, bD, aD;
    float g;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            
            r1 = (i == 0 || j == 0) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j - 1) * width) + 0];
            g1 = (i == 0 || j == 0) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j - 1) * width) + 1];
            b1 = (i == 0 || j == 0) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j - 1) * width) + 2];
            a1 = (i == 0 || j == 0) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j - 1) * width) + 3];

            r2 = (j == 0) ? BEYOND : rgba[(4 * (i - 0)) + (4 * (j - 1) * width) + 0];
            g2 = (j == 0) ? BEYOND : rgba[(4 * (i - 0)) + (4 * (j - 1) * width) + 1];
            b2 = (j == 0) ? BEYOND : rgba[(4 * (i - 0)) + (4 * (j - 1) * width) + 2];
            a2 = (j == 0) ? BEYOND : rgba[(4 * (i - 0)) + (4 * (j - 1) * width) + 3];

            r3 = (j == 0 || i == (width - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j - 1) * width) + 0];
            g3 = (j == 0 || i == (width - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j - 1) * width) + 1];
            b3 = (j == 0 || i == (width - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j - 1) * width) + 2];
            a3 = (j == 0 || i == (width - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j - 1) * width) + 3];

            r4 = (i == 0) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j - 0) * width) + 0];
            g4 = (i == 0) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j - 0) * width) + 1];
            b4 = (i == 0) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j - 0) * width) + 2];
            a4 = (i == 0) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j - 0) * width) + 3];

            r5 = rgba[(4 * (i - 0)) + (4 * (j - 0) * width) + 0];
            g5 = rgba[(4 * (i - 0)) + (4 * (j - 0) * width) + 1];
            b5 = rgba[(4 * (i - 0)) + (4 * (j - 0) * width) + 2];
            a5 = rgba[(4 * (i - 0)) + (4 * (j - 0) * width) + 3];

            r6 = (i == (width - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j - 0) * width) + 0];
            g6 = (i == (width - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j - 0) * width) + 1];
            b6 = (i == (width - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j - 0) * width) + 2];
            a6 = (i == (width - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j - 0) * width) + 3];

            r7 = (i == 0 || j == (height - 1)) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j + 1) * width) + 0];
            g7 = (i == 0 || j == (height - 1)) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j + 1) * width) + 1];
            b7 = (i == 0 || j == (height - 1)) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j + 1) * width) + 2];
            a7 = (i == 0 || j == (height - 1)) ? BEYOND : rgba[(4 * (i - 1)) + (4 * (j + 1) * width) + 3];

            r8 = (j == (height - 1)) ? BEYOND : rgba[(4 * (i - 0)) + (4 * (j + 1) * width) + 0];
            g8 = (j == (height - 1)) ? BEYOND : rgba[(4 * (i - 0)) + (4 * (j + 1) * width) + 1];
            b8 = (j == (height - 1)) ? BEYOND : rgba[(4 * (i - 0)) + (4 * (j + 1) * width) + 2];
            a8 = (j == (height - 1)) ? BEYOND : rgba[(4 * (i - 0)) + (4 * (j + 1) * width) + 3];

            r9 = (i == (width - 1) || j == (height - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j + 1) * width) + 0];
            g9 = (i == (width - 1) || j == (height - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j + 1) * width) + 1];
            b9 = (i == (width - 1) || j == (height - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j + 1) * width) + 2];
            a9 = (i == (width - 1) || j == (height - 1)) ? BEYOND : rgba[(4 * (i + 1)) + (4 * (j + 1) * width) + 3];
    
            rDx = (r1 * Sx1) + (r2 * Sx2) + (r3 * Sx3) + (r4 * Sx4) + (r5 * Sx5) + (r6 * Sx6) + (r7 * Sx7) + (r8 * Sx8) + (r9 * Sx9);        
            gDx = (g1 * Sx1) + (g2 * Sx2) + (g3 * Sx3) + (g4 * Sx4) + (g5 * Sx5) + (g6 * Sx6) + (g7 * Sx7) + (g8 * Sx8) + (g9 * Sx9);        
            bDx = (b1 * Sx1) + (b2 * Sx2) + (b3 * Sx3) + (b4 * Sx4) + (b5 * Sx5) + (b6 * Sx6) + (b7 * Sx7) + (b8 * Sx8) + (b9 * Sx9);        
            aDx = (a1 * Sx1) + (a2 * Sx2) + (a3 * Sx3) + (a4 * Sx4) + (a5 * Sx5) + (a6 * Sx6) + (a7 * Sx7) + (a8 * Sx8) + (a9 * Sx9);        
            
            rDy = (r1 * Sy1) + (r2 * Sy2) + (r3 * Sy3) + (r4 * Sy4) + (r5 * Sy5) + (r6 * Sy6) + (r7 * Sy7) + (r8 * Sy8) + (r9 * Sy9);        
            gDy = (g1 * Sy1) + (g2 * Sy2) + (g3 * Sy3) + (g4 * Sy4) + (g5 * Sy5) + (g6 * Sy6) + (g7 * Sy7) + (g8 * Sy8) + (g9 * Sy9);        
            bDy = (b1 * Sy1) + (b2 * Sy2) + (b3 * Sy3) + (b4 * Sy4) + (b5 * Sy5) + (b6 * Sy6) + (b7 * Sy7) + (b8 * Sy8) + (b9 * Sy9);        
            aDy = (a1 * Sy1) + (a2 * Sy2) + (a3 * Sy3) + (a4 * Sy4) + (a5 * Sy5) + (a6 * Sy6) + (a7 * Sy7) + (a8 * Sy8) + (a9 * Sy9);        
            
            rD = (float)sqrt(pow(rDx, 2) + pow(rDy, 2));
            gD = (float)sqrt(pow(gDx, 2) + pow(gDy, 2));
            bD = (float)sqrt(pow(bDx, 2) + pow(bDy, 2));
            aD = (float)sqrt(pow(aDx, 2) + pow(aDy, 2));

            g = (rD + gD + bD + aD) / 4.;
             
            energy_matrix[i + (j * width)] = g; 
        }
    }
}

float *energy_transpose(unsigned int width, unsigned int height, float *source)
{
    float *energy_map = malloc(sizeof(float) * width * height);
    assert(energy_map != NULL);

    for (int i = 0 ; i < width; i++){
        for (int j = 0; j < height; j++) {
            energy_map[j + (i * height)] = source[i + (j * width)];
        }
    }

    return energy_map;
}