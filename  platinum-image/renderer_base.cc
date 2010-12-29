// $Id$

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

#include "renderer_base.h"

int renderer_base::maxrendererID = 0;

using namespace std;

renderer_base::renderer_base()
{
    maxrendererID++;
    id=maxrendererID;

    the_rc=NULL;
    rc_id=0;

    the_rg=NULL;
    rg_id=0;

	original_rg=NULL;
    original_rg_id=0;
}

renderer_base::~renderer_base() {
	if(the_rc!=NULL){
		delete the_rc;
	}
	if(the_rg != NULL){
		delete the_rg;
	}
	if(original_rg!=NULL){
		delete original_rg;
	}
}


int renderer_base::get_id()
{
	return id; 
}

void renderer_base::connect_combination(rendercombination *rc)
 {
	the_rc=rc;
    rc_id=rc->get_id();
}

int renderer_base::combination_id()
{
    return rc_id;
}

void renderer_base::connect_geometry(rendergeometry_base *rg)
{
    the_rg=rg;
    rg_id=rg->get_id();
	original_rg = rg;
	original_rg_id = rg_id;
}

void renderer_base::use_other_geometry(rendergeometry_base *geom)
{
	the_rg = geom; 
	rg_id = geom->get_id();
}

bool renderer_base::is_my_geom(int id)
{
	return original_rg_id == id;
}

int renderer_base::geometry_id() const
{
	return rg_id;
}


void renderer_base::render_position(unsigned char *rgb, int rgb_sx, int rgb_sy)
{
	pt_error::error("Calling undefined render_position(...)",pt_error::warning);
}

void renderer_base::render_threshold(unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold)
{
	pt_error::error("Calling undefined render_threshold(...)",pt_error::warning);
}



void renderer_base::look_at(float x, float y, float z)
{}

void renderer_base::look_at(float x, float y, float z, float zoom)
{}

/*
void renderer_base::move( float pan_x, float pan_y, float pan_z)
{
    Vector3D pan = CreateVector3D(pan_x, pan_y, pan_z);
//    pan.Fill(0);
  //  pan[0]=pan_x;
    //pan[1]=pan_y;
   // pan[2]=pan_z;
    
    the_rg->look_at+=pan;
  
}
*/

void renderer_base::move_view(int vsize, int pan_x, int pan_y, int pan_z, float zoom_d)
{}


void renderer_base::move_voxels(int x,int y,int z)
{}


void renderer_base::rotate_dir(int dx_in_vp_pixels, int dy_in_vp_pixels)
{}

std::vector<int> renderer_base::world_to_view(rendergeometry_base *g, int sx, int sy, const Vector3D wpos)
{
    std::vector<int> view;
    Vector3D toView = wpos;
//    int vmin = std::min(sx,sy);
    int vmin = sx; //JK6
    //float wtvCenterScale = renderer_base::display_scale/((float)vmin*g->zoom*2);
   
    Matrix3D world_to_view_matrix;
    world_to_view_matrix = g->view_to_world_matrix(vmin).GetInverse();
    
   // toView = world_to_view_matrix * (toView - g->look_at);
    toView = world_to_view_matrix * (toView - g->get_lookat());

    view.push_back(round(toView[0]+sx/2.0));
    view.push_back(round(toView[1]+sy/2.0));

//	cout<<"world_to_view(sx="<<sx<<" sy="<<sy<<" wpos="<<wpos<<") --"<<view[0]<<" "<<view[1]<<endl;

    return view;
}

std::vector<float> renderer_base::world_dir_to_view_dir(rendergeom_image *rg,int sx,int sy,const Vector3D w_dir)
{
    std::vector<float> view;
   
    Matrix3D world_to_view_matrix;
//	int vmin = std::min (sx,sy);
//  world_to_view_matrix = rg->view_to_world_matrix(vmin).GetInverse();
    world_to_view_matrix = rg->view_to_world_matrix(sx).GetInverse(); //JK6 zoom redefinition
    
    Vector3D toView = world_to_view_matrix * (w_dir);
    	
    view.push_back(toView[0]);
    view.push_back(toView[1]);
    return view;
}


std::vector<int> renderer_base::world_to_view(int sx,int sy,const Vector3D wpos) const
{
	return world_to_view(the_rg,sx,sy,wpos);
    //return world_to_view((rendergeom_image*)the_rg,sx,sy,wpos);
}

std::map<std::string,string> renderer_base::resolve_values_view(int vx, int vy, int sx, int sy) const 
{
    //virtual function, MSVC gets hickups without namespace spec however :(
    return renderer_base::resolve_values_world(view_to_world(vx,vy,sx,sy));
}

std::map<std::string,string> renderer_base::resolve_values_world(Vector3D worldPos) const
{
    std::map<std::string,string> m;
	    for(rendercombination::iterator itr = the_rc->begin(); itr != the_rc->end();itr++){
			if(itr->pointer != NULL){
				m[itr->pointer->name()] = itr->pointer->resolve_value_world(worldPos);
			}
		}
    return m;    
}


Vector3D renderer_base::view_to_voxel(int vx, int vy, int sx, int sy, int imageID) const
{
	pt_error::error("Calling undefined view_to_voxel(...)",pt_error::warning);
	Vector3D tmp;
	return tmp;	//Corrects error in Visual C++ compilation... ("must return a value")
}

Vector3D renderer_base::view_to_world(int vx, int vy, int sx, int sy) const 
{
	pt_error::error("Calling undefined view_to_world(...)",pt_error::warning);
	Vector3D tmp;
	return tmp;	//Corrects error in Visual C++ compilation... ("must return a value")
}


void renderer_base::fill_rgbimage_with_value(unsigned char *rgb, int rgb_sx, int rgb_sy, int value)
{
	int tmp=0;
    for(int y=0; y<rgb_sy; y++){
	    for(int x=0; x<rgb_sx; x++){
			tmp = RGB_pixmap_bpp*(x+rgb_sx*y);
            rgb[tmp] = value;
            rgb[tmp+1] = value;
            rgb[tmp+2] = value;
        }
    }
}

void renderer_base::fill_rgbimage_with_value(unsigned char *rgb, int x, int y, int w, int h, int rgb_sx, int value)
{
	int tmp=0;
    for (int rgb_y=y; rgb_y < y+h; rgb_y++){
        for (int rgb_x=x; rgb_x < x+w; rgb_x++){
			tmp = RGB_pixmap_bpp*(rgb_x+rgb_sx*rgb_y);
            rgb[tmp] = value;
            rgb[tmp+1] = value;
            rgb[tmp+2] = value;
        }
    }
}



//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------



renderer_image_base::renderer_image_base():renderer_base()
{
}


renderer_image_base::~renderer_image_base()
{}
/*
rendergeom_image* renderer_image_base::get_the_rg()
{
	rendergeom_image *ret = (rendergeom_image*)this->the_rg;
	return ret;
}
*/

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

renderer_curve_base::renderer_curve_base():renderer_base(){

}

renderer_curve_base::~renderer_curve_base(){

}
