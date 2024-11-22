#include "scanRenderer.h"

ScanRenderer::ScanRenderer( Window *window, bool useRenderDrawFuncs )
{
    //ctor
    // copy init values
    w_window = window;
    w_useRenderDrawFuncs = useRenderDrawFuncs;
    w_width  = window->Getwidth();
    w_halfwidth = w_width / 2.0f;
    w_height = window->Getheight();
    w_halfheight = w_height / 2.0f;

    // init scanBuffer
    null_scanBuffer.resize( w_height * 2 );
    for ( int i = 0; i < w_height * 2; i++ )
    {
        null_scanBuffer[i] = 0;
    }
    r_scanBuffer.resize( w_height * 2 );
    ClearScanBuffer();

    // create viewspace tranform matrix ( world -> view )
    viewSpaceTransformMatrix = Matrix4f::createTranslation( 0, 0, 6 );

    // create perspective transform matrix with null settings ( view -> frustum )
    UpdatePerspective( 0, 0, 1000 );
}

void ScanRenderer::UpdatePerspective( float fov, float zNear, float zFar )
{
    perspectiveTransformMatrix = Matrix4f::perspectiveTransform( fov, w_width / w_height, zNear, zFar );

    // Update vpMatrix
    vpMatrix = perspectiveTransformMatrix * viewSpaceTransformMatrix;
}

void ScanRenderer::ClearScanBuffer()
{
    r_scanBuffer = null_scanBuffer;
}

int ScanRenderer::getScanValue( int yCoord, bool isMin )
{
    uint_fast8_t offset;

    if ( isMin )
    {
        offset = 0;
    }
    else
    {
        offset = 1;
    }

    yCoord = clipNumber( yCoord, 0, w_height );

    return r_scanBuffer[ yCoord * 2 + offset ];
}

void ScanRenderer::setScanValue( int yCoord, int value, bool isMin )
{
    uint_fast8_t offset;

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
        r_scanBuffer[ yCoord * 2 + offset ] = clipNumber(value, 0, w_width);
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
    //
    // Fill Convention is Top-Left (we use a ceil function)

    // set internal vars
    int yStart = std::ceil(yMinVert.y);
    int yEnd   = std::ceil(yMaxVert.y);

    // calculate distances
    float yDist = yMaxVert.y - yMinVert.y;
    float xDist = yMaxVert.x - yMinVert.x;
    // line stepping
    float xStep =  xDist / yDist;
    float yPrestep = yStart - yMinVert.y; // compensation for pixel y (vert y is most likely no int)
    float curX = yMinVert.x + yPrestep * xStep;

    // discard if wrong y orientation
    if ( yDist <= 0)
    {
        return;
    }

    // finally add scanlines
    for ( int i = yStart; i < clipNumber(yEnd, yStart, w_height); i++ )
    {
        setScanValue( i, std::ceil(curX), isMinX);
        curX += xStep;
    }
}


void ScanRenderer::FillShape( int yMin, int yMax, SDL_Color* color )
{
    yMin = clipNumber( yMin, 0, w_height );
    yMax = clipNumber( yMax, 0, w_height );

    // swap yMin and yMax if wrong way around
    if ( yMin > yMax )
    {
        int i = yMax;
        yMax = yMin;
        yMin = i;
    }

    // Reserve memory space for lines if SDL method
    if ( w_useRenderDrawFuncs )
    {
        w_window->reserveAddLines( yMax - yMin );
    }


    // Draw Shape for ys
    for ( int i = yMin; i < yMax; i++ )
    {
        // get x values
        int xMin = getScanValue( i, true  );
        int xMax = getScanValue( i, false );


        // SDL method
        if ( w_useRenderDrawFuncs )
        {
            w_window->drawLine( SDL_Point { xMin, i }, SDL_Point { xMax, i }, *color );
        }
        // "traditional" per-pixel manipulation
        else
        {
            // iterate through every x
            for ( int j = xMin; j < xMax; j++ )
            {
                w_window->drawPixel( j, i, *color );
            }
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

void ScanRenderer::FillTriangle( Vector2f v1, Vector2f v2, Vector2f v3, SDL_Color* color )
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

    // use of ceil for compliance with fill convention ( see drawLine() )
    FillShape( std::ceil(yMinVert.y), std::ceil(yMaxVert.y), color );
}

void ScanRenderer::FillTriangle( Vector4f v1, Vector4f v2, Vector4f v3, SDL_Color* color )
{
    // get Vector4s and apply screenspace transformation
    Vector2f yMinVert = ( vpMatrix * v1 ).screenspaceVec3( w_halfwidth, w_halfheight ).xy();
    Vector2f yMidVert = ( vpMatrix * v2 ).screenspaceVec3( w_halfwidth, w_halfheight ).xy();
    Vector2f yMaxVert = ( vpMatrix * v3 ).screenspaceVec3( w_halfwidth, w_halfheight ).xy();

    if ( printDebug ) [[unlikely]]
    {
        // debug print
        std::cout << "v1::" << " x: " << v1.x << " y: " << v1.y << " z: " << v1.z << " w: " << v1.w << std::endl;
        std::cout << "v2::" << " x: " << v2.x << " y: " << v2.y << " z: " << v2.z << " w: " << v2.w << std::endl;
        std::cout << "v3::" << " x: " << v3.x << " y: " << v3.y << " z: " << v3.z << " w: " << v3.w << std::endl;
        std::cout << "yMinVert (v1)::" << " x: " << yMinVert.x << " y: " << yMinVert.y << std::endl;
        std::cout << "yMidVert (v2)::" << " x: " << yMidVert.x << " y: " << yMidVert.y << std::endl;
        std::cout << "yMaxVert (v3)::" << " x: " << yMaxVert.x << " y: " << yMaxVert.y << std::endl;
    }

    // call Vector2f version of FillTriangle
    FillTriangle( yMinVert, yMidVert, yMaxVert, color );
}

ScanRenderer::~ScanRenderer()
{
    //dtor
    null_scanBuffer.clear();
    r_scanBuffer.clear();

    if ( printDebug ) [[unlikely]]
    {
        std::cout << "Dtor of scanRenderer object was called!" << std::endl;
    }
}
