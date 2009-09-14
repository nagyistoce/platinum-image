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

#include "threshold.h"

#include "FLTKviewport.h"
#include "rendermanager.h"

#include "image_label.hxx"

#include "rawimporter.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;

thresholdparvalue::thresholdparvalue ()
    {
    mode= THRESHOLD_2D_MODE_RECT;

    for (unsigned int d=0;d < THRESHOLDMAXCHANNELS; d++)
        {
        id[d]=NOT_FOUND_ID;
        high[d]=low[d]=0;
        }
    }

int  thresholdparvalue::get_id(int axis)
    {
    int a;
    for (a = 0; a < axis && a < THRESHOLDMAXCHANNELS && id[a] != NOT_FOUND_ID; a++ )
        {}

    if (a == axis)
        {
        return id[axis];
        }

    return NOT_FOUND_ID;
    }

int thresholdparvalue::make_threshold_image ()
    {
    short size[3];
    int num_images=0;
    int result_vol_ID;

    image_base * the_inputs [THRESHOLDMAXCHANNELS];

    image_label<3>  * the_result;

    for (int v=0;id[v] != NOT_FOUND_ID;v++)
        {
        the_inputs [v] = datamanagement.get_image<image_base>(id[v]); //TODO_R HaR!!!!!
        num_images = v+1;
        }

    if (num_images == 0)
        {
        pt_error::error("Segmentation: no images selected in threshold",pt_error::warning);

        return NOT_FOUND_ID;
        }

    for (int d=0; d < 3; d++)
        {size[d]=the_inputs[0]->get_size_by_dim(d);}

    //check that all images have same dimensions
    for (int v = 1; v < num_images; v++)
        {
        for (int d=0; d < 3; d++)
            {
            if (size[d]!=the_inputs[v]->get_size_by_dim(d))
                {
                pt_error::error("Segmentation: image sizes do not match",pt_error::warning);
                
                return NOT_FOUND_ID;
                }
            }
        }

    //input images are expected to all have the same dimensions, just use the first
    //as blueprint for an empty one

    result_vol_ID= datamanagement.create_empty_image(the_inputs[0], VOLDATA_UCHAR);
    the_result=(image_label<3> *)datamanagement.get_image<image_base>(result_vol_ID); //TODO_R HaR!!!!!

    for (short z = 0; z < size[2];z++)
        {
        for (short y = 0; y < size[1];y++)
            {
            for (short x = 0; x < size[0];x++)
                {
//                float rightey;
                bool value=true;
                float t_value [THRESHOLDMAXCHANNELS];

                for (int v = 0; v < num_images && value; v++)
                    {
                    t_value[v] = the_inputs [v]->get_number_voxel(x,y,z);

                    //rect threshold
                    value = value && t_value[v] > low[v] && t_value[v] < high[v];
                    }

                if (value && num_images == 2 && mode==THRESHOLD_2D_MODE_OVAL)
                    {
                    //oval threshold
                    value = value && (std::sqrt(powf((t_value[0]-((high[0]+low[0])/2.0))/((high[0]-low[0])/(high[1]-low[1])),2.0)+powf(t_value[1]-((high[1]+low[1])/2.0),2.0) ) <= (high[1]+low[1])/2.0);
                    }

                the_result->set_voxel(x,y,z,value ? 1 : 0);
//                rightey = the_result->get_number_voxel(x,y,z); //JK - to what use???
                }
            }
        }

    datamanagement.data_has_changed( result_vol_ID, true);

    return result_vol_ID;
    }

threshold_overlay::threshold_overlay(FLTK_Event_pane *fp, int r_index)
    {
    owner=fp;

    threshold=NULL;

//    width=owner->w();
//    height=owner->h();

    overlay_image_data = NULL;
//    overlay_image_data = new unsigned char [width*height*RGBA_pixmap_bpp];
    overlay_image_data = new unsigned char [owner->w()*owner->h()*RGBA_pixmap_bpp];

    rendererIndex= r_index;
    }

threshold_overlay::~threshold_overlay()
    {
    delete []overlay_image_data;
    }

void threshold_overlay::render (thresholdparvalue * t)
    {
    if (t!= NULL)
        {
        //previous threshold can be re-rendered (eg. when panning)
        threshold=t;
        }

    if (threshold !=NULL)
        {
//        rendermanagement.render_threshold (rendererIndex, overlay_image_data, width, height, threshold);
        rendermanagement.render_threshold (rendererIndex, overlay_image_data, owner->w(), owner->h(), threshold);
        //overlay_image->uncache();

        owner->damage (FL_DAMAGE_ALL);
        }
    }

void threshold_overlay::resize ()
{
    delete[] overlay_image_data;
	overlay_image_data = new unsigned char [owner->w()*owner->h()*RGBA_pixmap_bpp];
}

void threshold_overlay::FLTK_draw()
    {    
    if (threshold !=NULL)
        {        
        Fl_RGB_Image overlay_image (overlay_image_data, owner->w(), owner->h(), RGBA_pixmap_bpp, 0);

//        overlay_image.draw(owner->x(),owner->y());
        overlay_image.draw(0,0); //JK2 - is now drawn relative the "top" level window, which is FLTK_Pt_pane..last
        }
    }

void threshold_overlay::expire()
    {
    threshold=NULL;
    }

void threshold_overlay::renderer_index (int index)
    {
    rendererIndex=index;
    }