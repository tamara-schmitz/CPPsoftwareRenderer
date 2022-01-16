#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"

#include <stdio.h>
#include <sstream>

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
        SDL_Surface* GetSurface() { return SDL_GetWindowSurface( w_window ); }

        // Window functions
        void updateTitleWithFPS( int updateInterval ); // interval in seconds

        // Timer object
        Timer timer;

        // Render functions
        void drawSurface( SDL_Surface* surface, const SDL_Rect* dstrect );
        void drawPixel( int x, int y, const SDL_Color* color );
        void drawPixel( int x, int y, const SDL_Color& color );
        void reserveAddLines( Uint64 amount ); // reserves specified amount of lines (in addition to current reservation)
        void drawLine( SDL_Point p1, SDL_Point p2, SDL_Color* color );
        void updateWindow();
        void clearBuffers();

    protected:

    private:
        // Window and render vars
        SDL_Window*      w_window;
        SDL_Renderer*    r_renderer;
        SDL_Texture*     r_ptexture; // texture for direct pixel drawing
        SDL_Texture*     r_ltexture; // texture for line drawing

        // properties
        unsigned int w_width;
        unsigned int w_height;
        std::string w_title;
        double r_scale;
        unsigned int r_width;
        unsigned int r_height;

        // Pixel access pointers
        Uint32 *pixels_direct = nullptr;
        unique_ptr< Uint32[] >null_pixels = nullptr;

        // Line vectors
        std::vector< SDL_Point > line_points;
        std::vector< SDL_Color > line_colors;

        // State vars
        int r_pitch = 0;
        int r_pitch_div_4 = 0;
        std::chrono::system_clock::time_point w_title_fps_time = std::chrono::system_clock::now(); // contains time at which we are supposed to update window title 

        // Internal functions
        void LockRTexture();
        void UnlockRTexture();
};

#endif // WINDOW_H
