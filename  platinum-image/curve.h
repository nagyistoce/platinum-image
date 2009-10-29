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
#include <vnl/algo/vnl_fft_1d.h>
#include <vxl/vcl/vcl_complex.h>

class curve_base : public data_base
{	
	public:
        curve_base(string name, bool real);
		virtual ~curve_base();

		virtual void redraw();
		virtual void save_to_DCM_file(const std::string, const bool useCompression = true, const bool anonymize = true){};
		virtual void save_to_VTK_file(const std::string, const bool useCompression = true){};
		
		virtual double get_data(int i) const{return -1;};
		virtual int get_data_size() const = 0;
		virtual RGBvalue* get_color() const = 0;
		virtual char get_line() const = 0;
		virtual double get_max(SPECTRUM_TYPE curve = SP_TIME, SPECTRUM_TYPE t = SP_REAL) {return -1;};
		virtual double get_min(SPECTRUM_TYPE curve = SP_TIME, SPECTRUM_TYPE t = SP_REAL) {return -1;};
		virtual double get_scale() const = 0;
		virtual double get_offset() const = 0;
		virtual void set_scale(double scale){};
		virtual void set_offset(double offset){}; 
		virtual void set_color(int r, int g, int b){};
		virtual void set_line(char type){};
		virtual void increase_resolution(){};
		virtual Vector2D find_closest_maxima(int location, int direction) const {Vector2D d;return d;};
		virtual Vector2D find_closest_minima(int location, int direction) const {Vector2D d;return d;};
		virtual vector<Vector2D> find_maximas_in_intervall(int from, int to) const {vector<Vector2D> d; return d;};
		virtual vector<Vector2D> find_minimas_in_intervall(int from, int to) const {vector<Vector2D> d; return d;};

		static vector<RENDERER_TYPE> get_supported_renderers();

		virtual vector<double> approximate_curve(int degree) {vector<double> d; return d;};

		virtual void simplify_curve(){};

		virtual void save_curve_to_file(string s) {};
		virtual void read_curve_from_file(string s){};
		void helper_data_to_binary_image(){cout << "curve_base has not implemented this yet" << endl;};

		bool modified;

};

class curve_complex_base : public curve_base
{	
	public:
        curve_complex_base(string name);
		virtual ~curve_complex_base();

		virtual void save_to_DCM_file(const std::string, const bool useCompression = true, const bool anonymize = true){};
		virtual void save_to_VTK_file(const std::string, const bool useCompression = true){};
		
		virtual double get_data(int i) const{return -1;};

		virtual double get_real(int i, SPECTRUM_TYPE t) const{return -1;};
		virtual double get_imag(int i, SPECTRUM_TYPE t) const{return -1;};
		virtual double get_magnitude(int i, SPECTRUM_TYPE t) const{return -1;};
		virtual double get_phase(int i, SPECTRUM_TYPE t) const{return -1;};
		virtual vector<complex<double> > get_frequency_vector(){vector<complex<double> > d; return d;};

		virtual void set_data(int i, complex<double> val, bool recalc = true){};//automatically sets the data in the base_type vector and recalculates the other vector
		//virtual void set_active_curve(char curve);
		//virtual void set_time_shift(double shift);
		//virtual void set_phase_shift(double shift);
		
		//virtual double get_time_shift();
		//virtual double get_phase_shift();

		//virtual char get_active_curve();
		
		virtual int get_data_size() const{return -1;};
		virtual RGBvalue* get_color() const{return NULL;};
		virtual char get_line() const{return '-';};
		virtual double get_max(SPECTRUM_TYPE curve = SP_TIME, SPECTRUM_TYPE t = SP_REAL) {return -1;};
		virtual double get_min(SPECTRUM_TYPE curve = SP_TIME, SPECTRUM_TYPE t = SP_REAL) {return -1;};
		virtual double get_scale() const{return 0;}; //returns the scale for the active vector
		virtual double get_offset() const{return 0;}; //returns the offset for the active vector
		virtual void set_scale(double scale){}; //Sets the scale for the active vector
		virtual void set_offset(double offset){}; //sets the offset for the active vector
		virtual void set_color(int r, int g, int b){};//Sets the color for the active vector
		virtual void set_line(char type){}; //Sets the line for both vectors (they have to have the same for renderer compatibility)
		virtual void increase_resolution(){};//Not implemented for complex curves
		virtual Vector2D find_closest_maxima(int location, int direction) const {Vector2D d;return d;};
		virtual Vector2D find_closest_minima(int location, int direction) const {Vector2D d;return d;};
		virtual vector<Vector2D> find_maximas_in_intervall(int from, int to) const {vector<Vector2D> d; return d;};
		virtual vector<Vector2D> find_minimas_in_intervall(int from, int to) const {vector<Vector2D> d; return d;};

		static vector<RENDERER_TYPE> get_supported_renderers();

		virtual vector<double> approximate_curve(int degree) {vector<double> d; return d;};

		virtual void simplify_curve(){};

		virtual void save_curve_to_file(string s) {};
		virtual void read_curve_from_file(string s){};

		virtual void amares(string prior_knowledge, int nr_sinusoids){};
		
		
		char active_type;
		SPECTRUM_TYPE base_type;
		complex<double> phase_shift;
		double time_shift;
		bool modified;
		
private:
		//char active_type;
		//SPECTRUM_TYPE base_type;
		double time_res, time_offset;
		double freq_res, freq_offset;	
};


#endif
