#ifndef EDGE_H
#define EDGE_H

#include "common.h"
#include "Vertex.h"
#include "TexCoordsForEdge.h"

template< typename T >
class Edge
{
    // Represents a triangle edge. used for rasterisation.
    // You can walk along the y axis by using DoYStep() or GoToStep().
    // For each Y coordinate you can get your current X coord, texel coords, and depth

    public:
        Edge( const Vertex<T>& vertMin, const Vertex<T>& vertMax, const TexCoordsForEdge<T>& texcoordsforedge, int vertMin_Index );
        virtual ~Edge();

        // getters
        inline T GetYStart()   const { return yStart;   }
        inline T GetYEnd()     const { return yEnd;     }
        inline T GetCurrentX() const { return currentX; }
        inline T GetCurrentTexCoordX() const { return texCoord_currentX; }
        inline T GetCurrentTexCoordY() const { return texCoord_currentY; }
        inline T GetCurrentOneOverZ() const { return texCoord_currentOneOverZ; }
        inline T GetCurrentDepth() const { return currentDepth; }

        // functions
        void DoYStep(); // add xStep to currentX
        void GoToStep( int newY ); // set currentX to new Y (start is 0)

    protected:
        //-- runtime vars

        // pixel x and steps
        T currentX;
        T xStep;
        // texcoord x/y and steps
        T texCoord_currentX;
        T texCoordX_step;
        T texCoord_currentY;
        T texCoordY_step;
        T texCoord_currentOneOverZ;
        T texCoord_OneOverZ_step; // used for perspective correction
        T currentDepth; // used for depth testing
        T depth_step;

        T yPrestep;

        //-- constructor vars
        Vertex<T> vertMini;
        Vertex<T> vertMidi;
        Vertex<T> vertMaxi;
        TexCoordsForEdge<T> texcoords;
        int vertMini_Index;
        int yStart;
        int yEnd;
};

typedef Edge< float > Edgef;
typedef Edge< double > Edged;
typedef Edge< int > Edgei;


#endif // EDGE_H
