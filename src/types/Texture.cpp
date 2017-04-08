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
