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

#include "rendergeometry.h"

#include "rendermanager.h"
extern rendermanager rendermanagement;


//-----------------------------------------------------
//-----------------------------------------------------
int rendergeometry_base::new_rg_ID=1;

rendergeometry_base::rendergeometry_base()
{
	id=new_rg_ID++;
}

int rendergeometry_base::get_id()
{
    return id;
}


//-----------------------------------------------------
//-----------------------------------------------------

rendergeom_image::rendergeom_image():rendergeometry_base()
{
    look_at.Fill(0);	    //initialize look at to center
    dir.SetIdentity();		//initialize direction
    zoom=1;					//intialize zoom to 100%
}

Matrix3D rendergeom_image::view_to_world_matrix(int viewminsize) const
{
//    return dir*renderer_base::display_scale/(viewminsize*zoom);
	float val;
	Matrix3D ret;
	//ret = dir*ZOOM_CONSTANT;
	ret.SetIdentity();

//	val = viewminsize*zoom*(1.0/ZOOM_CONSTANT);
	val = ZOOM_CONSTANT/(viewminsize*zoom);
	for(int i = 0; i <3 ; i++)
		for(int j = 0; j<3; j++)
			ret[i][j] = dir[i][j]*val;
    //return dir*ZOOM_CONSTANT/(viewminsize*zoom);
	return ret;
}
Vector3D rendergeom_image::get_lookat() const {
	return look_at;
}

void rendergeom_image::refresh_viewports()
{
    rendermanagement.geometry_update_callback(this->id);
}
	
float rendergeom_image::distance_to_viewing_plane(Vector3D point)
{
	Vector3D v;
	v = point - look_at;
	Vector3D n = get_n();
	float distance = abs ( v * n);
	return distance;
}

Vector3D rendergeom_image::get_N()
{
	Vector3D direction = create_Vector3D(0, 0, 1);
	return dir * direction;
}

Vector3D rendergeom_image::get_n()
{
	Vector3D N = get_N();
	return N / N.GetNorm();
}

Vector3D rendergeom_image::get_X()
{
	return dir * create_Vector3D(1, 0, 0);
}

Vector3D rendergeom_image::get_Y()
{
	return dir * create_Vector3D(0, 1, 0);
}

line3D rendergeom_image::get_physical_line_of_intersection(rendergeom_image *rg2)
{
	plane3D p = plane3D(this->look_at,this->get_n());
	plane3D p2 = plane3D(rg2->look_at,rg2->get_n());
	return p.get_line_of_intersection(p2);
}

line2D rendergeom_image::get_physical_line_of_intersection_projected(rendergeom_image *rg2)
{
	line3D l = get_physical_line_of_intersection(rg2);
	plane3D p = plane3D(this->look_at,this->get_n());
	return p.get_projected_line(l);
}

Matrix3D rendergeom_image::get_scan_line_slop_matrix(image_base *the_image_pointer, float rgb_sx_div_by_zoom_constant)
{
	Matrix3D orientation_inv = Matrix3D(the_image_pointer->get_orientation().GetInverse());
	Matrix3D inv_size = Matrix3D(the_image_pointer->get_voxel_resize().GetInverse());
	return (inv_size * orientation_inv)/(this->zoom * rgb_sx_div_by_zoom_constant);
}
//-----------------------------------------------------
//-----------------------------------------------------
rendergeom_curve::rendergeom_curve():rendergeometry_base(){

	cx = cy = 0;
	start_y = 0;
	qx = qy = 1.0;  //Just in case transform is executed before set_borders in some case
	zoom = 1.0;
	curve = NULL;

	//These are for drawing the axis later
	x_offset = 200;
	x_scale = 0.1;
	
	//This is for drawing the vertical line at te mouse pointer location
	mouse_location[0] = 0;
	mouse_location[1] = 0;

	//This is for measure line
	measure_location[0] = -1;
	measure_location[1] = -1;

	color = new RGBvalue();
	color->set_rgb(255,0,0);
}
void rendergeom_curve::set_borders(curve_base *the_curve_pointer, int width, int height){
	double dx, dy;
	dx = (the_curve_pointer->get_data_size());
	dy = (the_curve_pointer->get_max() - the_curve_pointer->get_min() + 1);
	qx = (width/dx);//*zoom;
	qy = (height/dy);//*zoom;
	x_offset = the_curve_pointer->get_offset();
	x_scale = the_curve_pointer->get_scale();

	start_y = the_curve_pointer->get_min();

}
void rendergeom_curve::set_curve(curve_base *the_curve_pointer){
	curve = the_curve_pointer;
}

/*int rendergeom_curve::transform(int x, double y, int row_length, int col_length){
	int rows;
	int addr;
	int x_hat, y_hat;
	x_hat = round(qx*zoom*x) + cx;
	y_hat = round(qy*zoom*y) + cy;
	if(x_hat < 0 ||x_hat > row_length || y_hat < 0 || y_hat > col_length)
		return -1;
	rows = (col_length - y_hat -1);
	addr = rows*row_length + x_hat;
	return addr;
}*/
Vector3D rendergeom_curve::view_to_curve(int x_hat, int y_hat, int width, int height){
	Vector3D val;
	val[0] = round((x_hat - cx)/(qx*zoom));
	val[1] = -((y_hat - cy - height + 1)/(qy*zoom)) + (start_y/qy);
	val[2] = 0;
	return val;
}
Vector3D rendergeom_curve::curve_to_view(int x, double y, int width, int height){
	Vector3D val;
	val[0] = round(qx*zoom*x) + cx;
	val[1] = (height - round(qy*zoom*y) + cy - 1)  + (qy*start_y);
	val[2] = 0;
	return val;
}

void rendergeom_curve::get_value(int x_hat, double* val){

	double y = 0.0;
	int x;
	if(curve == NULL){
		val[0] = -1;
		val[1] = -1;
	}else{
		x = round((x_hat - cx)/(qx*zoom));
		val[1] = 0;
		if(x < curve->get_data_size() && x >= 0){
			val[1] = curve->get_data(x);
		}
		val[0] = x*x_scale + x_offset;
	}
}

Matrix3D rendergeom_curve::view_to_world_matrix(int viewminsize) const
{
	Matrix3D dummy;
	dummy.SetIdentity();
    return dummy;
}
Vector3D rendergeom_curve::get_lookat() const {
	Vector3D dummy;
	dummy.Fill(1);
	return dummy;
}

//-----------------------------------------------------
//-----------------------------------------------------


//-----------------------------------------------------
//-----------------------------------------------------
rendergeom_spectrum::rendergeom_spectrum():rendergeometry_base(){

	cx = cy = 0;
	start_y = 0;
	qx = qy = 1.0;  //Just in case transform is executed before set_borders in some case
	zoom = 1.0;
	curve = NULL;

	//These are for drawing the axis later
	x_offset = 200;
	x_scale = 0.1;
	
	//This is for drawing the vertical line at te mouse pointer location
	mouse_location[0] = 0;
	mouse_location[1] = 0;

	//This is for measure line
	measure_location[0] = -1;
	measure_location[1] = -1;

	color = new RGBvalue();
	color->set_rgb(255,0,0);
}
void rendergeom_spectrum::set_borders(curve_base *the_curve_pointer, bool *y_type, int width, int height){
	double t_max, t_min, max, min;
	double dx, dy;
	dx = (the_curve_pointer->get_data_size());

	min = numeric_limits<double>::max();
	max = -min;
	//enum SPECTRUM_TYPE {SP_REAL, SP_COMPLEX, SP_MAGNITUDE, SP_PHASE, SP_FREQ, SP_TIME};
	if(y_type[0]){
		if((t_max = the_curve_pointer->get_max(SP_REAL)) > max)
			max = t_max;
		if((t_min = the_curve_pointer->get_min(SP_REAL)) < min)
			min = t_min;
	}
	if(y_type[1]){
		if((t_max = the_curve_pointer->get_max(SP_COMPLEX)) > max)
			max = t_max;
		if((t_min = the_curve_pointer->get_min(SP_COMPLEX)) < min)
			min = t_min;
	}
	if(y_type[2]){
		if((t_max = the_curve_pointer->get_max(SP_MAGNITUDE)) > max)
			max = t_max;
		if((t_min = the_curve_pointer->get_min(SP_MAGNITUDE)) < min)
			min = t_min;
	}
	if(y_type[3]){
		if((t_max = the_curve_pointer->get_max(SP_PHASE)) > max)
			max = t_max;
		if((t_min = the_curve_pointer->get_min(SP_PHASE)) < min)
			min = t_min;
	}


	dy = (max - min + 1);
	qx = (width/dx);//*zoom;
	qy = (height/dy);//*zoom;
	x_offset = the_curve_pointer->get_offset(); //Fixa denna som en variabel av x_type
	x_scale = the_curve_pointer->get_scale();

	start_y = min;//the_curve_pointer->get_min();

}
void rendergeom_spectrum::set_curve(curve_base *the_curve_pointer){
	curve = the_curve_pointer;
}
Vector3D rendergeom_spectrum::view_to_curve(int x_hat, int y_hat, int width, int height){
	Vector3D val;
	val[0] = round((x_hat - cx)/(qx*zoom));
	val[1] = -((y_hat - cy - height + 1)/(qy*zoom)) + (start_y/qy);
	val[2] = 0;
	return val;
}
Vector3D rendergeom_spectrum::curve_to_view(int x, double y, int width, int height){
	Vector3D val;
	val[0] = round(qx*zoom*x) + cx;
	val[1] = (height - round(qy*zoom*y) + cy - 1)  + (qy*start_y);
	val[2] = 0;
	return val;
}

void rendergeom_spectrum::get_value(int x_hat, double* val){

	double y = 0.0;
	int x;
	if(curve == NULL){
		val[0] = -1;
		val[1] = -1;
	}else{
		x = round((x_hat - cx)/(qx*zoom));
		val[1] = 0;
		if(x < curve->get_data_size() && x >= 0){
			val[1] = curve->get_data(x);
		}
		val[0] = x*x_scale + x_offset;
	}
}

Matrix3D rendergeom_spectrum::view_to_world_matrix(int viewminsize) const
{
	Matrix3D dummy;
	dummy.SetIdentity();
    return dummy;
}
Vector3D rendergeom_spectrum::get_lookat() const {
	Vector3D dummy;
	dummy.Fill(1);
	return dummy;
}

//-----------------------------------------------------
//-----------------------------------------------------




rendergeom_MIP::rendergeom_MIP():rendergeom_image()
{
   use_perspective=false;
}
