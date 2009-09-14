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

#include "color.h"

#include <cmath>
#include <iostream>

color_base::color_base()
{}

color_base::color_base(const IMGELEMCOMPTYPE i)
{
    this->set_mono(i);
}

color_base::~color_base()
{}

void color_base::set_rgb(const IMGELEMCOMPTYPE r_, const IMGELEMCOMPTYPE g_, const IMGELEMCOMPTYPE b_)
{
    r(r_);
    g(g_);
    b(b_);
}

void color_base::set_mono (const IMGELEMCOMPTYPE i)
{
    r(i);
    g(i);
    b(i);
}

void color_base::rgb(IMGELEMCOMPTYPE &r_, IMGELEMCOMPTYPE &g_, IMGELEMCOMPTYPE &b_)
{
    r_ = r();
    g_ = g();
    b_ = b();
}


const IMGELEMCOMPTYPE color_base::mono()
{
    return (static_cast<IMGELEMCOMPTYPE>(0.3*r()+0.6*g()+0.1*b()));
}

//----------------------------------------------------

RGBvalue::RGBvalue(): color_base()
{}

RGBvalue::RGBvalue(const IMGELEMCOMPTYPE r_,const IMGELEMCOMPTYPE g_,const IMGELEMCOMPTYPE b_): color_base()
{
    set_rgb(r_,g_,b_);
}

RGBvalue::RGBvalue(const IMGELEMCOMPTYPE i) : color_base (i) {};


RGBvalue::RGBvalue(const IMGELEMCOMPTYPE *p)
{
    set_rgb(p);
}

void RGBvalue::set_rgb(const IMGELEMCOMPTYPE *p)
{
    memcpy(values,p,sizeof(IMGELEMCOMPTYPE)*3);
}

void RGBvalue::set_rgb(const IMGELEMCOMPTYPE r_, const IMGELEMCOMPTYPE g_, const IMGELEMCOMPTYPE b_)
{
    r(r_);
    g(g_);
    b(b_);
}

float RGBvalue::calc_hue_in_degrees()
{
	return calc_hue_in_degrees(r(),g(),b());
}

float RGBvalue::calc_hue_in_degrees(const IMGELEMCOMPTYPE r, const IMGELEMCOMPTYPE g, const IMGELEMCOMPTYPE b)
{
	float h = 0;			//if R == G == B (Max == Min)
	float ma = calc_value(r,g,b);
	float mi = 0;			

	if(r>=g && r>b){			//R == Max
		mi = std::min(g,b);
		float tmp = 60*float(g-b)/(ma-mi) + 360.0; //mod 360..
		h = int(tmp)%360;

	}else if(g>=b && g>r){	//G == Max
		mi = std::min(r,b);
		h = 60.0*float(b-r)/(ma-mi) + 120;

	}else if(b>=r && b>g){	//B == Max
		mi = std::min(r,g);
		h = 60.0*float(r-g)/(ma-mi) + 240;
	}
//	cout<<int(r)<<" "<<int(g)<<" "<<int(b)<<" --> "<<h<<" ("<<ma<<" "<<mi<<")"<<endl;
	return h;
}

float RGBvalue::calc_saturation()
{
	return calc_saturation( r(),g(),b() );
}

float RGBvalue::calc_saturation(const IMGELEMCOMPTYPE r, const IMGELEMCOMPTYPE g, const IMGELEMCOMPTYPE b)
{
	float ma = calc_value(r,g,b);
	float mi = std::min(r,std::min(g,b));
	return (ma-mi)/ma;			
}

IMGELEMCOMPTYPE RGBvalue::calc_value()
{
	return std::max( r(),std::max(g(),b()) );			
}

IMGELEMCOMPTYPE RGBvalue::calc_value(const IMGELEMCOMPTYPE r, const IMGELEMCOMPTYPE g, const IMGELEMCOMPTYPE b)
{
	return std::max(r,std::max(g,b));			
}

//----------------------------------------------------


void RGBAvalue::set_rgba (const IMGELEMCOMPTYPE * p)
{
    memcpy (values,p,sizeof (IMGELEMCOMPTYPE)*4);
}


RGBAvalue::RGBAvalue (const IMGELEMCOMPTYPE r_,const IMGELEMCOMPTYPE g_,const IMGELEMCOMPTYPE b_, IMGELEMCOMPTYPE a_): color_base()
{
    r(r_);
    g(g_);
    b(b_);
    a(a_);
}

RGBAvalue::RGBAvalue (const IMGELEMCOMPTYPE i, const IMGELEMCOMPTYPE a_): color_base(i)
{
    a(a_);
}




//----------------------------------------------------




colornode::colornode():RGBvalue()
{
	position=0;
}

/*
//TODO: ??? 2 overloads have no legal conversion for 'this' pointer...
colornode::colornode(colornode const &c)//:RGBvalue(c.r(),c.g(),c.b()) 
{
	const IMGELEMCOMPTYPE rr = c.r();
//	this->r( c.r() );
//	this->g( c.g() );
//	this->b( c.b() );
	this->position = c.position;
}
*/

colornode::colornode(float pos, IMGELEMCOMPTYPE r, IMGELEMCOMPTYPE g, IMGELEMCOMPTYPE b):RGBvalue(r,g,b)
{
	position = pos;
}