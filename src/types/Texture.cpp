#include "Texture.h"

Texture::Texture( Uint16 width, Uint16 height )
{
    //ctor
    t_width = width;
    t_height = height;
    t_pixels.resize( width * height );
    t_pixels.shrink_to_fit();
}

Texture::Texture( Uint16& width, Uint16& height )
{
    //ctor
    t_width = width;
    t_height = height;
    t_pixels.resize( width * height );
    t_pixels.shrink_to_fit();
}

Texture::Texture( const char* pathtofile )
{
    //ctor

    // create sdl_surface from bmp
    SDL_Surface* textureSurface = SDL_LoadBMP( pathtofile );

    // convert surface to texture
    if ( textureSurface != nullptr )
    {
        ImportFromSurface( textureSurface );
        SDL_FreeSurface( textureSurface );
    }
    else
    {
        std::cout << "Load BMP " << pathtofile << " failed!" << std::endl << SDL_GetError();
        throw std::runtime_error( SDL_GetError() );
    }
}

void Texture::ImportFromSurface( SDL_Surface* surface )
{
    // get dimensions of surface
    t_width = surface->w;
    t_height = surface->h;
    // allocate texture memory
    t_pixels.resize( t_width * t_height );
    t_pixels.shrink_to_fit();

    t_transparent = surface->format->Amask != 0; // alphaMask==0 if no alpha according to docs

    // Lock surface so that we get direct pixel access
    SDL_LockSurface( surface );

    // iterate over each surface pixel
    for ( Uint16 y = 0; y < t_height; y++ ) // iterate over ys
    {
        for ( Uint16 x = 0; x < t_width; x++ ) // iterate over xs
        {
            //pointers to colour values
            Uint8 r, g, b, a;

            if ( t_transparent )
            {
                SDL_GetRGBA( getPixelOn_SDLSurface( surface, x, y ), surface->format,
                             &r, &g, &b, &a );
            }
            else
            {
                SDL_GetRGB( getPixelOn_SDLSurface( surface, x, y ), surface->format,
                             &r, &g, &b );
                // set alpha to opaque
                a = SDL_ALPHA_OPAQUE;
            }

            // create new pixel
            SDL_Color new_colour = { r, g, b, a };

            // add to pixels vector
            SetPixel( x, y, new_colour );
        }
    }

    // Unlock surface as we are done
    SDL_UnlockSurface( surface );
}

SDL_Color Texture::GetPixel( const Uint16& x, const Uint16& y ) const
{
    if ( x < 0 || x >= t_width || y < 0 || y >= t_height )
    {
        throw ( std::runtime_error("Invalid coordinates for texture access!") );
    }
    return t_pixels.at( y * t_width + x );
}

void Texture::SetPixel( const Uint16& x, const Uint16& y, const SDL_Color& colour )
{
    t_transparent = colour.a != SDL_ALPHA_OPAQUE;

    // check if not out of bounds
    if ( x >= 0 && x < GetWidth() &&
         y >= 0 && y < GetHeight() )
    {
        t_pixels.at( y * t_width + x ) = colour;
    }
}

void Texture::FillWithRandomPixels()
{
    t_transparent = false;

    // iterate over pixels
    for ( Uint32 i = 0; i < t_pixels.size(); i++ )
    {
        SDL_Color randomPixel;
        randomPixel.a = SDL_ALPHA_OPAQUE;
        randomPixel.r = rand() % 256;
        randomPixel.g = rand() % 256;
        randomPixel.b = rand() % 256;
        t_pixels.at( i ) = randomPixel;
    }
}

void Texture::FillWithColour( SDL_Color colour )
{
    t_transparent = colour.a != SDL_ALPHA_OPAQUE;

    // iterate over pixels
    for ( Uint32 i = 0; i < t_pixels.size(); i++ )
    {
        t_pixels.at( i ) = colour;
    }
}

Texture::~Texture()
{
    //dtor
    #ifdef PRINT_DEBUG_STUFF
        cout << "Dtor of Texture object was called!" << endl;
    #endif // PRINT_DEBUG_STUFF
}
