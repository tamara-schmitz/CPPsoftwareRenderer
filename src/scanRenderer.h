#ifndef SCANRENDERER_H
#define SCANRENDERER_H

#include <vector>
#include <SDL2/SDL.h>
#include "window.h"
#include "typedefs.h"

class ScanRenderer
{
    public:
        // ctor & dtor
        ScanRenderer( Window *window );
        virtual ~ScanRenderer();

        // render functions
        void DrawToScanBuffer( int yCoord, int xMin, int xMax );
        void FillShape( int yMin, int yMax, SDL_Color color );
        void DrawLine( vertex2D yMinVert, vertex2D yMaxVert, bool isMinX );
        void DrawTriangle( vertex2D yMinVert,
                           vertex2D yMidVert,
                           vertex2D yMaxVert,
                           bool orientation );
        void FillTriangle( vertex2D v1,
                           vertex2D v2,
                           vertex2D v3,
                           SDL_Color color );

    protected:

    private:
        // render params
        Window *w_window;
        unsigned int w_width;
        unsigned int w_height;
        // scan buffer
        std::vector< int > r_scanBuffer;

        // functions
        int  getScanValue( int yCoord, bool isMin );
        void setScanValue( int yCoord, int value, bool isMin );
        int clipInt( int i, int iMin, int iMax );
        float triangleArea( vertex2D v1, vertex2D v2, vertex2D v3 );
};

#endif // SCANRENDERER_H
