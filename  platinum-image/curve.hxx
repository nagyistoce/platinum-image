
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

#ifndef __curve_hxx__
#define __curve_hxx__

#include "curve.h"
//#include "datawidget.h"


/*curve_base::curve_base(string name) : data_base(get_supported_renderers())
{
    //start empty
    widget = new datawidget<curve_base>(this, name);
	modified = false;
}


vector<RENDERER_TYPE> curve_base::get_supported_renderers(){
	vector<RENDERER_TYPE> vec;
	vec.push_back(RENDERER_CURVE);
	return vec;
}

curve_base::~curve_base() {}

void curve_base::redraw() {}*/

/*Begin curve_scalar*/
template<class ELEMTYPE>
class curve_scalar : public curve_base{
public:
	curve_scalar(int start_size, string name, double offset, double scale);
	curve_scalar<ELEMTYPE>* copy_curve();
	double get_data(int i) const;
	double get_max() const;
	double get_min() const;
	double get_scale() const;
	double get_offset() const;
	void set_scale(double scale);
	void set_offset(double offset);
	int get_data_size() const;
	RGBvalue* get_color() const;
	void set_color(int r, int g, int b);
	char get_line() const;
	void set_line(char type);
	Vector2D find_closest_maxima(int location, int direction) const;
	Vector2D find_closest_minima(int location, int direction) const;
	vector<Vector2D> find_maximas_in_intervall(int from, int to) const;
	vector<Vector2D> find_minimas_in_intervall(int from, int to) const;
	void simplify_curve();
	void create_dy_curve();


	vector<double> approximate_curve(int degree) const;

	void increase_resolution();
	void save_curve_to_file(std::string s) const;
	void read_curve_from_file(std::string s);
	pt_vector<ELEMTYPE> *my_data;

private:
	RGBvalue *color;
	char line;
};

template<class ELEMTYPE>
curve_scalar<ELEMTYPE>::curve_scalar(int start_size, string name, double offset, double scale) : curve_base(name){
	my_data = new pt_vector<ELEMTYPE>(start_size);
	my_data->config_x_axis(scale, offset);
	color = new RGBvalue();
	color->set_rgb(255,0,0); //Default color = red
	line = '-';
}
template<class ELEMTYPE>
curve_scalar<ELEMTYPE>* curve_scalar<ELEMTYPE>::copy_curve(){
	curve_scalar<ELEMTYPE> *c = new curve_scalar<ELEMTYPE>(0, name() + " copy", get_offset(), get_scale());
	c->my_data->assign(my_data->begin(), my_data->end());
	return c;
}

template<class ELEMTYPE>
double curve_scalar<ELEMTYPE>::get_data(int i) const{
	return static_cast<double>( my_data->at(i));
}

template<class ELEMTYPE>
double curve_scalar<ELEMTYPE>::get_min() const{
	return static_cast<double>( my_data->get_minimum_in_range(0, my_data->size()-1)[1]);
}

template<class ELEMTYPE>
double curve_scalar<ELEMTYPE>::get_max() const{
	int dummy;
	return static_cast<double>( my_data->get_maximum_in_range(0, my_data->size()-1, dummy));
}
template<class ELEMTYPE>
double curve_scalar<ELEMTYPE>::get_scale() const{
	return static_cast<double>( my_data->x_res);
}
template<class ELEMTYPE>
double curve_scalar<ELEMTYPE>::get_offset() const{
	return static_cast<double>( my_data->x_axis_start);
}

template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::set_scale(double scale){
	my_data->x_res = scale;
}
template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::set_offset(double offset){
	my_data->x_axis_start = offset;
}

template<class ELEMTYPE>
int curve_scalar<ELEMTYPE>::get_data_size() const{
	return my_data->size();
}
template<class ELEMTYPE>
RGBvalue* curve_scalar<ELEMTYPE>::get_color() const{
	return color;
}
template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::set_color(int r, int g, int b){
	color->set_rgb(r,g,b);
}

template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::set_line(char type){
	cout << "line type: " << type << endl;
	line = type;
}

template<class ELEMTYPE>
char curve_scalar<ELEMTYPE>::get_line() const{
	return line;
}

template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::increase_resolution(){
	pt_vector<ELEMTYPE> *temp = new pt_vector<ELEMTYPE>(my_data->size()*2-1);
	temp->assign(0, my_data->size()*2-1);
	temp->config_x_axis(my_data->x_res, my_data->x_axis_start);

	int i;
	for(i = 0; i < my_data->size()-1; i++){
		temp->push_back(my_data->at(i));
		temp->push_back(abs(my_data->at(i) + my_data->at(i+1))/2);
	}
	temp->push_back(my_data->at(i));
	
	temp->config_x_axis(temp->x_res/2,temp->x_axis_start);

	modified = true;
	my_data->clear();
	my_data = temp;
}


template<class ELEMTYPE>
Vector2D curve_scalar<ELEMTYPE>::find_closest_maxima(int location, int direction) const{
	if(location == 0){
		location++; //Needs one element on each side!
	}else if(location == my_data->size() -1){
		location--;
	}
	while((location-abs(direction) >= 0  && location+abs(direction) < my_data->size())  && 
		(my_data->at(location)  <= my_data->at(location - direction) || my_data->at(location)  <= my_data->at(location + direction))){
		
		location+=direction;
    }

	Vector2D ret_val;
	if(location > 0  && location < my_data->size()-1){ //found maxima
		ret_val[0] = location;
		ret_val[1] = my_data->at(location);
	}else{
		ret_val[0] = -1;
		ret_val[1] = -1;
	}
	return ret_val;
}

template<class ELEMTYPE>
Vector2D curve_scalar<ELEMTYPE>::find_closest_minima(int location, int direction) const{
	if(location == 0){
		location++; //Needs one element on each side!
	}else if(location == my_data->size() -1){
		location--;
	}
    while((location-abs(direction) >= 0  && location+abs(direction) < my_data->size()) && 
		(my_data->at(location)  >= my_data->at(location - direction) || my_data->at(location)  >= my_data->at(location + direction))){
        
		location+=direction;
    }
	Vector2D ret_val;
	if(location > 0  && location < my_data->size()-1){ //found minima
		ret_val[0] = location;
		ret_val[1] = my_data->at(location);
	}else{
		ret_val[0] = -1;
		ret_val[1] = -1;
	}
	return ret_val;
}

template<class ELEMTYPE>
vector<Vector2D> curve_scalar<ELEMTYPE>::find_maximas_in_intervall(int from, int to) const{
        
        vector<Vector2D> ret_vec;
        Vector2D new_pos;
		
		if(from < 0){
			return ret_vec;
		}
		new_pos[0] = from;
		new_pos[1] = my_data->at(from);

        while (new_pos[0] <= to){
                new_pos = find_closest_maxima(new_pos[0] + 1, 1);
                if(new_pos[0] != -1 && new_pos[0] <= to){
                       ret_vec.push_back(new_pos);
                }else{
                    return ret_vec;
                }
        }
		return ret_vec;
}
template<class ELEMTYPE>
vector<Vector2D> curve_scalar<ELEMTYPE>::find_minimas_in_intervall(int from, int to) const{
        
        vector<Vector2D> ret_vec;
        Vector2D new_pos;
		new_pos[0] = from;
		new_pos[1] = my_data->at(from);
		if(from < 0){
			return ret_vec;
		}
        while (new_pos[0] <= to){
                new_pos = find_closest_minima(new_pos[0] + 1, 1);
                if(new_pos[0] != -1 && new_pos[0] <= to){
                        ret_vec.push_back(new_pos);
                }else{
                        return ret_vec;
                }
        }
		return ret_vec;
}
/**
	Reduces the points to minima/maxima points (with fillers in between)
*/
template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::simplify_curve(){
        
        vector<Vector2D> maximas;
		vector<Vector2D> minimas;
		int min_loc, max_loc, index;
		min_loc = max_loc = 0;
		bool save;

        maximas = find_maximas_in_intervall(0, my_data->size());
		minimas = find_minimas_in_intervall(0, my_data->size());

		//my_data->assign(0, my_data->size());
		for(int i = 0; i < my_data->size(); i++){
			save = false;
			for(int x = min_loc; x < minimas.size(); x++){
				if(round(minimas.at(x)[0]) == i){
					save = true;
					min_loc = x;
					break;
				}
			}
			if(!save){
				for(int y = max_loc; y < maximas.size(); y++){
					if(round(maximas.at(y)[0]) == i){
						save = true;
						max_loc = y;
						break;
					}
				}
			}
			if(!save){
				my_data->at(i) = 0;
			}

		}
}
template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::create_dy_curve(){
	int y1, y2;
	if(my_data->empty())
		return;
	y1 = my_data->at(0);
	for(int i = 1; i < my_data->size(); i++){
		y2 = my_data->at(i);
		my_data->at(i-1) = abs(y2-y1);
		y1 = y2;
	}
	my_data->pop_back();
}


/* Fit points to polynomial curve with least square approximation. Can be slow due to matrix inverse calculation*/
template<class ELEMTYPE>
vector<double> curve_scalar<ELEMTYPE>::approximate_curve(int degree) const{
	vnl_matrix<double> A;
	A.set_size(my_data->size(), degree+1);

	vnl_vector<double> b(my_data->size());
	vnl_vector<double> ans(degree+1);
	vector<double> ret_vec;

	double scale, offset;
	//double y;
	double x;

	scale = get_scale();
	offset = get_offset();

	//Build A and b 
	for(int i = 0; i < my_data->size(); i++){
		//y = my_data->at(i);
		b[i] = my_data->at(i);
		x = i*scale + offset;

		A[i][degree] = 1; //x^0
		A[i][degree - 1] = x; //x^1

		for(int j = 0; j<degree -1; j++){
			A[i][j] = pow(x, degree - j);
		}
	}
	//ans = (A^T*A)^-1 *A^T*b
	ans = vnl_matrix_inverse<double>(A.transpose()*A)*A.transpose()*b;

	for(int t = 0; t<= degree ; t++)
		ret_vec.push_back(ans[t]);
	return ret_vec;
}
template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::save_curve_to_file(std::string s) const{
	ofstream myfile(s.c_str());
	if(myfile.is_open()){
		myfile << my_data->size() << "\n";
		myfile << get_offset() << " " << get_scale() << "\n";
		for(int i = 0; i < my_data->size(); i++){
			myfile << my_data->at(i) << "\n";
		}
		myfile.close();
		
	}
}
template<class ELEMTYPE>
void curve_scalar<ELEMTYPE>::read_curve_from_file(std::string s){
	ifstream myfile(s.c_str(), ios::binary);
	int size;
	double offset, scale;
	ELEMTYPE t;
	if(myfile.is_open()){
		myfile >> size;
		myfile >> offset >> scale;
		
		my_data->config_x_axis(scale, offset);
		for(int i = 0; i < size; i++){
			myfile >> t ;
			my_data->push_back(t);
		}
		myfile.close();
	}
	int index = s.find_last_of("/") + 1;
	int stop = s.find_last_of(".");
	this->name(s.substr(index,stop-index));
}


/* End curve_scalar*/

/*Begin curve_integer*/

/*template<class ELEMTYPE>
class curve_integer : public curve_scalar<ELEMTYPE>{
public:
	curve_integer(int start_size, string name);
	curve_integer(int start_size, int r, int g, int b);
};

template<class ELEMTYPE>
curve_integer<ELEMTYPE>::curve_integer(int start_size, string name) : curve_scalar<ELEMTYPE>(start_size, name){
	//Empty contructor
}
template<class ELEMTYPE>
curve_integer<ELEMTYPE>::curve_integer(int start_size, int r, int g, int b) : curve_scalar<ELEMTYPE>(start_size, r, g, b){
	//Empty contructor
}*/
#endif