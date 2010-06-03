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

#include "histogram.h"

#include "image_base.h"

#include "datamanager.h"
extern datamanager datamanagement;

// *** histogram_base ***

void histogram_base::clear_pixmap (uchar * image, unsigned int w,unsigned int h)
    {
    //clear image
    unsigned int isize=w*h*RGB_pixmap_bpp;
    for (unsigned int i=0;i < isize; i+=RGB_pixmap_bpp)
        {
        image[i]=image[i+1]=image[i+2]=0;
        }
    }

void histogram_base::reallocate_buckets_if_necessary(int new_num_buckets)
    {
	if (new_num_buckets >0 || this->bucket_vector==NULL){
        //resize(...) isn't used here because this function is called from resize, however the above condition will be false in that case

		if (new_num_buckets !=0){    //change #buckets
            this->num_buckets=new_num_buckets;
		}
		if(this->bucket_vector!= NULL){
			//this->bucket_vector->clear();
			//this->bucket_vector = NULL;
			delete this->bucket_vector;
			}
        this->bucket_vector=new pts_vector<unsigned long>(this->num_buckets);

	}
    }


histogram_base::histogram_base ()
    {
    num_distinct_values =0;
	num_elements_in_hist = 0;
	bucket_vector = NULL;
	if(bucket_vector != NULL){
		delete bucket_vector;
	}
	bucket_vector = new pts_vector<unsigned long>(10);
    }

histogram_base::~histogram_base ()
    {
    if (bucket_vector != NULL){
		//delete []buckets;	//Avoids memory loss... You might crash here if you have written outside the allocated memory
		//bucket_vector->clear();
		delete bucket_vector;
		}
    }

curve_scalar<unsigned long>* histogram_base::get_curve_for_rendering(){
	curve_scalar<unsigned long>* c = new curve_scalar<unsigned long>(0,"histogram curve",0,1);
	c->my_data = bucket_vector;
	return c;
}
int histogram_base::image_ID (int axis)
    {
    return threshold.get_id (axis);
    }

bool histogram_base::ready ()
{
    return readytorender;
}

pts_vector<unsigned long>* histogram_base::get_bucket_vector()
{
	return bucket_vector;
}


// *** histogram_2D_plot ***

void histogram_2D_plot::images (int image_hor,int image_vert)
    {
    threshold.id[0]=image_hor;
    threshold.id[1]=image_vert;

    calculate_from_image_data();
    }

bool histogram_2D_plot::ready ()
    {
    return (datamanagement.get_image<image_base>(threshold.id[0]) != NULL && datamanagement.get_image<image_base>(threshold.id[1]) != NULL ); //TODO_R HaR!!!!!
    }

void histogram_2D_plot::calculate_from_image_data(int foo)
    {
    image_base * vol_v= datamanagement.get_image<image_base>(threshold.id[1]); //TODO_R HaR!!!!!
    image_base * vol_h= datamanagement.get_image<image_base>(threshold.id[0]); //TODO_R HaR!!!!!

    if (vol_v != NULL && vol_h != NULL)
        {
        imagesdifferinsize= ! vol_v->same_size(vol_h);
        }
    }

void histogram_2D_plot::render_(uchar * image, unsigned int w,unsigned int h)
    {
    image_base * vol_v= datamanagement.get_image<image_base>(threshold.id[1]); //TODO_R HaR!!!!!
    image_base * vol_h= datamanagement.get_image<image_base>(threshold.id[0]); //TODO_R HaR!!!!!

    if (!imagesdifferinsize)
        {
        float pixfactor_h=w/vol_h->get_max_float(),
            pixfactor_v=h/vol_v->get_max_float();
        //position scaled based on char (0-255)
        //display pixel input 
        //TODO: take min value into account

        unsigned short vol_size[VOLUMEMAXDIMENSION];

        for (unsigned int d=0;d < VOLUMEMAXDIMENSION; d++)
            {
            vol_size[d]=vol_h->get_size_by_dim(d);
            }
        unsigned int pixpos_x, pixpos_y;
        unsigned short voxpos [3];
        const unsigned char pix_intensity = 255/5; //TEST: intensity of single voxel in point-based histogram
        voxpos[2]=0;
        do {
            voxpos[1]=0;
            do {
                voxpos[0]=0;
                do {
                    pixpos_x=h-vol_h->get_number_voxel(voxpos[0],voxpos[1],voxpos[2])*pixfactor_h;
                    pixpos_y=vol_v->get_number_voxel(voxpos[0],voxpos[1],voxpos[2])*pixfactor_v;

                    image[(pixpos_x+w*pixpos_y)*RGB_pixmap_bpp]+=pix_intensity;
                    image[(pixpos_x+w*pixpos_y)*RGB_pixmap_bpp+1]+=pix_intensity;
                    image[(pixpos_x+w*pixpos_y)*RGB_pixmap_bpp+2]+=pix_intensity;

                    } while (++voxpos[0] < vol_size[0]);
                } while (++voxpos[1] < vol_size[1]);
            } while (++voxpos[2] < vol_size[2]);
        }
    }

histogram_2D::histogram_2D ():histogram_base()
    {
    num_buckets = 96;
    }

histogram_2D::~histogram_2D () {
    if (bucket_vector != NULL)
        {delete []highlight_data;}
    }

void histogram_2D::images (int image_hor,int image_vert)
    {
    threshold.id[0]=image_hor;
    threshold.id[1]=image_vert;

    calculate_from_image_data();
    }

bool histogram_2D::ready ()
    {
    return readytorender;
    }

thresholdparvalue histogram_2D::get_threshold (float h_min,float h_max, float v_min, float v_max, int mode)
    {
    threshold.mode=mode;

    image_base * vol_v= datamanagement.get_image<image_base>(threshold.id[1]); //TODO_R HaR!!!!!
    image_base * vol_h= datamanagement.get_image<image_base>(threshold.id[0]); //TODO_R HaR!!!!!

    if (vol_h == NULL)
        {threshold.id[0]=NOT_FOUND_ID;}

    if (vol_v == NULL)
        {threshold.id[1]=NOT_FOUND_ID;}

    //TODO: account for nonzero image minimum

    if (vol_v != NULL && vol_h != NULL)
        {
        threshold.low [0]=h_min*vol_h->get_max_float();
        threshold.low [1]=v_min*vol_v->get_max_float();

        threshold.high [0]=h_max*vol_h->get_max_float();
        threshold.high [1]=v_max*vol_v->get_max_float();
        }
    else
        {
        threshold.low [0]=threshold.high [0]=threshold.low [1]=threshold.high [1]=0;
        }

    return threshold;
    }

void histogram_2D::calculate_from_image_data(int new_num_buckets)
    {
    if (new_num_buckets !=0 || bucket_vector==NULL)
        {
        if (new_num_buckets !=0)    //change #buckets
            {num_buckets=new_num_buckets;}

        //highlight_data is expected to be created and deleted
        //along with buckets

        if (bucket_vector != NULL)
            {
            delete [] highlight_data;
            highlight_data=NULL;

            bucket_vector->clear();
            bucket_vector=NULL;
            }

        bucket_vector=new pts_vector<unsigned long>(num_buckets*num_buckets);
        highlight_data=new bool [num_buckets*num_buckets];
        }

    readytorender=false;

	image_base * vol_v= datamanagement.get_image<image_base>(threshold.id[1]); //TODO_R HaR!!!!!
    image_base * vol_h= datamanagement.get_image<image_base>(threshold.id[0]); //TODO_R HaR!!!!!

    if (vol_v != NULL && vol_h != NULL)
        {
        //true if images have same dimensions
        readytorender=vol_v->same_size(vol_h);

        //reset buckets
        for (unsigned short j = 0; j < num_buckets; j++)
            {
            for (unsigned short i = 0; i < num_buckets; i++)
                {
                bucket_vector->at(i+j*num_buckets)=0;
                highlight_data[i+j*num_buckets]=false;
                }
            }
        }

    if (readytorender)
        {
        //ready to calculate, actually

        float scalefactor_h=(num_buckets-1)/vol_h->get_max_float(),
            scalefactor_v=(num_buckets-1)/vol_v->get_max_float();
        float value_h, value_v;
        unsigned short bucketpos_x, bucketpos_y;
        unsigned short voxpos [3];
        unsigned short vol_size[VOLUMEMAXDIMENSION];

        for (unsigned int d=0;d < VOLUMEMAXDIMENSION; d++)
            {
            vol_size[d]=vol_h->get_size_by_dim(d);
            }

        bucket_max=0;
        voxpos[2]=0;
		num_elements_in_hist=0;
        do {
            voxpos[1]=0;
            //in_region_z= voxpos[2] >= region_min[2] && voxpos[2] <= region_max[2];
            do {
                voxpos[0]=0;
                //in_region_y= voxpos[1] >= region_min[1] && voxpos[1] <= region_max[1];
                do {
                    value_h=vol_h->get_number_voxel(voxpos[0],voxpos[1],voxpos[2]);
                    value_v=vol_v->get_number_voxel(voxpos[0],voxpos[1],voxpos[2]);
					num_elements_in_hist++;

                    bucketpos_x=value_h*scalefactor_h;
                    bucketpos_y=value_v*scalefactor_v;

                    bucket_max=std::max(bucket_vector->at(bucketpos_x+bucketpos_y*num_buckets)++,bucket_max);
			
                    } while (++voxpos[0] < vol_size[0]);
                } while (++voxpos[1] < vol_size[1]);
            } while (++voxpos[2] < vol_size[2]);

        //heuristic for histogram calibration:
        //calculate mean histogram intensity and use twice that as max
        bucket_mean=0;
        for (unsigned short j = 0; j < num_buckets; j++)
            {
            for (unsigned short i = 0; i < num_buckets; i++)
                {
                bucket_mean+=bucket_vector->at(i+j*num_buckets)/(num_buckets*num_buckets);
                }
            }


        render_max = bucket_mean;
        //render_max=(float)((float)(vol_size[0]*vol_size[1]*vol_size[2])/(num_buckets*num_buckets*4));
        //render_max=(float)((float)(vol_size[0]*vol_size[1]*vol_size[2])/(float)(bucket_max));
        }
    }

    void histogram_2D::highlight (regionofinterest * region)
        {
        image_base * vol_v= datamanagement.get_image<image_base>(threshold.id[1]); //TODO_R HaR!!!!!
        image_base * vol_h= datamanagement.get_image<image_base>(threshold.id[0]); //TODO_R HaR!!!!!

        if (readytorender!=true)
            {calculate_from_image_data();}

        //if images matched, etc, histogram is ready to be highlighted
        if (readytorender==true)
            {
            //reset highlight
            for (long j = 0; j < num_buckets; j++)
                {
                for (long i = 0; i < num_buckets; i++)
                    {
                    highlight_data[i+j*num_buckets]=false;
                    }
                }

            float scalefactor_h=(num_buckets-1)/vol_h->get_max_float(),
                scalefactor_v=(num_buckets-1)/vol_v->get_max_float();
            float value_h, value_v;
            unsigned short bucketpos_x, bucketpos_y;
            unsigned short voxpos [3];

            unsigned short region_min[THRESHOLDMAXCHANNELS], region_max[THRESHOLDMAXCHANNELS]; //integer values of ROI boundaries

            if (region != NULL)
                {
                unsigned short vol_size[VOLUMEMAXDIMENSION];

                for (unsigned int d=0;d < VOLUMEMAXDIMENSION; d++)
                    {
                    vol_size[d]=vol_h->get_size_by_dim(d);
                    }

                for (unsigned int d=0;d < 3; d++)
                    {
                    region_min[d]=std::min(std::max ((float)0,region->start[d]),(float)vol_size[d]-1);
                    region_max[d]=std::max((float)0,std::min ((float)vol_size[d]-1,region->start[d]+region->size[d]));
                    }
                }

            voxpos[2]=region_min[2];
            do {
                voxpos[1]=region_min[1];
                do {
                    voxpos[0]=region_min[0];
                    do {
                        value_h=vol_h->get_number_voxel(voxpos[0],voxpos[1],voxpos[2]);
                        value_v=vol_v->get_number_voxel(voxpos[0],voxpos[1],voxpos[2]);

                        bucketpos_x=value_h*scalefactor_h;
                        bucketpos_y=value_v*scalefactor_v;

                        highlight_data [bucketpos_x+bucketpos_y*num_buckets]=true; 

                        } while (++voxpos[0] < region_max[0]);
                    } while (++voxpos[1] < region_max[1]);
                } while (++voxpos[2] < region_max[2]);
            }
        }

    void histogram_2D::render_(uchar * image, unsigned int w,unsigned int h)
        {
        float slope_x, slope_y;

        slope_x = num_buckets/(float)w;
        slope_y = num_buckets/(float)h;

        unsigned char intensity_value;
        float intensity_scale= 255.0/(float)render_max;

        if (readytorender)
            {
            for (unsigned short y=1; y < h; y++)
                {
                for (unsigned short x=0; x < w; x++)
                    {
                    float hist_pos_x, hist_pos_y;
                    int hist_index;

                    hist_pos_x=slope_x*x;
                    hist_pos_y=slope_y*y;

                    hist_index=((unsigned short) hist_pos_x)+((unsigned short) hist_pos_y)*num_buckets;
                    intensity_value=std::min((float)this->bucket_vector->at(hist_index)*(float)intensity_scale, (float)255.0);

                    image[(x+(h-y)*w)*RGB_pixmap_bpp]=(highlight_data [hist_index] ? intensity_value/2 : intensity_value);
                    image[(x+(h-y)*w)*RGB_pixmap_bpp+1]=(highlight_data [hist_index] ? intensity_value/2 + 127 : intensity_value);
                    image[(x+(h-y)*w)*RGB_pixmap_bpp+2]=(highlight_data [hist_index] ? intensity_value/2 + 127 : intensity_value);
                    }
                }
            }
        }