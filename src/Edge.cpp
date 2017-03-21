#include "Edge.h"

template< typename T >
Edge<T>::Edge( const Vector4<T>& vecMin, const Vector4<T>& vecMax )
{
    //ctor
    yStart = std::ceil( vecMin->y );
    yEnd   = std::ceil( vecMax->y );

    T yDist = vecMax->y - vecMin->y;
    T xDist = vecMax->x - vecMin->x;

    T yPrestep = yStart - vecMin.y;
    xStep = xDist / yDist;

    // we do -xStep so that first call of GetNextX
    // returns the "real" first currentX
    currentX = vecMin.x + yPrestep * xStep - xStep;

}

template< typename T >
T Edge<T>::GetNextX()
{
    // increases currentX by xStep and then returns currentX
    // Note: we did -xStep in cstor so that first call returns
    // the "real" first currentX
    currentX += xStep;
    return currentX;
}

template< typename T >
Edge<T>::~Edge()
{
    //dtor
}
