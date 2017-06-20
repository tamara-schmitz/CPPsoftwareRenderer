#include "Triangle.h"

Triangle::Triangle()
{
    //ctor

    // initialise normal
    normal_vec = Vector4f();

    // create 3 vertices
    verts[0] = Vertexf();
    verts[1] = Vertexf();
    verts[2] = Vertexf();

    // reset each of them to default values
    for ( Vertexf& vv : verts )
    {
        // reset posVec
        vv.posVec.x = vv.posVec.y = vv.posVec.z = vv.posVec.w = 0;
        // reset texVec
        vv.texVec.x = vv.texVec.y = 0;
    }
}

Triangle::Triangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 )
{
    // ctor

    // copy vertices to array
    verts[0] = v1;
    verts[1] = v2;
    verts[2] = v3;
    // calculate normal vector
    calculateTriangeNormal();
}

Triangle::Triangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3, const Vector4f& normal )
{
    // ctor

    // copy vertices to array
    verts[0] = v1;
    verts[1] = v2;
    verts[2] = v3;
    // copy normal vector
    normal_vec = normal;
}

void Triangle::sortVertsByY()
{
    // sorts verts by posVec.y
    if ( verts[2].posVec.y < verts[1].posVec.y )
    {
        std::swap( verts[1], verts[2] );
    }
    if ( verts[1].posVec.y < verts[0].posVec.y )
    {
        std::swap( verts[0], verts[1] );
    }
    if ( verts[2].posVec.y < verts[1].posVec.y )
    {
        std::swap( verts[1], verts[2] );
    }
}

void Triangle::calculateTriangeNormal()
{
    // calculate cross product of 2 vector edges
    normal_vec.x = verts[0].posVec.y * verts[1].posVec.z - verts[0].posVec.z * verts[1].posVec.y;
    normal_vec.y = verts[0].posVec.z * verts[1].posVec.x - verts[0].posVec.x * verts[1].posVec.z;
    normal_vec.z = verts[0].posVec.x * verts[1].posVec.y - verts[0].posVec.y * verts[1].posVec.x;
    // normalise vector
    normal_vec.normalize();
}
Triangle& Triangle::operator*=( const Matrix4f& matrix )
{
    // multiple vertices by matrix
    verts[0].posVec = matrix * verts[0].posVec;
    verts[1].posVec = matrix * verts[1].posVec;
    verts[2].posVec = matrix * verts[2].posVec;

    // recalculate normal vector
    calculateTriangeNormal();

    return *this;
}

Triangle Triangle::operator*( const Matrix4f& matrix )
{
    Triangle result = *this;
    result *= matrix;
    // recalculate normal vector
    result.calculateTriangeNormal();

    return result;
}

Triangle::~Triangle()
{
    //dtor
}
