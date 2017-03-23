#include "rasteriser.h"

Rasteriser::Rasteriser( Window* window )
{
    //ctor
    w_window = window;

    UpdateScreenspaceTransformMatrix();
}

void Rasteriser::UpdateViewMatrix( const Matrix4f& viewMatrix )
{
    viewSpaceTransformMatrix = viewMatrix;
    UpdateVpsMatrix();
}
void Rasteriser::UpdatePerspectiveMatrix( const Matrix4f& perspectiveMatrix )
{
    perspectiveTransformMatrix = perspectiveMatrix;
    UpdateVpsMatrix();
}
void Rasteriser::UpdatePerspectiveMatrix( const float& fov, const float& zNear, const float& zFar )
{
    perspectiveTransformMatrix = Matrix4f::perspectiveTransform( fov, w_window->Getwidth() / w_window->Getheight(), zNear, zFar );
    UpdateVpsMatrix();
}
void Rasteriser::UpdateScreenspaceTransformMatrix()
{
    screenspaceTransformMatrix = Matrix4f::screenspaceTransform( w_window->Getwidth() / 2.0f, w_window->Getheight() / 2.0f );
    UpdateVpsMatrix();
}
void Rasteriser::UpdateViewAndPerspectiveMatrix( const Matrix4f& viewMatrix, const Matrix4f& perspectiveMatrix )
{
    viewSpaceTransformMatrix = viewMatrix;
    perspectiveTransformMatrix = perspectiveMatrix;
    UpdateVpsMatrix();
}
void Rasteriser::UpdateViewAndPerspectiveMatrix( const Matrix4f& viewMatrix, const float& fov, const float& zNear, const float& zFar )
{
    viewSpaceTransformMatrix = viewMatrix;
    perspectiveTransformMatrix = Matrix4f::perspectiveTransform( fov, w_window->Getwidth() / w_window->Getheight(), zNear, zFar );
    UpdateVpsMatrix();
}

void Rasteriser::FillTriangle( const Vector4f& v1, const Vector4f& v2, const Vector4f& v3 )
{
    // set up alias for input vectors
//    Vector3f yMinVert = ( vpMatrix * v1 ).screenspaceVec3( w_window->Getwidth(), w_window->Getheight() );
//    Vector3f yMidVert = ( vpMatrix * v2 ).screenspaceVec3( w_window->Getwidth(), w_window->Getheight() );
//    Vector3f yMaxVert = ( vpMatrix * v3 ).screenspaceVec3( w_window->Getwidth(), w_window->Getheight() );
    Vector3f yMinVert = ( vpsMatrix * v1 ).xyz();
    Vector3f yMidVert = ( vpsMatrix * v2 ).xyz();
    Vector3f yMaxVert = ( vpsMatrix * v3 ).xyz();

    #ifdef PRINT_DEBUG_STUFF
            std::cout << "yMinVert - x: " << yMinVert.x << " y: " << yMinVert.y << " z: " << yMinVert.z << std::endl;
    #endif // PRINT_DEBUG_STUFF

    // sort verts
    if ( yMaxVert.y < yMidVert.y )
    {
        std::swap( yMidVert, yMaxVert );
    }
    if ( yMidVert.y < yMinVert.y )
    {
        std::swap( yMinVert, yMidVert );
    }
    if ( yMaxVert.y < yMidVert.y )
    {
        std::swap( yMidVert, yMaxVert );
    }

    float area = triangleArea< float >( yMinVert, yMaxVert, yMidVert );
    // true if right handed (and hence area smaller than 0)
    bool handedness = area >= 0;

    ScanTriangle( yMinVert, yMidVert, yMaxVert, handedness );
}

void Rasteriser::ScanTriangle( const Vector3f& vecMin, const Vector3f& vecMid, const Vector3f& vecMax, bool isRightHanded )
{
    // create edges
    Edgef topToBottom    = Edgef( vecMin, vecMax );
    Edgef topToMiddle    = Edgef( vecMin, vecMid );
    Edgef middleToBottom = Edgef( vecMid, vecMax );

    ScanEdges( topToBottom, topToMiddle, isRightHanded );
    ScanEdges( topToBottom, middleToBottom, isRightHanded );
}

void Rasteriser::ScanEdges( Edgef& a, Edgef& b, bool isRightHanded )
{
    // assign left and right based on $isRightHanded
    Edgef& left  = isRightHanded ? b : a;
    Edgef& right = isRightHanded ? a : b;

    Uint16 yStart = b.GetYStart();
    Uint16 yEnd   = b.GetYEnd();

    for ( Uint16 i = yStart; i < clipNumber( yEnd, yStart, (Uint16) w_window->Getheight() ); i++ )
    {
        DrawScanLine( left, right, i );
        left.DoStep();
        right.DoStep();
    }
}

void Rasteriser::DrawScanLine( const Edgef& left, const Edgef& right, Uint16 yCoord )
{
    int xMin = std::ceil( left.GetCurrentX() );
    int xMax = std::ceil( right.GetCurrentX() );

    for ( int x = clipNumber( xMin , 0, (int) w_window->Getwidth() ); x < clipNumber( xMax , xMin, (int) w_window->Getwidth() ); x++ )
    {
        w_window->drawPixel( x, yCoord, current_colour );
    }
}

Rasteriser::~Rasteriser()
{
    //dtor

    #ifdef PRINT_DEBUG_STUFF
    std::cout << "Dtor of Rasteriser object was called!" << std::endl;
    #endif // PRINT_DEBUG_STUFF
}
