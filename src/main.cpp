#include "window.h"
#include "starfield.h"
#include "scanRenderer.h"
#include <vector>
#include "typedefs.h"
#include "vmath-0.12/vmath.h"
#include <exception>

bool checkSDLQuit()
{
    SDL_Event e;
    while ( SDL_PollEvent( &e ) )
    {
        if ( e.type == SDL_QUIT )
        {
            return true;
        }
    }
    return false;
}

void demo_randomPixels( Window* window )
{
    bool running = true;

    while ( running )
    {
        running = !checkSDLQuit();

        window->clearBuffers();
        // splat down some random pixels
        for ( unsigned int i = 0; i < 50000; i++ )
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
        window->timer.printTimes();
    }
}

void demo_starfield( Window *window )
{
    Starfield field = Starfield( 0.3f, 0.8f, 80, window, 50000, 0.1f);

    bool running = true;
    while ( running )
    {
        running = !checkSDLQuit();

        window->clearBuffers();
        field.drawStarfield();
        window->updateWindow();
        window->timer.printTimes();

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
    SDL_Color triangleColor;
    triangleColor.r = 255;
    triangleColor.g = 50;
    triangleColor.b = 50;
    triangleColor.a = SDL_ALPHA_OPAQUE;

    // triangle
    Vector2f v1 = Vector2f {  50,  50 };
    Vector2f v2 = Vector2f { 300, 200 };
    Vector2f v3 = Vector2f { 100, 350 };

    Vector2f vv1 = Vector2f { 20, 10 };
    vv1.normalize();
    std::cout << "VV1: " << vv1.x << " " << vv1.y;

    bool running = true;
    while ( running )
    {
        running = !checkSDLQuit();

        window->clearBuffers();

        // draw a shape
        for ( int i = 100; i < 150; i++ )
        {
            renderer.DrawToScanBuffer( i, 300 - i, 300 + i );
        }
        renderer.FillShape( 100, 150, shapeColor );

        // draw a triangle
        renderer.FillTriangle( v1, v2, v3, triangleColor );

        window->updateWindow();
        window->timer.printTimes();
    }
}

int main( int argc, char** argv )
{
    // --switch between demos
    // 0: random pixels
    // 1: starfield
    // 2: shapes
    const int current_demo = 1;

    try
    {
        // create window and texture
        Window *window = new Window(800, 600, 1, "Software Renderer", 60);

        switch( current_demo )
        {
            case 0:
                demo_randomPixels( window );
                break;
            case 1:
                demo_starfield( window );
                break;
            case 2:
                demo_shapes( window );
        }

        //delete window;
    }
    catch ( std::exception& e )
    {
        std::cout << "Standard exception: " << e.what() << std::endl;
    }
    return 0;
}

