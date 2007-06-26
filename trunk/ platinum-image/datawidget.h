//////////////////////////////////////////////////////////////////////////
//
//  Datawidget $Revision$
//
//  The datawidget is the GUI representation of a dataset (image/point).
//  It provides controls for saving, deleting and shows the thumbnail.
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
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Box.H>

typedef enum {
    DATAHANDLER_VOLUME_3D= 1,
    DATAHANDLER_VECTOR_3D,
    DATAHANDLER_PIXMAP
    } datawidget_type;

#include "global.h"

#include "listedfactory.h"

#define MAXDATANAME 512

const std::string tfunction_names[] =
    {"Default",
    "Brightness/contrast",
    "Labels",
    "Linear",
    "Spline","" };

template <class ELEMTYPE >
class transfer_base;

template <class ELEMTYPE >
class image_storage;

class transferfactory //! transfer gets its own object factory type because constructors for templated classes cannot be stored
    {
    protected:
        int num_items;
    public:
        transferfactory ();

        ~transferfactory();

        template <class ELEMTYPE >
            transfer_base<ELEMTYPE > * Create(factoryIdType unique_id,image_storage<ELEMTYPE > * s);

        Fl_Menu_Item * function_menu (Fl_Callback * cb);
    };

// *** begin datawidget.fl

class datawidget : public Fl_Pack {
public:
  datawidget(int X, int Y, int W, int H, const char *L = 0); //don't use - just to make FLUID happy
private:
  int image_id;
  const static int thumbnail_size;
  uchar * thumbnail_image;
  std::string _name;
  enum {remove_mi_num=0,save_mi_num, dup_mi_num};
  Fl_Pack *hpacker;
  Fl_Input *filenamebutton;
  void cb_filenamebutton_i(Fl_Input*, void*);
  static void cb_filenamebutton(Fl_Input*, void*);
  Fl_Menu_Button *featuremenu;
  static Fl_Menu_Item menu_featuremenu[];
  Fl_Menu_Item * tfunction_submenu;
public:
  static Fl_Menu_Item *remove_mi;
  static Fl_Menu_Item *save_vtk_mi;
  static Fl_Menu_Item *duplicate_mi;
  static Fl_Menu_Item *tfunctionmenu;
  static Fl_Menu_Item *transferfunction_mi;
private:
  Fl_Box *thumbnail;
public:
  Fl_Pack *extras;
private:
  Fl_Group *tfunction_;
public:
  datawidget(int datatype,int id, std::string n);
  void tfunction(Fl_Group * t);;
  static void toggle_tfunction(Fl_Widget* callingwidget, void*);
  Fl_Group * reset_tf_controls();
  static void cb_transferswitch(Fl_Widget* o, void* v);
  void setup_transfer_menu(Fl_Menu_Item* submenuitem);

  // *** end datawidget.fl

protected:
    bool fromFile; ///indicates whether the data was created inside the program and perhaps needs to be saved;
public:
    ~datawidget ();
    void refresh_thumbnail ();

    static void change_name_callback(Fl_Widget *callingwidget, void *thisdatawidget);

    // *** access methods ***
    int get_image_id();
    void name(std::string n);
    const std::string name();
    void from_file(bool);
    bool from_file();
    };

#endif