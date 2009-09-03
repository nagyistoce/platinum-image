//////////////////////////////////////////////////////////////////////////
//
//   Renderer_base $Revision$
///
///  Renderers produce a 2D viewport pixmap out of images and point sets.
///  The base class has virtual functions for coordinate storage
///  conversion, common to different renderer types.
///
//   $LastChangedBy$

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

#ifndef __renderer_base__
#define __renderer_base__

#include <iostream>
#include <vector>
#include "rendercombination.h"
#include "rendergeometry.h"
#include "datamanager.h"
#include "threshold.h"
#include "global.h"
#include "listedfactory.h"

class image_base;

//enum RENDERER_TYPE {RENDERER_MPR=0, RENDERER_MIP=1, NUM_RENDERER_TYPES, NUM_RENDERER_TYPES_PLUS_END, RENDERER_CURVE = 2};
//const std::string renderer_labels[] = {"MPR renderer"}; //name strings for renderer types


class renderer_base
{

protected:
    int id;
    int rc_id;	// rendercombination id
    int rg_id;	// rendergeometry_base id
	int original_rg_id;
	static int maxrendererID;
    
public:
	renderer_base();
    virtual ~renderer_base();
    
	// ------------- parameters --------------------
    int get_id();
    
    rendercombination *the_rc;				//list of images to render, public, because it is managed by viewport too (make renderer and viewport friends?)
	void connect_combination(rendercombination *rc);
    int combination_id();    

    rendergeometry_base *the_rg;                 //lookat and direction vectors for rendering
	rendergeometry_base *original_rg;
    void connect_geometry(rendergeometry_base *rg);       //attach a certain geometry to this renderer

	void use_other_geometry(rendergeometry_base *geom){ the_rg = geom; rg_id = geom->get_id();};

	int geometry_id() const;
	virtual RENDERER_TYPE type() const = 0;

	// ----- rendering & data interaction ------
	virtual void render_position(unsigned char *rgb, int rgb_sx, int rgb_sy);
    virtual void render_threshold(unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue *threshold);
    virtual std::map<std::string,string> resolve_values_view(int vx, int vy, int sx, int sy) const; //get values from current view, pixel coordinates
    virtual std::map<std::string,string> resolve_values_world(Vector3D worldPos) const;             //get values from composite, unit image coordinates
    
    //convert view coordinates to voxels, virtual since the result depends on what's visible, which in turn depends on how it's rendered
    virtual Vector3D view_to_voxel(int vx, int vy, int sx, int sy, int imageID = -1) const;
    virtual Vector3D view_to_world(int vx, int vy, int sx, int sy) const;
    
    //result is deterministic regardless of what's visible, no virtual:
    std::vector<int> world_to_view(int view_size_x,int view_size_y,const Vector3D wpos) const;
	static std::vector<int> world_to_view(rendergeometry_base *rg,int sx,int sy,const Vector3D wpos);
    static std::vector<float> world_dir_to_view_dir(rendergeom_image *rg,int sx,int sy,const Vector3D w_dir);

    virtual void look_at(float x, float y, float z);
    virtual void look_at(float x, float y, float z, float zoom);
    
    //NOTE: none of the move commands update the rendered image,
    //that should be done once elsewhere
    
//    virtual void move(float pan_x, float pan_y, float pan_z);    //!move in world coordinates
    virtual void move_view(int vsize,int pan_x, int pan_y, int pan_z = 0, float zoom_d = 1);    //!move in view coordinates
    virtual void move_voxels(int,int,int);    //!move in voxels - which image's voxel is a question of definition  

	virtual void rotate_dir(int dx_in_vp_pixels,int dy_in_vp_pixels);
    
    virtual std::string find_typekey() const = 0; //gives name in GUI-lists 
    
    virtual bool supports_mode(int m)
        {return false;}
    
    virtual void connect_data(int dataID) = 0;

	virtual void paint_overlay(int h_offset, int vp_w, int vp_h_pane, bool paint_rendergeometry)=0;

	void fill_rgbimage_with_value(unsigned char *rgb, int rgb_sx, int rgb_sy, int value);
	void fill_rgbimage_with_value(unsigned char *rgb, int x, int y, int w, int h, int rgb_sx, int value);

	//---------------------------
	    // virtual const renderer_base &operator=(const renderer_base &k) { return k; }
    bool virtual operator<<(const renderer_base &k)
        { return id==k.id; }
    bool virtual operator==(const renderer_base &k)
        { return id==k.id; }
    bool virtual operator==(const int &k) { return id==k; }
    bool virtual operator!=(const renderer_base &k)
        { return id!=k.id; }
    bool virtual operator<(const renderer_base &k)
        { return id<k.id; }
    bool virtual operator>(const renderer_base &k)
        { return id>k.id; }
    friend std::istream &operator>>(std::istream &in, renderer_base &k)
        { in >> k.id; return in; }
    friend std::ostream &operator<<(std::ostream &ut, const renderer_base &k)
        { ut << "[renderer_base. ID= " << k.id << "] "; return ut; }

};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class renderer_image_base:public renderer_base
{

protected:
//	virtual rendergeom_image* get_the_rg();

public:
	renderer_image_base();
    virtual ~renderer_image_base();

};

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
class renderer_curve_base:public renderer_base{
public:
	renderer_curve_base();
	virtual ~renderer_curve_base();
};


#endif