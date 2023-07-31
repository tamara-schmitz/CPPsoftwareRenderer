#include "Edge.h"

template< typename T >
Edge<T>::Edge( const Vertex<T>& vertMin, const Vertex<T>& vertMax, const TexCoordsForEdge<T>& texcoordsforedge, int vertMin_Index )
{
    //ctor

    // copy vertices and texcoordsforedge
    vertMini = vertMin;
    vertMaxi = vertMax;
    texcoords = texcoordsforedge;
    vertMini_Index = vertMin_Index;

    // create references to posVecs
    Vector4<T>& posMin = vertMini.posVec;
//    Vector4<T>& posMid = vertMidi.posVec; // commented out because unused
    Vector4<T>& posMax = vertMaxi.posVec;

    // get absolute y values
    // we use ceil for compliance with our top-left fill convention!
    yStart = std::ceil( posMin.y );
    yEnd   = std::ceil( posMax.y );

    // get real distances
    T yDist = posMax.y - posMin.y;
    T xDist = posMax.x - posMin.x;

    // prevent division by zero
    if ( yDist == 0 )
        return;

    // calulate stepping for absolute and real prestep
    yPrestep = (T) yStart - posMin.y;
    xStep = xDist / yDist;

    // calculate texcoord steps
    texCoordX_step = texcoords.GetTexCoordX_YStep() + texcoords.GetTexCoordX_XStep() * xStep;
    texCoordY_step = texcoords.GetTexCoordY_YStep() + texcoords.GetTexCoordY_XStep() * xStep;
    texCoord_OneOverZ_step = texcoords.GetOneOverZ_YStep() + texcoords.GetOneOverZ_XStep() * xStep;
    depth_step = texcoords.GetDepth_YStep() + texcoords.GetDepth_XStep() * xStep;

    // initialise currentX and current texcoord x and y with first y-Step
    if ( yStart < 0 )
    {
        // skip ys outside of screen
        GoToStep( abs(yStart) );
        yStart = 0;
    }
    else
    {
        GoToStep( 0 );
    }

}

template< typename T >
void Edge<T>::DoYStep()
{
    // increases current-values by appropriate step
    currentX += xStep;
    texCoord_currentX += texCoordX_step;
    texCoord_currentY += texCoordY_step;
    texCoord_currentOneOverZ += texCoord_OneOverZ_step;
    currentDepth += depth_step;
}

template< typename T >
__attribute__((target_clones("arch=x86-64-v3","default")))
void Edge<T>::GoToStep( int newY )
{
    // set currentX to new Y. Also incorporate yPrestep.
    currentX = vertMini.posVec.x + yPrestep * xStep +
                                            xStep * newY;

    // update xPrestep (depends on currentX)
    xPrestep = currentX - vertMini.posVec.x;

    // set new current texCoord values. Also incorporate yPrestep and xPrestep.
    texCoord_currentX = texcoords.GetTexCoordX( vertMini_Index ) +
                (texcoords.GetTexCoordX_XStep() * xPrestep) +
                (texcoords.GetTexCoordX_YStep() * yPrestep) +
                                                texCoordX_step * newY;
    texCoord_currentY = texcoords.GetTexCoordY( vertMini_Index ) +
                (texcoords.GetTexCoordY_XStep() * xPrestep) +
                (texcoords.GetTexCoordY_YStep() * yPrestep) +
                                                texCoordY_step * newY;
    texCoord_currentOneOverZ = texcoords.GetOneOverZ( vertMini_Index ) +
                (texcoords.GetOneOverZ_XStep() * xPrestep) +
                (texcoords.GetOneOverZ_YStep() * yPrestep) +
                                                texCoord_OneOverZ_step * newY;
    currentDepth = texcoords.GetDepth( vertMini_Index ) +
                (texcoords.GetDepth_XStep() * xPrestep) +
                (texcoords.GetDepth_YStep() * yPrestep) + depth_step * newY;

}

template< typename T >
Edge<T>::~Edge()
{
    //dtor
}


// now make sure that the compiler includes implementations for float, double and int
template class Edge< float >;
template class Edge< double >;
template class Edge< int >;
