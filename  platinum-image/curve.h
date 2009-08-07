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
/*
	public:
		typedef std::map<int, Vector3D> pointStorage;
			
	protected:
        pointStorage thePoints;
		int active;	// This variable is set from FLTKuserIOpar_landmarks and is used in rendererMPR 
*/		
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

		bool modified;
/*
		point_collection(const point_collection & source);	// copy constructor
		const point_collection & operator=(const point_collection & source);
         
		pointStorage::const_iterator begin() const;
        pointStorage::iterator begin();
		pointStorage::const_iterator end() const;
        pointStorage::iterator end();

        void add(pointStorage::mapped_type); //add point or change if exist
        //void remove (pointStorage::iterator); //the opposite of above
		void remove(pointStorage::key_type);
		void add_pair(pointStorage::key_type, pointStorage::mapped_type);
		
//		pointStorage::size_type num_points;
		
		pointStorage::mapped_type get_point(int i);
        
        virtual void redraw ()
            {
            pt_error::error("Attempting redraw() on point_collection, not implemented yet");
            }
        
        virtual void save_to_DCM_file (const std::string, const bool useCompression = true, const bool anonymize = true)
            {
            pt_error::error("Attempting save_to_DCM_file on point_collection, not implemented yet");
            }
        virtual void save_to_VTK_file (const std::string, const bool useCompression = true)
            {
            pt_error::error("Attempting save_to_VTK_file on point_collection, not implemented yet");
            }
			
		
		void set_active(int);
		int get_active();

		void clear();
		
		bool contains( pointStorage::key_type index );
		
		bool empty();
		
		int size() const;

		void info();

		virtual	void save_histogram_to_txt_file(const std::string filename, const std::string separator=";");		
		string resolve_tooltip();			//combines tooltip data of this class with data from other classes
		string resolve_tooltip_point_collection();	//resolves tooltip data typical for this class
*/
};
	
#endif
