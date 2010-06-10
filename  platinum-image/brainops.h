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

#ifndef __brainops__
#define __braindops__
#include <vector>
#include "ptconfig.h"
#include "ptmath.h"
#include "image_binary.hxx"
#include "string.h"
#include "datamanager.h"
#include "filters.h"

using namespace std;

class brainops{
    private:
		
	public:
		static void remove_bg(vector<image_scalar<unsigned short,3>* > brain);
		static void get_val_from_cube(image_integer<unsigned short,3> *T1, int x, int y, int z, float &mean, float &var);
		static image_binary<3>* fit_WM_cube_and_grow_accordingly(image_integer<unsigned short,3> *T1, int &thresh, float q1, float q2);
};

#endif __brainops__