//////////////////////////////////////////////////////////////////////////
//
//   Data_base $Revision$
///
///  Untemplated base class for all data objects that are loadable/saveable
///  and may show up in the data list widgets (and others!)
///
//   $LastChangedBy$
//

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

#ifndef __data_base__
#define __data_base__

#include "vtkAlgorithmOutput.h"

#include "ptmath.h"

#include "global.h"
#include "error.h"
#include "metadata.h"
#include "additional_data.h"

class datawidget_base;

class data_base
    {
    private:
        void set_parameters();
        
        static int data_next_ID;
		vector<RENDERER_TYPE> supported_renderers;

    protected:
        int ID;
        datawidget_base *widget;

        void name_from_path(std::string filepath);   //sets image name from name portion of path
    
		data_base(data_base *const source=NULL, vector<RENDERER_TYPE> supported=vector<RENDERER_TYPE>());
/*
		data_base(vector<RENDERER_TYPE> supported);
		data_base();
		data_base(data_base * const, vector<RENDERER_TYPE> supported);
*/
		//data_base(const data_base & source);		// copy constructor

    public:
	
        virtual ~data_base();
		additional_data* helper_data;
		bool has_additionl_data();
		//virtual const data_base & operator=(const data_base & source);
		bool is_supported(RENDERER_TYPE type){
			for(int i = 0; i < supported_renderers.size(); i++){
				if(type == supported_renderers.at(i))
					return true;
			}
			return false;
		}

        //virtual const data_base &operator=(const data_base &k) { return k; }
        bool virtual operator<<(const data_base &k)
            { return ID==k.ID; }
        bool virtual operator==(const data_base &k)
            { return ID==k.ID; }
        bool virtual operator==(const int &k)
            { return ID==k; }
        bool virtual operator!=(const data_base &k)
            { return ID!=k.ID; }
        bool virtual operator<(const data_base &k)
            { return ID<k.ID; }
        bool virtual operator>(const data_base &k)
            { return ID>k.ID; }
        /*friend std::istream &operator>>(std::istream &in, image_base &k)
            { in >> k.ID; return in; }*/
        friend std::ostream &operator<<(std::ostream &ut, const data_base &k)
            {
            ut << "data_base" << "\"" << k.name() << "\" (ID " << k.ID << ")";
            return ut;
            }

        virtual void redraw () = 0;
        virtual void data_has_changed (bool);
        int get_id();
        void activate();
		virtual string resolve_value_world(Vector3D worldPos);

		// *** Metadata ***
		metadata meta;

        virtual void name(const std::string n);          //set name
        virtual const std::string name() const;          //get name
        bool from_file() const;
        void from_file(bool f); //set "from file" status
        virtual void save_to_DCM_file(const std::string, const bool useCompression = false, const bool anonymize = true) = 0;
        virtual void save_to_VTK_file(const std::string, const bool useCompression = true) = 0;
        virtual void save_histogram_to_txt_file(const std::string filename, const std::string separator=";");
		//virtual void helper_data_to_binary_image() = 0;
		string resolve_tooltip();			//combines tooltip data of this class with data from other classes
		string resolve_tooltip_data_base(); //resolves tooltip data typical for this class
		void set_tooltip(string s);
		virtual vtkAlgorithmOutput* getvtkStructuredPoints(); //allows porting to VTK for MIP-rendering
		int read_helper_data_from_file(string file_path);
		int save_helper_data_to_file(string file_path);
		bool draw_additional_data;
    };

#endif