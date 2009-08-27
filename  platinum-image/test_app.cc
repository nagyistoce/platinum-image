/////////////////////////////////////////////////////////////////////////////////
//
//  Test application for Platinum development
//
//  $LastChangedBy$
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

#include "../../platinumprogram.h"

#include <cstdio>
#include <iostream>

#include "../../global.h"
#include "../../image_binary.hxx"
#include "../../image_complex.hxx"
#include "../../curve.hxx"
#include "../../color.h"

//retrieve command-line arguments
int visid = -1;
int arg(int argc, char **argv, int &i) {
    if (argv[i][1] == 'v') {
        if (i+1 >= argc) return 0;
        visid = atoi(argv[i+1]);
        i += 2;
        return 2;
    }
    return 0;
}

void add_demo_image (int userIO_ID,int par_num)
{
    if (par_num == USERIO_CB_OK)
        {
        image_integer<unsigned char,3>* demo_image = new image_integer<unsigned char,3>(20,20,20,NULL);
        //demo_image->erase();
        demo_image->testpattern();
        
        datamanagement.add(demo_image);
        }
}

//Example function that creates a new image_binary, fills it with 1, and runs a function on it.

void expand_function(int userIO_ID,int par_num)
{
    if (par_num == USERIO_CB_OK)
        {
        image_binary<3>* bin = new image_binary<3>(20,20,20);
		bin->fill(1);
		image_binary<3>* bin2 = dynamic_cast<image_binary<3>*>( bin->expand_borders(2,2,2) );
        
		delete bin;
        datamanagement.add(bin2);
        }
}


void rgb_vol_function(int userIO_ID,int par_num)
{
    if(par_num == USERIO_CB_OK){
		int res = 50;
		image_scalar<float,3> *red = new image_scalar<float,3>(res,res,res,0);
		image_scalar<float,3> *gre = new image_scalar<float,3>(res,res,res,0);
		image_scalar<float,3> *blu = new image_scalar<float,3>(res,res,res,0);
		for(int z=0;z<red->nz();z++){
			for(int y=0;y<red->ny();y++){
				for(int x=0;x<red->nx();x++){
					red->set_voxel(x,y,z,x);
					gre->set_voxel(x,y,z,y);
					blu->set_voxel(x,y,z,z);

				}
			}
		}
		datamanagement.add(red,"red",true);
		datamanagement.add(gre,"green",true);
		datamanagement.add(blu,"blue",true);
	}
}

void link_load_function(int userIO_ID,int par_num)
{
    if(par_num == USERIO_CB_OK){

		string path = "K:/Joel/Data/Vinstudien/3P-Linköping/DICOM/DICOM/";

		image_scalar<float,3> *im = new image_scalar<float,3>(path + "IM_0001");
		image_scalar<short,3> *im2 = new image_scalar<short,3>(im);
		datamanagement.add(im,"im");
		datamanagement.add(im2,"im2");


	}
}



unsigned char calc_value(unsigned char r, unsigned char g, unsigned char b)
{
	return std::max(r,std::max(g,b));			
}

float calc_hue_in_degrees(unsigned char r, unsigned char g, unsigned char b)
{
	float h = 0;			//if R == G == B (Max == Min)
	float ma = calc_value(r,g,b);
	float mi = 0;			


	if(r>=g && r>b){			//R == Max
		mi = std::min(g,b);
		float tmp = 60*float(g-b)/(ma-mi) + 360.0; //mod 360..
		h = int(tmp)%360;

	}else if(g>=b && g>r){	//G == Max
		mi = std::min(r,b);
		h = 60.0*float(b-r)/(ma-mi) + 120;

	}else if(b>=r && b>g){	//B == Max
		mi = std::min(r,g);
		h = 60.0*float(r-g)/(ma-mi) + 240;
	}
//	cout<<int(r)<<" "<<int(g)<<" "<<int(b)<<" --> "<<h<<" ("<<ma<<" "<<mi<<")"<<endl;
	return h;
}

float calc_saturation(unsigned char r, unsigned char g, unsigned char b)
{
	float ma = calc_value(r,g,b);
	float mi = std::min(r,std::min(g,b));
	return (ma-mi)/ma;			
}


void rgb_test()
{
	unsigned char r=255;
	unsigned char g=255;
	unsigned char b=255;

	calc_hue_in_degrees(255,255,255);
	cout<<endl;
	calc_hue_in_degrees(255,0,0);
	calc_hue_in_degrees(0,255,0);
	calc_hue_in_degrees(0,0,255);
	cout<<endl;
	calc_hue_in_degrees(255,255,0);
	calc_hue_in_degrees(100,100,0);
	cout<<endl;
	
	//------------------------

	image_scalar<float,3> *hue = new image_scalar<float,3>(256,256,256,0);
	image_scalar<float,3> *sat = new image_scalar<float,3>(256,256,256,0);
	image_scalar<float,3> *val = new image_scalar<float,3>(256,256,256,0);
	for(int z=0;z<256;z++){
		for(int y=0;y<256;y++){
			for(int x=0;x<256;x++){
				hue->set_voxel(x,y,z,calc_hue_in_degrees(x,y,z));
				sat->set_voxel(x,y,z,calc_saturation(x,y,z));
				val->set_voxel(x,y,z,calc_value(x,y,z));
			}
		}
	}
	datamanagement.add(hue,"hue",true);
	datamanagement.add(sat,"sat",true);
	datamanagement.add(val,"val",true);

}


int main(int argc, char *argv[])
{

    //start up Platinum
    platinum_init();
    
    //set up main window
    Fl_Window window(800,600);

//	window.fullscreen();
    
    //prepare Platinum for userIO creation
    platinum_setup(window,2,1);
    
    // *** begin userIO control definitions ***
    int create_vol_demo_ID=userIOmanagement.add_userIO("Add demo image",add_demo_image,"Add");
//    userIOmanagement.add_par_image(create_vol_demo_ID,"Working image: ");
//	userIOmanagement.add_par_histogram_2D(create_vol_demo_ID,"histogram_2d");
	//userIOmanagement.add_par_voxelseed(create_vol_demo_ID,"voxelseed");
//	userIOmanagement.add_par_float_box(create_vol_demo_ID,"float_box",50,0);
    userIOmanagement.finish_userIO(create_vol_demo_ID);

    int expand_ID=userIOmanagement.add_userIO("expanding test",expand_function,"Run");
    userIOmanagement.finish_userIO(expand_ID);

	int rgb_vol_ID=userIOmanagement.add_userIO("Create RGB volumes",rgb_vol_function,"Run");
	userIOmanagement.finish_userIO(rgb_vol_ID);

	int link_load_test_ID=userIOmanagement.add_userIO("Linkoping Load",link_load_function,"Load");
    userIOmanagement.finish_userIO(link_load_test_ID);

    // *** end userIO control definitions ***
    
    //finish the window creation
    window.end();
    window.show(argc, argv);

	//------------------
	complex<float> c = complex<float>(3,4);
	complex<float> d = complex<float>(4,5);
	image_complex<float,3> *im = new image_complex<float,3>(10,10,10,&c);
	im->fill(d);
	datamanagement.add(im,"im");
	//------------------

//	RGBvalue val = RGBvalue();
//	val.set_rgb_from_complex(complex<float>(5.0,0.1),float(0.1), 5.0);
//	cout<<val<<endl;

//	rgb_test();

	//preset viewport configuration 
//	viewmanagement.set_vp_direction(1,X_DIR);
//	viewmanagement.set_vp_blend_mode(1,BLEND_AVG);
//	viewmanagement.set_vp_renderer(2,"VTK-MIP");
    
    return Fl::run();
}
