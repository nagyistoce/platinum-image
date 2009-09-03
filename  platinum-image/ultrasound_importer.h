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

#ifndef __usimport__
#define __usimport__


#include "image_scalar.h"
//#include "curve.hxx"
#include <string>
#include "pt_vector.h"
using namespace std;

class us_scan;


class ultrasound_importer{
protected:
	
public:
	ultrasound_importer(string);
	~ultrasound_importer(void);
	void read_file(string name);
	void read_old_file(string name);
	string set_current_scan(int i);
	int set_current_row(int i);
	pt_vector<unsigned short>* get_mean_vector_for_scan();
	
	vector<us_scan*> all_scans;
	int scan_index;
	int row_index;
	bool loaded;
	string name;
	
};

class us_scan{
public:

	us_scan(string name, int init_size, int lines);
	~us_scan();
	int id;
	string name;
	pts_vector<unsigned short> *mean_vector;
	vector<pts_vector<unsigned short>*> rows;

};

#endif	//__ultrasound_importer.h__