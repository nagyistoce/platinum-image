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