#ifndef VERTEXPROCESSOROBJ_H
#define VERTEXPROCESSOROBJ_H

#include "types/Vertex.h"
#include "types/Triangle.h"
#include "types/Texture.h"
#include "SDL2/SDL_types.h"

struct VertexProcessorInputObject
{
    // Stores resulting vertices from vertexprocessor as well as
    // triangle handedness.
    // Vertices are expected to always be sorted by their posVec.y component.

    Triangle tri;

    shared_ptr< Matrix4f > objMatrix;
    bool drawWithTexture;
    shared_ptr< SDL_Color > colour;
    shared_ptr< Texture > texture;

    // ctors
    VertexProcessorInputObject()
    {
        tri = Triangle();

        drawWithTexture = false;
        colour = make_shared< SDL_Color >();
        colour->r = colour->g = colour->b = 200;
        colour->a = SDL_ALPHA_OPAQUE;
    }
    VertexProcessorInputObject( const Triangle& triangle, const shared_ptr< Matrix4f >& objMatrix, const shared_ptr< SDL_Color >& colour )
    {
        tri = triangle;

        this->objMatrix = objMatrix;
        drawWithTexture = false;
        this->colour = colour;
    }
    VertexProcessorInputObject( const Triangle& triangle, const shared_ptr< Matrix4f >& objMatrix, const shared_ptr< Texture >& texture )
    {
        tri = triangle;

        this->objMatrix = objMatrix;
        drawWithTexture = true;
        this->texture = texture;
    }
};

struct VertexProcessorOutputObject
{
    // Stores resulting vertices from vertexprocessor as well as
    // triangle handedness.
    // Vertices are expected to always be sorted by their posVec.y component.

    Vertexf tris_verts[3];
    bool isRightHanded;

    bool drawWithTexture;
    shared_ptr< SDL_Color > colour;
    shared_ptr< Texture > texture;

    // ctors
    VertexProcessorOutputObject()
    {
        tris_verts[0] = tris_verts[1] = tris_verts[2] = Vertexf();
        isRightHanded = false;

        drawWithTexture = false;
        colour = shared_ptr< SDL_Color >( new SDL_Color() );
        colour->r = colour->g = colour->b = 200;
        colour->a = SDL_ALPHA_OPAQUE;
    }
    VertexProcessorOutputObject( const Vertexf& vertMin, const Vertexf& vertMid,
                                 const Vertexf& vertMax, bool isRightHanded, const shared_ptr< SDL_Color >& colour )
    {
        tris_verts[0] = vertMin;
        tris_verts[1] = vertMid;
        tris_verts[2] = vertMax;
        this->isRightHanded = isRightHanded;

        drawWithTexture = false;
        this->colour = colour;
    }
    VertexProcessorOutputObject( const Vertexf& vertMin, const Vertexf& vertMid,
                                 const Vertexf& vertMax, bool isRightHanded, const shared_ptr< Texture >& texture )
    {
        tris_verts[0] = vertMin;
        tris_verts[1] = vertMid;
        tris_verts[2] = vertMax;
        this->isRightHanded = isRightHanded;

        drawWithTexture = true;
        this->texture = texture;
    }
};

typedef VertexProcessorInputObject VPIO;
typedef VertexProcessorOutputObject VPOO;

#endif // VERTEXPROCESSOROBJ_H
