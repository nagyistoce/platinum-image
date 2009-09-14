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

#include "histo2D_tool.h"
//#include "histogram.h"
#include "FLTKuserIOparameters.h"
#include "datamanager.h"
#include "rendermanager.h"
#include "viewmanager.h"

extern datamanager datamanagement;
extern rendermanager rendermanagement;
extern viewmanager viewmanagement;

using namespace std;

int viewport::maxviewportID = 0;
//bool viewport::renderermenu_built=false;
panefactory viewport::pfactory = panefactory();


viewport::viewport(VIEWPORT_TYPE vpt)
{
    ID = ++maxviewportID;
    //VIEWPORT_TYPE vp_type = vpt;
	vp_type = vpt;

  
    rendererID=NO_RENDERER_ID;
    rendererIndex=-1;
	needs_re_rendering = true;

    busyTool = NULL;

    rgbpixmap = NULL;
    rgbpixmap_size[0] = 0;
    rgbpixmap_size[1] = 0;

	ROI_rectangle_x = -1;
	ROI_rectangle_y = -1;
	ROI_rectangle_w = -1;
	ROI_rectangle_h = -1;

	ROI_rect_is_changing = false;
   	paint_rendergeometry=false;

	#ifdef TESTMODE
	   	paint_rendergeometry=true;
	#endif
}

viewport::~viewport()
{
    if (busyTool != NULL)
        {delete busyTool;}
    
    if (rgbpixmap != NULL)
        {delete[] rgbpixmap; }
}


void viewport::initialize_viewport(int xpos, int ypos, int width, int height, VIEWPORT_TYPE vpt)
{
	vp_type = vpt;
	the_widget = NULL;
	
	int tmp_rendID;
	if(vp_type == PT_MPR){	//PT_MPR, PT_MIP, VTK_EXAMPLE, VTK_MIP, VTK_ISOSURF};
		tmp_rendID = rendermanagement.create_renderer(RENDERER_MPR);
	}else if(vp_type == PT_MIP){
		tmp_rendID = rendermanagement.create_renderer(RENDERER_MIP);
	}else if(vp_type == PT_CURVE){
		tmp_rendID = rendermanagement.create_renderer(RENDERER_CURVE);
	}
	viewmanagement.connect_renderer_to_viewport(ID,tmp_rendID);     //attach MPR renderer - so that all viewports can be populated for additional views


    const int buttonheight=20;
    const int buttonwidth=70;
    update_viewsize(width, height - 2*buttonheight);	//allocates/changes rgb_pixmap...
    
	the_widget = new FLTKviewport(xpos,ypos,width,height,this,buttonheight,buttonwidth); //The FLTKviewport can access the VIEWPORT_TYPE using the pointer to this viewport
}

int viewport::x(){
	return the_widget->x();
}
int viewport::y(){
	return the_widget->y();
}
int viewport::w(){
	return the_widget->w();
}
int viewport::h(){
	return the_widget->h();
}

int viewport::h_pane(){
	return the_widget->h_pane();
}


void viewport::clear_rgbpixmap()
{
    //blacken viewport
    for (long p=0; p < rgbpixmap_size[0]*rgbpixmap_size[1]*RGB_pixmap_bpp; p +=RGB_pixmap_bpp )
        {
        rgbpixmap[p] = rgbpixmap[p + 1] = rgbpixmap[p + 2] = 0;
        }
} 
/*
Matrix3D viewport::get_renderer_direction()
{
	return rendermanagement.get_direction(rendererIndex);
}
*/

void viewport::set_renderer_direction( const Matrix3D & dir )
{
    Matrix3D * dir_p = new Matrix3D(dir);
	
    rendermanagement.set_image_geometry( rendererIndex, dir_p );
//    cout<<"vp_id="<<this->ID<<" dir_p="<<dir_p<<endl;
    delete dir_p;
	
}

void viewport::set_renderer_direction( preset_direction direction ) 
{
	the_widget->pane_widget->set_renderer_direction(direction); 
}
void viewport::change_color( colors color ,VIEWPORT_TYPE type) 
{
	if(type == PT_CURVE)
		((FLTK_Pt_Curve_pane*)the_widget->pane_widget)->change_color(color); 
	else if(type == PT_SPECTRUM)
		((FLTK_Pt_Spectrum_pane*)the_widget->pane_widget)->change_color(color);
}
void viewport::change_line_type(char line, VIEWPORT_TYPE type){
	if(type == PT_CURVE)
		((FLTK_Pt_Curve_pane*)the_widget->pane_widget)->change_line(line); 
	else if(type == PT_SPECTRUM)
		((FLTK_Pt_Spectrum_pane*)the_widget->pane_widget)->change_line(line); 
}
void viewport::change_x(char x){
	((FLTK_Pt_Spectrum_pane*)the_widget->pane_widget)->change_x(x); 
}
void viewport::change_y(char y){
	((FLTK_Pt_Spectrum_pane*)the_widget->pane_widget)->change_y(y); 
}
void viewport::change_geom_type(int vp_id, VIEWPORT_TYPE type){
	if(type == PT_CURVE)
		((FLTK_Pt_Curve_pane*)the_widget->pane_widget)->change_geom(vp_id);
	else if(type == PT_MPR)
		((FLTK_Pt_MPR_pane*)the_widget->pane_widget)->change_geom(vp_id);
	else if(type == PT_SPECTRUM)
		((FLTK_Pt_Spectrum_pane*)the_widget->pane_widget)->change_geom(vp_id);
}


void viewport::set_vp_type_from_factoryIdType(string type)
{
	//pane_factory.Register<FLTK_Pt_MPR_pane>("MPR");			
    //pane_factory.Register<FLTK_VTK_Cone_pane>("VTK-Cone");	
    //pane_factory.Register<FLTK_VTK_MIP_pane>("VTK-MIP");	
	//pane_factory.Register<FLTK_Pt_Curve_pane>("Curve");	

	if(type=="MPR"){
		this->vp_type = PT_MPR;		//enum VIEWPORT_TYPE {PT_MPR, PT_MIP, VTK_EXAMPLE, VTK_MIP, VTK_ISOSURF, PT_CURVE};
	}else if(type=="VTK-Cone"){
		this->vp_type = VTK_EXAMPLE;
	}else if(type=="VTK-MIP"){
		this->vp_type = VTK_MIP;
	}else if(type=="Curve"){
		this->vp_type = PT_CURVE;

	}else if(type == "Spectrum"){
		this->vp_type = PT_SPECTRUM;
	}else{
		cout<<"oops. check set_vp_type_from_factoryIdType..."<<endl;
	}


}

int viewport::get_id () const
{
    return ID;
}

int viewport::get_renderer_id () const
{
    return rendererID;
}

void viewport::connect_renderer(int rID)
{
    rendererID = rID;               //this is the unique renderer ID within the class, NOT the vector array index!!!
    //pane_widget->needs_rerendering();
	needs_re_rendering = true;
    refresh();
}

void viewport::needs_rerendering()
{
//	cout<<"viewport::needs_rerendering()... (id="<<ID<<")"<<endl;
	needs_re_rendering = true;
}

void viewport::render_data(int data_id)
{
	//if image...

	int rID = viewmanagement.get_renderer_id(this->ID);
	if(rendermanagement.renderer_empty(rID)){
		rendermanagement.center3d_and_fit(rID, data_id); //JK
	}
	rendermanagement.connect_data_renderer(rID,data_id);
}

const int * viewport::pixmap_size () const
{
    return rgbpixmap_size;
}

void viewport::update_viewsize(int des_width, int des_height)
{
    const float grow_factor=1.5;  //margin area added when re-allocating of rgbpixmap
    
    if (abs(RGB_pixmap_bpp*des_width*des_height)  > sizeof (rgbpixmap))
        {
        long rgbpixmap_datasize=(long)RGB_pixmap_bpp*des_width*des_height*grow_factor;
        
        if (rgbpixmap != NULL)
            {
            delete []rgbpixmap;
            }
        
        if (rgbpixmap_datasize > 0 )
            {
            rgbpixmap= new uchar[rgbpixmap_datasize];
            }
        else
            {
            rgbpixmap=NULL;
            }
        }
    
    if (des_width <=0 || des_height <=0)
        {
        rgbpixmap_size[0] = rgbpixmap_size[1] = 0;
        }
    else
        {
        rgbpixmap_size[0] = des_width;
        rgbpixmap_size[1] = des_height;
        }
    
    clear_rgbpixmap();
}

void viewport::enable_and_set_direction( preset_direction direction )
{
	set_renderer_direction( direction );
}

void viewport::set_renderer(string renderer_type)
{
	the_widget->switch_pane(renderer_type);

//	the_widget->set_renderer_button_label(renderer_type);
}

void viewport::set_blend_mode(blendmode bm)
{
	int rend_index = rendermanagement.find_renderer_index(this->get_renderer_id());
	rendermanagement.set_blendmode(rend_index, bm);

//	the_widget->set_blend_mode(bm);
//	the_widget->set_blend_button_label(bm);
}


bool viewport::render_if_needed()
{
    if (rendererIndex>=0 && needs_re_rendering)
        {
//		cout<<"viewport::render_if_needed --> viewport_ID="<<ID<<" rendererIndex="<<rendererIndex<<endl;
        rendermanagement.render(rendererIndex, rgbpixmap, rgbpixmap_size[0], rgbpixmap_size[1]);
        needs_re_rendering = false;
        return true;
        }
    return false;
}


void viewport::refresh_after_toolswitch()
{
    if (busyTool != NULL)
        {
        delete busyTool;
        busyTool = NULL;
        
        refresh();
        }
}

void viewport::refresh_from_geometry(int g)
{
    if (rendererIndex >= 0 && rendermanagement.get_geometry_id(rendererIndex) == g)
        {
        refresh();
        }
}

void viewport::refresh_from_combination(int c)
{
    //conditional refresh, only perform if this instance uses the argument rendercombination
    
    if (rendererIndex >= 0 && rendermanagement.get_combination_id(rendererIndex) == c)
        {
        //this call means that the selection of rendered images has changed, possibly into nothing
        clear_rgbpixmap();
        refresh();
        }
}


void viewport::refresh()
{
    //called when re-render and redraw may be needed
    
    // we cache the vector ID to speed things up - now it must be recomputed
    rendererIndex = rendermanagement.find_renderer_index(rendererID);
    
    if(the_widget!=NULL){
        if(rendererIndex == -1){
            //our renderer has been deleted
            rendererID=NO_RENDERER_ID;
            clear_rgbpixmap();
            //button_pack_top->deactivate();
            }
        else
            {
            histo2D_tool * utool = dynamic_cast<histo2D_tool * > (busyTool);
            if (utool != NULL)
                {
				utool->attach(this,rendermanagement.get_renderer(rendererID));
				}
            
			if(the_widget !=NULL){
				the_widget->refresh_menus();
				the_widget->pane_widget->damage(FL_DAMAGE_ALL); //JK2-check if needed...
			}
            needs_re_rendering = true;

            }
        
        //pane_widget->damage(FL_DAMAGE_ALL);
		//the_widget->damage(FL_DAMAGE_ALL); 
//		if(the_widget->pane_widget !=NULL){
//			the_widget->pane_widget->damage(FL_DAMAGE_ALL); 
//		}
    }
}

void viewport::refresh_overlay()
{
	the_widget->refresh_overlay(); //JK2 -TODO - make work...//jk2 pass this on..
}

void viewport::paint_overlay()
{
//	rendermanagement.get_renderer(this->rendererID)->paint_overlay(w(), h());
//	cout<<"***h()="<<h_pane()<<" id="<<this->get_id()<<endl;

	if(vp_type == PT_MPR){	
		//here the height of the pane needs to be used since the height of the "viewport" includes the buttons...
		rendermanagement.get_renderer(this->rendererID)->paint_overlay(20, w(), h_pane(), paint_rendergeometry); //h_pane is needed to compensate for button height... 

		fl_color(FL_GRAY);
		fl_rect(0, 0, w(), h()); //SO - framing the viewports 888
		

		if(ROI_rectangle_x >-1 ){
			fl_color(FL_RED);
			//fl_line_style(FL_DOT, 2, 0); //fl_line_style(int style, int width=0, char* dashes=0)
			fl_rect(ROI_rectangle_x,ROI_rectangle_y,ROI_rectangle_w,ROI_rectangle_h);
			//fl_line_style(0); //reset line style
		}
	}
}


threshold_overlay * viewport::get_threshold_overlay (thresholdparvalue * threshold_par)
{    
    histo2D_tool * utool = NULL;
    
    //2D histogram should only allow this call when the uim tool is selected
    if (busyTool == NULL)
        {
//        viewport_event e = viewport_event(0,pane_widget);
		viewport_event e = viewport_event(0, (FLTK_Event_pane*)the_widget->pane_widget);
        busyTool = utool = new histo2D_tool (e,threshold_par,this, rendermanagement.get_renderer(rendererID));
        }
    
    if (busyTool != NULL) //might have been created earlier too
        {        
        utool = dynamic_cast<histo2D_tool *>(busyTool);
        
        if (utool != NULL)
            {
            return utool->get_overlay();
            }
        }
    
    return NULL;
}


void viewport::set_look_at_and_zoom(Vector3D pos, float zoom)
{
	rendermanagement.get_renderer(rendererID)->look_at(pos[0],pos[1],pos[2],zoom);
}
