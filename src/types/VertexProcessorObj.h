#ifndef VERTEXPROCESSOROBJ_H
#define VERTEXPROCESSOROBJ_H

#include "types/Vertex.h"

struct VertexProcessorObj
{
    // Stores resulting vertices from vertexprocessor as well as
    // triangle handedness.
    // Vertices are expected to always be sorted by their posVec.y component.

    Vertexf tris_verts[3];
    bool isRightHanded;

    // ctors
    VertexProcessorObj()
    {
        tris_verts[0] = tris_verts[1] = tris_verts[2] = Vertexf();
        isRightHanded = false;
    }
    VertexProcessorObj( const Vertexf& vertMin, const Vertexf& vertMid,
                        const Vertexf& vertMax, bool isRightHanded )
    {
        tris_verts[0] = vertMin;
        tris_verts[1] = vertMid;
        tris_verts[2] = vertMax;
        this->isRightHanded = isRightHanded;
    }
};

#endif // VERTEXPROCESSOROBJ_H
