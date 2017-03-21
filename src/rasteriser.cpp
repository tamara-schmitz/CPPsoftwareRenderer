#include "rasteriser.h"

Rasteriser::Rasteriser()
{
    //ctor
}

void Rasteriser::ScanTriangle( Vector4f& vecMin, Vector4f& vecMid, Vector4f& vecMax, bool isMin )
{
    // create edges
    EdgeVec4f topToBottom    = EdgeVec4f( vecMin, vecMax );
    EdgeVec4f topToMiddle    = EdgeVec4f( vecMin, vecMid );
    EdgeVec4f middleToBottom = EdgeVec4f( vecMid, vecMax );
}

Rasteriser::~Rasteriser()
{
    //dtor
}
