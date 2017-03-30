#ifndef RASTERISER_H
#define RASTERISER_H

#include "vmath-0.12/vmath.h"
#include "types/Edge.h"
#include "window/window.h"

class Rasteriser
{
    // rasterises triangles and blits them onto the screen using a Window object
    //
    // Our fill convention is top-left (so make sure to use ceil!)
    public:
        Rasteriser( Window* window );
        virtual ~Rasteriser();

        void SetRenderColour( const SDL_Color& color ){ current_colour = color; }
        void UpdateViewMatrix( const Matrix4f& viewMatrix );
        void UpdatePerspectiveMatrix( const Matrix4f& perspectiveMatrix );
        void UpdatePerspectiveMatrix( const float& fov, const float& zNear, const float& zFar );
        void UpdateScreenspaceTransformMatrix();
        void UpdateViewAndPerspectiveMatrix( const Matrix4f& viewMatrix, const Matrix4f& perspectiveMatrix );
        void UpdateViewAndPerspectiveMatrix( const Matrix4f& viewMatrix, const float& fov, const float& zNear, const float& zFar );

        void FillTriangle( const Vector4f& v1, const Vector4f& v2, const Vector4f& v3 );

    private:
        Window* w_window;

        // current render vars
        SDL_Color current_colour = SDL_Color{ 200, 200, 200, SDL_ALPHA_OPAQUE }; // contains current render color
        Matrix4f viewSpaceTransformMatrix = Matrix4f(); // from world to view
        Matrix4f perspectiveTransformMatrix = Matrix4f(); // from view to projection
        Matrix4f screenspaceTransformMatrix = Matrix4f(); // from projection to screenspace
        Matrix4f vpsMatrix = Matrix4f(); // = screenspaceTransformMatrix * perspectiveTransformMatrix * viewSpaceTransformMatrix
        void UpdateVpsMatrix() { vpsMatrix = screenspaceTransformMatrix * perspectiveTransformMatrix * viewSpaceTransformMatrix; }

        // internal render functions
        void ScanTriangle( const Vector3f& vecMin, const Vector3f& vecMid, const Vector3f& vecMax, bool isRightHanded );
        void ScanEdges( Edgef& a, Edgef& b, bool isRightHanded );
        void DrawScanLine( const Edgef& left, const Edgef& right, Uint16 yCoord );
};

#endif // RASTERISER_H
