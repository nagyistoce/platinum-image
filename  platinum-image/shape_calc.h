//////////////////////////////////////////////////////////////////////////////////
//
//  shape_calc $Revision: 706 $
///
/// 
///
//  $LastChangedBy: joel.kullberg $

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
#ifndef __shape_calc__
#define __shape_calc__

#include "ptmath.h"
#include <vector>


class shape_calc{
public:
	shape_calc(){};
	~shape_calc(){};
	static int sgn(long a);

	static vector<Vector3D> calc_line_2d(Vector3D start, Vector3D stop);
	static vector<Vector3D> calc_cirlce_2d(Vector3D center, int radius);
	static vector<Vector3D> calc_gauss_2d(Vector3D sigma, Vector3D my, double amplitude);

	static vector<Vector3D> calc_line_3d(Vector3D start, Vector3D stop);
	static vector<Vector3D> calc_cirlce_3d(Vector3D center, Vector3D normal, float radius);
	static vector<Vector3D> calc_rect_3d(Vector3D c1, Vector3D c2, Vector3D c3, Vector3D c4);

};
#endif