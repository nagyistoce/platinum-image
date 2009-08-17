/////////////////////////////////////////////////////////////////////////////////
//
//  Rendercombination $Revision$
///
/// Stores list of images and blend mode(s) for renderers
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

#ifndef __imagerenderinfo__
#define __imagerenderinfo__
#include "colormap.h"
#include "datamanager.h"
#include "global.h"
//const int MAXRENDERVOLUMES = 10; //maximum layers for a single renderer

//blending modes
enum blendmode {BLEND_ENDOFLIST=-2,
BLEND_NORENDER,
BLEND_OVERWRITE=0,
BLEND_MAX,
BLEND_MIN,
BLEND_AVG,
BLEND_DIFF,
BLEND_TINT,
BLEND_GREY_PLUS_RBG,
BLEND_GREY_PLUS_RED,
BLEND_GREY_PLUS_BLUE,
NUM_BLEND_MODES,
RENDER_THRESHOLD};  //RENDER_THRESHOLD is not user-selectable, used internally for rendering threshold overlays

//blend_mode_labels defined in viewport.cc

class rendercombination
{

public:
        class renderpair 
		{
		public:
            int ID;
            data_base* pointer;
            blendmode mode;
            renderpair();
            renderpair(const int, data_base*,const blendmode);
        };        

		typedef std::list<renderpair>::const_iterator iterator;

private:
        //int renderimages [MAXRENDERVOLUMES];
        std::list<renderpair> renderdata;

        //colormap colortable;
        int id; //id to identify this combination in callbacks
        static int new_rc_ID;   //unique id to assign newly created combinations
        blendmode blend_mode_;

public:
		rendercombination();
        rendercombination(int ID);  //constructor that populates the renderdata array from the beginning

		void data_vector_has_changed();   //image has been added or removed - update renderlist
        iterator begin() const;
        iterator end() const;
        bool empty() const;

		template<class T> T* top_image()const{ //topmost image
		    for (std::list<renderpair>::const_iterator itr = renderdata.begin();itr != renderdata.end();itr++){
				T* value = dynamic_cast<T* >(itr->pointer); //->getType kan man ha och sen en enumeration med typer
				if (value != NULL){
					return value;
				}
			}
			return NULL;
		}

		string change_top_image(int i){ //Denna har R lagt till för at testa!!!
			string s = "";
			if(!renderdata.empty()){
				if(i >0){
					renderdata.push_back(renderdata.front());
					renderdata.pop_front();
				}else if(i < 0){
					renderdata.push_front(renderdata.back());
					renderdata.pop_back();
				}
				//return the name of the last image to know it is the one to be configured
				s = renderdata.back().pointer->name();
			}
			return s;
		}

        void add_data(int dataID);
        void toggle_data(int dataID);
		void enable_data(int dataID);
		void disable_data(int dataID);
        void remove_data(int dataID);
        int image_rendered(int ID);//for updating various widgets: returns nonzero if
                                   //the image ID is included in this combination
                                   //may return value indicating per-image blending mode for this combination
		int get_id();
        blendmode blend_mode();          //get blendmoide
        void blend_mode(blendmode b);   //set blendmode
    };

#endif