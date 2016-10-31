#include "starfield.h"
#include <unistd.h>
#include <omp.h>

void Starfield::spawnPixel( unsigned int *index )
{
    // spawns a pixel at random location
    stars_posi[ *index ] = {
        float( ( rand() % 4000 - 2000 ) / 4000.0 ), // x
        float( ( rand() % 4000 - 2000 ) / 4000.0 ), // y
        float( ((rand() % 1000 ) / 4000.0) + z_far ) // z
    };

}

Starfield::pixel Starfield::toScreenSpace( star *posi )
{
    float half_width  = w_width  / 2;
    float half_height = w_height / 2;

    int x_pixel = ( posi->x / (posi->z*tanHalfFOV) ) * half_width  + half_width;
    int y_pixel = ( posi->y / (posi->z*tanHalfFOV) ) * half_height + half_height;

    // debug print
    //std::cout << "screenspace: " << x_pixel << ", " << y_pixel << std::endl;
    //std::cout << "worldspace: " << posi->x << ", " << posi->y << ", " << posi->z << std::endl;

        return pixel {
            x_pixel,
            y_pixel
        };
}

void Starfield::processStar( std::vector< unsigned char > *pixels, unsigned int id )
{
    star *current_star = &stars_posi[id];
    // check if empty
    if ( &current_star->z == nullptr )
    {
        spawnPixel( &id );
    }

    // check if outside view bounds
    if ( current_star->z < z_near )
    {
        spawnPixel( &id );
    }
    else
    {
        // transform z of star
        current_star->z -= 0.01f * speed;
    }

    // to screen space
    pixel pos = toScreenSpace( current_star );

    // finally draw pixel!
    // first check if pixel inside screen rect
    if ( pos.x >= 0 && pos.x < (int) w_width  &&
         pos.y >= 0 && pos.y < (int) w_height &&
         current_star->z > z_near && current_star->z < z_far )
    {
        int offset = pos.y * 4 * w_width + pos.x * 4;
        /*
        // hardcoded white
        (*pixels)[ offset + 0] = (unsigned char) 255; // b
        (*pixels)[ offset + 1] = (unsigned char) 255; // g
        (*pixels)[ offset + 2] = (unsigned char) 255; // r
        (*pixels)[ offset + 3] = SDL_ALPHA_OPAQUE; // a
        */

        // z based color strength
        unsigned char pixelColor = (1 - ( (current_star->z - z_near) / (z_far - z_near) ) ) * 255.0;
        (*pixels)[ offset + 0] = (unsigned char) pixelColor; // b
        (*pixels)[ offset + 1] = (unsigned char) pixelColor; // g
        (*pixels)[ offset + 2] = (unsigned char) pixelColor; // r
        (*pixels)[ offset + 3] = SDL_ALPHA_OPAQUE; // a
    }


    /*
    // debug star print
    if ( id == 0 )
    {
        std::cout << "Star worldpos : " << current_star->x << ", " << current_star->y << ", " << current_star->z << std::endl;
        std::cout << "Star screenpos: " << pos.x << ", " << pos.y << std::endl;
    }
    */
}

std::vector< unsigned char > Starfield::drawStarfield()
{
    std::vector< unsigned char > pixels( w_width * w_height * 4 );
    stars_posi.resize( numStars );

    // loop for every star
    #pragma omp parallel for
    for ( unsigned int i = 0; i < numStars; i++ )
    {
        processStar( &pixels, i);



    }

    return pixels;
}

Starfield::~Starfield()
{
    //dtor
}
