#ifndef SCANRENDERER_H
#define SCANRENDERER_H

#include <vector>
#include <SDL2/SDL.h>
#include "window.h"
#include "vmath-0.12/vmath.h"

class ScanRenderer
{
    public:
        // ctor & dtor
        ScanRenderer( Window *window );
        virtual ~ScanRenderer();

        // render functions
        void ClearScanBuffer();
        void DrawToScanBuffer( int yCoord, int xMin, int xMax );
        void FillShape( int yMin, int yMax, SDL_Color color );
        void DrawLine( Vector2f yMinVert, Vector2f yMaxVert, bool isMinX );
        void DrawTriangle( Vector2f yMinVert,
                           Vector2f yMidVert,
                           Vector2f yMaxVert,
                           bool orientation );
        void FillTriangle( Vector2f v1,
                           Vector2f v2,
                           Vector2f v3,
                           SDL_Color color );
        void FillTriangle( Vector4f v1,
                           Vector4f v2,
                           Vector4f v3,
                           SDL_Color color );

    protected:

    private:
        // render params
        Window *w_window;
        int w_width;
        int w_height;
        Matrix4f screenSpaceTransformMatrix;
        // scan buffer
        std::vector< int > r_scanBuffer;
        std::vector< int > null_scanBuffer;

        // functions
        int getScanValue( int yCoord, bool isMin );
        void setScanValue( int yCoord, int value, bool isMin );
        int clipInt( int i, int iMin, int iMax );
        float triangleArea( Vector2f v1, Vector2f v2, Vector2f v3 );
};

#endif // SCANRENDERER_H
