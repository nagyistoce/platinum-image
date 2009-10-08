//////////////////////////////////////////////////////////////////////////
//
//  point_base $Revision$
//
//  Untemplated base class for point collections
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

#ifndef __point_base__
#define __point_base__

#include "data_base.h"

class point_collection : public data_base
{
	public:
		typedef std::map<int, Vector3D> pointStorage;
			
	protected:
        pointStorage thePoints;
		int active;	// This variable is set from FLTKuserIOpar_landmarks and is used in rendererMPR 
		
	public:
        point_collection();
		~point_collection();

		point_collection(const point_collection & source);	// copy constructor
		const point_collection & operator=(const point_collection & source);

		static vector<RENDERER_TYPE> get_supported_renderers();

		void helper_data_to_binary_image(){cout << "point_base has not implemented this yet" << endl;};
         
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

};
	
#endif
