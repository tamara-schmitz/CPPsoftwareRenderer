#ifndef SCANRENDERER_H
#define SCANRENDERER_H

#include <vector>
#include <SDL2/SDL.h>
#include "window.h"

class ScanRenderer
{
    public:
        // ctor & dtor
        ScanRenderer( Window *window );
        virtual ~ScanRenderer();

        // render functions
        void DrawToScanBuffer( int yCoord, int xMin, int xMax );
        void FillShape( int yMin, int yMax, SDL_Color color );

    protected:

    private:
        // render params
        Window *w_window;
        unsigned int w_width;
        unsigned int w_height;
        // scan buffer
        std::vector< int > r_scanBuffer;
};

#endif // SCANRENDERER_H
