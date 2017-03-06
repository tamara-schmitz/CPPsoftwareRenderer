#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>

#include "timer.h"

class Window
{
    public:
        // cstor
        Window( int width, int height, double scale, const std::string& title, int fpsLock );
        // dstor
        virtual ~Window();

        // Getters
        SDL_Texture* GetRenderTexture() { return r_texture; }
        unsigned int Getwidth() { return r_width; }
        unsigned int Getheight() { return r_height; }

        Timer timer;

        // Render functions
        void drawPixel( int x, int y, SDL_Color color );
        void updateWindow();
        void clearBuffers();

    protected:

    private:
        // Window and render vars
        SDL_Window*     w_window;
        SDL_Renderer*   r_renderer;
        SDL_Texture*    r_texture;
        std::vector< Uint8 > pixels;
        std::vector< Uint8 > null_pixels;
        unsigned int w_width;
        unsigned int w_height;
        double r_scale;
        unsigned int r_width;
        unsigned int r_height;
};

#endif // WINDOW_H
