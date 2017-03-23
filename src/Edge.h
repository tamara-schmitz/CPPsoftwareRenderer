#ifndef EDGE_H
#define EDGE_H

#include "common.h"
#include "SDL2/SDL_types.h"
#include "vmath-0.12/vmath.h"

template< typename T >
struct Edge
{
    // represents an edge used in rasterisation
    //
    // call GetNextX() to get next X step value
    public:
        Edge( const Vector3<T>& vecMin, const Vector3<T>& vecMax );
        virtual ~Edge();

        // getters
        T GetYStart()   const { return yStart;   }
        T GetYEnd()     const { return yEnd;     }
        T GetCurrentX() const { return currentX; } // first call returns initial x!
        void DoStep();

    private:
        // constructor vars
        Uint16 yStart;
        Uint16 yEnd;

        // runtime vars
        T xStep;
        T currentX;

};

typedef Edge< float > Edgef;
typedef Edge< double > Edged;
typedef Edge< int > Edgei;


#endif // EDGE_H
