#include "window.h"


Window::Window( int width, int height, const std::string& title, int fpsLock )
{
    //ctor

    // set window parameters
    w_width    = width;
    w_height   = height;
    r_fpsLimit = fpsLock;
    if ( fpsLock > 0 )
    {
        r_tickLimit = 1000 / double(fpsLock);
    }

    // Initialise SDL context
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0 )
    {
        std::cout << "Couldn't init SDL!" << std::endl << SDL_GetError();
    }

    // Create Window
    w_window = SDL_CreateWindow( title.c_str(),
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      w_width, w_height, SDL_WINDOW_SHOWN );
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


void Window::updateWindow()
{
    // Displays changes made to renderTexture
    // flow: texture -> renderer -> window

    SDL_RenderCopy( r_renderer, r_texture, NULL, NULL );
    SDL_RenderPresent( r_renderer );
    SDL_UpdateWindowSurface( w_window );

    // TickCall (after every frame!)
    tickCall();
}

void Window::clearRenderer()
{
    // Clears the render texture
    // (no need to clear the window or renderer as it's not blending textures

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
    r_tickLast = r_tickNow;
    r_tickLast_main = r_tickNow_main;

    r_tickNow = double( SDL_GetPerformanceCounter() );

    // Calculate frametime and maintime
    double tickFreq = SDL_GetPerformanceFrequency();
    r_frametime = ( r_tickNow - r_tickLast ) * 1000 / tickFreq;
    r_maintime  = ( r_tickNow_main - r_tickLast_main ) * 1000 / tickFreq;

    // Sleep if maintime shorter than FPS-Limit
    if ( r_fpsLimit > 0 && r_maintime < r_tickLimit )
    {
        SDL_Delay( Uint32( r_tickLimit - r_maintime ) );
    }

    // update time tracker var
    r_tickNow_main = double( SDL_GetPerformanceCounter() );
}


Window::~Window()
{
    //dtor

    // Properly shutdown SDL
    SDL_DestroyTexture(  r_texture  );
    SDL_DestroyRenderer( r_renderer );
    SDL_DestroyWindow(   w_window   );
    SDL_Quit();
}
