#include "common.h"
#include <exception>
#include "vmath-0.12/vmath.h"
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
// 4: load BMP and draw it using the texture class (slow!)
const int current_demo = 3;

bool checkSDLQuit()
{
    SDL_Event e;
    while ( SDL_PollEvent( &e ) )
    {
        if ( e.type == SDL_QUIT || e.window.event == SDL_WINDOWEVENT_CLOSE )
        {
            std::cout << "SDL QUIT EVENT!" << std::endl;
            return true;
        }

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
        window->updateTitleWithFPS( 60 );
    }
}

void demo_starfield( Window *window )
{
    Starfield *field = new Starfield( 1, 100, 40, window, 100000, 10 );

    bool running = true;
    while ( running )
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

void demo_shapes( Window *window )
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
    while ( running )
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

void demo_rasteriser( Window *window )
{
    auto raster = shared_ptr<Rasteriser>( new Rasteriser( window ) );

    SDL_Color triangleColor = { 250, 60, 50, SDL_ALPHA_OPAQUE };
    auto triangleTexture = shared_ptr<Texture>( new Texture( (Uint16) 150, (Uint16) 150 ) );
    triangleTexture->FillWithRandomPixels();
    auto bmpTexture = shared_ptr<Texture>( new Texture( "tree.bmp" ) );

    Matrix4f viewMatrix = Matrix4f::createTranslation( 0, 0, 2.5f );

    raster->UpdateViewAndPerspectiveMatrix( viewMatrix, 80, 0.1f, 0.9f );
    raster->SetDrawColour( triangleColor );
    raster->SetDrawTexture( triangleTexture );
    raster->SetDrawTexture( bmpTexture );

    // triangle 3D
    Vertexf v1;
    Vertexf v2;
    Vertexf v3;
    v1.posVec = Vector4f {   -1, -1, 0, 1 };
    v2.posVec = Vector4f {    0,  1, 0, 1 };
    v3.posVec = Vector4f {    1, -1, 0, 1 };
    v1.texVec = Vector4f {    0,  1, 0, 0 }; // texels starts at bottom,left. texture at top,left.
    v2.texVec = Vector4f { 0.5f,  0, 0, 0 }; // texels coords compensate for that.
    v3.texVec = Vector4f {    1,  1, 0, 0 };

    bool running = true;
    while ( running )
    {
        running = !checkSDLQuit();

        window->clearBuffers();

        // get per frame rotation factor
        float rotationFactor = 100 * (window->timer.GetDeltaTime() / 1000000000.0);
        // create rotation matrix
        Matrix4f rotationMatrix = Matrix4f::createRotationAroundAxis( 0, rotationFactor, 0 );

        // apply rotation
        v1.posVec = rotationMatrix * v1.posVec;
        v2.posVec = rotationMatrix * v2.posVec;
        v3.posVec = rotationMatrix * v3.posVec;
        #ifdef PRINT_DEBUG_STUFF
            std::cout << "v1 - x: " << v1.posVec.x << " y: " << v1.posVec.y << " z: " << v1.posVec.z << std::endl;
        #endif // PRINT_DEBUG_STUFF

        // draw triangle
        raster->FillTriangle( v1, v2, v3 );

        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
            window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
        window->updateTitleWithFPS( 120 );
    }
}

void demo_DisplayTexture( Window* window )
{
    // create a texture
    auto texture1 = shared_ptr<Texture>( new Texture( "tree.bmp" ) );

    // draw loop
    bool running = true;
    while ( running )
    {
        running = !checkSDLQuit();

        window->clearBuffers();

        //draw texture
        for ( Uint16 y = 0; y < texture1->GetHeight(); y++ ) // iterate over ys
        {
            for ( Uint16 x = 0; x < texture1->GetWidth(); x++ ) // iterate over xs
            {
                // draw pixel
                window->drawPixel( x, y, texture1->GetPixel( x, y ) );
            }
        }

        // update window
        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
            window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
        window->updateTitleWithFPS( 120 );
    }
}

int main( int argc, char* argv[] )
{

    try
    {
        // create window and run demos
        Window *window = new Window( 1024, 768, 1, "Software Renderer", 0 );
        window->timer.SetDeltaLimits( (1.0/20.0) * 1000 ); // delta time >= 20 FPS

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
                break;
            case 3:
                demo_rasteriser( window );
                break;
            case 4:
                demo_DisplayTexture( window );
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
