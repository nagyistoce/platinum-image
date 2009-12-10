// $Id: filters.cc $

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

#ifndef __filters_cc__
#define __filters_cc__

#include "filters.h"


filter_median::filter_median(int nx, int ny, int nz, int xcenter, int ycenter, int zcenter) 
{
	if (xcenter<-1000) {xcenter=nx/2;}
	if (ycenter<-1000) {ycenter=ny/2;}
	if (zcenter<-1000) {zcenter=nz/2;}
	data = vnl_matrix<float>(nx*ny*nz,3);
	int element=0;
	for (int z=0; z<nz ; z++) {
		for (int y=0; y<ny ; y++) {
			for (int x=0; x<nx; x++) {	
				data.put(element, 0, x-xcenter);
				data.put(element, 1, y-ycenter);
				data.put(element, 2, z-zcenter);
				element++;
			}
		}
	}
}

float filter_median::apply(float* neighbourhood)
{
	int size=this->data.rows();
	sort(neighbourhood,neighbourhood+size);
	int undefined=0;
	for (int i=0; i<size; i++) {
		if (neighbourhood[i]>=std::numeric_limits<float>::max() ) {undefined++;}
	}	
	return neighbourhood[(size-undefined)/ 2];
}



filter_central_difference_magn_2d::filter_central_difference_magn_2d(int dir) 
{
	data = vnl_matrix<float>(4,4); //4 coordinates and 4 different values (x,y,z,w)...
	if(dir==0){
		//x					//y					//z				//w
		data.put(0,0,0); data.put(0,1,0);	data.put(0,2,-1); data.put(0,3,-1);	//neg z-dir
		data.put(1,0,0); data.put(1,1,0);	data.put(1,2,+1); data.put(1,3,+1);	//pos z-dir
		data.put(2,0,0); data.put(2,1,-1);	data.put(2,2,0);  data.put(2,3,-1);	//neg y-dir
		data.put(3,0,0); data.put(3,1,+1);	data.put(3,2,0);  data.put(3,3,+1);	//pos y-dir
	}else if(dir==1){
		data.put(0,0,0); data.put(0,1,0);	data.put(0,2,-1); data.put(0,3,-1);	//neg z-dir
		data.put(1,0,0); data.put(1,1,0);	data.put(1,2,+1); data.put(1,3,+1);	//pos z-dir
		data.put(2,0,-1);data.put(2,1,0);	data.put(2,2,0);  data.put(2,3,-1);	//neg x-dir
		data.put(3,0,+1);data.put(3,1,0);	data.put(3,2,0);  data.put(3,3,+1);	//pos x-dir
	}else{
		data.put(0,0,-1); data.put(0,1,0);	data.put(0,2,0); data.put(0,3,-1);	//neg x-dir
		data.put(1,0,+1); data.put(1,1,0);	data.put(1,2,0); data.put(1,3,+1);	//pos x-dir
		data.put(2,0,0);  data.put(2,1,-1);	data.put(2,2,0); data.put(2,3,-1);	//neg y-dir
		data.put(3,0,0);  data.put(3,1,+1);	data.put(3,2,0); data.put(3,3,+1);	//pos y-dir
	}
}

float filter_central_difference_magn_2d::apply(float* neighbourhood)
{
	//pixels outside boundary are set to "std::numeric_limits<float>::max()"...
	//undo this here to prevent overflow...
	for(int i=0;i<4;i++){
		if(neighbourhood[i] == std::numeric_limits<float>::max()){
			neighbourhood[i] = 0;
		}
	}

	float du = neighbourhood[1]-neighbourhood[0];
	float dv = neighbourhood[3]-neighbourhood[2];
	//note that the weights are never used...
	return sqrt(du*du+dv*dv);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------


void filter_linear::set_data_from_floats(float* w, int nx, int ny, int nz, int xcenter, int ycenter, int zcenter) 
{
	data = vnl_matrix<float>(nx*ny*nz,4);
	int element=0;
	for (int z=0; z<nz ; z++) {
		for (int y=0; y<ny ; y++) {
			for (int x=0; x<nx; x++) {
				data.put(element, 0, x-xcenter);
				data.put(element, 1, y-ycenter);
				data.put(element, 2, z-zcenter);
				data.put(element, 3, w[x+nx*y+nx*ny*z]);
				element++;
			}
		}		
	}
}

void filter_linear::set_data_from_ints(int *w, int nx, int ny, int nz, int xcenter, int ycenter, int zcenter)
{
	int num_elements=0;
	for (int i=0; i<nx*ny*nz; i++) {
		if (w[i]!=0) {num_elements++;}
	}
	data = vnl_matrix<float>(num_elements,4);
	int element=0;
	for (int z=0; z<nz ; z++) {
		for (int y=0; y<ny ; y++) {
			for (int x=0; x<nx; x++) {
				if (w[x+nx*y+nx*ny*z]!=0) {
					data.put(element, 0, x-xcenter);
					data.put(element, 1, y-ycenter);
					data.put(element, 2, z-zcenter);
					data.put(element, 3, w[x+nx*y+nx*ny*z]);
					element++;
				}
			}
		}		
	}
}

float filter_linear::apply(float* neighbourhood) {
	float res=0;
	for (int i=0; i<this->data.rows(); i++) {
		if (neighbourhood[i]<std::numeric_limits<float>::max() ) {
			res+=this->data.get(i,3)*neighbourhood[i];
		}
	}
	return res;
}

void filter_linear::print() {
	cout<<"filter_linear::print()"<<endl;

	for (int i=0; i<data.rows(); i++) {
		for (int j=0; j<data.cols(); j++) {
			cout << data.get(i,j) << "  ";
		}
		cout << endl;
	}
}

filter_sobel_2d::filter_sobel_2d(int orientation, int dir) {
	// orientation=in-plane direction; 0=east, 1=southeast, 2=south...7=northeast
	if (orientation<0) {orientation=0;}
	//int nx=3; int ny=3; int nz=1;
	int *w = new int[9];
	w[0]=abs((orientation+2)%8-3)-2; w[1]= abs((orientation+1)%8-3)-2;	w[2]= abs((orientation+0)%8-3)-2;
	w[3]=abs((orientation+3)%8-3)-2; w[4]= 0;							w[5]= abs((orientation+7)%8-3)-2;
	w[6]=abs((orientation+4)%8-3)-2; w[7]= abs((orientation+5)%8-3)-2;	w[8]= abs((orientation+6)%8-3)-2;
	//this->set_data_from_ints(w,nx,ny,nz,1,1,0);
	if (dir==0) {this->set_data_from_ints(w,1,3,3,0,1,1);}
	else if (dir==1) {this->set_data_from_ints(w,3,1,3,1,0,1);}
	else {this->set_data_from_ints(w,3,3,1,1,1,0);}
	delete w;
	cout << "Created 2d sobel filter, orientation: " << orientation%8 << endl;
}

filter_laplace_2d::filter_laplace_2d(int dir, bool eight_connected) //four-connected if not eight-connected
{
	int *w = new int[9];
	w[0]=eight_connected;	w[1]= 1;						w[2]= eight_connected;
	w[3]=1;					w[4]= -4 -4*eight_connected;	w[5]= 1;
	w[6]=eight_connected;	w[7]= 1;						w[8]= eight_connected;
	
	if (dir==0) {this->set_data_from_ints(w,1,3,3,0,1,1);}
	else if (dir==1) {this->set_data_from_ints(w,3,1,3,1,0,1);}
	else {this->set_data_from_ints(w,3,3,1,1,1,0);}

	delete w;
	cout << "Created 2d Laplace filter" << endl;
}

filter_laplace_1d::filter_laplace_1d(int dir)
{
	int *w = new int[3];
	w[0]= 1;
	w[1]=-2;
	w[2]= 1;
	
	if (dir==0) {this->set_data_from_ints(w,3,1,1,1,0,0);}
	else if (dir==1) {this->set_data_from_ints(w,1,3,1,0,1,0);}
	else {this->set_data_from_ints(w,1,1,3,0,0,1);}

	delete w;
	cout << "Created 1d Laplace filter" << endl;
}

filter_central_difference::filter_central_difference(int dir)
{
	int *w = new int[3];
	w[0]=-1;
	w[1]= 0;
	w[2]= 1;
	
	if (dir==0) {this->set_data_from_ints(w,3,1,1,1,0,0);}
	else if (dir==1) {this->set_data_from_ints(w,1,3,1,0,1,0);}
	else {this->set_data_from_ints(w,1,1,3,0,0,1);}

	delete w;
	cout << "Created 1d central difference filter" << endl;
}

filter_central_difference_plane::filter_central_difference_plane(int dir)
{
	int *w = new int[3];
	w[0]=-1;
	w[1]= 0;
	w[2]= 1;
	
	if (dir==0) {
		this->set_data_from_ints(w,3,1,1,1,0,0);
	}

	else if (dir==1) {
		this->set_data_from_ints(w,1,3,1,0,1,0); //JK-??? TODO, all "set_data" function overwrites the old data... --> "add_data_" needed
		this->set_data_from_ints(w,1,3,1,-1,1,-1);
		this->set_data_from_ints(w,1,3,1,-1,1,+1);
		this->set_data_from_ints(w,1,3,1,+1,1,-1);
		this->set_data_from_ints(w,1,3,1,+1,1,+1);
	}
	else {
		this->set_data_from_ints(w,1,1,3,0,0,1);
	}

	delete w;
	cout << "Created 1d central difference filter" << endl;
}


filter_second_derivative_1d::filter_second_derivative_1d(int dir)
{
	int *w = new int[3];
	w[0]= 1;
	w[1]= -2;
	w[2]= 1;
	
	if (dir==0) {this->set_data_from_ints(w,3,1,1,1,0,0);}
	else if (dir==1) {this->set_data_from_ints(w,1,3,1,0,1,0);}
	else {this->set_data_from_ints(w,1,1,3,0,0,1);}

	delete w;
	cout << "Created 1d second derivative filter" << endl;
}

filter_LoG_1d::filter_LoG_1d(int dir, float sigma_in_pixels, unsigned int num_elem)
{
	float *w = new float[num_elem];
	float center_elem = float(num_elem-1)/2.0;
	for(int i=0;i<num_elem;i++){
		w[i] = pt_LoG( float(i)-center_elem, sigma_in_pixels); // sigma is the distance from the center to the zero crossing
		cout<<"w[i]="<<w[i]<<endl;
	}
	
	if (dir==0) {this->set_data_from_floats(w,num_elem,1,1,center_elem,0,0);}
	else if (dir==1) {this->set_data_from_floats(w,1,num_elem,1,0,center_elem,0);}
	else {this->set_data_from_floats(w,1,1,num_elem,0,0,center_elem);}

	delete w;
	cout << "Created filter_LoG_1d" << endl;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------


filter_gaussian::filter_gaussian(int size, int dir, float std_dev, int center)
{	if (size%2==0) {cout << "WARNING: Creating gaussian filter with kernel of even size";}
	if (std_dev<=0) {std_dev=sqrt(float(2))*float(size)/float(5);}
	if (center<-1000) {center=size/2;}

	float *w = new float[size];
	float sum=0;
	gaussian gauss = gaussian(1,center,std_dev);
	for (int i=0; i<size; i++) {
		//w[i]=exp( -pow(i-center, 2)/pow(std_dev,2) );
		w[i]=gauss.evaluate_at(i);
		sum+=w[i];
	}
	for (int i=0; i<size; i++) {w[i]/=sum;	} // Normalize
	if (dir==0) {this->set_data_from_floats(w,size,1,1,center,0,0);}
	else if (dir==1) {this->set_data_from_floats(w,1,size,1,0,center,0);}
	else {this->set_data_from_floats(w,1,1,size,0,0,center);}
	delete w;
	cout << "Created gaussian filter, kernel size " << size << ", direction " << dir << ", standard deviation " << std_dev << endl;
}

filter_mean::filter_mean(int nx, int ny, int nz, int xcenter, int ycenter, int zcenter)
{
	if (xcenter<-1000) {xcenter=nx/2;}
	if (ycenter<-1000) {ycenter=ny/2;}
	if (zcenter<-1000) {zcenter=nz/2;}
	int num=nx*ny*nz;
	float *w = new float[num];
	for (int i=0; i<num; i++) {
		w[i]=(float)1/num;
	}
	set_data_from_floats(w,nx,ny,nz,xcenter,ycenter,zcenter);
	delete w;
	cout << "Created mean " << nx << "x" << ny << "x" << nz << " filter." << endl;
}


filter_square_wave_1d::filter_square_wave_1d(int dir, int num_high1, int num_low, int num_high2)
{
	int num = num_high1 + num_low + num_high2;
	float scale = float(num_high1 + num_high2)/float(num_low);

	float *w = new float[num];
	for (int i=0; i<num; i++) {
		if(i<num_high1){
			w[i]=1.0;
		}
		else if(i >= num_high1 && i < num_high1+num_low){
			w[i]=-1.0*scale;
		}
		else if(i >= (num_high1+num_low)){
			w[i]=1.0;
		}
		cout<<"w[i]="<<w[i]<<endl;
	}

	if(dir==0){
		set_data_from_floats(w,num,1,1,num/2.0,0,0);
	}
	else if(dir==1){
		set_data_from_floats(w,1,num,1,0,num/2.0,0);
	}
	else{
		set_data_from_floats(w,1,1,num,0,0,num/2.0);
	}
}



#endif

