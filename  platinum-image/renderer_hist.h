//////////////////////////////////////////////////////////////////////////
//
//   Renderer_curve $Revision: 706 $
///
///  Curve renderer subclass which also renders thumbnails via a static
///  function. Like other possible renderers it also converts between
///  local and global coordinate systems.
///
//   $LastChangedBy: joel.kullberg $

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef __renderer_hist__
#define __renderer_hist__

#include "cursor3D.h"
#include "renderer_base.h"
#include "colormap.h"
#include "datamanager.h"
#include "global.h"

//Complimentary vector functions (TODO: unuse and remove)
//Vector3D mult_elems(const Vector3D &one, const Vector3D &other);
//Vector3D div_elems(const Vector3D &numer, const Vector3D &denom);

class renderer_hist : public renderer_curve_base
    {
    private:

        static void render_(uchar *pixels, int rgb_sx, int rgb_sy, rendergeom_curve *rg, rendercombination *rc);
		static int sgn(long a);
		static void draw_bucket(uchar *pixels, curve_base *the_curve_pointer, rendergeom_curve *rg, int rgb_sx, int rgb_sy, RGBvalue *curve_color, int bucket);
		static void render_additional_data(uchar *pixels, curve_base *the_curve_pointer, rendergeom_curve *rg, int rgb_sx, int rgb_sy, vector<int> col);
		static void draw_line(uchar *pixels, int sx, int sy, int a, int b, int c, int d, std::vector<int> color);
		static void draw_axes(uchar *pixels, curve_base *curve, rendergeom_curve *rg, int width, int height);
	

    public:
		renderer_hist();
        void connect_data(int dataID);	//add image to rendering combination
		static RENDERER_TYPE renderer_type(){return RENDERER_HIST;} //return RENDERER_CURVE

        static void render_thumbnail(unsigned char *rgb, int rgb_sx, int rgb_sy, int image_ID); //Here for future support
		void render_threshold (unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold){}
        void render_position(unsigned char *rgb, int rgb_sx, int rgb_sy);

        virtual Vector3D view_to_voxel(int vx, int vy,int sx,int sy,int imageID = -1) const;
        virtual Vector3D view_to_world(int vx, int vy,int sx,int sy) const;
        
        virtual bool supports_mode (int m);
        
        static const std::string typekey ()
            {return "Histogram renderer";}

		RENDERER_TYPE type() const{
			return renderer_type();
		};
        
        virtual std::string find_typekey() const
            {return typekey();}

			void paint_overlay(int h_offset, int vp_w, int vp_h_pane, bool paint_rendergeometry){}
		
			void move_view(int vsize, int pan_x, int pan_y, int pan_z, float zoom_d){}
			void move(float pan_x, float pan_y, float pan_z){}
			void move_voxels(int x,int y,int z){}
			void look_at(float x, float y, float z){}
			void look_at(float x, float y, float z, float zoom){}
			virtual void rotate_dir(int dx_in_vp_pixels,int dy_in_vp_pixels){}
		
        };

#endif