//////////////////////////////////////////////////////////////////////////
//
//  curve $Revision:  $
//
//  class for handling of curve objects under data_base
//
//  $LastChangedBy: joel.kullberg $

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

#ifndef __additional_data__
#define __additional_data__

#include <vector>
#include <string>
#include <FL/fl_draw.H>
#include <iostream>
#include "global.h"
#include "rendergeometry.h"

class additional_data_base; 
class renderer_curve;
using namespace std;

class additional_data{	
public:
	additional_data();
	~additional_data();
	//void draw_all_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	void add_point(Vector3D p);
	void add_circle(Vector3D p, Vector3D n, float r, float r1 = -1);
	void add_text(Vector3D p, string text);
	void add_line(Vector3D start, Vector3D stop);
	void add_rect(Vector3D c1, Vector3D c2, Vector3D c3, Vector3D c4);
	void add_gauss(float mean, float std, float amp);
	void add_freehand(vector<Vector3D> p);
	void write_all_data_to_file(string file);
//	image_binary<3>* generate_binary_image(int x, int y, int z);
	void read_all_data_from_file(string file);
	bool show_up, show_down;
	vector<additional_data_base*> data;
private:
	
};
/* -------------------------------------------- */
/* -------------------------------------------- */
class additional_data_base{
public:
	additional_data_base();
	virtual void draw_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){}; 
	//virtual void draw_it(vector<Vector3D> points_to_draw, unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	virtual void calc_data(){};
	virtual void write_data(ofstream &myfile){};
	vector<Vector3D> points_to_draw;
	ADDITIONAL_TYPE type;
	
};
/* -------------------------------------------- */
/* -------------------------------------------- */
class point_data : public additional_data_base{
public:
	point_data(Vector3D p);
	void calc_data();
	//void draw_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	void write_data(ofstream &myfile);
	Vector3D p;
};
/* -------------------------------------------- */
/* -------------------------------------------- */
class circle_data : public additional_data_base{
public:
	circle_data(Vector3D p, Vector3D n, float r, float r1 = -1);
	void calc_data();
	//void draw_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	void write_data(ofstream &myfile);
	Vector3D c;
	Vector3D n;
	float radius, radius1;
};
/* -------------------------------------------- */
/* -------------------------------------------- */
class line_data : public additional_data_base{
public:
	line_data(Vector3D start, Vector3D stop);
	void calc_data();
	//void draw_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	void write_data(ofstream &myfile);
	Vector3D start;
	Vector3D stop;
};
/* -------------------------------------------- */
/* -------------------------------------------- */
class rect_data : public additional_data_base{
public:
	rect_data(Vector3D a, Vector3D b, Vector3D c, Vector3D d);
	void calc_data();
	//void draw_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	void write_data(ofstream &myfile);
	Vector3D c1, c2, c3, c4;
};
/* -------------------------------------------- */
/* -------------------------------------------- */
class text_data : public additional_data_base{
public:
	text_data(Vector3D p, string text);
	//void draw_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	void calc_data();
	void write_data(ofstream &myfile);
	string s;
	Vector3D p;
};

class gauss_data : public additional_data_base{
public:
	gauss_data(float mean, float std, float amplitude);
	//void draw_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	void calc_data();
	void write_data(ofstream &myfile);
	float omega, my, amp;
};

/* -------------------------------------------- */
/* -------------------------------------------- */
class freehand_data : public additional_data_base{
public:
	freehand_data(vector<Vector3D> p);
	void calc_data();
	//void draw_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type);
	void write_data(ofstream &myfile);
	vector<Vector3D> p;
};
	
#endif
