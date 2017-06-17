#include "Texture.h"

Texture::Texture( Uint16 width, Uint16 height )
{
    //ctor
    t_width = width;
    t_height = height;
    t_pixels.resize( width * height );
}

Texture::Texture( Uint16& width, Uint16& height )
{
    //ctor
    t_width = width;
    t_height = height;
    t_pixels.resize( width * height );
}

Texture::Texture( const char* pathtofile, SDL_PixelFormat* surface_format )
{
    //ctor

    // create sdl_surface from bmp
    SDL_Surface* textureSurface = SDL_LoadBMP( pathtofile );

    // convert surface to texture
    if ( textureSurface != NULL )
    {
        ImportFromSurface( textureSurface, surface_format );
        // free surface
        SDL_FreeSurface( textureSurface );
    }
    else
    {
        std::cout << "Load BMP " << pathtofile << " failed!" << std::endl << SDL_GetError();
    }
}

void Texture::ImportFromSurface( SDL_Surface* surface, SDL_PixelFormat* surface_format )
{
    // get dimensions of surface
    t_width = surface->w;
    t_height = surface->h;
    // allocate texture memory
    t_pixels.resize( t_width * t_height );
    // check if surface has transparency
    t_transparent = surface->format->Amask != 0; // alphaMask==0 if no alpha according to docs
    #ifdef PRINT_DEBUG_STUFF
        std::cout << "Texture transparent: " << t_transparent << std::endl;
    #endif // PRINT_DEBUG_STUFF

    // Lock surface so that we get direct pixel access
    SDL_LockSurface( surface );

    // Pointer to surface used for import
    SDL_Surface* conv_surface;
    bool conv_surface_isNew;
    // Check if surface is 32 bit
    if ( surface->format->BytesPerPixel != 4 )
    {
        // create new 32 bit surface
        conv_surface = SDL_ConvertSurface( surface, surface_format, 0 );
        SDL_LockSurface( conv_surface );
        conv_surface_isNew = true;
    }
    else
    {
        // point to original surface
        conv_surface = surface;
        conv_surface_isNew = false;
    }

    // iterate over each surface pixel
    for ( Uint16 y = 0; y < conv_surface->h; y++ ) // iterate over ys
    {
        for ( Uint16 x = 0; x < conv_surface->w; x++ ) // iterate over xs
        {
            // pointers to colour values
            Uint8 r, g, b, a;

            // get rgba values
            SDL_GetRGBA( ((Uint32*)conv_surface->pixels)[y*conv_surface->w + x], conv_surface->format,
                             &r, &g, &b, &a );

            // create new pixel
            SDL_Color new_pixel = { r, g, b, a };

            // add to pixels vector
            t_pixels.at( y*conv_surface->w + x ) = new_pixel;
        }
    }

    // Free conv surface if it's a new surface
    if ( conv_surface_isNew )
    {
        SDL_UnlockSurface( conv_surface );
        SDL_FreeSurface( conv_surface );
    }

    // Unlock surface as we are done
    SDL_UnlockSurface( surface );
}

SDL_Color Texture::GetPixel( const Uint16& x, const Uint16& y ) const
{
    return t_pixels.at( y * t_width + x );
}

void Texture::SetPixel( const Uint16& x, const Uint16& y, const SDL_Color& colour )
{
    // transparent colour?
    if ( colour.a != SDL_ALPHA_OPAQUE )
    {
        // yes
        t_transparent = true;
    }

    // check if not out of bounds
    if ( x >= 0 && x < GetWidth() &&
         y >= 0 && y < GetHeight() )
    {
        t_pixels.at( y * t_width + x ) = colour;
    }
}

void Texture::FillWithRandomPixels()
{
    // transparent? no
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
    // transparent?
    if ( colour.a == SDL_ALPHA_OPAQUE )
    {
        // no
        t_transparent = false;
    }
    else
    {
        // yes
        t_transparent = true;
    }

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
        std::cout << "Dtor of Texture object was called!" << std::endl;
    #endif // PRINT_DEBUG_STUFF
}
