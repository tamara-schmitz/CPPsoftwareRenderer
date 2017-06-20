#ifndef RASTERISER_H
#define RASTERISER_H

#include "common.h"
#include "vmath-0.12/vmath.h"
#include "types/Edge.h"
#include "types/TexCoordsForEdge.h"
#include "types/Texture.h"
#include "types/Mesh.h"
#include "window/window.h"

class Rasteriser
{
    // rasterises triangles and blits them onto the screen using a Window object
    //
    // Our fill convention is top-left (so make sure to use ceil!)
    public:
        explicit Rasteriser( Window* window );
        virtual ~Rasteriser();

        // Getters
        float GetNearZ() const { return near_z; }
        float GetFarZ()  const { return far_z;  }

        // set render colour / texture (setting one type deletes the other
        void SetDrawColour( const SDL_Color& color );
        void SetDrawTexture( shared_ptr<Texture> texture ); // passed by shared pointer

        //-- update matrices
        void UpdateObjectToWorldMatrix( const Matrix4f& objMatrix );
        void UpdateViewMatrix( const Matrix4f& viewMatrix );
//        void UpdatePerspectiveMatrix( const Matrix4f& perspectiveMatrix );
        void UpdatePerspectiveMatrix( const float& fov, const float& zNear, const float& zFar );
        void UpdateScreenspaceTransformMatrix();

        void DrawMesh( shared_ptr<Mesh> mesh ); // draws a mesh
        void FillTriangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 ); // draws a triangle

    private:
        Window* w_window;

        //-- render vars

        bool drawWithTexture = false; // determines whether a texture or a colour should be drawn
        SDL_Color current_colour = { 0, 0, 0, SDL_ALPHA_TRANSPARENT }; // contains current render color
        shared_ptr<Texture> current_texture = NULL; // contains shared pointer to current render texture
        Uint16 current_texture_width = 0, current_texture_height = 0;
        float near_z = 0, far_z = 1; // contains current near and far plane for culling

        // matrices
        Matrix4f objectToWorldTransformMatrix = Matrix4f(); // from object to world
        Matrix4f viewSpaceTransformMatrix = Matrix4f(); // from world to view
        Matrix4f perspectiveTransformMatrix = Matrix4f(); // from view to projection
        Matrix4f screenspaceTransformMatrix = Matrix4f(); // from projection to screenspace
        Matrix4f mvpsMatrix = Matrix4f(); // = screenspaceTransformMatrix * perspectiveTransformMatrix * viewSpaceTransformMatrix * objectToWorldTransformMatrix

        //-- internal render functions
        void UpdateMvpsMatrix() { mvpsMatrix = screenspaceTransformMatrix * perspectiveTransformMatrix * viewSpaceTransformMatrix * objectToWorldTransformMatrix; }
        void ScanTriangle( const Vertexf& vertMin, const Vertexf& vertMid, const Vertexf& vertMax, bool isRightHanded );
        void ScanEdges( Edgef& a, Edgef& b, bool isRightHanded );
        void DrawScanLine( const Edgef& left, const Edgef& right, Uint16 yCoord );
};

#endif // RASTERISER_H
