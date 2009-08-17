//////////////////////////////////////////////////////////////////////////////////
//
//  RendererMPR $Revision: 706 $
///
/// arbitrary 2D slices rendered by scanline
///
//  $LastChangedBy: joel.kullberg $

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

#define _renderer_curve_cc_
#include "renderer_curve.h"
#include "viewmanager.h"
#include "rendermanager.h"

#define MAXRENDERVOLUMES 1

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

renderer_curve::renderer_curve():renderer_curve_base()
{
	cout << "curve renderer created" << endl;
	the_rg = new rendergeom_curve();
}

void renderer_curve::connect_data(int dataID)
    {
    //TEST: wrapper, this should be done directly by rendermanagement
    the_rc->add_data(dataID);
	}

Vector3D renderer_curve::view_to_world(int vx, int vy, int sx, int sy) const
{
  
    Vector3D v;
        
	if (((rendergeom_curve*)the_rg)->curve !=NULL){
		v = ((rendergeom_curve*)the_rg)->view_to_curve(vx, vy, sx, sy);
		return v;
    }else{
        //no image there
        Vector3D n;
        n.Fill (-1);
            
        return n;
    }
}

Vector3D renderer_curve::view_to_voxel(int vx, int vy,int sx,int sy,int imageID) const
{
  
    Vector3D v;
        
    if (((rendergeom_curve*)the_rg)->curve !=NULL){
		v = ((rendergeom_curve*)the_rg)->view_to_curve(vx, vy, sx,sy);
		return v;
    }else{
        //no image there
        Vector3D n;
        n.Fill (-1);
            
        return n;
    }
    //return Vector3D();
}

bool renderer_curve::supports_mode (int m)
{ 
   /* switch (m)
        {
        case BLEND_OVERWRITE:
            return true;
            break;
        default:
            return false;
        }
		*/
	cout << "FrÂgar om " << m << endl;
	return m == BLEND_OVERWRITE;
}

void renderer_curve::render_thumbnail (unsigned char *rgb, int rgb_sx, int rgb_sy, int image_ID)
{
    rendercombination rc = rendercombination(image_ID);
    rendergeom_curve rg = rendergeom_curve();
    
    render_( rgb, rgb_sx, rgb_sy,&rg,&rc);
}

void renderer_curve::render_position(unsigned char *rgb, int rgb_sx, int rgb_sy)
{
    render_( rgb, rgb_sx, rgb_sy,(rendergeom_curve*)the_rg,the_rc);
}




//render orthogonal slices using memory-order scanline
void renderer_curve::render_(uchar *pixels, int rgb_sx, int rgb_sy, rendergeom_curve *rg, rendercombination *rc)
{
	cout << "Curve renderer used" << endl;

    if(rc->empty()){       //*** no images: exit ***
        return;
	}
    
   // Vector3D screen_center = create_Vector3D(rgb_sx, rgb_sy, 0);
   // screen_center/=2;
    
    blendmode blend_mode = rc->blend_mode();

	//S‰tter bakgrunden till vit tror jag. RGB_pixmap_bpp betyder rgb utan alphav‰rde (alltsÂ 3 bytes per pixel)
	for(long p=0; p < rgb_sx*rgb_sy*RGB_pixmap_bpp; p +=RGB_pixmap_bpp){
		pixels[p] = pixels[p + 1] = pixels[p + 2]=255;
	}
    //TODO s‰tt parametrar i rg frÂn den sista kurvan  i paiarItr 
    #pragma mark *** Per-image render loop ***

	/*rendercombination::iterator pairItr = rc->begin();
	if(rc->begin() != rc->end()){
		pairItr = rc->end();
		pairItr --;
		curve_base *the_curve_pointer;
		pt_error::error_if_null(pairItr->pointer,"Rendered data object is NULL");//Crash here when closing an image
        the_curve_pointer = dynamic_cast<curve_base *> (pairItr->pointer);
		if(the_curve_pointer != NULL)
			rg->set_borders(the_curve_pointer, rgb_sx, rgb_sy);
	}*/
	bool first = true;

	for(rendercombination::iterator pairItr = rc->begin();pairItr != rc->end();pairItr++){//den sista klammern ska flyttas lÂngt ner 
        curve_base *the_curve_pointer = NULL;
		pt_error::error_if_null(pairItr->pointer,"Rendered data object is NULL");//Crash here when closing an image
        the_curve_pointer = dynamic_cast<curve_base *> (pairItr->pointer);
		bool OKrender = the_curve_pointer != NULL && the_curve_pointer->is_supported(renderer_type());

        if(OKrender){
			
			RGBvalue *curve_color = the_curve_pointer->get_color();
			if(first){
				rg->set_borders(the_curve_pointer, rgb_sx, rgb_sy);
				first = false;
			}
			char type = the_curve_pointer->get_line();

			//Kolla om curve_pointer ‰r uppdaterad h‰r
		   /* Om uppdaterad
			*
			*/
			rg->set_curve(the_curve_pointer);
			
			int pix_addr = 0;
			double data  = 0.0;
			int size = the_curve_pointer->get_data_size();

			int min_bound, max_bound;
			min_bound = std::max((int)rg->view_to_curve(0,0,rgb_sx, rgb_sy)[0],0);
			max_bound = std::min((int)rg->view_to_curve(rgb_sx,0,rgb_sx, rgb_sy)[0], size);

			switch(type){
				case '.':
					for(int i = min_bound; i<max_bound; i++){
						data = the_curve_pointer->get_data(i);
						Vector3D point = rg->curve_to_view(i, data, rgb_sx, rgb_sy);
						if(point[0] <= rgb_sx && point[0] >= 0 && point[1] <= rgb_sy && point[1] >= 0 ){
							point[0] = round(point[0]);
							point[1] = round(point[1]);
							pix_addr = (point[1]*rgb_sx + point[0])*RGB_pixmap_bpp;
							pixels[pix_addr] = curve_color->r();
							pixels[pix_addr+1] = curve_color->g();
							pixels[pix_addr+2] = curve_color->b();
						}
					}
					break;
				case '-':
					if(size < 2){
						break;
					}else{
						double data2;
						Vector3D start, stop;
						data = the_curve_pointer->get_data(0);

						vector<int> color;
						color.push_back(curve_color->r());
						color.push_back(curve_color->g());
						color.push_back(curve_color->b());


						for(int j = min_bound+1; j < max_bound; j++){
							data2 = the_curve_pointer->get_data(j);
							start = rg->curve_to_view(j-1, data, rgb_sx, rgb_sy);
							stop = rg->curve_to_view(j,data2, rgb_sx, rgb_sy);
							draw_line(pixels, rgb_sx, rgb_sy, start[0], start[1], stop[0], stop[1], color);
							data = data2;
						}
					}
					break;
				case '|':
						vector<int> color;
						color.push_back(curve_color->r());
						color.push_back(curve_color->g());
						color.push_back(curve_color->b());
						Vector3D start;
						double y_min = rg->curve_to_view(0,rg->curve->get_min(), rgb_sx, rgb_sy)[1];

						for(int j = min_bound; j < max_bound; j++){
							data = the_curve_pointer->get_data(j);
							start = rg->curve_to_view(j,data, rgb_sx, rgb_sy);
							draw_line(pixels, rgb_sx, rgb_sy, start[0], start[1], start[0], y_min, color);
						}
					break;
			}
			Vector2D pos = rg->mouse_location;
			vector<int> col;
			col.push_back(255 - curve_color->r());
			col.push_back(255 - curve_color->g());
			col.push_back(255 - curve_color->b());
			draw_line(pixels, rgb_sx, rgb_sy, pos[0], 0, pos[0], rgb_sy, col);

			Vector2D measure = rg->measure_location;
			if(measure[0] != -1){
				draw_line(pixels, rgb_sx, rgb_sy, measure[0], 0, measure[0], rgb_sy, col);
			}
			if(the_curve_pointer->draw_additional_data){
				render_additional_data(pixels,the_curve_pointer, rg, rgb_sx, rgb_sy, col);
			}
			//the_curve_pointer->helper_data->draw_all_data(pixels,rgb_sx,rgb_sy,rg,renderer_type());
			//draw_axes(pixels, the_curve_pointer, rg, rgb_sx, rgb_sy);

			
		}
	}
}//render_ function
void renderer_curve::render_additional_data(uchar *pixels, curve_base *the_curve_pointer, rendergeom_curve *rg, int rgb_sx, int rgb_sy, vector<int> col){
	for(int i = 0; i < the_curve_pointer->helper_data->data.size(); i++){


		vector<Vector3D> vals = the_curve_pointer->helper_data->data.at(i)->points_to_draw;
		if(vals.empty() || the_curve_pointer->modified){
			the_curve_pointer->helper_data->data.at(i)->calc_data(pixels,rgb_sx, rgb_sy,rg,renderer_type());
			vals = the_curve_pointer->helper_data->data.at(i)->points_to_draw;
		}

		//Rita ut dom här istället...
		ADDITIONAL_TYPE type = the_curve_pointer->helper_data->data.at(i)->type;
		if(type == AT_STRING){
			Vector3D point = rg->curve_to_view(vals.at(0)[0], vals.at(0)[1], rgb_sx, rgb_sy);
			point[0] = round(point[0]);
			point[1] = round(point[1]);
			string s = (dynamic_cast<text_data*>(the_curve_pointer->helper_data->data.at(i)))->s;
			fl_font(FL_COURIER, 10);
			fl_color(FL_BLACK);
			fl_draw(s.c_str(),point[0],point[1]);
	
			//FL_DRAW ...
		}else{
		
			
			if(type == AT_POINT){
				for(int j = 0; j < vals.size(); j++){
					int pix_addr;
					Vector3D point = rg->curve_to_view(vals.at(j)[0], vals.at(j)[1], rgb_sx, rgb_sy);
					point[0] = round(point[0]);
					point[1] = round(point[1]);
					if(point[0] <= rgb_sx && point[0] >= 0 && point[1] <= rgb_sy && point[1] >= 0 ){
						pix_addr = (point[1]*rgb_sx + point[0])*RGB_pixmap_bpp;
						pixels[pix_addr] = col[0];
						pixels[pix_addr+1] = col[1];
						pixels[pix_addr+2] = col[2];
					}
				}
			}else{
				double data, data2;
				Vector3D start, stop;
				data = vals.at(0)[1];
				for(int j = 1; j < vals.size(); j++){
					data2 = vals.at(j)[1];
					start = rg->curve_to_view(vals.at(j-1)[0], data, rgb_sx, rgb_sy);
					stop = rg->curve_to_view(vals.at(j)[0],data2, rgb_sx, rgb_sy);
					draw_line(pixels, rgb_sx, rgb_sy, start[0], start[1], stop[0], stop[1], col);
					data = data2;
				}
			}
		}
	}
}
void renderer_curve::draw_axes(uchar *pixels, curve_base *curve, rendergeom_curve *rg, int width, int height){
	double max = curve->get_max();
	double min = curve->get_min();
	
	int button_offset = 20;

	double y_step = height/10.0;
	double x_step = width/10.0;

	vector<int> col;
	col.push_back(0); //Black
	col.push_back(0);
	col.push_back(0);

	fl_font(FL_COURIER, 10);
	fl_color(FL_BLACK);


	draw_line(pixels, width, height, 30, 0, 30, height-20, col);
	std::ostringstream s;
	//Draw y axis
	for(int i = height-20; i > 0; i-=y_step){
		draw_line(pixels, width, height, 25, i, 35, i, col);
		s.str("");
		//s = float2str(rg->view_to_curve(0,i, width, height)[1]);
		s << std::fixed << std::setprecision(2) << rg->view_to_curve(0,i, width, height)[1]; //2 decimaler duger nog
		fl_draw(s.str().c_str(),0,i+button_offset+5);
	}

	draw_line(pixels, width, height, 30, height-20, width, height-20, col);
	//Draw x axis
	for(int j = 30; j < width; j+=x_step){
		s.str("");
		draw_line(pixels, width, height, j, height-15, j, height-25, col);
		s << std::fixed << std::setprecision(2) << (rg->view_to_curve(j,0, width, height)[0]*rg->x_scale + rg->x_offset); //L‰gg in scale m.m i view_to_curve osv
		//s = float2str(rg->view_to_curve(j,0, width, height)[0]*rg->x_scale + rg->x_offset);
		fl_draw(s.str().c_str(),j-5,height + button_offset);
	}
}

int renderer_curve::sgn ( long a )
{
	if (a > 0)
		{ return +1; }
	else if (a < 0) 
		{ return -1; }
	else
		{ return 0; }
}
	
void renderer_curve::draw_line(uchar *pixels, int sx, int sy, int a, int b, int c, int d,  std::vector<int> color)
{
	// Line algorithm
	//http://www.cprogramming.com/tutorial/tut3.html

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