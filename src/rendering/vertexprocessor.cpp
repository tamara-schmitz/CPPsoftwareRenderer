#include "vertexprocessor.h"

VertexProcessor::VertexProcessor( shared_ptr< SafeQueue< VPIO > > in, shared_ptr< SafeDynArray< VPOO > > out )
{
    this->in_vpios = in;
    this->output_vpoos = out;
}

void VertexProcessor::ProcessQueue()
{
    bool queue_not_empty;
    VPIO current_vpio;
    do
    {
        queue_not_empty = in_vpios->pop( current_vpio );
        if ( queue_not_empty )
        {
            ProcessTriangle( current_vpio );
        }
    } while ( queue_not_empty );
}

void VertexProcessor::ProcessTriangle( VPIO current_vpio )
{
    // From here on current coordinate space of tri will be
    // indicated through comments.
    // Possible spaces: object, world, view, perspective, screen

    // -- Object Space
    current_vpio.tri *= *(current_vpio.objMatrix);
    // -- World Space
    current_vpio.tri *= viewMatrix;
    // -- View Space
    // TODO do clipping
    current_vpio.tri *= perspScreenMatrix;
    // -- Screen Space
    current_vpio.tri.verts[0].posVec.divideByWOnly();
    current_vpio.tri.verts[1].posVec.divideByWOnly();
    current_vpio.tri.verts[2].posVec.divideByWOnly();

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
