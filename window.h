#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <string>
#include <iostream>


class Window
{
    public:
        // cstor
        Window( int width, int height, const std::string& title, int fpsLock );
        // dstor
        virtual ~Window();

        // Getters
        SDL_Texture* GetRenderTexture() { return r_texture; }
        unsigned int Getwidth() { return w_width; }
        unsigned int Getheight() { return w_height; }
        unsigned int GetFPSLimit() { return r_fpsLimit; }
        double GetFrametime() { return r_frametime; }
        double GetFramerate()
        {
            if ( r_frametime > 0 )
            {
                return 1000 / r_frametime;
            }
            else
            {
                return 1000;
            }
        }

        // Render functions
        void updateWindow();
        void clearRenderer();

    protected:

    private:
        // Window and render vars
        SDL_Window* w_window;
        SDL_Renderer* r_renderer;
        SDL_Texture* r_texture;
        unsigned int w_width;
        unsigned int w_height;
        unsigned int r_fpsLimit;

        // tick vars
        double r_tickLast;
        double r_tickNow      = 0;
        double r_tickLast_main;
        double r_tickNow_main = 0;
        double r_frametime    = 0; // time between last 2 frames were shown
        double r_maintime     = 0; // time required to actually render a frame
        double r_tickLimit    = 0;

        // internal logic functions
        void tickCall();
};

#endif // WINDOW_H
