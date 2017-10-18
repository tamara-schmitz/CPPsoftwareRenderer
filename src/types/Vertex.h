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

    Vertex& lerp( Vertex other, T lerpamount )
    {
        posVec.lerp( lerpamount, other.posVec );
        texVec.lerp( lerpamount, other.texVec );

        return *this;
    }
    Vertex lerp_new( Vertex other, T lerpamount ) const
    {
        Vertex result = *this;
        result.lerp( other, lerpamount );

        return result;
    }

    T GetPosVecComponent ( uint_fast8_t index ) const
    {
        switch ( index )
        {
        case 0:
            return posVec.x;
        case 1:
            return posVec.y;
        case 2:
            return posVec.z;
        case 3:
            return posVec.w;
        default:
            throw std::out_of_range( "Not a valid posvec component index!" );
        }
    }
};

typedef Vertex< int > Vertexi;
typedef Vertex< float > Vertexf;
typedef Vertex< double > Vertexd;


#endif // VERTEX_H
