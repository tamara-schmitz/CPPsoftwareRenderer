#include "window.h"
#include "starfield.h"
#include "scanRenderer.h"
#include <vector>

void demo_randomPixels( Window *window )
{
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

        //window->clearBuffers();
        // splat down some random pixels
        for ( unsigned int i = 0; i < 1000; i++ )
        {
            const unsigned int x = rand() % window->Getwidth();
            const unsigned int y = rand() % window->Getheight();

            SDL_Color color;
            color.a = SDL_ALPHA_OPAQUE;
            color.b = rand() % 256;
            color.g = rand() % 256;
            color.r = rand() % 256;
            window->drawPixel( x, y, color );
        }

        window->updateWindow();
        std::cout << "Framerate: "  << window->GetFramerate()
                  << " Frametime: " << window->GetFrametime()
                  << " Maintime: "  << window->GetMaintime()
                  << std::endl;
    }
}

void demo_starfield( Window *window )
{
    Starfield field = Starfield( 0.3, 0.8, 100, window, 5000, 1);

    //std::vector< unsigned char > pixels( window->Getwidth() * window->Getheight() * 4, 0);

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

        window->clearBuffers();
        field.drawStarfield();
        window->updateWindow();
        std::cout << "Framerate: "  << window->GetFramerate()
                  << " Frametime: " << window->GetFrametime()
                  << " Maintime: "  << window->GetMaintime()
                  << std::endl;

    }
}

void demo_shapes( Window *window )
{
    ScanRenderer renderer = ScanRenderer( window );

    SDL_Color shapeColor;
    shapeColor.r = 200;
    shapeColor.g = 200;
    shapeColor.b = 200;
    shapeColor.a = SDL_ALPHA_OPAQUE;

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

        window->clearBuffers();

        // draw a shape
        for ( int i = 100; i < 200; i++ )
        {
            renderer.DrawToScanBuffer( i, 400 - i, 400 + i );
        }
        renderer.FillShape( 100, 200, shapeColor );

        window->updateWindow();
    }
}

int main ( int argc, char** argv )
{
    // --switch between demos
    // 0: random pixels
    // 1: starfield
    // 2: shapes
    const int current_demo = 2;

    // create window and texture
    Window window = Window(1024, 768, "Software Renderer", 62);

    switch( current_demo )
    {
        case 0:
            demo_randomPixels( &window );
            break;
        case 1:
            demo_starfield( &window );
            break;
        case 2:
            demo_shapes( &window );
    }

    return 0;
}
