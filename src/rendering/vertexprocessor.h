#ifndef VERTEXPROCESSOR_H
#define VERTEXPROCESSOR_H

#include "common.h"
#include "types/Vertex.h"
#include "types/Triangle.h"
#include "types/VertexProcessorObj.h"
#include "types/SafeQueue.h"
#include "types/SafeDynArray.h"

class VertexProcessor
{
    public:
        VertexProcessor(SafeQueue< Triangle >* in, SafeDynArray< VertexProcessorObj >* out,
                         Matrix4f objMatrix, Matrix4f viewMatrix, Matrix4f perspMatrix, Matrix4f screenMatrix,
                         Matrix4f objViewMatrix, Matrix4f perspScreenMatrix );

    private:
        SafeDynArray< VertexProcessorObj >* output_vpos;

        Matrix4f objMatrix, viewMatrix, perspMatrix, screenMatrix;
        Matrix4f objViewMatrix, perspScreenMatrix;

        void ProcessTriangle( Triangle tri );
        std::array< Vertexf, 3 > toModelViewTranslation ( Vertexf vert1, Vertexf vert2, Vertexf vert3 );
};

#endif // VERTEXPROCESSOR_H
