#ifndef RASTERISER_H
#define RASTERISER_H

#include "common.h"
#include "vmath-0.12/vmath.h"
#include "Edge.h"

class Rasteriser
{
    public:
        Rasteriser();
        virtual ~Rasteriser();

    protected:

    private:
        // internal render functions
        void ScanTriangle( Vector4f& vecMin, Vector4f& vecMid, Vector4f& vecMax, bool isMin );
};

#endif // RASTERISER_H
