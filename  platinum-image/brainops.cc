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

#include "brainops.h"
extern datamanager datamanagement;

void brainops::remove_bg(vector<image_scalar<unsigned short,3>* > brain){
	//Remove background according to article on region growing scull stripping
	for(int i = 0; i < brain.size(); i++){ //Zeros the background
		int lower = brain[i]->get_histogram()->get_intensity_at_histogram_lower_percentile(0.02,false);
		int higher = brain[i]->get_histogram()->get_intensity_at_histogram_higher_percentile(0.02,false);
		int val = 0.1*(higher-lower) + lower;
		brain[i]->map_values(1,val,0);
	}
}
void brainops::get_val_from_cube(image_integer<unsigned short,3> *T1, int x, int y, int z, float &mean, float &var){
	float val[125];
	int count = 0;
	for(int i = x-2; i <= x+2; i++){
		for(int j = y-2; j <= y+2; j++){
			for(int k = z-2; k <= z+2; k++){
				val[count] = T1->get_voxel(i,j,k);
				count++;
			}
		}
	}

	float sum = 0;
	float variance = 0;
	mean = 0;
	for(int i = 0; i < 125; i++){
		sum+=val[i];
		variance += val[i]*val[i];
		mean = mean +  (val[i]-mean)/(i+1.0);
	}
	//mean = sum;
	var = (variance - sum*mean)/125;

}
image_binary<3>* brainops::fit_WM_cube_and_grow_accordingly(image_integer<unsigned short,3> *T1, int &thresh, Vector3D &p , float q1, float q2){
//	image_binary<3> *mask;
	Vector3D pos = create_Vector3Dint(0,0,0);
	float max = 0;
	float mean = 0;
	float var = 0;
	float final_mean;

	for(int x = 2; x < T1->get_size_by_dim(0)-2; x+=5){
		for(int y = 2; y < T1->get_size_by_dim(1)-2; y+=5){
			for(int z = 2; z < T1->get_size_by_dim(2)-2; z+=5){
				get_val_from_cube(T1, x, y, z, mean, var);
				float val = mean/var;
				if(val > max){
					pos[0] = x;
					pos[1] = y;
					pos[2] = z;
					max = val;
					final_mean = mean;
				}
			}
		}
	}
	p = pos;
	std::stack<Vector3Dint> seed;
	for(int i = pos[0]-2; i <= pos[0]+2; i++){
		for(int j = pos[1]-2; j <= pos[1]+2; j++){
			for(int k = pos[2]-2; k <= pos[2]+2; k++){
				//T1->set_voxel(i,j,k,500);
				seed.push(create_Vector3D(i,j,k));
			}
		}
	}
	image_binary<3> *WM = T1->region_grow_robust_3D(seed,final_mean*q1,final_mean*q2);
	cout << "thresh: " <<  final_mean  << " " << max << endl;
	thresh = final_mean;
	//WM->largest_object_3D(); //Hopefully this will not be needed if the images are of "good" quality
//	WM->dilate_3D();
//	WM->fill_holes_3D();

	//DANGER!!!
	//T1->map_values(final_mean*q1,std::numeric_limits<unsigned short>::max(),0);

	return WM;
}