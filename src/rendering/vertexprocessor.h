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

        std::thread ProcessQueueAsThread() { return std::thread( &VertexProcessor::ProcessQueue, this ); }
        void ProcessQueue();

        Matrix4f perspScreenMatrix;
        Matrix4f viewMatrix, perspMatrix, screenMatrix;

    private:
        shared_ptr< SafeQueue< VPIO > > in_vpios;
        shared_ptr< SafeDynArray< VPOO > > output_vpoos;


        void ProcessTriangle( VPIO current_vpio );
        std::array< Vertexf, 3 > toModelViewTranslation ( Vertexf vert1, Vertexf vert2, Vertexf vert3 );
};

#endif // VERTEXPROCESSOR_H
