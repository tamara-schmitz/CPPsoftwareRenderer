#ifndef VERTEXPROCESSOR_H
#define VERTEXPROCESSOR_H

#include "common.h"
#include "types/Vertex.h"
#include "types/Triangle.h"
#include "types/VertexProcessorObjs.h"
#include "types/SafeQueue.h"
#include "types/SafeDynArray.h"

class VertexProcessor
{
    public:
        VertexProcessor( shared_ptr< SafeQueue< VPIO > > in, shared_ptr< SafeDynArray< VPOO > > out );
        virtual ~VertexProcessor();

        std::thread ProcessQueueAsThread() { return std::thread( &VertexProcessor::ProcessQueue, this ); }
        void ProcessQueue();

        Matrix4f perspScreenMatrix;
        Matrix4f viewMatrix, perspMatrix, screenMatrix;

    private:
        shared_ptr< SafeQueue< VPIO > > in_vpios;
        shared_ptr< SafeDynArray< VPOO > > output_vpoos;


        void ProcessTriangle( VPIO& current_vpio );
        void ClipTriangle( const VPIO& current_vpio, std::vector< Vertexf >& result_vertices );
        void ClipPolygonAxis( std::vector<Vertexf>& vertices, int_fast8_t componentIndex );
        void ClipPolygonComponent( const std::vector<Vertexf>& vertices, int_fast8_t componentIndex, float componentFactor, std::vector<Vertexf>& result );
        //std::array< Vertexf, 3 > toModelViewTranslation ( Vertexf vert1, Vertexf vert2, Vertexf vert3 );
};

#endif // VERTEXPROCESSOR_H
