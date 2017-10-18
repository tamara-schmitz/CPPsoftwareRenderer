#include "common.h"
#include <tclap/CmdLine.h>
#include "vmath-0.12/vmath.h"
#include "window/window.h"
#include "early_demos/starfield.h"
#include "early_demos/scanRenderer.h"
#include "rendering/renderer.h"

// This project is based on BennyQBD's 3D software renderer project
// Github: https://github.com/BennyQBD/3DSoftwareRenderer
// Youtube-Playlist: https://www.youtube.com/playlist?list=PLEETnX-uPtBUbVOok816vTl1K9vV1GgH5

//USAGE:
//
//   ./build/SDLsoftwarerenderer_linux64  [-z] [-s] [-t] [-l] [-v] [-i
//                                        <Integer from 0 to 4>] [--]
//                                        [--version] [-h]

// Global vars
Window *window;

bool checkQuit()
{
    // returns true if program should quit

    if ( testMode && window->timer.GetCurrentTick() > 60 )
    {
        return true;
    }

    if ( !headlessMode )
    {
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
    }

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
        if ( printDebug )
        {
            window->timer.printTimes();
        }
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
        if ( printDebug )
        {
            window->timer.printTimes();
        }
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
        if ( printDebug )
        {
            window->timer.printTimes();
        }
        window->updateTitleWithFPS( 60 );
    }

    // dtor
    delete renderer;
}

void demo_rasteriser( Window *window )
{
    auto render = make_shared<Renderer>( window );
    Do_VP_Clipping = true;

    SDL_Color triangleColor = { 250, 60, 50, SDL_ALPHA_OPAQUE };
    auto bmpTexture = make_shared<Texture>( "examples/tree.bmp" );
    auto sphereModel = make_shared<Mesh>( "examples/sphere.obj" );
    auto chaletTexture = make_shared<Texture>( "examples/chalet.bmp" );
    auto chaletModel = make_shared<Mesh>( "examples/chalet.obj" );

//    Matrix4f objMatrix = Matrix4f::createTranslation( 0, 0, 0 );
//    Matrix4f objMatrix = Matrix4f::createScale( 0.2f, 0.2f, 0.2f );
    float absoluteRotation = 0.0f;
    Matrix4f objMatrix_mesh = Matrix4f::createRotationAroundAxis( 0, 0, 90 );
    Matrix4f objMatrix_triangle = Matrix4f::createTranslation( 0, 0, 0 );
    Matrix4f viewMatrix = Matrix4f::createTranslation( 0, 0, 0.5f ) * Matrix4f::createScale( 0.125f, 0.125f, 0.125f );
    render->SetWorldToViewMatrix( viewMatrix );
    render->SetViewToPerspectiveMatrix( 70, 0.4f, 1.5f );

    render->SetDrawColour( triangleColor );
    render->SetDrawTexture( bmpTexture );

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
        render->ClearBuffers();

        // get per frame rotation factor
        float rotationFactor = 75 * (window->timer.GetDeltaTime() / 1000000000.0);
        absoluteRotation += rotationFactor;
        Matrix4f rotationMatrix = Matrix4f::createRotationAroundAxis( 0, rotationFactor, 0 );

        tris *= rotationMatrix;

        if ( printDebug )
        {
            cout << "v1 - x: " << tris.verts[0].posVec.x << " y: " << tris.verts[0].posVec.y << " z: " << tris.verts[0].posVec.z << endl;
        }

        // draw triangle
        render->SetDrawTexture( bmpTexture );
        render->SetObjectToWorldMatrix( objMatrix_triangle );
        render->FillTriangle( tris );

        // draw mesh
        objMatrix_mesh = Matrix4f::createRotationAroundAxis( 90, 0, absoluteRotation );
        render->SetObjectToWorldMatrix( objMatrix_mesh );
        render->DrawMesh( sphereModel );
        render->SetDrawTexture( chaletTexture );
//        render->DrawMesh( chaletModel );

        if ( !ignoreZBuffer )
        {
            render->DrawFarPlane();
//            render->DrawNearPlane();
        }

        render->WaitUntilFinished();
        window->updateWindow();
        if ( printDebug )
        {
            window->timer.printTimes();
        }
        window->updateTitleWithFPS( 60 );
    }
}

void demo_DisplayTexture( Window* window )
{
    auto texture1 = make_shared<Texture>( "examples/tree.bmp" );

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
        if ( printDebug )
        {
            window->timer.printTimes();
        }
        window->updateTitleWithFPS( 240 );
    }
}

int main( int argc, char* argv[] )
{

    try
    {
        // configure and parse command line arguments
        TCLAP::CmdLine cmd( "Software renderer written in C++", ' ' );
        TCLAP::ValueArg<int> demo_index( "i", "demo-index", "Index of the demo you would like to run", false, 3, "Integer from 0 to 4" );
        cmd.add( demo_index );
        TCLAP::SwitchArg printDebugStuff( "v", "verbose", "Prints extra debug information in console", cmd, false );
        TCLAP::SwitchArg headless( "l", "headless", "Runs renderer without creating a window", cmd, false );
        TCLAP::SwitchArg testing( "t", "test-mode", "Automatically stops program execution some time", cmd, false );
        TCLAP::SwitchArg slowRender( "s", "slow-rendering", "(demo 3 only!) Update window each time a triangle line was drawn", cmd, false );
        TCLAP::SwitchArg ignoreZ( "z", "ignoreZ", "(demo 3 only!) Ignore Z value stored in Z-buffer during fragment depth test", cmd, false );
        cmd.parse( argc, argv );
        current_demo_index = demo_index.getValue();
        printDebug = printDebugStuff.getValue();
        headlessMode = headless.getValue();
        testMode = testing.getValue();
        slowRendering = slowRender.getValue();
        ignoreZBuffer = ignoreZ.getValue();

        // create window and run demos
        window = new Window( 1024, 768, 1, "Software Renderer", 120 );
        window->timer.SetDeltaLimits( (1.0/20.0) * 1000 ); // delta time >= 20 FPS

        // --switch between demos
        // 0: random pixels
        // 1: starfield
        // 2: shapes
        // 3: rasteriser
        // 4: load BMP image file and draw it using the texture class (slow!)
        switch( current_demo_index )
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
            default:
                demo_rasteriser( window );
        }

        // dtor
        delete window;
    }
    catch ( TCLAP::ArgException &e )
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 1;
    }
    catch ( std::exception& e )
    {
        std::cerr << "Standard exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
