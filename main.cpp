#include "window.h"
#include "starfield.h"
#include <vector>
#include <exception>
#include <stdexcept>

void handle_eptr(std::exception_ptr eptr) // passing by value is ok
{
    try {
        if (eptr) {
            std::rethrow_exception(eptr);
        }
    } catch(const std::exception& e) {
        std::cout << "Caught exception \"" << e.what() << "\"\n";
    }
}

void demo_randomPixels(Window *window, SDL_Texture *texture)
{
    std::vector< unsigned char > pixels( window->Getwidth() * window->Getheight() * 4, 0);
    bool running = true;

    while ( running )
    {
        SDL_Event e;
        while ( SDL_PollEvent( &e ) )
        {
            if ( e.type == SDL_QUIT )
            {
                running = false;
                break;
            }
        }

        // splat down some random pixels
        for ( unsigned int i = 0; i < 1000; i++ )
        {
            const unsigned int x = rand() % window->Getwidth();
            const unsigned int y = rand() % window->Getheight();

            const unsigned int offset = ( window->Getwidth() * 4 * y ) + x * 4;
            pixels[ offset + 0 ] = rand() % 256;        // b
            pixels[ offset + 1 ] = rand() % 256;        // g
            pixels[ offset + 2 ] = rand() % 256;        // r
            pixels[ offset + 3 ] = SDL_ALPHA_OPAQUE;    // a
        }

        SDL_UpdateTexture(texture, NULL, &pixels[0], window->Getwidth() * 4);
        window->updateWindow();
        std::cout << "Framerate: "  << window->GetFramerate()
                  << " Frametime: " << window->GetFrametime()
                  << " Maintime: "  << window->GetMaintime()
                  << std::endl;
    }
}

void demo_starfield(Window *window, SDL_Texture *texture)
{
    Starfield field = Starfield( window->Getwidth(), window->Getheight(), 0.3, 0.8, 80, 100000, .5);

    std::vector< unsigned char > pixels( window->Getwidth() * window->Getheight() * 4, 0);

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

            //window->clearRenderer();
            pixels = field.drawStarfield();
            SDL_UpdateTexture(texture, NULL, &pixels[0], window->Getwidth() * 4);
            window->updateWindow();
            std::cout << "Framerate: "  << window->GetFramerate()
                  << " Frametime: " << window->GetFrametime()
                  << " Maintime: "  << window->GetMaintime()
                  << std::endl;

    }
}

int main ( int argc, char** argv )
{
    // --switch between demos
    // 0: random pixels
    // 1: starfield
    const int current_demo = 1;

    // create window and texture
    Window window = Window(1024, 768, "Software Renderer", 61);
    SDL_Texture *texture = window.GetRenderTexture();

    switch( current_demo )
    {
        case 0:
            demo_randomPixels( &window, texture );
            break;
        case 1:
            demo_starfield( &window, texture );
            break;
    }

    return 0;
}
