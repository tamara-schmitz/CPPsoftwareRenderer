#ifndef STARFIELD_H
#define STARFIELD_H

#include <SDL2/SDL.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>

class Starfield
{
    public:
        // general values
        const unsigned int numStars;
        float speed;

        // render settings
        unsigned int w_width;
        unsigned int w_height;
        float z_near;
        float z_far;
        float fov;

        //cstor and dstor
        Starfield( unsigned int screen_width, unsigned int screen_height,
                   float camera_near_z, float camera_far_z,
                   float camera_fov,
                   unsigned int numOfStars, float star_speed ) :
                        numStars( numOfStars ), speed( star_speed ),
                        w_width( screen_width ), w_height( screen_height ),
                        z_near( camera_near_z ), z_far( camera_far_z ),
                        fov( camera_fov ) {};
        virtual ~Starfield();

        // structs
        typedef struct
        {
            // a structure that holds information about a star
            float x;
            float y;
            float z;
        } star;
        const star nullStar { 0, 0, 0};
        typedef struct
        {
            // a structure that represents a pixel on a screen
            int x;
            int y;
        } pixel;

        // functions
        std::vector< unsigned char > drawStarfield();

    protected:

    private:

        // save xyz of all stars as a vector
        // coordinate system:
        // from -1 to 1 (x left to right, y top to bottom, z front to behind camera)

        std::vector< star > stars_posi;

        // vars
        float tanHalfFOV = std::tan( ( (fov / 2) * M_PI ) /180 );

        // functions
        void spawnPixel( unsigned int *index );
        pixel toScreenSpace( star *posi );
        void processStar( std::vector< unsigned char > *pixels, unsigned int id );


};

#endif // STARFIELD_H
