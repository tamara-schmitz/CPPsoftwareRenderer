#include "vertexprocessor.h"

VertexProcessor::VertexProcessor( shared_ptr< SafeQueue< VPIO > > in, shared_ptr< SafeDynArray< VPOO > > out,
                                  Matrix4f objMatrix, Matrix4f viewMatrix, Matrix4f perspMatrix, Matrix4f screenMatrix,
                                  Matrix4f objViewMatrix, Matrix4f perspScreenMatrix )
{
    this->objMatrix = objMatrix;
    this->viewMatrix = viewMatrix;
    this->perspMatrix = perspMatrix;
    this->screenMatrix = screenMatrix;
    this->objViewMatrix = objViewMatrix;
    this->perspScreenMatrix = perspScreenMatrix;
    this->output_vpoos = out;

    // fetch triangle and process until no more are left
    bool success;
    VPIO current_vpio;
    do
    {
        success = in->pop( current_vpio );
        if ( success )
        {
            ProcessTriangle( current_vpio );
        }
    } while ( success );
}

void VertexProcessor::ProcessTriangle( VPIO current_vpio )
{
    // From here on current coordinate space of tri will be
    // indicated through comments.
    // Possible spaces: object, world, view, perspective, screen

    // -- Object Space
    current_vpio.tri *= objViewMatrix;
    // -- View Space
    // TODO do clipping
    current_vpio.tri *= perspScreenMatrix;
    // -- Screen Space
    // TODO backface culling based on normal vector

    // calculate handedness
    float area = triangleArea< float >( current_vpio.tri.verts[0].posVec, current_vpio.tri.verts[1].posVec, current_vpio.tri.verts[2].posVec );
    // true if right handed (and hence area bigger than 0)
    bool handedness = area < 0;

    current_vpio.tri.sortVertsByY();

    if ( current_vpio.drawWithTexture )
    {
        output_vpoos->push_back( VPOO( current_vpio.tri.verts[0], current_vpio.tri.verts[1], current_vpio.tri.verts[2], handedness, current_vpio.texture ) );
    }
    else
    {
        output_vpoos->push_back( VPOO( current_vpio.tri.verts[0], current_vpio.tri.verts[1], current_vpio.tri.verts[2], handedness, current_vpio.colour ) );
    }
}
