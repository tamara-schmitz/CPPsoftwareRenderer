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
