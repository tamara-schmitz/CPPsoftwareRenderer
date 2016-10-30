#include "starfield.h"

void Starfield::spawnPixel( unsigned int *index )
{
    // spawns a pixel at random location
    stars_posi[ *index ] = {
        float( ( rand() % 4000 - 2000 ) / 4000.0 ), // x
        float( ( rand() % 4000 - 2000 ) / 4000.0 ), // y
        float( ( rand() % 4000 ) / 4000.0 + z_near) // z
    };

}

Starfield::pixel Starfield::toScreenSpace( star *posi )
{
    float half_width  = w_width  / 2;
    float half_height = w_height / 2;

    int x_pixel = ( posi->x / posi->z ) * half_width  + half_width;
    int y_pixel = ( posi->y / posi->z ) * half_height + half_height;

    //std::cout << "screenspace: " << x_pixel << ", " << y_pixel << std::endl;
    //std::cout << "worldspace: " << posi->x << ", " << posi->y << ", " << posi->z << std::endl;

        return pixel {
            x_pixel,
            y_pixel
        };
}

std::vector< unsigned char > Starfield::drawStarfield()
{
    std::vector< unsigned char > pixels( w_width * w_height * 4 );
    stars_posi.resize( w_width * w_height * 4 );

    pixel pos;
    int offset;

    // loop for every star
    for ( unsigned int i = 0; i < numStars; i++ )
    {
        star *current_star = &stars_posi[i];
        float *current_z = &current_star->z;
        // check if empty
        if ( current_star == nullptr )
        {
            spawnPixel( &i );
        }

        // check if outside view bounds
        if ( *current_z < z_near )
        {
            spawnPixel( &i );
        }
        else
        {
            // transform z of star
            *current_z -= 0.01f * speed;
        }

        // to screen space
        pos = toScreenSpace( current_star );

        // finally draw pixel!
        // first check if pixel inside screen rect
        if ( ( pos.x >= 0 && pos.x < (int) w_width  ) &&
             ( pos.y >= 0 && pos.y < (int) w_height ) &&
             ( *current_z > z_near && *current_z < z_far ) )
        {
            offset = pos.y * 4 * w_width + pos.x * 4;
            // hardcoded white
            pixels[ offset + 0] = 255; // b
            pixels[ offset + 1] = 255; // g
            pixels[ offset + 2] = 255; // r
            pixels[ offset + 3] = SDL_ALPHA_OPAQUE; // a
        }

        /*
        // debug star print
        if ( i == 5 )
        {
            std::cout << "Star worldpos : " << current_star->x << ", " << current_star->y << ", " << *current_z << std::endl;
            std::cout << "Star screenpos: " << pos.x << ", " << pos.y << std::endl;
        }
        */
    }

    return pixels;
}

Starfield::~Starfield()
{
    //dtor
}
