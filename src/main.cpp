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

// Global vars
Window *window;

// --switch between demos
// 0: random pixels
// 1: starfield
// 2: shapes
// 3: rasteriser
// 4: load BMP and draw it using the texture class (slow!)
const int current_demo = 3;

bool checkQuit()
{
    // returns true if program should quit

    #ifdef MODE_TEST
        if ( window->timer.GetCurrentTick() > 60 )
        {
            return true;
        }
    #endif

    #ifndef MODE_HEADLESS
        SDL_Event e;
        while ( SDL_PollEvent( &e ) )
        {
            if ( e.type == SDL_QUIT || e.window.event == SDL_WINDOWEVENT_CLOSE )
            {
                cout << "SDL QUIT EVENT!" << endl;
                return true;
            }

            if ( e.window.event == SDL_WINDOWEVENT_HIDDEN || e.window.event == SDL_WINDOWEVENT_MINIMIZED )
            {
                cout << "PAUSING EXECUTION!" << endl;

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
                            cout << "CONTINUING EXECUTION!" << endl;
                            return false;
                        }
                    }
                }
            }
        }
    #endif

    return false;
}

void demo_randomPixels( Window* window )
{
    bool running = true;

    while ( running )
    {
        running = !checkQuit();

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
        running = !checkQuit();

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
        running = !checkQuit();

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

    #ifdef PRINT_DEBUG_STUFF
        raster->slowRendering = false ;
    #endif

    SDL_Color triangleColor = { 250, 60, 50, SDL_ALPHA_OPAQUE };
    auto bmpTexture = shared_ptr<Texture>( new Texture( "examples/tree.bmp" ) );
    auto sphereModel = shared_ptr<Mesh>( new Mesh( "examples/sphere.obj" ) );
    auto chaletTexture = shared_ptr<Texture>( new Texture( "examples/chalet.bmp" ) );
    auto chaletModel = shared_ptr<Mesh>( new Mesh( "examples/chalet.obj" ) );

//    Matrix4f objMatrix = Matrix4f::createTranslation( 0, 0, 0 );
//    Matrix4f objMatrix = Matrix4f::createScale( 0.2f, 0.2f, 0.2f );
    float absoluteRotation = 0.0f;
    Matrix4f objMatrix_mesh = Matrix4f::createRotationAroundAxis( 0, 0, 90 );
    Matrix4f objMatrix_triangle = Matrix4f::createTranslation( 0, 0, 0 );
    Matrix4f viewMatrix = Matrix4f::createTranslation( 0, 0, 5.0f );
    raster->UpdateViewMatrix( viewMatrix );
    raster->UpdatePerspectiveMatrix( 70, 0.1f, 1000.0f );

    raster->SetDrawColour( triangleColor );
    raster->SetDrawTexture( bmpTexture );

    // triangle 3D
    Triangle tris = Triangle();
    tris.verts[0].posVec = Vector4f {   -1, -1, 0, 1 };
    tris.verts[1].posVec = Vector4f {    0,  1, 0, 1 };
    tris.verts[2].posVec = Vector4f {    1, -1, 0, 1 };
    tris.verts[0].texVec = Vector2f {    0,  1 }; // texels starts at bottom,left. texture at top,left.
    tris.verts[1].texVec = Vector2f { 0.5f,  0 }; // texels coords have to compensate for that.
    tris.verts[2].texVec = Vector2f {    1,  1 };

    bool running = true;
    while ( running )
    {
        running = !checkQuit();

        window->clearBuffers();

        // get per frame rotation factor
        float rotationFactor = 75 * (window->timer.GetDeltaTime() / 1000000000.0);
        absoluteRotation += rotationFactor;
        Matrix4f rotationMatrix = Matrix4f::createRotationAroundAxis( 0, rotationFactor, 0 );

        tris *= rotationMatrix;

        #ifdef PRINT_DEBUG_STUFF
            cout << "v1 - x: " << tris.verts[0].posVec.x << " y: " << tris.verts[0].posVec.y << " z: " << tris.verts[0].posVec.z << endl;
        #endif // PRINT_DEBUG_STUFF

        // draw triangle
        raster->SetDrawTexture( bmpTexture );
        raster->UpdateObjectToWorldMatrix( objMatrix_triangle );
        raster->FillTriangle( tris );

        // draw mesh
        objMatrix_mesh = Matrix4f::createRotationAroundAxis( 0, absoluteRotation, 0 );
        raster->UpdateObjectToWorldMatrix( objMatrix_mesh );
        raster->DrawMesh( sphereModel );
        raster->SetDrawTexture( chaletTexture );
        // raster->DrawMesh( chaletModel );

        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
            window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
        window->updateTitleWithFPS( 60 );
    }
}

void demo_DisplayTexture( Window* window )
{
    auto texture1 = shared_ptr<Texture>( new Texture( "examples/tree.bmp" ) );

    // draw loop
    bool running = true;
    while ( running )
    {
        running = !checkQuit();

        window->clearBuffers();

        //draw texture
        for ( Uint16 y = 0; y < texture1->GetHeight(); y++ ) // iterate over ys
        {
            for ( Uint16 x = 0; x < texture1->GetWidth(); x++ ) // iterate over xs
            {
                window->drawPixel( x, y, texture1->GetPixel( x, y ) );
            }
        }

        window->updateWindow();
        #ifdef PRINT_DEBUG_STUFF
            window->timer.printTimes();
        #endif // PRINT_DEBUG_STUFF
        window->updateTitleWithFPS( 240 );
    }
}

int main( int argc, char* argv[] )
{

    try
    {
        // create window and run demos
        window = new Window( 1024, 768, 1, "Software Renderer", 120 );
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
        cout << "Standard exception: " << e.what() << endl;
    }

    return 0;
}
