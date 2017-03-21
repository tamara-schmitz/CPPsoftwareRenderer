#ifndef EDGE_H
#define EDGE_H

#include "SDL2/SDL_types.h"
#include "vmath-0.12/vmath.h"

template< typename T >
struct Edge
{
    // represents an edge used in rasterisation
    //
    // call GetNextX() to get next X step value
    public:
        Edge( const Vector4<T>& vecMin, const Vector4<T>& vecMax );
        virtual ~Edge();

        // getters
        T GetYStart() { return yStart; }
        T GetYEnd()   { return yEnd;   }
        T GetNextX(); // first call returns initial x!

    private:
        // constructor vars
        Uint16 yStart;
        Uint16 yEnd;

        // runtime vars
        T xStep;
        T currentX;

};

typedef Edge< Vector2f > EdgeVec2f;
typedef Edge< Vector2d > EdgeVec2d;
typedef Edge< Vector2i > EdgeVec2i;
typedef Edge< Vector3f > EdgeVec3f;
typedef Edge< Vector3d > EdgeVec3d;
typedef Edge< Vector3i > EdgeVec3i;
typedef Edge< Vector4f > EdgeVec4f;
typedef Edge< Vector4d > EdgeVec4d;
typedef Edge< Vector4i > EdgeVec4i;


#endif // EDGE_H
