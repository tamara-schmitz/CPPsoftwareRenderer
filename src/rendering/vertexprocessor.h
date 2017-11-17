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

        shared_ptr< std::thread > ProcessQueueAsThread() { return make_shared< std::thread >( &VertexProcessor::ProcessQueue, this ); }
        void ProcessQueue();
        Uint32 GetProcessedVPIOsCount() const { return processedVPIOs_count; }

        Matrix4f perspScreenMatrix;
        Matrix4f viewMatrix, perspMatrix, screenMatrix;

    private:
        shared_ptr< SafeQueue< VPIO > > in_vpios;
        shared_ptr< SafeDynArray< VPOO > > output_vpoos;

        Uint32 processedVPIOs_count = 0;
        void ProcessTriangle( VPIO& current_vpio );
        void ClipTriangle( const VPIO& current_vpio, std::vector< Vertexf >& result_vertices );
        void ClipPolygonAxis( std::vector<Vertexf>& vertices, uint_fast8_t componentIndex );
        void ClipPolygonComponent( const std::vector<Vertexf>& vertices, uint_fast8_t componentIndex, float componentFactor, std::vector<Vertexf>& result );
};

#endif // VERTEXPROCESSOR_H
