#ifndef RASTERISER_H
#define RASTERISER_H

#include "vmath-0.12/vmath.h"
#include "types/Edge.h"
#include "types/TexCoordsForEdge.h"
#include "types/Texture.h"
#include "window/window.h"

class Rasteriser
{
    // rasterises triangles and blits them onto the screen using a Window object
    //
    // Our fill convention is top-left (so make sure to use ceil!)
    public:
        explicit Rasteriser( Window* window );
        virtual ~Rasteriser();

        // set render colour / texture (setting one type deletes the other
        void SetDrawColour( const SDL_Color& color );
        void SetDrawTexture( const Texture* texture ); // pass by pointer

        //-- update matrices
        void UpdateViewMatrix( const Matrix4f& viewMatrix );
        void UpdatePerspectiveMatrix( const Matrix4f& perspectiveMatrix );
        void UpdatePerspectiveMatrix( const float& fov, const float& zNear, const float& zFar );
        void UpdateScreenspaceTransformMatrix();
        void UpdateViewAndPerspectiveMatrix( const Matrix4f& viewMatrix, const Matrix4f& perspectiveMatrix );
        void UpdateViewAndPerspectiveMatrix( const Matrix4f& viewMatrix, const float& fov, const float& zNear, const float& zFar );

        void FillTriangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 );

    private:
        Window* w_window;

        //-- render vars

        // determines whether a texture or a colour should be drawn
        bool drawWithTexture;
        SDL_Color current_colour; // contains current render color
        const Texture* current_texture = nullptr; // contains pointer to current render texture
        Uint16 current_texture_width = 0, current_texture_height = 0;

        // matrices
        Matrix4f viewSpaceTransformMatrix = Matrix4f(); // from world to view
        Matrix4f perspectiveTransformMatrix = Matrix4f(); // from view to projection
        Matrix4f screenspaceTransformMatrix = Matrix4f(); // from projection to screenspace
        Matrix4f vpsMatrix = Matrix4f(); // = screenspaceTransformMatrix * perspectiveTransformMatrix * viewSpaceTransformMatrix

        //-- internal render functions
        void UpdateVpsMatrix() { vpsMatrix = screenspaceTransformMatrix * perspectiveTransformMatrix * viewSpaceTransformMatrix; }
        void ScanTriangle( const Vertexf& vertMin, const Vertexf& vertMid, const Vertexf& vertMax, bool isRightHanded );
        void ScanEdges( Edgef& a, Edgef& b, bool isRightHanded );
        void DrawScanLine( const Edgef& left, const Edgef& right, Uint16 yCoord );
};

#endif // RASTERISER_H