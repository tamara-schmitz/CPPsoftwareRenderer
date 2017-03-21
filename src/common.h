#ifndef COMMON_H
#define COMMON_H

#include <SDL2/SDL.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// SDL compare functions
bool compSDL_Color( SDL_Color c1, SDL_Color c2 );
bool compSDL_Point( SDL_Point p1, SDL_Point p2 );

// Math functions
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



#endif // COMMON_H
