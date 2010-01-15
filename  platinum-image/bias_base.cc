// $Id:$

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

#include "bias_base.h"

using namespace std;

/*
//bias_base::bias_base():data_base()
//    {
//		set_parameters();
//	}

bias_base::bias_base():data_base(get_supported_renderers())
    {
		cout<<"bias_base()-constructor"<<endl;
//		set_parameters();
	}
vector<RENDERER_TYPE> bias_base::get_supported_renderers(){
	vector<RENDERER_TYPE> vec;
	vec.push_back(RENDERER_MPR);
	vec.push_back(RENDERER_MIP);
	return vec;
}
*/
/*
bias_base::bias_base(bias_base* const b):data_base(b)
    {
//    set_parameters();
    //setting copy name at the root would be neat,
    //but is not possible since the widget isn't
    //created yet
    name("Copy of " + b->name());
    }
*/

/*

void bias_base::redraw()
{
//    rendermanagement.data_has_changed(ID);
}

void bias_base::data_has_changed(bool stats_refresh)
{
}

void bias_base::save_to_DCM_file(const std::string, const bool useCompression, const bool anonymize)
{
}

void bias_base::save_to_VTK_file(const std::string, const bool useCompression)
{
}
*/