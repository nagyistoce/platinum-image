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
extern datamanager datamanagement;


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

void ultra1dops::scale_peaks(curve_scalar<unsigned short> *curve, Vector3D *peaks){
	double max_dist = (pt_config::read<double>("max_dist_between_peaks",CURVE_CONF_PATH)/curve->get_scale())/2;
	
	int l_b = std::max(0,(int)(peaks[1][0] - max_dist));
	int r_b = std::min(curve->get_data_size()-1, (int)(peaks[0][0] + max_dist));
	int c_b = (l_b + r_b)/2;

	//adventitia peak
	double min = 70000, max = 0;
	for(int i = l_b; i <= c_b; i++){
		max = std::max(curve->get_data(i), max);
		min = std::min(curve->get_data(i), min);
	}
	for(int i = l_b; i <= c_b; i++){
		curve->my_data->at(i) = ((curve->my_data->at(i) - min)/(max-min))*numeric_limits<unsigned short>::max();
	}

	//intima peak
	min = 70000;
	max = 0;
	for(int i = c_b; i <= r_b; i++){
		max = std::max(curve->get_data(i), max);
		min = std::min(curve->get_data(i), min);
	}
	for(int i = c_b; i <= r_b; i++){
		curve->my_data->at(i) = ((curve->my_data->at(i) - min)/(max-min))*numeric_limits<unsigned short>::max();
	}
}

void ultra1dops::calc_intensity_histogram(pt_vector<unsigned short> *curve){
	//int nr_buckets = 50;
	int nr_buckets = numeric_limits<unsigned short>::max()+1;
	pts_vector<unsigned short> *intensity_distr = new pts_vector<unsigned short>(nr_buckets);

	int b_pos;
	intensity_distr->assign(nr_buckets,0);
	intensity_distr->x_res = curve->x_res;
	intensity_distr->x_axis_start = curve->x_axis_start;
	//double span = (numeric_limits<unsigned short>::max()+1) / nr_buckets;

	for(int i = 0; i < curve->size(); i++){
		//b_pos = floor(curve->at(i)/span);
		b_pos = curve->at(i);
		intensity_distr->at(b_pos)++;
	}
	for(int i = intensity_distr->size()-2; i >=0; i--){
		intensity_distr->at(i)+= intensity_distr->at(i+1);
	}
	for(int i = 0; i < curve->size(); i++){
		curve->at(i) /= intensity_distr->at(curve->at(i));
		//curve->at(i) /= intensity_distr->at(floor(curve->at(i)/span));
	}
	curve_scalar<unsigned short> *s = new curve_scalar<unsigned short>(0,"intensity",0,0.04); //Icke 0.04 i alla fall
	s->my_data = intensity_distr;
	datamanagement.add(s);
}

int ultra1dops::mark_point(curve_scalar<unsigned short> *curve, int from, int to){
	//from needs to be lower than to!!!
	int x, dy;
	int y1, y2;
	x = from;
	dy = 0;
	if(from == -1 || to == -1){
		return -1;
	}

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
		if(x + i*dir >= curve->get_data_size())
			return -1;
		if(min[1] > curve->get_data(x + i*dir)){
			min[0] = x + i*dir;
			min[1] = curve->get_data(x + i*dir);
		}
	}
	return min[0];
}

/*Checks so that there are no higer peaks in half left of the adventita*/
bool ultra1dops::highest_in_interval(vector<Vector3D> c, int start, int stop, unsigned short height){
	int index = c.size()-1;
	while(index >= 0 && c.at(index)[0] > start){
		if(c.at(index)[0] < stop){
			if(c.at(index)[1] > height)
				return false;

		}
		index--;
	}
	return true;
}

int ultra1dops::count_peaks(vector<Vector3D> c, curve_scalar<unsigned short> *curve, Vector3D *peak){
	int peaks = 0;
	Vector3D bottom;
	bottom[2] =0;
	int min_thresh = pt_config::read<int>("min_val_for_peak",CURVE_CONF_PATH);
	//TODO come up with a smart way to set dynamic threshold
	int sum = 0;
	int m_count = 0;
	for(int i = 0; i < c.size(); i++){
		if(is_max(c,i)){
			sum+=c[i][1];
			m_count++;
		}
	}
	min_thresh = (sum/m_count)/2;
	//cout << "threshold: " << min_thresh << endl;
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
			if(peaks == 1 && peak[0][1] < c[i][1]/3){ //Added to prevent bump before intima to become peak
				peaks--;
			}
			if(peaks < 2)
				peak[peaks] = c[i];
			peaks++;
		}
	}
	return -1;
}

vector<Vector3D> ultra1dops::simplify_the_curve(curve_scalar<unsigned short> *curve){
	vector<Vector3D> p;
	int step = pt_config::read<double>("simplCurveInterval",CURVE_CONF_PATH)/curve->get_scale(); //TODO detta lases fran bin...
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
	int max_c, min_c;
	double min_m, max_m;
	min_m = max_m = 0;
	min_c = max_c = 0;
	for(int t = 1; t < p.size()-1; t++){
		if(is_max(p,t)){
			max_m+=p.at(t)[1];
			max_c++;
		}
		else if(is_min(p,t)){
			min_m+=p.at(t)[1];
			min_c++;
		}
	}
	min_m = min_m / min_c;
	max_m = max_m / max_c;

	int mean = (max_m+min_m)/2;
	vector<Vector3D> keep;
	keep.push_back(p[0]);
	for(int a = 1; a < p.size()-1; a++){
		if(is_max(p,a)){
			if(p[a][1] < mean){
				p[a][1] = 0; //Fix this boundary!
			}
			keep.push_back(p[a]);
		}else if(is_min(p,a)){//Add all minimas as zero;
			p[a][1] = 0;
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
void ultra1dops::recalculate_weighted_mean_curve(us_scan * scan, vector<int> weight){
	scan->mean_vector->assign(scan->rows.at(0)->size(),0);

	for(int i = 0; i< scan->rows.size(); i ++){
		for(int j = 0; j<scan->rows.at(i)->size(); j++){
			scan->mean_vector->at(j) = scan->mean_vector->at(j) + ((scan->rows.at(i)->at(j) - scan->mean_vector->at(j))/(i+1))*weight.at(i);
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
int ultra1dops::straighten_the_peaks(us_scan * scan, int intima, int adventitia){
	pts_vector<int> *s = new pts_vector<int>(0);
	pts_vector<unsigned short> *mean = scan->mean_vector;

	s->assign(scan->rows.size(),0);
	int search_area = pt_config::read<double>("scope_for_maximum_diff",CURVE_CONF_PATH)/scan->rows.at(0)->x_res;
	int min_x, min_diff;
	int x_start , x_stop;
	int width, diff;

	x_start = adventitia - search_area*2;
	x_stop = intima + search_area*2;

	width = (intima + search_area)-(adventitia - search_area);
	if(x_start < 0 || x_stop >= scan->rows.at(0)->size()){
		return -1;
	}
	for(int i = 0; i < scan->rows.size(); i++){
		min_x = -1;
		min_diff = std::numeric_limits<int>::max();
		for(int x = x_start; x < x_start+search_area*2; x++){
			diff = 0;
			for(int a = 0; a < width; a++){
				diff +=  abs(mean->at(x_start+search_area+a) - scan->rows.at(i)->at(x+a));
			}
			if(diff < min_diff){
				min_diff = diff;
				min_x = x-(x_start+search_area);
			}
		}
		s->at(i) = min_x;
	}
	curve_scalar<int> *s_c = new curve_scalar<int>(0,"shift",0,0.04);
	shift(scan->rows, s);
	s_c->my_data = s;
	datamanagement.add(s_c);

	int mean_int = 0;
	vector<int> weight;
	weight.assign(scan->rows.size(),0);
	for(int i = (adventitia - search_area); i < (adventitia - search_area)+width; i++){
		mean_int+=mean->at(i);
	}
	for(int i = 0; i < scan->rows.size(); i++){
		for(int b = (adventitia - search_area); b < (adventitia - search_area)+width; b++){
			weight.at(i)+=scan->rows.at(i)->at(b);
		}
		if(weight.at(i)< 0.5*mean_int)
			weight.at(i) = 0;
		else
			weight.at(i) = 1;
	}
	recalculate_weighted_mean_curve(scan,weight);
	return 1;
}


int ultra1dops::straighten_the_peaks2(us_scan * scan, int intima, int adventitia){
	double mass, pos;//, cog;
	pts_vector<double> *s = new pts_vector<double>(0);
	int search_area = pt_config::read<double>("scope_for_maximum_diff",CURVE_CONF_PATH)/scan->rows.at(0)->x_res;
	vector<float> area;
	float a;
	for(int i = 0; i< scan->rows.size(); i++){
		a = 0;
		int count = 0;
		for(int j = adventitia -5; j < intima + 5; j++){
			if(j < scan->rows.at(i)->size() && j >=0){
				a+= scan->rows.at(i)->at(j)/10;
			}else{
				return -1;
			}
		}
		area.push_back(a);
	}

	for(int i = 0; i < scan->rows.size(); i++){
		mass = pos = 0;
		int temp_ = scan->rows.at(i)->size()-1 - intima;
		search_area = std::min(temp_, search_area);
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
		s_m->push_back(round(dx/A));
	}
	s_m->insert(s_m->begin(), s_m->at(0));
	s_m->insert(s_m->begin(), s_m->at(0));
	s_m->push_back(round(s->at(s->size()-2)));
	s_m->push_back(round(s->at(s->size()-1)));

	shift(scan->rows, s_m);
	recalculate_mean_curve(scan);
	return 1;
}


vector<gaussian> ultra1dops::fit_gaussian_curve_and_calculate(curve_scalar<unsigned short> *curve, int intima, int adventitia){
	
	int search_area = pt_config::read<double>("scope_for_vally_loc",CURVE_CONF_PATH)/curve->get_scale();
	gaussian in, adv;
	float i_l, i_m, m_a, s_a;
	Vector3D res;
	vector<gaussian> g;

	pts_vector<unsigned short> *v;
	if((v = dynamic_cast<pts_vector<unsigned short>*>(curve->my_data)) == NULL)
		return g;
	if(intima+search_area >= v->size() || intima-search_area < 0){
		Vector3D r;
		r[0] = r[1] = r[2] = -1;
		return g;
	}

	if(adventitia+search_area >= v->size() || adventitia-search_area < 0){
		Vector3D r;
		r[0] = r[1] = r[2] = -1;
		return g;
	}

	in = v->fit_gaussian_with_amoeba(intima-search_area, intima+search_area);
	//cout << in.sigma << "  " << v->from_x_to_val(search_area) << endl;
	if(in.sigma > v->from_x_to_val(search_area)){ //Detta kanske funkar så att anpassningen inte blir fel!
		return g;
	}
		//v->fit_gaussian_with_amoeba(amp, center, sigma, intima-search_area, intima+search_area);
	//v->fit_gaussian_to_intensity_range(amp,center,sigma,intima-search_area, intima+search_area, false);
	//Berakna tva matpunkter har

	adv = v->fit_gaussian_with_amoeba(adventitia-search_area, adventitia+search_area);
	if(adv.sigma > v->from_x_to_val(search_area)){ //Detta kanske funkar så att anpassningen inte blir fel!
		return g; //Ändra så de har samma coordinates kanske!!!!!!
	}

	//v->fit_gaussian_with_amoeba(amp2, center2, sigma2, adventitia-search_area, adventitia+search_area);

	i_l = in.center + v->from_x_to_val(intima-search_area) + in.sigma; //everything in x_scale coords. NOT index coords!
	i_m = in.center + v->from_x_to_val(intima-search_area) - in.sigma;//everything in x_scale coords
	m_a = adv.center + v->from_x_to_val(adventitia-search_area) + adv.sigma;//everything in x_scale coords
	s_a = adv.center + v->from_x_to_val(adventitia-search_area) - adv.sigma;//everything in x_scale coords

	/*check that it is normal*/
	if(i_m - m_a < 0.1)//media should always be bigger than 0.1 mm
		return g;

	double sig1, sig2;
	sig1 = (in.sigma - v->x_axis_start)/v->x_res; //convert to unrounded index coordinates
	sig2 = (adv.sigma - v->x_axis_start)/v->x_res; //convert to unrounded index coordinates

	res[0] = i_l - i_m;//intima
	res[1] = i_m - m_a;//media
	res[2] = i_l - s_a; //Total wall thickness

//	cout << "gaussian way" << endl;
//	cout << "intima: " << i_l - i_m << endl;
//	cout << "media: "  << i_m - m_a << endl;
//	cout << "total: " << i_l-s_a << endl;

	curve->helper_data->add_gauss(v->from_val_to_x(in.center) + (intima-search_area), sig1, in.amplitude);
	curve->helper_data->add_gauss(v->from_val_to_x(adv.center) + (adventitia-search_area), sig2, adv.amplitude);
	//TODO det ska inte vara andrat sigma har!!!!!
	in.center+=v->from_x_to_val(intima-search_area);
	adv.center+=v->from_x_to_val(adventitia-search_area);
	g.push_back(in);
	g.push_back(adv);

	return g;
}
/*int ultra1dops::temp_peak(curve_scalar<unsigned short> *curve, int start, int dir, int max){
		max = max/2;
		cout << "max: " << max;
		for(int i = 1; i < 10; i++){
			if(curve->get_data(start + (i*dir)) < max){
				
				return (i*dir)+ start;
			}
		}
		return -1;
}*/

Vector3D ultra1dops::find_steep_slope_and_calculate(curve_scalar<unsigned short> *curve, int intima, int adventitia){
	//curve->save_curve_to_file("./" + curve->name() + ".ptc");
	float i_l = mark_point(curve, intima, get_vally(curve, intima, 1)); //intima end
	float i_m = mark_point(curve, get_vally(curve, intima, -1), intima); //intima begin
	float m_a = mark_point(curve, adventitia, get_vally(curve, adventitia, 1)); //media begin
	float s_a = mark_point(curve, get_vally(curve, adventitia, -1), adventitia); //adventitia begin

	//i_l = (i_l+temp_peak(curve, intima, 1, curve->get_data(intima)))/2;
	//i_m = (i_m+temp_peak(curve, intima, -1, curve->get_data(intima)))/2;
	//m_a = (m_a + temp_peak(curve, adventitia, 1, curve->get_data(adventitia)))/2;
	//s_a = (s_a + temp_peak(curve, adventitia, -1, curve->get_data(adventitia)))/2;

	if(i_l == -1 ||i_m == -1 ||m_a == -1 || s_a == -1){
		Vector3D r;
		r[0] = r[1] = r[2] = -1;
		return r;
	}

	Vector3D res;

	Vector3D s;
	Vector3D p;
	s[0] = i_l;
	s[1] = curve->get_data(i_l);
	s[2] = 0;
	p = s;
	p[1] = 0;
	//	curve->helper_data->add_line(s,p);

	p[0] = i_m;
	s[0] = i_m;
	s[1] = curve->get_data(i_m);
	//	curve->helper_data->add_line(s,p);

	p[0] = m_a;
	s[0] = m_a;
	s[1] = curve->get_data(m_a);
	//	curve->helper_data->add_line(s,p);

	res[0] = (i_l - i_m)*curve->get_scale();
	res[1] = (i_m - m_a)*curve->get_scale();
	res[2] = (i_l - s_a)*curve->get_scale();

	//cout << "manual way" << endl;
	//cout << "intima: " << (i_l - i_m)*curve->get_scale() << endl;
	//cout << "media: "  << (i_m - m_a)*curve->get_scale()<< endl;
	
	return res;
}