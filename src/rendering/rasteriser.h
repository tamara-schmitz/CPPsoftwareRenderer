#ifndef RASTERISER_H
#define RASTERISER_H

#include "common.h"
#include "types/Edge.h"
#include "types/TexCoordsForEdge.h"
#include "types/Texture.h"
#include "types/Mesh.h"
#include "types/Triangle.h"
#include "types/VertexProcessorObjs.h"
#include "types/SafeDynArray.h"
#include "window/window.h"

class Rasteriser
{
    // rasterises triangles and blits them onto the screen using a Window object
    //
    // Our fill convention is top-left (so make sure to use ceil!)
    public:
        Rasteriser( shared_ptr< SafeDynArray< VPOO > > in, Window* window, shared_ptr< std::vector< float > > z_buffer,
                    const Uint16& y_begin, Uint16& y_end );
        virtual ~Rasteriser();

        void ProcessVPOOArray();

        float near_z, far_z; // contains current near and far plane for culling
        Uint16 y_begin = 0, y_end = 0; // area in which rasteriser is supposed to draw in.

    private:

        //-- render vars
        Window* w_window;

        shared_ptr< SafeDynArray< VPOO > > in_vpoos;
        shared_ptr< std::vector< float > > z_buffer;

        VPOO current_vpoo;

        float GetZ( const Uint32& index ) const;
        float GetZ( const Uint16& x, const Uint16& y ) const { return GetZ( y*w_window->Getwidth() + x ); }
        void SetZ( const Uint32& index, const float& z_value );
        void SetZ( const Uint16& x, const Uint16& y, const float& z_value ) { SetZ( y*w_window->Getwidth() + x, z_value ); }

        void ProcessCurrentVPOO();
        void ScanTriangle( const Vertexf& vertMin, const Vertexf& vertMid, const Vertexf& vertMax, bool isRightHanded );
        void ScanEdges( Edgef& a, Edgef& b, bool isRightHanded );
        void DrawScanLine( const Edgef& left, const Edgef& right, Uint16 yCoord );
        void DrawFragment( Uint16 x, Uint16 y, float current_depth, Uint16 texcoordX, Uint16 texcoordY );
        void DrawFragment( Uint16 x, Uint16 y, float current_depth );
};

#endif // RASTERISER_H
