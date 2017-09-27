#include "renderer.h"

Renderer::Renderer( Window* window )
{
    w_window = window;
    SetPerspectiveToScreenSpaceMatrix();
}

void Renderer::SetObjectToWorldMatrix( const Matrix4f& objectMatrix )
{
    objMatrix = objectMatrix;
    objViewMatrix = viewMatrix * objMatrix;
}
void Renderer::SetWorldToViewMatrix( const Matrix4f& viewMatrix )
{
    this->viewMatrix = viewMatrix;
    objViewMatrix = this->viewMatrix * objMatrix;
}
void Renderer::SetViewToPerspectiveMatrix( const float &fov, const float &zNear, const float &zFar )
{
    perspMatrix = Matrix4f::perspectiveTransform( fov, (float) w_window->Getwidth() / (float) w_window->Getheight(), zNear, zFar );
    perspScreenMatrix = screenMatrix * perspMatrix;
}
void Renderer::SetPerspectiveToScreenSpaceMatrix()
{
    screenMatrix = Matrix4f::screenspaceTransform( w_window->Getwidth() / 2.0f, w_window->Getheight() / 2.0f );;
    perspScreenMatrix = screenMatrix * perspMatrix;
}

void Renderer::SetDrawColour( const SDL_Color& color )
{
    drawWithTexture = false;
    current_colour = shared_ptr< SDL_Color >( new SDL_Color() );
    current_colour->r = color.r;
    current_colour->g = color.g;
    current_colour->b = color.b;
    current_colour->a = color.a;
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
    z_buffer->clear();
    z_buffer->resize( w_window->Getwidth() * w_window->Getheight() );
    z_buffer->shrink_to_fit();
    for ( Uint32 i = 0; i < z_buffer->size(); i++ )
    {
        z_buffer->at( i ) = far_z;
    }
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
        in_vpios->push( VPIO( tris, current_texture ) );
    }
    else
    {
        in_vpios->push( VPIO( tris, current_colour ) );
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
