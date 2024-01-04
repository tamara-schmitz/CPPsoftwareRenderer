#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "types/Vertex.h"
#include "types/Triangle.h"
#include "types/Mesh.h"
#include "types/Texture.h"
#include "types/SafeDeque.h"
#include "types/VertexProcessorObjs.h"
#include "rendering/vertexprocessor.h"
#include "rendering/rasteriser.h"
#include "window/window.h"

class Renderer
{
    public:
        Renderer( Window* window, Uint8 vp_thread_count = 1, Uint8 raster_thread_count = 2 );
        virtual ~Renderer();

        // settings
        void SetObjectToWorldMatrix( const Matrix4f& objectMatrix );
	// updating the following 3 matrices is not thread safe and 
	// functions should be called before any data is supplied
        void SetWorldToViewMatrix( const Matrix4f& viewMatrix );
        void SetViewToPerspectiveMatrix( const float& fov, const float& zNear, const float& zFar );
        void SetPerspectiveToScreenSpaceMatrix();
        void SetDrawColour( const SDL_Color& color );
        void SetDrawTexture( const shared_ptr< Texture >& texture );

        // render functions
        void ClearBuffers();
        void DrawMesh( const Matrix4f& objMat, shared_ptr<Mesh> mesh, const SDL_Color& colour);
        void DrawMesh( const Matrix4f& objMat, shared_ptr<Mesh> mesh, shared_ptr< Texture >& texture);
        void DrawMesh( const Matrix4f& objMat, shared_ptr<Mesh> mesh );
        void FillTriangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 );
        void FillTriangle( Triangle tris );
	void InitiateRendering();
        void WaitUntilFinished();

        // debug rendering functions
        void DrawFarPlane()  { DrawDebugPlane( far_z  ); }
        void DrawNearPlane() { DrawDebugPlane( near_z ); }

    private:
        Window* w_window;
        bool drawWithTexture = false; // determines whether a texture or a colour should be drawn
        shared_ptr< SDL_Color > current_colour = make_shared< SDL_Color >();
        shared_ptr< Texture > current_texture = nullptr;

        float near_z = 0;
        float far_z  = 1;

        shared_ptr< SafeDeque< VPIO > > in_vpios;
        shared_ptr< SafeDeque< VPOO > > out_vpoos;

        std::vector< shared_ptr< std::thread > > vp_threads;
        std::vector< shared_ptr< std::thread > > rast_threads;
        std::vector< shared_ptr< VertexProcessor > > vertex_processors;
        std::vector< shared_ptr< Rasteriser > > rasterisers;

        shared_ptr< Matrix4f > objMatrix = make_shared< Matrix4f >();
        Matrix4f viewMatrix = Matrix4f(), perspMatrix = Matrix4f(), screenMatrix = Matrix4f();

        void DrawDebugPlane( float z_value );
};

#endif // RENDERER_H
