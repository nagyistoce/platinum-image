//////////////////////////////////////////////////////////////////////////
//
//  Datawidget $Revision$
//
//  The datawidget is the GUI representation of a dataset (image/point).
//  It provides controls for saving, deleting and shows the thumbnail.
//	The FLTKgeom_base class aids the displaying/editing of geometrical information.
//
//  $LastChangedBy$
//

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

#ifndef __datawidget__
#define __datawidget__

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Value_Slider.H> 
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Box.H>

/*typedef enum {
    image,
    points
    } datawidget_type;*/

#include "global.h"

#define MAXDATANAME 512

#include "image_base.h"
#include "point_collection.h"
#include "curve.h"
/*class data_base;
class image_base;
class point_collection;*/

class FLTKgeom_base;


class datawidget_base : public Fl_Pack {

protected:
    datawidget_base(data_base *d, std::string n);

    int data_id;  
    bool fromFile; ///indicates whether the data was created inside the program and perhaps needs to be saved;

    Fl_Pack *hpacker;
	std::string _name;
    Fl_Input *data_name_field;
    void name_field_callback2(Fl_Input*, void*);
    static void name_field_callback(Fl_Input*, void*);

    // *** thumbnail
    const static int thumbnail_size;
    uchar *thumbnail_image;
	Fl_RGB_Image *fl_thumbnail_image;
 //   Fl_Box *thumbnail;

    // *** menus       
	Fl_Menu_Button *data_menu_button;
    enum {remove_mi_num=0,save_mi_num, dup_mi_num};
    const static Fl_Menu_Item the_base_items[];
    const static Fl_Menu_Item the_image_base_items[];
    const static Fl_Menu_Item the_curve_items[];
	const static Fl_Menu_Item the_complex_curve_items[];
    const static Fl_Menu_Item the_point_collection_items[];
    const static Fl_Menu_Item *remove_mi;		//mi = menu item
    const static Fl_Menu_Item *save_vtk_mi;
    const static Fl_Menu_Item *duplicate_mi;

	void copy_items(Fl_Menu_Item *to_items, int to_start_index, const Fl_Menu_Item from_items[]);
	void copy_items(Fl_Menu_Item *to_items, const Fl_Menu_Item from_items[]);

	Fl_Pack *extras;

	FLTKgeom_base *geom_widget;
	static void edit_geometry_callback(Fl_Widget *callingwidget, void *);
	static void edit_curve_geometry_callback(Fl_Widget *callingwidget, void *);
	static void additional_data_callback(Fl_Widget *callingwidget, void *);
	static void edit_shift_callback(Fl_Widget *callingwidget, void *);

public:
    static void change_name_callback(Fl_Widget *callingwidget, void *thisdatawidget);


    //void make_window();

    virtual ~datawidget_base ();
    void refresh_thumbnail ();

    // *** access methods ***
    int get_data_id() const;
    
    void name(std::string n);
    const std::string name() const;
    
    void from_file(bool);
    bool from_file() const;

	void show_hide_edit_geometry(char param = 'i');
	void edit_shift();
	void additional_data();

	void set_tooltip(const char* s);
    };

template <class DATATYPE>
class datawidget:public datawidget_base
{};

template <>
class datawidget<curve_complex_base>:public datawidget_base
{
public:
    datawidget(curve_complex_base *p, std::string n);
};

template <>
class datawidget<curve_base>:public datawidget_base
{
public:
    datawidget(curve_base *p, std::string n);
};

template <>
class datawidget<point_collection>:public datawidget_base
{
public:
    datawidget(point_collection *p, std::string n);
};

template <>
class datawidget<image_base>:public datawidget_base
{   
private:
    Fl_Group *tf_group;

	Fl_Menu_Item *the_image_items;			//The "new" version of the "main menu"
    Fl_Menu_Item *tfunction_submenu;			//The transfer function submenu. Transferfactory handles all versions of transfer functions
    const static Fl_Menu_Item tf_mi;			//The extra "transfer_funtion" row
    const static Fl_Menu_Item tf_show_hide_mi;	//The first Show/Hide tf row...

    void setup_transfer_menu(Fl_Menu_Item &submenuitem, image_base *im);

	static void cb_transferswitch(Fl_Widget* callingwidget, void* v);
    static void cb_show_hide_tfunction(Fl_Widget* callingwidget, void*);

public:
	datawidget(image_base *im, std::string n);
    virtual ~datawidget();
    
//    void tfunction(Fl_Group *t);
    Fl_Group *reset_tf_controls();
};


//---------------------- 

class FLTKslice_orientation_menu : public Fl_Group {
protected:
	Fl_Menu_Button *slice_menu;
	static void slice_menu_cb(Fl_Widget *w, void*);
	string my_slice_orientation_name;

public:
	FLTKslice_orientation_menu(string slice_orientation_name, int x=0, int y=0, int w=50, int h=10);
	void value(string slice_orientation);
	string value();
};


//----------------------
class FLTKVector3D : public Fl_Group {
protected:
	Fl_Value_Input *data_x;
	Fl_Value_Input *data_y;
	Fl_Value_Input *data_z;
	static void vector_cb(Fl_Widget *w, void*);
public:
	FLTKVector3D(Vector3D v, int x=0, int y=0, int w=30, int h=50, const char *sx=0, const char *sy=0, const char *sz=0);
	void value(Vector3D v);
	Vector3D value();
};

//----------------------
class FLTKVector2D : public Fl_Group {
protected:
	Fl_Value_Input *data_x;
	Fl_Value_Input *data_y;
	static void vector_cb(Fl_Widget *w, void*);
public:
	FLTKVector2D(Vector2D v, int x=0, int y=0, int w=30, int h=50, const char *sx=0, const char *sy=0);
	void value(Vector2D v);
	Vector2D value();
};

//----------------------
class FLTKButton : public Fl_Group {
protected:
	Fl_Button *butt;
	static void button_cb(Fl_Widget *w, void*);
public:
	FLTKButton(int x=0, int y=0, int w=30, int h=50, const char *sx=0);
};

class FLTKCheckButton : public Fl_Group {
protected:
	Fl_Check_Button *butt;
	static void check_cb(Fl_Widget *w, void*);
public:
	FLTKCheckButton(int x=0, int y=0, int w=30, int h=50, const char *sx=0);
	bool turned_on();
};

class FLTKSlide : public Fl_Group {
protected:
	Fl_Value_Slider *slide;
	static void slide_cb(Fl_Widget *w, void*);
public:
	FLTKSlide(int x=0, int y=0, int w=30, int h=50, const char *sx=0, float min = 0, float max = 10, float step = 1);
	float get_value();
};
//--------------------------------
class FLTKMatrix3D : public Fl_Group {
protected:
	Fl_Value_Input *data_00;	Fl_Value_Input *data_01;	Fl_Value_Input *data_02;
	Fl_Value_Input *data_10;	Fl_Value_Input *data_11;	Fl_Value_Input *data_12;
	Fl_Value_Input *data_20;	Fl_Value_Input *data_21;	Fl_Value_Input *data_22;
	static void matrix_cb(Fl_Widget *w, void*);
public:
	FLTKMatrix3D(Matrix3D m, int x=0, int y=0, int w=90, int h=50);
	void value(Matrix3D m);
	Matrix3D value();
};


//----------------------
class FLTKgeom_base : public Fl_Group{
protected:
    int data_id;  
public:
	FLTKgeom_base(int id, int x=0, int y=0, int w=280, int h=50);
};

/*
FLTKgeom_point_collection:public FLTKgeom_base{
public:
//    FLTKgeom_point_collection(point_collection* p, std::string n);
};
*/

class FLTKgeom_image : public FLTKgeom_base{
protected:
	FLTKVector3D *orig;
	FLTKVector3D *size;
	FLTKMatrix3D *orient;
	FLTKVector3D *rotation;
	FLTKslice_orientation_menu *slice;
	
	//Matrix3D start;
	
public:
	FLTKgeom_image(int id, int x=0, int y=0, int w=260, int h=60);
	static void orig_update_cb(Fl_Widget *w, void*);
	static void size_update_cb(Fl_Widget *w, void*);
	static void orient_update_cb(Fl_Widget *w, void*);
	static void rotation_update_cb(Fl_Widget * w, void * );
	static void slice_orient_update_cb(Fl_Widget * w, void * );
	
	//const Matrix3D get_start() const;
};


class FLTKgeom_curve : public FLTKgeom_base{
protected:
	FLTKVector2D *x_resolution;
	FLTKButton* smooth;
	FLTKCheckButton* other;
	//Matrix3D start;
	
public:
	FLTKgeom_curve(int id, int x=0, int y=0, int w=260, int h=60);
	static void x_offset_update_cb(Fl_Widget *w, void*);
	static void button_update_cb(Fl_Widget *w, void*);
	static void button2_update_cb(Fl_Widget *w, void*);
	static void check_update_cb(Fl_Widget *w, void*);
	//const Matrix3D get_start() const;
};


class FLTKhelper_data : public FLTKgeom_base{
protected:
	FLTKButton* submit;
	FLTKButton* fill;
	FLTKButton* remove;
	vector<FLTKCheckButton*> data;
	vector<FLTKCheckButton*> fill_check;
	//Matrix3D start;
	
public:
	FLTKhelper_data(int id, int x=0, int y=0, int w=260, int h=60);
	static void button_update_cb(Fl_Widget *w, void*);
	static void button_remove_cb(Fl_Widget *w, void*);
	//const Matrix3D get_start() const;
};

class FLTKShift : public FLTKgeom_base{
protected:
	FLTKSlide* phase;
	FLTKButton* amares;
	//Matrix3D start;
	
public:
	FLTKShift(int id, int x=0, int y=0, int w=260, int h=60);
	static void phase_cb(Fl_Widget *w, void*);
	static void amares_cb(Fl_Widget *w, void*);
	//const Matrix3D get_start() const;
};


#endif