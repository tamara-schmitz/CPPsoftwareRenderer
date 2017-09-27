#include "renderer.h"

Renderer::Renderer( Window* window )
{
    w_window = window;
    SetPerspectiveToScreenSpaceMatrix();

    // init vars with defaults
    z_buffer_empty.resize( w_window->Getwidth() * w_window->Getheight() );
    z_buffer_empty.shrink_to_fit();
    for ( Uint32 i = 0; i < w_window->Getwidth() * w_window->Getheight(); i++ )
    {
        z_buffer_empty.at( i ) = std::numeric_limits< float >::max();
    }
    *z_buffer = z_buffer_empty;

    // create workers
    vertex_processors.push_back( VertexProcessor( in_vpios, out_vpoos ) );
    Uint16 w_height = w_window->Getheight();
    rasterisers.push_back( Rasteriser( out_vpoos, w_window, z_buffer, 0, w_height ) );
}
void Renderer::SetObjectToWorldMatrix( const Matrix4f& objectMatrix )
{
    objMatrix  = shared_ptr< Matrix4f >( new Matrix4f() );
    *objMatrix = objectMatrix;
}
void Renderer::SetWorldToViewMatrix( const Matrix4f& viewMatrix )
{
    this->viewMatrix = viewMatrix;

    for ( Uint32 i = 0; i < vertex_processors.size(); i++ )
    {
        vertex_processors[i].viewMatrix = this->viewMatrix;
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
        vertex_processors[i].perspMatrix = perspMatrix;
        vertex_processors[i].perspScreenMatrix = perspScreenMatrix;
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
        vertex_processors[i].screenMatrix = screenMatrix;
        vertex_processors[i].perspScreenMatrix = perspScreenMatrix;
    }
}

void Renderer::SetDrawColour( const SDL_Color& color )
{
    drawWithTexture = false;
    current_colour = shared_ptr< SDL_Color >( new SDL_Color() );
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
    for ( Uint32 i = 0; i < vertex_processors.size(); i++ )
    {
        vertex_processors[i].ProcessQueue();
    }

    out_vpoos->block_new();
    for ( Uint32 i = 0; i < rasterisers.size(); i++ )
    {
        rasterisers[i].ProcessVPOOArray();
    }
}

void Renderer::DrawFarPlane()
{
    SDL_Color colour = { 255, 0, 255, SDL_ALPHA_OPAQUE };
    // TODO implement using two triangles
    // Generate 2 big triangles in perspective space that the screen and z = far_z.
    // Apply screenspace matrix to triangles and add to out_vpoos
}

void Renderer::DrawNearPlane()
{
    SDL_Color colour = { 255, 0, 255, SDL_ALPHA_OPAQUE };
    // TODO implement using two triangles
    // Generate 2 big triangles in perspective space that the screen and z = near_z.
    // Apply screenspace matrix to triangles and add to out_vpoos
}
