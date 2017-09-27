#include "rasteriser.h"

Rasteriser::Rasteriser( Window* window, shared_ptr< std::vector< float > > z_buffer, const Uint16& y_begin, Uint16& y_end,
                        const float& near_z, const float& far_z  )
{
    //ctor
    this->w_window = window;
    this->z_buffer = z_buffer;
    this->y_begin = y_begin;
    this->y_end   = y_end;
    this->near_z = near_z;
    this->far_z  = far_z;

    // Init with default values
    SetDrawColour( SDL_Color{ 200, 200, 200, SDL_ALPHA_OPAQUE } );
}

float Rasteriser::GetZ( const Uint32& index ) const
{
    if ( index < y_begin * w_window->Getwidth() || index > y_end * w_window->Getwidth() )
    {
        throw std::out_of_range( "Illegal z_buffer read in Rasteriser." );
    }
    return z_buffer->at( index );
}

void Rasteriser::SetZ( const Uint32& index, const float& z_value )
{
    if ( index < y_begin * w_window->Getwidth() || index > y_end * w_window->Getwidth() )
    {
        throw std::out_of_range( "Illegal z_buffer write in Rasteriser." );
    }
    z_buffer->at( index ) = z_value;
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

    Uint16 yEdge_Start = b.GetYStart();
    Uint16 yEdge_End   = b.GetYEnd();

    for ( Uint16 i = yEdge_Start; i < clipNumber( yEdge_End, yEdge_Start, y_end ); i++ )
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
//        if ( printDebug )
//        {
//            cout << "Pixel passed z-test with " << current_depth << ". Z-Buffer was " << GetZ( x * (int) yCoord ) << endl;
//        }
        if ( y < y_begin || y > y_end )
        {
            throw std::out_of_range( "Illegal pixel write in Rasteriser." );
        }
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
        if ( y < y_begin || y > y_end )
        {
            throw std::out_of_range( "Illegal pixel write in Rasteriser." );
        }
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
