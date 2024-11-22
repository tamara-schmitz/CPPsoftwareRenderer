#ifndef TEXCOORDSFOREDGE_H
#define TEXCOORDSFOREDGE_H

#include "Vertex.h"

template< typename T >
struct TexCoordsForEdge
{
    // Used by rasteriser to calculate texcoord steps.
    // Due to its knowledge about all the other edges it can do these caculations
    // in its ctor.

    public:
        TexCoordsForEdge() {} // empty constructor
        TexCoordsForEdge( const Vertex<T>& vertMin, const Vertex<T>& vertMid, const Vertex<T>& vertMax );
        virtual ~TexCoordsForEdge();

        // getters
        inline T GetTexCoordX_XStep() const { return texCoordX_XStep; }
        inline T GetTexCoordX_YStep() const { return texCoordX_YStep; }
        inline T GetTexCoordY_XStep() const { return texCoordY_XStep; }
        inline T GetTexCoordY_YStep() const { return texCoordY_YStep; }
        inline T GetOneOverZ_XStep() const { return oneOverZ_XStep; }
        inline T GetOneOverZ_YStep() const { return oneOverZ_YStep; }
        inline T GetDepth_XStep() const { return depth_XStep; }
        inline T GetDepth_YStep() const { return depth_YStep; }
        inline T GetTexCoordX( int index ) const { return texCoordX_values[index]; }
        inline T GetTexCoordY( int index ) const { return texCoordY_values[index]; }
        inline T GetOneOverZ( int index ) const { return oneOverZ_values[index]; }
        inline T GetDepth( int index ) const { return depth_values[index]; }

    private:
        //-- vars
        // texcoord steps
        T texCoordX_XStep;
        T texCoordX_YStep;
        T texCoordY_XStep;
        T texCoordY_YStep;
        T oneOverZ_XStep;
        T oneOverZ_YStep;
        T depth_XStep;
        T depth_YStep;
        // texcoord arrays (each element represents the 3 vertexes with 0=min, 1=mid, 2=max)
        T texCoordX_values[3];
        T texCoordY_values[3];
        T oneOverZ_values[3];
        T depth_values[3];

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
