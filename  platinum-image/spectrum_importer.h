//////////////////////////////////////////////////////////////////////////
//
//  header collecting mathematical functions used
//  throughout the platform
//
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

#ifndef __specimport__
#define __specimport__


//#include "image_scalar.h"
//#include "curve.hxx"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "pt_vector.h"
using namespace std;

class header;


class spectrum_importer{
protected:
	
public:
	spectrum_importer(string);
	~spectrum_importer(void);
	bool read_header(string name);
	void read_data(string name);
	inline bool my_isnan(double x){ return x != x;};
	
	bool loaded;
	string name;
	header *head;
	vector<ptc_vector<float>*> curves;
};

class keywords{
public:
	keywords(string k, double v);

	string key;
	double val;
};

class header{
public:

	header();
	~header();
	void add_info(string s);

	template<class T> T string_to_val(string s){
		istringstream buffer(s);
		T some_val;
		buffer >> some_val;
		return some_val;
	};

	double get_value(string key);
	
	vector<keywords> keys;
	int start_to_look;

};

#endif	//__spectrum_importer.h__