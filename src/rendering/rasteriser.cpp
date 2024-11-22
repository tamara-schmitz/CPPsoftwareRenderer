#include "rasteriser.h"

Rasteriser::Rasteriser( shared_ptr< SafeDeque< VPOO > > in, const Uint16& frame_width, const Uint16& frame_height , const Uint16& y_begin, const Uint16& y_end )
{
    //ctor
    this->in_vpoos = in;
    this->y_begin = y_begin;
    this->y_end   = y_end;
    this->frame_width = frame_width;
    this->frame_height = frame_height;
    this->r_texture = make_shared< Texture >(frame_width, y_end - y_begin);

    // init z_buffer
    size_t zsize = r_texture->GetWidth() * r_texture->GetHeight();
    z_buffer.resize( zsize );
    z_buffer.shrink_to_fit();
}

void Rasteriser::initFramebuffer()
{
    // clearing
    std::fill( z_buffer.begin(), z_buffer.end(), std::numeric_limits< float >::max() );
    //r_texture->clear();
    r_texture->FillWithRandomColour();
}

void Rasteriser::finaliseFrame()
{
    // TODO
}

void Rasteriser::ProcessVPOOArray()
{
    initFramebuffer();

    for ( int i = 0; !in_vpoos->isLastBlocked( i ); i++ )
    {
        current_vpoo = in_vpoos->at(i);
        ProcessCurrentVPOO();
    }

    finaliseFrame();
}

inline float Rasteriser::GetZ( const Uint32& index ) const
{
    return z_buffer.at(index);
}

inline void Rasteriser::SetZ( const Uint32& index, const float& z_value )
{
    z_buffer.at(index) = z_value;
}


void Rasteriser::ProcessCurrentVPOO()
{
    const Vertexf& vertMin = current_vpoo.tris_verts[0];
    const Vertexf& vertMid = current_vpoo.tris_verts[1];
    const Vertexf& vertMax = current_vpoo.tris_verts[2];

    // create texcoords and edges
    TexCoordsForEdgef texcoords = TexCoordsForEdgef( vertMin, vertMid, vertMax );
    Edgef topToBottom    = Edgef( vertMin, vertMax, texcoords, 0 );
    Edgef topToMiddle    = Edgef( vertMin, vertMid, texcoords, 0 );
    Edgef middleToBottom = Edgef( vertMid, vertMax, texcoords, 1 );

    ScanEdges( topToBottom, topToMiddle, current_vpoo.isRightHanded );
    ScanEdges( topToBottom, middleToBottom, current_vpoo.isRightHanded );
}

void Rasteriser::ScanEdges( Edgef& a, Edgef& b, bool isRightHanded )
{
    // Scans triangle edges by iterating over each line.
    // Edges keep track of texcoords and slope

    bool leftEdgeIsB = isRightHanded || b.GetCurrentX() < a.GetCurrentX();
    Edgef& left  = leftEdgeIsB ? b : a;
    Edgef& right = leftEdgeIsB ? a : b;
//    Edgef& left  = isRightHanded ? b : a;
//    Edgef& right = isRightHanded ? a : b;

    Uint16 yEdge_Start = b.GetYStart();
    Uint16 yEdge_End   = b.GetYEnd();

    for ( Uint16 i = yEdge_Start; i < clipNumber( yEdge_End, yEdge_Start, y_end ); i++ )
    {
        if ( i >= y_begin )
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

    // stop here if both x are exactly the same
    if ( xDist == 0 ) [[unlikely]]
    {
        return;
    }

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
    for ( int x = clipNumber( xMin , 0, (int) r_texture->GetWidth() ); x < clipNumber( xMax , xMin, (int) r_texture->GetWidth() ); x++ )
    {
        if ( current_vpoo.texture != nullptr )
        {
            float z = 1.0f / current_oneOverZ;

            // calculate texture coords
            Uint16 textureX = clipNumber< Uint16 >( std::ceil((current_texCoordX * z) * (current_vpoo.texture->GetWidth()  - 1) + 0.5f ),
                                                                                      0, current_vpoo.texture->GetWidth()  - 1  + 0.5f );
            Uint16 textureY = clipNumber< Uint16 >( std::ceil((current_texCoordY * z) * (current_vpoo.texture->GetHeight() - 1) + 0.5f ),
                                                                                      0, current_vpoo.texture->GetHeight() - 1  + 0.5f );

            DrawFragment( x, yCoord - y_begin, current_depth, textureX, textureY );
        }
        else
        {
            DrawFragment( x, yCoord - y_begin, current_depth );
        }

        // add steps
        current_texCoordX += texCoordXX_step;
        current_texCoordY += texCoordYX_step;
        current_oneOverZ  += oneOverZX_step;
        current_depth     += depthX_step;
    }
}

inline void Rasteriser::DrawFragment( Uint16 x, Uint16 y, float current_depth, Uint16 texcoordX, Uint16 texcoordY )
{
    // depth test
//    if ( ignoreZBuffer || ( current_depth <= GetZ( x * y ) && current_depth >= near_z && current_depth <= far_z ) )
    if ( ignoreZBuffer || current_depth <= GetZ( x, y ) )
    {
        /*
        if ( printDebug ) [[unlikely]]
        {
            cout << "Pixel passed z-test with " << current_depth << ". Z-Buffer was " << GetZ( x * (int) y ) << endl;
        }
        */
        SetZ( x, y, current_depth );
        int offset = y * r_texture->GetWidth() + x;
        r_texture->t_pixels.at( offset ) = current_vpoo.texture->t_pixels.at( 
                                                    current_vpoo.texture->GetWidth() * texcoordY + texcoordX );
    }
}

inline void Rasteriser::DrawFragment( Uint16 x, Uint16 y, float current_depth )
{
    // depth test
//    if ( ignoreZBuffer || ( current_depth <= GetZ( x, y ) && current_depth >= near_z && current_depth <= far_z ) )
    if ( ignoreZBuffer || current_depth <= GetZ( x, y ) )
    {
        /*
        if ( printDebug ) [[unlikely]]
        {
            cout << "Pixel passed z-test with " << current_depth << ". Z-Buffer was " << GetZ( x * (int) y ) << endl;
        }
        */
        SetZ( x, y, current_depth );
        int offset = y * r_texture->GetWidth() + x;
        r_texture->t_pixels.at( offset ) = getPixelFor_SDLColor( &current_vpoo.colour );
    }
}

Rasteriser::~Rasteriser()
{
    //dtor

    //SDL_FreeSurface( framebuffer.get() );

    if ( printDebug ) [[unlikely]]
    {
        cout << "Dtor of Rasteriser object was called!" << endl;
    }
}
