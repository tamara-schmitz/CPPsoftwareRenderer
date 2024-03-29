#include "TexCoordsForEdge.h"

template< typename T >
TexCoordsForEdge<T>::TexCoordsForEdge(const Vertex<T>& vertMin, const Vertex<T>& vertMid, const Vertex<T>& vertMax)
{
    //ctor

    // prevent division by zero
    assert( vertMin.posVec.w != 0 );
    assert( vertMid.posVec.w != 0 );
    assert( vertMax.posVec.w != 0 );

    // calculuate oneOver_dX and oneOver_dY
    T dX =( (vertMid.posVec.x - vertMax.posVec.x) *
                   (vertMin.posVec.y - vertMax.posVec.y) ) -
                 ( (vertMin.posVec.x - vertMax.posVec.x) *
                   (vertMid.posVec.y - vertMax.posVec.y) );
    assert( dX != 0 );
    T oneOverdX = 1.0f / dX;
    T oneOverdY = -oneOverdX;

    // calculate texCoord values from verts
    depth_values[0] = vertMin.posVec.z;
    depth_values[1] = vertMid.posVec.z;
    depth_values[2] = vertMax.posVec.z;
    oneOverZ_values[0] = 1.0f / vertMin.posVec.w;
    oneOverZ_values[1] = 1.0f / vertMid.posVec.w;
    oneOverZ_values[2] = 1.0f / vertMax.posVec.w;
    texCoordX_values[0] = vertMin.texVec.x * oneOverZ_values[0];
    texCoordX_values[1] = vertMid.texVec.x * oneOverZ_values[1];
    texCoordX_values[2] = vertMax.texVec.x * oneOverZ_values[2];
    texCoordY_values[0] = vertMin.texVec.y * oneOverZ_values[0];
    texCoordY_values[1] = vertMid.texVec.y * oneOverZ_values[1];
    texCoordY_values[2] = vertMax.texVec.y * oneOverZ_values[2];

    // calculate texCoord steps
    texCoordX_XStep = calcXStep( texCoordX_values, oneOverdX, vertMin, vertMid, vertMax );
    texCoordX_YStep = calcYStep( texCoordX_values, oneOverdY, vertMin, vertMid, vertMax );
    texCoordY_XStep = calcXStep( texCoordY_values, oneOverdX, vertMin, vertMid, vertMax );
    texCoordY_YStep = calcYStep( texCoordY_values, oneOverdY, vertMin, vertMid, vertMax );
    oneOverZ_XStep  = calcXStep( oneOverZ_values,  oneOverdX, vertMin, vertMid, vertMax );
    oneOverZ_YStep  = calcYStep( oneOverZ_values,  oneOverdY, vertMin, vertMid, vertMax );
    depth_XStep     = calcXStep( depth_values, oneOverdX, vertMin, vertMid, vertMax );
    depth_YStep     = calcYStep( depth_values, oneOverdY, vertMin, vertMid, vertMax );
}

template< typename T >
T TexCoordsForEdge<T>::calcXStep(const T(& values)[3], const T& oneOver_dX, const Vertex<T>& vertMin, const Vertex<T>& vertMid, const Vertex<T>& vertMax) const
{
    return ( ( (values[1] - values[2]) *
             (vertMin.posVec.y - vertMax.posVec.y) ) -
             ( (values[0] - values[2]) *
             (vertMid.posVec.y - vertMax.posVec.y) ) ) * oneOver_dX;
}

template< typename T >
T TexCoordsForEdge<T>::calcYStep(const T(& values)[3], const T& oneOver_dY, const Vertex<T>& vertMin, const Vertex<T>& vertMid, const Vertex<T>& vertMax) const
{
    return ( ( (values[1] - values[2]) *
             (vertMin.posVec.x - vertMax.posVec.x) ) -
             ( (values[0] - values[2]) *
             (vertMid.posVec.x - vertMax.posVec.x) ) ) * oneOver_dY;
}

template< typename T >
TexCoordsForEdge<T>::~TexCoordsForEdge()
{
    //dtor

}

// now make sure that the compiler includes implementations for float, double and int
template struct TexCoordsForEdge< float >;
template struct TexCoordsForEdge< double >;
template struct TexCoordsForEdge< int >;
