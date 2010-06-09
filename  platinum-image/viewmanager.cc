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

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_File_Chooser.H>

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Widget.H>

#include <FL/fl_draw.H>

#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Light_Button.H>

#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Group.H>

#include "global.h"

#include "viewmanager.h"
#include "rendermanager.h"
#include "userIOmanager.h"

extern rendermanager rendermanagement;
extern userIOmanager userIOmanagement;
viewmanager viewmanagement;

void viewmanager::list_viewports()
{
	std::cout << "# of viewports: " << viewports.size() << std::endl;
	for (unsigned int i=0; i < viewports.size(); i++) { std::cout << viewports[i] << std::endl; } 
}

int viewmanager::create_viewport()
    {
    //viewport *aviewport;
    //aviewport = new viewport;
    viewports.push_back(viewport());

    return viewports.back().get_id(); //return ID of the viewport just created
    }

void create_viewport(char namn[])
    {
    //... int identifierare ... ger unik identifierare Ât varje objekt?

    //viewports.assign(...); 
    }

void delete_viewport(int viewportID) 
    {
    //iterate through viewportmapping. all ID equal to param are set to  -1
    //viewports.delete(ID);
    }

/*void set_view_at_viewport(int virtualview, int horizontal, int vertical, int viewportID)
{}*/

//// this contains the setup information - should be loaded/stored, changed by interacting w. widgets, ...
//

bool viewmanager::show_in_empty_viewport(int data_id)
{
	int vp_id = this->find_viewport_no_images(); //JK TODO change to no_data...
	if(vp_id != NOT_FOUND_ID){
		viewmanagement.get_viewport(vp_id)->render_data(data_id);
		return true;
	}
	return false;
}

void viewmanager::setup_demo_tiles()
{
    irregular_tiles = true;
    
    const int demonoofhorizontaltiles[MAXVIRTUALVIEWS] = { 2,0,0 };
    for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) { noofhorizontaltiles[vv] = demonoofhorizontaltiles[vv]; }

const int demonoofverticaltiles[MAXVIRTUALVIEWS][MAXHORIZONTALGRID] = { { 2,2,0,0  } };
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
for (int h=0; h < MAXHORIZONTALGRID ; h++)
    { noofverticaltiles[vv][h] = demonoofverticaltiles[vv][h];  }

const float demotilewidthpercent[MAXVIRTUALVIEWS][MAXHORIZONTALGRID] = { { 0.5, 0.5, 0.0, 0.0 } }; //last will be the remainder; i.e. not used
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
for (int h=0; h < MAXHORIZONTALGRID ; h++)
    { tilewidthpercent[vv][h] =  demotilewidthpercent[vv][h]; }

const float demotileheightpercent[MAXVIRTUALVIEWS][MAXHORIZONTALGRID][MAXVERTICALGRID] = { { { 0.5, 0.0, 0.0, 0.0 }, { 0.5, 0.0, 0.0, 0.0 }, { 0.5, 0.0, 0.0, 0.0 }, { 0.3, 0.1, 0.3, 0.1 }, { 0.2, 0.5, 0.1, 0.1 } } }; // last will be the remainder; i.e. not used
for (int vv=0; vv < MAXVIRTUALVIEWS; vv++)
for (int h=0; h < MAXHORIZONTALGRID ; h++)
for (int v=0; v < MAXVERTICALGRID; v++)
    { tileheightpercent[vv][h][v] = demotileheightpercent[vv][h][v]; }
    }

void viewmanager::setup_regular_tiles(int t_h, int t_v, int t_vv)
    {
    //basic tile setup with even grid spacing
    
    //range checking
    if (t_h > MAXHORIZONTALGRID)
        {
#ifdef _DEBUG
        std::cout << "Error in setup_regular_tiles - requested # of horizontal tiles (" << t_h << ") larger than MAXHORIZONTALGRID (" << MAXHORIZONTALGRID << ")"  << std::endl;
#endif
        t_h = 1;
        }
    
    if (t_v > MAXVERTICALGRID)
        {
#ifdef _DEBUG
        std::cout << "Error in setup_regular_tiles - requested # of vertical tiles (" << t_v << ") larger than MAXVERTICALGRID (" << MAXVERTICALGRID << ")"  << std::endl;
#endif
        t_v = 1;
        }
    
    if (t_vv > MAXVIRTUALVIEWS)
        {
#ifdef _DEBUG
        std::cout << "Error in setup_regular_tiles - requested # of virtual views (" << t_vv << ") larger than MAXVIRTUALVIEWS (" << MAXVIRTUALVIEWS << ")"  << std::endl;
#endif
        t_vv = 1;
        }

    
    irregular_tiles = false;
    for(int vv= 0; vv < t_vv; vv++){
        noofhorizontaltiles[vv] = t_h;
        
        for(int h = 0; h < t_h; h++ ){
            noofverticaltiles [vv][h] = t_v;
            tilewidthpercent[vv][h] = 1.0/t_h;
            
            for(int v = 0; v < t_v; v++ ){
                tileheightpercent[vv][h][v] = 1.0/t_v;
			}
		}
	}
}

void viewmanager::setup_irregular_tiles_h(vector<int> num_vert_in_these_columns, int t_vv)
    {

	//TODO - range checking
    
    irregular_tiles = true;
    
	int t_h = num_vert_in_these_columns.size();
	int t_v = 0;
    for(int vv=0; vv<t_vv; vv++)
		{
		noofhorizontaltiles[vv] = t_h;
        
        for(int h=0; h<t_h; h++)
            {
            noofverticaltiles[vv][h] = num_vert_in_these_columns[h];
            tilewidthpercent[vv][h] = 1.0/t_h;
            
			t_v = num_vert_in_these_columns[h];
            for(int v=0; v<t_v; v++)
                {
                tileheightpercent[vv][h][v] = 1.0/t_v; 
                }
            }
        }
    }

void viewmanager::setup_irregular_tiles_h(int v0, int v1, int v2, int v3)
{
	vector<int> v;
	v.push_back(v0);
	v.push_back(v1);
	v.push_back(v2);
	v.push_back(v3);
	setup_irregular_tiles_h(v);
}










































// Vilken av de N vyerna ska vi visa (vi kan bara titta pÂ en Ât gÂngen)?
void viewmanager::setup_views(int virtualview, int areawidth, int areaheight) // 0...antal som anv‰nds-1
    {
    // storage for all tiles et.c. generated in the loop (not really needed)
    Fl_Widget *boxes[MAXHORIZONTALGRID*MAXVERTICALGRID];
    int vtilesused = 0;
    int boxesused = 0;
	int tile_width_px = 0;
	int tile_height_px = 0;

    ////
    // this sets up a full virtual view in the CURRENT window. We prepared ...
    // Fl_Window window(areawidth,areaheight) ...[you are here]... window.end(); window.show(argc,argv);
    // ...  outside this call
    //Fl_Tile *htile = new Fl_Tile(0,0,areawidth,areaheight);
    
    Fl_Tile *verticaltiles[MAXHORIZONTALGRID];

   // float tilewidthpercentsum = 0;
    for (int h=0; h < noofhorizontaltiles[virtualview]; h++)
        {
        int tilewidth = int(tilewidthpercent[virtualview][h]*areawidth); //JK //RN Warning "round"
        if (h==noofhorizontaltiles[virtualview]-1){
			tilewidth=areawidth-tile_width_px;
			//tilewidth=areawidth-int(tilewidthpercentsum*areawidth); //Fippel
		} // avoid rounding errors in last column
        if (irregular_tiles){
			//verticaltiles[vtilesused] = new Fl_Tile(int(tilewidthpercentsum*areawidth), 0,tilewidth ,areaheight); //Fippel
			verticaltiles[vtilesused] = new Fl_Tile(tile_width_px, 0,tilewidth ,areaheight);
		}
      //  float tileheightpercentsum = 0;
		tile_height_px = 0;
        for (int v=0; v < noofverticaltiles[virtualview][h]; v++)
            {
            int tileheight = int(tileheightpercent[virtualview][h][v]*areaheight);
            if (v==noofverticaltiles[virtualview][h]-1){
				//tileheight = areaheight - int(tileheightpercentsum*areaheight); //Fippel
				tileheight = areaheight - tile_height_px;
			}
            if (viewportmapping[v][h][virtualview] >= 0)
                {
                int VPind = find_viewport_index( viewportmapping[v][h][virtualview] );
				VIEWPORT_TYPE vp_type = PT_MPR; //andra start

                //Create viewport widget and set position

               // viewports[ VPind ].initialize_viewport(int(tilewidthpercentsum*areawidth),int(tileheightpercentsum*areaheight),tilewidth,tileheight,vp_type); //Fippel
				viewports[ VPind ].initialize_viewport(tile_width_px,tile_height_px,tilewidth,tileheight,vp_type);
                }
            else
                {
                //placeholder
               // boxes[boxesused] = new Fl_Button(int(tilewidthpercentsum*areawidth),int(tileheightpercentsum*areaheight),tilewidth,tileheight,"placeholder"); //Fippel
				boxes[boxesused] = new Fl_Button(tile_width_px,tile_height_px,tilewidth,tileheight,"placeholder");
                boxes[boxesused]->box(FL_DOWN_BOX);
                boxes[boxesused]->align(FL_ALIGN_CLIP);
                boxes[boxesused]++;
                }
        //    tileheightpercentsum += tileheightpercent[virtualview][h][v];
			tile_height_px += tileheightpercent[virtualview][h][v]*areaheight;
            }
                if (irregular_tiles)
                    {verticaltiles[vtilesused]->end();}
        vtilesused++;
  //      tilewidthpercentsum += tilewidthpercent[virtualview][h];
		tile_width_px += tilewidthpercent[virtualview][h]*areawidth;
        }

    //htile->box(FL_BORDER_FRAME);
    //htile->end();

    //make htile (outer group for viewports) the resizable portion of the window
    //htile->parent()->resizable(htile);
    }

void viewmanager::erase_all_connections()
    {
    //// s‰tt upp kopplingsarrayen mellan de virtuella vyerna (varav en ‰r synlig) och de viewport:s som finns
    //
    for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) // samma mˆnster fˆr alla virtuella vyer - distribuera viewports
        {
        for (int v=0; v < MAXVERTICALGRID; v++)
            for (int h=0; h < MAXHORIZONTALGRID; h++)
                {
                viewportmapping[v][h][vv] = -1;
                }
        }
    }

int viewmanager::find_viewport_index(int id) // Goes from ID to INDEX returns NOT_FOUND_ID if none found
    {
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        if (viewports[i] == id) { return i; break; } // should break x3 but too lazy now...
        } 
    return NOT_FOUND_ID;
    }

//// v h vv in the viewarray -> viewport index
//
int viewmanager::get_viewport_id(int v, int h, int virtualview)
    {
    return viewportmapping[v][h][virtualview];
    }

//// return unique ID
//
int viewmanager::get_renderer_id(int viewportid)
    {
    int vpindex = find_viewport_index(viewportid);
    return viewports[vpindex].get_renderer_id();
    }

int viewmanager::find_viewport_not_in_view()
    {
    int unused = NOT_FOUND_ID;
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        int alreadyinuse = 0;
        for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) // samma mˆnster fˆr alla virtuella vyer - distribuera viewports
            {
            for (int v=0; v < MAXVERTICALGRID; v++)
                for (int h=0; h < MAXHORIZONTALGRID; h++)
                    {
                    if (viewports[i] == viewportmapping[v][h][vv]) { alreadyinuse = 1; break; }
                    // should break x3 but too lazy now...
                    }
            }
        if (alreadyinuse == 0)
            {
            unused = viewports[i].get_id();
            //    std::cout << "Found viewport " << unused << std::endl; 
            break;
            } // found one
        } 
    return unused;
    }

int viewmanager::find_viewport_no_renderer()
    {
    // returns -1 if none found

    int free_vp_id = NOT_FOUND_ID;
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        if (viewports[i].get_renderer_id()==NO_RENDERER_ID)
            {
            //found viewport without renderer
            free_vp_id = viewports[i].get_id();
            }
        }
    return free_vp_id;
    }

int viewmanager::find_viewport_no_images() 
    {
    // returns -1 if none found

    std::vector<viewport>::iterator itr=viewports.begin();
    int empty_vp_id = NOT_FOUND_ID;

    while (itr != viewports.end())
        {
        if (rendermanagement.renderer_empty((*itr).get_renderer_id()) == RENDERER_EMPTY) //null if threshold parameter does not match any rendered image
            {return (*itr).get_id();}
        itr++;
        }
    return empty_vp_id;
    }


void viewmanager::connect_views_viewports()
    {
    //int num_vp = viewports.size();

    //set up connection between virtual views and viewports
    
    for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) // same pattern for each virtual view
        {
        for (int h=0; h < noofhorizontaltiles[vv] ; h++) // no use to waste views outside the visible space
            {
            for (int v=0; v < noofverticaltiles[vv][h]; v++) // no use to waste views outside the visible space
                {
                int nyvp = find_viewport_not_in_view();
                // std::cout << nyvp << std::endl;
                if (nyvp != NOT_FOUND_ID) { viewportmapping[v][h][vv] = nyvp; }
                else  { viewportmapping[v][h][vv] = NOT_FOUND_ID; }
                }
            }
        }
    }

void viewmanager::connect_renderer_to_viewport(int viewportID, int rendererID)
    {
    int vpindex = find_viewport_index(viewportID);
    viewports[vpindex].connect_renderer(rendererID);
    //new renderer, refresh viewport!
    }

void viewmanager::list_connections()
    {
    //int num_vp = viewports.size();

    std::cout << "Listing of viewport to view connections:" << std::endl;
    for (int vv=0; vv < MAXVIRTUALVIEWS; vv++) // samma mˆnster fˆr alla virtuella vyer - distribuera viewports
        {
        std::cout << "Virtual view " << vv << std::endl;
        for (int v=0; v < MAXVERTICALGRID; v++)
            {
            for (int h=0; h < MAXHORIZONTALGRID; h++)
                {
                int VPind = find_viewport_index( viewportmapping[v][h][vv] );
                std::cout << viewportmapping[v][h][vv] << "(index " << VPind << ") ";
                }
            std::cout << std::endl;
            }
        }
    }

void viewmanager::refresh_viewports_after_toolswitch()
    {
    for (std::vector<viewport>::iterator itr=viewports.begin(); itr != viewports.end(); itr++) 
        {
        (*itr).refresh_after_toolswitch();
        }
    }

void viewmanager::refresh_viewports_from_combination(int c)
    {
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        viewports[i].refresh_from_combination(c);
        }
    }

void viewmanager::refresh_viewports_from_geometry(int g)
    {
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        viewports[i].refresh_from_geometry(g);
        }
    }
/*
void viewmanager::refresh_viewports_from_data_id(int id)
{
	viewports[0].refresh_from_combination(..refresh_from_geometry(g);
 	cout<<"refresh_viewports_from_data_id("<<id<<")... "<<endl;
	std::vector<int> combs = rendermanagement.combinations_from_data(id);
    for (unsigned int c=0; c < combs.size(); c++) 
       {
			cout<<"comb="<<c<<endl;
			view
			refresh_viewports_from_combination(c);
        }
}
*/



void viewmanager::update_overlays()
{
    for(unsigned int i=0; i < viewports.size(); i++){
		viewports[i].refresh_overlay();
	}
}


void viewmanager::refresh_overlays()
{
//	cout<<"viewmanager::refresh_overlays()... ";
    for(unsigned int i=0; i < viewports.size(); i++){
//		cout<<i<<" ";
		viewports[i].refresh_overlay();
	}
//	cout<<endl;
}

void viewmanager::refresh_viewports()
    {
    for (unsigned int i=0; i < viewports.size(); i++) 
        {
        viewports[i].refresh();
        }
    }

std::vector<threshold_overlay *> viewmanager::get_overlays (thresholdparvalue * threshold)
    {
    std::vector<viewport>::iterator itr=viewports.begin();
    std::vector<threshold_overlay *> result;

    userIOmanagement.select_tool ("Histogram highlight");
    
    while (itr != viewports.end())
        {
        threshold_overlay * olay;

        olay=(*itr).get_threshold_overlay (threshold);
        if (olay != NULL) //null if threshold parameter does not match any rendered image
            {result.push_back(olay);}
        itr++;
        }
    return result;
    }
	
std::vector<int> viewmanager::viewports_from_renderers(const std::vector<int> & rendererIDs)
{
	// Two viewports can not have the same renderer
	
	std::vector<int> viewport_ids;

	for ( std::vector<int>::const_iterator ritr = rendererIDs.begin(); ritr != rendererIDs.end(); ritr++ )
	{	
		int vp = viewport_from_renderer(*ritr);
		
		if ( vp != -1 )
			{ viewport_ids.push_back(vp); }		
	}
	
	return viewport_ids;
}

int viewmanager::viewport_from_renderer(int renderer_id)
{
	for ( std::vector<viewport>::iterator vitr = viewports.begin(); vitr != viewports.end(); vitr++ )
	{
		if ( (*vitr).get_renderer_id() == renderer_id )
			{ return (*vitr).get_id(); }
	}
		
	return -1;
}


viewport * viewmanager::get_viewport( int viewportID )
{
	int viewport_index = find_viewport_index(viewportID);
	
	if ( viewport_index != NOT_FOUND_ID )
	{ 
		return & viewports[viewport_index];
	}
	
	return NULL;
}

void viewmanager::show_point_by_renderers ( const Vector3D & point, const std::vector<int> & rendererIDs, const int margin )
{

	for ( std::vector<int>::const_iterator itr = rendererIDs.begin(); itr != rendererIDs.end(); itr++ )
	{ 
		Vector3D tmp;
		
		rendergeom_image* geometry = (rendergeom_image*)rendermanagement.get_geometry ( *itr );

		const int viewport_id = viewmanagement.viewport_from_renderer(*itr);
		viewport * vp = viewmanagement.get_viewport(viewport_id);
		
		const int sx = vp->pixmap_size()[0];
		const int sy = vp->pixmap_size()[1];

		renderer_base * myRenderer = rendermanagement.get_renderer(*itr);
		std::cout << "show_point_by_renderers" << endl; 
		std::vector<int> point_in_view = myRenderer->world_to_view(sx, sy, point);
		
		Vector3D at = geometry->look_at;

		if ( margin == -1 || point_in_view[0] < 0 + margin || point_in_view[0] > sx - margin || point_in_view[1] < 0 + margin || point_in_view[1] > sy - margin )
		{	// the margin is -1 OR the point is outside the viewport margin -> show the plane of the point AND center the point 
			tmp = point;
		}
		else
		{	// the point is inside the viewport margin -> show the plane of the point

			Vector3D v = point - at;			// vector from look_at to point

			Vector3D n = geometry->get_n();		// unit normal vector

			Vector3D v_proj_n = n * ( v * n );	// orthogonal projection of v onto n

			tmp =  at + v_proj_n;
		}
		
		
		rendermanagement.set_image_geometry(*itr, tmp); 
	}

}

void viewmanager::show_point_by_combination ( const Vector3D & point, const int combinationID,  const int margin )
{
	std::vector<int> dataIDs = rendermanagement.data_from_combination ( combinationID );	// get all data ids in the combination
	std::vector<int> rendererIDs = rendermanagement.renderers_from_data ( dataIDs );		// get the renderers that is connected to at least one of the images
	//const int id = 	rendermanagement.renderer_from_combination(combinationID);				// id of the active viewport

	// remove the id of the active viewport from the vector to prevent it from being updated
	// Redraw its overlay first...
/*	for ( std::vector<int>::iterator itr = rendererIDs.begin(); itr != rendererIDs.end(); itr++ )
	{
		if ( *itr == id )
		{
			itr = rendererIDs.erase(itr);
			break;
		}
	}
*/	show_point_by_renderers ( point, rendererIDs, margin );
}

void viewmanager::show_point_by_data ( const Vector3D & point, const int dataID, const int margin )
{
	std::vector<int> rendererIDs = rendermanagement.renderers_from_data ( dataID );		// return any renderer connected to this data id
	show_point_by_renderers ( point, rendererIDs, margin );
}

void viewmanager::zoom_specific_vp(int vp_id, Vector3D worldCenter, float zoom) //SO 0814
{
	this->get_viewport(vp_id)->set_look_at_and_zoom(worldCenter,zoom);
	this->get_viewport(vp_id)->refresh();
}


void viewmanager::set_vp_direction(int vp_id, preset_direction dir)
{
	this->get_viewport(vp_id)->enable_and_set_direction(dir);
}

void viewmanager::set_vp_renderer(int vp_id, string renderer_type)
{
	this->get_viewport(vp_id)->set_renderer(renderer_type);
}

void viewmanager::set_vp_blend_mode(int vp_id, blendmode bm)
{
	this->get_viewport(vp_id)->set_blend_mode(bm);
}

void viewmanager::disable_all_data_in_all_vps()
{
	for(int i=0;i<viewports.size();i++){
		rendermanagement.disable_all_data(viewports[i].get_renderer_id());
	}
}


void viewmanager::enable_data_in_vp(int vp_id, int data_id)
{
	int r_id = this->get_viewport(vp_id)->get_renderer_id();
	rendermanagement.enable_data(r_id,data_id);
}

void viewmanager::enable_data_in_vp(int vp_id, data_base *d)
{
	this->enable_data_in_vp(vp_id,d->get_id());
}



std::vector<int> viewmanager::get_viewport_ids_from_same_geometry_types(const int geometry_id)
{
	std::vector<int> vp_ids;
	for(int i =0; i<viewports.size();i++){
		//compare geomatry types... (add vp_ids to vector if equal)
		//rendergeometry *rg = rendermanagement.get_geometry(viewports[i].get_renderer_id);
		//renderer_base *rb = rendermanagement.get_renderer(viewports[i].get_renderer_id);
		//rb->find_typekey();
	}
	return vp_ids;
}
	 
