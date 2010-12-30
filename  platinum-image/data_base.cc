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

//#include "data_base.h"

#include "fileutils.h"

#include <sstream>
#include "datawidget.h"

#include "datamanager.h"
extern datamanager datamanagement;

int data_base::data_next_ID = 1;

using namespace std;

data_base::~data_base()
    {
	//cout<<"~data_base()"<<endl;
    //destructor may be called from a menu choice in widget, it would crash if deleted with delete(...)
    //Fl::delete_widget is designed to avoid this
    if(widget != NULL){
		Fl::delete_widget (widget); 
	}
    datamanagement.remove_data (this);
    }

void data_base::refresh_thumbnail()
{
    if(widget != NULL){ //the widget may not have been created yet... (when the image is first loaded...)
		widget->refresh_thumbnail();
	}else{
		pt_error::pt_error("Attempt to set name(const string) on a widget-less data object",pt_error::warning);
    }
}


void data_base::name (const string n)
   {
    if (widget != NULL)
        { widget->name(n); }
    else
        {
		pt_error::pt_error ("Attempt to set name(const string) on a widget-less data object",pt_error::warning);
        }
    }

const string data_base::name() const
{
    if (widget != NULL)
	{
        return widget->name();	//returns a copy
	}
	
    return ( "(untitled)" );
}

const char* data_base::name_ptr()
{
    if (widget != NULL)
	{
        return widget->name_ptr();
	}
    return string("(widget==NULL)").c_str();
}

data_base::data_base(data_base *const source, vector<RENDERER_TYPE> supported)
    {
	ID = data_next_ID++;
//	cout<<"data_base, new ID="<<ID<<endl;
	my_tooltip = "";

	widget = NULL;
	draw_additional_data = false;

//    pt_error::error_if_null(source,"Attempting to copyconstruct data object from NULL object");

	if(source == NULL){
		from_file(false);
		helper_data = new additional_data();
		meta = metadata();
	}else{
		from_file(source->from_file());
		helper_data = source->helper_data;
		meta = source->meta;
	}

	if(supported.size()==0){
		supported_renderers.push_back(RENDERER_MPR); //Defaults to MPR, but for clarity this should not be used!
	}else{
		supported_renderers = supported;
	}

    }
/*
data_base::data_base(vector<RENDERER_TYPE> support)
    {
	supported_renderers = support;
    ID = data_next_ID++;
    widget = NULL;
    from_file(false);
	helper_data = new additional_data();
	draw_additional_data = false;
	meta = metadata();
    }

data_base::data_base()
    {
	supported_renderers.push_back(RENDERER_MPR); //Defaults to MPR, but for clarity this should not be used!
    ID = data_next_ID++;
    widget = NULL;
    from_file(false);
	helper_data = new additional_data();
	draw_additional_data = false;
	meta = metadata();
    }

data_base::data_base(data_base * const source, vector<RENDERER_TYPE> support)
    {
    pt_error::error_if_null(source,"Attempting to copyconstruct data object from NULL object");

	supported_renderers = support;
    ID = data_next_ID++;
    widget = NULL;
    from_file(source->from_file());
	helper_data = source->helper_data;
	draw_additional_data = false;
	meta = source->meta;
    }
*/
/*
data_base::data_base(const data_base & source)
{
	std::cout << "data_base --- copy constructor" << std::endl;

	ID = data_next_ID++;
	widget = NULL;
	from_file(source.from_file());
	meta = source.meta;
}

const data_base & data_base::operator=(const data_base & source)
{
	if ( this != &source )	// make sure not the same object
	{
		std::cout << "data:base --- operator=" << std::endl;

		ID = data_next_ID++;
		widget = NULL;
		from_file(source.from_file());
		meta = source.meta;
	}
	return *this;
}
*/

bool data_base::has_additionl_data()
{
	if(helper_data->data.size()>0){
		return true;
	}
	return false;
}

void data_base::data_has_changed (bool) 
{}
        
int data_base::get_id()
{ 
	return ID; 
}

void data_base::activate()
{
    //widget->show();
    widget->activate();
}
string data_base::resolve_value_world(Vector3D worldPos)
{
	return "data_base::resolve_value_world - not defined";
}


bool data_base::from_file() const
    {
    if (widget !=NULL)
        { return (widget->from_file()); }
    else
        { return false; }
    }

void data_base::from_file(bool f)
    {
    if (widget !=NULL)
        { widget->from_file(f); }

    else
        {
//        pt_error::error("Attempt to set from_file(bool) on a widget-less data object",pt_error::warning);
        }
    }

void data_base::name_from_path(string file_path)
    {
    name(path_end (file_path));
    
    //named from file, comes from file. Set status accordingly:
    from_file(true);
    }

void data_base::save_histogram_to_txt_file(const std::string filename, const std::string separator)
    {
        pt_error::pt_error ("Attempt to save_histogram_to_txt_file on a data_base object",pt_error::warning);
    }

void data_base::make_histogram_curve()
    {
        pt_error::pt_error ("Attempt to add_histogram on a data_base object",pt_error::warning);
    }


	
string data_base::resolve_tooltip()
{
	return resolve_tooltip_data_base();
}

string data_base::resolve_tooltip_data_base()
{
	return "name=\"" + widget->name() + "\" (id="+int2str(ID)+")\nfrom_file="+bool2str(from_file())+"\n";
}

void data_base::set_tooltip(string s)
{
	my_tooltip = s;
	widget->set_tooltip(my_tooltip.c_str());
}

vtkAlgorithmOutput* data_base::getvtkStructuredPoints()
{
	return NULL;
}
int data_base::read_helper_data_from_file(string file_path){
	helper_data->read_all_data_from_file(file_path);
	return 1;
}

int data_base::save_helper_data_to_file(string file_path){
	helper_data->write_all_data_to_file(file_path);
	return 1;
}
