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

int ScanRenderer::getScanValue( int yCoord, bool isMin )
{
    int offset;

    if ( isMin )
    {
        offset = 0;
    }
    else
    {
        offset = 1;
    }

    yCoord = clipInt( yCoord, 0, w_height );

    return r_scanBuffer[ yCoord * 2 + offset ];
}

void ScanRenderer::setScanValue( int yCoord, int value, bool isMin )
{
    int offset;

    if ( isMin )
    {
        offset = 0;
    }
    else
    {
        offset = 1;
    }

    if ( yCoord >= 0 && yCoord < (int) w_height )
    {
        r_scanBuffer[ yCoord * 2 + offset ] = clipInt(value, 0, w_width);
    }
}

int ScanRenderer::clipInt( int i, int iMin, int iMax )
{
    if ( i < iMin )
    {
        return iMin;
    }
    else if ( i > iMax )
    {
        return iMax;
    }
    else
    {
        return i;
    }
}

float ScanRenderer::triangleArea( vertex2D v1, vertex2D v2, vertex2D v3 )
{
    int x1 = v2.x - v1.x;
    int y1 = v2.y - v1.y;

    int x2 = v3.x - v1.x;
    int y2 = v3.y - v1.y;

    return ( 0.5f * (x1 * y2 - x2 * y1) );
}

void ScanRenderer::DrawToScanBuffer( int yCoord, int xMin, int xMax )
{
    // return if y is outside of scanBuffer
    if ( yCoord < 0 || yCoord > (int) w_height )
    {
        return;
    }

    // Take shape data for a line
    setScanValue( yCoord, xMin, true  );
    setScanValue( yCoord, xMax, false );
}


void ScanRenderer::DrawLine( vertex2D yMinVert, vertex2D yMaxVert, bool isMinX )
{
    // draws a line to the scan buffer
    // either at xMin or at xMax
    // the following must be true:
    // yMinVert < yMaxVert

    // set internal vars
    int yStart = yMinVert.y;
    int yEnd   = yMaxVert.y;
    int xStart = yMinVert.x;
    int xEnd   = yMaxVert.x;
    float curX = (float) xStart;
    // calculate distances
    int yDist = yEnd - yStart;
    int xDist = xEnd - xStart;
    // line stepping
    float xStep = (float) xDist / (float) yDist;
    // discard if wrong y
    if ( yDist <= 0)
    {
        return;
    }

    // finally add scanlines
    for ( int i = yStart; i < clipInt(yEnd, yStart, w_height); i++ )
    {
        setScanValue( i, curX, isMinX);
        curX += xStep;
    }
}


void ScanRenderer::FillShape( int yMin, int yMax, SDL_Color color )
{
    yMin = clipInt( yMin, 0, w_height );
    yMax = clipInt( yMax, 0, w_height );

    // Draw Shape for ys
    for ( int i = yMin; i < yMax; i++ )
    {
        // get x values
        int xMin = clipInt(getScanValue( i, true ), 0, w_width);
        int xMax = clipInt(getScanValue( i, false), 0, w_width);

        // iterate through every x
        for ( int j = xMin; j < xMax; j++ )
        {
            w_window->drawPixel( j, i, color );
        }
    }
}

void ScanRenderer::DrawTriangle( vertex2D yMinVert, vertex2D yMidVert, vertex2D yMaxVert, bool rightHanded )
{
    // draw triangle lines
    DrawLine( yMinVert, yMaxVert,  rightHanded );
    DrawLine( yMinVert, yMidVert, !rightHanded );
    DrawLine( yMidVert, yMaxVert, !rightHanded );
}

void ScanRenderer::FillTriangle( vertex2D v1, vertex2D v2, vertex2D v3, SDL_Color color )
{
    vertex2D yMinVert = v1;
    vertex2D yMidVert = v2;
    vertex2D yMaxVert = v3;

    // sort verts
    if ( yMaxVert.y < yMidVert.y )
    {
        vertex2D temp = yMaxVert;
        yMaxVert = yMidVert;
        yMidVert = temp;
    }
    if ( yMidVert.y < yMinVert.y )
    {
        vertex2D temp = yMidVert;
        yMidVert = yMinVert;
        yMinVert = temp;
    }
    if ( yMaxVert.y < yMidVert.y )
    {
        vertex2D temp = yMaxVert;
        yMaxVert = yMidVert;
        yMidVert = temp;
    }

    float area = triangleArea( yMinVert, yMaxVert, yMidVert );
    bool handedness = area < 0;

    DrawTriangle( yMinVert, yMidVert, yMaxVert, handedness );
    FillShape( yMinVert.y, yMaxVert.y, color );
}

ScanRenderer::~ScanRenderer()
{
    //dtor
    r_scanBuffer.clear();
}
