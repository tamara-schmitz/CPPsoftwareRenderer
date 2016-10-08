#include "window.h"
#include <iostream>


Window::Window(int width, int height, const std::string& title, int fpsLock)
{
    //ctor

    // set window parameters
    this->w_width  = width;
    this->w_height = height;
    this->w_title  = title;

    // Initialise SDL context
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0 )
    {
        cout << "Couldn't init SDL!" << endl << SDL_GetError();
        return -1;
    }

    // Create Window
    this->w_window = SDL_CreateWindow(this->w_title,
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      this->w_width, this->w_height, SDL_WINDOW_SHOWN);
    if ( this->w_window == NULL )
    {
        cout << "Couldn't init window!" << endl << SDL_GetError();
        return -1;
    }
    SDL_SetWindowMinimumSize(this->w_window, this->w_width, this->w_height);

    // Create Renderer
    this->r_renderer = SDL_CreateRenderer(this->w_window, -1, 0);
    if ( this->r_renderer == NULL )
    {
        cout << "Couldn't init renderer!" << endl << SDL_GetError();
        return -1;
    }
    SDL_SetRenderDrawBlendMode(this->r_renderer, SDL_BLENDMODE_NONE);

    // Create Texture for Renderer
    this->r_texture = SDL_CreateTexture(this->r_renderer,
                                        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                        this->w_width, this->w_height);
    if ( this->r_texture == NULL )
    {
        cout << "Couldn't init texture!" << endl << SDL_GetError();
        return -1;
    }
    SDL_SetTextureBlendMode(this->r_texture, SDL_BLENDMODE_NONE);

}

Window::~Window()
{
    //dtor

    // Properly shutdown SDL
    SDL_DestroyWindow(this->w_window);
    SDL_Quit();

    return 0;
}
