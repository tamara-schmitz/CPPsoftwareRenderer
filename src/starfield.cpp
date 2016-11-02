#include "starfield.h"
#include <unistd.h>
#include <omp.h>

Starfield::Starfield( float camera_near_z, float camera_far_z,
                      float camera_fov, Window *window,
                      unsigned int numOfStars, float star_speed )
{
    numStars = numOfStars;
    speed    = star_speed;
    w_width  = window->Getwidth();
    w_height = window->Getheight();
    z_near   = camera_near_z;
    z_far    = camera_far_z;
    fov      = camera_fov;
    w_window = window;

    tanHalfFOV = std::tan( ( (fov / 2) * M_PI ) /180 );

    stars_posi.resize( numStars );
    z_buffer.resize( w_width * w_height );
    clearZBuffer();

    std::cout << "Starfield initialised!" << std::endl;
}

void Starfield::clearZBuffer()
{
    z_buffer.clear();
    z_buffer.resize( w_width * w_height );
    for ( unsigned int i = 0; i < w_width * w_height; i++ )
    {
        z_buffer[ i ] = z_far + 1;
    }
}

void Starfield::spawnStar( unsigned int *index )
{
    // spawns a pixel at random location
    stars_posi[ *index ] = {
        float( ( rand() % 4000 - 2000.0 ) / 4000.0 ), // x
        float( ( rand() % 4000 - 2000.0 ) / 4000.0 ), // y
        float( ((rand() % 4000 ) / 4000.0) + z_far ) // z
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

void Starfield::processStar( unsigned int id )
{
    star *current_star = &stars_posi[id];
    // check if empty
    if ( &current_star->z == nullptr )
    {
        spawnStar( &id );
    }

    // check if outside view bounds
    if ( current_star->z < z_near )
    {
        spawnStar( &id );
    }
    else
    {
        // transform z of star
        current_star->z -= 0.01f * speed;
    }

    // to screen space
    pixel pos = toScreenSpace( current_star );

    // finally draw pixel!
    // first check if pixel visible
    if ( pos.x >= 0 && pos.x < (int) w_width  &&
         pos.y >= 0 && pos.y < (int) w_height &&
         current_star->z > z_near && current_star->z < z_far )
    {
        // pixel offset
        int offset = pos.y * w_width + pos.x;

        // do depth test
        if ( current_star->z < z_buffer[ offset ] )
        {
            // set new z
            z_buffer[ offset ] = current_star->z;

            // z based color strength
            Uint8 pixelColor = (1 - ( (current_star->z - z_near) / (z_far - z_near) ) ) * 255.0;
            SDL_Color color;
            color.a = SDL_ALPHA_OPAQUE;
            color.b = pixelColor;
            color.g = pixelColor;
            color.r = pixelColor;
            w_window->drawPixel( pos.x, pos.y, color );
        }

    }

    /*
    // debug star print
    if ( id == 0 )
    {
        int offset = pos.y * w_width + pos.x;
        std::cout << "Star worldpos : " << current_star->x << ", " << current_star->y << ", " << current_star->z << std::endl;
        std::cout << "Star screenpos: " << pos.x << ", " << pos.y << std::endl;
        std::cout << "Star z: " << current_star->z << " Buffer z: " << z_buffer[ offset ] << std::endl;
    }
    */

}

//std::vector< unsigned char > Starfield::drawStarfield()
void Starfield::drawStarfield()
{
    clearZBuffer();

    // loop for every star
    for ( unsigned int i = 0; i < numStars; i++ )
    {
        processStar( i );
    }
}

Starfield::~Starfield()
{
    //dtor
    z_buffer.clear();
    stars_posi.clear();
}
