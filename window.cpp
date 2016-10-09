#include "window.h"


Window::Window( int width, int height, const std::string& title, int fpsLock )
{
    //ctor

    // set window parameters
    this->w_width    = width;
    this->w_height   = height;
    this->r_fpsLimit = fpsLock;
    if ( fpsLock > 0 )
    {
        this->r_tickLimit = 1000 / double(fpsLock);
    }

    // Initialise SDL context
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0 )
    {
        std::cout << "Couldn't init SDL!" << std::endl << SDL_GetError();
    }

    // Create Window
    this->w_window = SDL_CreateWindow( title.c_str(),
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      this->w_width, this->w_height, SDL_WINDOW_SHOWN );
    if ( this->w_window == NULL )
    {
        std::cout << "Couldn't init window!" << std::endl << SDL_GetError();
    }
    SDL_SetWindowMinimumSize(this->w_window, this->w_width, this->w_height);

    // Create Renderer
    this->r_renderer = SDL_CreateRenderer( this->w_window, -1, 0 );
    if ( this->r_renderer == NULL )
    {
        std::cout << "Couldn't init renderer!" << std::endl << SDL_GetError();
    }
    SDL_SetRenderDrawBlendMode(this->r_renderer, SDL_BLENDMODE_NONE);

    // Create Texture for Renderer
    this->r_texture = SDL_CreateTexture( this->r_renderer,
                                        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                        this->w_width, this->w_height );
    if ( this->r_texture == NULL )
    {
        std::cout << "Couldn't init texture!" << std::endl << SDL_GetError();
    }
    SDL_SetTextureBlendMode( this->r_texture, SDL_BLENDMODE_NONE );

    // Done
    std::cout << "Init complete!" << std::endl;
}


void Window::updateWindow()
{
    // Displays changes made to renderTexture
    // flow: texture -> renderer -> window

    SDL_RenderCopy( this->r_renderer, this->r_texture, NULL, NULL );
    SDL_RenderPresent( this->r_renderer );
    SDL_UpdateWindowSurface( this->w_window );

    // TickCall (after every frame!)
    tickCall();
}


void Window::tickCall()
{
    // Should be called whenever a frame has finished rendering
    // Measures frametime and enforces FPS limit

    // update time tracker vars
    this->r_tickLast = this->r_tickNow;
    this->r_tickLast_main = this->r_tickNow_main;

    this->r_tickNow = double( SDL_GetPerformanceCounter() );

    // Calculate frametime and maintime
    double tickFreq = SDL_GetPerformanceFrequency();
    this->r_frametime = ( this->r_tickNow - this->r_tickLast ) * 1000 / tickFreq;
    this->r_maintime  = ( this->r_tickNow_main - this->r_tickLast_main ) * 1000 / tickFreq;

    // Sleep if maintime shorter than FPS-Limit
    if ( this->r_fpsLimit > 0 && this->r_maintime < this->r_tickLimit )
    {
        SDL_Delay( Uint32( this->r_tickLimit - this->r_maintime ) );
    }

    // update time tracker var
    this->r_tickNow_main = double( SDL_GetPerformanceCounter() );
}


Window::~Window()
{
    //dtor

    // Properly shutdown SDL
    SDL_DestroyTexture(  this->r_texture  );
    SDL_DestroyRenderer( this->r_renderer );
    SDL_DestroyWindow(   this->w_window   );
    SDL_Quit();
}
