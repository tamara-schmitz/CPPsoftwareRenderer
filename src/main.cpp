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
        #ifdef PRINT_DEBUG_STUFF
        window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
    }
}

void demo_starfield( Window *window )
{
    Starfield field = Starfield( 0.3f, 0.8f, 80, window, 50000, 0.05f);

    bool running = true;
    while ( running )
    {
        running = !checkSDLQuit();

        window->clearBuffers();
        field.drawStarfield();
        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
        window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF

    }
}

void demo_shapes( Window *window )
{
    ScanRenderer renderer = ScanRenderer( window, true );
    renderer.UpdatePerspective( 70, 2, 2000 );

    SDL_Color color2D;
    color2D.r = 50;
    color2D.g = 200;
    color2D.b = 20;
    color2D.a = SDL_ALPHA_OPAQUE;
    SDL_Color triangleColor;
    triangleColor.r = 255;
    triangleColor.g = 50;
    triangleColor.b = 50;
    triangleColor.a = SDL_ALPHA_OPAQUE;
    SDL_Color triangleColor2;
    triangleColor2.r = 60;
    triangleColor2.g = 170;
    triangleColor2.b = 150;
    triangleColor2.a = SDL_ALPHA_OPAQUE;

    // triangle 2D
    Vector2f v1_2d = Vector2f { 200, 53};
    Vector2f v2_2d = Vector2f { -20, 10};
    Vector2f v3_2d = Vector2f { 100, 364};
    // triangle 3D
    Vector4f v1 = Vector4f { -0.9f, -0.9f, 0, 1 };
    Vector4f v2 = Vector4f {    0,  0.9f, 0, 1 };
    Vector4f v3 = Vector4f {  0.9f, -0.9f, 0, 1 };
    // triangle 3D no.2
    Vector4f v1_no2 = Vector4f { -0.7f, -0.85f, 0, 1 };
    Vector4f v2_no2 = Vector4f {    0.2f,  0.2f, 0, 1 };
    Vector4f v3_no2 = Vector4f {  0.84f, -1.2f, 0.1f, 1 };

    float rotationFactor = 0;

    bool running = true;
    while ( running )
    {
        running = !checkSDLQuit();

        window->clearBuffers();

        // get per frame rotation factor
        rotationFactor = rotationFactor + 0.000000001f * (window->timer.GetFrametime() / 1000000000);

        // rotate and then translate triangle
        Matrix4f rotationMatrix = Matrix4f::createRotationAroundAxis( 0, rotationFactor, 0);
        Matrix4f rotationMatrix_no2 = Matrix4f::createRotationAroundAxis( rotationFactor * 0.5f, 0, rotationFactor * 0.1f );
        v1 = rotationMatrix * v1;
        v2 = rotationMatrix * v2;
        v3 = rotationMatrix * v3;
        v1_no2 = rotationMatrix_no2 * v1_no2;
        v2_no2 = rotationMatrix_no2 * v2_no2;
        v3_no2 = rotationMatrix_no2 * v3_no2;

        // draw triangles
        renderer.FillTriangle( v1_2d, v2_2d, v3_2d, color2D );
        renderer.FillTriangle( v1, v2, v3, triangleColor );
        renderer.FillTriangle( v1_no2, v2_no2, v3_no2, triangleColor2 );

        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
        window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
    }
}

int main( int argc, char** argv )
{
    // --switch between demos
    // 0: random pixels
    // 1: starfield
    // 2: shapes
    const int current_demo = 2;

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

