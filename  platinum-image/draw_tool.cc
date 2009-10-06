// $Id:$

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

#include <iomanip>

#include "draw_tool.h"
#include "renderer_base.h"	
#include "viewmanager.h"
#include "userIOmanager.h"

#include "rendermanager.h"
//#include "curve.h"
//#include "curve.hxx"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;
extern userIOmanager userIOmanagement;


draw_tool::draw_tool(viewport_event & event) : nav_tool(event)
{
	draw_type = 'x';
	if (event.type() == pt_event::hover || event.type() == pt_event::key ||event.type() == pt_event::adjust ||
		event.type() == pt_event::browse|| event.type() == pt_event::create || event.type() == pt_event::scroll)
	{
        event.grab();
	}
}

draw_tool::~draw_tool()
{
}

const std::string draw_tool::name()
{
	return "draw tool";
}

void draw_tool::init()
{
}

void draw_tool::handle(viewport_event &event)
{
    const int * mouse2d = event.mouse_pos_local();
    FLTK_Event_pane *fp = event.get_FLTK_Event_pane();

	if(event.state() == pt_event::end && myPort->ROI_rect_is_changing && event.type() != pt_event::hover){
		//hover is excluded since this event.type is thown when mouse is first clicked
		   //Knapp slapps upp
		cout << "Knapp slapps upp" << endl;
    }
	if(event.state() == pt_event::begin){
		start = create_Vector2D(event.mouse_pos_local()[0], event.mouse_pos_local()[1]);
		event.grab();
	}
	if(event.state() == pt_event::end){

		switch(draw_type){
			case 'c':
				//find the dataobjects in viewport!
				break;
			case 'l':
				break;
			case 'p':
				break;
			case 'r':
				break;
			case 'f':
				break;
			default:
				break;
		}
	}

	switch (event.type())
	{
		case pt_event::key:  
			numbers.str("");
			if ( event.key_combo( pt_event::f1_key ) ){
				draw_type = 'p';
				numbers << "Drawing: point";
				event.grab();
			}else if(event.key_combo( pt_event::f2_key )){
				draw_type = 'l';
				numbers << "Drawing: line";
				event.grab();
			}else if(event.key_combo( pt_event::f3_key )){
				draw_type = 'c';
				numbers << "Drawing: circle";
				event.grab();
			}else if(event.key_combo( pt_event::f4_key )){
				draw_type = 'r';
				numbers << "Drawing: rectangle";
				event.grab();
			}else if(event.key_combo( pt_event::f5_key )){
				draw_type = 'f';
				numbers << "Drawing: freehand";
				event.grab();
			}
			userIOmanagement.interactive_message(numbers.str());
			fp->needs_rerendering();
			break;

		case pt_event::adjust://left mouse button
			if ( event.state() == pt_event::iterate){ //moving kursor
                    event.grab();
					if(draw_type == 'f'){
						Vector2Dint i = create_Vector2Dint(event.mouse_pos_local()[0], event.mouse_pos_local()[1]);
						free.push_back(i);
						fp->needs_rerendering();
					}
			}
			break;

	}
		// NOTE: no break, update hovering also (scroll is ignored because there is no iterate event)
	nav_tool::handle(event);
	//}
}