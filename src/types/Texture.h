#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include <exception>
#include <stdexcept>

class Texture
{
    public:
        Texture( Uint16 width, Uint16 height ); // default constructor
        Texture( Uint16& width, Uint16& height ); // reference constructor
        explicit Texture( const char* pathtofile ); // texture from bmp constructor

        ~Texture(); // default deconstructor

        // getters and setters
        Uint16 GetWidth() const { return t_width; }
        Uint16 GetHeight() const { return t_height; }
        SDL_Color GetPixel( const Uint16& x, const Uint16& y ) const;
        void SetPixel( const Uint16& x, const Uint16& y, const SDL_Color& colour );
        bool isTransparent() const { return t_transparent; };

        // texture modifiers
        void FillWithRandomPixels();
        void FillWithColour( SDL_Color colour );

    protected:
        // pixels are saved as SDL_Color vector
        std::vector< SDL_Color > t_pixels;
        Uint16 t_width, t_height;
        bool t_transparent;

        // imports pixels from an sdl_surface
        void ImportFromSurface( SDL_Surface* surface );

};

#endif // TEXTURE_H
