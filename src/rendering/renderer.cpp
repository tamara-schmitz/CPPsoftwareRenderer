#include "renderer.h"

Renderer::Renderer( Window* window, Uint8 vp_thread_count, Uint8 raster_thread_count )
{
    w_window = window;
    SetPerspectiveToScreenSpaceMatrix();

    // init vars with defaults
    in_vpios = make_shared< SafeQueue< VPIO > >();
    out_vpoos = make_shared< SafeDynArray< VPOO > >();
    z_buffer = make_shared< std::vector< float > >();

    z_buffer_empty.resize( w_window->Getwidth() * w_window->Getheight() );
    z_buffer_empty.shrink_to_fit();
    for ( Uint32 i = 0; i < w_window->Getwidth() * w_window->Getheight(); i++ )
    {
        z_buffer_empty.at( i ) = std::numeric_limits< float >::max();
    }
    *z_buffer = z_buffer_empty;

    // create workers
    for ( Uint8 i = 0; i < vp_thread_count; i++ )
    {
        if ( printDebug )
            cout << "'in_vpios' uses: " << in_vpios.use_count() << " 'out_vpoos' uses: " << out_vpoos.use_count() << endl;
        vertex_processors.push_back( make_shared< VertexProcessor >( in_vpios, out_vpoos ) );
//        vp_threads.push_back( vertex_processors[i]->ProcessQueueAsThread() );
    }

    if ( printDebug )
        cout << "Spawned " << vertex_processors.size() << " vertex processing threads." << endl;

    float y_count = 0;
    float y_incre = (float) (w_window->Getheight() - 1) / (float) raster_thread_count;
    for ( Uint8 i = 0; i < raster_thread_count; i++ )
    {
        Uint16 y_begin;
        if ( y_count > 0 && y_count == (float) y_count )
            y_begin = y_count - 1;
        else
            y_begin = std::ceil( y_count );
        y_count += y_incre;
        Uint16 y_end   = std::floor( y_count );

        rasterisers.push_back( Rasteriser( out_vpoos, w_window, z_buffer, y_begin, y_end ) );
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
    perspScreenMatrix = screenMatrix * perspMatrix;

    for ( Uint32 i = 0; i < vertex_processors.size(); i++ )
    {
        vertex_processors[i]->perspMatrix = perspMatrix;
        vertex_processors[i]->perspScreenMatrix = perspScreenMatrix;
    }
    for ( Uint32 i = 0; i < rasterisers.size(); i++ )
    {
        rasterisers[i].near_z = zNear;
        rasterisers[i].far_z  = zFar;
    }
}
void Renderer::SetPerspectiveToScreenSpaceMatrix()
{
    screenMatrix = Matrix4f::screenspaceTransform( w_window->Getwidth() / 2.0f, w_window->Getheight() / 2.0f );;
    perspScreenMatrix = screenMatrix * perspMatrix;

    for ( Uint32 i = 0; i < vertex_processors.size(); i++ )
    {
        vertex_processors[i]->screenMatrix = screenMatrix;
        vertex_processors[i]->perspScreenMatrix = perspScreenMatrix;
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
    *z_buffer = z_buffer_empty;
}

void Renderer::DrawMesh( shared_ptr<Mesh> mesh )
{
    // iterate over each triangle
    for ( Uint32 i = 0; i < mesh->GetTriangleCount(); i++ )
    {
        FillTriangle( mesh->GetTriangle( i ) );
    }
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
        in_vpios->push( VPIO( tris, objMatrix, current_texture ) );
    }
    else
    {
        in_vpios->push( VPIO( tris, objMatrix, current_colour ) );
    }
}

void Renderer::WaitUntilFinished()
{
    // waits for vertex processing and rasteriser to be finished.
    // returns once frame is completed


    // TODO turn this into a thread.join() at some point
    in_vpios->block_new();
//    assert( vertex_processors.size() == vp_threads.size() );
    for ( Uint32 i = 0; i < vertex_processors.size(); i++ )
    {
        vertex_processors[i]->ProcessQueue();
//        vp_threads[i]->join();
    }

    if ( printDebug )
        cout << "out_vpoos queue size after all vps are finished: " << out_vpoos->size() << endl;

    out_vpoos->block_new();
    for ( Uint32 i = 0; i < rasterisers.size(); i++ )
    {
        rasterisers[i].ProcessVPOOArray();
    }
}

void Renderer::DrawDebugPlane( float z_value )
{
    // Sorry but this is quite hacky...

    shared_ptr< SDL_Color > colour = make_shared< SDL_Color >();
    colour->r = colour->b = 255;
    colour->g = 0;
    colour->a = SDL_ALPHA_OPAQUE;
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

    out_vpoos->push_back( VPOO( tri1.verts[0], tri1.verts[1], tri1.verts[2], tri1_handedness, colour ) );
    out_vpoos->push_back( VPOO( tri2.verts[0], tri2.verts[1], tri2.verts[2], tri2_handedness, colour ) );
}

Renderer::~Renderer()
{
    //dtor

    if ( printDebug )
    {
        cout << "Dtor of Renderer object was called!" << endl;
    }
}
