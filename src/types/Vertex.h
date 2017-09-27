#ifndef VERTEX_H
#define VERTEX_H

#include "common.h"

// a vertex represents point in a 3 dimensional coordinate system.
// position is stored in a Vector4 as well as the texel coords of the vertex
template< typename T >
struct Vertex
{
    Vector4<T> posVec;
    Vector2<T> texVec;

    // ctor that inits with 0
    Vertex()
    {
        posVec.x = posVec.y = posVec.z = 0;
        posVec.w = 1;
        texVec.x = texVec.y = 0;
    }
};

typedef Vertex< int > Vertexi;
typedef Vertex< float > Vertexf;
typedef Vertex< double > Vertexd;


#endif // VERTEX_H
