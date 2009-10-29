//////////////////////////////////////////////////////////////////////////////////
//
//  RendererMPR $Revision$
///
/// arbitrary 2D slices rendered by scanline
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

#define _rendererMPR_cc_
#include "rendererMPR.h"
#include "viewmanager.h"
#include "rendermanager.h"

#define MAXRENDERVOLUMES 50

//Use arbitrary-angle rendering code
#define USE_ARBITRARY

extern datamanager datamanagement;
extern viewmanager viewmanagement;
extern rendermanager rendermanagement;

using namespace std;

template<class T>                   
T signed_ceil(T & x){   //ceil that returns rounded absolute upwards
    return (x < 0 ? floor (x) : ceil (x));
    }

rendererMPR::rendererMPR():renderer_image_base()
{
	the_rg = new rendergeom_image();
	original_rg = the_rg;
}

void rendererMPR::connect_data(int dataID)
    {
    //TEST: wrapper, this should be done directly by rendermanagement
    the_rc->add_data(dataID);
	}

void rendererMPR::paint_overlay(int h_offset, int vp_w, int vp_h_pane, bool paint_rendergeometry)
{
//	cout<<"rendererMPR::paint_overlay..("<<vp_w<<" "<<vp_h_pane<<") rc_id="<<the_rg->get_id()<<endl;
	paint_slice_locators_to_overlay(h_offset, vp_w, vp_h_pane, (rendergeom_image*)the_rg, the_rc);
	if(paint_rendergeometry){
		paint_rendergeometry_to_overlay(h_offset, vp_w, vp_h_pane, (rendergeom_image*)the_rg, the_rc);
	}
	paint_data_name_order_to_overlay(h_offset, vp_w, vp_h_pane, the_rc); //JK***
}

Vector3D rendererMPR::view_to_world(int vx, int vy, int sx, int sy) const
{
    Vector3D viewCentered,world;
    vector<float> v;
//    float viewmin = std::min(sx,sy); //777
    float viewmin = sx; //JK6
    
    viewCentered[0]=vx-sx/2;
    viewCentered[1]=vy-sy/2;
    viewCentered[2]=0;
    
    //transform to world coordinates
    world = ((rendergeom_image*)the_rg)->view_to_world_matrix(viewmin) * viewCentered;
	world = world + ((rendergeom_image*)this->the_rg)->look_at;
//	cout<<"view_to_world="<<vx<<","<<vy<<","<<sx<<","<<sy<<","<<"("<<world<<")("<<((rendergeom_image*)this->the_rg)->zoom<<")"<<endl;
    return world;
}

Vector3D rendererMPR::view_to_voxel(int vx, int vy,int sx,int sy,int imageID) const
{
    if (imageID != -1)
        {
        return datamanagement.get_image<image_base>(imageID)->world_to_voxel (view_to_world(vx, vy,sx,sy)); //TODO_R HaR!!!!!
        }
    else
        {    
        Vector3D v;
        
        image_base * image = the_rc->top_image<image_base>(); //TODO_R top
        
        if (image !=NULL)
            {
            v = image->world_to_voxel (view_to_world(vx, vy,sx,sy));
            
            for (int d = 0;d < 3;d++)
                {if (v[d] >= image->get_size_by_dim(d))
                    {       
                    Vector3D n;
                    n.Fill (-1);
                    
                    return n;
                    }
                }
            
            return v;
            }
        else
            {
            //no image there
            Vector3D n;
            n.Fill (-1);
            
            return n;
            }
        }
    //return Vector3D();
}

bool rendererMPR::supports_mode (int m)
{
    switch (m)
        {
        case BLEND_OVERWRITE:
        case BLEND_MAX:
        case BLEND_MIN:
        case BLEND_AVG:
        case BLEND_DIFF:
        case BLEND_TINT:
        case BLEND_GREY_PLUS_RBG:
        case BLEND_GREY_PLUS_RED:
        case BLEND_GREY_PLUS_BLUE:
        case RENDER_THRESHOLD:
            return true;
            break;
        default:
            return false;
        }
}

void rendererMPR::render_thumbnail (unsigned char *rgb, int rgb_sx, int rgb_sy, int image_ID)
{
    rendercombination rc = rendercombination(image_ID);
    rendergeom_image rg = rendergeom_image();
    
    render_( rgb, rgb_sx, rgb_sy,&rg,&rc,NULL);
}

void rendererMPR::render_threshold (unsigned char *rgba, int rgb_sx, int rgb_sy, thresholdparvalue * threshold)
{
    render_( rgba, rgb_sx, rgb_sy,(rendergeom_image*)the_rg,the_rc,threshold);
}

void rendererMPR::render_position(unsigned char *rgb, int rgb_sx, int rgb_sy)
{
    render_( rgb, rgb_sx, rgb_sy,(rendergeom_image*)the_rg,the_rc,NULL);
}




//render orthogonal slices using memory-order scanline
void rendererMPR::render_(uchar *pixels, int rgb_sx, int rgb_sy, rendergeom_image *rg, rendercombination *rc, thresholdparvalue * threshold)
{
//	cout<<"rendererMPR used"<<endl;

    if(rc->empty()){       //*** no images: exit ***
        return;
	}
        
    int vol_count; //number of images to render in this call
    Vector3D voxel_offset[MAXRENDERVOLUMES]; //The voxel index ,for each volume, that should be displayed in the top left corner of the viewport
    float rgb_min_norm=min(float(rgb_sx),float(rgb_sy));
    vol_count = static_cast<int> (std::distance(rc->begin(), rc->end()));
    
    Vector3D screen_center = create_Vector3D(rgb_sx, rgb_sy, 0);
    screen_center/=2;
    
    //fill background color, standard MIN blending leaves background white, instead background is marked
    //with zero intensity in G+B channels, and tinted in a second pass
    
    blendmode blend_mode = rc->blend_mode();
    if(threshold != NULL){ 
		blend_mode = RENDER_THRESHOLD;
	}
    
    switch(blend_mode){
        case RENDER_THRESHOLD:
            for(long p=0; p < rgb_sx*rgb_sy*RGBA_pixmap_bpp; p +=RGBA_pixmap_bpp){
                pixels[p] = pixels[p + 1] = pixels[p + 2]= pixels[p + 3]=0;
			}
            break;
        case BLEND_MIN:
            for(long p=0; p < rgb_sx*rgb_sy*RGB_pixmap_bpp; p +=RGB_pixmap_bpp){
                pixels[p] = 255;
                pixels[p + 1] = pixels[p + 2] = 0;
                }
            break;
        default:
            for(long p=0; p < rgb_sx*rgb_sy*RGB_pixmap_bpp; p +=RGB_pixmap_bpp)
                {pixels[p] = pixels[p + 1] = pixels[p + 2]=0;}
        }
       
    #pragma mark *** Per-image render loop ***
  
    int the_image = 0;

    for(rendercombination::iterator pairItr = rc->begin();pairItr != rc->end();pairItr++){ 
        image_base *the_image_pointer, *the_other_image_pointer;
        
		pt_error::error_if_null(pairItr->pointer,"Rendered data object is NULL");//Crash here when closing an image

        the_image_pointer = dynamic_cast<image_base *> (pairItr->pointer);
        
        bool OKrender = the_image_pointer != NULL && the_image_pointer->is_supported(renderer_type());
        
        if(blend_mode == RENDER_THRESHOLD){
            the_image_pointer       = datamanagement.get_image<image_base>(threshold->id[0]); //TODO_R HaR!!!!!
            the_other_image_pointer = datamanagement.get_image<image_base>(threshold->id[1]); //TODO_R HaR!!!!!
            
            OKrender = the_image_pointer != NULL && the_other_image_pointer != NULL && the_image_pointer->is_supported(renderer_type());
        }
        
        //render images in first pass, points in second
        if(OKrender){ 

			//color for tint mode, Note that the order is set to RED, BLUE, GREEN 
            int tint_r=(((the_image % 3) ==0) ^ (the_image > 2));
            int tint_g=(((the_image % 3) ==1) ^ (the_image > 2));	
            int tint_b=(((the_image % 3) ==2) ^ (the_image > 2));
        
           //pixel fill start & end points, used in common blend mode code
			long fill_y_start;
		    long fill_y_end;
            long fill_x_start;
			long fill_x_end;
            
            //RGB value in loop
            RGBvalue value = RGBvalue();
            bool threshold_value = false;
            

//            const float scale = rgb_min_norm / ZOOM_CONSTANT; //constant = the number of mms that should fit inside a viewport at zoom 1
            const float scale = rgb_sx / ZOOM_CONSTANT; //constant = the number of mms that should fit inside a viewport at zoom 1 //JK zoom_const redef
			//"(rc->dir * screen_center)/(rc->zoom * scale)"  
			//This rotates/scales/returns vector from top left viewport corner to viewport center in world coordinates...
			voxel_offset[the_image] = the_image_pointer->world_to_voxel( rg->look_at - ( (rg->dir * screen_center) / (rg->zoom * scale) ) );

            //start position in image
            Vector3D start,end;
            start = voxel_offset[the_image];
                        

			//set slope to size of render plane in unit coordinates
			//------------------
			//this matrix transforms translations in viewport pixels to translations in volume voxels
			Matrix3D slope = rg->get_scan_line_slop_matrix(the_image_pointer,scale);

			//------------------
/*
			Matrix3D orientation_inv;
            orientation_inv = the_image_pointer->get_orientation().GetInverse();
//			Vector3D origin = the_image_pointer->get_origin();
                        
            Matrix3D slope;			//this matrix transforms translations in viewport pixels to translations in volume voxels
            slope = orientation_inv;
			
			Matrix3D inv_size;
			inv_size = the_image_pointer->get_voxel_resize().GetInverse();
            slope = inv_size * slope;
            
            slope/= rg->zoom * scale;
*/
			//------------------

			//calculate the "slope" in volume voxels for x-steps in viewpost pixels 
            Vector3D slope_x = create_Vector3D(1,0,0);
            slope_x = slope * rg->dir * slope_x;
			//rc->dir is required to make other views than axial "z-dir" possible...

			//calculate the "slope" in volume voxels for y-steps in viewpost pixels 
            Vector3D slope_y = create_Vector3D(0,1,0);
            slope_y = slope * rg->dir * slope_y;
			//rc->dir is required to make other views than axial "z-dir" possible...


            //position to read in voxel data grid
            Vector3D vox;
			int pixmap_addr;
            
            // Render loop
            
            //1. iterate Y and determine new position of horizontal scanline
            //2. check that position is within data bounds
            //3. render pixel
            //4. move along scanline (iterate x)
            //5. repeat
             
            for(fill_y_start=0; fill_y_start < rgb_sy; fill_y_start++){
                fill_y_end=fill_y_start+1;
                vox=start+slope_y*(float)fill_y_start;
                
                for(fill_x_start=0; fill_x_start < rgb_sx; fill_x_start++){
                    fill_x_end=fill_x_start+1;

                    //get actual value in data, this has been scaled to fit the range of unsigned char
//					if( the_image_pointer->is_voxelpos_within_image_3D(vox) ){
					if( vox[0]>0&&vox[1]>0&&vox[2]>0 && vox[0]<the_image_pointer->get_size_by_dim(0)&&vox[1]<the_image_pointer->get_size_by_dim(1)&&vox[2]<the_image_pointer->get_size_by_dim(2) ){

                        if(blend_mode == RENDER_THRESHOLD){
                            float t_value[2];
                            
                            t_value[0] = the_image_pointer->get_number_voxel(vox[0],vox[1],vox[2]);
                            t_value[1] = the_other_image_pointer->get_number_voxel(vox[0],vox[1],vox[2]);
                            
                            //rect threshold
                            threshold_value = ( t_value[0] > threshold->low[0] && t_value[0] < threshold->high[0] &&
                                                t_value[1] > threshold->low[1] && t_value[1] < threshold->high[1]);
                            
                            if(threshold_value && threshold->mode==THRESHOLD_2D_MODE_OVAL){
                                //oval threshold
                                threshold_value = (sqrt(powf((t_value[0]-((threshold->high[0]+threshold->low[0])/2.0))/((threshold->high[0]-threshold->low[0])/(threshold->high[1]-threshold->low[1])),2.0)+powf(t_value[1]-((threshold->high[1]+threshold->low[1])/2.0),2.0) ) <= (threshold->high[1]+threshold->low[1])/2.0);
							}
						}


                        else{ 
							the_image_pointer->get_display_voxel(value,vox[0],vox[1],vox[2]); //TODO kolla vad denna gor egentligen
						}
                        
                        
                        for(long rgb_fill_y=fill_y_start; (rgb_fill_y <  fill_y_end);rgb_fill_y++){
                            for(long rgb_fill_x=fill_x_start;(rgb_fill_x < fill_x_end);rgb_fill_x++){
								pixmap_addr = RGB_pixmap_bpp*(rgb_fill_x+rgb_sx*rgb_fill_y);

                                switch (blend_mode){
                                    case BLEND_OVERWRITE:
                                        value.write(pixels+pixmap_addr);
                                        break;
                                        
                                    case BLEND_MIN:
                                        if(pixels[pixmap_addr] >= value.mono()){
                                            //we can assume that the R value represents total pixel intensity
                                            //because previous pixel value was set with the same mode
                                            
                                            //more than or equal (>=) above is important because
                                            //we want to replace the background even for the r=255 case
                                            //to mask out background color
                                            
                                            value.write(pixels+pixmap_addr);
                                            }
                                        break;
                                        
                                    case BLEND_MAX:
                                        if (pixels[pixmap_addr] < value.mono())
                                            {
                                            //we can assume that the R value represents total pixel intensity
                                            //because previous pixel value was set with the same mode
                                            
                                            value.write(pixels+pixmap_addr);
                                            }
                                        break;
                                        
                                    case BLEND_AVG:
                                        {
                                            pixels[pixmap_addr] += value.r()/vol_count;
                                            pixels[pixmap_addr + 1] += value.g()/vol_count;
                                            pixels[pixmap_addr + 2] += value.b()/vol_count;
                                        }
                                        break;
                                    case BLEND_DIFF:
                                        {
											if(the_image==0){ //if first image --> Grey scale
		                                        value.write(pixels+pixmap_addr);
											}else if(the_image==1){ //Fist image minus this...
 												pixels[pixmap_addr] = abs( pixels[pixmap_addr] - value.r() );
												pixels[pixmap_addr + 1] = abs( pixels[pixmap_addr+1] - value.g() );
												pixels[pixmap_addr + 2] = abs( pixels[pixmap_addr+2] - value.b() );
											}
	
                                        }
                                        break;
                                        
                                    case BLEND_TINT:
                                        pixels[pixmap_addr] += tint_r*value.mono();
                                        pixels[pixmap_addr + 1] += tint_b*value.mono();
                                        pixels[pixmap_addr + 2] += tint_g*value.mono();
                                        break;

									case BLEND_GREY_PLUS_RBG:
										if(the_image==0){ //if first image --> Grey scale
                                            value.write(pixels+pixmap_addr);

										}else if(the_image==1){ //--> R
											//pixels[pixmap_addr] *= (1-value.r()/255.0);
	                                        pixels[pixmap_addr + 1] *= (1-value.g()/255.0);
			                                pixels[pixmap_addr + 2] *= (1-value.b()/255.0);

										}else if(the_image==2){ //--> B
											pixels[pixmap_addr] *= (1-value.r()/255.0);
	                                        pixels[pixmap_addr + 1] *= (1-value.g()/255.0);
//			                                pixels[pixmap_addr + 2] *= (1-value.b()/255.0);

										}else if(the_image==3){ //--> G
											pixels[pixmap_addr] *= (1-value.r()/255.0);
//	                                        pixels[pixmap_addr + 1] *= (1-value.g()/255.0);
			                                pixels[pixmap_addr + 2] *= (1-value.b()/255.0);
										}
                                        break;

									case BLEND_GREY_PLUS_RED:
										if(the_image==0){ //if first image --> Grey scale
//											value.r(value.r()/vol_count);
//											value.g(value.g()/vol_count);
//											value.b(value.b()/vol_count);
                                            value.write(pixels+pixmap_addr);
										}else{ //--> weight in the red direction...
//											pixels[pixmap_addr] *= value.r()/255; // /vol_count
	                                        pixels[pixmap_addr + 1] *= (1-value.g()/255.0);
			                                pixels[pixmap_addr + 2] *= (1-value.b()/255.0);
										}
                                        break;

									case BLEND_GREY_PLUS_BLUE:
										if(the_image==0){ //if first image --> Grey scale
                                            value.write(pixels+pixmap_addr);
										}else{ //--> weight in the blue direction...
											pixels[pixmap_addr] *= (1-value.r()/255.0);
	                                        pixels[pixmap_addr + 1] *= (1-value.g()/255.0);
//			                                pixels[pixmap_addr + 2] *= (1-value.b()/255.0);
										}
                                        break;
                                        
                                    case RENDER_THRESHOLD:
                                        if (threshold_value)
                                            {
                                            RGBAvalue value = RGBAvalue (IMGELEMCOMPMAX,0,0, IMGELEMCOMPMAX);
                                            value.write(pixels+RGBA_pixmap_bpp*(rgb_fill_x+rgb_sx*rgb_fill_y));
                                            }
                                        break;
                                    default:
                                        {
                                            //suppress GCC enum warning
                                        }
                                    } // switch (blend_mode)
                                
                                } //rgb_fill_x loop
                            
                            } //rgb_fill_y loop
                        
                        } //if within data bounds
                    vox+=slope_x;
                    } //fill_x_start loop
                    } //fill_y_start loop
            
            if (blend_mode== BLEND_MIN)
			{
                for (long p=0; p < rgb_sx*rgb_sy*RGB_pixmap_bpp; p +=RGB_pixmap_bpp )
				{
                    if (pixels[p] != pixels[p+1])
					{
                        //unaltered background, fill with final background color
                        
                        pixels[p] = pixels[p + 1] = 0;
                        pixels[p + 2] = 127;
					}
				}
			}
            
		} //the_image_pointer != NULL
        //TODO_R add drawing of the additional data here!
		if(the_image_pointer->draw_additional_data)
			draw_additional_data(the_image_pointer, rg, pixels, rgb_sx, rgb_sy);
        the_image++;
	} //per-image loop
    	

//	draw_slice_locators(pixels, rgb_sx, rgb_sy, rc, rc);


    #pragma mark *** per-point render loop ***
    	
    for (rendercombination::iterator pairItr = rc->begin();pairItr != rc->end();pairItr++)  
	{
        point *pointPointer = dynamic_cast<point *> (pairItr->pointer);
				
				
		if ( !dynamic_cast<point *> (pairItr->pointer) )
		{	// Not a point
			if ( point_collection * points = dynamic_cast<point_collection *> (pairItr->pointer) )
			{	// A point_collection
				point_collection::pointStorage::iterator iter;
				for (iter = points->begin(); iter != points->end(); iter++)
				{
					vector<int> on_spot_rgb(3);
					on_spot_rgb[0] = 0;
					on_spot_rgb[1] = 0;
					on_spot_rgb[2] = 255;

					if ( points->get_active() == iter->first )
					{	// this point is active
						on_spot_rgb[0] = 255;
						on_spot_rgb[1] = 0;
						on_spot_rgb[2] = 0;
					}

					draw_cross ( pixels, rgb_sx, rgb_sy, rg, points->get_point(iter->first), on_spot_rgb );
				}
			}
		}


        if (pointPointer != NULL )
		{	// A point (a point_collection with one element)
			vector<int> on_spot_rgb(3);
			on_spot_rgb[0] = 0;
			on_spot_rgb[1] = 0;
			on_spot_rgb[2] = 255;
		
			draw_cross ( pixels, rgb_sx, rgb_sy, rg, pointPointer->get_origin(), on_spot_rgb );
		}
	
	}//point rendering loop

}//render_ function


void rendererMPR::draw_additional_data(image_base* the_image_pointer, rendergeom_image* rg,  uchar *pixels, int rgb_sx, int rgb_sy){
	std::vector<int> col;
	col.push_back(255);
	col.push_back(0);
	col.push_back(0);

	for(int i = 0; i < the_image_pointer->helper_data->data.size(); i++){


		vector<Vector3D> vals = the_image_pointer->helper_data->data.at(i)->points_to_draw;
		if(vals.empty()){
			the_image_pointer->helper_data->data.at(i)->calc_data();
			vals = the_image_pointer->helper_data->data.at(i)->points_to_draw;
		}

		ADDITIONAL_TYPE type = the_image_pointer->helper_data->data.at(i)->type;
		if(type == AT_POINT){
			for(int j = 0; j < vals.size(); j++){
				int pix_addr;
				vector<int> point = world_to_view(rg, rgb_sx, rgb_sy, vals.at(i));
				if(point[0] <= rgb_sx && point[0] >= 0 && point[1] <= rgb_sy && point[1] >= 0 ){
					pix_addr = (point[1]*rgb_sx + point[0])*RGB_pixmap_bpp;
					pixels[pix_addr] = col[0];
					pixels[pix_addr+1] = col[1];
					pixels[pix_addr+2] = col[2];
				}
			}
		}else{
			vector<int> start;
			vector<int> stop;
			start = world_to_view(rg, rgb_sx, rgb_sy,vals.at(0));
			for(int j = 1; j < vals.size(); j++){
				stop = world_to_view(rg, rgb_sx, rgb_sy, vals.at(j));
				color_coded_line(pixels, rgb_sx, rgb_sy, start[0], start[1], stop[0], stop[1], col, 
					rg->signed_distance_to_viewing_plane(vals.at(j-1)), rg->signed_distance_to_viewing_plane(vals.at(j)),
					the_image_pointer->helper_data->show_up, the_image_pointer->helper_data->show_down);
				start = stop;
			}
		}
	}
}


void rendererMPR::draw_cross(uchar *pixels, int rgb_sx, int rgb_sy, rendergeom_image *rc, Vector3D point, std::vector<int> on_spot_rgb)
{


	int default_size = 2;
	int on_spot_size = 3;
	float default_threshold = 10.0;
	float on_spot_threshold = 0.5;
	float distance = rc->distance_to_viewing_plane(point);
	float alpha = abs(distance - default_threshold) / default_threshold;

	vector<int> default_rgb(3);	
	default_rgb[0] = 255;
	default_rgb[1] = 255;
	default_rgb[2] = 0;
	
//	vector<int> on_spot_rgb(3);
//	on_spot_rgb[0] = 255;
//	on_spot_rgb[1] = 0;
//	on_spot_rgb[2] = 0;

	

	if ( distance > default_threshold )
		{ return; }
		
	int size = default_size;
	vector<int> rgb = default_rgb;
	
	if ( distance < on_spot_threshold )
	{
		size = on_spot_size;
		rgb = on_spot_rgb;
	}

	std::vector<int> loc = world_to_view(rc, rgb_sx, rgb_sy, point);
	
	// Vertical part
	if ( loc[0] >= 0 && loc[0] <= rgb_sx )
	{
		for (int d = -size; d <= size; d++)
		{
			if ( loc[1] + d >= 0 && loc[1] + d <= rgb_sy )
			{	
				float current_vertical_r = pixels[RGB_pixmap_bpp * (loc[0]+rgb_sx*(loc[1]+d)) + RADDR];
				float current_vertical_g = pixels[RGB_pixmap_bpp * (loc[0]+rgb_sx*(loc[1]+d)) + GADDR];
				float current_vertical_b = pixels[RGB_pixmap_bpp * (loc[0]+rgb_sx*(loc[1]+d)) + BADDR];
								
				pixels[RGB_pixmap_bpp * (loc[0]+rgb_sx*(loc[1]+d)) + RADDR] = rgb[0] * alpha + current_vertical_r * (1 - alpha);
				pixels[RGB_pixmap_bpp * (loc[0]+rgb_sx*(loc[1]+d)) + GADDR] = rgb[1] * alpha + current_vertical_g * (1 - alpha);
				pixels[RGB_pixmap_bpp * (loc[0]+rgb_sx*(loc[1]+d)) + BADDR] = rgb[2] * alpha + current_vertical_b * (1 - alpha);
			}
		}
	}

	// Horizontal part
	if ( loc[1] >= 0 && loc[1] <= rgb_sy )
	{								
		for (int d = -size; d <= size; d++)
		{
			if ( loc[0] + d >= 0 && loc[0] + d <= rgb_sx )
			{
				float current_horizontal_r = pixels[RGB_pixmap_bpp * (loc[0]+d+rgb_sx*loc[1]) + RADDR];
				float current_horizontal_g = pixels[RGB_pixmap_bpp * (loc[0]+d+rgb_sx*loc[1]) + GADDR];
				float current_horizontal_b = pixels[RGB_pixmap_bpp * (loc[0]+d+rgb_sx*loc[1]) + BADDR];

				pixels[RGB_pixmap_bpp * (loc[0]+d+rgb_sx*loc[1]) + RADDR] = rgb[0] * alpha + current_horizontal_r * (1 - alpha);
				pixels[RGB_pixmap_bpp * (loc[0]+d+rgb_sx*loc[1]) + GADDR] = rgb[1] * alpha + current_horizontal_g * (1 - alpha);
				pixels[RGB_pixmap_bpp * (loc[0]+d+rgb_sx*loc[1]) + BADDR] = rgb[2] * alpha + current_horizontal_b * (1 - alpha);
			}
		}
	}
}

void rendererMPR::draw_slice_locators(uchar *pixels, int sx, int sy, rendergeom_image *rg, rendercombination *rc)
{
	std::vector<rendergeom_image *> geometries = rendermanagement.geometries_by_image_and_direction( rc->get_id() );	// get geometries that holds at least one of the images in the input combination
// and have a different direction than the input geometry (i.e. not the same direction nor the opposite direction)
	
	renderer_base * renderer = rendermanagement.get_renderer( rendermanagement.renderer_from_geometry(rg->get_id()) );
	//because class/function is static

	// increase the length of the vector to eliminate problems when converting from world to view coordinates (ie it becomes zero at certain scales) 
	int smin = std::min(sx, sy);
	
	Vector3D vmin = renderer->view_to_world(smin, 0, sx, sy) - renderer->view_to_world(0, 0, sx, sy);
	
	Vector3D a = rg->get_N();

	for ( std::vector<rendergeom_image *>::const_iterator itr = geometries.begin(); itr != geometries.end(); itr++ )
	{			
		Vector3D b = (*itr)->get_N();
		Vector3D c = CrossProduct( a, b);
		float factor = vmin.GetNorm() / c.GetNorm();
		c *= factor / 2;
		std::vector<int> p = world_to_view(rg, sx, sy, (*itr)->look_at);	// determine the position of one of the "other" planes in the active viewport
		std::vector<int> v = world_to_view(rg, sx, sy, c + (*itr)->look_at);

		int dx = abs ( p[0] - v[0] );
		int dy = abs ( p[1] - v[1] );

		std::vector<int> begin(2);
		std::vector<int> end(2);

		if ( dx == 0 )
		{	// a vertical line
			begin[0] = p[0];
			begin[1] = 0;
			end[0] = p[0];
			end[1] = sy;
		}
		else if ( dy == 0 )
		{	// a horizontal line
			begin[0] = 0;
			begin[1] = p[1];
			end[0] = sx;
			end[1] = p[1];
		}
		else 
		{	// calculate start and end point for the line to draw in the viewport
			// TODO: solve with equations instead!!
		
			int t = 0;
			while ( (p[0] + t * v[0]) >= 0 && (p[0] + t * v[0]) <= sx && (p[1] + t * v[1]) >= 0  && (p[1] + t * v[1]) <= sy)
			{
				t++;
			}

			begin[0] = p[0] + t * v[0];
			begin[1] = p[1] + t * v[0];

			t = 0;
			while ( (p[0] + t * v[0]) >= 0 && (p[0] + t * v[0]) <= sx && (p[1] + t * v[1]) >= 0  && (p[1] + t * v[1]) <= sy)
			{
				t--;
			}

			end[0] = p[0] + t * v[0];
			end[1] = p[1] + t * v[0];

		}
		
		vector<int> line_color(3);
		line_color[0] = 0;
		line_color[1] = 255;
		line_color[2] = 0;

		draw_line(pixels, sx, sy, begin[0], begin[1], end[0], end[1], line_color);

	}
}

void rendererMPR::paint_slice_locators_to_overlay(int h_offset, int vp_w, int vp_h_pane, rendergeom_image *rg, rendercombination *rc)
{
//	cout<<"paint_slice_locators_to_overlay("<<vp_w<<" "<<vp_h_pane<<"...)"<<endl;

	std::vector<rendergeom_image*> geoms = rendermanagement.geometries_by_image_and_direction(rc->get_id());	// get geometries that holds at least one of the images in the input combination

//	cout<<"geoms.size()="<<geoms.size()<<endl;

	if(geoms.size()>0){
//		renderer_base *renderer = rendermanagement.get_renderer( rendermanagement.renderer_from_geometry(rc->get_id()) );	//because class/function is static
		renderer_base *renderer = rendermanagement.get_renderer( rendermanagement.renderer_from_combination(rc->get_id()) );	//because class/function is static
		

		int smin = std::min(vp_w, vp_h_pane);
		Vector3D vmin = renderer->view_to_world(smin, 0, vp_w, vp_h_pane) - renderer->view_to_world(0, 0, vp_w, vp_h_pane);
		//vmin contains the length of the smallest vp direction in world coordinates...

		line3D phys_line;
		line2D local_vp_line;

		for(int i=0; i<geoms.size();i++){
			phys_line = rg->get_physical_line_of_intersection(geoms[i]);
			std::vector<int> view1 = world_to_view(rg, vp_w, vp_h_pane, phys_line.get_point());
//			std::vector<int> view1 = world_to_view(rg, vp_w, vp_h, phys_line.get_point());
//			cout<<"line->view1="<<view1[0]<<","<<view1[1]<<endl;
			std::vector<float> dir_loc = world_dir_to_view_dir(rg,vp_w, vp_h_pane, phys_line.get_direction());
			local_vp_line.set_point(view1[0],view1[1]);
			local_vp_line.set_direction(dir_loc[0],dir_loc[1]);

//			paint_overlay_line(vp_w, vp_h, local_vp_line);
			paint_overlay_line(h_offset, vp_w, vp_h_pane, local_vp_line); //It is important that the right pane_height is given...
		}//for

//		fl_rect( vp_offset_x+1, vp_offset_y+1, vp_w-2, vp_h-2, FL_YELLOW);

	}//if
}

void rendererMPR::paint_rendergeometry_to_overlay(int h_offset, int vp_w, int vp_h_pane, rendergeom_image *rg, rendercombination *rc)
{
	if(rc->top_image<image_base>()!=NULL){
		fl_font(FL_COURIER, 10);
		fl_color(FL_RED);
		fl_draw(Matrix3Drow2str(rg->dir,0).c_str(), 5, h_offset+15);
		fl_draw(Matrix3Drow2str(rg->dir,1).c_str(), 5, h_offset+30);
		fl_draw(Matrix3Drow2str(rg->dir,2).c_str(), 5, h_offset+45);

		int str_w, str_h;
		string tmp = Vector3D2str(rg->look_at);
		fl_measure(tmp.c_str(),str_w,str_h);
		fl_draw(tmp.c_str(), vp_w-str_w-5, h_offset+15);
		tmp = float2str(rg->zoom);
		fl_measure(tmp.c_str(),str_w,str_h);
		fl_draw(tmp.c_str(), vp_w-str_w-5, h_offset+45);
	}
}


void rendererMPR::paint_overlay_line(int h_offset, int vp_w, int vp_h_pane, line2D local_vp_line)
{
//	cout<<"paint_overlay_line..."<<endl;
//	cout<<"local_vp_line="<<local_vp_line.get_point()<<" "<<local_vp_line.get_direction()<<endl;
	Vector2D p = local_vp_line.get_point();
	Vector2D d = local_vp_line.get_direction();

	int x;
	int y;
	int x2;
	int y2;

	float dydx = float(d[1])/float(d[0]);
	float dxdy = float(d[0])/float(d[1]);

	fl_color(FL_RED);
	if(d[0]==0){
		x = p[0];
		y = 0;
		x2 = x;
		y2 = vp_h_pane;
		if(x>=0 && x<=vp_w){ //make sure no drawing is made outside current vp... 
			fl_line(x, h_offset+y, x2, h_offset+y2);
		}
	}else{
		x = 0;
		y = p[1] - dydx*p[0];
		x2 = vp_w;
		y2 = p[1] + dydx*(vp_w-p[0]);
				
		//remove drawing outside viewport (vp) in y-direction

		if(y<0){ //drawing outside(above) upper left
			x = p[0] - dxdy*p[1];
			y = 0;
		}
		if(y2<0){
			x2 = p[0] - dxdy*p[1];
			y2 = 0;
		}
		if(y>vp_h_pane){
			x = p[0] + dxdy*(vp_h_pane-p[1]);
			y = vp_h_pane-1;
		}
		if(y2>vp_h_pane){
			x2 = p[0] + dxdy*(vp_h_pane-p[1]);
			y2 = vp_h_pane-1;
		}

//		cout<<"("<<x<<","<<y<<")   ("<<x2<<","<<y2<<")"<<endl;

		fl_line(x, h_offset+y, x2, h_offset+y2);
	}
}

void rendererMPR::paint_data_name_order_to_overlay(int h_offset, int vp_w, int vp_h_pane, rendercombination *rc)
{
	fl_color(FL_CYAN);
	int str_w, str_h;
	string tmp;

	int i=1;

    for(rendercombination::iterator itr = rc->begin(); itr != rc->end();itr++){
		if(datamanagement.get_data(itr->ID)->has_additionl_data()){
			tmp="*";
		}else{
			tmp=" ";
		}
		tmp += datamanagement.get_data_name(itr->ID);
//		cout<<i<<" itr->ID="<<itr->ID<<"   "<<tmp<<" "<<rc->get_number_of_enabled_data_objects()<<endl;
		fl_measure(tmp.c_str(),str_w,str_h);
		fl_draw(tmp.c_str(), 2, h_offset+vp_h_pane-5 -(rc->get_number_of_enabled_data_objects()-i)*(str_h+2)); //type
		i++;
	}
}


int rendererMPR::sgn ( long a )
{
	if (a > 0)
		{ return +1; }
	else if (a < 0) 
		{ return -1; }
	else
		{ return 0; }
}
	
void rendererMPR::draw_line(uchar *pixels, int sx, int sy, int a, int b, int c, int d,  std::vector<int> color)
{
	// Line algorithm
	// http://www.cprogramming.com/tutorial/tut3.html

	long u, s, v, d1x, d1y, d2x, d2y, m, n;
	int  i;
	
	u = c - a;
	v = d - b;
	
	d1x = sgn(u);
	d1y = sgn(v);
	
	d2x = sgn(u);
	d2y = 0;
	
	m = abs(u);
	n = abs(v);
	
	if ( m <= n )
	{
		d2x = 0;
		d2y = sgn(v);
		m = abs(v);
		n = abs(u);
	}
	
	s = (int) (m / 2);
	
	for ( i=0; i < round(m); i++ )
	{
		if ( a >= 0 && a <= sx && b >= 0 && b <= sy )
		{	// inside the viewport
			pixels[RGB_pixmap_bpp * (a + sx * b) + RADDR] = color[0];
			pixels[RGB_pixmap_bpp * (a + sx * b) + GADDR] = color[1];
			pixels[RGB_pixmap_bpp * (a + sx * b) + BADDR] = color[2]; 
		}  
	
		s += n;
		if (s >= m)
		{
		  s -= m;
		  a += d1x;
		  b += d1y;
		}
		else 
		{
		  a += d2x;
		  b += d2y;
		}
	}
}

void rendererMPR::color_coded_line(uchar *pixels, int sx, int sy, int a, int b, int c, int d,  std::vector<int> color, float start_distance, float end_distance, bool up, bool down)
{
	// Line algorithm
	// http://www.cprogramming.com/tutorial/tut3.html
	//cout << "dist" << start_distance << "  " << end_distance << endl;

	long u, s, v, d1x, d1y, d2x, d2y, m, n;
	int  i;
	double dist_span = end_distance-start_distance;
	double dist_quota = dist_span/std::max(abs(a-c),abs(b-d));
	
	u = c - a;
	v = d - b;
	
	d1x = sgn(u);
	d1y = sgn(v);
	
	d2x = sgn(u);
	d2y = 0;
	
	m = abs(u);
	n = abs(v);
	
	if ( m <= n )
	{
		d2x = 0;
		d2y = sgn(v);
		m = abs(v);
		n = abs(u);
	}
	
	s = (int) (m / 2);
	
	for ( i=0; i < round(m); i++ )
	{
		int dist = (i*dist_quota)+start_distance;
		if ( a >= 0 && a <= sx && b >= 0 && b <= sy)
		{
			// inside the viewport
			if(dist <= 0.01 && dist >= -0.01){
				pixels[RGB_pixmap_bpp * (a + sx * b) + RADDR] = color[0];
				pixels[RGB_pixmap_bpp * (a + sx * b) + GADDR] = color[1];
				pixels[RGB_pixmap_bpp * (a + sx * b) + BADDR] = color[2];
			}else if(dist >0.01 && up){
				pixels[RGB_pixmap_bpp * (a + sx * b) + RADDR] = 255;
				pixels[RGB_pixmap_bpp * (a + sx * b) + GADDR] = 255;
				pixels[RGB_pixmap_bpp * (a + sx * b) + BADDR] = 0;
			}else if (dist <-0.01 && down){
				pixels[RGB_pixmap_bpp * (a + sx * b) + RADDR] = 0;
				pixels[RGB_pixmap_bpp * (a + sx * b) + GADDR] = 255;
				pixels[RGB_pixmap_bpp * (a + sx * b) + BADDR] = 255;
			}
		}  
	
		s += n;
		if (s >= m)
		{
		  s -= m;
		  a += d1x;
		  b += d1y;
		}
		else 
		{
		  a += d2x;
		  b += d2y;
		}
	}
}
/*
rendergeom_image* rendererMPR::get_the_rg()
{
	rendergeom_image *ret = (rendergeom_image*)this->the_rg;
	return ret;
}
*/
void rendererMPR::move_view(int vsize, int pan_x, int pan_y, int pan_z, float zoom_d)
{
    //also relative to view orientation, zoom is a multiple of previous zoom value
    
    Vector3D pan;
    pan.Fill(0);
//    pan[0]=pan_x*renderer_base::display_scale/(float)(vsize * the_rg->zoom);
//    pan[1]=pan_y*renderer_base::display_scale/(float)(vsize * the_rg->zoom);
    pan[0]=pan_x*ZOOM_CONSTANT/(float)(vsize * ((rendergeom_image*)the_rg)->zoom);
    pan[1]=pan_y*ZOOM_CONSTANT/(float)(vsize * ((rendergeom_image*)the_rg)->zoom);
    pan[2]=pan_z;
    
    ((rendergeom_image*)the_rg)->zoom*=zoom_d;
    ((rendergeom_image*)the_rg)->look_at += ((rendergeom_image*)the_rg)->dir*pan;
}


void rendererMPR::rotate_dir(int dx_in_vp_pixels, int dy_in_vp_pixels)
{
	Matrix3D m = create_rot_matrix_3D(dy_in_vp_pixels*pt_PI/180.0, -dx_in_vp_pixels*pt_PI/180.0, 0.0);
//	get_the_rg()->dir = get_the_rg->dir*m;
	((rendergeom_image*)this->the_rg)->dir = ((rendergeom_image*)this->the_rg)->dir*m;
}

void rendererMPR::look_at(float x, float y, float z)
{
    ((rendergeom_image*)the_rg)->look_at[0]=x;
    ((rendergeom_image*)the_rg)->look_at[1]=y;
    ((rendergeom_image*)the_rg)->look_at[2]=z;
}

void rendererMPR::look_at(float x, float y, float z, float zoom)
{
	look_at(x,y,z);
	((rendergeom_image*)the_rg)->zoom = zoom;
}

void rendererMPR::move( float pan_x, float pan_y, float pan_z)
{
    Vector3D pan = create_Vector3D(pan_x, pan_y, pan_z);
//    pan.Fill(0);
  //  pan[0]=pan_x;
    //pan[1]=pan_y;
   // pan[2]=pan_z;
    
    ((rendergeom_image*)the_rg)->look_at+=pan;
  
}

void rendererMPR::move_voxels(int x,int y,int z)
{
    Vector3D dir;
    dir[0] = x; dir[1] = y; dir[2] = z;
    
    //combinations of images are rendered (which may have varying voxel sizes, 
    //positions and orientation), here it is defined that
    //move_voxels will move by the voxels of the topmost image
    
    image_base* image = the_rc->top_image<image_base>(); //TODO_R top
    
    dir = ((rendergeom_image*)the_rg)->dir*dir;
    Matrix3D reOrient;
    reOrient = image->get_orientation().GetInverse();
    
    dir = reOrient* image->get_voxel_resize() * image->get_orientation() * dir;
            
    move(dir[0],dir[1],dir[2]);
}