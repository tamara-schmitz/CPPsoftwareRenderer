#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>


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
        unsigned int GetFPSLimit() { return r_fpsLimit; }
        Uint64 GetFrametime() { return r_frametime_nano; } // return frametime in ns
        Uint64 GetMaintime()  { return r_maintime_nano; }  // return maintime in ns
        float GetFramerate()
        {
            if ( r_frametime_nano > 0 )
            {
                return (1000.0 * 1000.0 * 1000.0) / r_frametime_nano;
            }
            else
            {
                return 1000.0;
            }
        }

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
        unsigned int w_width;
        unsigned int w_height;
        double r_scale;
        unsigned int r_width;
        unsigned int r_height;
        unsigned int r_fpsLimit;

        // tick vars
        Uint64 r_tickLast;
        Uint64 r_tickNow      = 0;
        Uint64 r_tickLast_main;
        Uint64 r_tickNow_main = 0;
        Uint64 r_frametime_nano = 0; // time between last and previous frames were shown
        Uint64 r_maintime_nano  = 0; // time required to actually render a frame
        double r_tickLimit    = 0;

        // internal logic functions
        void tickCall();
};

#endif // WINDOW_H
