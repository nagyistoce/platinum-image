// $Id$

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "global.h"

#include <cstdlib> //for NULL

#include "FLTKutilities.h"

#include <FL/Fl.H>
#include "event.h"

void init_fl_menu_item(Fl_Menu_Item & m)
    {
    m.label("");
    m.shortcut(0);
    m.callback((Fl_Callback *)NULL,0);
    m.user_data(NULL);
    m.flags= 0;
    m.labeltype(FL_NORMAL_LABEL);
    m.labelfont(0);
    m.labelsize(FLTK_LABEL_SIZE);
    m.labelcolor(FL_BLACK);
    }

int fl_menu_size (const Fl_Menu_Item * const m)
    {
    if (m != 0)
        {
        int item;
        for (item = 0; m[item].label() !=NULL;item++)
            {}
        return item;
        }

    return -1;
    }

void fl_menu_userdata_delete (const Fl_Menu_Item * m)
    {
    int size = fl_menu_size(m);

    for (int i = 0; i < size;i++)
        {
        void * ud = m[i].user_data();
        if (m != NULL)
            { delete ud; }
        }
    }

horizresizeablescroll::horizresizeablescroll(int x, int y, int w, int h, const char *label) : Fl_Scroll (x,y,w,h,label)
    {
    //children are deleted automatically with parents, so  
    interior=new Fl_Pack(x,y,w-FLTK_SCROLLBAR_SIZE,h);
    type(Fl_Scroll::VERTICAL_ALWAYS);
    interior->type(FL_VERTICAL);
    box(FL_FLAT_BOX);
    interior->box(FL_NO_BOX);
	
	xpos = 0;
	ypos = 0;
	
	//interior->callback(widget_callback, (void*) this);
	this->callback(widget_callback, (void*) this);

	//interior->end();	//JKJK just added this *** but it seems ended via "void horizresizeablescroll::end()"
    }

void horizresizeablescroll::begin()
    {
    interior->begin();
    }

void horizresizeablescroll::end()
    {
    interior->end();
    Fl_Scroll::end();
    }

void horizresizeablescroll::resize(int newx, int newy, int neww, int newh)
    {
//	std::cout << "horizresizeablescroll::resize ("<<newx<<" "<<newy<<" "<<neww<<" "<<newh<<")"<<std::endl;
    int scroll_x=interior->x()-x();
    int scroll_y=interior->y()-y();

    Fl_Widget::resize(newx, newy, neww, newh);
    interior->resize(newx+scroll_x, newy+scroll_y, neww-FLTK_SCROLLBAR_SIZE, interior->h());
	std::cout << "interior->resize("<<newx+scroll_x<<" "<<newy+scroll_y<<" "<<neww-FLTK_SCROLLBAR_SIZE<<" "<<interior->h()<<")"<<std::endl;
    }



void horizresizeablescroll::widget_callback(Fl_Widget* callingwidget, void* test) {
	std::cout << "horizresizeablescroll::widget_callback" << std::endl;
}

//void horizresizeablescroll::setupCallback() {
//	this->callback(widget_callback, (void*) this);
//}

int horizresizeablescroll::handle(int event)
{
//	std::cout << "horizresizeablescroll::handle" << std::endl;
	int ret = 0;

//	if (Fl::event() == FL_DRAG )
//		{ std::cout << "FL_DRAG" << std::endl; }

//	while ( Fl::event_button1() )
//	{
//		std::cout << "event_button1" << std::endl;
//		Fl::flush();
//	}

//	//if (event == FL_DRAG)
//	if (Fl::event_state(FL_BUTTON1))
//	{
//		std::cout << "FL_DRAG" << std::endl;
//		while (Fl::event_buttons() != 0)
//		{
//			std::cout << "hold" << std::endl;
//			if (abs(ypos - yposition()) > 0 || abs(xpos - xposition()) > 0)
//			{ 
//				ypos = yposition();
//				xpos = xposition();
//
//				std::cout << "redraw" << std::endl;
//				this->redraw(); 
//				
//			}		
//		}
//	}



//	if (Fl::event_state(FL_BUTTON1))
//	{
//		std::cout << "ADJUST" << std::endl;
//	}
//	
//	if (event == FL_RELEASE)
//		{ std::cout << "FL_RELEASE" << std::endl; }

	//std::cout << yposition() << std::endl;
	//std::cout << event << std::endl;
//	if (abs(ypos - yposition()) > 0 || abs(xpos - xposition()) > 0)
//	{ 
//		ypos = yposition();
//		xpos = xposition();
//
//		std::cout << "redraw" << std::endl;
//		this->redraw(); 
//		
//	}
	
	//Fl_Widget::do_callback();
	
	Fl_Scroll::handle(event);
	return 1; //Fl_Scroll::handle(event);
	//return ( Fl_Scroll::handle(event) ? 1 : ret );

}

// -------- FLTK_Editable_Slider ------------

void FLTK_Editable_Slider::Slider_CB2() {
        static int recurse = 0;
        if(recurse){ 
			std::cout<<"FLTK_Editable_Slider-Slider_CB2-recurse"<<std::endl;
			return;
		}else{
            recurse = 1;
			input->value( float2str(slider->value()).c_str() );    // pass slider's value to input
//			std::cout<<"label()="<<this->label()<<std::endl;
			if(this->callback()!=NULL){ //if connected to a callback!!!
				this->do_callback(); //calls callback function connected to this "FLTK_Editable_Slider"
			}
			recurse = 0;
        }
    }

void FLTK_Editable_Slider::Slider_CB(Fl_Widget *w, void *data) {

        ((FLTK_Editable_Slider*)data)->Slider_CB2();
    }


void FLTK_Editable_Slider::Input_CB2(){
        static int recurse = 0;
        if(recurse){
			std::cout<<"FLTK_Editable_Slider-Input_CB2-recurse"<<std::endl;
			return;
		}else{
            recurse = 1;
            slider->value(atof(input->value()));
//			std::cout<<"label()="<<this->label()<<std::endl;
			this->do_callback(); //calls callback function connected to this class
            recurse = 0;
        }
    }

void FLTK_Editable_Slider::Input_CB(Fl_Widget *w, void *data) {
        ((FLTK_Editable_Slider*)data)->Input_CB2();
    }

FLTK_Editable_Slider::FLTK_Editable_Slider(int x, int y, int w, int h, const char *l, int input_w) : Fl_Group(x,y,w,h,0) {
        input  = new Fl_Float_Input(x, y, input_w, h);
        input->callback(Input_CB, (void*)this);
        input->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
		input->label(l);
		input->labelsize(h-2);
		input->textsize(h-2);

        slider = new Fl_Slider(x+input_w+5, y, w - (input_w+5), h);
        slider->type(FL_HOR_SLIDER);
        slider->callback(Slider_CB, (void*)this);

		end();			// close the group
    }

float FLTK_Editable_Slider::value() 
	{ return atof(input->value()); }

void FLTK_Editable_Slider::value(float val) 
	{ slider->value(val); Slider_CB2(); }

void FLTK_Editable_Slider::value_no_Fl_callback(float val)
	{ input->value(float2str(val).c_str()); slider->value(val);}    // pass slider's value to input

void FLTK_Editable_Slider::minimum(float val)
	{ slider->minimum(val); }

float FLTK_Editable_Slider::minimum()
	{ return slider->minimum(); }

void FLTK_Editable_Slider::maximum(float val) 
	{ slider->maximum(val); }

float FLTK_Editable_Slider::maximum() 
	{ return slider->maximum(); }

void FLTK_Editable_Slider::bounds(float low, float high) 
	{ slider->bounds(low, high); }

void FLTK_Editable_Slider::step(float val)
	{ slider->step(val); }

void FLTK_Editable_Slider::precision(float val)
	{ slider->precision(val); }

void FLTK_Editable_Slider::labelsize(unsigned char s)
	{ input->labelsize(s); }

void FLTK_Editable_Slider::textsize(unsigned char s)
	{ input->textsize(s); }

const char* FLTK_Editable_Slider::label()
	{ return input->label();};
