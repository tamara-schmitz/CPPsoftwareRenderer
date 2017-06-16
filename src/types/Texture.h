#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <exception>
#include <stdexcept>
#include <SDL2/SDL.h>
#include "common.h"

class Texture
{
    public:
        Texture( Uint16 width, Uint16 height ); // default constructor
        Texture( Uint16& width, Uint16& height ); // reference constructor
        explicit Texture( const char* pathtofile ); // texture from bmp constructor

        // getters
        Uint16 GetWidth() const { return pixels.size() / height; }
        Uint16 GetHeight() const { return height; }
        SDL_Color GetPixel( Uint16 x, Uint16 y ) const;
        void SetPixel( Uint16 x, Uint16 y, SDL_Color colour );

        // texture modifiers
        void FillWithRandomPixels();
        void FillWithColour( SDL_Color colour );

    protected:
        // pixels are saved as SDL_Color vector
        std::vector< SDL_Color > pixels;
        Uint16 height;

        // imports pixels from an sdl_surface
        void ImportFromSurface( SDL_Surface* surface );

};

#endif // TEXTURE_H
