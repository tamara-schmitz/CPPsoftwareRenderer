#include "window.h"
#include <vector>

int main ( int argc, char** argv )
{
    Window window = Window(800, 600, "Software Renderer", 60);
    SDL_Texture* texture = window.GetRenderTexture();

    std::vector< unsigned char > pixels( window.Getwidth() * window.Getheight() * 4, 0 );
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
            const unsigned int x = rand() % window.Getwidth();
            const unsigned int y = rand() % window.Getheight();

            const unsigned int offset = ( window.Getwidth() * 4 * y ) + x * 4;
            pixels[ offset + 0 ] = rand() % 256;        // b
            pixels[ offset + 1 ] = rand() % 256;        // g
            pixels[ offset + 2 ] = rand() % 256;        // r
            pixels[ offset + 3 ] = SDL_ALPHA_OPAQUE;    // a
        }

        SDL_UpdateTexture(texture, NULL, &pixels[0], window.Getwidth() * 4);
        window.updateWindow();
        std::cout << "Frametime: " << window.GetFramerate() << std::endl;
    }

    return 0;

}
