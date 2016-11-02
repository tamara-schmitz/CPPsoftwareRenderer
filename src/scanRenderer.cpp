#include "scanRenderer.h"

ScanRenderer::ScanRenderer( Window *window )
{
    //ctor
    // copy init values
    w_window = window;
    w_width  = window->Getwidth();
    w_height = window->Getheight();

    // resize scanBuffer
    r_scanBuffer.resize( w_height * 2 );
}

void ScanRenderer::DrawToScanBuffer( int yCoord, int xMin, int xMax )
{
    // Take shape data for a line
    r_scanBuffer[ yCoord * 2     ] = xMin;
    r_scanBuffer[ yCoord * 2 + 1 ] = xMax;
}

void ScanRenderer::FillShape( int yMin, int yMax, SDL_Color color )
{
    // Draw Shape for ys
    for ( int i = yMin; i < yMax; i++ )
    {
        int xMin = r_scanBuffer[ i * 2 ];
        int xMax = r_scanBuffer[ i * 2 + 1 ];

        for ( int j = xMin; j < xMax; j++ )
        {
            w_window->drawPixel( j, i, color );
        }
    }
}

ScanRenderer::~ScanRenderer()
{
    //dtor
}
