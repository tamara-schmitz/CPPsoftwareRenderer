#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "types/Vertex.h"
#include "types/Triangle.h"

#include "tiny_obj_loader.h"

class Mesh
{
    public:
        Mesh();
        Mesh( std::string pathToOBJ ); // imports mesh from OBJ
        virtual ~Mesh();

        // getters
        Triangle GetTriangle( Uint32 index ) const;
        Vertexf GetVertex( Uint32 index ) const { return m_vertices.at(index); }
        Uint32  GetIndex( Uint32 index  ) const { return m_indices.at(index);  }
        Vector3f GetNormal( Uint32 index  ) const { return m_normals.at(index);  }
        Uint32 GetTriangleCount() const { return m_indices.size() / 3; }
        Uint32 GetVertexCount() const { return m_vertices.size(); }
        Uint32 GetIndicesCount() const { return m_indices.size(); }

    protected:

    private:
        std::vector< Vertexf > m_vertices;
        std::vector< Uint32 > m_indices;
        std::vector< Vector3f > m_normals;

        // internal functions
        int64_t OBJindexToNewIndex( const tinyobj::attrib_t& attrib, const tinyobj::index_t& obj_index, shared_ptr< std::unordered_map<int, Uint32> > OBJtoNew_List );

};

#endif // MESH_H
