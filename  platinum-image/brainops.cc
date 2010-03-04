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
image_binary<3>* brainops::get_ventricles(vector<image_scalar<unsigned short,3>* > brain){
	image_binary<3> *csf = get_CSF(brain);
	csf->erode_3D();
	csf->largest_objects_3D(2);
	//calculate CSF cog. Find largest object in that area maybe?
	//Maybe the ventricles are the most homogenous and largest csf object in the image. This can be used
	//
	return csf;
}
image_binary<3>* brainops::get_CSF(vector<image_scalar<unsigned short,3>* > brain){

	unsigned short min[3];
	unsigned short max[3];
	unsigned short temp;
	for(int i = 0; i<brain.size(); i++){
		min[i] = brain[i]->get_min();
		max[i] = brain[i]->get_histogram()->get_intensity_at_histogram_higher_percentile(0.01,false);
		temp = brain[i]->get_max();
		brain[i]->map_values(max[i],brain[i]->get_max(),max[i]);
		cout << "min: " << min[i] << "   max: " << max[i] << "  real max: " << temp << endl;
		datamanagement.add(brain[i]);
	}

	unsigned short prob_mat[5][3];
	prob_mat[0][0] = 0.02*max[0]-min[0];	prob_mat[0][1] = 0.02*max[1]-min[1];	prob_mat[0][2] = 0.02*max[2]-min[2];
	prob_mat[1][0] = 0.75*max[0]-min[0];	prob_mat[1][1] = 0.64*max[1]-min[1];	prob_mat[1][2] = 0.46*max[2]-min[2];
	prob_mat[2][0] = 0.55*max[0]-min[0];	prob_mat[2][1] = 0.70*max[1]-min[1];	prob_mat[2][2] = 0.55*max[2]-min[2];
	prob_mat[3][0] = 0.15*max[0]-min[0];	prob_mat[3][1] = 0.76*max[1]-min[1];	prob_mat[3][2] = 1.00*max[2]-min[2];
	prob_mat[4][0] = 1.00*max[0]-min[0];	prob_mat[4][1] = 1.00*max[1]-min[1];	prob_mat[4][2] = 1.00*max[2]-min[2];

	float weight[5][3];
	//T1					PD						T2
	weight[0][0] = 0.33;	weight[0][1] = 0.33;	weight[0][2] = 0.33; // bg
	weight[1][0] = 0.50;	weight[1][1] = 0.25;	weight[1][2] = 0.25; // wm
	weight[2][0] = 0.70;	weight[2][1] = 0.15;	weight[2][2] = 0.15; // gm
	weight[3][0] = 0.70;	weight[3][1] = 0.15;	weight[3][2] = 0.15; // csf
	weight[4][0] = 0.34;	weight[4][1] = 0.33;	weight[4][2] = 0.33; // fat

	//bg,wm,gm,csf,fat
	vector<image_scalar<float,3>* > vec;
	for(int i = 0; i < 5; i++){
		vec.push_back(new image_scalar<float,3>(brain[0],false));
	}
	image_binary<3> *summery = new image_binary<3>(brain[0],false);
	image_binary<3> *bg = new image_binary<3>(brain[0],false);

	//unsigned short color[] = {0, 100, 200, 0, 50};
	unsigned short color[] = {0, 100, 200, 0, 0};
	for(int x = 0; x<brain[0]->get_size_by_dim(0); x++){
		for(int y = 0; y<brain[0]->get_size_by_dim(1); y++){
			for(int z = 0; z<brain[0]->get_size_by_dim(2); z++){
				unsigned short t1, t2,pd;
				t1 = brain[0]->get_voxel(x,y,z);
				pd = brain[1]->get_voxel(x,y,z);
				t2 = brain[2]->get_voxel(x,y,z);
				float m = 0;
				int ind;
				float val;
				for(int i = 0; i < vec.size(); i++){
					val =	(max[0]-abs(prob_mat[i][0]-t1))*weight[i][0] +
							(max[1]-abs(prob_mat[i][1]-pd))*weight[i][1] + 
							(max[2]-abs(prob_mat[i][2]-t2))*weight[i][2];
					vec[i]->set_voxel(x,y,z,val);
					if (val > m){
						ind = i;
						m = val;
					}
				}
				
				if(ind == 3){
					summery->set_voxel(x,y,z,1);
				}else
					summery->set_voxel(x,y,z,0);
				if(ind == 0){
					bg->set_voxel(x,y,z,1);
				}else{
					bg->set_voxel(x,y,z,0);
				}
			}
		}
	}
	datamanagement.add(vec[3],"prob csf",true);
	summery->combine(bg,COMB_MAX);
	bg->largest_object_2D();
	summery->combine(bg,COMB_SUB);
	return summery;

}

image_scalar<float,3>* brainops::get_csf_probability_map(vector<image_scalar<unsigned short,3>* > brain){
	unsigned short min[3];
	unsigned short max[3];
	for(int i = 0; i<brain.size(); i++){
		min[i] = brain[i]->get_min();
		max[i] = brain[i]->get_histogram()->get_intensity_at_histogram_higher_percentile(0.01,false);
		brain[i]->map_values(max[i],brain[i]->get_max(),max[i]);
	}

	unsigned short prob_mat[3] = {0.15*max[0]-min[0], 0.76*max[1]-min[1], 1.00*max[2]-min[2]};
	float weight[3] = {0.70, 0.15, 0.15};

	image_scalar<float,3>* csf = new image_scalar<float,3>(brain[0],false);

	for(int x = 0; x<brain[0]->get_size_by_dim(0); x++){
		for(int y = 0; y<brain[0]->get_size_by_dim(1); y++){
			for(int z = 0; z<brain[0]->get_size_by_dim(2); z++){
				unsigned short t1, t2,pd;
				t1 = brain[0]->get_voxel(x,y,z);
				pd = brain[1]->get_voxel(x,y,z);
				t2 = brain[2]->get_voxel(x,y,z);
				float m = 0;
				int ind;
				float val;
					val =	(max[0]-abs(prob_mat[0]-t1))*weight[0] +
							(max[1]-abs(prob_mat[1]-pd))*weight[1] + 
							(max[2]-abs(prob_mat[2]-t2))*weight[2];
					csf->set_voxel(x,y,z,val);
			}
		}
	}
	csf->scale(0,1);
	datamanagement.add(csf,"prob csf",true);
	return csf;	
}
image_scalar<float,3>* brainops::get_ventricle_distance_map(image_scalar<float,3>* csf_prob){
	image_scalar<float,3>* ventricle;// =  new image_scalar<float,3>(csf_prob);
	image_binary<3>* cog_image =  new image_binary<3>(csf_prob,false);

	Vector3D cog = csf_prob->get_center_of_gravity_from_range(0.85);
	cout << "COG: " <<  cog[0] << " " << cog[1] << " " << cog[2] << endl;
	//cog_image->fill_region_3D(cog[0]-10,cog[1]-10,cog[2]-10,20,20,20,1);
	//datamanagement.add(cog_image,"COG",true);
	cog_image->set_voxel(cog,1);
	cog_image->invert();
	ventricle = cog_image->distance_3D(true);
	//image_binary<3> *temp = csf_prob->threshold(0.85);
	//temp->combine(cog_image,COMB_MIN);
	//temp->erode_2D();
	//image_label<3>* hmm = temp->label_connected_objects_2D();
	//temp->largest_object_2D();
	//datamanagement.add(hmm,"Labeled",true);
	

	delete cog_image;
	datamanagement.add(ventricle,"dist",true);
	return ventricle;
}

image_binary<3>* brainops::get_grown_shell(image_scalar<float,3>* csf, image_scalar<float,3>* distance_from_ventricles){
	image_binary<3>* shell =  csf->threshold(0.85);
	datamanagement.add(shell,"Shell",true);

	return shell;
}
float brainops::chance_of_error(image_binary<3>* shell){
	return 0.0;
}
void brainops::extract_brain_with_shell(vector<image_scalar<unsigned short,3>* > brain, image_binary<3>* shell){
	for(int i = 0; i < brain.size(); i++){
		brain[i]->mask_out(shell);
	}
}