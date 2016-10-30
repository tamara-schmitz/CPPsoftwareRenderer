#include "starfield.h"

void Starfield::spawnPixel( unsigned int *index )
{
    // spawns a pixel at random location
    stars_posi[ *index ] = {
        float( float( rand() ) * 3.0f - 1.0f ), // x
        float( float( rand() ) * 3.0f - 1.0f ), // y
        float( float( rand() ) )  // z
    };

}

Starfield::pixel Starfield::toScreenSpace( star *posi )
{
    unsigned int half_width  = w_width  / 2;
    unsigned int half_height = w_height / 2;

    int x_pixel = posi->x * ( half_width  ) *  + ( half_width  );
    int y_pixel = posi->y * ( half_height ) *  + ( half_height );

    /*
    if ( ( posi.z < z_near || posi.z < z_far  ) || // if z before or in front of camera
         ( x_pixel < 0 || x_pixel > w_width  ) || // if x outside of screen
         ( y_pixel < 0 || y_pixel > w_height ) )  // if y outside of screen
    {
        return NULL;
    }
    else
    {
    */
        return pixel {
            x_pixel,
            y_pixel
        };
    //}
}

std::vector< unsigned char > Starfield::drawStarfield()
{
    std::vector< unsigned char > pixels( w_width * w_height * 4 );

    // loop for every star
    for ( unsigned int i = 0; i < numStars; i++ )
    {
        pixel pos;
        star &current_star = stars_posi[i];
        float &current_z = current_star->z;
        // check if empty or behind camera

        /*
        if ( *current_star == NULL )
        {
            spawnPixel( i );
        }
        else */
        if ( *current_z < z_near || *current_z > z_far )
        {
            spawnPixel( &i );
        }
        else
        {
            // transform z of star
            current_z -= 0.01f * speed;
        }

        // finally add star to pixel array
        pos = toScreenSpace( current_star );
        // hardcoded white
        pixels[ i + 0] = 255; // b
        pixels[ i + 1] = 255; // g
        pixels[ i + 2] = 255; // r
        pixels[ i + 3] = SDL_ALPHA_OPAQUE; // a

    }

    return pixels;
}

Starfield::~Starfield()
{
    //dtor
}
