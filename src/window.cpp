// import unix/windows library for tickCall()
#ifdef __unix__
    #include <unistd.h>
#elif __cplusplus <= 199711L
    #include <chrono>
#endif

#include "window.h"

Window::Window( int width, int height, double scale, const std::string& title, int fpsLock )
{
    //ctor

    // set window parameters
    w_width    = width;
    w_height   = height;
    r_scale    = scale;
    r_width    = width  * scale;
    r_height   = height * scale;

    // create pixelformat
    r_format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);

    // set FPS limit in Timer class
    timer.SetFPSLimit( fpsLock );

    // Initialise SDL context
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS ) < 0 )
    {
        std::cout << "Couldn't init SDL!" << std::endl << SDL_GetError();
    }

    // Create Window
    w_window = SDL_CreateWindow( title.c_str(),
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      w_width, w_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if ( w_window == nullptr )
    {
        std::cout << "Couldn't init window!" << std::endl << SDL_GetError();
    }
    SDL_SetWindowMinimumSize( w_window, w_width, w_height );

    // Create Renderer
    r_renderer = SDL_CreateRenderer( w_window, -1, NULL );
    if ( r_renderer == nullptr )
    {
        std::cout << "Couldn't init renderer!" << std::endl << SDL_GetError();
    }
    SDL_SetRenderDrawBlendMode( r_renderer, SDL_BLENDMODE_NONE );
    SDL_RenderSetLogicalSize( r_renderer, r_width, r_height );
//    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ); // hint better texture scaling

    // Create Texture for Pixelaccess
    r_ptexture = SDL_CreateTexture( r_renderer,
                                        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                        r_width, r_height );
    if ( r_ptexture == NULL )
    {
        std::cout << "Couldn't init texture!" << std::endl << SDL_GetError();
    }
    SDL_SetTextureBlendMode( r_ptexture, SDL_BLENDMODE_NONE );

    // Create Texture for line rendering
    r_ltexture = SDL_CreateTexture( r_renderer,
                                        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                                        r_width, r_height );
    if ( r_ltexture == NULL )
    {
        std::cout << "Couldn't init texture!" << std::endl << SDL_GetError();
    }
    SDL_SetTextureBlendMode( r_ltexture, SDL_BLENDMODE_BLEND );


    // allocate memory for null_pixels pointer (needs to be freed in dstor!)
    LockRTexture(); // we need r_pitch, so unlock texture
    null_pixels = (Uint32*) malloc ( r_height * r_pitch );
    // fill with color black
    Uint32 rgbamap = SDL_MapRGBA( r_format, 0, 0, 0, SDL_ALPHA_TRANSPARENT );
    for ( int i=0; i < int( sizeof(Uint32) * r_height ); i++ )
    {
        null_pixels[ i ] = rgbamap;
    }

    // Done
    std::cout << "Init complete!" << std::endl;
}

void Window::reserveAddLines( Uint64 amount )
{
    // reserve requested amount lines in addition to already reserved ones
    line_points.reserve( 4 * amount + line_points.capacity() );
    line_colors.reserve( amount + line_colors.capacity() );
}

void Window::drawPixel( int x, int y, SDL_Color color)
{
    // return if outside of screen bounds
    if ( x >= (int) r_width  || x < 0 ||
         y >= (int) r_height || y < 0 )
    {
        return;
    }

//    int offset = y * 4 * r_width + x * 4;
//    pixels[ offset + 0] = color.b; // b
//    pixels[ offset + 1] = color.g; // g
//    pixels[ offset + 2] = color.r; // r
//    pixels[ offset + 3] = color.a; // a

    int offset = y * (r_pitch / 4) + x;

    LockRTexture();
    pixels_direct[ offset ] = SDL_MapRGBA( r_format, color.r, color.g, color.b, color.a );

}

void Window::drawLine( int x1, int y1, int x2, int y2, SDL_Color color )
{
    // adds line to vectors
    line_points.push_back( x1 );
    line_points.push_back( y1 );
    line_points.push_back( x2 );
    line_points.push_back( y2 );

    line_colors.push_back( color );
}

void Window::updateWindow()
{
    // Draw pixels to render texture
    UnlockRTexture();

    // Draw lines to render texture
    if ( line_points.size() > 0 )
    {
        SDL_SetRenderTarget( r_renderer, r_ltexture );
        for ( int i=0; i < (int) line_colors.size(); i++ )
        {
            int offset = i * 4;
            SDL_Color& cur_color = line_colors[ i ];

            SDL_SetRenderDrawColor( r_renderer, cur_color.r, cur_color.g, cur_color.b, cur_color.a );
            SDL_RenderDrawLine( r_renderer, line_points[ offset     ],
                                            line_points[ offset + 1 ],
                                            line_points[ offset + 2 ],
                                            line_points[ offset + 3 ] );

            #ifdef PRINT_DEBUG_STUFF
            if ( i == 400 )
            {
                std::cout << "Current Line - x1: " << line_points[ offset ] << " y1: " << line_points[ offset + 1 ]
                          << " x2: " << line_points[ offset + 2 ] << " y2: " << line_points[ offset + 3 ]
                          << " Colour: " <<  cur_color.r <<  cur_color.g <<  cur_color.b <<  cur_color.a << std::endl;
            }
            #endif // PRINT_DEBUG_STUFF
        }
    }

    // Displays changes made to renderTexture
    // ptexture + ltexture -> renderer -> window

    SDL_SetRenderTarget( r_renderer, NULL );
    SDL_RenderCopy( r_renderer, r_ptexture, NULL, NULL );
    SDL_RenderCopy( r_renderer, r_ltexture, NULL, NULL );
    SDL_RenderPresent( r_renderer );
    SDL_UpdateWindowSurface( w_window );

    // TickCall (after every frame!)
    timer.TickCall();
}

void Window::clearBuffers()
{
    // Clears the render texture and pixel array
    // (no need to clear the window or renderer as it's not blending textures

    UnlockRTexture();

    // clear line array
    line_points.clear();
    line_colors.clear();

    // RenderTargets
    SDL_SetRenderTarget( r_renderer, r_ptexture );
    SDL_SetRenderDrawBlendMode( r_renderer, SDL_BLENDMODE_NONE );
    SDL_SetRenderDrawColor( r_renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT );
    SDL_RenderClear( r_renderer );
    SDL_SetRenderTarget( r_renderer, r_ltexture );
    SDL_RenderClear( r_renderer );


    // clear texture to black
    LockRTexture();
    memcpy ( pixels_direct, null_pixels, r_pitch * r_height );
}

void Window::LockRTexture()
{
    if ( pixels_direct == nullptr )
    {
        SDL_LockTexture( r_ptexture, NULL, (void**)&pixels_direct, &r_pitch );
    }
}

void Window::UnlockRTexture()
{
    if ( pixels_direct != nullptr )
    {
        SDL_UnlockTexture( r_ptexture );
        pixels_direct = nullptr;
        r_pitch = 0;
    }
}

Window::~Window()
{
    //dtor
    free ( null_pixels );
//    delete[] null_pixels;
    line_points.clear();
    line_colors.clear();

    // Properly shutdown SDL
    SDL_DestroyTexture(  r_ptexture  );
    SDL_DestroyRenderer( r_renderer );
    SDL_DestroyWindow(   w_window   );
    SDL_Quit();
}
