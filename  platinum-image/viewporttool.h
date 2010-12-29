//////////////////////////////////////////////////////////////////////////
//
//  Viewporttool $Revision$
//
/// The base class for implementing mouse behaviors/actions in a viewport
///
//  $LastChangedBy$
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

#ifndef __viewporttool__
#define __viewporttool__

#include "event.h"

#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>

#include <map>
#include <sstream>

#include "point.h"

class image_base;
class viewport;
class renderer_base;
class statusarea;
class thresholdparvalue;

class viewporttool 
{
public:
    //typedef viewporttool * (*taste_fcn_pointer)(viewport_event &);
    typedef viewporttool * (*vpt_create_pointer)(viewport_event &);

private:
    static statusarea * statusArea;
    //static std::map<std::string, taste_fcn_pointer> tools;
    static std::map<std::string, vpt_create_pointer> tools;
    
    static std::string selected;	//key for the currently selected tool
	static std::vector<std::string> name_vector;
	static std::vector<std::string> letter_vector;
	//string name;					//buffered to avoid memory leaks due to strdup...

    static void cb_toolbutton (Fl_Widget *,void *);
    
    template <class TOOL>
        static void Register ();
    template <class TOOL>
        static viewporttool *CreateObject(viewport_event &event);

protected:
    image_base *    image; //do dynamic_cast to whatever class that is needed //TITTA HaR!!!
    viewport *      myPort;
    renderer_base * myRenderer; 

public:
    //static void Register (std::string,vpt_ctor_pointer);
    
    //tool selection & controls
    const static int toolbox_w = 250;
    static Fl_Pack * toolbox;
    
    viewporttool(viewport_event &);
    virtual ~viewporttool();
    
    static void init(int x, int y,statusarea *);
    
    static void select (const std::string);
	static void select_only( const std::string key );
    static viewporttool * taste(viewport_event &,viewport *,renderer_base *);  //if the current tool responds to the event, return instance (which will be getting the events from now on until another tool is selected)
    virtual void handle(viewport_event &) = 0;
    //bool handle(int event,enum {create, adjust} );
};

class nav_tool : public viewporttool
{
protected:
    static const float wheel_factor;
    static const float zoom_factor;

    int last_global_x;					//pt_event store drag origin (most useful) but for panning we need the pos at last iteration
    int last_global_y;					//pt_event store drag origin (most useful) but for panning we need the pos at last iteration
    int last_local_x;					
    int last_local_y;					
    Vector3D physical_zoom_start_pos;		//pt_event store initial "Global" position of mouse, used to zoom on the first position clicked...
    std::vector<int> local_zoom_start_pos;	//pt_event store initial "Local" position of mouse, used to zoom on the first position clicked...
    renderer_base * renderer;
    std::ostringstream numbers;

	void refresh_by_image_and_direction();
//	void refresh_overlay_by_image_and_direction();
	void move_voxels( int x, int y, int z );
	void center3d_and_fit();
	void center2d();
		
public:
    nav_tool (viewport_event &);
    virtual void handle(viewport_event &);
    static void init (); 
    static const std::string name ();

};

class dummy_tool : public viewporttool //test tool
{
public:
    dummy_tool (viewport_event &);
    virtual void handle(viewport_event &);
    static void init (); 
    static const std::string name ();
};

class cursor_tool;

class cursor_tool : public nav_tool //subclass of nav_tool because it is useful to be able to
                                    //do some navigation while working with a selection
{
protected:
    static point * selection;
    static cursor_tool * selectionOwner;
    
    //controls
    static Fl_Output * coord_display;
    static Fl_Button * make_button;
    
    void make();
    static void cb_make(Fl_Widget*, void*); //wrapper for make
    
public:
    cursor_tool (viewport_event &);
    ~cursor_tool();
    
	static Vector3D get_global_selection_coords();
    static const std::string name ();
    static void init (); //initialize controls in statusArea
    virtual void handle(viewport_event &);
	void update_interactive_message(viewport_event &event);
};


class freeform_ROI_tool : public viewporttool
{};

#endif