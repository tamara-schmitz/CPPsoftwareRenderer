#include "vertexprocessor.h"

VertexProcessor::VertexProcessor(SafeQueue< Triangle >* in, SafeDynArray< VertexProcessorObj >* out ,
                                 Matrix4f objMatrix, Matrix4f viewMatrix, Matrix4f perspMatrix, Matrix4f screenMatrix,
                                 Matrix4f objViewMatrix, Matrix4f perspScreenMatrix)
{
    this->objMatrix = objMatrix;
    this->viewMatrix = viewMatrix;
    this->perspMatrix = perspMatrix;
    this->screenMatrix = screenMatrix;
    this->objViewMatrix = objViewMatrix;
    this->perspScreenMatrix = perspScreenMatrix;
    this->output_vpos = out;

    // fetch triangle and process until no more are left
    bool success;
    Triangle current_tri;
    do
    {
        success = in->pop( current_tri );
        if ( success )
        {
            ProcessTriangle( current_tri );
        }
    } while ( success );
}

void VertexProcessor::ProcessTriangle( Triangle tri )
{
    // From here on current coordinate space of tri will be
    // indicated through comments.
    // Possible spaces: object, world, view, perspective, screen

    // -- Object Space
    tri *= objViewMatrix;
    // -- View Space
    // TODO do clipping
    tri *= perspScreenMatrix;
    // -- Screen Space
    // TODO backface culling based on normal vector

    // calculate handedness
    float area = triangleArea< float >( tri.verts[0].posVec, tri.verts[1].posVec, tri.verts[2].posVec );
    // true if right handed (and hence area bigger than 0)
    bool handedness = area < 0;

    tri.sortVertsByY();
    output_vpos->push_back( VertexProcessorObj( tri.verts[0], tri.verts[1], tri.verts[2], handedness ) );
}
