#include <exception>
#include <vector>
#include "vmath-0.12/vmath.h"
#include "common.h"
#include "window/window.h"
#include "early_demos/starfield.h"
#include "early_demos/scanRenderer.h"
#include "rasteriser.h"

// This project is based on BennyQBD's 3D software renderer project
// Github: https://github.com/BennyQBD/3DSoftwareRenderer
// Youtube-Playlist: https://www.youtube.com/playlist?list=PLEETnX-uPtBUbVOok816vTl1K9vV1GgH5


// --switch between demos
// 0: random pixels
// 1: starfield
// 2: shapes
// 3: rasteriser
const int current_demo = 3;

bool checkSDLQuit()
{
    SDL_Event e;
    while ( SDL_PollEvent( &e ) )
    {

        if ( e.window.event == SDL_WINDOWEVENT_HIDDEN || e.window.event == SDL_WINDOWEVENT_MINIMIZED )
        {
            // loop if window is hidden
            std::cout << "PAUSING EXECUTION!" << std::endl;

            bool pause = true;
            while ( pause )
            {
                SDL_Delay( 10 );
                while ( SDL_WaitEvent( &e ) )
                {
                    if ( e.window.event == SDL_WINDOWEVENT_SHOWN ||
                         e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                         e.window.event == SDL_WINDOWEVENT_EXPOSED )
                    {
                        pause = false;
                        std::cout << "CONTINUE EXECUTION!" << std::endl;
                        return false;
                    }
                }
            }
        }
        if ( e.type == SDL_QUIT )
        {
            return true;
        }
    }
    return false;
}

void demo_randomPixels( Window* window, bool& continue_loop )
{
    bool running = true;

    while ( running && continue_loop )
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
        window->updateTitleWithFPS( 60 );
    }
}

void demo_starfield( Window *window, bool& continue_loop )
{
    Starfield *field = new Starfield( 1, 100, 40, window, 100000, 10 );

    bool running = true;
    while ( running && continue_loop )
    {
        running = !checkSDLQuit();

        window->clearBuffers();

        field->drawStarfield();

        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
            window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
        window->updateTitleWithFPS( 60 );

    }

    // dtor
    delete field;
}

void demo_shapes( Window *window, bool& continue_loop )
{
    ScanRenderer *renderer = new ScanRenderer( window, true );
    renderer->UpdatePerspective( 70, 2, 2000 );

    SDL_Color color2D = { 50, 200, 20, SDL_ALPHA_OPAQUE };
    SDL_Color triangleColor = { 255, 50, 50, SDL_ALPHA_OPAQUE };
    SDL_Color triangleColor2 = { 60, 170, 150, SDL_ALPHA_OPAQUE };

    // triangle 2D
    Vector2f v1_2d = Vector2f { 200, 53};
    Vector2f v2_2d = Vector2f { -20, 10};
    Vector2f v3_2d = Vector2f { 100, 364};
    // triangle 3D
    Vector4f v1 = Vector4f { -0.9f, -0.9f, 0, 1 };
    Vector4f v2 = Vector4f {    0,  0.9f, 0, 1 };
    Vector4f v3 = Vector4f {  0.9f, -0.9f, 0, 1 };
    // triangle 3D no.2
    Vector4f v1_no2 = Vector4f { -0.7f, -2.85f, 1.2f, 1 };
    Vector4f v2_no2 = Vector4f {    -0.2f,  0.2f, 0.2f, 1 };
    Vector4f v3_no2 = Vector4f {  1.84f, -1.2f, 0.1f, 1 };

    float rotationFactor = 0;

    bool running = true;
    while ( running && continue_loop )
    {
        running = !checkSDLQuit();

        window->clearBuffers();

        // get per frame rotation factor
        rotationFactor = 100 * (window->timer.GetDeltaTime() / 1000000000.0);

        // rotate and then translate triangle
        Matrix4f rotationMatrix = Matrix4f::createRotationAroundAxis( 0, rotationFactor, 0);
        Matrix4f rotationMatrix_no2 = Matrix4f::createRotationAroundAxis( rotationFactor * 0.2f, - rotationFactor * 0.3f, rotationFactor * 0.1f );
        v1 = rotationMatrix * v1;
        v2 = rotationMatrix * v2;
        v3 = rotationMatrix * v3;
        v1_no2 = rotationMatrix_no2 * v1_no2;
        v2_no2 = rotationMatrix_no2 * v2_no2;
        v3_no2 = rotationMatrix_no2 * v3_no2;

        // draw triangles
        renderer->FillTriangle( v1_2d, v2_2d, v3_2d, color2D );
        renderer->FillTriangle( v1, v2, v3, triangleColor );
        renderer->FillTriangle( v1_no2, v2_no2, v3_no2, triangleColor2 );

        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
            window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
        window->updateTitleWithFPS( 60 );
    }

    // dtor
    delete renderer;
}

void demo_rasteriser( Window *window, bool& continue_loop )
{
    Rasteriser* raster = new Rasteriser( window );

    SDL_Color triangleColor = { 250, 60, 50, SDL_ALPHA_OPAQUE };
    Matrix4f viewMatrix = Matrix4f::createTranslation( 0, -0.4f, 3 );

    raster->UpdateViewAndPerspectiveMatrix( viewMatrix, 80, 0.1f, 0.9f );
    raster->SetRenderColour( triangleColor );

    // triangle 3D
    Vector4f v1 = Vector4f { -0.9f, -0.9f, 0, 1 };
    Vector4f v2 = Vector4f {     0,  0.9f, 0, 1 };
    Vector4f v3 = Vector4f {  0.9f, -0.9f, 0, 1 };

    bool running = true;
    while ( running && continue_loop )
    {
        running = !checkSDLQuit();

        window->clearBuffers();

        // get per frame rotation factor
        float rotationFactor = 100 * (window->timer.GetDeltaTime() / 1000000000.0);
        // rotate and then translate triangle
        Matrix4f rotationMatrix = Matrix4f::createRotationAroundAxis( 0, rotationFactor, 0);

        // apply rotation
        v1 = rotationMatrix * v1;
        v2 = rotationMatrix * v2;
        v3 = rotationMatrix * v3;
        #ifdef PRINT_DEBUG_STUFF
            std::cout << "v1 - x: " << v1.x << " y: " << v1.y << " z: " << v1.z << std::endl;
        #endif // PRINT_DEBUG_STUFF

        // draw triangle
        raster->FillTriangle( v1, v2, v3 );

        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
            window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
        window->updateTitleWithFPS( 60 );
    }

    // dtor
    delete raster;
}

int main( int argc, char* argv[] )
{
    // set to false to halt demo
    bool keep_running = true;

    try
    {
        // create window and texture
        Window *window = new Window( 1024, 768, 1, "Software Renderer", 0 );
        window->timer.SetDeltaLimits( (1.0/20.0) * 1000 );

        switch( current_demo )
        {
            case 0:
                demo_randomPixels( window, keep_running );
                break;
            case 1:
                demo_starfield( window, keep_running );
                break;
            case 2:
                demo_shapes( window, keep_running );
                break;
            case 3:
                demo_rasteriser( window, keep_running );
                break;
        }

        // dtor
        delete window;
    }
    catch ( std::exception& e )
    {
        std::cout << "Standard exception: " << e.what() << std::endl;
    }
    return 0;
}
