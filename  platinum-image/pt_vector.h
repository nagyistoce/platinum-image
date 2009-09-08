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

#ifndef __ptvector__
#define __ptvector__

#include <vector>
#include <math.h>
#include <limits>
#include <iostream>

#include "ptmath.h"
#include "threshold.h"
#include "global.h"
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_amoeba.h>

using namespace std;

template <class ELEMTYPE>
class fit_gaussians_to_curve_cost_function;

template<class ELEMTYPE>
class fit_rayleighian_to_histogram_1D_cost_function;

/* Base starts here */
template<class ELEMTYPE>
class pt_vector : public vector<ELEMTYPE>{
protected:
	
public:
	pt_vector(int);
	virtual ~pt_vector(void){};
	void config_x_axis(double resolution, double start);
	
	double x_res, x_axis_start;

private:
	
	
};

/* BEGIN PT_VECTOR FUNCTIONS */
template<class ELEMTYPE>
pt_vector<ELEMTYPE>::pt_vector(int start_size) : vector<ELEMTYPE>(start_size){
	x_res = 1.0;
	x_axis_start = 0;
}

/* Sets the scaling of the x axis */
template<class ELEMTYPE>
void pt_vector<ELEMTYPE>::config_x_axis(double resolution, double start){
	
	x_res = resolution;
	x_axis_start = start;
}
/* END PT_VECTOR FUNCTIONS */

template<class ELEMTYPE>
class pts_vector : public pt_vector<ELEMTYPE>{
public: 
	pts_vector(int);
	~pts_vector(void){};

	//Moved from pt vector
	ELEMTYPE get_maximum_in_range(int from, int to, int &max_val_index_pos);
	ELEMTYPE get_minimum_in_range(int from, int to, int &min_val_index_pos);

	ELEMTYPE get_max_value_in_range(int from, int to);
	ELEMTYPE get_min_value_in_range(int from, int to);
	float get_variance_in_range(int from, int to);
	void mean_value_smoothing(int filter_size);
	double get_mean_in_range(int from, int to);
	double get_mean_x_in_range(int from, int to);
	double get_slope_at_location(int location);
	double distance_between_points(int x1, int x2);
	double get_mean_in_range_for_vector(int from, int to, vector<ELEMTYPE> vec);
	double area_between_points(int x1, int x2);
	double from_x_to_val(int x);
	int from_val_to_x(double val);
	void normalize_to_maximum();

	ELEMTYPE get_x_at_lower_percentile(float percentile, bool ignore_zero_intensity);

//------ Fitting of gaussian functions ------
	void fit_gaussian_to_intensity_range(float &amp, float &center, float &sigma, double from, double to, bool print_info=false);

	float find_better_amplitude(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
	float find_better_center(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
	float find_better_sigma(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
	double get_sum_square_diff_between_buckets(vector<gaussian> v, int from_bucket, int to_bucket, bool ignore_zeros=true);
	double get_sum_square_diff_between_buckets(gaussian g, int from_bucket, int to_bucket, bool ignore_zeros=true);
	double get_sum_square_diff(vector<gaussian> v, bool ignore_zeros=true);
	double get_sum_square_diff(gaussian g, bool ignore_zeros=true);
	double get_gaussian_area(gaussian g, int from_bucket, int to_bucket);
	double get_gaussian_area(gaussian g);
	vector<double> get_gaussian_areas(vector<gaussian> v);
	double get_sum_square_gaussian_overlap(vector<gaussian> v, int from_bucket, int to_bucket);
	double get_sum_square_gaussian_overlap(vector<gaussian> v);
	vector<double> get_overlaps_in_percent(vector<gaussian> v);
	vnl_vector<double> get_vnl_vector_with_start_guess_of_num_gaussians(int num_gaussians);
	ELEMTYPE fit_two_gaussians_to_histogram_and_return_threshold(string save_histogram_file_path = "");

	gaussian fit_gaussian_with_amoeba(int from, int to);


	//------ Fitting of rayleigh functions ------
	void fit_rayleigh_distr_to_intensity_range(float &amp, float &M, ELEMTYPE from_int, ELEMTYPE to_int, bool print_info=false);
	double get_sum_square_diff(rayleighian r, bool ignore_zeros=true);
	double get_sum_square_diff_from_buckets(rayleighian r, int from_bucket, int to_bucket, bool ignore_zeros=true);

};
template<class ELEMTYPE>
pts_vector<ELEMTYPE>::pts_vector(int start_size) : pt_vector<ELEMTYPE>(start_size){
	
}
/* Looks in a specified range and returns the maximum value found */
template<class ELEMTYPE>
ELEMTYPE pts_vector<ELEMTYPE>::get_maximum_in_range(int from, int to, int &max_val_index_pos){
	ELEMTYPE maximum = numeric_limits<ELEMTYPE>::min();
	if(this->size() < from)
		return NULL;
	maximum = this->at(from);
	max_val_index_pos = from;
	for(int i = from; i<=to && i<this->size(); i++){
		if(this->at(i) > maximum){
			maximum = this->at(i);
			max_val_index_pos = i;
		}
	}
	return maximum;
}

/* Looks in a specified range and returns the minimum value found */
template<class ELEMTYPE>
ELEMTYPE pts_vector<ELEMTYPE>::get_minimum_in_range(int from, int to, int &min_val_index_pos){
	
	ELEMTYPE minimum = numeric_limits<ELEMTYPE>::max();
	if(this->size() < from)
		return NULL;

	minimum = this->at(from);
	min_val_index_pos = from;
	for(int i = from; i<=to && i<this->size(); i++){
		if(this->at(i) < minimum){
			minimum = this->at(i);
			min_val_index_pos = i;
		}
	}

	return minimum;
}

template <class ELEMTYPE>
ELEMTYPE pts_vector<ELEMTYPE>::get_max_value_in_range(int from, int to)
{
	ELEMTYPE max_value = std::numeric_limits<ELEMTYPE>::min();

	for(int i=from; i<=to; i++){
		if(this->at(i)>max_value){
			max_value = this->at(i);
		}
	}

	return max_value;
}

template <class ELEMTYPE>
ELEMTYPE pts_vector<ELEMTYPE>::get_min_value_in_range(int from, int to)
{
	ELEMTYPE min_value = std::numeric_limits<ELEMTYPE>::max();

	for(int i=from; i<=to; i++){
		if(this->at(i)>min_value){
			min_value = this->at(i);
		}
	}

	return min_value;
}

/* Returns the slope of the curve at a specified location */
template<class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_slope_at_location(int location){
	
	int start;

	if(this->size() < 2)
		return 0;

	start = location -1;
	if(start < 0){
		start++;
		location++;
	}
	
	return this->at(location) - this->at(start);
	//Safest to return double because of unknown datatype in calculation
}

/* Returns the slope of the curve at a specified location */
template<class ELEMTYPE>
void pts_vector<ELEMTYPE>::mean_value_smoothing(int filter_size){
	
	vector<ELEMTYPE> queue;
	typename vector<ELEMTYPE>::iterator first_element;
	int pos;
	pos = 0;
	if(this->size() < filter_size){
		return;
	}
	int i;
	for(i = 0; i< filter_size; i++){
		queue.push_back(this->at(0));
	}
	for(i = 0; i<= filter_size; i++){
		queue.push_back(this->at(i));
	}
	first_element = queue.begin();

	while(pos < this->size()){
		this->at(pos) = get_mean_in_range_for_vector(0, queue.size(), queue);
		
		queue.erase(first_element);
		first_element = queue.begin();

		pos++;
		if(pos+filter_size < this->size()){
			queue.push_back(this->at(pos+filter_size));
		}else{
			queue.push_back(this->at(this->size()-1));
		}
		
	}

}

template<class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_mean_x_in_range(int from, int to){
	
	double mean;

	mean = 0;
	for(double i = from; i < to; i++)
		mean +=(from_x_to_val(i)/(to-from+1));

	return mean;
}

template<class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_mean_in_range(int from, int to){
	
	double mean;

	mean = 0;
	for(int i = from; i < to; i++)
		mean +=(this->at(i)/(to-from+1));

	return mean;
}
template<class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_mean_in_range_for_vector(int from, int to, vector<ELEMTYPE> vec){
	
	double mean;

	mean = 0;
	for(int i = from; i < to; i++)
		mean +=(vec.at(i)/(to-from+1));

	return mean;
}

template<class ELEMTYPE>
double pts_vector<ELEMTYPE>::distance_between_points(int x1, int x2){
	
	return abs(x1-x2)*x_res;
}

template<class ELEMTYPE>
double pts_vector<ELEMTYPE>::area_between_points(int x1, int x2){
	double area_approx;
	area_approx = 0;
	for(int i = x1; i < x2; i++){
		area_approx += abs(this->at(i))*(x_res);//h*b
	}
	return area_approx; //same unit as x_res is given in
}

/* This does not work yet... */
template<class ELEMTYPE>
void pts_vector<ELEMTYPE>::normalize_to_maximum(){
	int dummy;
	ELEMTYPE min = get_minimum_in_range(0,this->size()-1,dummy);
	ELEMTYPE max = get_maximum_in_range(0,this->size()-1,dummy);

	ELEMTYPE new_max = numeric_limits<ELEMTYPE>::max();
	ELEMTYPE new_min = numeric_limits<ELEMTYPE>::min();

	long double dy = max-min;
	ELEMTYPE dy_new = new_max-new_min;
	ELEMTYPE shift = new_min - min;
	cout << "innan: " << this->at(23);
	for(int i = 0; i < this->size(); i++){
		this->at(i) = (ELEMTYPE) floor( (this->at(i)+shift)*(dy_new/dy) );
	}
	cout << "   efter" << this->at(23) << endl;
}

template<class ELEMTYPE>
int pts_vector<ELEMTYPE>::from_val_to_x(double val){
	
	return round((val - x_axis_start)/x_res);
}

template<class ELEMTYPE>
double pts_vector<ELEMTYPE>::from_x_to_val(int x){
	
	return x*x_res + x_axis_start;
}

template <class ELEMTYPE>
float pts_vector<ELEMTYPE>::get_variance_in_range(int from, int to)
{
	float mean = get_mean_x_in_range(from,to); //TITTA pÂ mean
	float sum = 0;
	float num_values = 0;
	float diff=0;
	for(int i=from; i<=to; i++){
		diff = mean-from_x_to_val(i);
		sum += float(this->at(i))*diff*diff; //this number of pixels with this (mean_intensity_diff)^2...
		num_values += float(this->at(i));
	}
	return sum/num_values;
}


template <class ELEMTYPE>
ELEMTYPE pts_vector<ELEMTYPE>::get_x_at_lower_percentile(float percentile, bool ignore_zero_intensity)
{
//    cout<<"get_bucket_at_histogram_lower_percentile("<<percentile<<")"<<endl;
	//if histogram comes from masked region... following line wont work....
	//float num_elem_limit = float(this->images[0]->get_num_elements())*percentile;

	double total_value = 0;
	for(int i = 0; i< this->size(); i++){
		total_value +=  this->at(i);
	}

	unsigned short the_zero_bucket = from_val_to_x(0);

	double num_elem_limit=0;
	if( ignore_zero_intensity && (the_zero_bucket>=0) && (the_zero_bucket<this->size()) ){
		num_elem_limit = double(total_value - this->at(the_zero_bucket) )*percentile;
	}else{
		num_elem_limit = double(total_value*percentile);
	}

	double sum_elements=0;
	if(ignore_zero_intensity){
		for(unsigned short i=0; i<this->size(); i++){
			if(i!=the_zero_bucket){
				sum_elements += this->at(i);
				if(sum_elements>=num_elem_limit){
					return i;
				}
			}
		}
	}else{
		for(unsigned short i=0; i<this->size(); i++){
			sum_elements += this->at(i);
			if(sum_elements>=num_elem_limit){
				return i;
			}
		}
	}

	return this->size()-1;
}

/* Base ends here */

/* Scalar starts here */
template <class ELEMTYPE>
void pts_vector<ELEMTYPE>::fit_gaussian_to_intensity_range(float &amp, float &center, float &sigma, double from_int, double to_int, bool print_info)
{
//	cout<<"fit_gaussian_to_intensity_range..."<<endl;
//	cout<<"this->max()="<<this->max()<<endl;
//	cout<<"this->min()="<<this->min()<<endl;
//	cout<<"this->size()="<<this->size()<<endl;
	cout<<"from_int="<<from_int<<endl;
	cout<<"to_int="<<to_int<<endl;
	cout<<"this->get_max_value_in_range(0,3)="<<this->get_max_value_in_range(0,3)<<endl;

	int from_bucket = std::max(0, this->from_val_to_x(from_int));
	int to_bucket = std::min(int(this->size()-1), this->from_val_to_x(to_int));
//	cout<<"from_bucket="<<from_bucket<<endl;
//	cout<<"to_bucket="<<to_bucket<<endl;

	gaussian g(amp,center,sigma);
	g.amplitude = float(this->get_max_value_in_range(from_bucket,to_bucket));
	g.center = this->get_mean_x_in_range(from_bucket,to_bucket); //detta ‰r inte sÂ logiskt fˆr kurvor.....
//	g.center = this->get_max_value_in_range(from_bucket,to_bucket);
	g.sigma = sqrt(this->get_variance_in_range(from_bucket,to_bucket)); //intensity variance...
	int dyn_from_bucket = std::max(from_bucket, this->from_val_to_x(g.center-1.5*g.sigma));
	int dyn_to_bucket = std::min(to_bucket, this->from_val_to_x(g.center+1.5*g.sigma));

//	cout<<"***INIT***"<<endl;
//	cout<<"amp="<<g.amplitude<<endl;
//	cout<<"center="<<g.center<<endl;
//	cout<<"sigma="<<g.sigma<<endl;

	if(print_info)
	{
	cout<<"**************"<<endl;
	cout<<"**************"<<endl;
	cout<<"from_bucket="<<from_bucket<<endl;
	cout<<"to_bucket="<<to_bucket<<endl;
	cout<<"amp="<<g.amplitude<<endl;
	cout<<"center="<<g.center<<endl;
	cout<<"sigma="<<g.sigma<<endl;
	cout<<"dyn_from_bucket="<<dyn_from_bucket<<endl;
	cout<<"dyn_to_bucket="<<dyn_to_bucket<<endl;
	}

	float tmp;
	int nr_iterations=300;
	for(int i=0;i<nr_iterations;i++){
		g.amplitude = find_better_amplitude(g,dyn_from_bucket,dyn_to_bucket,0.8,1.2,10+i);
		g.center = find_better_center(g,dyn_from_bucket,dyn_to_bucket,0.8,1.2,10+i);
		g.sigma = find_better_sigma(g,dyn_from_bucket,dyn_to_bucket,0.8,1.2,10+i);
		
		//consider shape of gaussian but dont go outside given limits...
		tmp = std::max(float(from_int),float(g.center-1.1*g.sigma)); //limits intensity range...
		dyn_from_bucket = std::max(from_bucket, this->from_val_to_x(tmp)); //limits bucket range...
		dyn_to_bucket = std::min(to_bucket, this->from_val_to_x(g.center+1.1*g.sigma));

		if(print_info){
			cout<<"**************"<<endl;
			cout<<"iter="<<i<<"/"<<nr_iterations<<endl;
			cout<<"g.amplitude="<<g.amplitude<<endl;
			cout<<"g.center="<<g.center<<endl;
			cout<<"g.sigma="<<g.sigma<<endl;
			cout<<"tmp="<<tmp<<endl;
			cout<<"dyn_from_bucket="<<dyn_from_bucket<<endl;
			cout<<"dyn_to_bucket="<<dyn_to_bucket<<endl;
		}
	}

	//set the reference variables again...
	amp = g.amplitude;
	center = g.center;
	sigma = g.sigma;
}



template <class ELEMTYPE>
float pts_vector<ELEMTYPE>::find_better_amplitude(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	float start = factor1*g.amplitude;
	float end = factor2*g.amplitude;
	float step = (end-start)/float(nr_steps);
	float best = 0;
	double error_sum = 0;
	double error_min = 100000000000000000.0;

	for(g.amplitude=start; g.amplitude<=end; g.amplitude+=step){
		error_sum = get_sum_square_diff_between_buckets(g, from_bucket, to_bucket);
		if(error_sum<error_min){
			error_min = error_sum;
			best = g.amplitude;
		}
	}
	return best;
}

template <class ELEMTYPE>
float pts_vector<ELEMTYPE>::find_better_center(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	float start = factor1*g.center;
	float end = factor2*g.center;
	if(g.center==0){
		start = this->from_x_to_val(from_bucket);
		end = this->from_x_to_val(to_bucket);
	}
	float step = (end-start)/float(nr_steps);
	float best = 0;
	double error_sum = 0;
	double error_min = 100000000000000000.0;

	for(g.center=start; g.center<=end; g.center+=step){
		error_sum = get_sum_square_diff_between_buckets(g, from_bucket, to_bucket);
		if(error_sum<error_min){
			error_min = error_sum;
			best = g.center;
		}
	}
	return best;
}

template <class ELEMTYPE>
float pts_vector<ELEMTYPE>::find_better_sigma(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	float start = factor1*g.sigma;
	float end = factor2*g.sigma;
	float step = (end-start)/float(nr_steps);
	float best = 0;
	double error_sum = 0;
	double error_min = 100000000000000000.0;

	for(g.sigma=start; g.sigma<=end; g.sigma+=step){
		error_sum = get_sum_square_diff_between_buckets(g, from_bucket, to_bucket);
		if(error_sum<error_min){
			error_min = error_sum;
			best = g.sigma;
		}
	}
	return best;
}

template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_sum_square_diff_between_buckets(vector<gaussian> v, int from_bucket, int to_bucket, bool ignore_zeros){
	double error = 0;
	float val=0;
	int zero_intensity_bucket = this->from_val_to_x(0);

	for(int j=from_bucket;j<=to_bucket;j++){
		//it is very important to use the intensity, and not the bucket position...

		if(this->at(j)>0 || !ignore_zeros){
			if(j != zero_intensity_bucket){ //dont include the commonly seen peak at zero intensity

				val=0;
				double intensity = this->from_x_to_val(j);
				for(int i=0;i<v.size();i++){
					val += v[i].evaluate_at(intensity);	
				}

				if(j<0){
					error += pow(val - 0, 2);
				}else{
					error += pow(val - float(this->at(j)), 2);
				}
			}

		}
	}
	return error;
}


template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_sum_square_diff_between_buckets(gaussian g, int from_bucket, int to_bucket, bool ignore_zeros){
	vector<gaussian> v;
	v.push_back(g);
	return get_sum_square_diff_between_buckets(v,from_bucket,to_bucket);
}

template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_sum_square_diff(vector<gaussian> v, bool ignore_zeros){
	return get_sum_square_diff_between_buckets(v,0,this->size()-1, ignore_zeros);
}

template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_sum_square_diff(gaussian g, bool ignore_zeros){
	vector<gaussian> v;
	v.push_back(g);
	return get_sum_square_diff_between_buckets(v,0,this->size()-1,ignore_zeros);
}


template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_gaussian_area(gaussian g, int from_bucket, int to_bucket)
{
	double area=0;
	for(int i=from_bucket;i<=to_bucket;i++){
		area += g.evaluate_at(this->from_x_to_val(i));
	}
	return area;
}

template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_gaussian_area(gaussian g)
{
	return get_gaussian_area(g,0,this->size()-1);
}

template <class ELEMTYPE>
vector<double> pts_vector<ELEMTYPE>::get_gaussian_areas(vector<gaussian> v)
{
	vector<double> v2;
	for(int i=0;i<v.size();i++){
		v2.push_back(this->get_gaussian_area(v[i]));
	}
	return v2;
}

template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_sum_square_gaussian_overlap(vector<gaussian> v, int from_bucket, int to_bucket)
{
	double sum=0;
	double min_val=10000;
	for(int i=from_bucket;i<=to_bucket;i++){
		for(int j=0;j<v.size();j++){
			min_val = std::min( min_val, double(v[j].evaluate_at(this->from_x_to_val(i))) );
		}
		sum += min_val*min_val;
		min_val=10000;
	}
	return sum;
}

template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_sum_square_gaussian_overlap(vector<gaussian> v)
{
	return get_sum_square_gaussian_overlap(v,0,this->size()-1);
}

template <class ELEMTYPE>
vector<double> pts_vector<ELEMTYPE>::get_overlaps_in_percent(vector<gaussian> v)
{
	vector<double> areas = this->get_gaussian_areas(v);
	vector<double> overlap_percent;

	//for each g, for each bucket, max_of_others, min(my_height,max_of_others);
	double my_val;
	double moa_val; //max_of_others
	double sum_overlap;

	for(int j=0;j<v.size();j++){
		sum_overlap=0;
		for(int i=0;i<this->size();i++){
			my_val = v[j].evaluate_at(this->from_x_to_val(i));
			moa_val=0;
			for(int k=0;k<v.size();k++){
				//cout<<"j,i,k-"<<j<<","<<i<<","<<k<<endl;
				if(k != j){
					moa_val = std::max( moa_val, double(v[k].evaluate_at(this->from_x_to_val(i))) );
				}
			}

			sum_overlap += std::min(my_val,moa_val);
		}

		overlap_percent.push_back( sum_overlap/areas[j] );
//		cout<<"overlap_percent="<<overlap_percent[j]<<endl;
	}

	return overlap_percent;
}




template <class ELEMTYPE>
vnl_vector<double> pts_vector<ELEMTYPE>::get_vnl_vector_with_start_guess_of_num_gaussians(int num_gaussians){
	vnl_vector<double> x(3*num_gaussians);

	int from_bucket=1;
	int to_bucket=0;
	for(int i=0;i<num_gaussians;i++){
		to_bucket = this->get_x_at_lower_percentile( float(i+1)/float(num_gaussians), true);
		//cout<<from_bucket<<" -->"<<to_bucket<<endl;
		//height
		//center (intensity)
		//SD (int)
		x[i*3+0] = this->get_max_value_in_range(from_bucket,to_bucket);
		x[i*3+1] = from_x_to_val(this->get_x_at_lower_percentile( float(i+0.5)/float(num_gaussians), true ));
		x[i*3+2] = sqrt(this->get_variance_in_range(from_bucket,to_bucket));

		from_bucket = to_bucket;
	}
	return x;
}


template <class ELEMTYPE>
gaussian pts_vector<ELEMTYPE>::fit_gaussian_with_amoeba(int from, int to)
{
	pts_vector<ELEMTYPE> *temp = new pts_vector<ELEMTYPE>(0);
	temp->config_x_axis(this->x_res, this->x_axis_start);
	for(int i = from; i <= to; i++){
		temp->push_back(this->at(i));
	}

	fit_gaussians_to_curve_cost_function<ELEMTYPE> cost(temp,1, false, false);
	vnl_amoeba amoeba_optimizer = vnl_amoeba(cost);
	amoeba_optimizer.verbose = false;
	amoeba_optimizer.set_x_tolerance(1);
//	amoeba_optimizer.set_relative_diameter(0.10);
//	amoeba_optimizer.set_max_iterations(10);
//	amoeba_optimizer.set_f_tolerance(1);

	vnl_vector<double> x = temp->get_vnl_vector_with_start_guess_of_num_gaussians(1);
	//cout<<"x="<<x<<endl;
	amoeba_optimizer.minimize(cost,x);
	//cout<<"x="<<x<<endl;
	//cout<<"amoeba_optimizer.maxiter="<<amoeba_optimizer.maxiter<<endl;
	//cout<<"amoeba_optimizer.F_tolerance="<<amoeba_optimizer.F_tolerance<<endl;
	//cout<<"amoeba_optimizer.X_tolerance="<<amoeba_optimizer.X_tolerance<<endl;

	return gaussian(x[0],x[1],x[2]);
	//amp = x[0];
	//center = x[1];
	//sigma = x[2];
}


template <class ELEMTYPE>
ELEMTYPE pts_vector<ELEMTYPE>::fit_two_gaussians_to_histogram_and_return_threshold(string save_histogram_file_path)
{
	fit_gaussians_to_curve_cost_function<ELEMTYPE> cost(this,2, true, true);
	vnl_amoeba amoeba_optimizer = vnl_amoeba(cost);
	amoeba_optimizer.verbose = false;
	amoeba_optimizer.set_x_tolerance(1);
//	amoeba_optimizer.set_relative_diameter(0.10);
//	amoeba_optimizer.set_max_iterations(10);
//	amoeba_optimizer.set_f_tolerance(1);

	vnl_vector<double> x = this->get_vnl_vector_with_start_guess_of_num_gaussians(2);
	cout<<"x="<<x<<endl;
	amoeba_optimizer.minimize(cost,x);
	cout<<"x="<<x<<endl;
	cout<<"amoeba_optimizer.maxiter="<<amoeba_optimizer.maxiter<<endl;
	cout<<"amoeba_optimizer.F_tolerance="<<amoeba_optimizer.F_tolerance<<endl;
	cout<<"amoeba_optimizer.X_tolerance="<<amoeba_optimizer.X_tolerance<<endl;

	gaussian g = gaussian(x[0],x[1],x[2]);
	gaussian g2 = gaussian(x[3],x[4],x[5]);

	if(save_histogram_file_path != ""){
		vector<gaussian> v; v.push_back(g); v.push_back(g2);
		//this->save_histogram_to_txt_file(save_histogram_file_path,v);
	}

//	return x[1] + 3*x[2]; //pos + 2*SD
	return g.get_value_at_intersection_between_centers(g2);
}

//-----------------------------

template <class ELEMTYPE>
void pts_vector<ELEMTYPE>::fit_rayleigh_distr_to_intensity_range(float &amp, float &sigma, ELEMTYPE from_int, ELEMTYPE to_int, bool print_info)
{
	cout<<"fit_rayleigh_distr_to_intensity_range..."<<endl;

	fit_rayleighian_to_histogram_1D_cost_function<ELEMTYPE> cost(this);

	vnl_amoeba amoeba_optimizer = vnl_amoeba(cost);
	amoeba_optimizer.verbose = false;
	amoeba_optimizer.set_x_tolerance(1000);
//	amoeba_optimizer.set_relative_diameter(0.10);
//	amoeba_optimizer.set_max_iterations(10);
//	amoeba_optimizer.set_f_tolerance(1);

	vnl_vector<double> x(2);
	int hist_max_val_bucket_pos;
	ELEMTYPE *m_temp = this->get_maximum_in_range(this->from_val_to_x(from_int),this->from_val_to_x(to_int));
	
	ELEMTYPE hist_max = m_temp[1];
	hist_max_val_bucket_pos = m_temp[0];
	free(m_temp);
	//ELEMTYPE hist_max = this->get_max_value_in_range(this->from_val_to_x(from_int),this->from_val_to_x(to_int), hist_max_val_bucket_pos);
	ELEMTYPE intensity_at_hist_max = this->from_x_to_val(hist_max_val_bucket_pos);

//	x[1] = sqrt(2.0)*intensity_at_hist_max; //M = sqrt(2)*intensity_at_hist_max
//	x[0] = 0.5*hist_max*x[1]*exp(1.0); //amp = 0.5*hist_max_val*M*exp(1)

	x[1] = intensity_at_hist_max; //sig = intensity_at_hist_max
	x[0] = hist_max*x[1]*exp(0.5); //amp = hist_max_val*sigma*exp(0.5)

	cout<<"x="<<x<<endl;
	amoeba_optimizer.minimize(cost,x);
	cout<<"x="<<x<<endl;

	cout<<"amoeba_optimizer.maxiter="<<amoeba_optimizer.maxiter<<endl;
	cout<<"amoeba_optimizer.F_tolerance="<<amoeba_optimizer.F_tolerance<<endl;
	cout<<"amoeba_optimizer.X_tolerance="<<amoeba_optimizer.X_tolerance<<endl;

	amp = x[0];
	sigma = x[1];
}

		
template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_sum_square_diff(rayleighian r, bool ignore_zeros){
	return get_sum_square_diff_from_buckets(r, 0, this->size(), ignore_zeros);
}

template <class ELEMTYPE>
double pts_vector<ELEMTYPE>::get_sum_square_diff_from_buckets(rayleighian r, int from_bucket, int to_bucket, bool ignore_zeros){
	double error = 0;
	float val=0;
	int zero_intensity_bucket = this->from_val_to_x(0);

	for(int j=from_bucket;j<=to_bucket;j++){
		//it is very important to use the intensity, and not the bucket position...

		if(this->at(j)>0 || !ignore_zeros){
			if(j != zero_intensity_bucket){ //dont include the commonly seen peak at zero intensity

				ELEMTYPE intensity = this->from_x_to_val(j);
				val = r.evaluate_at(intensity);	
				error += pow(val - float(this->at(j)), 2);
			}
		}
	}
	return error;
}































/* Scalar ends here */
/* Complex starts here */
template<class ELEMTYPE>
class ptc_vector : public pt_vector<complex<ELEMTYPE>>{
public:
	ptc_vector(int);
	~ptc_vector(void){};
};



template<class ELEMTYPE>
ptc_vector<ELEMTYPE>::ptc_vector(int start_size) : pt_vector<complex<ELEMTYPE>>(start_size){
}
/* Complex ends here */





template<class ELEMTYPE>
class fit_gaussians_to_curve_cost_function : public vnl_cost_function
{
	pts_vector<ELEMTYPE> *the_hist;
	int num_gaussians;
	bool punish_overlap;
	bool punish_large_area_differences;

public:
	fit_gaussians_to_curve_cost_function(pts_vector<ELEMTYPE> *h, int num, bool punish_overlap=false, bool punish_large_area_differences=false);
	double f(vnl_vector<double> const &x);
};

template<class ELEMTYPE>
fit_gaussians_to_curve_cost_function<ELEMTYPE>::fit_gaussians_to_curve_cost_function(pts_vector<ELEMTYPE> *h, int num, bool punish_overl, bool punish_large_area_diffs):vnl_cost_function(num*3)
{
	the_hist = h;
	num_gaussians = num;
	punish_overlap = punish_overl;
	punish_large_area_differences = punish_large_area_diffs;
}

template<class ELEMTYPE>
double fit_gaussians_to_curve_cost_function<ELEMTYPE>::f(vnl_vector<double> const &x)
{
	vector<gaussian> v;
	for(int i=0;i<num_gaussians;i++){
		v.push_back( gaussian(x[i*3+0],x[i*3+1],x[i*3+2]) );
	}
	double res = the_hist->get_sum_square_diff(v);
//	cout<<x<<"-->"<<res<<endl;

	//-------------------------------
	//-------------------------------
	//if(punish_variance_differences){
	double mean_sigma=0;
	double mean_sigma_diff=0;
	for(int i=0;i<v.size();i++){
		mean_sigma += v[i].sigma;
	}
	mean_sigma = mean_sigma/v.size();

	for(int i=0;i<v.size();i++){
		mean_sigma_diff += abs(v[i].sigma-mean_sigma)/mean_sigma;
	}
	mean_sigma_diff = mean_sigma_diff/v.size();
	res = res*(1+0.2*mean_sigma_diff);

	//-------------------------------
	//-------------------------------


	if(punish_overlap){
		std::vector<double> overlaps = the_hist->get_overlaps_in_percent(v);
		double mean_ = mean<double>(overlaps);
//		cout<<"mean_="<<mean_<<endl;
		res = res*(1+mean_);

		res += the_hist->get_sum_square_gaussian_overlap(v);
	}

	if(punish_large_area_differences){

		vector<double> areas = the_hist->get_gaussian_areas(v);
		
		double mean_area = mean<double>(areas);
//		cout<<"mean_area="<<mean_area<<endl;

		double mean_diff_percent=0;
		for(int i=0;i<areas.size();i++){
			mean_diff_percent += abs(mean_area - areas[i])/mean_area;
//			cout<<"mean_diff_percent="<<mean_diff_percent<<endl;
		}
		mean_diff_percent = mean_diff_percent/areas.size();

//		cout<<"*mean_diff_percent="<<mean_diff_percent<<endl;

		res = res*(1+0.2*mean_diff_percent);
	}

	return res;
}




template<class ELEMTYPE>
class fit_rayleighian_to_histogram_1D_cost_function : public vnl_cost_function
{
	pts_vector<ELEMTYPE> *the_hist;

public:
	fit_rayleighian_to_histogram_1D_cost_function(pts_vector<ELEMTYPE> *h);
	double f(vnl_vector<double> const &x);
};

template<class ELEMTYPE>
fit_rayleighian_to_histogram_1D_cost_function<ELEMTYPE>::fit_rayleighian_to_histogram_1D_cost_function(pts_vector<ELEMTYPE> *h):vnl_cost_function(2)
{
	the_hist = h;
}

template<class ELEMTYPE>
double fit_rayleighian_to_histogram_1D_cost_function<ELEMTYPE>::f(vnl_vector<double> const &x)
{
	rayleighian r(x[0],x[1]);
	double res = the_hist->get_sum_square_diff(r, true);
	cout<<x<<"-->"<<res<<endl;

	return res;
}







#endif	//__pt_vector.h__