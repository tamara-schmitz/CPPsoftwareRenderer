#ifndef MESH_H
#define MESH_H

#include "common.h"
#include "Vertex.h"

#include "tiny_obj_loader.h"

class Mesh
{
    public:
        Mesh();
        Mesh( std::string pathToOBJ ); // imports mesh from OBJ
        virtual ~Mesh();

        // getters
        Vertexf GetVertex( Uint32 index ) const { return m_vertices.at(index); }
        Uint32  GetIndex( Uint32 index  ) const { return m_indices.at(index);  }
        Uint32 GetVertexCount() const { return m_vertices.size(); }
        Uint32 GetIndicesCount() const { return m_indices.size(); }

    protected:

    private:
        std::vector< Vertexf > m_vertices;
        std::vector< Uint32 > m_indices;

        // internal functions
        int OBJindexToNewIndex( const tinyobj::attrib_t& attrib, const tinyobj::index_t& obj_index, shared_ptr< std::unordered_map<int, Uint32> > OBJtoNew_List );

};

#endif // MESH_H
