#include "common.h"

bool compSDL_Color( SDL_Color c1, SDL_Color c2 )
{
    if ( c1.a == c2.a &&
         c1.b == c2.b &&
         c1.g == c2.g &&
         c1.r == c2.r )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool compSDL_Point( SDL_Point p1, SDL_Point p2 )
{
    if ( p1.x == p2.x &&
         p1.y == p2.y )
    {
        return true;
    }
    else
    {
        return false;
    }
}

Uint32 getPixelFor_SDLColor( const SDL_Color* colour )
{
    // assumes that pixelformat is ARGB8888
    return colour->a << 24 | colour->r << 16 | colour->g << 8 | colour->b;
}

Uint32 getPixelOn_SDLSurface(SDL_Surface* surface, Uint16 x, Uint16 y)
{
    Uint32 offset = y * surface->w + x;

    return getPixelOn_SDLSurface( surface, offset );
}

Uint32 getPixelOn_SDLSurface(SDL_Surface* surface, Uint32 i)
{
    int bpp = surface->format->BytesPerPixel; // bpp = bytes per pixel

    // p = pixel we want to get
    Uint8 *p = &( (Uint8 *)surface->pixels )[i * bpp];

    switch (bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;
    }
}
