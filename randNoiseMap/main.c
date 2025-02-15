//
//  main.c
//  randNoiseMap
//
//  Created by Kenneth Anderson on 2/14/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PERLIN_SIZE 16

/**
 *  @brief This function is intended to create a 2 dimensional int array with no value inside
 *
 *  @param width The specified width of the map
 *  @param height The specified height of the map
 *  @return returns the array, or NULL if there was an error
 */
int **create2DArray(int width, int height) {
    // double check inputs
    if (height <= 0 || width <= 0) {
        return NULL;
    }
    
    // allocate and double-check memory
    int **arr = (int **)malloc(height * sizeof(int *));
    if (arr == NULL) {
        return NULL;// return memory allocation error
    }
    
    // Allocate the 2nd dimentional memory
    for (int i = 0; i < height; i++) {
        arr[i] = (int *)malloc(width * sizeof(int));
        if (arr[i] == NULL) {
            // if one allocation fails, clean up everything so far
            for (int j = 0; j < i; j++) {
                free(arr[j]);
            }
            free(arr);
            
            return NULL; // return memory allocation error
        }
    }
    
    return arr;
}

// Free and clean up a 2D array after use
void free2DArray(int **arr, int height) {
    if (arr == NULL) return; // dont free if null
    
    for (int i = 0; i < height; i++) {
        free(arr[i]);
    }
    
    free(arr);
}

/**
 *  @brief Function creates and fills a 2 dimensional array with pseudorandom values
 *
 *  The program creates a 2d array throught the "create2DArray" function, and then fills the array with pseudorandom values. There is no pattern or gradient to the variables at all
 *
 *  @param width The specified width of the map, defaults to 8 when left null
 *  @param height The specified height of the map, defaults to 8 when left null
 *  @param print a bool which decides whether the function should print the generated array
 *  @return returns the entire array to be rendered or used later, or NULL if there was an error
 *
 *  @note The program does not render the array
 *  @note It is reccomended not to select print if planning on rendering the values
 */
int **createStaticNoise(int width, int height, _Bool print) {
    int **noiseMap = create2DArray(width, height);
    if (noiseMap == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            noiseMap[i][j] = rand() % 256;
        }
    }
    
    if (print) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                printf("%3d ", noiseMap[i][j]);
            }
            
            printf("\n");
        }
    }
    
    return noiseMap;
}

// Fade function to smooth out values
double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation between a and b
double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

// Compute dot product of gradient vector and distance vector
double grad(int hash, double x, double y) {
    int h = hash & 3;
    double u = h < 2 ? x : y;
    double v = h < 2 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
}

// Perlin noise generation at (x, y)
double perlinNoise(double x, double y, int *perm) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    double u = fade(x);
    double v = fade(y);

    int aa = perm[X] + Y;
    int ab = perm[X] + Y + 1;
    int ba = perm[X + 1] + Y;
    int bb = perm[X + 1] + Y + 1;

    double g1 = grad(perm[aa], x, y);
    double g2 = grad(perm[ba], x - 1, y);
    double g3 = grad(perm[ab], x, y - 1);
    double g4 = grad(perm[bb], x - 1, y - 1);

    double lerpX1 = lerp(g1, g2, u);
    double lerpX2 = lerp(g3, g4, u);
    
    // Map the result from [-1, 1] to [0, 1]
    double result = lerp(lerpX1, lerpX2, v);
    return (result + 1) / 2.0;  // Shift to [0, 1] range
}

/**
 *  @brief Function creates and fills a 2 dimensional array with perlin noise
 *
 *  The program creates a 2d array throught the "perlinNoise" function, and then fills the array with a perlin noise function
 *
 *  @param width The specified width of the map
 *  @param height The specified height of the map
 *  @param print a bool which decides whether the function should print the generated array
 *  @return returns the entire array to be rendered or used later, or NULL if there was an error
 *
 *  @note The program does not render the array
 *  @note It is reccomended not to select print if planning on rendering the values
 */
double **createPerlinNoise(int width, int height, _Bool print) {
    // Generate permutation table
    int *perm = malloc(512 * sizeof(int));
    int p[256];
    for (int i = 0; i < 256; i++) p[i] = i;

    // Shuffle array
    for (int i = 255; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = p[i];
        p[i] = p[j];
        p[j] = temp;
    }

    // Duplicate the permutation array
    for (int i = 0; i < 256; i++) perm[i] = perm[i + 256] = p[i];

    // Generate the Perlin noise map
    double **noiseMap = (double **)malloc(height * sizeof(double *));
    for (int i = 0; i < height; i++) {
        noiseMap[i] = (double *)malloc(width * sizeof(double));
        for (int j = 0; j < width; j++) {
            noiseMap[i][j] = perlinNoise((double)j / PERLIN_SIZE, (double)i / PERLIN_SIZE, perm);
        }
    }

    if (print) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                printf("%5.2f ", noiseMap[i][j]);
            }
            
            printf("\n");
        }
    }
    // Clean up
    free(perm);
    return noiseMap;
}

// free perlin noise map
void freePerlinNoise(double **arr, int height) {
    for (int i = 0; i < height; i++) {
        free(arr[i]);
    }
    free(arr);
}

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(NULL));;
    printf("Generating static noise map:\n\n");
    
    int width = 32, height = 32;
    int **noiseMap = createStaticNoise(width, height, 1); // Generate and print static noise map
    if (noiseMap == NULL) {
        printf("Error, could not complete noiseMap operation.\n");
        return -1;
    }
    
    free2DArray(noiseMap, height);
    
    printf("\nNow for the perlin noise map:\n\n");
    double **perlinMap = createPerlinNoise(width, height, 1); // Generate and print the perlin noise map
    if (perlinMap == NULL) {
        printf("Error, could not complete noiseMap operation.\n");
        return -1;
    }
    
    freePerlinNoise(perlinMap, height);
    
    printf("\n");
    return 0;
}
