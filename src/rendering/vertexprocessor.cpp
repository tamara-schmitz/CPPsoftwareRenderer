#include "vertexprocessor.h"

VertexProcessor::VertexProcessor( shared_ptr< SafeQueue< VPIO > > in, shared_ptr< SafeDynArray< VPOO > > out )
{
    this->in_vpios = in;
    this->output_vpoos = out;
}

void VertexProcessor::ProcessQueue()
{
    bool queue_not_empty;
    VPIO current_vpio = VPIO();
    do
    {
        queue_not_empty = in_vpios->pop( current_vpio );
        if ( queue_not_empty )
        {
            ProcessTriangle( current_vpio );
        }
    } while ( queue_not_empty );
}

void VertexProcessor::ProcessTriangle( VPIO& current_vpio )
{
    // From here on current coordinate space of tri will be
    // indicated through comments.
    // Possible spaces: object, world, view, perspective, screen

    if ( current_vpio.isEmpty )
        return;

    // -- Object Space
    current_vpio.tri *= *(current_vpio.objMatrix);
    // -- World Space
    current_vpio.tri *= viewMatrix;

    // -- View Space
    std::vector< Vertexf > tri_vertices = { current_vpio.tri.verts[0], current_vpio.tri.verts[1], current_vpio.tri.verts[2] };
    ClipTriangle( current_vpio, tri_vertices );

    for ( uint_fast8_t i = 0; i < tri_vertices.size(); i = i + 3 )
    {
        tri_vertices.at( i + 0 ).posVec = perspScreenMatrix * tri_vertices.at( i + 0 ).posVec ;
        tri_vertices.at( i + 1 ).posVec = perspScreenMatrix * tri_vertices.at( i + 1 ).posVec ;
        tri_vertices.at( i + 2 ).posVec = perspScreenMatrix * tri_vertices.at( i + 2 ).posVec ;
        // -- Screen Space
        tri_vertices.at( i + 0 ).posVec.divideByWOnly();
        tri_vertices.at( i + 1 ).posVec.divideByWOnly();
        tri_vertices.at( i + 2 ).posVec.divideByWOnly();

        // TODO backface culling based on normal vector

        // calculate handedness
        float area = triangleArea< float >( tri_vertices.at( i + 0 ).posVec, tri_vertices.at( i + 1 ).posVec, tri_vertices.at( i + 2 ).posVec );
        // true if right handed (and hence area bigger than 0)
        bool handedness = area < 0;

        if ( current_vpio.drawWithTexture )
        {
            output_vpoos->push_back( VPOO( tri_vertices.at( i + 0 ), tri_vertices.at( i + 1 ), tri_vertices.at( i + 2 ),
                                           handedness, current_vpio.texture ) );
        }
        else
        {
            output_vpoos->push_back( VPOO( tri_vertices.at( i + 0 ), tri_vertices.at( i + 1 ), tri_vertices.at( i + 2 ),
                                           handedness, current_vpio.colour ) );
        }

    }
}

void VertexProcessor::ClipTriangle( const VPIO& current_vpio, std::vector< Vertexf >& result_vertices )
{
    ClipPolygonAxis( result_vertices, 0);
    ClipPolygonAxis( result_vertices, 1);
    ClipPolygonAxis( result_vertices, 2);

    assert( result_vertices.size() % 3 == 0 );
}

void VertexProcessor::ClipPolygonAxis( std::vector<Vertexf>& vertices, int_fast8_t componentIndex )
{
    // clips all vertices of a certain axis. results overwrite existing vertices
    std::vector<Vertexf> result_temp;

    // clip against w=1
    ClipPolygonComponent( vertices, componentIndex, 1.0f, result_temp );
    vertices.clear();

    // clip against w=-1
    ClipPolygonComponent( result_temp, componentIndex, -1.0f, vertices );
    result_temp.clear();
}

void VertexProcessor::ClipPolygonComponent( const std::vector<Vertexf>& vertices, int_fast8_t componentIndex, float componentFactor, std::vector<Vertexf>& result )
{
    // iterates over each vertex and do one dimensional lerping

    if ( vertices.size() <= 0 )
        return;

    // for the first component comparison we just take the last one in the list
    int previousVertex = vertices.size() - 1;
    float previousComponent = vertices.at( previousVertex ).GetPosVecComponent( componentIndex ) * componentFactor;
    bool previousInside = previousComponent <= vertices.at( previousVertex ).posVec.w;

    for ( uint_fast8_t i = 0; i < vertices.size(); i++ )
    {
        float currentComponent = vertices.at( i ).GetPosVecComponent( componentIndex ) * componentFactor;
        bool currentInside = currentComponent <= vertices.at( i ).posVec.w;

        if ( printDebug && componentIndex == 2 )
            cout << "vertex with index " << (int) i << " for component " << (int) componentIndex
                 << " has w " << vertices.at( i ).posVec.w << " compared to currentComponent " << currentComponent << endl;

        // we only need to clip if last polys are both inside and outside at the same time
        if ( currentInside ^ previousInside )
        {
            float lerp = ( vertices.at( previousVertex ).posVec.w - previousComponent ) /
                         ( ( vertices.at( previousVertex ).posVec.w - previousComponent ) -
                           ( vertices.at( i ).posVec.w - currentComponent ) );
            result.push_back( vertices.at( previousVertex ).lerp_new( vertices.at( i ), lerp ) );
        }

        if ( currentInside )
            result.push_back( vertices.at( i ) );

        previousVertex = i;
        previousComponent = currentComponent;
        previousInside = currentInside;
    }
}

VertexProcessor::~VertexProcessor()
{
    //dtor

    if ( printDebug )
    {
        cout << "Dtor of VertexProcessor object was called!" << endl;
    }
}
