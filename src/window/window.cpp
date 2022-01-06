#include "window/window.h"

Window::Window( int width, int height, double scale, std::string title, double fpsLock )
{
    //ctor

    // set window parameters
    w_width    = width;
    w_height   = height;
    w_title    = title;
    r_scale    = scale;
    r_width    = width  * scale;
    r_height   = height * scale;

    // set FPS limit in Timer class
    timer.SetFPSLimit( fpsLock );

    if ( !headlessMode )
    {
        // Initialise SDL context
        if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS ) < 0 )
        {
            cout << "Couldn't init SDL!" << endl << SDL_GetError();
        }

        // Create Window
        w_window = SDL_CreateWindow( title.c_str(),
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          w_width, w_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
        if ( w_window == nullptr )
        {
            cout << "Couldn't init window!" << endl << SDL_GetError();
        }
        SDL_SetWindowMinimumSize( w_window, w_width, w_height );

        // Create Renderer
        r_renderer = SDL_CreateRenderer( w_window, -1, 0 );
        if ( r_renderer == nullptr )
        {
            cout << "Couldn't init renderer!" << endl << SDL_GetError();
        }
        SDL_SetRenderDrawBlendMode( r_renderer, SDL_BLENDMODE_NONE );
        SDL_RenderSetLogicalSize( r_renderer, r_width, r_height );
        SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ); // hint nearest pixel texture scaling

        // Create Texture for Pixelaccess
        r_ptexture = SDL_CreateTexture( r_renderer,
                                            SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                            r_width, r_height );
        if ( r_ptexture == NULL )
        {
            cout << "Couldn't init ptexture!" << endl << SDL_GetError();
        }
        SDL_SetTextureBlendMode( r_ptexture, SDL_BLENDMODE_NONE );

        // Create Texture for line rendering
        r_ltexture = SDL_CreateTexture( r_renderer,
                                            SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                                            r_width, r_height );
        if ( r_ltexture == NULL )
        {
            cout << "Couldn't init ltexture!" << endl << SDL_GetError();
        }
        SDL_SetTextureBlendMode( r_ltexture, SDL_BLENDMODE_BLEND );


        // allocate memory for null_pixels pointer (needs to be freed in dstor!)
        null_pixels = new Uint32[r_height * r_width];
        // fill with color black
        SDL_Color colour_black = { 0, 0, 0, SDL_ALPHA_OPAQUE };
        Uint32 rgbamap = getPixelFor_SDLColor( &colour_black );

        for ( Uint32 i=0; i <  r_width * r_height ; i++ )
        {
            null_pixels[ i ] = rgbamap;
        }
    }

    // Done
    cout << "Init complete!" << endl;
}

void Window::reserveAddLines( Uint64 amount )
{
    // reserve requested amount lines in addition to already reserved ones
    line_points.reserve( 2 * amount + line_points.size() );
    line_colors.reserve( amount + line_colors.size() );
}

void Window::drawPixel( int x, int y, SDL_Color color)
{
    if ( !headlessMode )
    {
        // return if outside of screen bounds
        if ( x >= (int) r_width  || x < 0 ||
             y >= (int) r_height || y < 0 )
        {
            return;
        }

        int offset = y * r_pitch_div_4 + x;

        // obtain fast write access to texture
        LockRTexture();
        pixels_direct[ offset ] = getPixelFor_SDLColor( &color );
    }
}

void Window::drawLine( SDL_Point p1, SDL_Point p2, SDL_Color color )
{
    if ( !headlessMode )
    {
        if ( compSDL_Point( p1, p2 ) )
        {
            // draw pixel if points are the same
            drawPixel(p1.x, p2.y, color );
        }
        else
        {
            // adds line to vectors
            line_points.push_back( p1 );
            line_points.push_back( p2 );

            line_colors.push_back( color );
        }
    }
}

void Window::updateWindow()
{
    if ( !headlessMode )
    {
        // Draw lines to line texture
        if ( line_points.size() > 1 && line_points.size() % 2 == 0 &&
             line_colors.size() == line_points.size() / 2 ) // we need 2 points per line and 1 colour per line
        {
            SDL_SetRenderTarget( r_renderer, r_ltexture );
            SDL_SetRenderDrawBlendMode( r_renderer, SDL_BLENDMODE_NONE );

            // batch multiple lines with the same colour together
            Uint64 batch_begin_count[ 2 ] = { 0, 0 }; // [0] ^= first point [1] ^= count points
            SDL_Color cur_color = SDL_Color { 0, 0, 0, SDL_ALPHA_TRANSPARENT };

            for ( int i=0; i < (int) line_colors.size(); i++ )
            {
                if ( compSDL_Color( cur_color, line_colors[ i ] ) )
                {
                    // current colour and next colour are equal?
                    // --> add line to batch
                    batch_begin_count[ 1 ] = batch_begin_count[ 1 ] + 2;
                }
                else
                {
                    // end of batch? flush lines from batch
                    if ( batch_begin_count[ 1 ] >= 2 ) // ensure that there are lines to draw
                    {
                        SDL_SetRenderDrawColor( r_renderer, cur_color.r, cur_color.g, cur_color.b, cur_color.a );
                        SDL_RenderDrawLines( r_renderer,
                                             &line_points[ batch_begin_count[ 0 ] ],
                                             batch_begin_count[ 1 ] );
                    }

                    // reset counter and colour
                    batch_begin_count[ 0 ] = i * 2;
                    batch_begin_count[ 1 ] = 2;
                    cur_color = line_colors[ i ];
                }
            }

            if ( line_points.size() > batch_begin_count[ 0 ] )
            {
                // draw last batch (missing because for-loop ended earlier)
                SDL_SetRenderDrawColor( r_renderer, cur_color.r, cur_color.g, cur_color.b, cur_color.a );
                SDL_RenderDrawLines( r_renderer,
                                     &line_points[ batch_begin_count[ 0 ] ],
                                     batch_begin_count[ 1 ] );
            }
        }

        // Displays changes made to renderTexture
        // ptexture + ltexture (l drawn over p) -> renderer -> window

        // Obtain read access to pixel texture
        UnlockRTexture();
        SDL_SetRenderTarget( r_renderer, NULL );
        SDL_RenderCopy( r_renderer, r_ptexture, NULL, NULL );
        if ( line_points.size() > 0 )
        {
            SDL_SetRenderDrawBlendMode( r_renderer, SDL_BLENDMODE_BLEND );
            SDL_RenderCopy( r_renderer, r_ltexture, NULL, NULL );
            SDL_SetRenderDrawBlendMode( r_renderer, SDL_BLENDMODE_NONE );
        }
        SDL_RenderPresent( r_renderer );
        SDL_UpdateWindowSurface( w_window );

    }

    // TickCall (after every frame!)
    timer.TickCall();
}

void Window::clearBuffers()
{
    // Clears the render texture and pixel array
    // (no need to clear the window or renderer as it's not blending textures

    if ( !headlessMode )
    {
        UnlockRTexture();

        // clear line array
        if ( line_points.size() > 0 || line_colors.size() > 0 )
        {
            line_points.clear();
            line_colors.clear();

            // clear ltexture
            SDL_SetRenderTarget( r_renderer, r_ltexture );
            SDL_SetRenderDrawBlendMode( r_renderer, SDL_BLENDMODE_NONE );
            SDL_SetRenderDrawColor( r_renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT );
            SDL_RenderClear( r_renderer );
        }
        // clear window
        SDL_SetRenderTarget( r_renderer, NULL );
        SDL_SetRenderDrawColor( r_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderClear( r_renderer );


        // clear ptexture to black
        LockRTexture();
        memcpy ( pixels_direct, null_pixels, r_pitch * r_height ); // WARNING! DIRECT MEMORY ACCESS
    }
}

void Window::updateTitleWithFPS( int updateInterval )
{
    // updates windows title every updateInterval (in seconds)
    if ( !headlessMode )
    {
        if ( w_title_fps_time < std::chrono::system_clock::now() )
        {
            w_title_fps_time = std::chrono::system_clock::now() + std::chrono::seconds( updateInterval );
            std::stringstream title;
            title << w_title << " - " << timer.GetCurrentFPS() << " FPS";
            SDL_SetWindowTitle( w_window, title.str().c_str() );
        }
    }
}

void Window::LockRTexture()
{
    if ( !headlessMode )
    {
        if ( pixels_direct == nullptr )
        {
            SDL_LockTexture( r_ptexture, NULL, (void**)&pixels_direct, &r_pitch );
            r_pitch_div_4 = r_pitch / 4;
        }
    }
}

void Window::UnlockRTexture()
{
    if ( !headlessMode )
    {
        if ( pixels_direct != nullptr )
        {
            SDL_UnlockTexture( r_ptexture );
            pixels_direct = nullptr;
            r_pitch = r_pitch_div_4 = 0;
        }
    }
}

Window::~Window()
{
    //dtor

    if ( !headlessMode )
    {
        delete[] null_pixels;

        // Properly shutdown SDL
        SDL_DestroyTexture(  r_ptexture );
        SDL_DestroyTexture(  r_ltexture );
        SDL_DestroyRenderer( r_renderer );
        SDL_DestroyWindow(   w_window   );
        SDL_Quit();
    }


    if ( printDebug )
    {
        cout << "Dtor of Window object was called!" << endl;
    }
}
