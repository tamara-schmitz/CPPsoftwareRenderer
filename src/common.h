#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <limits>

#include <SDL2/SDL.h>

#include "vmath-0.12/vmath.h"

using std::cout;
using std::endl;
using std::shared_ptr;
using std::weak_ptr;

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Global vars
extern int current_demo_index;
extern bool printDebug;
extern bool headlessMode;
extern bool testMode;
extern bool slowRendering;
extern bool ignoreZBuffer;

// SDL specific functions
bool compSDL_Color( SDL_Color c1, SDL_Color c2 );
bool compSDL_Point( SDL_Point p1, SDL_Point p2 );
Uint32 getPixelFor_SDLColor( const SDL_Color* colour );
Uint32 getPixelOn_SDLSurface( SDL_Surface *surface, Uint32 i );
Uint32 getPixelOn_SDLSurface( SDL_Surface *surface, Uint16 x, Uint16 y );

// Math functions
template< typename I >
const I& clipNumber( const I& i, const I& iMin )
{
    if ( i < iMin )
    {
        return iMin;
    }
    else
    {
        return i;
    }
}
template< typename I >
const I& clipNumber( const I& i, const I& iMin, const I& iMax )
{
    if ( i < iMin )
    {
        return iMin;
    }
    else if ( i > iMax )
    {
        return iMax;
    }
    else
    {
        return i;
    }
}
template< typename I >
const bool AlmostEqual( const I& a, const I& b )
{
    I epsilon = 4.88e-04;
    I diff = fabs( fabs(a) - fabs(b) );

    if ( a == b ) // in case that's actually true
    {
        return true;
    }
    else if ( a == 0 || b == 0 || diff < std::numeric_limits<I>::lowest() )
    {
        // a or b close to zero, hence relative error is less relevant
        return diff < ( epsilon * std::numeric_limits<I>::lowest() );
    }
    else
    {
        // use relative error
        return diff / std::min<I>( ( fabs(a) + fabs(b) ), std::numeric_limits<I>::max() );
    }

}

template< typename I, typename T >
const I triangleArea( const T& v1, const T& v2, const T& v3 )
{
    const I x1 = v2.x - v1.x;
    const I y1 = v2.y - v1.y;

    const I x2 = v3.x - v1.x;
    const I y2 = v3.y - v1.y;

    return (I) ( (x1 * y2 - x2 * y1) / 2 );
}

template< typename I >
const Vector3< I > calculateNormal( const Vector4< I >& v1, const Vector4< I >& v2 )
{
    Vector3< I > normal_vec = Vector3< I >();
    // calculate cross product of 2 vector edges
    normal_vec.x = v1.y * v2.z - v1.z * v2.y;
    normal_vec.y = v1.z * v2.x - v1.x * v2.z;
    normal_vec.z = v1.x * v2.y - v1.y * v2.x;
    // normalise vector
    normal_vec.normalize();

    return normal_vec;
}

#endif // COMMON_H
