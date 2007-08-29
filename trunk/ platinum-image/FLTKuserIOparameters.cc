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

#include "FLTKuserIOparameters.h"

#include <algorithm>

#include <FL/fl_draw.H>

#include "userIOmanager.h"
#include "datamanager.h"
#include "viewmanager.h"

extern datamanager datamanagement;
extern viewmanager viewmanagement;

//for testing, various 2D histogram classes can be interchanged:
#define HISTOGRAM2DVARIETY histogram_2D

FLTKuserIOparameter_base::FLTKuserIOparameter_base() : Fl_Group (NA,NA,NA,NA)
    {
    }

FLTKuserIOparameter_base::FLTKuserIOparameter_base(int w, int h,const std::string name) : Fl_Group (NA,NA,w,h)
    {
    box(FL_THIN_DOWN_BOX);
    align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
    labelsize(FLTK_SMALL_LABEL);

    strcpy(widget_label,name.c_str());
    label(widget_label);
    resizable (this);
    }

void FLTKuserIOparameter_base::set_callback(userIO_callback * p_callback)
{
    par_callback=p_callback;
}

void FLTKuserIOparameter_base::par_update_callback (Fl_Widget *callingwidget, void * foo)
    {
    FLTKuserIOparameter_base * par_group;
    userIO * userIO_group;
    int par_num;

    par_group=(FLTKuserIOparameter_base * )callingwidget->parent();
    userIO_group=(userIO *)par_group->parent();

    par_num=userIO_group->get_par_num(par_group);
    //check own parameter num
    if (par_group->par_callback != NULL)
        {
        par_group->par_callback (userIO_group->id(),par_num);
        }
    }


#pragma mark *** FLTKuserIOpar_coord3Ddisplay ***

std::string FLTKuserIOpar_coord3Ddisplay::resolve_teststring()
	{
//		cout<<"FLTKuserIOpar_coord3Ddisplay...resolve_teststring()"<<endl;
//		cout<<"coord[0]="<<coord[0]<<endl;
//		cout<<"coord[1]="<<coord[1]<<endl;
//		cout<<"coord[2]="<<coord[2]<<endl;

		char s[30];
		string tmp = "(";
		sprintf(s, "%5.1f", coord[0] );		tmp+=std::string(s);	//%5.1f - means five digits and one decimal...
		sprintf(s, "%5.1f", coord[1] );		tmp+=", "+std::string(s);
		sprintf(s, "%5.1f", coord[2] );		tmp+=", "+std::string(s)+")";

		return coord_name+" - "+coord_type_name+" - "+tmp;
	}

FLTKuserIOpar_coord3Ddisplay::FLTKuserIOpar_coord3Ddisplay(std::string c_name, std::string c_type_name, Vector3D v) : FLTKuserIOparameter_base(INITPARWIDGETWIDTH,int(STDPARWIDGETHEIGHT/2), "")
	{
		cout<<"FLTKuserIOpar_coord3Ddisplay..."<<endl;
		coord_name = c_name;
		coord_type_name = c_type_name;
		set_coordinate(v);

		control = new Fl_Output(x(),y(),w(),STDPARWIDGETHEIGHT-PARTITLEMARGIN);
		control->box(FL_FLAT_BOX);
		control->color(FL_BACKGROUND_COLOR);
		control->textsize(12);
		control->value(resolve_teststring().c_str()); //update();

		resizable(control);
		end();
	}

const std::string FLTKuserIOpar_coord3Ddisplay::type_name ()
	{
		return "coord3Ddisplay";
	}

void FLTKuserIOpar_coord3Ddisplay::par_value (Vector3D &v)
    {
	v = coord;
    }

void FLTKuserIOpar_coord3Ddisplay::set_coordinate(Vector3D v)
	{
	coord = v;
	}
 
void FLTKuserIOpar_coord3Ddisplay::update()
    {
	cout<<"FLTKuserIOpar_coord3Ddisplay...update()"<<endl;
	control->value(resolve_teststring().c_str());
	control->redraw();
    }



#pragma mark *** FLTKuserIOpar_float ***

FLTKuserIOpar_float::FLTKuserIOpar_float (const std::string name, float ma, float mi) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Value_Slider(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->type(FL_HOR_SLIDER);
    control->maximum(ma);
    control->minimum(mi);

    resizable(control);

    //this->add(control);
    end();
    }

void FLTKuserIOpar_float::par_value (float & v)
    {
    v=control->value();
    }

const std::string FLTKuserIOpar_float::type_name ()
    {
    return "float";
    }

#pragma mark *** FLTKuserIOpar_float_box ***

FLTKuserIOpar_float_box::FLTKuserIOpar_float_box (const std::string name, float ma, float mi) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Value_Input(x(),y()+PARTITLEMARGIN,PARMENUWIDTH,h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->maximum(ma);
    control->minimum(mi);
    control->step(0.001);
//	control->value(default_val)

    resizable(NULL);

    end();
    }

void FLTKuserIOpar_float_box::par_value (float & v)
    {
    v=static_cast<float>(control->value());
    }

const std::string FLTKuserIOpar_float_box::type_name ()
    {
    return "float_box (Fl_Value_Input)";
    }


#pragma mark *** FLTKuserIOpar_longint ***

FLTKuserIOpar_longint::FLTKuserIOpar_longint (const std::string name, long ma, long mi) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Value_Slider(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->type(FL_HOR_SLIDER);
    control->maximum(ma);
    control->minimum(mi);
    control->step(1);

    resizable(control);

    end();
    }

void FLTKuserIOpar_longint::par_value (long & v)
    {
    v=static_cast<long>(control->value());
    }

const std::string FLTKuserIOpar_longint::type_name ()
    {
    return "long (slider)";
    }

#pragma mark *** FLTKuserIOpar_longint_box ***

FLTKuserIOpar_longint_box::FLTKuserIOpar_longint_box (const std::string name, long ma, long mi) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Value_Input(x(),y()+PARTITLEMARGIN,PARMENUWIDTH,h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->maximum(ma);
    control->minimum(mi);
    control->step(1);

    resizable(NULL);

    end();
    }

void FLTKuserIOpar_longint_box::par_value (long & v)
    {
    v=static_cast<long>(control->value());
    }

const std::string FLTKuserIOpar_longint_box::type_name ()
    {
    return "long (slider)";
    }

#pragma mark *** FLTKuserIOpar_bool ***

FLTKuserIOpar_bool::FLTKuserIOpar_bool (const std::string name, bool init_status ) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Check_Button(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);

    control->callback(par_update_callback);

    control->type(FL_TOGGLE_BUTTON);
    control->value(init_status);

    resizable(control);

    end();
    }

void FLTKuserIOpar_bool::par_value (bool & v)
    {
    v=(control->value() == 0 ? false : true);
    }

const std::string FLTKuserIOpar_bool::type_name ()
    {
    return "boolean";
    }


#pragma mark *** FLTKuserIOpar_string ***

FLTKuserIOpar_string::FLTKuserIOpar_string (const std::string name, std::string init_status ) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    //label set by superclass constructor
    //position & width set by parent userIO later, including margin
    control = new Fl_Input(x(),y()+PARTITLEMARGIN,w(),h()-PARTITLEMARGIN);

    control->callback(par_update_callback);
//    control->type(FL_TOGGLE_BUTTON);
	control->value(init_status.c_str());

    resizable(control);

    end();
    }

void FLTKuserIOpar_string::par_value (std::string & s)
    {
    s = std::string(control->value());
    }

const std::string FLTKuserIOpar_string::type_name ()
    {
	return "std::string";
    }


#pragma mark *** FLTKuserIOpar_image ***

FLTKuserIOpar_image::FLTKuserIOpar_image (const std::string name) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
    {
    control = new FLTKimage_choice(x(),y()+PARTITLEMARGIN);
    control->callback(par_update_callback);

    control->data_vector_has_changed();
    
    resizable(NULL);

    end();
    }

void FLTKuserIOpar_image::par_value (imageIDtype & v)
    {
    v = control->id_value();
    }

void FLTKuserIOpar_image::data_vector_has_changed ()
    {
    control->data_vector_has_changed();
    }


const std::string FLTKuserIOpar_image::type_name ()
    {
    return "image ID";
    }

#pragma mark *** FLTKuserIOpar_points ***

FLTKuserIOpar_points::FLTKuserIOpar_points (const std::string name) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
{
    control = new FLTKpoint_choice(x(),y()+PARTITLEMARGIN);
    control->callback(par_update_callback);
    
    control->data_vector_has_changed();
    
    resizable(NULL);
    
    end();
}

void FLTKuserIOpar_points::par_value (Vector3D & v)
{
    point * p = dynamic_cast<point *> (datamanagement.get_data(control->id_value()));

    pt_error::error_if_null(p,"requested Point3D value, point object is of different subclass",pt_error::serious);
    
    v = p->get_origin();
}

void FLTKuserIOpar_points::par_value (pointIDtype & v)
{
    v = control->id_value();
}

void FLTKuserIOpar_points::data_vector_has_changed ()
{
    control->data_vector_has_changed();
}

const std::string FLTKuserIOpar_points::type_name ()
{
    return "point ID";
}

//data choice widget base class

FLTKdataitem_choice::FLTKdataitem_choice (int x, int y):Fl_Choice(x,y,PARMENUWIDTH,STDPARWIDGETHEIGHT-PARTITLEMARGIN)
{}

void FLTKdataitem_choice::data_change (const Fl_Menu_Item * base_menu)
{
    //store selection
    int selected_item=value();
    long selected_vol=NOT_FOUND_ID;
    
    const Fl_Menu_Item *cur_menu=menu();   
    
    if (cur_menu !=NULL)
        {selected_vol=cur_menu[selected_item].argument();}
    
    selected_item=0; //set to no selection if previous selection isn't found below
    
    //rebuild
    Fl_Menu_Item new_menu[datamanager::IMAGEVECTORMAX+2];
    
    new_menu[0].label("(no selection)");
    new_menu[0].shortcut(0);
    new_menu[0].callback((Fl_Callback *)NULL,0);
    new_menu[0].argument(0);
    new_menu[0].flags= 0;
    new_menu[0].labeltype(FL_NORMAL_LABEL);
    new_menu[0].labelfont(0);
    new_menu[0].labelsize(FLTK_LABEL_SIZE);
    new_menu[0].labelcolor(FL_BLACK);
    
    int m=1;
    
    while (base_menu[m-1].label() !=NULL) 
        {    
        memcpy (&new_menu[m],&base_menu[m-1],sizeof(Fl_Menu_Item));
        
        if (new_menu[m].argument() == selected_vol)
            {selected_item=m;}
        m++;
        }
    new_menu[m].label(NULL);
    
    copy(new_menu);
    value(selected_item);
    redraw();
}

#pragma mark *** General image choice widget ***

FLTKimage_choice::FLTKimage_choice (int x, int y) : FLTKdataitem_choice(x,y)
    {
    data_vector_has_changed();
    }

imageIDtype FLTKimage_choice::id_value ()
    {
    const Fl_Menu_Item *cur_menu=menu();   
    int selected_item=value();

    if (selected_item==0)
        {return NOT_FOUND_ID;}

    if (cur_menu !=NULL)
        {return cur_menu[selected_item].argument();}

    return NOT_FOUND_ID;
    }

void FLTKimage_choice::data_vector_has_changed()
{
    data_change(datamanagement.FLTK_image_menu());
}

#pragma mark *** General point choice widget ***

FLTKpoint_choice::FLTKpoint_choice (int x, int y) : FLTKdataitem_choice(x,y)
{
    data_vector_has_changed();
}

pointIDtype FLTKpoint_choice::id_value ()
{
    const Fl_Menu_Item *cur_menu=menu();   
    int selected_item=value();
    
    if (selected_item==0)
        {return NOT_FOUND_ID;}
    
    if (cur_menu !=NULL)
        {return cur_menu[selected_item].argument();}
    
    return NOT_FOUND_ID;
}

void FLTKpoint_choice::data_vector_has_changed()
{
    data_change(datamanagement.FLTK_point_menu());
}

#pragma mark *** Histogram display widget ***

    //TODO: variable resolution
    //right mouse button on whole widget acts as a slider
    //up-down, with single bucket resolution at the top and
    //1:1 (depending on widget size) at the bottom

FLTK_histogram_base::FLTK_histogram_base (int x, int y, int w, int h) : Fl_Widget (x,y,w,h)
    {
    //screen_image=NULL;
    screen_image_data=NULL;

    screen_image_data = new unsigned char [w*h*RGBpixmap_bytesperpixel];
    //screen_image = new Fl_RGB_Image(screen_image_data, w, h,RGBpixmap_bytesperpixel, 0);
    }

FLTK_histogram_base::~FLTK_histogram_base ()
    {
    //delete screen_image;
    delete []screen_image_data;
    }

void FLTK_histogram_base::resize(int x, int y, int w, int h)
    {
    if (w != this->w() || h != this->h())
        {
        //delete screen_image;
        delete [] screen_image_data;

        screen_image_data = new unsigned char [w*h*RGBpixmap_bytesperpixel];
        //screen_image = new Fl_RGB_Image(screen_image_data, w, h,RGBpixmap_bytesperpixel, 0);
        }

    Fl_Widget::resize(x, y, w, h);

    refresh();
    }

FLTK_histogram_2D::FLTK_histogram_2D (int x, int y, int w, int h) : FLTK_histogram_base (x, y, w, h)
    {
    histogram = NULL;
    histogram = new HISTOGRAM2DVARIETY ();
    drag_mode=READY_RECT;
    }

FLTK_histogram_2D::~FLTK_histogram_2D()
    {
    delete histogram;
    }

int FLTK_histogram_2D::handle(int event)
    {
    //define thresholds as percentage, let histogram object transform these into actual values

    threshold=histogram->get_threshold();

    if ( histogram->ready() )
        {
        switch (event)
            {
            case FL_PUSH:
                {
                drag_ref_x=(Fl::event_x()-x())/(float)w();
                drag_ref_y=1.0-(Fl::event_y()-y())/(float)h();

                if (Fl::event_button() == FL_LEFT_MOUSE && threshold.id[0]!=NOT_FOUND_ID && threshold.id[1]!=NOT_FOUND_ID)
                    {
                    if (drag_mode==READY_OVAL)
                        {
                        if (std::sqrt(powf((drag_ref_x-((h_treshold_max+h_treshold_min)/2.0))/((h_treshold_max-h_treshold_min)/(v_treshold_max-v_treshold_min)),2.0)+powf(drag_ref_y-((v_treshold_max+v_treshold_min)/2.0),2.0)) <= (v_treshold_max-v_treshold_min)/2.0)
                            {drag_mode=MOVE_OVAL;}
                        else
                            {drag_mode=SET_OVAL;}
                        }

                    if (drag_mode==READY_RECT)
                        {
                        if (drag_ref_x >= h_treshold_min && drag_ref_x <= h_treshold_max && drag_ref_y >= v_treshold_min && drag_ref_y <= v_treshold_max)
                            {drag_mode=MOVE_RECT;}
                        else
                            {drag_mode=SET_RECT;}
                        }

                    thresholders = viewmanagement.get_overlays(& threshold);
                    }
                }
            break;

            case FL_DRAG:
                {
                float drag_point_x = (float)(Fl::event_x()-x())/(float)w();
                float drag_point_y = 1.0-(float)(Fl::event_y()-y())/(float)h();

                if (Fl::event_button() == FL_LEFT_MOUSE)
                    {
                    switch (drag_mode)
                        {
                        case SET_RECT:                    
                            //check positive/negative dimensions to make
                            //thresholding values more digestible
                            if (drag_point_x - drag_ref_x > 0)
                                {
                                h_treshold_min=drag_ref_x;
                                h_treshold_max=drag_point_x;
                                }
                            else
                                {
                                h_treshold_min=drag_point_x;
                                h_treshold_max=drag_ref_x;
                                }

                            if (drag_point_y -drag_ref_y > 0)
                                {
                                v_treshold_min=drag_ref_y;
                                v_treshold_max=drag_point_y;
                                }
                            else
                                {
                                v_treshold_min=drag_point_y;
                                v_treshold_max=drag_ref_y;
                                }
                            break;
                        case SET_OVAL:
                            {
                            //check positive/negative dimensions to make
                            //thresholding values more digestible
                            float size_x=fabs(drag_point_x-drag_ref_x)*std::sqrt(2.0);
                            float size_y=fabs(drag_point_y-drag_ref_y)*std::sqrt(2.0);                            
                            //float r_extra=fabs(size_x-size_y);

                            h_treshold_min=drag_ref_x-size_x;
                            h_treshold_max=drag_ref_x+size_x;

                            v_treshold_min=drag_ref_y-size_y;
                            v_treshold_max=drag_ref_y+size_y;
                            }
                        break;

                        case MOVE_OVAL:
                        case MOVE_RECT:

                            h_treshold_min+=drag_point_x-drag_ref_x;
                            h_treshold_max+=drag_point_x-drag_ref_x;

                            v_treshold_min+=drag_point_y-drag_ref_y;
                            v_treshold_max+=drag_point_y-drag_ref_y;

                            //move is relative, set reference to last coordinate
                            drag_ref_x=drag_point_x;
                            drag_ref_y=drag_point_y;
                            break;
                        default:
                            {
                                //suppress GCC enum warning
                            }
                        }
                    }

                if (Fl::event_button() == FL_RIGHT_MOUSE)
                    {
                    //float drag_point_x = (float)(Fl::event_x()-x())/(float)w();
                    //float drag_point_y = 1.0-(float)(Fl::event_y()-y())/(float)h();
                    //change histogram resolution
                    int resolution=std::min(std::max(h()*(1-drag_point_y),(float)1),(float)h());

                    histogram->calculate(resolution);

                    /*refresh();*/

                    //screen_image->uncache();
                    
                    histogram->render(screen_image_data,w(),h());

                    draw();
                    }
                
                }  //end of FL_DRAG block
            break;

            case FL_RELEASE:
                if (Fl::event_button() == FL_LEFT_MOUSE)
                    {
                    if (drag_mode == SET_RECT ||drag_mode == MOVE_RECT  )
                        {drag_mode = READY_RECT;}

                    if (drag_mode == SET_OVAL ||drag_mode == MOVE_OVAL  )
                        {drag_mode = READY_OVAL;}
                    }

                damage(FL_DAMAGE_ALL);
                thresholders.clear();
                break;

            case FL_MOUSEWHEEL:
                { 
                //let user widen/shrink selection
                float center_x=(h_treshold_max+h_treshold_min)/2.0;
                float center_y=(v_treshold_max+v_treshold_min)/2.0;

                float size_x=fabs(h_treshold_max-h_treshold_min)*(0.5+((float)Fl::event_dy())*0.02);
                float size_y=fabs(v_treshold_max-v_treshold_min)*(0.5+((float)Fl::event_dy())*0.02);   

                Fl::event_dy();

                h_treshold_min=center_x-size_x;
                h_treshold_max=center_x+size_x;

                v_treshold_min=center_y-size_y;
                v_treshold_max=center_y+size_y;

                thresholders = viewmanagement.get_overlays(& threshold);
                }
            break;

            default:
                return 0;
            }

            if (thresholders.size() > 0)
                {
                //selection changed;
                //set new values for threshold
                threshold = ((HISTOGRAM2DVARIETY *)histogram)->get_threshold(h_treshold_min,h_treshold_max,v_treshold_min,v_treshold_max, (drag_mode > MOVE_RECT ? THRESHOLD_2D_MODE_OVAL : THRESHOLD_2D_MODE_RECT ));

                std::vector<threshold_overlay *>::iterator oitr=thresholders.begin();
                while (oitr != thresholders.end())
                    {
                    (*oitr)->render (&threshold);
                    oitr++;
                    }

                damage (FL_DAMAGE_ALL);
                }
        }
    else
        {
        // < 2 images selected, no event processed
        return 0;
        }

    return 1;
    }

    void FLTK_histogram_2D::set_selmode(int mode)
        {
        if (mode==THRESHOLD_2D_MODE_OVAL)
            {drag_mode=READY_OVAL;}

        if (mode==THRESHOLD_2D_MODE_RECT)
            {drag_mode=READY_RECT;}

        damage (FL_DAMAGE_ALL);
        }

    void FLTK_histogram_2D::set_images (int hor, int vert)
        {
        ((HISTOGRAM2DVARIETY *)histogram)->images(hor,vert);

        thresholders.clear();
        refresh();
        }

    thresholdparvalue FLTK_histogram_base::get_threshold ()
        {
        threshold = histogram->get_threshold();
        return threshold;
        }

    void FLTK_histogram_base::draw ()
        {
        Fl_RGB_Image screen_image(screen_image_data, w(), h(),RGBpixmap_bytesperpixel, 0);
                
        screen_image.draw(x(),y());
        }

    void FLTK_histogram_2D::draw ()
        {
        fl_push_clip(x(),y(),w(),h());
        
        Fl_RGB_Image screen_image(screen_image_data, w(), h(),RGBpixmap_bytesperpixel, 0);

        screen_image.draw(x(),y());

        fl_color(fl_rgb_color(255, 0, 0));

        if ( drag_mode > MOVE_RECT)
            {
            fl_arc(h_treshold_min*w()+x(),h()-v_treshold_max*h()+y(),(h_treshold_max-h_treshold_min)*w(),(v_treshold_max-v_treshold_min)*h(),0,360) ;
            }
        else
            {
            fl_rect(h_treshold_min*w()+x(),h()-v_treshold_max*h()+y(),(h_treshold_max-h_treshold_min)*w(),(v_treshold_max-v_treshold_min)*h()) ;
            }

        fl_pop_clip();
        }

    void FLTK_histogram_base::refresh()
        {
        //allocate image data, 
        //screen_image_data = new unsigned char [w()*h()*RGBpixmap_bytesperpixel];

        //screen_image->uncache();
        histogram->render(screen_image_data,w(),h());

        damage (FL_DAMAGE_ALL);
        }

    #pragma mark *** Histogram parameters ***

    FLTKuserIOpar_histogram2D::FLTKuserIOpar_histogram2D (std::string name) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT+INITPARWIDGETWIDTH, name)
        {
        const int small_widget_size= STDPARWIDGETHEIGHT-PARTITLEMARGIN;

        histogram_widget = new FLTK_histogram_2D(x(), y()+STDPARWIDGETHEIGHT, w(), w());

        image_v= new FLTKimage_choice(x(),y()+PARTITLEMARGIN);
        image_h= new FLTKimage_choice(x()+w()-PARMENUWIDTH,y()+PARTITLEMARGIN+STDPARWIDGETHEIGHT+w());

        image_h->callback(vol_change_callback);
        image_v->callback(vol_change_callback);
        ((HISTOGRAM2DVARIETY *)histogram_widget->histogram)->images(image_v->id_value(),image_h->id_value());

        oval_button=new Fl_Button (x(),y()+PARTITLEMARGIN+STDPARWIDGETHEIGHT+w(),small_widget_size,small_widget_size,"@circle");
        oval_button->labelsize(9);
        oval_button->callback(selmode_change_callback);
        oval_button->argument(THRESHOLD_2D_MODE_OVAL);

        rect_button=new Fl_Button (x()+small_widget_size,y()+PARTITLEMARGIN+STDPARWIDGETHEIGHT+w(),small_widget_size,small_widget_size,"@square");
        rect_button->labelsize(9);
        rect_button->callback(selmode_change_callback);
        rect_button->argument(THRESHOLD_2D_MODE_RECT);

        oval_button->value(1);
        histogram_widget->set_selmode(THRESHOLD_2D_MODE_OVAL);

        resizable (NULL);
        end();
        }

    void FLTKuserIOpar_histogram2D ::vol_change_callback (Fl_Widget *callingwidget, void * foo)
        {
        FLTKuserIOpar_histogram2D * that = (FLTKuserIOpar_histogram2D *)callingwidget->parent();

        that->set_images (that->image_h->id_value(),that->image_v->id_value());
        }

    void FLTKuserIOpar_histogram2D::set_images (int hor, int vert)
        {
        histogram_widget->set_images (hor,vert);
        }

    void FLTKuserIOpar_histogram2D ::selmode_change_callback (Fl_Widget *callingwidget, void * foo)
        {
        FLTKuserIOpar_histogram2D * that = (FLTKuserIOpar_histogram2D *)callingwidget->parent();

        that->rect_button->value(callingwidget->argument() == THRESHOLD_2D_MODE_RECT );
        that->oval_button->value(callingwidget->argument() == THRESHOLD_2D_MODE_OVAL);

        that->histogram_widget->set_selmode(callingwidget->argument());
        }

    void FLTKuserIOpar_histogram2D::resize(int x, int y, int w, int h)
        {
        Fl_Widget::resize(x,y,w,w+STDPARWIDGETHEIGHT*2-PARTITLEMARGIN);

        image_v->position(this->x(),this->y()+PARTITLEMARGIN);

        histogram_widget->resize(this->x(), this->y()+STDPARWIDGETHEIGHT, this->w(), this->w());

        image_h->position(this->x()+this->w()-image_h->w(),this->y()+STDPARWIDGETHEIGHT+histogram_widget->h());

        oval_button->position(this->x(),this->y()+STDPARWIDGETHEIGHT+histogram_widget->h());
        rect_button->position(this->x()+STDPARWIDGETHEIGHT-PARTITLEMARGIN,this->y()+STDPARWIDGETHEIGHT+histogram_widget->h());
        }

    void FLTKuserIOpar_histogram2D::data_vector_has_changed ()
        {
        image_h->data_vector_has_changed();
        image_v->data_vector_has_changed();

        set_images (image_h->id_value(),image_v->id_value());
        }

    int FLTKuserIOpar_histogram2D::histogram_image_ID (int axis)
        {
        return histogram_widget->histogram->image_ID(axis);
        }

    void FLTKuserIOpar_histogram2D::highlight_ROI (regionofinterest * region)
        {
        ((HISTOGRAM2DVARIETY *)histogram_widget->histogram)->highlight(region);
        histogram_widget->refresh();
        }

    void FLTKuserIOpar_histogram2D::par_value (thresholdparvalue & v)
        {
        v = histogram_widget->get_threshold();
        }

    const std::string FLTKuserIOpar_histogram2D::type_name ()
        {
        return "2D threshold";
        }

#pragma mark *** Message "parameter" ***

    FLTKuserIOpar_message::FLTKuserIOpar_message (std::string name,std::string msgtext) : FLTKuserIOparameter_base (INITPARWIDGETWIDTH,STDPARWIDGETHEIGHT, name)
        {
        message = new Fl_Output (x(),y()+PARTITLEMARGIN,w(),STDPARWIDGETHEIGHT-PARTITLEMARGIN);

        message->box(FL_FLAT_BOX);
        message->color(FL_BACKGROUND_COLOR);
        message->value (msgtext.c_str());

        resizable (message);
        end();
        }

    const std::string FLTKuserIOpar_message::type_name ()
        {
        return "message";
        }