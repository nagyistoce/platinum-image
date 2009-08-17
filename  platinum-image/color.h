//////////////////////////////////////////////////////////////////////////
//
//  Color $Revision$
//
//  Classes storing/manipulating color values
//
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

#ifndef __color__
#define __color__

#include <vector>
#include <complex>

#include "global.h"

// HSL (or HSI) and HSV (or HSB):			
//see http://en.wikipedia.org/wiki/HSL_and_HSV

class color_base
{
    protected:
        color_base();
        color_base(const IMGELEMCOMPTYPE i);   //monochrome constructor
        virtual ~color_base();

    public:
        // *** get functions ***
        /*virtual void hsv(float &h, float &s, float &v) = 0;*/
        virtual void rgb(IMGELEMCOMPTYPE &r_, IMGELEMCOMPTYPE &g_, IMGELEMCOMPTYPE &b_);

        virtual const IMGELEMCOMPTYPE r() = 0;
        virtual const IMGELEMCOMPTYPE g() = 0;
        virtual const IMGELEMCOMPTYPE b() = 0;
        /*virtual const float h() = 0;
        virtual const float s() = 0;
        virtual const float v() = 0;*/
        virtual const IMGELEMCOMPTYPE mono();

        // *** set functions *** 
        /*virtual void set_hsv(const float r,const  float g,const float b) = 0; // convert to HSV */
        virtual void set_rgb(const IMGELEMCOMPTYPE r_, const IMGELEMCOMPTYPE g_, const IMGELEMCOMPTYPE b_);
        virtual void set_mono(const IMGELEMCOMPTYPE);
        virtual void r (const IMGELEMCOMPTYPE) = 0;
        virtual void g (const IMGELEMCOMPTYPE) = 0;
        virtual void b (const IMGELEMCOMPTYPE) = 0;
        /*virtual void h (float) = 0;
        virtual void s (float) = 0;
        virtual void v (float) = 0;*/
    };

class HSVvalue:public color_base
    {
    protected:
        float h,s,v;
    };

/*class RGB_base:public color_base  //abstract base with common parts of RGB and RGBA
    {
    protected:
        RGB_base () {};
        RGB_base (const IMGELEMCOMPTYPE i) : color_base (i) {};
    };*/

class RGBvalue:public color_base
    {
    protected:
        IMGELEMCOMPTYPE values [3];
    public:
        RGBvalue();
        RGBvalue(const IMGELEMCOMPTYPE r_,const IMGELEMCOMPTYPE g_,const IMGELEMCOMPTYPE b_);
        RGBvalue(const IMGELEMCOMPTYPE i);
        RGBvalue(const IMGELEMCOMPTYPE *p); //pixel pointer constructor

        // *** get functions ***
        virtual const IMGELEMCOMPTYPE r(){return values[RADDR];}
        virtual const IMGELEMCOMPTYPE g(){return values[GADDR];}
        virtual const IMGELEMCOMPTYPE b(){return values[BADDR];}
        virtual void write(IMGELEMCOMPTYPE * addr)  //write value at address
            { memcpy (addr,values,sizeof (IMGELEMCOMPTYPE)*3);}    

        // *** set functions ***
        void set_rgb (const IMGELEMCOMPTYPE * p);
        void set_rgb(const IMGELEMCOMPTYPE r_, const IMGELEMCOMPTYPE g_, const IMGELEMCOMPTYPE b_);
        virtual void r(const IMGELEMCOMPTYPE r_){values[RADDR] = r_;}
        virtual void g(const IMGELEMCOMPTYPE g_){values[GADDR] = g_;}
        virtual void b(const IMGELEMCOMPTYPE b_){values[BADDR] = b_;}

        // *** calc functions ***
		float calc_hue_in_degrees(); //0...360
		float calc_hue_in_degrees(const IMGELEMCOMPTYPE r, const IMGELEMCOMPTYPE g, const IMGELEMCOMPTYPE b); //0...360

		float calc_saturation();
		float calc_saturation(const IMGELEMCOMPTYPE r, const IMGELEMCOMPTYPE g, const IMGELEMCOMPTYPE b);

		IMGELEMCOMPTYPE calc_value();
		IMGELEMCOMPTYPE calc_value(const IMGELEMCOMPTYPE r, const IMGELEMCOMPTYPE g, const IMGELEMCOMPTYPE b);

//		template<class E>
			void set_rgb_from_complex(std::complex<float> &c, float magn_min, float magn_max) 
			{
				float grey = (abs(c)-magn_min)/magn_max*255.0;
				set_mono( (IMGELEMCOMPTYPE)grey );
			}
		friend std::ostream &operator<<(std::ostream &ut, const RGBvalue &v)
        { ut << "[color==("<<int(v.values[0])<<" "<<int(v.values[1])<<" "<<int(v.values[2])<<")"; return ut; }

    };

class RGBAvalue:public color_base
    {
    protected:
        IMGELEMCOMPTYPE values [4];
    public:
        RGBAvalue (const IMGELEMCOMPTYPE r_,const IMGELEMCOMPTYPE g_,const IMGELEMCOMPTYPE b_, IMGELEMCOMPTYPE a_ = IMGELEMCOMPMAX);
        RGBAvalue (const IMGELEMCOMPTYPE i,const IMGELEMCOMPTYPE a_ = IMGELEMCOMPMAX); //intensity + alpha constructor
        virtual const IMGELEMCOMPTYPE r()
            {return values[RADDR];}
        virtual const IMGELEMCOMPTYPE g()
            {return values[GADDR];}
        virtual const IMGELEMCOMPTYPE b()
            {return values[BADDR];}
        virtual const IMGELEMCOMPTYPE a()
            {return values[AADDR];}
       
        void write (IMGELEMCOMPTYPE * addr)
            {memcpy (addr,values,sizeof (IMGELEMCOMPTYPE)*4);}

        // *** set functions ***
        void set_rgba (const IMGELEMCOMPTYPE * p);
        virtual void r(const IMGELEMCOMPTYPE r_)
            {values[RADDR] = r_;}
        virtual void g(const IMGELEMCOMPTYPE g_)
            {values[GADDR] = g_;}
        virtual void b(const IMGELEMCOMPTYPE b_)
            {values[BADDR] = b_;} 
        virtual void a(IMGELEMCOMPTYPE a_)
            {values[AADDR] = a_;}
    };

class colornode:public RGBvalue
    {
    protected:

	public:
		colornode();
//		colornode(colornode const &c);
		colornode(float pos, IMGELEMCOMPTYPE r, IMGELEMCOMPTYPE g, IMGELEMCOMPTYPE b);
        float position; // what voxel/pixel/... value this color corresponds to

        // convert from HSV - behövs EFTER interpolation - kunde vara en fristående funktion men passar här
        // dessa behöver "vector" så vi kan sortera arrayen
        // void operator=(const color &k) { }; // DEFAULT OPERATOR OK
        bool operator==(const colornode &k)
            { return position==k.position; }
        bool operator!=(const colornode &k)
            { return position!=k.position; }
        bool operator<(const colornode &k)
            { return position<k.position; }
        bool operator>(const colornode &k)
            { return position>k.position; }
    };

#endif
