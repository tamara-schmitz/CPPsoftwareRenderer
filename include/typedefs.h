#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// this files contains some typedefs used throughout the program
using namespace std;

struct vertex2D
{
    // a structure that holds a 2D vertex
    float x;
    float y;


};

struct pixel
{
    // a structure that represents a pixel on a screen
    int x;
    int y;
};

#endif // TYPEDEFS_H
