//////////////////////////////////////////////////////////////////////////
//
//  Datamanager $Revision$
//
//  The datamanager maintains a list of data loaded or created at
//  runtime. It has functions for loading and saving data (that invoke
//  functions in the relevant classes)
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

#ifndef __datamanager__
#define __datamanager__

#include <iostream>
#include <fstream>
#include <list>
#include <vector>

#include <FL/Fl_Pack.H>
#include <FL/Fl_File_Chooser.H>

#include "FLTKutilities.h"  //for horizresizeablescroll class
#include "global.h"
#include "datawidget.h"
#include "ptconfig.h"
//#include "image_base.h"


class datamanager
{
    public:
        static const unsigned int DATA_VECTOR_MAX = 10000;

    protected:
        std::vector<data_base*> dataItems;

		bool closing_program;	//Destructor has been called, --> all FLTK widgets are already dead

        horizresizeablescroll *data_widget_box;  //Widget that lists the datawidgets.

        Fl_Menu_Item *image_menu;
		Fl_Menu_Item *objects_menu;
		Fl_Menu_Item *point_menu;

		template <class OCLASS>
            Fl_Menu_Item* object_menu();
        
        void data_vector_has_changed();

    public:
        datamanager();
        ~datamanager();

        void datawidgets_setup();

        void add_datawidget(datawidget_base *the_widget);
        void remove_datawidget(datawidget_base *the_widget);
        void refresh_datawidgets(); //trigger redraw of widgets when list information has been updated
        //void loadimage_setup();
        void print_dataItems();
        void FLTK_running(bool running);
        bool FLTK_running();        //when datamanager destructor is called,
                                    //the window is closed and all FLTK widgets (including
                                    //the data_widget_box) have been deleted.
                                    //Functions called at this precious moment
                                    //need a way of knowing whether their widgets
                                    //still exist.
                                    //If widget-associated objects (like the datawidget)
                                    //are FLTK subclasses, they are deleted automatically
                                    //and this won't be a problem.

        std::vector<data_base*>::iterator begin() const;
        std::vector<data_base*>::iterator end() const;
        
        std::string get_data_name(int ID);
        void set_image_name(int ID,std::string n);
        
        const Fl_Menu_Item * FLTK_image_menu() const;
        const Fl_Menu_Item * FLTK_objects_menu() const;
        const Fl_Menu_Item * FLTK_point_menu() const;
        
        static void clear_data_list_button_callback(Fl_Widget *callingwidget, void *thisdatamanager);
        static void loadimage_callback(Fl_Widget *callingwidget, void *thisdatamanager);
        void loadimages();
        static void dcm_import_callback(Fl_Widget *callingwidget, void *thisdatamanager);
        static void dcm_series_callback(Fl_Widget *callingwidget, void *thisdatamanager);


        static void removedata_callback(Fl_Widget *, void *);
		static void save_additional_data_callback(Fl_Widget *, void *);
		static void save_curve_callback(Fl_Widget *, void *);
		static void toggle_additional_data(Fl_Widget *, void *);
		static void connect_additional_data_callback(Fl_Widget *, void *);
        static void save_dcm_callback(Fl_Widget *, void *);
        static void save_vtk_callback(Fl_Widget *, void *);
        static void save_hist_callback(Fl_Widget *, void *);

        void add(image_base *im, string name="", bool data_changed=false, string slc_orient="");           //add image to vector, notify other managers
//		void add(image_base &v);			//JK-test
		void add(curve_base *curve);
		void add(point_collection * v); 

		// Use delete_data() to remove data (data_base::~data_base() calls remove_data() after the allcoated data is removed)
        void remove_data (int id);          //remove image/point from vector, 
        void remove_data (data_base * d);   //notify other managers
        
        void delete_data (int id);          //delete data object and remove it
        void delete_data (data_base * d);   //from vector
        void delete_all ();
        
        void data_has_changed (int image_ID, bool recalibrate = false);   
        //signal that contents of a image has changed,
        //to update thumbnails, display
        //recalibrate determines whether max values etc. are refreshed
        // - doing this may distort the display of changes to the image
        
        void rebuild_point_menu();
        void rebuild_objects_menu();
        void rebuild_image_menu();
        
		//JK geometry information will be difficult to import, as files can be chosen arbitraryly...
		//Set the geometry info to "default" (i.e no rotation, origin (0,0,0), scaling (1,1,1) )
		void load_dcm_import_vector(std::vector<std::string> dcm_filenames, std::string import_vol_name, DCM_IMPORT_WIN_TYPE win_type);


        int create_empty_image(int x, int y, int z, int unit);      //creates empty image of given size
                                                                    //and fills it with testpattern()
                                                                    //returns ID
                                                                    //DEPRECATED: use constructor and datamanager::add instead
        
        int create_empty_image(image_base *, imageDataType unit);    //create empty image with same
                                                                     //dimensions as argument
                                                                     //DEPRECATED: use image_base::alike instead
        int find_data_index(int uniqueID);
		
		template<class T> T* get_image (int ID){ //TODO_R kolla sa det funkar med detta 
			vector<data_base*>::iterator itr=dataItems.begin();

			while (itr != dataItems.end()){
				if (**itr == ID){
					T * i = dynamic_cast<T *>(*itr); //TODO_R Andra denna rad och return tpe
            
					 if (pt_error::error_if_null(i,"Trying to get_image when requested ID is not image type",pt_error::fatal) != NULL){
						 return i; 
					 }
				}
			itr++;
			}

			return NULL;
		}

        data_base * get_data (int ID);
		int last_image();
        
        void parse_and_load_file(char filename[]);
        void parse_identifier(std::ifstream &in, int &identifierindex);
    };

#endif
