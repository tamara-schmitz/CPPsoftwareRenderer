#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <math.h>

// this files contains some typedefs used throughout the program

struct vertex2D
{
    // a structure that holds a 2D vertex
    float x;
    float y;


};

// vertex3d
struct vertex3D
{
    // a structure that holds a 3D vertex
    float x;
    float y;
    float z;
};

// vector2d
struct vector2D
{
    // a structure that represents a 2D vector
    // -vars
    float x;
    float y;

    // -functions
    vector2D& operator= ( const vector2D &a )
    {
        x = a.x;
        y = a.y;
        return *this;
    }
    vector2D& operator= ( const float &a )
    {
        x = a;
        y = a;
        return *this;
    }
    vector2D& operator+ ( const vector2D &a )
    {
        x += a.x;
        y += a.y;
        return *this;
    }
    vector2D& operator+ ( const float &a )
    {
        x += a;
        y += a;
        return *this;
    }
    vector2D& operator- ( const vector2D &a )
    {
        x -= a.x;
        y -= a.y;
        return *this;
    }
    vector2D& operator- ( const float &a )
    {
        x -= a;
        y -= a;
        return *this;
    }
    vector2D& operator* ( const vector2D &a )
    {
        x *= a.x;
        y *= a.y;
        return *this;
    }
    vector2D& operator* ( const float &a )
    {
        x *= a;
        y *= a;
        return *this;
    }
    vector2D& operator/ ( const vector2D &a )
    {
        x /= a.x;
        y /= a.y;
        return *this;
    }
    vector2D& operator/ ( const float &a )
    {
        x /= a;
        y /= a;
        return *this;
    }
    bool operator== ( const vector2D &a ) const
    {
        return ( x == a.x && y == a.y );
    }
    float length()
    {
        return std::sqrt( x*x + y*y );
    }
    float max()
    {
        return std::max( x, y );
    }
    float dot( vector2D &v )
    {
        return x * v.x + y * v.y;
    }
    vector2D normalised()
    {
        return { x / length(), y / length() };
    }
    float cross( vector2D &v )
    {
        return x * v.y - y * v.x;
    }
    vector2D lerp( vector2D dest, float lerpFactor )
    {
        return (dest - *this) * lerpFactor + *this;
    }
    vector2D rotate( float &angle )
    {
        double rad = (angle * M_PI ) / 180;
        double cos = std::cos( rad );
        double sin = std::sin( rad );

        x = x*cos - y*sin;
        y = x*sin + y*cos;
        return *this;
    }
};

#endif // TYPEDEFS_H
