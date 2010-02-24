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

image_binary<3>* brainops::get_CSF(vector<image_scalar<unsigned short,3>* > brain){

	cout << "Removing BG... ";
	remove_bg(brain);
	cout << "done" << endl;
	
	int t1_l = brain[0]->get_histogram()->get_intensity_at_histogram_lower_percentile(0.80,true);
	int t2_h = brain[1]->get_histogram()->get_intensity_at_histogram_higher_percentile(0.20,true);
	
	/*image_binary<3> *bin1 = brain[0]->threshold(1,t1_l);
	image_binary<3> *bin2 = brain[1]->threshold(t2_h,brain[1]->get_max());
	image_binary<3> *csf = new image_binary<3>(bin1);*/

	unsigned short min[] = {brain[0]->get_histogram()->get_intensity_at_histogram_lower_percentile(0.02,true), brain[1]->get_histogram()->get_intensity_at_histogram_lower_percentile(0.02,true)};
	unsigned short max[] = {brain[0]->get_histogram()->get_intensity_at_histogram_higher_percentile(0.02,false), brain[1]->get_histogram()->get_intensity_at_histogram_higher_percentile(0.02,false)};
	cout << "T1: " << min[0] << "  " << ((max[0]-min[0])*0.33)+min[0] << endl;
	cout << "T2: " << ((max[1]-min[1])*0.75)+min[1] << "  " << brain[1]->get_max() << endl;

	image_binary<3> *bin1 = brain[0]->threshold(0,((max[0]-min[0])*0.43)+min[0]);//0.33
	//image_binary<3> *bin2 = brain[1]->threshold(((max[1]-min[1])*0.80)+min[1],brain[1]->get_max());//0.80
	//image_binary<3> *csf = new image_binary<3>(bin1);

	image_scalar<unsigned short,3>* csf = new image_scalar<unsigned short,3>(brain[0]);
	image_scalar<unsigned short,3>* wm = new image_scalar<unsigned short,3>(brain[0]);
	image_scalar<unsigned short,3>* gm = new image_scalar<unsigned short,3>(brain[0]);

//	csf->combine(bin2,COMB_MIN);
	cout << "done";
	bin1->invert();

	bin1->erode_3D(5);
	bin1->largest_object_3D();
	bin1->dilate_3D(6);

	//     csf wm gm
	// T1
	// T2
	int thresh[2][3];

		thresh[0][0] = ((max[0]-min[0])*0.15)+min[0];
		thresh[0][1] = ((max[0]-min[0])*0.75)+min[0];
		thresh[0][2] = ((max[0]-min[0])*0.40)+min[0];
		thresh[1][0] = ((max[1]-min[1])*0.90)+min[1];
		thresh[1][1] = ((max[1]-min[1])*0.40)+min[1];
		thresh[1][2] = ((max[1]-min[1])*0.70)+min[1];
	datamanagement.add(bin1);
	//datamanagement.add(bin2);
	//datamanagement.add(csf);
	//vector<Vector3Dint> seeds;

	for(int x = 0; x < brain[0]->get_size_by_dim(0); x++){
		for(int y = 0; y < brain[0]->get_size_by_dim(1); y++){
			for(int z = 0; z < brain[0]->get_size_by_dim(2); z++){
				csf->set_voxel(x,y,z,abs(brain[0]->get_voxel(x,y,z)-thresh[0][0]) + abs(brain[1]->get_voxel(x,y,z)-thresh[1][0]));
				wm->set_voxel(x,y,z,abs(brain[0]->get_voxel(x,y,z)-thresh[0][1]) + abs(brain[1]->get_voxel(x,y,z)-thresh[1][1]));
				gm->set_voxel(x,y,z,abs(brain[0]->get_voxel(x,y,z)-thresh[0][2]) + abs(brain[1]->get_voxel(x,y,z)-thresh[1][2]));
			}
		}
	}
	csf->invert();
	csf->mask_out(bin1);
	wm->invert();
	wm->mask_out(bin1);
	gm->invert();
	gm->mask_out(bin1);
	datamanagement.add(csf, "CSF");
	datamanagement.add(wm,"WM");
	datamanagement.add(gm, "GM");
	return bin1;

}

void brainops::bridge_burner(vector<image_scalar<unsigned short,3>* > brain, float t_min, float t_max, float t_grad, float p, float g){
	
	//Place a cube inside brain to find WM
	//TODO
	int S_w = 320;
	//Segment WM/GM and removing csf,fat and air. Still some stuff remains
	image_binary<3> *a = brain.at(0)->threshold(S_w*t_min,S_w*t_max);

	image_binary<3> *b = new image_binary<3>(a);
	b->erode_3D(6);
	
	image_binary<3> *c = new image_binary<3>(b);
	c->largest_object_3D();

	image_binary<3> *d = new image_binary<3>(c);
	d->dilate_3D(6);
	d->fill_holes_3D();
	image_scalar<unsigned short,3>* masked = new image_scalar<unsigned short,3>(brain.at(0));
	masked->mask_out(d);
	datamanagement.add(a,"threshold",true);
	datamanagement.add(b,"erode",true);
	datamanagement.add(c,"largest",true);
	datamanagement.add(d,"dilate",true);
	datamanagement.add(masked,"brain",true);

	image_scalar<unsigned short,3>* temp = new image_scalar<unsigned short,3>(masked);
	temp->fill(520);
	temp->mask_out(d);
	temp->combine(masked,COMB_SUB);
	datamanagement.add(temp,"filter",true);
}
void brainops::morph(vector<image_scalar<unsigned short,3>* > brain, float t_min, float t_max){
	int S_w = 320;
	//Segment WM/GM and removing csf,fat and air. Still some stuff remains
	image_binary<3> *a = brain.at(0)->threshold(S_w*t_min,S_w*t_max);

	image_binary<3> *b = new image_binary<3>(a);
	b->erode_3D(6);
	
	image_binary<3> *c = new image_binary<3>(b);
	c->largest_object_3D();
	double mean = 0;
	int count = 1;
	for(int x = 0; x < brain.at(0)->get_size_by_dim(0); x++){
		for(int y = 0; y < brain.at(0)->get_size_by_dim(1); y++){
			for(int z = 0; z < brain.at(0)->get_size_by_dim(2); z++){
				if(c->get_voxel(x,y,z)==1){
					mean = mean + ((brain.at(0)->get_voxel(x,y,z)-mean))/count;
					count++;
				}
			}
		}
	}
	cout << "mean for WM: " << mean << endl;

	image_binary<3> *d = new image_binary<3>(c);
	d->dilate_3D(6);
	d->fill_holes_3D();
	image_scalar<unsigned short,3>* masked = new image_scalar<unsigned short,3>(brain.at(0));
	masked->mask_out(d);

	datamanagement.add(a,"threshold",true);
	datamanagement.add(b,"erode",true);
	datamanagement.add(c,"largest",true);
	datamanagement.add(d,"dilate",true);
	datamanagement.add(masked,"brain",true);

	image_scalar<int,3>* temp = new image_scalar<int,3>(masked, false);
	temp->fill(S_w);
	temp->mask_out(d);
	temp->combine(masked,COMB_SUB);
	datamanagement.add(temp,"filter",true);

	mean = 0;
	count = 1;
	for(int x = 0; x < brain.at(0)->get_size_by_dim(0); x++){
		for(int y = 0; y < brain.at(0)->get_size_by_dim(1); y++){
			for(int z = 0; z < brain.at(0)->get_size_by_dim(2); z++){
				if(temp->get_voxel(x,y,z) >= 75){
					mean = mean + ((brain.at(0)->get_voxel(x,y,z)-mean))/count;
					count++;
					temp->set_voxel(x,y,z,100);
				}else if(temp->get_voxel(x,y,z) != 0){
					temp->set_voxel(x,y,z,50);
				}
			}
		}
	}
	cout << "mean for GM: " << mean << endl;
	image_scalar<float,3>* dist;
	dist = d->distance_3D();
	datamanagement.add(dist,"distance",true);

}