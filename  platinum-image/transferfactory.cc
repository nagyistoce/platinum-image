//$Id$

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

#include "FLTKutilities.h"
#include "transfer.h"

//adding a new transfer function:

//1. add name to tfunction_names
//2. add a corresponding line to Create (...) (in datawidget.h)

const std::string transferfactory::tfunction_names[] =
    {"Default",
    "Min/Max/Brightn/Contr",
    "Intensity Threshold",
    "Color Linear",
    "Labels",
    "Linear",
    "Spline",
    "" };

const std::string panefactory::pane_names[] =
    {"Pt_MPR",
    "VTK_MIP",
	"Pt_Curve",
    "" };

transferfactory transfer_manufactured::factory = transferfactory();

transferfactory::transferfactory ()
    {
    num_items = 0;

    while  (tfunction_names[num_items] != "")
        { num_items++; }
    }

transferfactory::~transferfactory()
    { }

Fl_Menu_Item * transferfactory::function_menu (Fl_Callback * cb) //! get menu 
    {
    Fl_Menu_Item * fmenu;

    fmenu = new Fl_Menu_Item [num_items+1];

    for (int m=0; m < num_items; m++)
        {
        init_fl_menu_item(fmenu[m]);

        fmenu[m].label(tfunction_names[m].c_str());
        fmenu[m].callback(cb);
        //fmenu[m].argument(m);
        fmenu[m].flags = FL_MENU_RADIO;
        }

    fmenu[num_items].label(NULL);

    return fmenu;
	}

const std::string transferfactory::tf_name(int n)
    {
    return tfunction_names[n];
    }