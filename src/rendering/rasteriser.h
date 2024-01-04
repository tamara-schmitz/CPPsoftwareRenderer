#ifndef RASTERISER_H
#define RASTERISER_H

#include "common.h"
#include "types/Edge.h"
#include "types/TexCoordsForEdge.h"
#include "types/Texture.h"
#include "types/Mesh.h"
#include "types/Triangle.h"
#include "types/SafeDeque.h"
#include "types/VertexProcessorObjs.h"

class Rasteriser
{
    // rasterises triangles and blits them onto the screen using a Window object
    //
    // Our fill convention is top-left (so make sure to use ceil!)
    public:
        Rasteriser( shared_ptr< SafeDeque< VPOO > > in, const Uint16& frame_width, const Uint16& frame_height , const Uint16& y_begin, const Uint16& y_end );
        virtual ~Rasteriser();

        shared_ptr< std::thread > ProcessVPOOArrayAsThread() { return make_shared< std::thread >( &Rasteriser::ProcessVPOOArray, this ); }
        void ProcessVPOOArray();

        float near_z = 0, far_z = 0; // contains current near and far plane for culling
        Uint16 y_begin = 0, y_end = 0; // area in which rasteriser is supposed to draw in.
        Uint16 frame_width = 0, frame_height = 0; // area in which rasteriser is supposed to draw in.

        shared_ptr< Texture > r_texture = nullptr;

    private:

        //-- render vars
        std::vector< float > z_buffer;

        void initFramebuffer();
        void finaliseFrame();

        shared_ptr< SafeDeque< VPOO > > in_vpoos = nullptr;
        VPOO current_vpoo;

        float GetZ( const Uint32& index ) const;
        inline float GetZ( const Uint16& x, const Uint16& y ) const { return GetZ( y*r_texture->GetWidth() + x ); }
        void SetZ( const Uint32& index, const float& z_value );
        inline void SetZ( const Uint16& x, const Uint16& y, const float& z_value ) { SetZ( y*r_texture->GetWidth() + x, z_value ); }

        void ProcessCurrentVPOO();
        void ScanTriangle( const Vertexf& vertMin, const Vertexf& vertMid, const Vertexf& vertMax, bool isRightHanded );
        void ScanEdges( Edgef& a, Edgef& b, bool isRightHanded );
        void DrawScanLine( const Edgef& left, const Edgef& right, Uint16 yCoord );
        void DrawFragment( Uint16 x, Uint16 y, float current_depth, Uint16 texcoordX, Uint16 texcoordY );
        void DrawFragment( Uint16 x, Uint16 y, float current_depth );
};

#endif // RASTERISER_H
