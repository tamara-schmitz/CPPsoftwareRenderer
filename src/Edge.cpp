#include "Edge.h"

template< typename T >
Edge<T>::Edge( const Vector3<T>& vecMin, const Vector3<T>& vecMax )
{
    //ctor

    // we use ceil for compliance with our top-left fill convention!
    yStart = clipNumber< Uint16 >( std::ceil( vecMin.y ), 0 );
    yEnd   = clipNumber< Uint16 >( std::ceil( vecMax.y ), 0 );

    T yDist = vecMax.y - vecMin.y;
    T xDist = vecMax.x - vecMin.x;

    T yPrestep = yStart - vecMin.y;
    xStep = xDist / yDist;

    currentX = vecMin.x + yPrestep * xStep;

}

template< typename T >
void Edge<T>::DoStep()
{
    // increases currentX by xStep
    currentX += xStep;
}

template< typename T >
Edge<T>::~Edge()
{
    //dtor
}


// now make sure compiler includes implementations for float, double and int
template class Edge< float >;
template class Edge< double >;
template class Edge< int >;
