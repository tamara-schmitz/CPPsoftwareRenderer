#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "types/Vertex.h"
#include "types/Triangle.h"
#include "types/Mesh.h"
#include "types/Texture.h"
#include "types/SafeQueue.h"
#include "types/SafeDynArray.h"
#include "types/VertexProcessorObjs.h"
#include "window/window.h"

class Renderer
{
    public:
        Renderer( Window* window );

        // settings
        void SetObjectToWorldMatrix( const Matrix4f& objectMatrix );
        void SetWorldToViewMatrix( const Matrix4f& viewMatrix );
        void SetViewToPerspectiveMatrix( const float& fov, const float& zNear, const float& zFar );
        void SetPerspectiveToScreenSpaceMatrix();
        void SetDrawColour( const SDL_Color& color );
        void SetDrawTexture( const shared_ptr< Texture >& texture );

        // render functions
        void ClearBuffers();
        void DrawMesh( shared_ptr<Mesh> mesh );
        void FillTriangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 );
        void FillTriangle( Triangle tris );

        // debug rendering functions
        void DrawFarPlane();
        void DrawNearPlane();

    private:
        Window* w_window;
        bool drawWithTexture = false; // determines whether a texture or a colour should be drawn
        shared_ptr< SDL_Color > current_colour;
        shared_ptr< Texture > current_texture;

        float near_z = 0;
        float far_z  = 1;

        shared_ptr< SafeQueue< VPIO > > in_vpios;
        shared_ptr< SafeDynArray< VPOO > > out_vpoos;
        shared_ptr< std::vector< float > > z_buffer;

        Matrix4f objMatrix, viewMatrix, perspMatrix, screenMatrix;
        Matrix4f objViewMatrix, perspScreenMatrix;
};

#endif // RENDERER_H
