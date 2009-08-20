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

#include "ultra1dops.h"


bool ultra1dops::is_max(vector<Vector3D> p, int i){
	if(i > 0 && i < p.size()-1)
		return p[i][1] == max(p[i][1],max(p[i-1][1],p[i+1][1]));
	return true;
}
bool ultra1dops::is_min(vector<Vector3D> p, int i){
	if(i > 0 && i < p.size()-1)
		return p[i][1] == min(p[i][1],min(p[i-1][1],p[i+1][1]));
	return true;
}

int ultra1dops::mark_point(curve_scalar<unsigned short> *curve, int from, int to){
	//from needs to be lower than to!!!
	int x, dy;
	int y1, y2;
	x = from;
	dy = 0;

	y1 = curve->get_data(from);
	for(int i = from+1 ; i <= to; i++){
		y2 = curve->get_data(i);
		if(dy <= abs(y2-y1)){
			dy = abs(y2-y1);
			x = i; 
		}
		y1 = y2;
	}
	return x;
}

int ultra1dops::get_vally(curve_scalar<unsigned short> *curve, int x, int dir){
	Vector3D min;
	min[0] = x;
	min[1] = curve->get_data(x);
	min[2] = 0;

	int scope = pt_config::read<float>("scope_for_vally_loc",CURVE_CONF_PATH)/curve->get_scale();

	for(int i = 1; i < scope; i++){
		if(min[1] > curve->get_data(x + i*dir)){
			min[0] = x + i*dir;
			min[1] = curve->get_data(x + i*dir);
		}
	}
	return min[0];
}

int ultra1dops::count_peaks(vector<Vector3D> c, curve_scalar<unsigned short> *curve, Vector3D *peak){
	int peaks = 0;
	Vector3D bottom;
	bottom[2] =0;
	int min_thresh = pt_config::read<int>("min_val_for_peak",CURVE_CONF_PATH);
	double max_dist = pt_config::read<double>("max_dist_between_peaks",CURVE_CONF_PATH)/curve->get_scale();
	int start = 0;
	for(int i = c.size()-1; i>= 0; i --){
 		if(start - c[i][0] > max_dist){
				return peaks;
		}
		if(c[i][1] > min_thresh && is_max(c,i)){
			start = c[i][0];
			bottom = c[i];
			bottom[1] = 0;
			//curve->helper_data->add_line(c[i],bottom);
			if(peaks < 2)
				peak[peaks] = c[i];
			peaks++;
		}
	}
	return -1;
}

vector<Vector3D> ultra1dops::simplify_the_curve(curve_scalar<unsigned short> *curve){
	vector<Vector3D> p;
	int step = pt_config::read<double>("simplCurveInterval",CURVE_CONF_PATH)/curve->get_scale(); //TODO detta läses från bin...
	for(int x = 0; x < curve->get_data_size(); x= x+step){
		Vector3D m;
		m[0] = x;
		m[1] = curve->my_data->at(x);
		m[2] = 0;
		int stop = std::min(curve->get_data_size()-x, step);
		for(int a = x+1; a < x+stop; a++){
			if(m[1] < curve->my_data->at(a)){
				m[0] = a;
				m[1] = curve->my_data->at(a);
			}
		}
		p.push_back(m);
	}
	vector<Vector3D> keep;
	keep.push_back(p[0]);
	for(int a = 1; a < p.size()-1; a++){
		if(is_max(p,a) || is_min(p,a)){	
			keep.push_back(p[a]);
		}
		
	}
	keep.push_back(p.back());
	return keep;
}
void ultra1dops::recalculate_mean_curve(us_scan * scan){
	scan->mean_vector->assign(scan->rows.at(0)->size(),0);

	for(int i = 0; i< scan->rows.size(); i ++){
		for(int j = 0; j<scan->rows.at(i)->size(); j++){
			scan->mean_vector->at(j) = scan->mean_vector->at(j) + (scan->rows.at(i)->at(j) - scan->mean_vector->at(j))/(i+1);
		}
	}
}

void ultra1dops::shift(vector<pts_vector<unsigned short>*> curve, pts_vector<int> *s){
	for(int i = 0; i < s->size(); i++){
		if(s->at(i) > 0){
			curve.at(i)->erase(curve.at(i)->begin(), curve.at(i)->begin() + s->at(i));
			for(int j = 0; j < s->at(i); j++){
				curve.at(i)->push_back(0);
			}
		}else{
			for(int j = 0; j < abs(s->at(i)); j++){
				curve.at(i)->pop_back();
				curve.at(i)->insert(curve.at(i)->begin(),0);
			}
		}
	}
}

void ultra1dops::straighten_the_peaks(us_scan * scan, int intima, int adventitia){
	double mass, pos, cog;
	pts_vector<double> *s = new pts_vector<double>(0);
	int search_area = pt_config::read<double>("scope_for_maximum_diff",CURVE_CONF_PATH)/scan->rows.at(0)->x_res;
	vector<float> area;
	float a;
	for(int i = 0; i< scan->rows.size(); i++){
		a = 0;
		for(int j = adventitia -5; j < intima + 5; j++){
			a+= scan->rows.at(i)->at(j)/10;
		}
		area.push_back(a);
	}

	for(int i = 0; i < scan->rows.size(); i++){
		mass = pos = 0;
		for(int j = -search_area; j <= search_area; j++){
			mass += scan->rows.at(i)->at(intima+j);
			pos += scan->rows.at(i)->at(intima+j)*(intima+j);
		}
		s->push_back(pos/mass - intima);
	}
	s->mean_value_smoothing(1);
	pts_vector<int> *s_m = new pts_vector<int>(0);
	double weight, A, dx, c;
	for(int i = 2; i < s->size()-2; i++){
		A = dx = weight = c = 0;
		for(int j = -2; j<=2; j++){
			A += area.at(i);
			dx += s->at(i)*area.at(i);
		}
		//s_m->push_back(round(dx/A));
		s_m->push_back(round(dx/A));
	}
	s_m->insert(s_m->begin(), s_m->at(0));
	s_m->insert(s_m->begin(), s_m->at(0));
	s_m->push_back(round(s->at(s->size()-2)));
	s_m->push_back(round(s->at(s->size()-1)));

	cout << endl;
	curve_scalar<int> *diff = new curve_scalar<int>(0, "diff", 0, 1);
	extern datamanager datamanagement;
	datamanagement.add(diff);
	diff->my_data = s_m;

	shift(scan->rows, s_m);
	recalculate_mean_curve(scan);
}


void ultra1dops::fit_gaussian_curve_and_calculate(pts_vector<double> *curve;, int intima, int adventitia){
	int search_area = pt_config::read<double>("scope_for_maximum_diff",CURVE_CONF_PATH)/scan->rows.at(0)->x_res;
	
}