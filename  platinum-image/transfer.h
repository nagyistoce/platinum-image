//////////////////////////////////////////////////////////////////////////
//
//  Transfer $Revision$
//
//  Transfer function classes
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

#ifndef __transfer__
#define __transfer__

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>

#include "color.h"
#include "colormap.h"
#include "FLTKutilities.h"
#include "points_seq_func1D.h"	//used for spline/linear knots
#include "transferfactory.h"

template <class ELEMTYPE>
class image_storage;

template <class ELEMTYPE, int IMAGEDIM>
class image_complex;

template <class ELEMTYPE>
class histogram_1D;

class transferchart;

template <class ELEMTYPE >
class transfer_interpolated;

#define REDRAWCALLBACKPTYPE image_storage<ELEMTYPE > * 

class transfer_manufactured //! Sub-base class that holds the static factory object
    {
    public:
        static transferfactory factory; //instatiated in transferfactory.cc
    };


//----------------------------- transfer_base --------------------------
class transfer_base: public transfer_manufactured
    {
    protected:
        Fl_Group *pane;
		transfer_base(){};

    public:
		virtual ~transfer_base(){};
        virtual void update(){} //!update t-function controls
    };


//----------------------------- transfer_scalar_base --------------------------
template <class ELEMTYPE >
class transfer_scalar_base: public transfer_base
    {
    protected:
        image_storage<ELEMTYPE > *source;
        transfer_scalar_base(image_storage<ELEMTYPE > *s);
        static void redraw_image_cb( Fl_Widget* o, void* p );//!redraw associated image after touching a control

    public:
        virtual ~transfer_scalar_base();
        virtual void get(const ELEMTYPE v, RGBvalue &p) = 0; //????
    };




template <class ELEMTYPE >
class transfer_default: public transfer_scalar_base <ELEMTYPE >
{
protected:
	Fl_Box *white;
	Fl_Box *black;
public:
	transfer_default(image_storage <ELEMTYPE > *s);

	void get(const ELEMTYPE v, RGBvalue &p);
//	void get(const complex<ELEMTYPE> v, RGBvalue &p){} //????	

	virtual void update();
};

template <class ELEMTYPE >
class transfer_brightnesscontrast: public transfer_scalar_base <ELEMTYPE >
{
protected:
//	Fl_Value_Slider* min_ctrl;
//	Fl_Value_Slider* max_ctrl;
	FLTK_Editable_Slider *min_ctrl;
	FLTK_Editable_Slider *max_ctrl;
	Fl_Slider* brightness_ctrl; //if all the way to the left --> center of transfer function line is at max intensity
	Fl_Slider* contrast_ctrl;	//angle of the transfer function line... (0...pi/2 radians)
	ELEMTYPE min;
	ELEMTYPE max;
	float get_src_int_range();
	float get_transfer_center();
	float get_brightness_slider_value();	
	float get_contrast_slider_value();
public:
	transfer_brightnesscontrast (image_storage <ELEMTYPE > *);
	void get (const ELEMTYPE v, RGBvalue &p);
//	void get(const complex<ELEMTYPE> v, RGBvalue &p){} //????
	void update(string slider_label);				//Updates intensity/contrast parameters from FLTK sliders ...
	static void slider_cb(Fl_Widget *w, void *data);	//slider callback
};

//-------------------

template <class ELEMTYPE >
class transfer_threshold_illustrator: public transfer_scalar_base <ELEMTYPE >
{
protected:
//	Fl_Value_Slider* min_ctrl;
//	Fl_Value_Slider* max_ctrl;
	FLTK_Editable_Slider *min_ctrl;
	FLTK_Editable_Slider *max_ctrl;
public:
	transfer_threshold_illustrator (image_storage <ELEMTYPE > *);
	void get (const ELEMTYPE v, RGBvalue &p);
//	void get(const complex<ELEMTYPE> v, RGBvalue &p){} //????	
//	void update(string slider_label);				//Updates intensity/contrast parameters from FLTK sliders ...
	static void slider_cb(Fl_Widget *o, void *v); //slicer callback
};


template <class ELEMTYPE >
class transfer_mapcolor: public transfer_scalar_base <ELEMTYPE >
{
public:
	transfer_mapcolor (image_storage <ELEMTYPE > * s);
	void get (const ELEMTYPE v, RGBvalue &p);
//	void get(const complex<ELEMTYPE> v, RGBvalue &p){}; //????	
};


template <class ELEMTYPE >
class transfer_interpolated: public transfer_scalar_base <ELEMTYPE >
{
public:

    class transferchart :protected Fl_Widget
    {
	protected:
        friend class			transfer_interpolated;
        unsigned long			lookupSize;
        float					lookupStart, lookupScale, lookupEnd; //redundant, but allows a faster implementation of get(...)
        IMGELEMCOMPTYPE         *lookup ;
        histogram_1D<ELEMTYPE > *histogram;
        IMGELEMCOMPTYPE			*imgdata;
        Fl_RGB_Image			*histimg;	//keeps the histogram background layer
//        points_seq_func1D<float,float>	intensity_knots;		//used for anchor points handling and interpolation 
        points_seq_func1D<float,unsigned char>	intensity_knots;	//used for anchor points handling and interpolation 
																	//knots are of course also wanted for R,G,B, respectively.


        void calc_lookup_params (int newSize = 0);
        virtual void update() = 0;
        transferchart(histogram_1D<ELEMTYPE > *, int, int, int, int);

	public:
		virtual ~transferchart();
        
        void draw();
		int handle(int);
	};

protected:
    transferchart *chart;
    transfer_interpolated(image_storage <ELEMTYPE > * s);

public:
	virtual ~transfer_interpolated();
    void get(const ELEMTYPE v, RGBvalue &p);
//	void get(const complex<ELEMTYPE> v, RGBvalue &p){} //????
    virtual void update();
};


template <class ELEMTYPE >
class transfer_linear: public transfer_interpolated <ELEMTYPE >
    {
    protected:
        class transferchart_linear: public transfer_interpolated<ELEMTYPE >::transferchart
            {
            public:
                transferchart_linear(histogram_1D<ELEMTYPE > * hi, int x, int y, int w, int h);
                virtual void update ();
            };

    public:
        transfer_linear (image_storage <ELEMTYPE > * s);
    };


template <class ELEMTYPE >
class transfer_spline: public transfer_interpolated <ELEMTYPE >
{
    protected:
        class transferchart_spline: public transfer_interpolated<ELEMTYPE >::transferchart
            {
            public:
                transferchart_spline(histogram_1D<ELEMTYPE > * hi, int x, int y, int w, int h);
                virtual void update ();
            };
public:
	transfer_spline (image_storage <ELEMTYPE > * s);
};


template <class ELEMTYPE >
class transfer_scalar_to_RGB_linear: public transfer_scalar_base <ELEMTYPE >
{
protected:
//	Some_FL_color_vector_widget *w;
	colormap map;
public:
	transfer_scalar_to_RGB_linear(image_storage <ELEMTYPE > *);
	void get(const ELEMTYPE v, RGBvalue &p);
//	void get(const complex<ELEMTYPE> v, RGBvalue &p){} //????	
//	void update(string slider_label);				//Updates intensity/contrast parameters from FLTK sliders ...
//	static void slider_cb(Fl_Widget *o, void *v); //slicer callback
};






template <class ELEMTYPE, int IMAGEDIM>
class transfer_complex: public transfer_base
{
protected:
    image_complex<ELEMTYPE,IMAGEDIM> *source;
	Fl_Box *white;
	Fl_Box *black;

public:
	transfer_complex(image_complex<ELEMTYPE,IMAGEDIM> *s);

	void get(const complex<ELEMTYPE> v, RGBvalue &p){
		float ma = source->get_max_float();
		float scale = 255.0/ma;
//		float res = abs(v)*scale;
		float res;
		res = float(v.real())*scale;
		if(res>0){
			p.r( abs(res) );
			p.b( abs(res) );
		}else{
			p.b( abs(res) );
//			p.set_mono(res);
		}
//		p.set_rgb(0,0,100);
	} //????	
	virtual void update();
};


#include "transferfactory.hxx"
#endif