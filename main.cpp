#include "window.h"
#include "starfield.h"
#include <vector>

int main ( int argc, char** argv )
{
    Window window = Window(800, 600, "Software Renderer", 30);
    SDL_Texture* texture = window.GetRenderTexture();
    Starfield field = Starfield(window.Getwidth(), window.Getheight(), 0.2, 0.8, 1000, 5);

    std::vector< unsigned char > pixels( window.Getwidth() * window.Getheight() * 4, 0);

    bool running = true;
    while ( running )
    {
        // event handling
        SDL_Event e;
        while ( SDL_PollEvent( &e ) )
        {
            if ( e.type == SDL_QUIT )
            {
                running = false;
                break;
            }
        }


        window.clearRenderer();
        pixels = field.drawStarfield();
        SDL_UpdateTexture(texture, NULL, &pixels[0], window.Getwidth() * 4);
        window.updateWindow();
        std::cout << "Framerate: " << window.GetFramerate() << std::endl;
    }

    return 0;

}
