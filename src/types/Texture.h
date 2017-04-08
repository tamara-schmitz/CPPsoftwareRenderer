#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_pixels.h>

class Texture
{
    public:
        Texture( Uint16 width, Uint16 height ); // default constructor
        Texture( Uint16& width, Uint16& height ); // reference constructor

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

};

#endif // TEXTURE_H
