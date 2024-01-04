#ifndef VERTEXPROCESSOROBJ_H
#define VERTEXPROCESSOROBJ_H

#include "types/Vertex.h"
#include "types/Triangle.h"
#include "types/Texture.h"
#include "types/Mesh.h"
#include "SDL2/SDL_types.h"

struct VertexProcessorInputObject
{
    // Stores resulting vertices from vertexprocessor as well as
    // triangle handedness.
    // Vertices are expected to always be sorted by their posVec.y component.

    shared_ptr< Mesh > mesh = nullptr;

    Matrix4f objMatrix = Matrix4f();
    SDL_Color colour = SDL_Color();
    shared_ptr< Texture > texture = nullptr;

    // ctors
    VertexProcessorInputObject()
    {
        colour.r = colour.g = colour.b = 200;
        colour.a = SDL_ALPHA_OPAQUE;
    }
    VertexProcessorInputObject( const VertexProcessorInputObject& vpio)
    {
        mesh = vpio.mesh;
        objMatrix = vpio.objMatrix;
        colour = vpio.colour;
        texture = vpio.texture;
    }
    VertexProcessorInputObject( const Triangle& triangle, const Matrix4f& objMatrix, const SDL_Color& colour )
    {
        mesh = make_shared< Mesh >(triangle);

        this->objMatrix = objMatrix;
        this->colour = colour;
    }
    VertexProcessorInputObject( const Triangle& triangle, const Matrix4f& objMatrix, const shared_ptr< Texture >& texture )
    {
        mesh = make_shared< Mesh >(triangle);

        this->objMatrix = objMatrix;
        this->texture = texture;
    }
    VertexProcessorInputObject( const shared_ptr< Mesh >& mesh, const Matrix4f& objMatrix, const shared_ptr< Texture >& texture )
    {
        this->mesh = mesh;

        this->objMatrix = objMatrix;
        this->texture = texture;
    }

    VertexProcessorInputObject( const shared_ptr< Mesh >& mesh, const Matrix4f& objMatrix, const SDL_Color& colour )
    {
        this->mesh = mesh;

        this->objMatrix = objMatrix;
        this->colour = colour;
    }
};

struct VertexProcessorOutputObject
{
    // Stores resulting vertices from vertexprocessor as well as
    // triangle handedness.
    // Vertices are expected to always be sorted by their posVec.y component.

    Vertexf tris_verts[3] = { Vertexf(), Vertexf(), Vertexf() };
    bool isRightHanded = false;

    SDL_Color colour = SDL_Color();
    shared_ptr< Texture > texture = nullptr;

    // ctors
    VertexProcessorOutputObject()
    {
        colour.r = colour.g = colour.b = 200;
        colour.a = SDL_ALPHA_OPAQUE;
    }
    VertexProcessorOutputObject( const Vertexf& vertMin, const Vertexf& vertMid,
                                 const Vertexf& vertMax, bool isRightHanded, const SDL_Color& colour )
    {
        tris_verts[0] = vertMin;
        tris_verts[1] = vertMid;
        tris_verts[2] = vertMax;
        sortVertsByY();
        this->isRightHanded = isRightHanded;

        this->colour = colour;
    }
    VertexProcessorOutputObject( const Vertexf& vertMin, const Vertexf& vertMid,
                                 const Vertexf& vertMax, bool isRightHanded, const shared_ptr< Texture >& texture )
    {
        tris_verts[0] = vertMin;
        tris_verts[1] = vertMid;
        tris_verts[2] = vertMax;
        sortVertsByY();
        this->isRightHanded = isRightHanded;

        this->texture = texture;
    }

    VertexProcessorOutputObject( const Vertexf& vertMin, const Vertexf& vertMid,
                                 const Vertexf& vertMax, bool isRightHanded,
                                 const shared_ptr< Texture >& texture, const SDL_Color& colour )
    {
        tris_verts[0] = vertMin;
        tris_verts[1] = vertMid;
        tris_verts[2] = vertMax;
        sortVertsByY();
        this->isRightHanded = isRightHanded;

        this->texture = texture;
        this->colour = colour;
    }


    void sortVertsByY()
    {
        // sorts verts by posVec.y
        if ( tris_verts[2].posVec.y < tris_verts[1].posVec.y )
        {
            std::swap( tris_verts[1], tris_verts[2] );
        }
        if ( tris_verts[1].posVec.y < tris_verts[0].posVec.y )
        {
            std::swap( tris_verts[0], tris_verts[1] );
        }
        if ( tris_verts[2].posVec.y < tris_verts[1].posVec.y )
        {
            std::swap( tris_verts[1], tris_verts[2] );
        }
    }
};

typedef VertexProcessorInputObject VPIO;
typedef VertexProcessorOutputObject VPOO;

#endif // VERTEXPROCESSOROBJ_H
