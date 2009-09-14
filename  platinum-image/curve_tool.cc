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

#include "curve_tool.h"
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


curve_tool::curve_tool(viewport_event & event) : nav_tool(event)
{
	if (event.type() == pt_event::hover || event.type() == pt_event::key ||event.type() == pt_event::adjust ||
		event.type() == pt_event::browse|| event.type() == pt_event::create || event.type() == pt_event::scroll)
	{
        event.grab();
	}
}

curve_tool::~curve_tool()
{
}

const std::string curve_tool::name()
{
	return "Curve manipulation tool";
}

void curve_tool::init()
{
}

void curve_tool::handle(viewport_event &event)
{
	if(dynamic_cast<rendergeom_curve*> (myRenderer->the_rg) == NULL){
		//cout << "Wrong tool. Curve geometry is not beeing used" << endl;
		return;
	}else if(dynamic_cast<curve_base*>((dynamic_cast<rendergeom_curve *> (myRenderer->the_rg))->curve) == NULL){
		//cout << "Wrong tool. There has to be a curve in viewport" << endl;
		return;
	}
    const int * mouse2d = event.mouse_pos_local();
    FLTK_Event_pane *fp = event.get_FLTK_Event_pane();

	if(event.state() == pt_event::end && myPort->ROI_rect_is_changing && event.type() != pt_event::hover){
		//hover is excluded since this event.type is thown when mouse is first clicked
		   //Knapp slapps upp
		cout << "Knapp slapps upp" << endl;
    }
	if(event.state() == pt_event::begin){
		zoom_start = ((rendergeom_curve *)myRenderer->the_rg)->view_to_curve(event.mouse_pos_local()[0], event.mouse_pos_local()[1], fp->w(), fp->h());	
		zoom_start_world = ((rendergeom_curve *)myRenderer->the_rg)->curve_to_view(zoom_start[0], zoom_start[1], fp->w(), fp->h());
		event.grab();
	}
	if(event.state() == pt_event::end){
		((rendergeom_curve *)myRenderer->the_rg)->measure_location[0] = -1;
		((rendergeom_curve *)myRenderer->the_rg)->measure_location[1] = -1;
	}

	switch (event.type())
	{
		case pt_event::key:  
			if ( event.key_combo( pt_event::space_key ) ){			
				event.grab();
				((rendergeom_curve *)myRenderer->the_rg)->cx = 0;
				((rendergeom_curve *)myRenderer->the_rg)->cy = 0;
				((rendergeom_curve *)myRenderer->the_rg)->zoom = 1.0;


				event.grab();
				fp->needs_rerendering();
				//RN This is added because all viewports that use this edited rg should be updated
				viewmanagement.refresh_viewports_from_geometry(myRenderer->geometry_id());
				break;
			}

			/*if (event.key_combo(pt_event::space_key + pt_event::shift_key)){
				event.grab();	
			}*/
			
		case pt_event::scroll:
			if ( event.state() == pt_event::iterate){
			event.grab();
			//change curve to configure
			string curve_name = myRenderer->the_rc->change_top_image(event.scroll_delta());
			cout << "scrollar: " << event.scroll_delta() << endl;

			numbers.str("");
			numbers << "Curve at config position: " << curve_name;
			userIOmanagement.interactive_message(numbers.str());
			fp->needs_rerendering();
			//RN This is added because all viewports that use this edited rg should be updated
			viewmanagement.refresh_viewports_from_geometry(myRenderer->geometry_id());
			}
			break;

		case pt_event::create:
			if ( event.state() == pt_event::iterate){
                    event.grab();

					last_local_y = event.mouse_pos_local()[1];
					last_local_x = event.mouse_pos_local()[0];

					Vector3D after = ((rendergeom_curve *)myRenderer->the_rg)->view_to_curve(last_local_x, last_local_y, fp->w(), fp->h());
					
					((rendergeom_curve *)myRenderer->the_rg)->measure_location[0] = last_local_x;
					((rendergeom_curve *)myRenderer->the_rg)->measure_location[1] = last_local_y;
					//Can use zoom_start for this also!
					double dx = abs(after[0] - zoom_start[0]);
					numbers.str("");
					numbers << "width: " << std::fixed << std::setprecision(2) << dx*((rendergeom_curve *)myRenderer->the_rg)->x_scale;
					userIOmanagement.interactive_message(numbers.str());

                    fp->needs_rerendering();
					//RN This is added because all viewports that use this edited rg should be updated
					viewmanagement.refresh_viewports_from_geometry(myRenderer->geometry_id());
			}
			break;

		case pt_event::browse:
			if ( event.state() == pt_event::iterate){
                    event.grab();

					((rendergeom_curve *)myRenderer->the_rg)->cx-=last_local_x-event.mouse_pos_local()[0];
					((rendergeom_curve *)myRenderer->the_rg)->cy-=last_local_y-event.mouse_pos_local()[1];
					last_local_x = event.mouse_pos_local()[0];
					last_local_y = event.mouse_pos_local()[1];
                    fp->needs_rerendering();
					//RN This is added because all viewports that use this edited rg should be updated
					viewmanagement.refresh_viewports_from_geometry(myRenderer->geometry_id());
			}
			break;
		case pt_event::adjust:
			if ( event.state() == pt_event::iterate){
                    event.grab();
			
					float z = abs(float(1.0+(event.mouse_pos_local()[1]-last_local_y)*zoom_factor)); //the absolute value is needed since negative values inverts the image...
					
					last_local_y = event.mouse_pos_local()[1];
					last_local_x = event.mouse_pos_local()[0];
					
					//zoom...
					((rendergeom_curve *)myRenderer->the_rg)->zoom*=z;

					Vector3D after = ((rendergeom_curve *)myRenderer->the_rg)->curve_to_view(zoom_start[0], zoom_start[1], fp->w(), fp->h());
					

					//Move image to zoom around mouse pointer
					int dx = after[0] - zoom_start_world[0];
					int dy = after[1] - zoom_start_world[1];

					((rendergeom_curve *)myRenderer->the_rg)->cx-=dx;
					((rendergeom_curve *)myRenderer->the_rg)->cy-=dy;

					fp->needs_rerendering();
					//RN This is added because all viewports that use this edited rg should be updated
					viewmanagement.refresh_viewports_from_geometry(myRenderer->geometry_id());
			}
			break;
		case pt_event::hover:
             //display values
			switch (event.state()){
				case pt_event::begin:
                case pt_event::iterate:
					{
						numbers.str("");
						//get values and update statusfield
						double val[2];
						
							((rendergeom_curve *)myRenderer->the_rg)->get_value(mouse2d[0], val);

							((rendergeom_curve *)myRenderer->the_rg)->mouse_location[0] = event.mouse_pos_local()[0];
							numbers << "x: " << std::fixed << std::setprecision(2) << val[0] << " f(x): " << val[1];
							userIOmanagement.interactive_message(numbers.str());
							event.grab();
							fp->needs_rerendering();
							//RN This is added because all viewports that use this edited rg should be updated
							viewmanagement.refresh_viewports_from_geometry(myRenderer->geometry_id());
					}
					break;
				default:
					break;
			}
		/*case pt_event::draw:
			event.grab;
			break;
 */
		default:
			break;

	}
		// NOTE: no break, update hovering also (scroll is ignored because there is no iterate event)
	nav_tool::handle(event);
	//}
}