#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "types/Vertex.h"
#include "types/Texture.h"
#include "types/Mesh.h"

class Renderer
{
    public:
        Renderer();

        // settings
        void SetObjectToWorldMatrix( const Matrix4f& objectMatrix );
        void SetWorldToViewMatrix( const Matrix4f& viewMatrix );
        void SetViewToPerspectiveMatrix( const Matrix4f& perspectiveMatrix );
        void SetViewToPerspectiveMatrix( const float& fov, const Uint16& width, const Uint16& height,
                                         const float& zNear, const float& zFar );
        void SetPerspectiveToScreenSpaceMatrix( const Matrix4f& screenspaceMatrix );

        // render functions
        void DrawMesh( shared_ptr<Mesh> mesh ); // draws a mesh
        void FillTriangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 ); // draws a triangle
        void FillTriangle( Triangle tris ); // draws a triangle

    private:
        Matrix4f objMatrix, viewMatrix, perspMatrix, screenMatrix;
        Matrix4f objViewMatrix, perspScreenMatrix;
};

#endif // RENDERER_H
