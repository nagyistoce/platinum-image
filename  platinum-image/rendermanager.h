//////////////////////////////////////////////////////////////////////////
//
//  Rendermanager $Revision$
///
/// Keeps a list of renderer objects and handles communication between
/// viewport and renderer.
///
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

#ifndef __rendermanager__
#define __rendermanager__

#include <vector>

#include "ptmath.h"

#include "renderer_base.h"
#include "rendercombination.h"
#include "rendergeometry.h"
#include "global.h"
#include "FLTKviewport.h"

//-1 is used as empty status value (when the renderer could not be found),
//so these values are chosen to be separate from that
#define RENDERER_EMPTY 1
#define RENDERER_NOT_EMPTY 0

class rendermanager
    {
    private:
        std::vector<renderer_base*>			renderers;
        std::vector<rendercombination*>		combinations;
        std::vector<rendergeometry_base*>	geometries;
        
    public:
        rendermanager ();
        ~rendermanager();

//		static listedfactory<renderer_base> renderer_factory;
		static listedfactory<FLTKpane> pane_factory;
        
        void print_renderers();
        int create_renderer(RENDERER_TYPE rendertype);
        void remove_renderer(renderer_base *r);
        void remove_renderer(int ID);
        int find_renderer_index(int uniqueID);
        int find_renderer_id(int index);
        renderer_base* get_renderer(int ID);
        void connect_data_renderer(int rendererID, int dataID);

		void data_vector_has_changed();
        void data_has_changed(int ID);

//        void move(int rendererIndex, float panX, float panY, float panZ=0, float scale=1);//alter rendergeometry (pan,zoom)
        void render(int rendererIndex, unsigned char *rgbimage, int rgbXsize, int rgbYsize);
        void render_thumbnail(unsigned char *rgb, int rgb_sx, int rgb_sy, int dataID);
        void render_threshold(int rendererIndex, unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold);
       

        void toggle_data(int rendererIndex, int dataID);   //turn image on or off in a combination, triggered by menu selection
		void enable_data(int rendererID, int dataID);	// turn image (data) on or leave it on
		void disable_data(int rendererID, int dataID);	// turn image (data) off or leave it off
		void disable_all_data(int rendererID);			// turn image (data) off or leave it off
		
		
        //int get_renderer_type(int rendererIndex);
        int get_blend_mode(int rendererIndex);
        bool renderer_supports_mode(int rendererIndex, int m);
        
        factoryIdType get_renderer_type(int ID);
        
        int get_combination_id(int rendererIndex);              //get id of combination object associated with renderer rendererIndex
        rendercombination* get_combination(int ID);

        int get_geometry_id(int rendererIndex);                 //get id of geometry object associated with renderer rendererIndex
		rendergeometry_base* get_geometry(int rendererID);			// get rendergeometry from renderer id
		rendergeometry_base* get_geometry_from_its_id(int geomID);
		std::vector<rendergeometry_base*> get_geometries(){return geometries;}; //RN fix

		image_base* get_top_image_from_renderer(int r_id);

																
        void combination_update_callback(int c_id);
        std::vector<int> combinations_from_data(int dataID);    //return any combinations containing the data object with dataID
		std::vector<int> renderers_from_data(int dataID);	// return any renderer connected to this data id
	
		std::vector<int> renderers_from_data(const std::vector<int> & dataIDs);	// return a set of renderers. each renderer in the returned set
																					// holds at least one of the items in the data set

		int renderer_from_combination(const int combination_id) const; // return a renderer id from a combination id
		std::vector<int> renderers_from_combinations(const std::vector<int> & combination_ids);	// return a set of renderer ids from a set of combination ids
		
		int renderer_from_geometry(const int geometry_id) const;	 // return a renderer id from a geometry id
		std::vector<int> geometries_from_renderers ( const std::vector<int> & renderer_ids );	// return a set of geometries from a set of renderers
	
		std::vector<int> geometries_by_direction ( const int geometryID, const std::vector<int> & geometryIDs );	
		// return geometries from the given set that have a different direction than the input geometry i.e. not the same nor the opposite direction
		std::vector<int> geometries_by_direction ( const int geometryID );	// return geometries that has a different direction than the input geometry
																			// i.e not the same nor the opposite direction
		std::vector<int> geometryIDs_by_image_and_direction ( const int combinationID );	// return geometries that holds at least one of the images in the input combination
																						// and have a different direction than the input geometry (i.e. not the same nor the opposite direction)
		std::vector<rendergeom_image*> geometries_by_image_and_direction ( const int combinationID );	// return geometries that holds at least one of the images in the input combination
																						// and have a different direction than the input geometry (i.e. not the same nor the opposite direction)
		std::vector<int> rendIDs_by_image_and_direction ( const int combinationID );	// return geometries that holds at least one of the images in the input combination
																						
		std::vector<int> geometries_by_image ( const int combinationID );	// return geometries that holds at least one of the images in the input combination
		
		std::vector<int> renderers_with_images () const;
				
        void geometry_update_callback (int g_id);

        int image_rendered(int rendererIndex, int volID);  //pass-through for checking if
                                                            //a certain image (id) is part of a combination
                                                            //for renderer rendererIndex

        int image_at_priority (int rendererIndex, int priority);   //pass through for getting visible
                                                                    //image ID at certain priority (for iterating)
                                                                    //returns NOT_FOUND_ID past end

        int renderer_empty (int rendererID);       //returns whether renderer has no images to render or not
//        std::map<std::string,float> get_values(int rendererIndex, int px, int py,int sx, int sy);
        Vector3D get_location(int rendererIndex, int imageID, int px, int py, int sx, int sy);

//        Matrix3D get_direction(int renderer_index);
        void set_image_geometry(int renderer_ID, Vector3D look_at,float zoom = 0);
        void set_image_geometry(int renderer_index, Matrix3D * dir);
		void use_other_geometry_for_all_other_renderers(int renderer_index_src=0);
		void use_other_geometry_for_renderer(int renderer_index_target, int renderer_index_src=0);

        void set_blendmode(int rend_index,blendmode mode);   //sets combination-wide blend mode, if blendmode is later defined
                                                             //for each image in a combination, this should set all of them
		Vector3D rendermanager::center_of_image(const int imageID) const;
		void center2d(const int rendererID, const int imageID);															 

		void center3d_and_fit(const int rendererID, const int imageID);
		void center3d_and_fit(const int imageID);
		
		std::vector<int> images_from_combination ( const int combinationID );	// return all image ids in a combination
		
		std::vector<int> data_from_combination ( const int combinationID );		// return all data ids in a combination
		
		std::vector<int> get_renderers();

    };

#endif
