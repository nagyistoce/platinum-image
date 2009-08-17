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
#include <vector>
#include <algorithm>

#include "rendermanager.h"
#include "rendererMPR.h"
#include "rendererMIP.h"
#include "renderer_curve.h"
#include "viewmanager.h"
#include "datamanager.h"

//listedfactory<renderer_base> rendermanager::renderer_factory;
listedfactory<FLTKpane> rendermanager::pane_factory; //= listedfactory<renderer_base>(); //JK2

rendermanager rendermanagement;
extern datamanager datamanagement;
extern viewmanager viewmanagement;

using namespace std;

rendermanager::rendermanager()
{
    pane_factory.Register<FLTK_Pt_MPR_pane>("MPR");			//JK2 //Bytte från PT_pane
    pane_factory.Register<FLTK_VTK_Cone_pane>("VTK-Cone");	//JK2
    pane_factory.Register<FLTK_VTK_MIP_pane>("VTK-MIP");	//JK2
	pane_factory.Register<FLTK_Pt_Curve_pane>("Curve");	//Hmm...

	//---- Old version ---
//    renderer_factory.Register<rendererMPR>();	//JK2
//    renderer_factory.Register<rendererMIP>();	//JK2  //MIP renderer not created yet...
}

rendermanager::~rendermanager()
{
    renderers.clear();
    geometries.clear();
    combinations.clear();
}

void rendermanager::render(int rendererIndex, unsigned char *rgbimage, int rgbXsize, int rgbYsize)
    {
    renderers[rendererIndex]->render_position(rgbimage, rgbXsize, rgbYsize);
    }

void rendermanager::render_thumbnail(unsigned char *rgb, int rgb_sx, int rgb_sy, int dataID)
    {
    //decision: MPRs are nice for thumbnails

    rendererMPR::render_thumbnail(rgb, rgb_sx, rgb_sy, dataID);
    }

void rendermanager::render_threshold (int rendererIndex, unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold)
    {
    renderers[rendererIndex]->render_threshold(rgba, rgb_sx, rgb_sy,threshold);
    }

int rendermanager::find_renderer_index(int uniqueID)
{
    for(unsigned int i=0; i<renderers.size(); i++){
		if(renderers[i]->get_id() == uniqueID){
            return i;
		}
	}
	return -1; // not found
}

int rendermanager::find_renderer_id (int index)
{
    return renderers[index]->get_id();
}

renderer_base * rendermanager::get_renderer (int ID)
{
    for ( vector<renderer_base*>::iterator itr = renderers.begin();itr != renderers.end();itr++)
        {
        if (**itr==ID)
            { return *itr;}
        }
    
    return NULL;
}

vector<int> rendermanager::combinations_from_data (int dataID)
    {
    //when thumbnails are implemented, they should be re-rendered at this point

    vector<renderer_base*>::iterator itr = renderers.begin();
    vector<int> found_combinations;
    vector<int>::iterator citr;
    int index=0;

    while (itr != renderers.end())
        {
        if (image_rendered(index, dataID) != BLEND_NORENDER)
            {
            int combination = get_combination_id(index);

            if (find(found_combinations.begin(),found_combinations.end(),combination) == found_combinations.end())
                //check for duplicate
                {found_combinations.push_back(combination);}
            }
        itr++;
        index++;
        }

    return found_combinations;
    }

std::vector<int> rendermanager::renderers_from_data ( int dataID )
{
	std::vector<int> rendererIDs;

	for ( std::vector<renderer_base *>::const_iterator itr = renderers.begin(); itr != renderers.end(); itr++ )
	{
		int rendererIndex = find_renderer_index( (*itr)->get_id() );
		rendercombination * combination = get_combination( get_combination_id( rendererIndex ) );

		for ( std::list<rendercombination::renderpair>::const_iterator rpitr = combination->begin(); rpitr != combination->end(); rpitr++ )
		{ 
			if ( rpitr->pointer->get_id() == dataID )
			{
				rendererIDs.push_back( renderer_from_combination( (*itr)->get_id() ) );
				break;
			}
		}
	}
	return rendererIDs;
}

std::vector<int> rendermanager::renderers_from_data ( const std::vector<int> & dataIDs )
{
	std::vector<int> rendererIDs;

	for ( std::vector<renderer_base *>::const_iterator itr = renderers.begin(); itr != renderers.end(); itr++ )
	{	
		std::vector<int> images = images_from_combination ( (*itr)->combination_id() );
		
		for ( std::vector<int>::const_iterator ditr = dataIDs.begin(); ditr != dataIDs.end(); ditr++ )
		{
			if ( find ( images.begin(), images.end(), *ditr ) != images.end() )
			{	// the renderer holds at least one of the items in the data set (dataIDs)
				rendererIDs.push_back( (*itr)->get_id() );
				break;	// leave this iteration
			}
		}
	}
	return rendererIDs;
}

 
int rendermanager::renderer_from_combination(const int combination_id) const
{
	for ( std::vector<renderer_base*>::const_iterator itr = renderers.begin(); itr != renderers.end(); itr++ )
	{
		if ( (*itr)->combination_id() == combination_id )
			{ return (*itr)->get_id(); }
	}
	return -1;	
}

int rendermanager::renderer_from_geometry(const int geometry_id) const
{
	for ( std::vector<renderer_base*>::const_iterator itr = renderers.begin(); itr != renderers.end(); itr++ )
	{
		if ( (*itr)->geometry_id() == geometry_id )
			{ return (*itr)->get_id(); }
	}
	return -1;	
}

std::vector<int> rendermanager::geometries_from_renderers ( const std::vector<int> & rendererIDs )
{
	std::vector<int> geometryIDs;
	
	for ( std::vector<int>::const_iterator itr = rendererIDs.begin(); itr != rendererIDs.end(); itr++ )
	{
		geometryIDs.push_back( get_geometry_id( find_renderer_index(*itr) ) );
	}
	
	return geometryIDs;
}

std::vector<int> rendermanager::geometries_by_direction ( const int geometryID, const std::vector<int> & geometryIDs )
{
	std::vector<int> IDs;
	Vector3D a = ((rendergeom_image*)get_geometry_from_its_id(geometryID))->get_N();
	Vector3D b;
	for ( std::vector<int>::const_iterator itr = geometryIDs.begin(); itr != geometryIDs.end(); itr++ ){
		b = ((rendergeom_image*)this->get_geometry_from_its_id(*itr))->get_N();
//		cout<<"a="<<a<<endl;
//		cout<<"b="<<b<<endl;
		if ( (a!=b) && (a!=-b) ){
			IDs.push_back( *itr );
		}
	}
	return IDs;
}

std::vector<int> rendermanager::geometries_by_direction ( const int geometryID )
{
//	cout<<"hej..."<<endl;
	std::vector<int> geometryIDs;
	
	for ( std::vector<rendergeometry_base *>::const_iterator itr = geometries.begin(); itr != geometries.end(); itr++ )
	{
		geometryIDs.push_back( (*itr)->get_id() );
	} 
	
	return geometries_by_direction ( geometryID, geometryIDs );
}

std::vector<int> rendermanager::geometryIDs_by_image_and_direction ( const int combinationID )
{
	int rendererID = renderer_from_combination( combinationID );
	int geometryID = get_geometry_id( find_renderer_index( rendererID ) );

	std::vector<int> imageIDs = images_from_combination( combinationID );		// get the images in the combination
	std::vector<int> rendererIDs = renderers_from_data( imageIDs );				// get the renderers that is connected to at least one of the images
	std::vector<int> geometryIDs = geometries_from_renderers( rendererIDs );	// get the geometry for each renderer
	geometryIDs = geometries_by_direction( geometryID, geometryIDs );			// get the geometries, from the given set, that have a different direction than
																				// the input geomtry (i.e. not the same nor the opposite direction)
	return geometryIDs;
}

std::vector<int> rendermanager::rendIDs_by_image_and_direction ( const int combinationID )
{
	std::vector<int> geom_IDs = geometryIDs_by_image_and_direction ( combinationID );
	std::vector<int> rIDs;
	int my_rendererID = renderer_from_combination( combinationID );

	for(int r=0;r<renderers.size();r++){
		if( renderers[r]->get_id() != my_rendererID ){

			for(int g=0;g<geom_IDs.size();g++){
				if( renderers[r]->geometry_id() == geom_IDs[g] ){
					rIDs.push_back( renderers[r]->get_id() );
				}
			}

		}
	}

	return rIDs;
}

std::vector<rendergeom_image*> rendermanager::geometries_by_image_and_direction ( const int combinationID )
{
//	std::vector<int> geomIDs = rendermanagement.geometryIDs_by_image_and_direction(combinationID);
//	std::vector<int> geomIDs = this->geometryIDs_by_image_and_direction(combinationID); //Jk2
	std::vector<int> rendIDs = this->rendIDs_by_image_and_direction(combinationID); //Jk2

	std::vector<rendergeom_image *> geom;
	for(int i=0;i<rendIDs.size();i++){
//		geom.push_back( rendermanagement.get_geometry(geomIDs[i]) );		
		geom.push_back( (rendergeom_image*)this->get_geometry(rendIDs[i]) );		//JK2
	}
	return geom;
}


std::vector<int> rendermanager::geometries_by_image ( const int combinationID )
{
	std::vector<int> imageIDs = images_from_combination( combinationID );		// get the images in the combination
	std::vector<int> rendererIDs = renderers_from_data( imageIDs );				// get the renderers that is connected to at least one of the images
	std::vector<int> geometryIDs = geometries_from_renderers( rendererIDs );	// get the geometry for each renderer
	
	return geometryIDs;
}

std::vector<int> rendermanager::renderers_from_combinations(const std::vector<int> & combination_ids)
{
	std::vector<int> renderer_ids;
	
	for ( std::vector<int>::const_iterator itr = combination_ids.begin(); itr != combination_ids.end(); itr++ )
		{ renderer_ids.push_back( renderer_from_combination(*itr) ); }
		
    return renderer_ids;
}

std::vector<int> rendermanager::renderers_with_images() const
{
	std::vector<int> rendererIDs;

	for ( std::vector<rendercombination *>::const_iterator itr = combinations.begin(); itr != combinations.end(); itr++ )
	{
		if ( !(*itr)->empty() )
		{	// the rendercombination is not empty
			for ( std::list<rendercombination::renderpair>::const_iterator rpitr = (*itr)->begin(); rpitr != (*itr)->end(); rpitr++ )
			{ 
				if ( dynamic_cast<image_base* >( (rpitr)->pointer ) )
				{	// it is an image
					rendererIDs.push_back( renderer_from_combination( (*itr)->get_id() ) );
				}
			}
		}
	}
	return rendererIDs;
}

factoryIdType rendermanager::get_renderer_type (int ID)
{
    renderer_base * r = get_renderer(ID);
    if (r != NULL)
        {return r->find_typekey();}
    
    return "";
}

int rendermanager::get_combination_id(int rendererIndex)
{
	int c=-1;
	if(rendererIndex>=0 && rendererIndex<renderers.size()){
		c = renderers[rendererIndex]->the_rc->get_id();
	}
	return c;
}
	
int rendermanager::get_geometry_id(int rendererIndex)
    {
    return renderers[rendererIndex]->the_rg->get_id();
    }
	
rendergeometry_base * rendermanager::get_geometry ( int rendererID )
{
	return (rendergeometry_base*)get_renderer(rendererID)->the_rg;
}

rendergeometry_base * rendermanager::get_geometry_from_its_id(int geomID)
{
    for(unsigned int g=0; g < geometries.size(); g++) {
		if(geometries[g]->get_id() == geomID){
			return geometries[g];
		}
	} 

	return NULL;
}



void rendermanager::print_renderers()
{
    for (unsigned int i=0; i < renderers.size(); i++) {
		std::cout << *renderers[i] << std::endl; 
	} 
}

int rendermanager::create_renderer(RENDERER_TYPE rendertype)
    {
    renderer_base *arenderer;
    switch(rendertype)
        {
        case RENDERER_MPR:
            arenderer = new rendererMPR();
			geometries.push_back(new rendergeom_image());
            break;
        case RENDERER_MIP:
            arenderer = new rendererMIP();
			geometries.push_back(new rendergeom_image());
            break;
		case RENDERER_CURVE:
			arenderer = new renderer_curve();
			geometries.push_back(new rendergeom_curve());
			break;
        default:
            arenderer=NULL;
            pt_error::error("Unknown renderer. Not initialized.",pt_error::fatal); break;
        }
    renderers.push_back(arenderer);

    //geometries.push_back(new rendergeom_image());
    arenderer->connect_geometry (geometries[geometries.size()-1]);

    combinations.push_back(new rendercombination());
    arenderer->connect_combination (combinations[combinations.size()-1]);

    viewmanagement.refresh_viewports();
    return arenderer->get_id();
    }

void rendermanager::remove_renderer (int ID)
    {
    remove_renderer(get_renderer(ID));
    }

void rendermanager::remove_renderer (renderer_base * r)
{
    for (std::vector<renderer_base *>::iterator itr = renderers.begin();itr != renderers.end();)
        {
        if ((*itr) == r)
            {
            delete (*itr);
            itr = renderers.erase(itr);
            viewmanagement.refresh_viewports();

            //break;
			}else{
				itr++;
			}
        }
}

void rendermanager::toggle_data(int rendererIndex, int dataID)
{
    renderers[rendererIndex]->the_rc->toggle_data(dataID);
}
	
void rendermanager::enable_data(int rendererID, int dataID)
{
	int rendererIndex = find_renderer_index(rendererID);
	renderers[rendererIndex]->the_rc->enable_data(dataID);
}

void rendermanager::disable_data(int rendererID, int dataID)
{
	int rendererIndex = find_renderer_index(rendererID);
	renderers[rendererIndex]->the_rc->disable_data(dataID);
}

int rendermanager::image_rendered(int rendererIndex, int volID)
    {
    return renderers[rendererIndex]->the_rc->image_rendered(volID);
    }

int rendermanager::renderer_empty(int rendererID)
    {
    int rendererIndex = find_renderer_index(rendererID);

    if (rendererIndex != -1)
        {return (renderers[rendererIndex]->the_rc->empty() ? RENDERER_EMPTY : RENDERER_NOT_EMPTY);}

    return -1;
    }

bool rendermanager::renderer_supports_mode(int rendererIndex, int m)
{
    return renderers[rendererIndex]->supports_mode(m); //JK corrects Visual C++ compile error ("must return a value")
}

/*
std::map<std::string,float> rendermanager::get_values(int index, int px, int py,int sx, int sy)
    {
    if (index != -1)
        {
        return renderers[index]->resolve_values_view(px,py,sx,sy);
        }

    std::map<std::string,float> m;

    return m;  //if ID is invalid
    }
*/

Vector3D rendermanager::get_location (int rendererIndex, int imageID, int px, int py, int sx, int sy)
    {
    return renderers[rendererIndex]->view_to_voxel(px, py, sx, sy,imageID);
    }

void rendermanager::connect_data_renderer(int rendererID, int dataID)
    {
    int renderindex = find_renderer_index(rendererID);

    renderers[renderindex]->connect_data(dataID);
    }
	
void rendermanager::data_has_changed(int ID)
    {
    vector<int> combos = rendermanager::combinations_from_data (ID);

    for (vector<int>::iterator c = combos.begin();c != combos.end(); c++)
        {
//        rendermanagement.combination_update_callback(*c);
        this->combination_update_callback(*c); //JK2
        }
    }

void rendermanager::combination_update_callback (int c_id)
    {
    viewmanagement.refresh_viewports_from_combination(c_id);
    }

void rendermanager::geometry_update_callback (int g_id)
    {
    viewmanagement.refresh_viewports_from_geometry(g_id);
    }

void rendermanager::data_vector_has_changed() 
{
    for (std::vector<renderer_base *>::iterator itr = renderers.begin();itr != renderers.end();itr++)
        {
        //rendercombination * c = (*itr)->the_rc;
        
        (*itr)->the_rc->data_vector_has_changed();
		
        /*if ((*itr)->the_rc->empty())
            {
            //renderer has no images, we might want to kill it - BUT
            //the renderer owns the rendercombination object and is thus
            //the nexus for viewing images, and to let users
            //leave viewports empty and then select images to view
            //we 
            have to leave the renderer active
            
            //see also end of initialize_viewport
            
            remove_renderer(renderers[v]->get_id());
            }*/
        }
}

void rendermanager::set_image_geometry(int renderer_index, Matrix3D * dir)
    {
    ((rendergeom_image*)renderers[renderer_index]->the_rg)->dir=(*dir);
    ((rendergeom_image*)renderers[renderer_index]->the_rg)->refresh_viewports();
    }

void rendermanager::set_image_geometry(int renderer_ID,Vector3D look_at,float zoom)
{
    int index = find_renderer_index(renderer_ID);
    rendergeom_image *r = (rendergeom_image*)renderers[index]->the_rg;
    if(zoom>0){
		r->zoom = zoom;
	}
    r->look_at = look_at;
	r->refresh_viewports();
}

int rendermanager::get_blend_mode (int rendererIndex)
    {
    return renderers[rendererIndex]->the_rc->blend_mode();
    }

rendercombination* rendermanager::get_combination (int ID)
{
    return get_renderer(ID)->the_rc;
}

image_base* rendermanager::get_top_image_from_renderer(int r_id)
{
	int cid = this->get_renderer(r_id)->combination_id();
	return this->get_combination(cid)->top_image<image_base>(); //TODO_R top
}


void rendermanager::set_blendmode(int renderer_index,blendmode mode)
    {
    renderers[renderer_index]->the_rc->blend_mode(mode);
    }

Vector3D rendermanager::center_of_image(const int imageID) const
{
	image_base * image = datamanagement.get_image<image_base>(imageID); //TODO_R HÄR!!!!!
	return image->get_origin() + image->get_orientation() * (image->get_physical_size() / 2);
}

void rendermanager::center2d(const int rendererID, const int imageID)
{
	Vector3D center = center_of_image(imageID);
	rendergeom_image * where = (rendergeom_image*)get_geometry(rendererID);
	
	if(where!=NULL){
		center = center - (where->get_n() * (where->get_n() * (center - where->look_at)));
		set_image_geometry(rendererID, center);
	}
}

void rendermanager::center3d_and_fit(const int rendererID, const int imageID)
{
	image_base * image = datamanagement.get_image<image_base>(imageID); //TODO_R HÄR!!!!!
	int vp_ID = viewmanagement.viewport_from_renderer(rendererID);
	viewport *vp = viewmanagement.get_viewport(vp_ID); //JK Warning rendererID and vp_ID might not be the same in the future....

//	cout<<"center3d_and_fit..."<<endl;
    int r_ind = find_renderer_index(rendererID);
//	cout<<"X="<<renderers[r_ind]->the_rg->get_X()<<endl;
//	cout<<"Y="<<renderers[r_ind]->the_rg->get_Y()<<endl;
	float phys_span_x = image->get_phys_span_in_dir(((rendergeom_image*)renderers[r_ind]->the_rg)->get_X());
	float phys_span_y = image->get_phys_span_in_dir(((rendergeom_image*)renderers[r_ind]->the_rg)->get_Y());
	float phys_ratio = phys_span_y/phys_span_x;
//	cout<<"phys_span_y="<<phys_span_y<<endl;
//	cout<<"phys_span_x="<<phys_span_x<<endl;
//	cout<<"phys_ratio="<<phys_ratio<<endl;

	float vp_x = float(vp->w());
	float vp_y = float(vp->h_pane());
	float vp_ratio = vp_y/vp_x;
//	cout<<"vp_x="<<vp_x<<endl;
//	cout<<"vp_y="<<vp_y<<endl;
//	cout<<"vp_ratio="<<vp_ratio<<endl;
	
	float newzoom=1;
	float a = phys_span_x/vp_x;
	float b = phys_span_y/vp_y;
	if( a > b ){
//		cout<<"a..."<<endl;
		newzoom = (ZOOM_CONSTANT/vp_x)/(a) ; //JK6
	}else{
//		cout<<"b..."<<endl;
		newzoom = (ZOOM_CONSTANT/vp_x)/(b) ; //JK6
	}

	set_image_geometry(rendererID, center_of_image(imageID), newzoom); //JK2
}

void rendermanager::center3d_and_fit( const int imageID )
{
//	std::vector<int> rendererIDs = rendermanagement.renderers_from_data( imageID );
	std::vector<int> rendererIDs = this->renderers_from_data( imageID ); //JK2
	
	for ( std::vector<int>::iterator itr = rendererIDs.begin(); itr != rendererIDs.end(); itr++ )
	{
		center3d_and_fit( *itr, imageID );
	}
}

std::vector<int> rendermanager::images_from_combination ( const int combinationID ) 
{
	std::vector<int> imageIDs;

	rendercombination * combination = get_combination( combinationID );
	
	for ( std::list<rendercombination::renderpair>::const_iterator itr = combination->begin(); itr != combination->end(); itr++ )
	{
		if ( dynamic_cast<image_base* >( (itr)->pointer ) )
		{	// an image
			imageIDs.push_back( itr->ID );
		}
	}
	
	return imageIDs;
}

std::vector<int> rendermanager::data_from_combination ( const int combinationID )
{
	std::vector<int> dataIDs;
	
	rendercombination * combination = get_combination ( combinationID );
	
	for ( std::list<rendercombination::renderpair>::const_iterator itr = combination->begin(); itr != combination->end(); itr++ )
	{
		dataIDs.push_back( itr->ID );
	}
	
	return dataIDs;
}

std::vector<int> rendermanager::get_renderers()
{
	std::vector<int> rendererIDs;

    for ( std::vector<renderer_base *>::iterator itr = renderers.begin(); itr != renderers.end(); itr++ )
	{
		rendererIDs.push_back( (*itr)->get_id() );
	}
	
	return rendererIDs;
}