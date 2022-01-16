#include "renderer.h"

Renderer::Renderer( Window* window, Uint8 vp_thread_count, Uint8 raster_thread_count )
{
    w_window = window;
    SetPerspectiveToScreenSpaceMatrix();

    // init vars with defaults
    in_vpios = make_shared< SafeDeque< VPIO > >();
    out_vpoos = make_shared< SafeDeque< VPOO > >();

    // create workers (vps and rasterisers)
    for ( Uint8 i = 0; i < vp_thread_count; i++ )
    {
        if ( printDebug )
            cout << "'in_vpios' uses: " << in_vpios.use_count() << " 'out_vpoos' uses: " << out_vpoos.use_count() << endl;
        vertex_processors.push_back( make_shared< VertexProcessor >( in_vpios, out_vpoos ) );
    }

    if ( printDebug )
        cout << "Spawned " << vertex_processors.size() << " vertex processing threads." << endl;

    // For the rasterisers we split the rendering surface vertically into even parts that do not overlap. Each one has their own SDL_Surface.
    float y_count = 0;
    float y_incre = (float) (w_window->Getheight() - 1) / (float) raster_thread_count;
    for ( Uint8 i = 0; i < raster_thread_count; i++ )
    //for ( Uint8 i = 0; i < 1; i++ )
    {
        Uint16 y_begin;
        if ( y_count > 0 && y_count == (float) y_count )
            y_begin = y_count - 1;
        else
            y_begin = std::ceil( y_count );
        y_count += y_incre;
        Uint16 y_end   = std::floor( y_count );
    
        SDL_Surface* new_surface = SDL_CreateRGBSurfaceWithFormat(0, w_window->Getwidth(), y_end - y_begin, 32, SDL_PIXELFORMAT_ARGB8888);
        SDL_SetSurfaceRLE( new_surface, 0 ); // disable hw acceleration
        if ( new_surface == nullptr )
        {
            cout << "Could not create rasteriser surface! " << SDL_GetError() << endl;
        }
        else
        {
            rasterisers.push_back( make_shared< Rasteriser >( out_vpoos, w_window, shared_ptr< SDL_Surface >(new_surface), y_begin, y_end ) );
            cout << "Rasteriser " << i << " has y_begin " << y_begin << " and y_end " << y_end << endl;
        }
    }

    if ( printDebug )
        cout << "Spawned " << rasterisers.size() << " rasteriser threads." << endl;
}
void Renderer::SetObjectToWorldMatrix( const Matrix4f& objectMatrix )
{
    objMatrix  = make_shared< Matrix4f >();
    *objMatrix = objectMatrix;
}
void Renderer::SetWorldToViewMatrix( const Matrix4f& viewMatrix )
{
    this->viewMatrix = viewMatrix;

    for ( Uint32 i = 0; i < vertex_processors.size(); i++ )
    {
        vertex_processors[i]->viewMatrix = this->viewMatrix;
    }
}
void Renderer::SetViewToPerspectiveMatrix( const float &fov, const float &zNear, const float &zFar )
{
    near_z = zNear;
    far_z  = zFar;
    perspMatrix = Matrix4f::perspectiveTransform( fov, (float) w_window->Getwidth() / (float) w_window->Getheight(), zNear, zFar );

    for ( Uint32 i = 0; i < vertex_processors.size(); i++ )
    {
        vertex_processors[i]->perspMatrix = perspMatrix;
    }
    for ( Uint32 i = 0; i < rasterisers.size(); i++ )
    {
        rasterisers[i]->near_z = zNear;
        rasterisers[i]->far_z  = zFar;
    }
}
void Renderer::SetPerspectiveToScreenSpaceMatrix()
{
    screenMatrix = Matrix4f::screenspaceTransform( w_window->Getwidth() / 2.0f, w_window->Getheight() / 2.0f );;

    for ( Uint32 i = 0; i < vertex_processors.size(); i++ )
    {
        vertex_processors[i]->screenMatrix = screenMatrix;
    }
}

void Renderer::SetDrawColour( const SDL_Color& color )
{
    drawWithTexture = false;
    current_colour = make_shared< SDL_Color >( SDL_Color() );
    *current_colour = color;
}

void Renderer::SetDrawTexture(const shared_ptr< Texture >& texture )
{
    drawWithTexture = true;
    current_texture = texture;
}

void Renderer::ClearBuffers()
{
    in_vpios->clear();
    out_vpoos->clear();
}

void Renderer::DrawMesh( const Matrix4f& objMat, shared_ptr<Mesh> mesh, shared_ptr< Texture >& texture)
{
    VPIO vpio = VPIO( mesh, objMat, texture );
    in_vpios->push_back( vpio );
}

void Renderer::DrawMesh( const Matrix4f& objMat, shared_ptr<Mesh> mesh, const SDL_Color& colour)
{
    VPIO vpio = VPIO( mesh, objMat, colour );
    in_vpios->push_back( vpio );
}

void Renderer::FillTriangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 )
{
    Triangle triangle = Triangle( v1, v2, v3 );
    FillTriangle( triangle );
}

void Renderer::FillTriangle( Triangle tris )
{
    if ( drawWithTexture )
    {
        VPIO vpio = VPIO( tris, *objMatrix, current_texture );
        in_vpios->push_back( vpio );
    }
    else
    {
        VPIO vpio = VPIO( tris, *objMatrix, *current_colour );
        in_vpios->push_back( vpio );
    }
}

void Renderer::InitiateRendering()
{
    in_vpios->unblock_new();
    out_vpoos->unblock_new();

    for ( Uint8 i = 0; i < vertex_processors.size(); i++ )
    {
        vp_threads.push_back( vertex_processors[i]->ProcessQueueAsThread() );
    }
    for ( Uint8 i = 0; i < rasterisers.size(); i++ )
    {
        rast_threads.push_back( rasterisers[i]->ProcessVPOOArrayAsThread() );
    }
}

void Renderer::WaitUntilFinished()
{
    // waits for vertex processing and rasteriser to be finished.
    // returns once frame is completed


    // TODO turn this into a thread.join() at some point

    // Wait for vertex processors
    in_vpios->block_new();
    int i_vp = 0; // tracks vertex_processor processed in queue.
    for ( auto it = vp_threads.begin(); it != vp_threads.end(); )
    {
        vp_threads[0]->join(); // always 0 because we empty the queue and never skip an elemen
        vp_threads.erase( it );

        if ( printDebug )
            cout << "VP " << i_vp << " has processed a total of " << (int) vertex_processors[ i_vp ]->GetProcessedVPIOsCount() << " VPIOs." << endl;

        i_vp++;
    }

    if ( printDebug )
        cout << "out_vpoos queue size after all vps are finished: " << out_vpoos->size() << endl;
    
    // Wait for rasterisers and draw their surfaces
    out_vpoos->block_new();
    int i_rr = 0;
    for ( auto it = rast_threads.begin(); it != rast_threads.end(); )
    {
        rast_threads[0]->join(); // always 0 because we empty the queue and never skip an element
        rast_threads.erase( it );

        Uint32* pixelss = (Uint32*) rasterisers[i_rr]->framebuffer.get()->pixels;
        pixelss[0] = 0xff00ffff;
        pixelss[1] = 0xff00ffff;
        pixelss[2] = 0xff00ffff;
        pixelss[500] = 0xff00ffff;
        pixelss[501] = 0xff00ffff;
        pixelss[502] = 0xff00ffff;

        SDL_Rect dstrect;
        dstrect.x = 0;
        dstrect.y = rasterisers[i_rr]->y_begin;
        dstrect.w = w_window->Getwidth();
        dstrect.h = rasterisers[i_rr]->y_end - rasterisers[i_rr]->y_begin;
        w_window->drawSurface( rasterisers[i_rr]->framebuffer.get(), &dstrect );

        cout << "h " << dstrect.h << " y " << dstrect.y << endl;

        i_rr++;
    }
}

void Renderer::DrawDebugPlane( float z_value )
{
    // Sorry but this is quite hacky...

    SDL_Color colour = SDL_Color();
    colour.r = colour.b = 255;
    colour.g = 0;
    colour.a = SDL_ALPHA_OPAQUE;
    // Generate 2 big triangles in perspective space that cover the screen with z = far_z.
    // Then apply screenspace matrix to triangles and add to out_vpoos
    Triangle tri1, tri2;

    // manually set values of triangle verts
    tri1.verts[0].posVec.z = tri1.verts[1].posVec.z = tri1.verts[2].posVec.z =
           tri2.verts[0].posVec.z = tri2.verts[1].posVec.z = tri2.verts[2].posVec.z = z_value;

    tri1.verts[0].posVec.w = tri1.verts[1].posVec.w = tri1.verts[2].posVec.w =
           tri2.verts[0].posVec.w = tri2.verts[1].posVec.w = tri2.verts[2].posVec.w = 1;

    tri1.verts[0].posVec.x = tri2.verts[0].posVec.x = tri1.verts[0].posVec.y = tri2.verts[0].posVec.y = -1;
    tri1.verts[1].posVec.x = tri2.verts[1].posVec.x = tri1.verts[1].posVec.y = tri2.verts[1].posVec.y = 1;
    tri1.verts[2].posVec.x = -1;
    tri1.verts[2].posVec.y = 1;
    tri2.verts[2].posVec.x = 1;
    tri2.verts[2].posVec.y = -1;

    // go from view to screen space
    tri1 *= screenMatrix;
    tri2 *= screenMatrix;
    tri1.verts[0].posVec.divideByWOnly();
    tri1.verts[1].posVec.divideByWOnly();
    tri1.verts[2].posVec.divideByWOnly();
    tri2.verts[0].posVec.divideByWOnly();
    tri2.verts[1].posVec.divideByWOnly();
    tri2.verts[2].posVec.divideByWOnly();
    tri1.sortVertsByY();
    tri2.sortVertsByY();

    bool tri1_handedness = triangleArea< float >( tri1.verts[0].posVec, tri1.verts[1].posVec, tri1.verts[2].posVec ) < 0;
    bool tri2_handedness = triangleArea< float >( tri2.verts[0].posVec, tri2.verts[1].posVec, tri2.verts[2].posVec ) < 0;

    VPOO vpoo;
    vpoo = VPOO( tri1.verts[0], tri1.verts[1], tri1.verts[2], tri1_handedness, colour );
    out_vpoos->push_back( vpoo );
    vpoo = VPOO( tri2.verts[0], tri2.verts[1], tri2.verts[2], tri2_handedness, colour );
    out_vpoos->push_back( vpoo );
}

Renderer::~Renderer()
{
    //dtor

    if ( printDebug )
    {
        cout << "Dtor of Renderer object was called!" << endl;
    }
}
