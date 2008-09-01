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
/*class data_base;
class image_base;
class point_collection;*/

class FLTKgeom_base;


class datawidget_base : public Fl_Pack {
protected:
    int data_id;  
    bool fromFile; ///indicates whether the data was created inside the program and perhaps needs to be saved;
    std::string _name;
    
    Fl_Pack *hpacker;
    Fl_Input *datanamebutton; //JK set tooltip on this 
    void cb_filenamebutton_i(Fl_Input*, void*);
    static void cb_filenamebutton(Fl_Input*, void*);
	static void edit_geometry_callback(Fl_Widget *callingwidget, void *);
	Fl_Menu_Button *featuremenu;

    // *** thumbnail
    const static int thumbnail_size;
    uchar * thumbnail_image;
    Fl_Box *thumbnail;
    
    // *** menus       
    enum {remove_mi_num=0,save_mi_num, dup_mi_num};
    const static Fl_Menu_Item menu_featuremenu_base[];
    const static Fl_Menu_Item menu_featuremenu_point_collection[];
    const static Fl_Menu_Item *remove_mi;
    const static Fl_Menu_Item *save_vtk_mi;
    const static Fl_Menu_Item *duplicate_mi;
    Fl_Pack *extras;

	FLTKgeom_base *geom_widget;
    
    datawidget_base(data_base * d, std::string n);
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

	void show_hide_edit_geometry();

	void set_tooltip(string s);
    };

template <class DATATYPE>
class datawidget:public datawidget_base
{};

template <>
class datawidget<point_collection>:public datawidget_base
{
public:
    datawidget(point_collection* p, std::string n);
};

template <>
class datawidget<image_base>:public datawidget_base
{   
    const static Fl_Menu_Item tfunctionmenu;			//the extra "transfer_funtion row"
    const static Fl_Menu_Item transferfunction_mi; 
    static void cb_transferswitch(Fl_Widget* callingwidget, void* v);
    static void cb_show_hide_tfunction(Fl_Widget* callingwidget, void*);
    
    Fl_Group *tfunction_;
    Fl_Menu_Item * menu_featuremenu_plustf;		//The "new" version of the "main menu"

    Fl_Menu_Item * tfunction_submenu;			//The transfer function submenu. Transferfactory handles all versions of transfer functions

    void setup_transfer_menu(Fl_Menu_Item* submenuitem, image_base * im);
public:
	datawidget(image_base* im, std::string n);
    virtual ~datawidget();
    
    void tfunction(Fl_Group * t);
    Fl_Group * reset_tf_controls();
    
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
	
	//Matrix3D start;
	
public:
	FLTKgeom_image(int id, int x=0, int y=0, int w=260, int h=50);
	static void orig_update_cb(Fl_Widget *w, void*);
	static void size_update_cb(Fl_Widget *w, void*);
	static void orient_update_cb(Fl_Widget *w, void*);
	static void rotation_update_cb ( Fl_Widget * w, void * );
	
	//const Matrix3D get_start() const;
};

#endif