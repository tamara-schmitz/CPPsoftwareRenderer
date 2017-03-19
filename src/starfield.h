#ifndef STARFIELD_H
#define STARFIELD_H

#include <SDL2/SDL.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>
#include "window.h"
#include "typedefs.h"

class Starfield
{
    public:
        // general values
        unsigned int numStars;
        float speed;

        //cstor and dstor
        Starfield( float camera_near_z, float camera_far_z,
                   float camera_fov, Window *window,
                   unsigned int numOfStars, float star_speed );
        virtual ~Starfield();

        // structs
        typedef struct
        {
            // a structure that holds information about a star
            float x;
            float y;
            float z;
        } star;

        // functions
        //std::vector< unsigned char > drawStarfield();
        void drawStarfield();

    protected:

    private:
        // window and render vars
        unsigned int w_width;
        unsigned int w_height;
        float z_near;
        float z_far;
        float fov;
        Window *w_window;

        // save xyz of all stars as a vector
        // coordinate system:
        // from -1 to 1 (x left to right, y top to bottom, z front to behind camera)

        std::vector< star > stars_posi;
        std::vector< float > z_buffer;
        std::vector< float > null_z_buffer;

        // vars
        float tanHalfFOV;

        // functions
        void clearZBuffer();
        void spawnStar( unsigned int index );
        pixel toScreenSpace( star *posi );
        void processStar( unsigned int id );


};

#endif // STARFIELD_H
