#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <vector>

#include "typedefs.h"
#include "timer.h"

class Window
{
    public:
        // cstor
        Window( int width, int height, double scale, const std::string& title, int fpsLock );
        // dstor
        virtual ~Window();

        // Getters
        SDL_Texture* GetRenderTexture() { return r_ptexture; }
        unsigned int Getwidth() { return r_width; }
        unsigned int Getheight() { return r_height; }

        Timer timer;

        // Render functions
        void reserveAddLines( Uint64 amount ); // reserves specified amount of lines (in addition to current reservation)
        void drawPixel( int x, int y, SDL_Color color );
        void drawLine( SDL_Point p1, SDL_Point p2, SDL_Color color );
        void updateWindow();
        void clearBuffers();

    protected:

    private:
        // Window and render vars
        SDL_Window*      w_window;
        SDL_Renderer*    r_renderer;
        SDL_Texture*     r_ptexture; // texture for direct pixel drawing
        SDL_Texture*     r_ltexture; // texture for line drawing
        SDL_PixelFormat* r_format;

        // Pixel access
        Uint32 *pixels_direct = nullptr;
        Uint32 *null_pixels;

        int r_pitch = 0;
        // Line vectors
        std::vector< SDL_Point > line_points;
        std::vector< SDL_Color > line_colors;

        unsigned int w_width;
        unsigned int w_height;
        double r_scale;
        unsigned int r_width;
        unsigned int r_height;

        // Internal functions
        void LockRTexture();
        void UnlockRTexture();
};

#endif // WINDOW_H
