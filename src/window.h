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
        float GetFrametime() { return r_frametime_nano / ( 1000.0 * 1000.0 ); }
        float GetMaintime()  { return r_maintime_nano  / ( 1000.0 * 1000.0 ); }
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
        SDL_Window* w_window;
        SDL_Renderer* r_renderer;
        SDL_Texture* r_texture;
        std::vector< Uint8 > pixels;
        unsigned int w_width;
        unsigned int w_height;
        double r_scale;
        unsigned int r_width;
        unsigned int r_height;
        unsigned int r_fpsLimit;

        // tick vars
        //long r_tickLast;
        //long r_tickNow      = 0;
        //long r_tickLast_main;
        //long r_tickNow_main = 0;
        std::chrono::nanoseconds r_tickLast;
        std::chrono::nanoseconds r_tickNow = std::chrono::nanoseconds::zero();
        std::chrono::nanoseconds r_tickLast_main;
        std::chrono::nanoseconds r_tickNow_main = std::chrono::nanoseconds::zero();
        long r_frametime_nano = 0; // time between last 2 frames were shown
        long r_maintime_nano  = 0; // time required to actually render a frame
        double r_tickLimit    = 0;

        // internal logic functions
        void tickCall();
};

#endif // WINDOW_H
