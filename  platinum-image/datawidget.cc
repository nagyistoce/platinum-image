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

#include "datawidget.h"

#include "datamanager.h"
#include "rendermanager.h"
#include "transfer.h"

using namespace std;

extern datamanager datamanagement;
extern rendermanager rendermanagement;
//extern uchar *animage; //defined in datamanager.cc

const int datawidget_base::thumbnail_size = 128;

const char *slice_orientation_labels[] = {"axial","sagittal","coronal", "undefined"};


#pragma mark transferfactory statics

transferfactory::tf_menu_params::tf_menu_params (const std::string t,image_base * i)
    {
    image = i;
    type = t;
    }

void transferfactory::tf_menu_params::switch_tf()
    {
    pt_error::error_if_null(image,"Trying to make tfunction menu item with image = NULL");
    image->transfer_function(type);
	image->data_has_changed(); //JK - One might allow access to image->redraw()... (transfer function type/defaults might change...)
    }

#pragma mark datawidget_base

// *** begin FLUID ***

void datawidget_base::name_field_callback2(Fl_Input*, void*) {
	cout<<"name_field_callback2..."<<endl;
  datamanagement.set_image_name(data_id,string(data_name_field->value()));
}
void datawidget_base::name_field_callback(Fl_Input* o, void* v) {
  ((datawidget_base*)(o->parent()->parent()))->name_field_callback2(o,v);
}

void datawidget_base::edit_geometry_callback(Fl_Widget *callingwidget, void *){
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
//	cout<<"the_datawidget->get_data_id()="<<the_datawidget->get_data_id()<<endl;
    the_datawidget->show_hide_edit_geometry('i');
}
void datawidget_base::edit_curve_geometry_callback(Fl_Widget *callingwidget, void *){
    datawidget_base * the_datawidget=(datawidget_base *)(callingwidget->user_data());
//	cout<<"the_datawidget->get_data_id()="<<the_datawidget->get_data_id()<<endl;
    the_datawidget->show_hide_edit_geometry('c');
}

const Fl_Menu_Item datawidget_base::the_base_items[] = {
 {"Remove", 0,  (Fl_Callback*)datamanager::removedata_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Duplicate", 0,  0, 0, 1, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

const Fl_Menu_Item datawidget_base::the_image_base_items[] = {
 {"Save as DCM", 0,  (Fl_Callback*)datamanager::save_dcm_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save as VTK", 0,  (Fl_Callback*)datamanager::save_vtk_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save histogram", 0,  (Fl_Callback*)datamanager::save_hist_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save additional data", 0,  (Fl_Callback*)datamanager::save_additional_data_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Connect additional data", 0,  (Fl_Callback*)datamanager::connect_additional_data_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Geometry Edit(Show/Hide)", 0, (Fl_Callback*)datawidget_base::edit_geometry_callback,0,0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

const Fl_Menu_Item datawidget_base::the_curve_items[] = {
 {"Remove", 0,  (Fl_Callback*)datamanager::removedata_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save additional data", 0,  (Fl_Callback*)datamanager::save_additional_data_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save curve", 0,  (Fl_Callback*)datamanager::save_curve_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Connect additional data", 0,  (Fl_Callback*)datamanager::connect_additional_data_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Property Edit(Show/Hide)", 0, (Fl_Callback*)datawidget_base::edit_curve_geometry_callback,0,0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

const Fl_Menu_Item datawidget_base::the_point_collection_items[] = {
 {"Remove", 0,  (Fl_Callback*)datamanager::removedata_callback, (void*)(&datamanagement), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

// *** custom constructor declared in FLUID: ***

datawidget_base::datawidget_base(data_base * d, std::string n):Fl_Pack(0,0,270,130,NULL) {
    data_id = d->get_id();
    thumbnail_image = new unsigned char [thumbnail_size*thumbnail_size];
    
    deactivate(); //activated when the image is added to datamanager

    // *** resume FLUID inits ***

    datawidget_base *o = this;
    o->box(FL_THIN_DOWN_FRAME);
    o->color(FL_BACKGROUND_COLOR);
    o->selection_color(FL_BACKGROUND_COLOR);
    o->labeltype(FL_NO_LABEL);
    o->labelfont(FL_HELVETICA_BOLD);
//    o->labelfont(0);
    o->labelsize(14);
    o->labelcolor(FL_FOREGROUND_COLOR);
    o->align(FL_ALIGN_TOP);
    o->when(FL_WHEN_RELEASE);
        { Fl_Pack* o = hpacker = new Fl_Pack(0, 0, 270, 25);
        o->type(1);
            { Fl_Input* o = data_name_field = new Fl_Input(0, 0, 240, 25);
            o->color(FL_LIGHT1);
            o->callback((Fl_Callback*)name_field_callback, (void*)(this));
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
            Fl_Group::current()->resizable(o);
//			o->tooltip( datamanagement.get_data(data_id)->get_tooltip().c_str() ); //widget created before data is fully loaded...
            }
            { Fl_Menu_Button* o = data_menu_button = new Fl_Menu_Button(240, 0, 30, 25);
            o->box(FL_THIN_UP_BOX);
            o->user_data((void*)(this));
            o->menu(NULL);
            }
        resizable(data_name_field);
        o->end();
        }
        { Fl_Box* o = thumbnail = new Fl_Box(0, 25, 270, 65);
        o->box(FL_EMBOSSED_BOX);
        o->hide();
        image( new Fl_RGB_Image(thumbnail_image, thumbnail_size, thumbnail_size, 1));
        image( NULL);
        }
        { Fl_Pack* o = extras = new Fl_Pack(0, 90, 270, 40);
    o->end();
        }
    type(VERTICAL);
    resizable(extras);
    name(n);
    datamanagement.add_datawidget(this);
    end();
    }

// *** end FLUID ***

datawidget_base::~datawidget_base()
{
//	cout<<"~datawidget_base ()"<<endl;
    datamanagement.remove_datawidget(this);

    delete image();
    image(NULL);

    if (thumbnail_image != NULL){
		delete [] thumbnail_image;
//		cout<<"delete [] thumbnail_image"<<endl;
	}

	//JK TODO, delete all objects created...
}

void datawidget_base::copy_items(Fl_Menu_Item *to_items, int to_start_index, const Fl_Menu_Item from_items[])
{
    int s = fl_menu_size(from_items);
    for(int i=0; i<s; i++){        
        to_items[i+to_start_index] = from_items[i];
	}
}

void datawidget_base::copy_items(Fl_Menu_Item *to_items, const Fl_Menu_Item from_items[])
{
	copy_items(to_items,0, from_items);
}


void datawidget_base::refresh_thumbnail ()
    {
    rendermanagement.render_thumbnail(thumbnail_image, thumbnail_size, thumbnail_size, data_id);
    }

int datawidget_base::get_data_id() const
    {
    return data_id;
    }

const string datawidget_base::name() const
    {
    return std::string(_name);
    }

void datawidget_base::name(std::string n)
    {
    _name.assign(n);

    //data_name_field->value(NULL);
    //((Fl_Input *)data_name_field)->value(n.c_str());
    data_name_field->value(_name.c_str());

    //when interactively changed, redrawing widget is
    //done elsewhere (most notably in datamanagement.set_image_name( ... )
    }

void datawidget_base::from_file(bool f)
    {
    fromFile = f;
    }

bool datawidget_base::from_file() const
    {
    return fromFile;
    }

void datawidget_base::show_hide_edit_geometry(char param)
{
	if(geom_widget==NULL){
		if(param == 'c'){
			geom_widget = new FLTKgeom_curve(data_id);
		}else{
			geom_widget = new FLTKgeom_image(data_id);
		}
		extras->add(geom_widget);
		geom_widget->show();
	}else{
		if(geom_widget->visible()){
			geom_widget->hide();
		}else{
			geom_widget->show();
		}
	}
	
	geom_widget->parent()->parent()->parent()->parent()->redraw();
}

void datawidget_base::set_tooltip(string s)
{
	data_name_field->tooltip(strdup(s.c_str()));
}


#pragma mark datawidget<image_base>

const Fl_Menu_Item datawidget<image_base>::tf_mi = {"Transfer function", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0};
const Fl_Menu_Item datawidget<image_base>::tf_show_hide_mi = {"Show/hide", 0,  (Fl_Callback*)cb_show_hide_tfunction, 0, 128, FL_NORMAL_LABEL, 0, 14, 0};

datawidget<image_base>::datawidget(image_base *im, std::string n): datawidget_base(im,n)
{
    { Fl_Group* o = tf_group = new Fl_Group(0, 90, 270, 40);
        o->box(FL_EMBOSSED_FRAME);
        o->labelsize(11);
        o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
        o->hide();
        o->end();
    }
    extras->add(tf_group);


	geom_widget=NULL;	//Cannot access geometry data yet since data has not been added to datamanagement 
//  extras->add(geom_widget);

   //------------------------------------------
    int s = fl_menu_size(the_base_items);
    int s2 = fl_menu_size(the_image_base_items);
    the_image_items = new Fl_Menu_Item[s+s2+1+1];
	this->copy_items(the_image_items, the_base_items);
	this->copy_items(the_image_items, s, the_image_base_items);

	the_image_items[s+s2] = tf_mi;
    setup_transfer_menu(the_image_items[s+s2],im);
    the_image_items[s+s2+1].label(NULL);
    
   //------------------------------------------

    data_menu_button->menu(the_image_items);
}


void datawidget<image_base>::setup_transfer_menu(Fl_Menu_Item &submenuitem, image_base *im) {
    const int NUM_ITEMS = 1;

	Fl_Menu_Item *tfunctions = transfer_manufactured::factory.function_menu(static_cast<Fl_Callback*>(cb_transferswitch));
    int subMenuSize = NUM_ITEMS+fl_menu_size(tfunctions);
    tfunction_submenu = new Fl_Menu_Item[subMenuSize+1];
    tfunction_submenu[0] = tf_show_hide_mi;
    
    for(int i=NUM_ITEMS; i<subMenuSize; i++){
        int fmenuindex = i-NUM_ITEMS;
        tfunction_submenu[i] = tfunctions[fmenuindex];
        tfunction_submenu[i].user_data(new transferfactory::tf_menu_params (transferfactory::tf_name(fmenuindex), im ));
	}

	tfunction_submenu[subMenuSize].label(NULL);
    tfunction_submenu[NUM_ITEMS].set();
    
    submenuitem.flags = FL_SUBMENU_POINTER;
    submenuitem.user_data(tfunction_submenu);
    
    delete [] tfunctions;
}

datawidget<image_base>::~datawidget()
{
    fl_menu_userdata_delete(tfunction_submenu);
    delete [] the_image_items;
    delete [] tfunction_submenu;
}

Fl_Group * datawidget<image_base>::reset_tf_controls()
{
    tf_group->clear();
    tf_group->begin();
    return tf_group;
}

void datawidget<image_base>::cb_transferswitch(Fl_Widget* callingwidget, void* v) {
	cout<<"cb_transferswitch"<<endl;
	transferfactory::tf_menu_params * par = reinterpret_cast<transferfactory::tf_menu_params *>(v);
	par->switch_tf();

	//Each item has space for a callback function and an argument for that function. 
	//Due to back compatability, the Fl_Menu_Item itself is not passed to the callback, 
	//instead you have to get it by calling ((Fl_Menu_*)w)->mvalue() where w is the widget argument.
	const Fl_Menu_Item * item = reinterpret_cast<Fl_Menu_*>(callingwidget)->mvalue();
	const_cast<Fl_Menu_Item *>(item)->setonly();

	datawidget<image_base> * the_datawidget = dynamic_cast<datawidget<image_base> * >(reinterpret_cast<datawidget_base *>(callingwidget->user_data()));
	pt_error::error_if_null(the_datawidget,"cb_transferswitch called with datawidget type not being datawidget<image_base>",pt_error::fatal);

	//    the_datawidget->tf_group->hide();
	//resize the the current "extras" Fl_Pack...
	the_datawidget->extras->size(the_datawidget->extras->w(),the_datawidget->extras->h()+the_datawidget->tf_group->h());
	the_datawidget->tf_group->show();	//Always show the GUI for a selected transfer function 
	the_datawidget->parent()->parent()->redraw();	
}


void datawidget<image_base>::cb_show_hide_tfunction(Fl_Widget* callingwidget, void*)
{
	cout<<"cb_show_hide_tfunction"<<endl;
    //datawidget_base * the_datawidget_base=reinterpret_cast<datawidget_base *>(callingwidget->user_data());
    datawidget<image_base> * the_datawidget = dynamic_cast<datawidget<image_base> * >(reinterpret_cast<datawidget_base *>(callingwidget->user_data()));
    
    pt_error::error_if_null(the_datawidget,"cb_show_hide_tfunction called with datawidget type not being datawidget<image_base>",pt_error::fatal);
    
    if(the_datawidget->tf_group->visible()){
        the_datawidget->tf_group->hide();
	}else{ 
		the_datawidget->extras->size(the_datawidget->extras->w(),the_datawidget->extras->h()+the_datawidget->tf_group->h());
		the_datawidget->tf_group->show();
	}
    
    the_datawidget->parent()->parent()->redraw();
}




#pragma mark datawidget<curve_base>

datawidget<curve_base>::datawidget(curve_base *p, std::string n): datawidget_base(p,n)
{
    data_menu_button->menu(the_curve_items);
	geom_widget = NULL;
}


#pragma mark datawidget<point_collection>

datawidget<point_collection>::datawidget (point_collection* p, std::string n): datawidget_base (p,n)
{
    data_menu_button->menu(the_point_collection_items);
}

//---------------------- 


FLTKslice_orientation_menu::FLTKslice_orientation_menu(string slice_orientation, int x, int y, int w, int h):Fl_Group(x,y,w,h)
{
	this->color(FL_BACKGROUND_COLOR);
	slice_menu = new Fl_Menu_Button(x,y,w,h);
    slice_menu->box(FL_THIN_UP_BOX);
    slice_menu->labelsize(FLTK_SMALL_LABEL);
	slice_menu->label(strdup(slice_orientation.c_str()));

	//-----------------------------
    Fl_Menu_Item slice_menu_items[4+1];
    int m;
    for(m=0;m<4;m++){
        init_fl_menu_item(slice_menu_items[m]);
        slice_menu_items[m].label(slice_orientation_labels[m]);
        slice_menu_items[m].flags= FL_MENU_RADIO;
    }
    slice_menu_items[m].label(NULL);    //terminate menu
	//-----------------------------

	slice_menu->callback(slice_menu_cb);
    slice_menu->copy(slice_menu_items);
	end();
}

void FLTKslice_orientation_menu::slice_menu_cb(Fl_Widget *w, void*)
{
	cout<<"slice_menu_cb(Fl_Widget *w, void*)"<<endl;
	FLTKslice_orientation_menu* m = (FLTKslice_orientation_menu*)w->parent();
	m->do_callback(m);
}

void FLTKslice_orientation_menu::value(string s)
{
	slice_menu->label(strdup(s.c_str()));
    //slice_menu_items[AXIAL].setonly(); //AXIAL_NEG
}

string FLTKslice_orientation_menu::value()
{
//	cout<<"val="<<slice_menu->value()<<endl;
    ((Fl_Menu_Item*)slice_menu->menu())[slice_menu->value()].setonly();
//	cout<<"lab="<<mi[slice_menu->value()].label()<<endl;
	slice_menu->label(  ((Fl_Menu_Item*)slice_menu->menu())[slice_menu->value()].label()  );
	return string(  ((Fl_Menu_Item*)slice_menu->menu())[slice_menu->value()].label()  );
}



#pragma mark FLTKVector3D
FLTKVector3D::FLTKVector3D(Vector3D v, int x, int y, int w, int h, const char *sx, const char *sy, const char *sz):Fl_Group(x,y,w,h)
{
	int dh = int(float(h)/3.0);
	const int margin = 15;
	
	data_x = new Fl_Value_Input(x + margin, y, w - margin, dh-2, sx);
	data_y = new Fl_Value_Input(x + margin, y + dh, w - margin, dh-2, sy);
	data_z = new Fl_Value_Input(x + margin, y + 2*dh, w - margin, dh-2, sz);

	data_x->callback(vector_cb);	data_x->when(FL_WHEN_RELEASE);
	data_y->callback(vector_cb);	data_y->when(FL_WHEN_RELEASE);
	data_z->callback(vector_cb);	data_z->when(FL_WHEN_RELEASE);

	value(v);
	
//	resizable(NULL);

	end();
}	

void FLTKVector3D::value(Vector3D v)
{
	data_x->value(v[0]);
	data_y->value(v[1]);
	data_z->value(v[2]);
}

Vector3D FLTKVector3D::value()
{
	Vector3D v;
	v[0] = data_x->value();
	v[1] = data_y->value();
	v[2] = data_z->value();
	return v;
}

/*------------------------------------------------------------*/
FLTKVector2D::FLTKVector2D(Vector2D v, int x, int y, int w, int h, const char *sx, const char *sy):Fl_Group(x,y,w,h)
{
	int dh = int(float(h)/2.0);
	const int margin = 15;
	
	data_x = new Fl_Value_Input(x + margin, y, w - margin, dh-2, sx);
	data_y = new Fl_Value_Input(x + margin, y + dh, w - margin, dh-2, sy);
	data_x->callback(vector_cb);	data_x->when(FL_WHEN_RELEASE);
	data_y->callback(vector_cb);	data_y->when(FL_WHEN_RELEASE);

	value(v);
	
//	resizable(NULL);

	end();
}	

void FLTKVector2D::value(Vector2D v)
{
	data_x->value(v[0]);
	data_y->value(v[1]);
}

Vector2D FLTKVector2D::value()
{
	Vector2D v;
	v[0] = data_x->value();
	v[1] = data_y->value();
	return v;
}

/*------------------------------------------------------------*/
void FLTKVector2D::vector_cb(Fl_Widget *w, void*)
{
//	cout<<"vector_cb(Fl_Widget *w, void*)"<<endl;
	FLTKVector2D* v2D = (FLTKVector2D*)w->parent();
	v2D->do_callback(v2D);
}
/*----------------------------------------------------------*/

FLTKButton::FLTKButton(int x, int y, int w, int h, const char *sx):Fl_Group(x,y,w,h)
{
	int dh = int(float(h)/2.0);
	const int margin = 15;
	
	butt = new Fl_Button(x + margin, y, w - margin, dh-2, sx);
	butt->callback(button_cb);	butt->when(FL_WHEN_RELEASE);

	end();
}	

/*------------------------------------------------------------*/
void FLTKButton::button_cb(Fl_Widget *w, void*)
{
//	cout<<"vector_cb(Fl_Widget *w, void*)"<<endl;
	FLTKButton* b = (FLTKButton*)w->parent();
	b->do_callback(b);
}

/*----------------------------------------------------------*/

void FLTKVector3D::vector_cb(Fl_Widget *w, void*)
{
//	cout<<"vector_cb(Fl_Widget *w, void*)"<<endl;
	FLTKVector3D* v3D = (FLTKVector3D*)w->parent();
	v3D->do_callback(v3D);
}

FLTKCheckButton::FLTKCheckButton(int x, int y, int w, int h, const char *sx):Fl_Group(x,y,w,h)
{
	int dh = int(float(h)/2.0);
	const int margin = 15;
	
	butt = new Fl_Check_Button(x + margin, y, w - margin, dh-2, sx);
	butt->callback(check_cb);	butt->when(FL_WHEN_RELEASE);
	butt->value(0);

	end();
}	

/*------------------------------------------------------------*/
void FLTKCheckButton::check_cb(Fl_Widget *w, void*)
{
//	cout<<"vector_cb(Fl_Widget *w, void*)"<<endl;
	FLTKButton* b = (FLTKButton*)w->parent();
	b->do_callback(b);
}
bool FLTKCheckButton::turned_on(){
	bool on = butt->value() ? 1 : 0;
	return on;
}


//-----------------------------
FLTKMatrix3D::FLTKMatrix3D(Matrix3D m, int x, int y, int w, int h):Fl_Group(x,y,w,h)
{
	int dh = int(float(h)/3.0);
	int dw = int(float(w)/3.0);
	data_00 = new Fl_Value_Input(x+5	,y		,dw-5,dh-2);
	data_10 = new Fl_Value_Input(x+5	,y+dh	,dw-5,dh-2);
	data_20 = new Fl_Value_Input(x+5	,y+2*dh	,dw-5,dh-2);
	data_01 = new Fl_Value_Input(x+5+dw	,y		,dw-5,dh-2);
	data_11 = new Fl_Value_Input(x+5+dw	,y+dh	,dw-5,dh-2);
	data_21 = new Fl_Value_Input(x+5+dw	,y+2*dh	,dw-5,dh-2);
	data_02 = new Fl_Value_Input(x+5+2*dw,y		,dw-5,dh-2);
	data_12 = new Fl_Value_Input(x+5+2*dw,y+dh	,dw-5,dh-2);
	data_22 = new Fl_Value_Input(x+5+2*dw,y+2*dh,dw-5,dh-2);

	data_00->callback(matrix_cb);	data_00->when(FL_WHEN_RELEASE);
	data_01->callback(matrix_cb);	data_01->when(FL_WHEN_RELEASE);
	data_02->callback(matrix_cb);	data_02->when(FL_WHEN_RELEASE);
	data_10->callback(matrix_cb);	data_10->when(FL_WHEN_RELEASE);
	data_11->callback(matrix_cb);	data_11->when(FL_WHEN_RELEASE);
	data_12->callback(matrix_cb);	data_12->when(FL_WHEN_RELEASE);
	data_20->callback(matrix_cb);	data_20->when(FL_WHEN_RELEASE);
	data_21->callback(matrix_cb);	data_21->when(FL_WHEN_RELEASE);
	data_22->callback(matrix_cb);	data_22->when(FL_WHEN_RELEASE);

	value(m);

	end();
}	

void FLTKMatrix3D::value(Matrix3D m)
{
	data_00->value(m[0][0]);
	data_01->value(m[0][1]);
	data_02->value(m[0][2]);

	data_10->value(m[1][0]);
	data_11->value(m[1][1]);
	data_12->value(m[1][2]);

	data_20->value(m[2][0]);
	data_21->value(m[2][1]);
	data_22->value(m[2][2]);
}

Matrix3D FLTKMatrix3D::value()
{
	Matrix3D m;
	m[0][0] = data_00->value();
	m[0][1] = data_01->value();
	m[0][2] = data_02->value();

	m[1][0] = data_10->value();
	m[1][1] = data_11->value();
	m[1][2] = data_12->value();

	m[2][0] = data_20->value();
	m[2][1] = data_21->value();
	m[2][2] = data_22->value();
	return m;
}
void FLTKMatrix3D::matrix_cb(Fl_Widget *w, void*)
{
//	cout<<"matrix_cb(Fl_Widget *w, void*)"<<endl;
	FLTKMatrix3D* m3D = (FLTKMatrix3D*)w->parent();
	m3D->do_callback(m3D);
}


//--------------------------
#pragma mark FLTKgeom_base
FLTKgeom_base::FLTKgeom_base(int id, int x, int y, int w, int h):Fl_Group(x,y,w,h)
{
    data_id = id;
//	end();
}

//FLTKgeom_image::FLTKgeom_image(image_base *im):FLTKgeom_image(im->get_id()){}


FLTKgeom_image::FLTKgeom_image(int id, int x, int y, int w, int h):FLTKgeom_base(id,x,y,w,h)
{

	// the sum of these should be <= w
	const int orig_w = 17.0/60.0*w;
	const int size_w = 10.0/60.0*w;
	const int orient_w = 18.0/60.0*w;
	const int rotation_w = 15.0/60.0*w;
	const int slice_w = 20.0/60.0*w;

	int h_coord = h*3/4;

	orig = new FLTKVector3D(datamanagement.get_image<image_base>(data_id)->get_origin(), x, y, orig_w, h_coord, "x", "y", "z"); //TODO_R HÄR!!!!!
	size = new FLTKVector3D(datamanagement.get_image<image_base>(data_id)->get_voxel_size(), x + orig_w, y, size_w, h_coord, "dx", "dy", "dz"); //TODO_R HÄR!!!!!
	orient = new FLTKMatrix3D(datamanagement.get_image<image_base>(data_id)->get_orientation(), x + orig_w + size_w, y, orient_w, h_coord); //TODO_R HÄR!!!!!
	
	//start = datamanagement.get_image(data_id)->get_orientation();
	
	Vector3D r;
	r.Fill(0);
	rotation = new FLTKVector3D(r, x + orig_w + size_w + orient_w, y, rotation_w, h_coord, "x", "y", "z");
	
	slice = new FLTKslice_orientation_menu(datamanagement.get_image<image_base>(data_id)->get_slice_orientation(),x,h_coord,slice_w,h-h_coord-1); //TODO_R HÄR!!!!!

	orig->callback(orig_update_cb);
	size->callback(size_update_cb);
	orient->callback(orient_update_cb);
	rotation->callback(rotation_update_cb);
	slice->callback(slice_orient_update_cb);
		
	resizable(NULL);
	
	end();
}

void FLTKgeom_image::orig_update_cb(Fl_Widget *w, void*)
{
	FLTKVector3D *v = (FLTKVector3D*)w;
	FLTKgeom_image *g = (FLTKgeom_image*)v->parent();
	datamanagement.get_image<image_base>(g->data_id)->set_origin(v->value()); //TODO_R HÄR!!!!!
	datamanagement.data_has_changed(g->data_id);
}

void FLTKgeom_image::size_update_cb(Fl_Widget *w, void*)
{
	FLTKVector3D *v = (FLTKVector3D*)w;
	FLTKgeom_image *g = (FLTKgeom_image*)v->parent();
	datamanagement.get_image<image_base>(g->data_id)->set_voxel_size(v->value()); //TODO_R HÄR!!!!!
	datamanagement.data_has_changed(g->data_id);
}

void FLTKgeom_image::orient_update_cb(Fl_Widget *w, void*)
{
	FLTKMatrix3D *m = (FLTKMatrix3D*)w;
	FLTKgeom_image *g = (FLTKgeom_image*)m->parent();
	datamanagement.get_image<image_base>(g->data_id)->set_orientation(m->value()); //TODO_R HÄR!!!!!
	datamanagement.data_has_changed(g->data_id);
}

void FLTKgeom_image::rotation_update_cb ( Fl_Widget * w, void * )
{
	FLTKVector3D * v = dynamic_cast<FLTKVector3D *> ( w );
	FLTKgeom_image * g = dynamic_cast<FLTKgeom_image *> ( v->parent() );

	int nc = g->children();
	
	FLTKMatrix3D * matrix3d;
	
	for ( int c = 0; c < nc; c++ )
	{
		if ( matrix3d = dynamic_cast<FLTKMatrix3D *>( g->child(c) ) )
			{ break; }
	}
	
	Matrix3D m = matrix3d->value();
	
	Vector3D angle = v->value();
	
	// convert degrees to radians
	angle *= ( pt_PI / 180.0 );	
	
//	matrix_generator mg;
//	m = mg.get_rot_matrix_3D ( angle[2], angle[1], angle[0] ) * datamanagement.get_image(g->data_id)->get_orientation();
//	m = mg.get_rot_matrix_3D ( angle[2], angle[1], angle[0] ) * g->get_start();
	m = create_rot_matrix_3D(angle[0], angle[1], angle[2]) * datamanagement.get_image<image_base>(g->data_id)->get_orientation(); //TODO_R HÄR!!!!!

	matrix3d->value(m);
	
	datamanagement.get_image<image_base>(g->data_id)->set_orientation(m); //TODO_R HÄR!!!!!
	datamanagement.data_has_changed(g->data_id);
	
	// rendermanagement.center3d_and_fit( g->data_id );	
}


void FLTKgeom_image::slice_orient_update_cb(Fl_Widget *w, void*)
{
	FLTKslice_orientation_menu *m = (FLTKslice_orientation_menu*)w;
	FLTKgeom_image *g = (FLTKgeom_image*)m->parent();
	datamanagement.get_image<image_base>(g->data_id)->set_slice_orientation(m->value()); //TODO_R HÄR!!!!!
	datamanagement.data_has_changed(g->data_id);
}


FLTKgeom_curve::FLTKgeom_curve(int id, int x, int y, int w, int h):FLTKgeom_base(id,x,y,w,h){
	
	const int slice_w = 20.0/60.0*w;

	int h_coord = h*3/4;
	int button_increase = h_coord/2;
	
	
	Vector2D res;
	res[0] = datamanagement.get_image<curve_base>(data_id)->get_scale();
	res[1] = datamanagement.get_image<curve_base>(data_id)->get_offset();
	int x_val;
	int y_val;
	fl_measure("x offset",x_val,y_val);
	int b_x, b_y;
	fl_measure("dummy",b_x,b_y);
	
	x_resolution = new FLTKVector2D(res, x+x_val, y, slice_w, h_coord, "x scale", "x offset");
	smooth = new FLTKButton(x+x_val+slice_w, y, b_x*2, h_coord, "dummy");
	//other = new FLTKButton(x+x_val+slice_w, y+button_increase, b_x*2, h_coord, "dummy2");
	other = new FLTKCheckButton(x+x_val+slice_w, y+button_increase, b_x*2, h_coord, "h data");
	//Knappar
	x_resolution->callback(x_offset_update_cb);
	smooth->callback(button_update_cb);
	other->callback(check_update_cb);

	resizable(NULL);
}

void FLTKgeom_curve::x_offset_update_cb(Fl_Widget *w, void*)
{
	FLTKVector2D *v = (FLTKVector2D*)w;
	FLTKgeom_curve *g = (FLTKgeom_curve*)v->parent();
	datamanagement.get_image<curve_base>(g->data_id)->set_scale(v->value()[0]);
	datamanagement.get_image<curve_base>(g->data_id)->set_offset(v->value()[1]);
	datamanagement.data_has_changed(g->data_id);
}

void FLTKgeom_curve::button_update_cb(Fl_Widget *w, void*)
{
	FLTKButton *v = (FLTKButton*)w;
	FLTKgeom_curve *g = (FLTKgeom_curve*)v->parent();
	//datamanagement.get_image<curve_base>(g->data_id)->increase_resolution();
	std::cout << "dummy button pushed" << std::endl;
	datamanagement.data_has_changed(g->data_id);
}
void FLTKgeom_curve::button2_update_cb(Fl_Widget *w, void*)
{
	FLTKButton *v = (FLTKButton*)w;
	FLTKgeom_curve *g = (FLTKgeom_curve*)v->parent();
	//datamanagement.get_image<curve_base>(g->data_id)->increase_resolution();
	std::cout << "dummy2 button pushed" << std::endl;
	datamanagement.data_has_changed(g->data_id);
}
void FLTKgeom_curve::check_update_cb(Fl_Widget *w, void*)
{
	FLTKCheckButton *v = (FLTKCheckButton*)w;
	FLTKgeom_curve *g = (FLTKgeom_curve*)v->parent();
	//datamanagement.get_image<curve_base>(g->data_id)->increase_resolution();
	std::cout << "draw additional data: " << v->turned_on() << std::endl;
	datamanagement.get_data(g->data_id)->draw_additional_data = v->turned_on();
	datamanagement.data_has_changed(g->data_id);
}


//const Matrix3D FLTKgeom_image::get_start() const
//{
//	return start;
//}


//	static void orient_update_cb(Fl_Widget *w, void*);

