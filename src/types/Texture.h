#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"

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
        const SDL_Color GetPixel( const Uint16& x, const Uint16& y ) const;
        void SetPixel( const Uint16& x, const Uint16& y, const SDL_Color& colour );

        // texture modifiers
        void FillWithRandomPixels();
        void FillWithColour( const SDL_Color& colour );
        void clear();

        std::vector< Uint32 > t_pixels;

    protected:
        Uint16 t_width = 0, t_height = 0;
        bool t_transparent = false;

        // imports pixels from an sdl_surface
        void ImportFromSurface( SDL_Surface* surface );

};

#endif // TEXTURE_H
