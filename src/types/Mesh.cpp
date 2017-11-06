#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Mesh::Mesh()
{
    //ctor
}

Mesh::Mesh( std::string pathToOBJ )
{
    //ctor

    // temporary vars
    tinyobj::attrib_t attrib;
    std::vector< tinyobj::shape_t > shapes;
    std::vector< tinyobj::material_t > materials;
    std::string error_msg;
    shared_ptr< std::unordered_map<int, Uint32> > OBJtoNewposindex_transl =
        make_shared< std::unordered_map<int, Uint32> >();  // .at(obj_index) gives corresponding index in m_indices

    // load obj. throw exception on failure
    if ( !tinyobj::LoadObj( &attrib, &shapes, &materials, &error_msg, pathToOBJ.c_str() ) )
    {
        throw std::runtime_error( error_msg );
    }

    // check if OBJ contains normals and texcoords
    bool hasNormals = attrib.normals.size() > 0;
    bool hasTexCoords = attrib.texcoords.size() > 0;

    // iterate over each shape (aka. meshes. multiple will be batched together)
    for ( const auto& shape : shapes )
    {
        // we need 3 indices per triangle. assert if count not multiple of 3
        assert( (shape.mesh.indices.size() % 3) == 0 );

        // iterate over each index
        for ( Uint32 i = 0; i < shape.mesh.indices.size(); i++ )
        {
            const tinyobj::index_t& current_index = shape.mesh.indices.at( i );

            // check if index already exists as a vertex
            int64_t objindex = OBJindexToNewIndex( attrib, current_index, OBJtoNewposindex_transl );
            objindex = -1;
            if ( objindex >= 0 )
            {
                // vertex already exists. hence refer to it in indices.
                m_indices.push_back( (Uint32) objindex );
            }
            else
            {
                // create new vertex from OBJ data
                Vertexf new_vertex = Vertexf();
                new_vertex.posVec.x = attrib.vertices.at( 3 * current_index.vertex_index + 0 );
                new_vertex.posVec.y = attrib.vertices.at( 3 * current_index.vertex_index + 1 );
                new_vertex.posVec.z = attrib.vertices.at( 3 * current_index.vertex_index + 2 );
                new_vertex.posVec.w = 1;
                if ( hasTexCoords )
                {
                    new_vertex.texVec.x = attrib.texcoords.at( 2 * current_index.texcoord_index + 0 );
                    new_vertex.texVec.y = 1.0f - attrib.texcoords.at( 2 * current_index.texcoord_index + 1 );
                }
                // add vertex to m_vertices
                m_vertices.push_back( new_vertex );

                // add new_vertex_index to OBJtoNewposindex_transl
                Uint32 new_vertex_index = m_vertices.size() - 1;
                OBJtoNewposindex_transl->insert( { 3 * current_index.vertex_index, new_vertex_index } );

                // add vertex_id to m_indices
                m_indices.push_back( new_vertex_index );
            }

            // copy / generate normals
            // this have to be done for every triangle not every vertex
            if ( (i + 1) % 3 == 0 && i >= 2 )
            {
                Vector3f normal;
                if ( hasNormals )
                {
                    // create normal vector based on OBJ data
                    normal = Vector3f();
                    normal.x = attrib.normals.at( 3 * current_index.normal_index + 0 );
                    normal.y = attrib.normals.at( 3 * current_index.normal_index + 1 );
                    normal.z = attrib.normals.at( 3 * current_index.normal_index + 2 );
                }
                else
                {
                    normal = calculateNormal( GetVertex( GetIndex( i - 2 ) ).posVec,
                                              GetVertex( GetIndex( i - 1 ) ).posVec );
                }
                // add normal vector to m_normals
                m_normals.push_back( normal );
            }
        }
    }

    // indices count should be a multiple of 3
    assert( (m_indices.size() % 3) == 0 );

    // reduce capacity of vectors
    m_vertices.shrink_to_fit();
    m_indices.shrink_to_fit();
    m_normals.shrink_to_fit();
}

Triangle Mesh::GetTriangle( Uint32 index ) const
{
    return Triangle( GetVertex( GetIndex(3 * index) ), GetVertex( GetIndex(3 * index + 1) ),
                     GetVertex( GetIndex(3 * index + 2) ), GetNormal( index ) );
}

int64_t Mesh::OBJindexToNewIndex( const tinyobj::attrib_t& attrib, const tinyobj::index_t& obj_index, shared_ptr< std::unordered_map<int, Uint32> > OBJtoNew_List )
{
    // checks if obj_index is already part of OBJtoNew_List
    // @return index of vertex in m_vertices. or -1 if no duplicate

    // steps we need to do:
    //  0 check if vertex_index is in OBJtoNewposindex_translation
    //      0.0 if yes check if texcoord is same.
    //          0.0.0 if yes add index from OBJtoNew_List to m_indices
    //          0.0.1 if no goto 1.2.
    //      0.1 if no create new vertex. add new index to m_vertices. add new index to OBJtoNew_List.

    // try looking up index in OBJtoNew_List
    auto new_index_iterator = OBJtoNew_List->find( 3 * obj_index.vertex_index );
    if ( new_index_iterator != OBJtoNew_List->end() )
    {
        // case 0.0
        int new_index = new_index_iterator->second;

        tinyobj::real_t obj_texcoord_x = attrib.texcoords.at( 2 * obj_index.texcoord_index + 0 );
        tinyobj::real_t obj_texcoord_y = attrib.texcoords.at( 2 * obj_index.texcoord_index + 1 );
        Vector2f new_texvec = m_vertices[new_index].texVec;

        if ( AlmostEqual<float>( (float) obj_texcoord_x, new_texvec.x ) &&
             AlmostEqual<float>( (float) obj_texcoord_y, new_texvec.y ) )
        {
            // case 0.0.0
            return new_index;
        }
        else
        {
            // case 0.0.1
            return -1;
        }
    }
    else
    {
        // case 0.1
        return -1;
    }
}

Mesh::~Mesh()
{
    //dtor
    if ( printDebug )
    {
        cout << "Dtor of Mesh object was called!" << endl;
    }
}
