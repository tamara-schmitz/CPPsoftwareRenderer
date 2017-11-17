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
    bool isEmpty = true;

    Matrix4f objMatrix;
    bool drawWithTexture;
    SDL_Color colour;
    shared_ptr< Texture > texture;

    // ctors
    VertexProcessorInputObject()
    {
        isEmpty = false;
        tri = Triangle();

        drawWithTexture = false;
        colour = SDL_Color();
        colour.r = colour.g = colour.b = 200;
        colour.a = SDL_ALPHA_OPAQUE;
    }
    VertexProcessorInputObject( const Triangle& triangle, const Matrix4f& objMatrix, const SDL_Color& colour )
    {
        isEmpty = false;
        tri = triangle;

        this->objMatrix = objMatrix;
        drawWithTexture = false;
        this->colour = colour;
    }
    VertexProcessorInputObject( const Triangle& triangle, const Matrix4f& objMatrix, const shared_ptr< Texture >& texture )
    {
        isEmpty = false;
        tri = triangle;

        this->objMatrix = objMatrix;
        drawWithTexture = true;
        this->texture = texture;
    }
    VertexProcessorInputObject createEmpty()
    {
        isEmpty = true;
        return *this;
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
    SDL_Color colour;
    shared_ptr< Texture > texture;

    // ctors
    VertexProcessorOutputObject()
    {
        tris_verts[0] = tris_verts[1] = tris_verts[2] = Vertexf();
        isRightHanded = false;

        drawWithTexture = false;
        colour = SDL_Color();
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

        drawWithTexture = false;
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

        drawWithTexture = true;
        this->texture = texture;
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