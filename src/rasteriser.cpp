#include "rasteriser.h"

Rasteriser::Rasteriser( Window* window )
{
    //ctor
    w_window = window;

    // Init with default values
    UpdateScreenspaceTransformMatrix();
    SetDrawColour( SDL_Color{ 200, 200, 200, SDL_ALPHA_OPAQUE } );

    z_buffer.resize( w_window->Getwidth() * w_window->Getheight() );
    z_buffer.shrink_to_fit();
    z_buffer_empty.resize( w_window->Getwidth() * w_window->Getheight() );
    z_buffer_empty.shrink_to_fit();
    for ( Uint32 i = 0; i < z_buffer_empty.size(); i++ )
    {
        z_buffer_empty.at( i ) = GetFarZ();
    }
    ClearZBuffer();
}

void Rasteriser::SetDrawColour( const SDL_Color& color )
{
    drawWithTexture = false;
    current_colour = color;
}

void Rasteriser::SetDrawTexture(const shared_ptr<Texture> texture )
{
    drawWithTexture = true;
    current_texture = texture;
}

void Rasteriser::UpdateObjectToWorldMatrix( const Matrix4f& objMatrix )
{
    objectToWorldTransformMatrix = objMatrix;
    UpdateMvpsMatrix();
}

void Rasteriser::UpdateViewMatrix( const Matrix4f& viewMatrix )
{
    viewSpaceTransformMatrix = viewMatrix;
    UpdateMvpsMatrix();
}

void Rasteriser::UpdatePerspectiveMatrix( const float& fov, const float& zNear, const float& zFar )
{
//    perspectiveTransformMatrix = Matrix4f::createFrustum( fov, w_window->Getwidth(), w_window->Getheight(), zNear, zFar );
    perspectiveTransformMatrix = Matrix4f::perspectiveTransform( fov, (float) w_window->Getwidth() / (float) w_window->Getheight(), zNear, zFar );
    UpdateMvpsMatrix();
    near_z = zNear;
    far_z = zFar;

    // adjust z_buffer for new farz
    for ( Uint32 i = 0; i < z_buffer_empty.size(); i++ )
    {
        z_buffer_empty.at( i ) = std::numeric_limits< float >::max();
    }
    ClearZBuffer();
}
void Rasteriser::UpdateScreenspaceTransformMatrix()
{
    screenspaceTransformMatrix = Matrix4f::screenspaceTransform( w_window->Getwidth() / 2.0f, w_window->Getheight() / 2.0f );
    UpdateMvpsMatrix();
}

void Rasteriser::DrawFarPlane()
{
    SDL_Color colour = { 255, 0, 255, SDL_ALPHA_OPAQUE };
    SetDrawColour( colour );

    for ( Uint16 y = 0; y < w_window->Getheight(); y++ )
    {
        for ( Uint16 x = 0; x < w_window->Getwidth(); x++ )
        {
            DrawFragment( x, y, GetFarZ() );
        }
    }
}

void Rasteriser::DrawNearPlane()
{
    SDL_Color colour = { 255, 0, 255, SDL_ALPHA_OPAQUE };
    SetDrawColour( colour );

    for ( Uint16 y = 0; y < w_window->Getheight(); y++ )
    {
        for ( Uint16 x = 0; x < w_window->Getwidth(); x++ )
        {
            DrawFragment( x, y, GetNearZ() );
        }
    }
}

void Rasteriser::DrawMesh( shared_ptr<Mesh> mesh )
{
    // iterate over each triangle
    for ( Uint32 i = 0; i < mesh->GetTriangleCount(); i++ )
    {
        // fill triangle
        FillTriangle( mesh->GetTriangle( i ) );
    }
}

void Rasteriser::FillTriangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 )
{
    Triangle triangle = Triangle( v1, v2, v3 );
    FillTriangle( triangle );
}

void Rasteriser::FillTriangle( Triangle tris )
{
    tris *= mvpsMatrix;

    // perspective divison
    tris.verts[0].posVec.divideByWOnly();
    tris.verts[1].posVec.divideByWOnly();
    tris.verts[2].posVec.divideByWOnly();

    // // cull triangle if z of every posvec is smaller than near plane or bigger than far plane
    // if ( ( tris.verts[0].posVec.z < GetNearZ() && tris.verts[1].posVec.z < GetNearZ() && tris.verts[2].posVec.z < GetNearZ() ) ||
    //      ( tris.verts[0].posVec.z > GetFarZ() && tris.verts[1].posVec.z > GetFarZ() && tris.verts[2].posVec.z > GetFarZ()) )
    // {
    //     if ( printDebug )
    //     {
    //         cout << "Culled because v1.posVec.z: " << tris.verts[0].posVec.z << " v2.posVec.z: " << tris.verts[1].posVec.z << " v3.posVec.z: " << tris.verts[2].posVec.z << endl;
    //     }
    //     return;
    // }

    // calculate handedness
    float area = triangleArea< float >( tris.verts[0].posVec, tris.verts[1].posVec, tris.verts[2].posVec );

    // true if right handed (and hence area bigger than 0)
    bool handedness = area < 0;
    if ( printDebug )
    {
        cout << "Area: " << area << endl;
    }
    // cull triangle if right-handed (we use left-handed cartesian coordinates)
    if ( handedness )
    {
        if ( printDebug )
        {
            cout << "Triangle culled because right-handed." << endl;
        }
        return;
    }

    tris.sortVertsByY();

    if ( printDebug )
    {
        cout << "yMinVert - x: " << tris.verts[0].posVec.x << " y: " << tris.verts[0].posVec.y << " z: " << tris.verts[0].posVec.z << endl;
    }


    ScanTriangle( tris.verts[0], tris.verts[1], tris.verts[2], handedness );
}

void Rasteriser::ScanTriangle( const Vertexf& vertMin, const Vertexf& vertMid, const Vertexf& vertMax, bool isRightHanded )
{
    // create texcoords and edges
    TexCoordsForEdgef texcoords = TexCoordsForEdgef( vertMin, vertMid, vertMax );
    Edgef topToBottom    = Edgef( vertMin, vertMax, texcoords, 0 );
    Edgef topToMiddle    = Edgef( vertMin, vertMid, texcoords, 0 );
    Edgef middleToBottom = Edgef( vertMid, vertMax, texcoords, 1 );

    ScanEdges( topToBottom, topToMiddle, isRightHanded );
    ScanEdges( topToBottom, middleToBottom, isRightHanded );
}

void Rasteriser::ScanEdges( Edgef& a, Edgef& b, bool isRightHanded )
{
    // Scans triangle edges by iterating over each line.
    // Edges keep track of texcoords and slope

    Edgef& left  = isRightHanded || b.GetCurrentX() < a.GetCurrentX() ? b : a;
    Edgef& right = isRightHanded || b.GetCurrentX() < a.GetCurrentX() ? a : b;
//    Edgef& left  = isRightHanded ? b : a;
//    Edgef& right = isRightHanded ? a : b;

    Uint16 yStart = b.GetYStart();
    Uint16 yEnd   = b.GetYEnd();

    for ( Uint16 i = yStart; i < clipNumber( yEnd, yStart, (Uint16) w_window->Getheight() ); i++ )
    {
        DrawScanLine( left, right, i );
        left.DoYStep();
        right.DoYStep();
    }
}

void Rasteriser::DrawScanLine( const Edgef& left, const Edgef& right, Uint16 yCoord )
{
    // ceil xMin and xMax for compliance with our top-left fill convention
    int xMin = std::ceil( left.GetCurrentX() );
    int xMax = std::ceil( right.GetCurrentX() );

    // draw line while taking into account our texels and texture
    // texels have to be interpolated along the x-axis

    // calculate xPrestep and xDist for our texCoords
    float xPrestep = xMin - left.GetCurrentX();
    float xDist = right.GetCurrentX() - left.GetCurrentX();

    // now calculate xx, yx and zx steps
    float texCoordXX_step = ( right.GetCurrentTexCoordX() - left.GetCurrentTexCoordX() ) / xDist;
    float texCoordYX_step = ( right.GetCurrentTexCoordY() - left.GetCurrentTexCoordY() ) / xDist;
    float oneOverZX_step  = ( right.GetCurrentOneOverZ()  - left.GetCurrentOneOverZ()  ) / xDist;
    float depthX_step = ( right.GetCurrentDepth() - left.GetCurrentDepth() ) / xDist;

    // then find the texcoords we want to iterate over
    float current_texCoordX = left.GetCurrentTexCoordX() + texCoordXX_step * xPrestep;
    float current_texCoordY = left.GetCurrentTexCoordY() + texCoordYX_step * xPrestep;
    float current_oneOverZ  = left.GetCurrentOneOverZ()  + oneOverZX_step  * xPrestep;
    float current_depth = left.GetCurrentDepth() + depthX_step * xPrestep;

    // if xMin smaller than 0, add steps that are outside of screen area
    if ( xMin < 0 )
    {
        int distance = abs( 0 - xMin );
        current_texCoordX += texCoordXX_step * distance;
        current_texCoordY += texCoordYX_step * distance;
        current_oneOverZ  += oneOverZX_step  * distance;
        current_depth     += depthX_step     * distance;

        xMin = 0;
    }

    // loop through each x and draw pixel, clip ensures that pixels outside of screen are not iterated over
    for ( int x = clipNumber( xMin , 0, (int) w_window->Getwidth() ); x < clipNumber( xMax , xMin, (int) w_window->Getwidth() ); x++ )
    {
        if ( drawWithTexture )
        {
            float z = 1.0f / current_oneOverZ;

            // calculate texture coords
            Uint16 textureX = clipNumber< Uint16 >( std::ceil((current_texCoordX * z) * (current_texture->GetWidth()  - 1)),
                                                                                      0, current_texture->GetWidth()  - 1 );
            Uint16 textureY = clipNumber< Uint16 >( std::ceil((current_texCoordY * z) * (current_texture->GetHeight() - 1)),
                                                                                      0, current_texture->GetHeight() - 1 );

            DrawFragment( x, yCoord, current_depth, textureX, textureY );
        }
        else
        {
            DrawFragment( x, yCoord, current_depth );
        }

        // add steps
        current_texCoordX += texCoordXX_step;
        current_texCoordY += texCoordYX_step;
        current_oneOverZ  += oneOverZX_step;
        current_depth     += depthX_step;
    }

    if ( slowRendering && yCoord >= 0 && yCoord % 8 == 0 )
    {
        w_window->updateWindow();
    }
}

void Rasteriser::DrawFragment( Uint16 x, Uint16 y, float current_depth, Uint16 texcoordX, Uint16 texcoordY )
{
    if ( !drawWithTexture )
    {
        cout << "WARNING: Attempting to draw fragment with texcoords although draw with no texture was defined!" << endl;
    }

    // depth test
//    if ( ignoreZBuffer || ( current_depth < GetZ( (int) (x * y) ) && current_depth > GetNearZ() && current_depth < GetFarZ() ) )
    if ( ignoreZBuffer || current_depth < GetZ( x, y ) )
    {
//    if ( printDebug )
//    {
//        cout << "Pixel passed z-test with " << current_depth << ". Z-Buffer was " << GetZ( x * (int) yCoord ) << endl;
//    }
        SetZ( x, y, current_depth );
        w_window->drawPixel( x, y,
                             current_texture->GetPixel( texcoordX, texcoordY ) );
    }
}

void Rasteriser::DrawFragment( Uint16 x, Uint16 y, float current_depth )
{
    if ( drawWithTexture )
    {
        cout << "WARNING: Attempting to draw fragment without texcoords although drawWithTexture was set!" << endl;
    }

    // depth test
//    if ( ignoreZBuffer || ( current_depth < GetZ( x, y ) && current_depth > GetNearZ() && current_depth < GetFarZ() ) )
    if ( ignoreZBuffer || current_depth < GetZ( x, y ) )
    {
//        if ( printDebug )
//        {
//        cout << "Pixel passed z-test with " << current_depth << ". Z-Buffer was " << GetZ( x * (int) yCoord ) << endl;
//        }
        SetZ( x, y, current_depth );
        w_window->drawPixel( x, y, current_colour );
    }
}

Rasteriser::~Rasteriser()
{
    //dtor

    if ( printDebug )
    {
        cout << "Dtor of Rasteriser object was called!" << endl;
    }
}
