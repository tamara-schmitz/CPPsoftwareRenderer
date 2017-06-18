#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <SDL2/SDL.h>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::weak_ptr;

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

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

template< typename I, typename T >
const I triangleArea( const T& v1, const T& v2, const T& v3 )
{
    const I x1 = v2.x - v1.x;
    const I y1 = v2.y - v1.y;

    const I x2 = v3.x - v1.x;
    const I y2 = v3.y - v1.y;

    return (I) ( 0.5 * (x1 * y2 - x2 * y1) );
}

#endif // COMMON_H
