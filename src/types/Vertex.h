#ifndef VERTEX_H
#define VERTEX_H

#include "common.h"

// a vertex represents point in a 3 dimensional coordinate system.
// position is stored in a Vector4 as well as the texel coords of the vertex
template< typename T >
struct Vertex
{
    Vector4<T> posVec = Vector4<T>();
    Vector2<T> texVec = Vector2<T>();

    Vertex()
    {
        posVec = Vector4<T>();
        texVec = Vector2<T>();
    }
    
    Vertex( const Vertex& v )
    {
        posVec =  Vector4<T>(v.posVec);
        texVec = Vector2<T>(v.texVec);
    }

    Vertex& lerp( Vertex other, T lerpamount )
    {
        posVec = posVec.lerp( lerpamount, other.posVec );
        texVec = texVec.lerp( lerpamount, other.texVec );

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
