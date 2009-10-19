//////////////////////////////////////////////////////////////////////////
//
//   UserIOmanager $Revision$
//
///  Maintains a list of userIO blocks, and relays all operations on
///  blocks and their parameters. The interface to this class is
///  independent of the FL toolkit.
//
//   $LastChangedBy$

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

#ifndef __userIOmanager__
#define __userIOmanager__

#include <list>

#include "FLTKutilities.h"

#include "ptmath.h"
#include "userIO.h"
#include "statusarea.h"

#include "global.h"

class userIOmanager
    {
    private:
        horizresizeablescroll * widgets_scroll;

        std::list<userIO *> IOblocks;
        userIO * block_from_ID(int IOblockID);

		FLTKuserIOpar_landmarks * get_landmarks(int userIO_ID);
		
        //void maintenance ();    //called on idle or other time, to cleanup deleted objects and such

    public:
        userIOmanager() { }

        ~userIOmanager();
		
//		static void test_callback(Fl_Widget* callingwidget, void* test);

        void setup();                   //create FLTK list widget and all, puts associated widgets in Fl_Group::current()

        void data_vector_has_changed();    //triggers rebuild of

        int add_userIO (std::string name, userIO_callback* cback = NULL,std::string ok_label = "");
        
        statusarea * status_area;        //! create userIO block, add to manager and return ID

        typedef enum {status,block} displayMethod;
        void show_message (std::string name, std::string message, displayMethod method = status );  //the simple way to display information; in a userIO block or in the status area (or, in the future, say, an alert window)
        void interactive_message (const std::string m = ""); //display fast-update message in statusarea, 
                                       //in char array stored elsewhere
                                       //do or die: must be cleared with interactive_message (NULL)
                                       //before array is deleted

        void progress_update (int step = 0, std::string message = "", int num_steps = 0);

        void finish_userIO(int userIO_ID);         //end group and redraw all widgets, ready to use

        void delete_userIO (int userIO_ID);

        //add parameter to userIO block, return parameter number
		int add_par_filepath(int userIO_ID, const std::string name, const std::string default_path="");
		int add_par_folderpath(int userIO_ID, const std::string name, const std::string default_path="");
		int add_par_coordinate3Ddisplay(int userIO_ID, std::string coord_name, std::string coord_type_name, Vector3D v);
		int add_par_landmarks ( int userIO_ID, const std::string name );
//		int add_par_landmarks(int userIO_ID, const std::string name,  const std::vector<std::string> & landmark_names, const std::vector<std::string> & option_names, int landmarks_id);
        int add_par_float(int userIO_ID, std::string new_param_name,float max=1,float min=0,float start_val=std::numeric_limits<float>::max());
        int add_par_float_box(int userIO_ID, std::string new_param_name,float max=1,float min=0);
        int add_par_longint(int userIO_ID, std::string new_param_name,long max=255,long min=0,long start_val=std::numeric_limits<long>::max());
        int add_par_longint_box(int userIO_ID, std::string new_param_name,long max=255,long min=0);
        int add_par_bool(int userIO_ID, std::string new_param_name,bool init_status = false);
		int add_par_string(int userIO_ID, std::string new_param_name,std::string init_status = "");
		int add_par_stringlist(int userIO_ID, std::string new_param_name,std::vector<std::string> strvector);

        int add_par_image(int userIO_ID, std::string new_param_name);
		int add_par_imageshow( int userIO_ID, std::string new_param_name );
        int add_par_points(int userIO_ID, std::string new_param_name);
        int add_par_voxelseed(int userIO_ID, std::string new_param_name);

        int add_par_histogram_2D(int userIO_ID, std::string new_param_name);
        int add_par_message(int userIO_ID, std::string new_param_name, std::string message);

        template <class ptype>
            ptype get_parameter(int IOblockID, int num);
        void select_tool (const std::string);
		void select_tool_only( const std::string key );

        std::vector<FLTKuserIOpar_histogram2D *> get_histogram_for_image (int imageID);
		
		int get_landmarksID(int userIO_ID);
    };

template <class ptype>
ptype userIOmanager::get_parameter(int uIO_ID, int par_num)
    {
    ptype parameter;

    parameter = block_from_ID(uIO_ID)->template get_par<ptype>(par_num);

    return parameter;
    }

#endif