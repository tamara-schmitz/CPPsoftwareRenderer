#include "window.h"
#include <unistd.h>


Window::Window( int width, int height, const std::string& title, int fpsLock )
{
    //ctor

    // set window parameters
    pixels.resize( width * height * 4 );
    w_width    = width;
    w_height   = height;
    r_fpsLimit = fpsLock;
    if ( fpsLock > 0 )
    {
        r_tickLimit = (1000.0 * 1000.0 * 1000.0) / fpsLock;
    }

    // Initialise SDL context
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS ) < 0 )
    {
        std::cout << "Couldn't init SDL!" << std::endl << SDL_GetError();
    }

    // Create Window
    w_window = SDL_CreateWindow( title.c_str(),
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      w_width, w_height, SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS );
    if ( w_window == NULL )
    {
        std::cout << "Couldn't init window!" << std::endl << SDL_GetError();
    }
    SDL_SetWindowMinimumSize(w_window, w_width, w_height);

    // Create Renderer
    r_renderer = SDL_CreateRenderer( w_window, -1, 0 );
    if ( r_renderer == NULL )
    {
        std::cout << "Couldn't init renderer!" << std::endl << SDL_GetError();
    }
    SDL_SetRenderDrawBlendMode(r_renderer, SDL_BLENDMODE_NONE);

    // Create Texture for Renderer
    r_texture = SDL_CreateTexture( r_renderer,
                                        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                        w_width, w_height );
    if ( r_texture == NULL )
    {
        std::cout << "Couldn't init texture!" << std::endl << SDL_GetError();
    }
    SDL_SetTextureBlendMode( r_texture, SDL_BLENDMODE_NONE );

    // Done
    std::cout << "Init complete!" << std::endl;
}

void Window::drawPixel( int x, int y, SDL_Color color )
{
    int offset = y * w_width + x;
    pixels[ offset * 4 + 0] = color.b; // b
    pixels[ offset * 4 + 1] = color.g; // g
    pixels[ offset * 4 + 2] = color.r; // r
    pixels[ offset * 4 + 3] = color.a; // a
}

void Window::updateWindow()
{
    // Draw pixels to render texture
    SDL_UpdateTexture(r_texture, NULL, &pixels[0], w_width * 4);

    // Displays changes made to renderTexture
    // flow: texture -> renderer -> window

    SDL_RenderCopy( r_renderer, r_texture, NULL, NULL );
    SDL_RenderPresent( r_renderer );
    SDL_UpdateWindowSurface( w_window );

    // TickCall (after every frame!)
    tickCall();
}

void Window::clearBuffers()
{
    // Clears the render texture and pixel array
    // (no need to clear the window or renderer as it's not blending textures

    // empty pixel array
    pixels.clear();
    pixels.resize( w_width * w_height * 4 );

    SDL_SetRenderTarget( r_renderer, r_texture );
    SDL_SetRenderDrawBlendMode( r_renderer, SDL_BLENDMODE_NONE );
    SDL_SetRenderDrawColor( r_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
    SDL_RenderFillRect( r_renderer, NULL );

}

void Window::tickCall()
{
    // Should be called whenever a frame has finished rendering
    // Measures frametime and enforces FPS limit

    // update time tracker vars
    r_tickNow_main = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
    r_tickLast = r_tickNow;
    r_tickNow = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();

    // Calculate frametime and maintime
    r_frametime_nano = (r_tickNow      - r_tickLast      ).count();
    r_maintime_nano  = (r_tickNow_main - r_tickLast_main ).count();


    // Sleep if maintime shorter than FPS-Limit
    if ( r_fpsLimit > 0 && r_maintime_nano < r_tickLimit )
    {
        //usleep( (unsigned int) ( r_tickLimit - r_maintime) * 1000);
        struct timespec framesleep,sleepremains;
        framesleep.tv_sec  = 0;
        framesleep.tv_nsec = r_tickLimit - (double) r_maintime_nano;
        nanosleep( &framesleep, &sleepremains );
    }

    // update time tracker var
    r_tickLast_main = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();

}


Window::~Window()
{
    //dtor

    pixels.clear();
    // Properly shutdown SDL
    SDL_DestroyTexture(  r_texture  );
    SDL_DestroyRenderer( r_renderer );
    SDL_DestroyWindow(   w_window   );
    SDL_Quit();
}
