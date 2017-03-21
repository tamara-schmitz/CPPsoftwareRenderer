#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "timer.h"

class Window
{
    public:
        // cstor
        Window( int width, int height, double scale, std::string title, double fpsLock );
        // dstor
        virtual ~Window();

        // Getters
        SDL_Texture* GetRenderTexture() { return r_ptexture; }
        unsigned int Getwidth() { return r_width; }
        unsigned int Getheight() { return r_height; }

        // Window functions
        void updateTitleWithFPS( Uint32 updateInterval );

        // Timer object
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

        // properties
        unsigned int w_width;
        unsigned int w_height;
        std::string w_title;
        double r_scale;
        unsigned int r_width;
        unsigned int r_height;

        // Pixel access pointers
        Uint32 *pixels_direct = nullptr;
        Uint32 *null_pixels;

        // Line vectors
        std::vector< SDL_Point > line_points;
        std::vector< SDL_Color > line_colors;

        // State vars
        int r_pitch = 0;
        Uint32 title_fps_count = 0; // shows how many ticks since last title update

        // Internal functions
        void LockRTexture();
        void UnlockRTexture();
};

#endif // WINDOW_H
