#include "scanRenderer.h"

ScanRenderer::ScanRenderer( Window *window )
{
    //ctor
    // copy init values
    w_window = window;
    w_width  = window->Getwidth();
    w_height = window->Getheight();

    // init scanBuffer
    null_scanBuffer.resize( w_height * 2 );
    for ( int i = 0; i <w_height * 2; i++ )
    {
        null_scanBuffer[i] = 0;
    }
    r_scanBuffer.resize( w_height * 2 );
    ClearScanBuffer();

    // create screenspace tranform matrix for current window dimensions
    screenSpaceTransformMatrix = Matrix4f::screenspaceTransform( w_width  / 2.0f, w_height / 2.0f );
    #ifdef PRINT_DEBUG_STUFF
    // debug print
    std::cout << "Screenspace Transform Matrix:: " << std::endl;
    std::cout << "0,0: " << screenSpaceTransformMatrix.at(0, 0) << std::endl;
    std::cout << "0,1: " << screenSpaceTransformMatrix.at(0, 1) << std::endl;
    std::cout << "0,2: " << screenSpaceTransformMatrix.at(0, 2) << std::endl;
    std::cout << "0,3: " << screenSpaceTransformMatrix.at(0, 3) << std::endl;
    std::cout << "1,0: " << screenSpaceTransformMatrix.at(1, 0) << std::endl;
    std::cout << "1,1: " << screenSpaceTransformMatrix.at(1, 1) << std::endl;
    std::cout << "1,2: " << screenSpaceTransformMatrix.at(1, 2) << std::endl;
    std::cout << "1,3: " << screenSpaceTransformMatrix.at(1, 3) << std::endl;
    std::cout << "2,0: " << screenSpaceTransformMatrix.at(2, 0) << std::endl;
    std::cout << "2,1: " << screenSpaceTransformMatrix.at(2, 1) << std::endl;
    std::cout << "2,2: " << screenSpaceTransformMatrix.at(2, 2) << std::endl;
    std::cout << "2,3: " << screenSpaceTransformMatrix.at(2, 3) << std::endl;
    std::cout << "3,0: " << screenSpaceTransformMatrix.at(3, 0) << std::endl;
    std::cout << "3,1: " << screenSpaceTransformMatrix.at(3, 1) << std::endl;
    std::cout << "3,2: " << screenSpaceTransformMatrix.at(3, 2) << std::endl;
    std::cout << "3,3: " << screenSpaceTransformMatrix.at(3, 3) << std::endl;
    #endif // PRINT_DEBUG_STUFF
}

void ScanRenderer::ClearScanBuffer()
{
    r_scanBuffer = null_scanBuffer;
}

int ScanRenderer::getScanValue( int yCoord, bool isMin )
{
    Uint8 offset;

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
    Uint8 offset;

    if ( isMin )
    {
        offset = 0;
    }
    else
    {
        offset = 1;
    }

    // make sure that within range and clamp value
    if ( yCoord >= 0 && yCoord < w_height )
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

float ScanRenderer::triangleArea( Vector2f v1, Vector2f v2, Vector2f v3 )
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
    if ( yCoord < 0 || yCoord > w_height )
    {
        return;
    }

    // Take shape data for a line
    setScanValue( yCoord, xMin, true  );
    setScanValue( yCoord, xMax, false );
}


void ScanRenderer::DrawLine( Vector2f yMinVert, Vector2f yMaxVert, bool isMinX )
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
        int xMin = getScanValue( i, true  );
        int xMax = getScanValue( i, false );

        // iterate through every x
        for ( int j = xMin; j < xMax; j++ )
        {
            w_window->drawPixel( j, i, color );
        }
    }
}

void ScanRenderer::DrawTriangle( Vector2f yMinVert, Vector2f yMidVert,
                                 Vector2f yMaxVert, bool rightHanded )
{
    // draw triangle lines
    DrawLine( yMinVert, yMaxVert,  rightHanded );
    DrawLine( yMinVert, yMidVert, !rightHanded );
    DrawLine( yMidVert, yMaxVert, !rightHanded );
}

void ScanRenderer::FillTriangle( Vector2f v1, Vector2f v2, Vector2f v3, SDL_Color color )
{
    // set up alias for input vectors
    Vector2f& yMinVert = v1;
    Vector2f& yMidVert = v2;
    Vector2f& yMaxVert = v3;

    // sort verts
    if ( yMaxVert.y < yMidVert.y )
    {
        Vector2f temp = yMaxVert;
        yMaxVert = yMidVert;
        yMidVert = temp;
    }
    if ( yMidVert.y < yMinVert.y )
    {
        Vector2f temp = yMidVert;
        yMidVert = yMinVert;
        yMinVert = temp;
    }
    if ( yMaxVert.y < yMidVert.y )
    {
        Vector2f temp = yMaxVert;
        yMaxVert = yMidVert;
        yMidVert = temp;
    }

    float area = triangleArea( yMinVert, yMaxVert, yMidVert );
    // true if right handed (and hence area smaller than 0)
    bool handedness = area < 0;

    DrawTriangle( yMinVert, yMidVert, yMaxVert, handedness );
    FillShape( yMinVert.y, yMaxVert.y, color );
}

void ScanRenderer::FillTriangle( Vector4f v1, Vector4f v2, Vector4f v3, SDL_Color color )
{
    // get Vector4s and apply screenspace transformation
    Vector2f yMinVert = (screenSpaceTransformMatrix * v1).xy();
    Vector2f yMidVert = (screenSpaceTransformMatrix * v2).xy();
    Vector2f yMaxVert = (screenSpaceTransformMatrix * v3).xy();

    #ifdef PRINT_DEBUG_STUFF
    // debug print
    std::cout << "v1::" << " x: " << v1.x << " y: " << v1.y << " z: " << v1.z << " w: " << v1.w << std::endl;
    std::cout << "v2::" << " x: " << v2.x << " y: " << v2.y << " z: " << v2.z << " w: " << v2.w << std::endl;
    std::cout << "yMinVert (v1)::" << " x: " << yMinVert.x << " y: " << yMinVert.y << std::endl;
    std::cout << "yMidVert (v2)::" << " x: " << yMidVert.x << " y: " << yMidVert.y << std::endl;
    #endif // PRINT_DEBUG_STUFF

    // call Vector2f version of FillTriangle
    FillTriangle( yMinVert, yMidVert, yMaxVert, color );
}

ScanRenderer::~ScanRenderer()
{
    //dtor
    null_scanBuffer.clear();
    r_scanBuffer.clear();
}
