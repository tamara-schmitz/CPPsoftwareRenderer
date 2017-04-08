#ifndef TEXCOORDSFOREDGE_H
#define TEXCOORDSFOREDGE_H

#include "Vertex.h"

template< typename T >
struct TexCoordsForEdge
{
    public:
        TexCoordsForEdge() {} // empty constructor
        TexCoordsForEdge( const Vertex<T>& vertMin, const Vertex<T>& vertMid, const Vertex<T>& vertMax );
        virtual ~TexCoordsForEdge();

        // getters
        T GetTexCoordX_XStep() { return texCoordX_XStep; }
        T GetTexCoordX_YStep() { return texCoordX_YStep; }
        T GetTexCoordY_XStep() { return texCoordY_XStep; }
        T GetTexCoordY_YStep() { return texCoordY_YStep; }
        T GetOneOverZ_XStep() { return oneOverZ_XStep; }
        T GetOneOverZ_YStep() { return oneOverZ_YStep; }
        T GetTexCoordX( int index ) { return texCoordX_values[index]; }
        T GetTexCoordY( int index ) { return texCoordY_values[index]; }
        T GetOneOverZ( int index ) { return oneOverZ_values[index]; }

    private:
        // TODO implement TexCoords Class

        //-- vars
        // texcoord steps
        T texCoordX_XStep;
        T texCoordX_YStep;
        T texCoordY_XStep;
        T texCoordY_YStep;
        T oneOverZ_XStep;
        T oneOverZ_YStep;
        // texcoord arrays (each element represents the 3 vertexes with 0=min, 1=mid, 2=max)
        T texCoordX_values[3];
        T texCoordY_values[3];
        T oneOverZ_values[3];

        //-- step calc functions
        // these functions calculate the xstep / ystep for given vertexes and values passed by reference
        T calcXStep( const T (&values)[3], const T& oneOver_dX,
                     const Vertex<T>& vertMin, const Vertex<T>& vertMid, const Vertex<T>& vertMax ) const;
        T calcYStep( const T (&values)[3], const T& oneOver_dY,
                     const Vertex<T>& vertMin, const Vertex<T>& vertMid, const Vertex<T>& vertMax ) const;

};

typedef TexCoordsForEdge< float > TexCoordsForEdgef;
typedef TexCoordsForEdge< double > TexCoordsForEdged;
typedef TexCoordsForEdge< int > TexCoordsForEdgei;

#endif // TEXCOORDSFOREDGE_H
