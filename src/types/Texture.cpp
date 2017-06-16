#include "Texture.h"

Texture::Texture( Uint16 width, Uint16 height )
{
    //ctor
    this->height = height;
    pixels.resize( width * height );
}

Texture::Texture( Uint16& width, Uint16& height )
{
    //ctor
    this->height = height;
    pixels.resize( width * height );
}

Texture::Texture( const char* pathtofile )
{
    //ctor

    // create sdl_surface from bmp
    SDL_Surface* textureSurface = SDL_LoadBMP( pathtofile );

    // convert surface to texture
    if ( textureSurface != NULL )
    {
        ImportFromSurface( textureSurface );
    }
    // free surface
    SDL_FreeSurface( textureSurface );
}

void Texture::ImportFromSurface( SDL_Surface* surface )
{
    // get dimensions of surface
    Uint16 width = surface->w;
    this->height = surface->h;
    // allocate texture memory
    pixels.resize( width * this->height );

    // convert surface to different display format
    SDL_LockSurface( surface );
//    SDL_Surface* conv_surface = SDL_ConvertSurfaceFormat( surface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_Surface* conv_surface = surface;
    SDL_UnlockSurface( surface );

    // Lock conv_surface for pixel access
    SDL_LockSurface( conv_surface );


    // iterate over each surface pixel
    for ( Uint32 i = 0; i < (Uint32) (width * height); i++ )
    {
        // pointers to colour values
        Uint8 r, g, b, a;

        // get rgba values
        SDL_GetRGBA( getPixelOn_SDLSurface( conv_surface, i ), conv_surface->format,
                     &r, &g, &b, &a );

        // create new pixel
        SDL_Color new_pixel = { r, g, b, a };

        // add to pixels vector
        pixels.at( i ) = new_pixel;
    }

    // Unlock and delete conv_surface because we are done
    SDL_UnlockSurface( conv_surface );
    SDL_FreeSurface( conv_surface );
}

SDL_Color Texture::GetPixel( Uint16 x, Uint16 y ) const
{
    return pixels.at( x * y );
}

void Texture::SetPixel( Uint16 x, Uint16 y, SDL_Color colour )
{
    pixels.at( x * y ) = colour;
}

void Texture::FillWithRandomPixels()
{
    // iterate over pixels
    for ( Uint32 i = 0; i < pixels.size(); i++ )
    {
        SDL_Color randomPixel;
        randomPixel.a = SDL_ALPHA_OPAQUE;
        randomPixel.r = rand() % 256;
        randomPixel.g = rand() % 256;
        randomPixel.b = rand() % 256;
        pixels.at( i ) = randomPixel;
    }
}

void Texture::FillWithColour( SDL_Color colour )
{
    // iterate over pixels
    for ( Uint32 i = 0; i < pixels.size(); i++ )
    {
        pixels.at( i ) = colour;
    }
}
