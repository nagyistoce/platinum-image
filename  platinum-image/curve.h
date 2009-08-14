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

#ifndef __curve_base__
#define __curve_base__

#include "data_base.h"
#include "pt_vector.h"

class curve_base : public data_base
{	
	public:
        curve_base(string name);
		virtual ~curve_base();

		virtual void redraw();
		virtual void save_to_DCM_file(const std::string, const bool useCompression = true, const bool anonymize = true){};
		virtual void save_to_VTK_file(const std::string, const bool useCompression = true){};
		virtual double get_data(int i) const = 0;
		virtual int get_data_size() const = 0;
		virtual RGBvalue* get_color() const = 0;
		virtual char get_line() const = 0;
		virtual double get_max() const = 0;
		virtual double get_min() const = 0;
		virtual double get_scale() const = 0;
		virtual double get_offset() const = 0;
		virtual void set_scale(double scale){};
		virtual void set_offset(double offset){}; 
		virtual void set_color(int r, int g, int b){};
		virtual void set_line(char type){};
		virtual void increase_resolution(){};
		virtual Vector2D find_closest_maxima(int location, int direction) const = 0;
		virtual Vector2D find_closest_minima(int location, int direction) const = 0;
		virtual vector<Vector2D> find_maximas_in_intervall(int from, int to) const = 0;
		virtual vector<Vector2D> find_minimas_in_intervall(int from, int to) const = 0;

		static vector<RENDERER_TYPE> get_supported_renderers();

		virtual vector<double> approximate_curve(int degree) const = 0;

		virtual void simplify_curve(){};

		virtual void save_curve_to_file(string s) const = 0;
		virtual void read_curve_from_file(string s){};

		bool modified;

};
	
#endif
