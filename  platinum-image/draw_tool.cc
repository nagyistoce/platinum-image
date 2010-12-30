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
	remove_data = false;
	drawed = false;
	if (event.type() == pt_event::hover || event.type() == pt_event::key ||event.type() == pt_event::adjust)
	{
        event.grab();
	}
}

draw_tool::~draw_tool()
{
}

const std::string draw_tool::name()
{
	return "Draw tool";
}

void draw_tool::init()
{
}

void draw_tool::handle(viewport_event &event)
{
    //const int * mouse2d = event.mouse_pos_local();
    FLTK_Event_pane *fp = event.get_FLTK_Event_pane();

	switch (event.type())
	{
		case pt_event::key:  
			numbers.str("");
			if ( event.key_combo( '1') ){
				draw_type = 'p';
				numbers << "Drawing: point";
				event.grab();
			}else if(event.key_combo( '2' )){
				draw_type = 'l';
				numbers << "Drawing: line";
				event.grab();
			}else if(event.key_combo( '3' )){
				draw_type = 'c';
				numbers << "Drawing: circle";
				event.grab();
			}else if(event.key_combo( '4' )){
				draw_type = 'r';
				numbers << "Drawing: rectangle";
				event.grab();
			}else if(event.key_combo( '5' )){
				draw_type = 'f';
				numbers << "Drawing: freehand";
				event.grab();
			}else if(event.key_combo( pt_event::up_key )){
				myRenderer->the_rc->top_image<data_base>()->helper_data->show_up = 
					!myRenderer->the_rc->top_image<data_base>()->helper_data->show_up;
				event.grab();
			}else if(event.key_combo( pt_event::down_key )){
				myRenderer->the_rc->top_image<data_base>()->helper_data->show_down = 
					!myRenderer->the_rc->top_image<data_base>()->helper_data->show_down;
				event.grab();
			}else if(event.key_combo( pt_event::left_key)){
				myRenderer->the_rc->top_image<data_base>()->draw_additional_data = true;
				event.grab();
			}else if(event.key_combo( pt_event::right_key)){
				myRenderer->the_rc->top_image<data_base>()->draw_additional_data = false;
				event.grab();
			}else if(event.key_combo( pt_event::space_key )){

				if(draw_type != 'x' && !drawed){
					cout << "starting!!!!" << endl;
					start = myRenderer->view_to_world(event.mouse_pos_local()[0], event.mouse_pos_local()[1],fp->w(),fp->h());
					cout << "location: " << start[0] << " " << start[1] << " " << start[2] << endl; 
					drawed = true;
					if(draw_type == 'p'){//No need for double start-stop when point
						//data_base * base = myRenderer->the_rc->top_image<data_base>();
						draw_data(event, fp);
						draw_type = 'x';
						drawed = false;
						remove_data = false;
						free.clear();
					}
				}else if(draw_type != 'x' && drawed){
					data_base * base = myRenderer->the_rc->top_image<data_base>();
					if(remove_data){
						base->helper_data->data.pop_back();
					}
					draw_data(event, fp);
					draw_type = 'x';
					drawed = false;
					remove_data = false;
					free.clear();
				}else{
					cout << "No draw type selected" << endl;
				}
				event.grab();
			}
			userIOmanagement.interactive_message(numbers.str());
			fp->needs_rerendering();
			event.grab();
			break;

		default:
		{
			//suppress GCC enum warning
		}
	}
	if(event.state() == pt_event::iterate && drawed){
		if(remove_data){
			data_base * base = myRenderer->the_rc->top_image<data_base>();
			base->helper_data->data.pop_back();
		}else{
			remove_data = true;
		}
		if(draw_type == 'f' && drawed){
			free.push_back(myRenderer->view_to_world(event.mouse_pos_local()[0], event.mouse_pos_local()[1],fp->w(),fp->h()));
		}
		draw_data(event, fp);
		fp->needs_rerendering();
		event.grab();
	}
		// NOTE: no break, update hovering also (scroll is ignored because there is no iterate event)
	nav_tool::handle(event);
	//}
}

void draw_tool::draw_data(viewport_event &event, FLTK_Event_pane *fp){
	data_base * base = myRenderer->the_rc->top_image<data_base>();
	Vector3D stop = myRenderer->view_to_world(event.mouse_pos_local()[0], event.mouse_pos_local()[1],fp->w(),fp->h());
	std::vector<int> w_start, w_stop;
	Vector3D middle, ender;
	//Vector3D vpos = myRenderer->view_to_voxel(mouse2d[0], mouse2d[1],fp->w(),fp->h());
	switch(draw_type){
		case 'c':
			cout << "adding circle" << endl;
			base->helper_data->add_circle(start, myRenderer->the_rg->get_n(),sqrt(
				pow((start[0]-stop[0]),2)+
				pow((start[1]-stop[1]),2)+
				pow((start[2]-stop[2]),2)
				));
			break;
		case 'l':
			base->helper_data->add_line(start, stop);
			//cout << "adding line: " << start << "   " << stop << endl;
			break;
		case 'p':
			cout << "adding point" << endl;
			base->helper_data->add_point(start);
			break;
		case 'r':
			cout << "adding rectangle" << endl;
			w_start = myRenderer->world_to_view(fp->w(), fp->h(),start);
			w_stop = myRenderer->world_to_view(fp->w(), fp->h(),stop);
			middle = myRenderer->view_to_world(w_start[0]+(w_stop[0]-w_start[0]), w_start[1],fp->w(), fp->h());
			ender = myRenderer->view_to_world(w_start[0], w_start[1]+(w_stop[1]-w_start[1]),fp->w(), fp->h());
			base->helper_data->add_rect(start, middle, stop, ender);
			break;
		case 'f':
			cout << "adding freehand" << endl;
			//free.push_back(stop);
			base->helper_data->add_freehand(free);
			break;
		default:
			cout << "This should not happen..." << endl;
			break;
	}
	//draw_type = 'x';
	//drawed = false;
}