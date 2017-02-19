#include "window.h"
#include "starfield.h"
#include "scanRenderer.h"
#include <vector>
#include "typedefs.h"
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

void demo_randomPixels( Window *window )
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
        std::cout << "Framerate: "  << window->GetFramerate()
                  << " Frametime: " << window->GetFrametime() / ( 1000.0f * 1000.0f )
                  << " Maintime: "  << window->GetMaintime() / ( 1000.0f * 1000.0f )
                  << std::endl;
    }
}

void demo_starfield( Window *window )
{
    Starfield field = Starfield( 0.3f, 0.8f, 70, window, 10000, 0.25f);

    //std::vector< unsigned char > pixels( window->Getwidth() * window->Getheight() * 4, 0);

    bool running = true;
    while ( running )
    {
        running = !checkSDLQuit();

        window->clearBuffers();
        field.drawStarfield();
        window->updateWindow();
        std::cout << "Framerate: "  << window->GetFramerate()
                  << " Frametime: " << window->GetFrametime() / ( 1000.0 * 1000.0 )
                  << " Maintime: "  << window->GetMaintime() / ( 1000.0 * 1000.0 )
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
    SDL_Color triangleColor;
    triangleColor.r = 255;
    triangleColor.g = 50;
    triangleColor.b = 50;
    triangleColor.a = SDL_ALPHA_OPAQUE;

    // triangle
    vertex2D v1 = vertex2D {  50,  50 };
    vertex2D v2 = vertex2D { 300, 200 };
    vertex2D v3 = vertex2D { 100, 350 };

    vector2D vv1 = vector2D { 20, 10 };
    vv1 = vv1.normalised();
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

