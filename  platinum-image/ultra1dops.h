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
//#include "histogram.h"
//#include "histogram.hxx"

//#define CURVE_CONF_PATH "../../Platinum/Apps/HighResUS/curve_conf.inp"
#define CURVE_CONF_PATH "../../src/Apps/HighResUS/curve_conf.inp"

using namespace std;

class ultra1dops{
    private:
//		ConfigFile cf;

	public:
		static bool is_max(vector<Vector3D> p, int i);
		static bool is_min(vector<Vector3D> p, int i);

		static void calc_intensity_histogram(pt_vector<unsigned short> *curve);
		static void scale_peaks(curve_scalar<unsigned short> *curve, Vector3D *peaks);

		static int mark_point(curve_scalar<unsigned short> *curve, int from, int to);
		static int get_vally(curve_scalar<unsigned short> *curve, int x, int dir);
		static bool highest_in_interval(vector<Vector3D> c, int start, int stop, unsigned short height);
		static int count_peaks(vector<Vector3D> c, curve_scalar<unsigned short> *curve, Vector3D *peak);
		static vector<Vector3D> simplify_the_curve(curve_scalar<unsigned short> *curve);

		static int straighten_the_peaks(us_scan * scan, int intima, int adventitia);
		static int straighten_the_peaks2(us_scan * scan, int intima, int adventitia);
		static void shift(vector<pts_vector<unsigned short>*> curve, pts_vector<int> *s);
		static void recalculate_mean_curve(us_scan * scan);
		static void recalculate_weighted_mean_curve(us_scan * scan, vector<int> weight);
		static vector<gaussian> fit_gaussian_curve_and_calculate(curve_scalar<unsigned short> *curve, int intima, int adventitia);

		static Vector3D find_steep_slope_and_calculate(curve_scalar<unsigned short> *curve, int intima, int adventitia);
		//static int temp_peak(curve_scalar<unsigned short> *curve, int start, int dir, int max);
};

#endif __ultra1dops__