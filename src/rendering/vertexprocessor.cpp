#include "vertexprocessor.h"

VertexProcessor::VertexProcessor( shared_ptr< SafeQueue< VPIO > > in, shared_ptr< SafeDynArray< VPOO > > out )
{
    this->in_vpios = in;
    this->output_vpoos = out;
}

void VertexProcessor::ProcessQueue()
{
    processedVPIOs_count = 0;
    bool queue_not_empty;
    VPIO current_vpio;
    do
    {
        queue_not_empty = in_vpios->pop( current_vpio );
        if ( queue_not_empty )
	{
            ProcessTriangle( current_vpio );
            processedVPIOs_count++;
	}

    } while ( queue_not_empty );
}

__attribute__((target_clones("avx2","arch=westmere","default")))
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
    current_vpio.tri *= perspMatrix;
    // -- Perspective Space
    std::vector< Vertexf > tri_vertices = { current_vpio.tri.verts[0], current_vpio.tri.verts[1], current_vpio.tri.verts[2] };
   
    // cull triangle earlier if all posVec.w are outside of frustum
    bool cull_early = true;
    for ( uint_fast8_t i = 0; i < tri_vertices.size(); i++ )
    {
        if ( abs( tri_vertices.at( i ).posVec.x ) <= abs( tri_vertices.at( i ).posVec.w ) &&
             abs( tri_vertices.at( i ).posVec.y ) <= abs( tri_vertices.at( i ).posVec.w ) &&
             abs( tri_vertices.at( i ).posVec.z ) <= abs( tri_vertices.at( i ).posVec.w ) )
        {
            cull_early = false;
            break;
        }
    }
    if ( cull_early )
    {
        if ( printDebug )
            cout << "Tri was culled before vp clipping." << endl;
        return;
    }
    
    bool clipping_required = false;
    if ( Do_VP_Clipping )
    {
        // this check ensures that triangles inside the frustum get passed through directly
        for ( uint_fast8_t i = 0; i < tri_vertices.size(); i++ )
        {
            if ( abs( tri_vertices.at( i ).posVec.x ) > abs( tri_vertices.at( i ).posVec.w ) ||
                 abs( tri_vertices.at( i ).posVec.y ) > abs( tri_vertices.at( i ).posVec.w ) ||
                 abs( tri_vertices.at( i ).posVec.z ) > abs( tri_vertices.at( i ).posVec.w ) )
            {
                clipping_required = true;
                break;
            }
        }

        if ( clipping_required )
            ClipTriangle( current_vpio, tri_vertices );
    }

    if ( tri_vertices.size() <= 0 )
        return;

    // prepare verts for rasterisation
    for ( uint_fast8_t i = 0; i < tri_vertices.size(); i++ )
    {
        tri_vertices.at( i ).posVec = screenMatrix * tri_vertices.at( i ).posVec;
        // -- Screen Space
        tri_vertices.at( i ).posVec.divideByWOnly();
    }

   // It is possible that we end up with more than 3 vertices after clipping. so we have to create more than 1 triangle. we can create these new triangles
   // by assuming that all triangles share at least one vertices.
   for ( uint_fast8_t i = 0; i <= tri_vertices.size() - 3; i++ )
    {
        // TODO backface culling based on normal vector

        // calculate handedness
        float area = triangleArea< float >( tri_vertices.at( 0 ).posVec, tri_vertices.at( i + 1 ).posVec, tri_vertices.at( i + 2 ).posVec );
        // true if right handed (and hence area bigger than 0)
        bool handedness = area < 0;

        /*
        // cull tiny triangle that probably wont affect the final result
        if ( abs( area ) < 0.1 )
            continue;
        */

        if ( current_vpio.drawWithTexture )
        {
            output_vpoos->push_back( VPOO( tri_vertices.at( 0 ), tri_vertices.at( i + 1 ), tri_vertices.at( i + 2 ),
                                           handedness, current_vpio.texture ) );
        }
        else
        {
            output_vpoos->push_back( VPOO( tri_vertices.at( 0 ), tri_vertices.at( i + 1 ), tri_vertices.at( i + 2 ),
                                           handedness, current_vpio.colour ) );
        }
    }
}

void VertexProcessor::ClipTriangle( const VPIO& current_vpio, std::vector< Vertexf >& result_vertices )
{
    ClipPolygonAxis( result_vertices, 0);
    ClipPolygonAxis( result_vertices, 1);
    ClipPolygonAxis( result_vertices, 2);
}

void VertexProcessor::ClipPolygonAxis( std::vector<Vertexf>& vertices, uint_fast8_t componentIndex )
{
    // clips all vertices of a certain axis. results overwrite existing vertices
    std::vector<Vertexf> result_temp; // like vertices has to be passed by reference
    result_temp.clear();

    // clip against w=1
    ClipPolygonComponent( vertices, componentIndex, 1.0f, result_temp );
    vertices.clear();

    // clip against w=-1
    ClipPolygonComponent( result_temp, componentIndex, -1.0f, vertices );
    result_temp.clear();
}

__attribute__((target_clones("avx2","arch=westmere","default")))
void VertexProcessor::ClipPolygonComponent( const std::vector<Vertexf>& vertices, uint_fast8_t componentIndex, float componentFactor, std::vector<Vertexf>& result )
{
    // iterate over each vertex and do one dimensional lerping

    if ( vertices.size() <= 0 )
    {
        if ( printDebug )
            cout << "There were no verts left for component " << (int) componentIndex << "!" << endl;

        return;
    }

    // for the initial component comparison we just take the last one in the list
    uint_fast8_t previousVertex = vertices.size() - 1;
    float previousComponent = vertices.at( previousVertex ).GetPosVecComponent( componentIndex ) * componentFactor;
    bool previousInside = previousComponent <= vertices.at( previousVertex ).posVec.w;

    for ( uint_fast8_t i = 0; i < vertices.size(); i++ )
    {
        float currentComponent = vertices.at( i ).GetPosVecComponent( componentIndex ) * componentFactor;

        // currentComponent gets inverted if componentFactor is negativ. Hence only <= is required.
        bool currentInside = currentComponent <= vertices.at( i ).posVec.w;

        /* 
        if ( printDebug && componentIndex > 0 )
            cout << "Vertex with index " << (int) i << " for component " << (int) componentIndex
                 << " has w of " << vertices.at( i ).posVec.w << " compared to currentComponent " << currentComponent 
                 << ". Hence inside: " << currentInside << endl;
        */

        // we only need to clip if the vert was inside and the last vert was outside of the frustrum
        // (or the over way around)
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
