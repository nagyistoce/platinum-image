//////////////////////////////////////////////////////////////////////////
//
//  ptconfig $Revision:$
//
//  Platinum configuration file handling: "last_path", "view_port_setup", "load_this_image_on_startup"
//
//  $LastChangedBy: joel.kullberg $
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

#ifndef __ultra1dops__
#define __ultra1dops__
#include <vector>
#include "ptconfig.h"
#include "ptmath.h"
#include "curve.hxx"
#include "string.h"
#include "ultrasound_importer.h"
#include "datamanager.h"

#define CURVE_CONF_PATH "../PlatinumPrivateApps/HighResUS/curve_conf.inp"

using namespace std;

class ultra1dops{
    private:
//		ConfigFile cf;

	public:
		static bool is_max(vector<Vector3D> p, int i);
		static bool is_min(vector<Vector3D> p, int i);
		static int mark_point(curve_scalar<unsigned short> *curve, int from, int to);
		static int get_vally(curve_scalar<unsigned short> *curve, int x, int dir);
		static int count_peaks(vector<Vector3D> c, curve_scalar<unsigned short> *curve, Vector3D *peak);
		static vector<Vector3D> simplify_the_curve(curve_scalar<unsigned short> *curve);

		static void straighten_the_peaks(us_scan * scan, int intima, int adventitia);
		static void shift(vector<pts_vector<unsigned short>*> curve, pts_vector<int> *s);
		static void recalculate_mean_curve(us_scan * scan);
		static void fit_gaussian_curve_and_calculate(pts_vector<double> *curve;, int intima, int adventitia)

};

#endif __ultra1dops__