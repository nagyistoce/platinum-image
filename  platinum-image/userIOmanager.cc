// $Id$

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

#define __userIOmanager_cc__

#include "userIOmanager.h"
#include "viewporttool.h"
#include "viewmanager.h"

extern viewmanager viewmanagement;
userIOmanager userIOmanagement;

#include <FL/Fl.H>

void userIOmanager::setup()
    {
    const unsigned int xpos=Fl_Group::current()->x();
    const unsigned int ypos=DATALISTINITHEIGHT;
    const unsigned int height=Fl_Group::current()->h()-DATALISTINITHEIGHT; //claim the remaining vertical space below datawidget list
    const unsigned int width=Fl_Group::current()->w();

    widgets_scroll = new horizresizeablescroll(xpos,ypos,width,height);
	//widgets_scroll->callback(test_callback, this);
	//widgets_scroll->setupCallback();
    widgets_scroll->end();
	
	
    
    //status area is initialized in mainwindow.cc, through its own constructor
    }

userIOmanager::~userIOmanager()
{
    IOblocks.clear();
}

//void userIOmanager::test_callback(Fl_Widget *callingwidget, void* test) {
//	std::cout << "test_callback" << std::endl;
//}


void userIOmanager::show_message (std::string name, std::string message, displayMethod method  )
{
    switch (method)
        {
        case block:
			{
            int uioid = add_userIO (name,NULL,"");
            
            add_par_message(uioid,"",message);
            finish_userIO(uioid);
            break;
			}
        case status:
			{
            status_area->message(message);
            break;
			}
        }
}

void userIOmanager::interactive_message (const std::string m)
{
    status_area->interactive_message(m);
}

void userIOmanager::progress_update (int s, std::string m, int n)
{
    status_area->progress(s,m,n);
}

int userIOmanager::add_userIO (std::string name, userIO_callback* cback,std::string ok_label)
    {
    widgets_scroll->begin();
    
    unsigned int new_userIO_y_pos=widgets_scroll->interior->y(); 
    
    if (IOblocks.size() >0)
    {new_userIO_y_pos=IOblocks.back()->y()+IOblocks.back()->h();}
    IOblocks.push_back(new userIO(widgets_scroll->interior->x(),new_userIO_y_pos,widgets_scroll->interior->w(),block_header_height,name,cback,ok_label));
    
    widgets_scroll->end();

    return IOblocks.back()->id();
    }

void userIOmanager::finish_userIO(int userIO_ID)
    {
    //no action for the specific userIO object, that may be
    //needed for other toolkits however
    
    widgets_scroll->redraw();
    }

void userIOmanager::delete_userIO (int userIO_ID)
    {
    userIO * tbd = block_from_ID (userIO_ID);

    //remove from userIO blocks vector
    IOblocks.remove(tbd);

    //remove from widget list
    tbd->hide();
    widgets_scroll->remove(tbd);

    //this is how deletion should be done theoretically,
    //the close button is removed and "safe deleted" first,
    //then delete the whole group (everything but
    //the button)
    //causes subsequent crash for some reason
   
    //delete tbd;
    }

userIO * userIOmanager::block_from_ID(int IOblockID)
    {
    std::list<userIO *>::iterator itr=IOblocks.begin();

    while (itr != IOblocks.end())
        {
        if (**itr == IOblockID)
            { return *itr; }
        itr++;
        }

    return (NULL);
    }



int userIOmanager::add_par_filepath(int userIO_ID, const std::string name, const std::string default_path)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_path(name, 1, default_path);
    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_folderpath(int userIO_ID, const std::string name, const std::string default_path)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_path(name, 0, default_path);
    return block_from_ID(userIO_ID)->add_par(par);
    }


int userIOmanager::add_par_coordinate3Ddisplay(int userIO_ID, std::string coord_name, std::string coord_type_name, Vector3D v)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_coord3Ddisplay(coord_name, coord_type_name, v);
    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_landmarks(int userIO_ID, const std::string name)
//int userIOmanager::add_par_landmarks(int userIO_ID, const std::string name, const std::vector<std::string> & landmark_names, const std::vector<std::string> & option_names, int landmarks_id)
{
	FLTKuserIOpar_landmarks * par = new FLTKuserIOpar_landmarks( name );
//	FLTKuserIOpar_landmarks * par = new FLTKuserIOpar_landmarks(name, landmark_names, option_names, landmarks_id);
	return block_from_ID(userIO_ID)->add_par(par);
}

int userIOmanager::add_par_float(int userIO_ID, std::string new_param_name,float max,float min, float start_val)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_float (new_param_name,max,min);
	if (start_val<=max && start_val>=min) {
		FLTKuserIOpar_float * p = (FLTKuserIOpar_float*)par;
		p->set_value(start_val);
	}
    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_float_box(int userIO_ID, std::string new_param_name,float max,float min, float start_val)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_float_box(new_param_name,max,min,start_val);
    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_longint(int userIO_ID, std::string new_param_name,long max,long min, long start_val)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_longint (new_param_name,max,min);
	if (start_val<=max && start_val>=min) {
		FLTKuserIOpar_longint * p = (FLTKuserIOpar_longint*)par;
		p->set_value_no_Fl_callback(start_val); //JK
	}
    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_longint_box(int userIO_ID, std::string new_param_name,long max,long min)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_longint_box (new_param_name,max,min);
    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_bool (int userIO_ID, std::string new_param_name,bool init_status)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_bool (new_param_name,init_status);
    return block_from_ID(userIO_ID)->add_par(par);
    }

int userIOmanager::add_par_string(int userIO_ID, std::string new_param_name,std::string init_status)
{
	FLTKuserIOparameter_base * par=new FLTKuserIOpar_string (new_param_name,init_status);
    return block_from_ID(userIO_ID)->add_par(par);
}

int userIOmanager::add_par_stringlist(int userIO_ID, std::string new_param_name,std::vector<std::string> strvector)
{
	FLTKuserIOparameter_base * par=new FLTKuserIOpar_stringlist (new_param_name,strvector);
    return block_from_ID(userIO_ID)->add_par(par);
}

int userIOmanager::add_par_image(int userIO_ID, std::string new_param_name)
    {
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_image (new_param_name);
    return block_from_ID(userIO_ID)->add_par(par);
    }
	
int userIOmanager::add_par_imageshow( int userIO_ID, std::string new_param_name )
{
    FLTKuserIOparameter_base * par = new FLTKuserIOpar_imageshow( new_param_name );
    return block_from_ID( userIO_ID )->add_par( par );
}

int userIOmanager::add_par_points(int userIO_ID, std::string new_param_name)
{
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_points (new_param_name);
	return block_from_ID(userIO_ID)->add_par(par);
}
/*
int userIOmanager::add_par_voxelseed(int userIO_ID, std::string new_param_name)
{
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_voxelseed (new_param_name + "(Activate Cursor Tool, Right click, and Catch..)");
	return block_from_ID(userIO_ID)->add_par(par);
}
*/
int userIOmanager::add_par_histogram_2D(int userIO_ID, std::string new_param_name)
{
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_histogram2D (new_param_name);
    return block_from_ID(userIO_ID)->add_par(par);
}

int userIOmanager::add_par_message(int userIO_ID, std::string new_param_name, std::string message)
{
    FLTKuserIOparameter_base * par=new FLTKuserIOpar_message (new_param_name, message);
    return block_from_ID(userIO_ID)->add_par(par);
}

void userIOmanager::data_vector_has_changed()
    {
    std::list<userIO *>::iterator itr=IOblocks.begin();

    while (itr != IOblocks.end())
        {
        (*itr)->data_vector_has_changed();
        itr++;
        }
    }

void userIOmanager::select_tool ( const std::string key )
{
    viewporttool::select(key);
    userIOmanagement.status_area->switch_pane(key);
    
    viewmanagement.refresh_viewports_after_toolswitch();  
}

void userIOmanager::select_tool_only( const std::string key )
{
	viewporttool::select_only( key );
}

std::vector<FLTKuserIOpar_histogram2D *> userIOmanager::get_histogram_for_image (int imageID)
    {
    std::vector<FLTKuserIOpar_histogram2D *> result;

    std::list<userIO *>::iterator itr=IOblocks.begin();

    while (itr != IOblocks.end())
        {
        std::vector<FLTKuserIOpar_histogram2D *> found;

        found=(*itr)->get_histogram_for_image(imageID);

        std::vector<FLTKuserIOpar_histogram2D *>::iterator ritr =found.begin();

            while (ritr != found.end())
                {
                result.push_back((*ritr));
                ritr++;
                }
        itr++;
        }

    return result;
    }
	

// --- private ---
FLTKuserIOpar_landmarks * userIOmanager::get_landmarks(int userIO_ID)
{
	FLTKuserIOpar_landmarks * landmarks=NULL;
	
	userIO * userIO_block = block_from_ID(userIO_ID);
	
	int nc = userIO_block->children();
	
	for (int c = 0; c < nc; c++)
	{
		if ( landmarks = dynamic_cast<FLTKuserIOpar_landmarks *>(userIO_block->child(c)) )
		{
			return landmarks;
		}
	}
	
	return landmarks;
}

int userIOmanager::get_landmarksID(int userIO_ID)
{
	 return get_landmarks(userIO_ID)->get_landmarksID();
}

