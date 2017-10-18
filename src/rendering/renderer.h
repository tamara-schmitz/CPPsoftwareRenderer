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
#include "rendering/vertexprocessor.h"
#include "rendering/rasteriser.h"
#include "window/window.h"

class Renderer
{
    public:
        Renderer( Window* window, Uint8 vp_thread_count = 2, Uint8 raster_thread_count = 4 );
        virtual ~Renderer();

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
        void WaitUntilFinished();

        // debug rendering functions
        void DrawFarPlane()  { DrawDebugPlane( far_z  ); }
        void DrawNearPlane() { DrawDebugPlane( near_z ); }

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
        std::vector< float > z_buffer_empty;

        std::vector< std::thread > vp_threads;
        std::vector< VertexProcessor > vertex_processors;
        std::vector< Rasteriser > rasterisers;

        shared_ptr< Matrix4f > objMatrix = make_shared< Matrix4f >();
        Matrix4f viewMatrix, perspMatrix, screenMatrix;
        Matrix4f perspScreenMatrix;

        void DrawDebugPlane( float z_value );
};

#endif // RENDERER_H
