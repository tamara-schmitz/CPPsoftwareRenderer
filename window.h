#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <string>


class Window
{
    public:
        // cstor
        Window(int width, int height, const std::string& title, int fpsLock);
        // dstor
        virtual ~Window();

        // Getters and Setters
        unsigned int Getwidth() { return w_width; }
        unsigned int Getheight() { return w_height; }
        std::string& Gettitle() { return w_title; }
        void Settitle(char* val) { w_title = val; }
        SDL_Window* Getwindow() { return w_window; }

        // public window vars
        SDL_Window* w_window;
        SDL_Renderer* r_renderer;
        SDL_Texture* r_texture;

    protected:

    private:
        unsigned int w_width; //!< Member variable "w_width"
        unsigned int w_height; //!< Member variable "w_height"
        std::string& w_title; //!< Member variable "w_title"
};

#endif // WINDOW_H
