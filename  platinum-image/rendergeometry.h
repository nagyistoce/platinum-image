//////////////////////////////////////////////////////////////////////////
//
//  Rendergeometry $Revision$
///
/// Rendergeometry stores rendering coordinates (look at, orientation
/// and zoom) for renderers
///
//  $LastChangedBy$

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

#ifndef __rendergeometry__
#define __rendergeometry__

#include "ptmath.h"
#include "global.h"
#include "colormap.h"

class image_base;
class curve_base;

class rendergeometry_base 
{
    private:
        static int new_rg_ID;                   //ID counter for new geometry objects

	protected:
        int id;

	public:
        rendergeometry_base();
		int get_id();
		virtual Matrix3D view_to_world_matrix(int vms) const = 0;
		virtual Vector3D get_lookat() const = 0;
		virtual Vector3D get_n(){return create_Vector3D(0,0,1);};// return unit normal vector
};

//-----------------------------------------------------
//-----------------------------------------------------

class rendergeom_image : public rendergeometry_base
{
	public:
        rendergeom_image();

        // *** NOTE: some render parameter constraints (like not zooming in/out at insane levels) are implemented in renderer_base::move

		Vector3D look_at;	//center viewpoint, this will be the middle of the rendering and pivot point for the slice direction.
        Matrix3D dir;       //view to composite image matrix    //only direction (no scaling) = normal of slice plane
        float zoom;			//user-determined magnification, zoom=1 renders 50mm in viewport_x direction (ZOOM_CONSTANT)
					
		void set_dir_matrix_from_preset_direction(preset_direction pd);

		Matrix3D view_to_world_matrix(int viewminsize) const;
		Vector3D get_lookat() const;
        void refresh_viewports();   //refresh viewports using this combination
		
		float distance_to_viewing_plane(Vector3D point);
		float signed_distance_to_viewing_plane(Vector3D point);

		Vector3D get_N();	// return normal vector
		Vector3D get_n();	// return unit normal vector

		Vector3D get_X();	// return global vector for viewport x-dir
		Vector3D get_Y();	// return global vector for viewport y-dir

		line3D get_physical_line_of_intersection(rendergeom_image *rg2);
		line2D get_physical_line_of_intersection_projected(rendergeom_image *rg2);

		Matrix3D get_scan_line_slop_matrix(image_base *the_image_pointer, float rgb_sx_div_by_zoom_constant);
	};

//-----------------------------------------------------
//-----------------------------------------------------


class rendergeom_MIP : public rendergeom_image
{
	public:
        rendergeom_MIP();

		bool use_perspective;

};

class rendergeom_curve : public rendergeometry_base
{
	public:
        rendergeom_curve();
		void set_borders(curve_base *the_curve_pointer, int width, int height); //
		void set_curve(curve_base *the_curve_pointer);
		//int transform(int x, double y, int row_length, int col_length);
		Vector3D view_to_curve(int x_hat, int y_hat, int width, int height);
		Vector3D curve_to_view(int x_hat, double y_hat, int width, int height);
		void get_value(int mouse_x, double* val);
		Vector2D mouse_location;
		Vector2D measure_location;
		float cx, cy;
		float start_y;
		float qx, qy;
		float zoom;
		float x_scale;
		float x_offset;
		curve_base *curve;
		RGBvalue *color;
		RGBvalue *bg;
		Matrix3D view_to_world_matrix(int viewminsize) const; //Dummy function. Needed for interface compatibility
		Vector3D get_lookat() const; //Dummy function. Needed for interface compatibility
		
};

class rendergeom_spectrum: public rendergeometry_base
{
	public:
        rendergeom_spectrum();
		void set_borders(curve_base *the_curve_pointer, char x_type, bool* y_type, int width, int height); //
		void set_curve(curve_base *the_curve_pointer);
		//int transform(int x, double y, int row_length, int col_length);
		Vector3D view_to_curve(int x_hat, int y_hat, int width, int height);
		Vector3D curve_to_view(int x_hat, double y_hat, int width, int height);
		void get_value(int mouse_x, double* val);
		Vector2D mouse_location;
		Vector2D measure_location;
		float cx, cy;
		float start_y;
		float qx, qy;
		float zoom;
		float x_scale;
		float x_offset;
		curve_base *curve;
		RGBvalue *color;
		RGBvalue *bg;
		Matrix3D view_to_world_matrix(int viewminsize) const; //Dummy function. Needed for interface compatibility
		Vector3D get_lookat() const; //Dummy function. Needed for interface compatibility
		
};

#endif