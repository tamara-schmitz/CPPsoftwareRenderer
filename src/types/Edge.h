#ifndef EDGE_H
#define EDGE_H

#include "common.h"
#include "SDL2/SDL_types.h"
#include "vmath-0.12/vmath.h"
#include "Vertex.h"
#include "TexCoordsForEdge.h"

template< typename T >
class Edge
{
    // represents an edge used in rasterisation
    //
    // call GetNextX() to get next X step value
    public:
        Edge( const Vertex<T>& vertMin, const Vertex<T>& vertMax, const TexCoordsForEdge<T>& texcoordsforedge, int vertMin_Index );
        virtual ~Edge();

        // getters
        T GetYStart()   const { return yStart;   }
        T GetYEnd()     const { return yEnd;     }
        T GetCurrentX() const { return currentX; }
        T GetCurrentTexCoordX() const { return texCoord_currentX; }
        T GetCurrentTexCoordY() const { return texCoord_currentY; }
        T GetCurrentOneOverZ() const { return texCoord_currentOneOverZ; }

        // functions
        void DoYStep(); // add xStep to currentX
        void GoToStep( Uint16 newY ); // set currentX to new Y (start is 0)

    protected:
        //-- constructor vars
        Vertex<T> vertMini;
        Vertex<T> vertMidi;
        Vertex<T> vertMaxi;
        TexCoordsForEdge<T> texcoords;
        int vertMini_Index;
        Uint16 yStart;
        Uint16 yEnd;

        //-- runtime vars

        // pixel x and steps
        T yPrestep;
        T xPrestep;
        T xStep;
        T currentX;
        // texcoord x/y and steps
        T texCoordX_step;
        T texCoordY_step;
        T texCoord_OneOverZ_step;
        T texCoord_currentX;
        T texCoord_currentY;
        T texCoord_currentOneOverZ;

};

typedef Edge< float > Edgef;
typedef Edge< double > Edged;
typedef Edge< int > Edgei;


#endif // EDGE_H
