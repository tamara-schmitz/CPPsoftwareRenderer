#include "rasteriser.h"

Rasteriser::Rasteriser( Window* window )
{
    //ctor
    w_window = window;

    // Init with default values
    UpdateScreenspaceTransformMatrix();
    SetDrawColour( SDL_Color{ 200, 200, 200, SDL_ALPHA_OPAQUE } );
}

void Rasteriser::SetDrawColour( const SDL_Color& color )
{
    drawWithTexture = false;

    current_colour = color;

    current_texture_width = 0;
    current_texture_height = 0;
}

void Rasteriser::SetDrawTexture( shared_ptr<Texture> texture )
{
    drawWithTexture = true;

    current_texture = texture;

    current_texture_width  = current_texture->GetWidth();
    current_texture_height = current_texture->GetHeight();
}

void Rasteriser::UpdateViewMatrix( const Matrix4f& viewMatrix )
{
    viewSpaceTransformMatrix = viewMatrix;
    UpdateVpsMatrix();
}
void Rasteriser::UpdatePerspectiveMatrix( const Matrix4f& perspectiveMatrix )
{
    perspectiveTransformMatrix = perspectiveMatrix;
    UpdateVpsMatrix();
}
void Rasteriser::UpdatePerspectiveMatrix( const float& fov, const float& zNear, const float& zFar )
{
    perspectiveTransformMatrix = Matrix4f::perspectiveTransform( fov, w_window->Getwidth() / w_window->Getheight(), zNear, zFar );
    UpdateVpsMatrix();
}
void Rasteriser::UpdateScreenspaceTransformMatrix()
{
    screenspaceTransformMatrix = Matrix4f::screenspaceTransform( w_window->Getwidth() / 2.0f, w_window->Getheight() / 2.0f );
    UpdateVpsMatrix();
}
void Rasteriser::UpdateViewAndPerspectiveMatrix( const Matrix4f& viewMatrix, const Matrix4f& perspectiveMatrix )
{
    viewSpaceTransformMatrix = viewMatrix;
    perspectiveTransformMatrix = perspectiveMatrix;
    UpdateVpsMatrix();
}
void Rasteriser::UpdateViewAndPerspectiveMatrix( const Matrix4f& viewMatrix, const float& fov, const float& zNear, const float& zFar )
{
    viewSpaceTransformMatrix = viewMatrix;
    perspectiveTransformMatrix = Matrix4f::perspectiveTransform( fov, w_window->Getwidth() / w_window->Getheight(), zNear, zFar );
    UpdateVpsMatrix();
}

void Rasteriser::DrawMesh( shared_ptr<Mesh> mesh, const Matrix4f& objToWorld )
{
    // iterate over each triangle
    for ( Uint32 i = 0; i < mesh->GetIndicesCount(); i += 3 )
    {
        // get vertices
        Vertexf v1 = mesh->GetVertex( mesh->GetIndex( i ) );
        Vertexf v2 = mesh->GetVertex( mesh->GetIndex( i + 1 ) );
        Vertexf v3 = mesh->GetVertex( mesh->GetIndex( i + 2 ) );
        // apply objToWorld transform
        v1.posVec = objToWorld * v1.posVec;
        v2.posVec = objToWorld * v2.posVec;
        v3.posVec = objToWorld * v3.posVec;
        // fill triangle
        FillTriangle( v1, v2, v3 );
    }
}

void Rasteriser::FillTriangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 )
{
    // set up alias for input vectors
//    Vector3f yMinVert = ( vpMatrix * v1 ).screenspaceVec3( w_window->Getwidth(), w_window->Getheight() );
//    Vector3f yMidVert = ( vpMatrix * v2 ).screenspaceVec3( w_window->Getwidth(), w_window->Getheight() );
//    Vector3f yMaxVert = ( vpMatrix * v3 ).screenspaceVec3( w_window->Getwidth(), w_window->Getheight() );

    // copy vertexes
    Vertexf yMinVert = v1;
    Vertexf yMidVert = v2;
    Vertexf yMaxVert = v3;

    // apply transformation matrix
    yMinVert.posVec = ( vpsMatrix * yMinVert.posVec );
    yMidVert.posVec = ( vpsMatrix * yMidVert.posVec );
    yMaxVert.posVec = ( vpsMatrix * yMaxVert.posVec );

    // divide posVec by W (this is the perspective divison)
    yMinVert.posVec.divideByWOnly();
    yMidVert.posVec.divideByWOnly();
    yMaxVert.posVec.divideByWOnly();

    // calculate handedness
    float area = triangleArea< float >( yMinVert.posVec, yMaxVert.posVec, yMidVert.posVec );
    // true if right handed (and hence area smaller than 0)
    bool handedness = area >= 0;

    // cull triangle if right-handed (we use left-handed cartesian coordinates)
    if ( handedness )
    {
        return;
    }

    #ifdef PRINT_DEBUG_STUFF
            cout << "yMinVert - x: " << yMinVert.posVec.x << " y: " << yMidVert.posVec.y << " z: " << yMaxVert.posVec.z << endl;
    #endif // PRINT_DEBUG_STUFF

    // sort verts
    if ( yMaxVert.posVec.y < yMidVert.posVec.y )
    {
        std::swap( yMidVert, yMaxVert );
    }
    if ( yMidVert.posVec.y < yMinVert.posVec.y )
    {
        std::swap( yMinVert, yMidVert );
    }
    if ( yMaxVert.posVec.y < yMidVert.posVec.y )
    {
        std::swap( yMidVert, yMaxVert );
    }



    ScanTriangle( yMinVert, yMidVert, yMaxVert, handedness );
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
    // assign left and right based on $isRightHanded
    Edgef& left  = isRightHanded ? b : a;
    Edgef& right = isRightHanded ? a : b;

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

    // are we supposed to draw with one solid colour or with a texture?
    if ( drawWithTexture == false || current_texture == nullptr )
    {
        // draw line with solid colour

        // clip is used so that loop only runs within screen boundaries
        for ( int x = clipNumber( xMin , 0, (int) w_window->Getwidth() ); x < clipNumber( xMax , xMin, (int) w_window->Getwidth() ); x++ )
        {
            w_window->drawPixel( x, yCoord, current_colour );
        }
    }
    else
    {
        // draw line while taking into account our texels and texture
        // texels have to be interpolated along the x-axis

        // calculate xPrestep and xDist for our texCoords
        float xPrestep = xMin - left.GetCurrentX();
        float xDist = right.GetCurrentX() - left.GetCurrentX();

        // now calculate xx, yx and zx steps
        float texCoordXX_step = ( right.GetCurrentTexCoordX() - left.GetCurrentTexCoordX() ) / xDist;
        float texCoordYX_step = ( right.GetCurrentTexCoordY() - left.GetCurrentTexCoordY() ) / xDist;
        float oneOverZX_step =  ( right.GetCurrentOneOverZ()  - left.GetCurrentOneOverZ()  ) / xDist;

        // then find the texcoords we want to iterate over
        float current_texCoordX = left.GetCurrentTexCoordX() + texCoordXX_step * xPrestep;
        float current_texCoordY = left.GetCurrentTexCoordY() + texCoordYX_step * xPrestep;
        float current_oneOverZ  = left.GetCurrentOneOverZ()  + oneOverZX_step  * xPrestep;

        // if xMin smaller than 0, add steps that are outside of screen area
        if ( xMin < 0 )
        {
            int distance = abs(0 - xMin);

            current_texCoordX += texCoordXX_step * distance;
            current_texCoordY += texCoordYX_step * distance;
            current_oneOverZ  += oneOverZX_step  * distance;
        }

        // loop through each x and draw pixel, clip ensures that pixels outside of screen are not iterated over
        for ( int x = clipNumber( xMin , 0, (int) w_window->Getwidth() ); x < clipNumber( xMax , xMin, (int) w_window->Getwidth() ); x++ )
        {
            float z = 1.0f / current_oneOverZ;

            // calculate coords in texture
            Uint16 textureX = clipNumber< Uint16 >( std::ceil((current_texCoordX * z) * (current_texture_width  - 1)),
                                                    0, current_texture_width  - 1 );
            Uint16 textureY = clipNumber< Uint16 >( std::ceil((current_texCoordY * z) * (current_texture_height - 1)),
                                                    0, current_texture_height - 1);

            // add steps
            current_texCoordX += texCoordXX_step;
            current_texCoordY += texCoordYX_step;
            current_oneOverZ  += oneOverZX_step;

            // draw pixel
            w_window->drawPixel( x, yCoord,
                        current_texture->GetPixel( textureX, textureY ) );
        }

    }
}

Rasteriser::~Rasteriser()
{
    //dtor

    #ifdef PRINT_DEBUG_STUFF
        cout << "Dtor of Rasteriser object was called!" << endl;
    #endif // PRINT_DEBUG_STUFF
}
