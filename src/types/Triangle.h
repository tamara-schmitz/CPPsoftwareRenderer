#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "common.h"
#include "types/Vertex.h"

class Triangle
{
    public:
        Triangle(); // init triangle with "empty" vertices
        Triangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3 ); // copy 3 vertices
        Triangle( const Vertexf& v1, const Vertexf& v2, const Vertexf& v3, const Vector3f& normal ); // copy 3 vertices
        virtual ~Triangle();

        // variables
        Vertexf verts [3];
        Vector3f normal_vec;

        // modifier functions
        void sortVertsByY(); // after completion: 0 ^= yMinVert, 1 ^= yMidVert, 2 ^= yMaxVert
        void calculateTriangleNormal(); // calculates normal vector and stores result in normal_vec

        // operator overloads
        Triangle& operator*=( const Matrix4f& matrix );
        Triangle operator*( const Matrix4f& matrix );
        Triangle& operator+=( const Vector4f& vec );
        Triangle operator+( const Vector4f& vec );
        Triangle& operator-=( const Vector4f& vec );
        Triangle operator-( const Vector4f& vec );
};

#endif // TRIANGLE_H
