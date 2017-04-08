#ifndef VERTEX_H
#define VERTEX_H

#include "common.h"
#include "SDL2/SDL_types.h"
#include "vmath-0.12/vmath.h"

// a vertex represents point in a 3 dimensional coordinate system.
// position is stored in a Vector4 as well as the texel coords of the vertex
template< typename T >
struct Vertex
{
    Vector4<T> posVec;
    Vector4<T> texVec;
};

typedef Vertex< int > Vertexi;
typedef Vertex< float > Vertexf;
typedef Vertex< double > Vertexd;


#endif // VERTEX_H
