
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
	double get_max(SPECTRUM_TYPE curve = SP_TIME, SPECTRUM_TYPE t = SP_REAL);
	double get_min(SPECTRUM_TYPE curve = SP_TIME, SPECTRUM_TYPE t = SP_REAL);
	double get_scale() const;
	double get_offset() const;
	void set_scale(double scale);
	void set_offset(double offset);
	int get_data_size() const;
	RGBvalue* get_color() const;
	void set_color(int r, int g, int b);
	char get_line() const;
	void set_line(char type);
	//char get_x_type() const {return 'tmp';}; //Dummy function
	//bool get_y_type(int i) const {return false;}; //Dummy function
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
	pts_vector<ELEMTYPE> *my_data;

private:
	RGBvalue *color;
	char line;
};

template<class ELEMTYPE>
curve_scalar<ELEMTYPE>::curve_scalar(int start_size, string name, double offset, double scale) : curve_base(name, true){
	my_data = new pts_vector<ELEMTYPE>(start_size);
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
double curve_scalar<ELEMTYPE>::get_min(SPECTRUM_TYPE curve, SPECTRUM_TYPE t) {
	int dummy;
	return static_cast<double>( my_data->get_minimum_in_range(0, my_data->size()-1,dummy));
}

template<class ELEMTYPE>
double curve_scalar<ELEMTYPE>::get_max(SPECTRUM_TYPE curve, SPECTRUM_TYPE t) {
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
	//pt_vector<ELEMTYPE> *temp = new pt_vector<ELEMTYPE>(my_data->size()*2-1);
//	temp->assign(0, my_data->size()*2-1);
//	temp->config_x_axis(my_data->x_res, my_data->x_axis_start);
//
//	int i;
//	for(i = 0; i < my_data->size()-1; i++){
//		temp->push_back(my_data->at(i));
//		temp->push_back(abs<ELEMTYPE>(my_data->at(i) + my_data->at(i+1))/2);
//	}
//	temp->push_back(my_data->at(i));
//	
//	temp->config_x_axis(temp->x_res/2,temp->x_axis_start);
//
//	modified = true;
//	my_data->clear();
//	my_data = temp;
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
		int min_loc, max_loc;//, index;
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

/*/*Begin curve_scalar*/
template<class ELEMTYPE>
class curve_complex : public curve_complex_base{
public:
	curve_complex(int start_size, string name, double offset, double scale);
//	curve_complex<ELEMTYPE>* copy_curve();

//	void save_curve_to_file(std::string s) const;
//	void read_curve_from_file(std::string s);
	
	double get_data(int i) const; //same as get_real for compatibility reasons
	double get_real(int i, SPECTRUM_TYPE t) const;
	double get_imag(int i, SPECTRUM_TYPE t) const;
	double get_phase(int i, SPECTRUM_TYPE t) const;
	double get_magnitude(int i, SPECTRUM_TYPE t) const;


	double get_max(SPECTRUM_TYPE curve = SP_TIME, SPECTRUM_TYPE t = SP_REAL);
	double get_min(SPECTRUM_TYPE curve = SP_TIME, SPECTRUM_TYPE t = SP_REAL);
	//complex<ELEMTYPE> get_data(int i);
	
	
	double get_scale() const;//
	double get_offset() const;//
	int get_data_size() const;//
	RGBvalue* get_color() const {return color;};//
	char get_line() const {return line;};//
	void set_data(int i, complex<double>, bool recalc = false);
	void set_main_data(ptc_vector<ELEMTYPE> *vals, SPECTRUM_TYPE t);
	void set_scale(double scale);
	void set_offset(double offset);
	void set_color(int r, int g, int b){color->r(r); color->g(g); color->b(b);};
	void set_line(char type){line = type;};


	void save_curve_to_file(std::string s) const;
	void read_curve_from_file(std::string s);


	void amares(string prior_knowledge, int nr_sinusoids);
	vector<vector<double> > read_starting_vals(string file);
	vnl_vector<double> amares_start_vals(vector<double> f, vector<double> d, vnl_vector<ELEMTYPE> y, int N, int K);

private:
	RGBvalue *color;
	
	void transform(SPECTRUM_TYPE t);
	vnl_vector<vcl_complex<ELEMTYPE> > flip_halves(vnl_vector<vcl_complex<ELEMTYPE> > u);
	char line;
	ptc_vector<ELEMTYPE> *time;
	ptc_vector<ELEMTYPE> *frequency;
	//SPECTRUM_TYPE y_type;
	//SPECTRUM_TYPE x_type;
};

template<class ELEMTYPE>
curve_complex<ELEMTYPE>::curve_complex(int start_size, string name, double offset, double scale) : curve_complex_base(name){
	
	time = new ptc_vector<ELEMTYPE>(start_size);
	time->config_x_axis(scale, offset);

	frequency = new ptc_vector<ELEMTYPE>(start_size);
	frequency->config_x_axis(scale, offset);

	color = new RGBvalue();
	color->set_rgb(255,0,0); //Default color = red
	line = '-';

	active_type = 't';
	phase_shift = complex<double>(1, 0);
	//x_type = SP_TIME;
	//y_type = SP_REAL;
}
template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_real(int i, SPECTRUM_TYPE t) const {
	complex<double> temp = phase_shift;
	if(t == SP_TIME)
		temp*= time->at(i);
	else
		temp*= frequency->at(i);
	return temp.real();
}
template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_data(int i) const{
	complex<double> temp = phase_shift;
	if(this->active_type == 't')
		temp*= time->at(i);
	else
		temp*= frequency->at(i);
	return temp.real();
}

template<class ELEMTYPE>
int curve_complex<ELEMTYPE>::get_data_size() const{
	if(this->active_type == 't')
		return time->size();
	else
		return frequency->size();
}

template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_max(SPECTRUM_TYPE curve, SPECTRUM_TYPE t){
	ELEMTYPE max = numeric_limits<ELEMTYPE>::max();
	max = -max;
	int size = t == SP_FREQ ? frequency->size() : time->size();
	if(t == SP_REAL){
		for(int i = 0; i < size; i++){
			if(get_real(i, curve) > max)
				max = get_real(i, curve);
		}
	}else if(t == SP_COMPLEX){
		for(int i = 0; i < size; i++){
			if(get_imag(i, curve) > max)
				max = get_imag(i, curve);
		}
	}else if(t == SP_MAGNITUDE){
		for(int i = 0; i < size; i++){
			if(get_magnitude(i, curve) > max)
				max = get_magnitude(i, curve);
		}
	}else if(t == SP_PHASE){
		for(int i = 0; i < size; i++){
			if(get_phase(i, curve) > max)
				max = get_phase(i, curve);
		}
	}
	return static_cast<double>(max);
}
template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_min(SPECTRUM_TYPE curve, SPECTRUM_TYPE t) {
	ELEMTYPE min = numeric_limits<ELEMTYPE>::max();
	int size = t == SP_FREQ ? frequency->size() : time->size();
	if(t == SP_REAL){
		for(int i = 0; i < size; i++){
			if(get_real(i, curve) < min)
				min = get_real(i, curve);
		}
	}else if(t == SP_COMPLEX){
		for(int i = 0; i < size; i++){
			if(get_imag(i, curve) < min)
				min = get_imag(i, curve);
		}
	}else if(t == SP_MAGNITUDE){
		for(int i = 0; i < size; i++){
			if(get_magnitude(i, curve) < min)
				min = get_magnitude(i, curve);
		}
	}else if(t == SP_PHASE){
		for(int i = 0; i < size; i++){
			if(get_phase(i, curve) < min)
				min = get_phase(i, curve);
		}
	}
	return static_cast<double>(min);
}


template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_imag(int i, SPECTRUM_TYPE t) const{
	complex<double> temp = phase_shift;
	if(t == SP_TIME)
		temp*= time->at(i);
	else
		temp*= frequency->at(i);
	return temp.imag();
}
template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_phase(int i, SPECTRUM_TYPE t) const{
	complex<double> temp = phase_shift;
	if(t == SP_TIME)
		temp*= time->at(i);
	else
		temp*= frequency->at(i);
	return arg(temp);
}
template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_magnitude(int i, SPECTRUM_TYPE t) const{
	double r = pow(this->get_real(i,t),2); //Should this use the phase shifted values?
	double c = pow(this->get_imag(i,t),2);
	return sqrt(r + c);
}

template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_scale() const {
		if(this->base_type == SP_TIME){return static_cast<double>( time->x_res);}
		else{return static_cast<double>( frequency->x_res);}
}//
template<class ELEMTYPE>
double curve_complex<ELEMTYPE>::get_offset() const {
		if(this->base_type == SP_TIME){return static_cast<double>( time->x_axis_start);}
		else{return static_cast<double>(frequency->x_axis_start);}
}

template<class ELEMTYPE>
void curve_complex<ELEMTYPE>::set_scale(double scale){
		if(this->base_type == SP_TIME){time->x_res = scale;}
		else{frequency->x_res = scale;}
}//
template<class ELEMTYPE>
void curve_complex<ELEMTYPE>::set_offset(double offset){
		if(this->base_type == SP_TIME){time->x_axis_start = offset;}
		else{frequency->x_axis_start = offset;}
}//

template<class ELEMTYPE>
void curve_complex<ELEMTYPE>::set_main_data(ptc_vector<ELEMTYPE> *vals, SPECTRUM_TYPE t){
	base_type = t;
	time->clear();
	frequency->clear();
	//Scale and offset is not copied to this curve!!!
	if(t == SP_TIME){
		for(int i = 0; i < vals->size(); i++){
			time->push_back(vals->at(i));
		}
	}else{
		for(int i = 0; i < vals->size(); i++){
			frequency->push_back(vals->at(i));
		}
	}
	cout << "transform" << endl;
	transform(base_type);
	cout << "transform done" << endl;
}
template<class ELEMTYPE>
void curve_complex<ELEMTYPE>::set_data(int i, complex<double> val, bool recalc){
	if( i < 0)
		return;
	
	if(base_type == SP_TIME && i < time->size()){
		time->at(i) = val;
	}else if(base_type == SP_FREQ && i < frequency->size()){
		frequency->at(i) = val;
	}
	if(recalc)
		transform(base_type);
}


template<class ELEMTYPE>
void curve_complex<ELEMTYPE>::save_curve_to_file(std::string s) const{
	ofstream myfile(s.c_str());
	if(myfile.is_open()){
		myfile << time->size() << "\n";
		myfile << get_offset() << " " << get_scale() << "\n";
		for(int i = 0; i < time->size(); i++){
			myfile << get_real(i) <<  " " << get_imag(i) << "\n";
		}
		myfile.close();
		
	}
}
template<class ELEMTYPE>
void curve_complex<ELEMTYPE>::read_curve_from_file(std::string s){
	ifstream myfile(s.c_str(), ios::binary);
	int size;
	double offset, scale;
	ELEMTYPE r, c;
	if(myfile.is_open()){
		myfile >> size;
		myfile >> offset >> scale;
		
		time->config_x_axis(scale, offset);
		for(int i = 0; i < size; i++){
			myfile >> r >> c ;
			time->push_back(complex<ELEMTYPE>(r,c));
		}
		myfile.close();
	}
	int index = s.find_last_of("/") + 1;
	int stop = s.find_last_of(".");
	this->name(s.substr(index,stop-index));
}

template<class ELEMTYPE>
void curve_complex<ELEMTYPE>::transform(SPECTRUM_TYPE from){
	int N;
	if(from == SP_TIME)
		N = time->size();
	else
		N = frequency->size();

	int n=N;
	//cout<<"n="<<n<<endl;
	n = get_smallest_power_above(n);
	//cout<<"n="<<n<<endl;
	vnl_vector<vcl_complex<ELEMTYPE> > u(n);
	u.fill(0); //fill vector even outside "image" span!
	vnl_fft_1d<ELEMTYPE> fft2(n);


	for(int i=0;i<n;i++){
		if(i<N)
			u[i] = vcl_complex<ELEMTYPE>( get_real(i,from),get_imag(i,from));
		else
			u[i] = vcl_complex<ELEMTYPE>( 0, 0 );
	}
	if(from == SP_FREQ){
		cout << "from freq to time" << endl;
		u = flip_halves(u);
		fft2.bwd_transform(u);
		time->clear();
		for(int i=0;i<N;i++)
			time->push_back(complex<ELEMTYPE>(u[i].real(), u[i].imag()));
	}else{
		cout << "from time to freq" << endl;
		fft2.fwd_transform(u);
		u = flip_halves(u);
		frequency->clear();
		for(int i=0;i<N;i++)
			frequency->push_back(complex<ELEMTYPE>(u[i].real(), u[i].imag()));
	}
	cout << "time: " << time->size() << "   freq: " << frequency->size() << endl;
}

template<class ELEMTYPE>
vnl_vector<vcl_complex<ELEMTYPE> > curve_complex<ELEMTYPE>::flip_halves(vnl_vector<vcl_complex<ELEMTYPE> > u){
	vnl_vector<vcl_complex<ELEMTYPE> > reversed(u.size());
	int size;
	size = u.size();
	int half_size = size/2;
	for(int i = 0; i < half_size; i++){
		reversed[i] = u[i+half_size];
		reversed[i+half_size] = u[i];
	}
	return reversed;
}


template<class ELEMTYPE>
class amares_cost_function;

/*Does an AMARES optimization and returns the amplitudes of the peaks found*/
template<class ELEMTYPE>
void curve_complex<ELEMTYPE>::amares(string prior_knowledge, int nr_sinusoids){
	/*vector<vector<double> > starting_vals;


	starting_vals = read_starting_vals(prior_knowledge);
	//read file from here!
	
	vnl_vector<ELEMTYPE> y(time->size()*2);

	for(int i = 0; i < time->size(); i++){
		y[i*2] = time->at(i).real();
		y[i*2+1] = time->at(i).imag();
	}

	amares_cost_function<ELEMTYPE> cost(y,time->size(), nr_sinusoids);
	vnl_amoeba amoeba_optimizer = vnl_amoeba(cost);
	amoeba_optimizer.verbose = false;
	amoeba_optimizer.set_x_tolerance(1);

//	amoeba_optimizer.set_max_iterations(10);

	vnl_vector<double> x = amares_start_vals(starting_vals.at(0), starting_vals.at(1), y,time->size(), nr_sinusoids);
	//cout<<"x="<<x<<endl;
	amoeba_optimizer.minimize(cost,x);*/
}

template<class ELEMTYPE>
vector<vector<double> > curve_complex<ELEMTYPE>::read_starting_vals(string prior_knowledge){
	string::size_type i = prior_knowledge.find_last_of(".");
	string path = prior_knowledge.substr(0,i);
	path = path + ".sv";
	vector<vector<double> > result;
	/*vector<double> freq;
	vector<double> d;
	double val;
	double ds;
	char c;

	ifstream myfile(path.c_str(),ios::binary);
	if(myfile.is_open()){
		while(!myfile.eof()){
			c = myfile.get();
			if(c == '#'){
				myfile.get();
				myfile.read((char*)(&val), sizeof(val));
				char* temp = (char*) (&val);
				cout << "---------------------------------" << endl;
				char t = temp[0];
				temp[0] = temp[7];
				temp[7] = t;

				t = temp[1];
				temp[1] = temp[6];
				temp[6] = t;

				t = temp[2];
				temp[2] = temp[5];
				temp[5] = t;

				t = temp[3];
				temp[3] = temp[4];
				temp[4] = t;

				cout << "freq: " << val << endl;
				freq.push_back(val);

				myfile.get();
				myfile.read((char*)(&ds), sizeof(ds));



				temp = (char*) (&ds);
				t = temp[0];
				temp[0] = temp[7];
				temp[7] = t;

				t = temp[1];
				temp[1] = temp[6];
				temp[6] = t;

				t = temp[2];
				temp[2] = temp[5];
				temp[5] = t;

				t = temp[3];
				temp[3] = temp[4];
				temp[4] = t;



				cout << "d: " << ds << endl;
				d.push_back(ds);
			}
		}
	}
	myfile.close();
	result.push_back(freq);
	result.push_back(d);*/
	return result;
}


template<class ELEMTYPE>
vnl_vector<double> curve_complex<ELEMTYPE>::amares_start_vals(vector<double> f, vector<double> d, vnl_vector<ELEMTYPE> y, int N, int K){

	/*vnl_matrix<ELEMTYPE> grep(N*2,K);
	vnl_vector<ELEMTYPE> I;

	vector<double> t(N); //This needs to be calcualted!!!

	for(int n = 0; n < N; n++){
		t.at(n) == n;
		for(int k = 0; k < K; k++){
			grep[n*2][k]   = exp(-d[k]*t[n])*cos(2*pt_PI*f[k]*t[n]); //This is a rewrite of [5] with no rewrite for gauss/lorentzian???
			grep[n*2+1][k] = exp(-d[k]*t[n])*sin(2*pt_PI*f[k]*t[n]);
		}
	}

	vnl_matrix<ELEMTYPE> pseudo_grep = vnl_matrix_inverse<ELEMTYPE>(grep.transpose()*grep)*grep.transpose();

	I = pseudo_grep*y;*/

	vnl_vector<double> x(K*4);
	//TODO read these from file or something!!!
	/*for(int i = 0; i < K; i++){
		x[i*4+0] = f[i];
		x[i*4+1] = d[i];
		//x[i*4+3] = sqrt(pow(I[i*2],2)+pow(I[i*2+1],2)); //add this later. quick compile error fix!
		x[i*4+2] = acos(I[i*2]/x[i*4+3]);
		
	}*/
	return x;
}

template<class ELEMTYPE>
class amares_cost_function : public vnl_cost_function
{
private:
	vnl_vector<ELEMTYPE> y;
	int N;//nr_points
	int K;//nr_sinusoids

public:
	amares_cost_function(vnl_vector<ELEMTYPE> curve, int nr_points, int nr_sinusoids);
	double f(vnl_vector<double> const &x);
};

template<class ELEMTYPE>
amares_cost_function<ELEMTYPE>::amares_cost_function(vnl_vector<ELEMTYPE> curve, int nr_points, int nr_sinusoids):vnl_cost_function(nr_sinusoids*4)//four vals per points
{
	y = curve;
	N = nr_points;
	K = nr_sinusoids;
}

template<class ELEMTYPE>
double amares_cost_function<ELEMTYPE>::f(vnl_vector<double> const &x)
{
	//d = 0 f = 1 p = 2
	/*vnl_matrix<ELEMTYPE> grep(N*2,K);
	vnl_vector<ELEMTYPE> I(K);
	vnl_vector<ELEMTYPE> res_vec;//(N*2);
	vector<double> t(N); //This needs to be calcualted!!!

	for(int k = 0; k < K; k++){
			I[k] = x[k*4];
	}

	for(int n = 0; n < N; n++){
		t.at(n) == n;
		for(int k = 0; k < K; k++){
			//grep[n*2][k]   = exp(-d[k]*t[n])*cos(2*pt_PI*f[k]*t[n] + p[k]); //This is a rewrite of [6] with no rewrite for gauss/lorentzian???
			//grep[n*2+1][k] = exp(-d[k]*t[n])*sin(2*pt_PI*f[k]*t[n] + p[k]);
			grep[n*2][k]   = exp(-x[k+(n*4)]*t[n])*cos(2*pt_PI*x[k+(n*4)+1]*t[n] + x[k+(n*4)+2]); //This is a rewrite of [6] with no rewrite for gauss/lorentzian???
			grep[n*2][k]   = exp(-x[k+(n*4)]*t[n])*sin(2*pt_PI*x[k+(n*4)+1]*t[n] + x[k+(n*4)+2]);
		}
	}
	
	res_vec = y - grep*I;
	double res = 0;
	for(int i = 0; i<2*N; i++){
		//res+= pow(res_vec[i],2); //add this later. quick compile error fix!
	}
	return sqrt(res);*/
	return 0; //remove when uncommenting

}


#endif