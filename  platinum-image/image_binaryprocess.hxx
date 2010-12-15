//////////////////////////////////////////////////////////////////////////
//
//  Binary process
//
//  Task-specific processing of binary images
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

// Loops over the dimension given by direction and performs a slice-wise hole filling

template <int IMAGEDIM>
image_binary<3>* image_binary<IMAGEDIM>::convex_hull_2D(int dir)
{
	image_binary<IMAGEDIM>* res = new image_binary<IMAGEDIM>(this);
	for (int w=0; w<this->get_size_by_dim(dir); w++)
		convex_hull_in_slice_2D(res, dir, w);
	return res;
}

template <int IMAGEDIM>
image_binary<3>* image_binary<IMAGEDIM>::convex_hull_objectwise_2D(int dir)
{
	image_label<3>* label_image=this->label_connected_objects_2D(dir);
	for (int w=0; w<this->get_size_by_dim(dir); w++)
		convex_hull_objectwise_in_slice_2D(label_image, dir, w);
	image_binary<3>* res=label_image->threshold(1);
	delete label_image;
	return res;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::convex_hull_in_slice_2D(image_binary<3>* image, int dir, int slice )
{
	int usize=image->get_size_by_dim_and_dir(0,dir);
	int vsize=image->get_size_by_dim_and_dir(1,dir);
	// put object voxels in vector<Vector2D>
	vector<Vector2D> points;
	for (int u=0; u<usize; u++) {
		for (int v=0; v<vsize; v++) {
			if (image->get_voxel_by_dir(u,v,slice,dir))
				points.push_back(create_Vector2D(u,v));
		}
	}
	// if 3 or more object voxels, calculate convex hull
	if (points.size()>2) {
		vector<Vector2D> hull=get_convex_hull_2D(points);
		// get line2D:s constituting convex hull
		vector<line2D> hull_lines;
		vector<Vector2D>::iterator i;
        for(i=hull.begin(); i!=hull.end()-1; i++)
			hull_lines.push_back(line2D(*i, *(i+1)));
		hull_lines.push_back(line2D(hull.back(), hull.front()));
		if (hull.size()==2)
		{
			hull_lines.push_back(line2D(hull[0].GetElement(0), hull[0].GetElement(1), hull[0].GetElement(0)+hull_lines[0].get_direction().GetElement(1), hull[0].GetElement(1)-hull_lines[0].get_direction().GetElement(0)));
			hull_lines.push_back(line2D(hull[1].GetElement(0), hull[1].GetElement(1), hull[1].GetElement(0)+hull_lines[1].get_direction().GetElement(1), hull[1].GetElement(1)-hull_lines[1].get_direction().GetElement(0)));
		}

		// voxels "left" of all lines are inside convex hull
		vector<line2D>::iterator j;
		bool inside_hull;
		for (int u=0; u<usize; u++) {
			for (int v=0; v<vsize; v++) {
				inside_hull=true;
				for(j=hull_lines.begin(); j!=hull_lines.end(); j++)
					if ((*j).is_point_right_of_line(u,v)) {
						inside_hull=false;
						break;
					}
				if (inside_hull)
					image->set_voxel_by_dir(u,v,slice,1,dir);
			}
		}
	}
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::convex_hull_objectwise_in_slice_2D(image_label<3>* image, int dir, int slice)
{
	int usize=image->get_size_by_dim_and_dir(0,dir);
	int vsize=image->get_size_by_dim_and_dir(1,dir);
	// for each object, put object voxels in a vector<Vector2D>
	vector<vector<Vector2D> > points;
	IMGLABELTYPE p;
	for (int u=0; u<usize; u++) {
		for (int v=0; v<vsize; v++) {
		p=image->get_voxel_by_dir(u,v,slice,dir);
			if (p>0) {
				while (points.size()<p)
					points.push_back(vector<Vector2D>() );
				points[p-1].push_back(create_Vector2D(u,v));
			}
		}
	}
	int num_objects=points.size();
	vector<vector<Vector2D> > hulls(num_objects);
	vector<vector<line2D> > hull_lines(num_objects);
	vector<Vector2D>::iterator i;
	for (int k=0; k<num_objects; k++) {
		// for each object, if 3 or more object voxels, calculate convex hull
		if (points[k].size()>2) {
			hulls[k]=get_convex_hull_2D(points[k]);
			// get line2D:s constituting convex hull
			for(i=hulls[k].begin(); i!=hulls[k].end()-1; i++)
				hull_lines[k].push_back(line2D(*i, *(i+1)));
			hull_lines[k].push_back(line2D(hulls[k].back(), hulls[k].front()));	
			if (hulls[k].size()==2)
			{
				hull_lines[k].push_back(line2D(hulls[k][0].GetElement(0), hulls[k][0].GetElement(1), hulls[k][0].GetElement(0)+hull_lines[k][0].get_direction().GetElement(1), hulls[k][0].GetElement(1)-hull_lines[k][0].get_direction().GetElement(0)));
				hull_lines[k].push_back(line2D(hulls[k][1].GetElement(0), hulls[k][1].GetElement(1), hulls[k][1].GetElement(0)+hull_lines[k][1].get_direction().GetElement(1), hulls[k][1].GetElement(1)-hull_lines[k][1].get_direction().GetElement(0)));
			}
		}
	}
	// for each object, voxels "left" of all object lines are inside convex hull
	vector<line2D>::iterator j;
	vector<bool> inside_hull(num_objects);
	for (int u=0; u<usize; u++) {
		for (int v=0; v<vsize; v++) {
			for (int k=0; k<num_objects; k++) {
				if(hulls[k].size()<3) {
					inside_hull[k]=false;
				} else {
				inside_hull[k]=true;
				for(j=hull_lines[k].begin(); j!=hull_lines[k].end(); j++)
					if ((*j).is_point_right_of_line(u,v)) {
						inside_hull[k]=false;
						break;
					}
				}
				if (inside_hull[k])
					image->set_voxel_by_dir(u,v,slice,k+1,dir);
			}
		}
	}
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::fill_holes_2D(int direction, IMGBINARYTYPE object_value) //assumes object not connected to borders... (use "expand_borders")
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<int, IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p!=object_value)//Note: we want to label bkg-objects in order to remove holes
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		int* changes=new int[number_of_objects];
		int i;
		for(i=0; i<number_of_objects; i++)
			changes[i]=i;
		//merge labels
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;						
					}
				}
			}
		int* lut=new int[number_of_objects];
		memset(lut, 0, sizeof(int)*number_of_objects);
		new_label=1;
		for(i=1; i<number_of_objects; i++)
			{
			label=i;
			while(label!=changes[label])
				label=changes[label];
			if(label==i)
				{
				lut[i]=new_label;
				new_label++;
				}
			else
				lut[i]=lut[label];
			}
		//set all labels !=1 -> obj
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(lut[label]==1)//First label is background
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				else //all other labels are object
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				}
			}
		delete[] changes;
		delete[] lut;
		}	

	//this->image_has_changed();
	//label_image->save_to_file("D:/Joel/TMP/SIM_0b_binary_body_label_image.vtk");
	delete label_image;
	}
	
template <int IMAGEDIM>
image_label<3>* image_binary<IMAGEDIM>::label_connected_objects_2D(int direction, IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0, direction);
	max_v=this->get_size_by_dim_and_dir(1, direction);
	max_w=this->get_size_by_dim_and_dir(2, direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;
	changes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)	{
					up=(v>0)? label_image->get_voxel_by_dir(u,v-1,w,direction) : 0;
					left=(u>0)? label_image->get_voxel_by_dir(u-1,v,w,direction) : 0;
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(up>new_label)
						changes[up]=new_label;
					if(left>new_label)
						changes[left]=new_label;
					if(new_label==number_of_objects) {
						changes.push_back(number_of_objects);
						number_of_objects++;	
					}
					label_image->set_voxel_by_dir(u,v,w,new_label,direction);
				}
				else
					label_image->set_voxel_by_dir(u,v,w,0,direction);
			}
		}
	}
	
	// calculate look-up-table
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
		}
		else
			lut[i]=lut[label];
	}
	
    // set new labels from look-up table
	image_label<IMAGEDIM> * label_image_label = new image_label<IMAGEDIM> (label_image);
	int max_label=0;
	int new_max_label;
	for(w=0; w<max_w; w++) {
		new_max_label=max_label;
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel_by_dir(u,v,w,direction);
				if (lut[label]>new_max_label) 
					new_max_label=lut[label];
				if (label>0)
					label_image_label->set_voxel_by_dir(u,v,w,lut[label]-max_label,direction);
			}
		}
		max_label=new_max_label;
	}
	delete[] lut;
	delete label_image;
	return label_image_label;
}

template <int IMAGEDIM>
image_integer<unsigned long, 3>* image_binary<IMAGEDIM>::label_connected_objects_with_area_2D(int direction, IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
    image_integer<unsigned long,IMAGEDIM> * label_image = new image_integer<unsigned long,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;	//changes[a] = b //--> replace index a with b... Note that changes[1] is always = 1...
	changes.push_back(0);
	vector<int> sizes;
	sizes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)	{
					up=(v>0)? label_image->get_voxel_by_dir(u,v-1,w,direction) : 0;		//read up and left
					left=(u>0)? label_image->get_voxel_by_dir(u-1,v,w,direction) : 0;
					while(up!=changes[up])												//update up and left according to already known changes
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;		//start off by guessing new_label to be "number_of_objects" 
					if(up>0 && up<new_label)			//update new_label if "up"/"left" already belongs to an object with "lower" label
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(up>new_label)					//i.e. if left was "lower" than up...
						changes[up]=new_label;
					if(left>new_label)					//i.e. if up was "lower" than left...
						changes[left]=new_label;
					if(new_label==number_of_objects) {			//i.e. a "new" object is found
						changes.push_back(number_of_objects);
						sizes.push_back(0);
						number_of_objects++;	
					}
					label_image->set_voxel_by_dir(u,v,w,new_label,direction);
					sizes[new_label]++;
				}
				else
					label_image->set_voxel_by_dir(u,v,w,0,direction);
			}
		}
	}
	
	// calculate look-up-table and sizes
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	vector<int> tot_sizes;
	tot_sizes.push_back(0);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
			tot_sizes.push_back(0);
		}
		else
			lut[i]=lut[label];
		tot_sizes[lut[i]]+=sizes[i];
	}
	
    // set new labels from look-up table and sizes
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel_by_dir(u,v,w,direction);
				if (label>0)
					label_image->set_voxel_by_dir(u,v,w,tot_sizes[lut[label]],direction);
			}
		}
	}
	delete[] lut;
	return label_image;
}

template <int IMAGEDIM>
image_integer<unsigned long, 3>* image_binary<IMAGEDIM>::label_connected_objects_with_perimeter_2D(int direction, IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left,right,down;//neighbour labels 
    image_integer<unsigned long,IMAGEDIM> * label_image = new image_integer<unsigned long,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;	//changes[a] = b //--> replace index a with b... Note that changes[1] is always = 1...
	changes.push_back(0);
	vector<float> sizes;
	sizes.push_back(0);
	int num_bg_pix=0;
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)	{
					up=(v>0)? label_image->get_voxel_by_dir(u,v-1,w,direction) : 0;		//read up and left
					left=(u>0)? label_image->get_voxel_by_dir(u-1,v,w,direction) : 0;
					while(up!=changes[up])												//update up and left according to already known changes
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;		//start off by guessing new_label to be "number_of_objects" 
					if(up>0 && up<new_label)			//update new_label if "up"/"left" already belongs to an object with "lower" label
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(up>new_label)					//i.e. if left was "lower" than up...
						changes[up]=new_label;
					if(left>new_label)					//i.e. if up was "lower" than left...
						changes[left]=new_label;
					if(new_label==number_of_objects) {			//i.e. a "new" object is found
						changes.push_back(number_of_objects);
						sizes.push_back(0);
						number_of_objects++;	
					}
					label_image->set_voxel_by_dir(u,v,w,new_label,direction);

					down=(v<max_v-1)? label_image->get_voxel_by_dir(u,v+1,w,direction) : 0;		//read down
					right=(u<max_u-1)? label_image->get_voxel_by_dir(u+1,v,w,direction) : 0;		//read right
					
					num_bg_pix=0;
					if(up==0)
						num_bg_pix++;
					if(left==0)
						num_bg_pix++;
					if(down==0)
						num_bg_pix++;
					if(right==0)
						num_bg_pix++;

					if( num_bg_pix == 2 ){
						sizes[new_label] += 1.414214;
					}else if( num_bg_pix >0 ){
						sizes[new_label]++;					
					}
//					if( (up==0)||(left==0)||(down==0)||(right==0) ){
//						sizes[new_label]++;
//					}
					/*
					if(up==0)
						sizes[new_label]++;
					if(left==0)
						sizes[new_label]++;
					if(down==0)
						sizes[new_label]++;
					if(right==0)
						sizes[new_label]++;
					*/
				}
				else
					label_image->set_voxel_by_dir(u,v,w,0,direction);
			}
		}
	}
	
	// calculate look-up-table and sizes
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	vector<int> tot_sizes;
	tot_sizes.push_back(0);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
			tot_sizes.push_back(0);
		}
		else
			lut[i]=lut[label];
		tot_sizes[lut[i]]+=sizes[i];
	}
	
    // set new labels from look-up table and sizes
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel_by_dir(u,v,w,direction);
				if (label>0)
					label_image->set_voxel_by_dir(u,v,w,tot_sizes[lut[label]],direction);
			}
		}
	}
	delete[] lut;
	return label_image;
}

template <int IMAGEDIM>
image_scalar<float, 3>* image_binary<IMAGEDIM>::label_connected_objects_with_p2a_2D(int direction, IMGBINARYTYPE object_value)
{
	image_integer<unsigned long,3> *perim = this->label_connected_objects_with_perimeter_2D(direction,object_value);
	image_integer<unsigned long,3> *area = this->label_connected_objects_with_area_2D(direction,object_value);

	image_scalar<float,3> *p2a = new image_scalar<float,3>(perim);
	image_scalar<float,3> *area2 = new image_scalar<float,3>(area);
	p2a->combine(p2a,COMB_MULT);
	p2a->combine(area2,COMB_DIV);
	delete perim;
	delete area;
	delete area2;
	return p2a;
}


template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_object_2D(int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		int* changes=new int[number_of_objects];
		int i;
		for(i=0; i<number_of_objects; i++)
			changes[i]=i;
		int* sizes=new int[number_of_objects];
		memset(sizes, 0, sizeof(int)*number_of_objects);
		//merge labels
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;
					sizes[new_label]++;						
					}
				}
			}
		int* lut=new int[number_of_objects];
		memset(lut, 0, sizeof(int)*number_of_objects);
		int* tot_sizes=new int[number_of_objects];
		memset(tot_sizes, 0, sizeof(int)*number_of_objects);
		new_label=1;
		for(i=1; i<number_of_objects; i++)
			{
			label=i;
			while(label!=changes[label])
				label=changes[label];
			if(label==i)
				{
				lut[i]=new_label;
				new_label++;
				}
			else
				lut[i]=lut[label];
			tot_sizes[lut[i]]+=sizes[i];
			}
		int max_size=0;
		int max_label=1;
		for(i=1; i<new_label; i++)
			{
			if(tot_sizes[i]>max_size)
				{
				max_size=tot_sizes[i];
				max_label=i;
				}
			}
		//set all labels !=1 -> obj
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(lut[label]==max_label)//First label is background
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				else //all other labels are object
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				}
			}
		delete[] changes;
		delete[] sizes;
		delete[] lut;
		delete[] tot_sizes;
		}

	//this->image_has_changed();
	delete label_image;				
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_objects_2D(int num_objects, IMGBINARYTYPE object_value)
{
	image_binary<3> *res = new image_binary<3>(this,0);
	res->fill(!object_value);

	image_binary<3> *tmp;
	for(int i=0;i<num_objects;i++){
	//	cout<<"i="<<i<<endl;
		tmp = new image_binary<3>(this);
		tmp->mask_out(res,!object_value);	//remove already selected objects...
		tmp->largest_object_2D();
		res->combine(tmp,COMB_MAX);
	//	cout<<"res_vol="<<res->get_number_of_voxels_with_value(object_value)<<endl;
		delete tmp;
	}

	copy_data(res,this);
	delete res;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::threshold_size_2D(int min_size, int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		int* changes=new int[number_of_objects];
		int i;
		for(i=0; i<number_of_objects; i++)
			changes[i]=i;
		int* sizes=new int[number_of_objects];
		memset(sizes, 0, sizeof(int)*number_of_objects);
		//merge labels
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;
					sizes[new_label]++;						
					}
				}
			}
		int* lut=new int[number_of_objects];
		memset(lut, 0, sizeof(int)*number_of_objects);
		int* tot_sizes=new int[number_of_objects];
		memset(tot_sizes, 0, sizeof(int)*number_of_objects);
		new_label=1;
		for(i=1; i<number_of_objects; i++)
			{
			label=i;
			while(label!=changes[label])
				label=changes[label];
			if(label==i)
				{
				lut[i]=new_label;
				new_label++;
				}
			else
				lut[i]=lut[label];
			tot_sizes[lut[i]]+=sizes[i];
			}
		//set all labels !=1 -> obj
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(tot_sizes[lut[label]]>=min_size)
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				else
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				}
			}
		delete[] changes;
		delete[] sizes;
		delete[] lut;
		delete[] tot_sizes;
		}
	
	//this->image_has_changed();
	delete label_image;				
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::cog_inside_2D(image_binary<IMAGEDIM>* mask, int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p,m;//pixel value
	int label,up,left;//neighbour labels 
	//image_general<int, IMAGEDIM> label_image(max_u, max_v, max_w);
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	for(w=0; w<max_w; w++)
		{
		int number_of_objects=1;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)//Note: we want to label bkg-objects in order to remove holes
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		int* changes=new int[number_of_objects];
		int i;
		for(i=0; i<number_of_objects; i++)
			changes[i]=i;
		int* sizes=new int[number_of_objects];
		memset(sizes, 0, sizeof(int)*number_of_objects);
		//merge labels
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;
					sizes[new_label]++;						
					}
				}
			}
		int* lut=new int[number_of_objects];
		memset(lut, 0, sizeof(int)*number_of_objects);
		int* tot_sizes=new int[number_of_objects];
		memset(tot_sizes, 0, sizeof(int)*number_of_objects);
		double *cog_u=new double[number_of_objects];
		memset(cog_u, 0, sizeof(double)*number_of_objects);
		double *cog_v=new double[number_of_objects];
		memset(cog_v, 0, sizeof(double)*number_of_objects);
		new_label=1;
		for(i=1; i<number_of_objects; i++)
			{
			label=i;
			while(label!=changes[label])
				label=changes[label];
			if(label==i)
				{
				lut[i]=new_label;
				new_label++;
				}
			else
				lut[i]=lut[label];
			tot_sizes[lut[i]]+=sizes[i];
			}
		if(new_label>1)
			{
			for(v=0; v<max_v; v++)
				{
				for(u=0; u<max_u; u++)
					{
					label=label_image->get_voxel(u,v,w);
					if(label>0)
						{
						cog_u[lut[label]]+=u;
						cog_v[lut[label]]+=v;
						}
					}
				}
			}
		int u0=max_u;
		int u1=0;
		int v0=max_v;
		int v1=0;
		
		//Find bounding rectangle to mask
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				m=mask->get_voxel_by_dir(u,v,w,direction);			
				if(m==object_value)
					{
					if(u<u0)
						u0=u;
					if(u>u1)
						u1=u;
					if(v<v0)
						v0=v;
					if(v>v1)
						v1=v;
					}
				}
			}
		for(i=1; i<new_label; i++)
			{
			if(tot_sizes[i]>0)
				{
				cog_u[i]/=tot_sizes[i];
				cog_v[i]/=tot_sizes[i];
				u=(int)(cog_u[i]+0.5);
				v=(int)(cog_v[i]+0.5);
				m=mask->get_voxel_by_dir(u,v,w,direction);				
				if(m!=object_value)
					{					
					int nHits=0;
					int du;
					int dv;
					bool hit;
					
					//Right
					hit=false;
					for(du=u; du<=u1 &&!hit; du++)
						{
						m=mask->get_voxel_by_dir(du,v,w,direction);				
						if(m==object_value)
							{
							hit=true;
							nHits++;
							}
						}
					//Left
					hit=false;
					for(du=u; du>=u0 &&!hit; du--)
						{
						m=mask->get_voxel_by_dir(du,v,w,direction);				
						if(m==object_value)
							{
							hit=true;
							nHits++;
							}
						}

					//Up
					hit=false;						
					for(dv=v; dv>=v0 &&!hit; dv--)
						{
						m=mask->get_voxel_by_dir(u,dv,w,direction);				
						if(m==object_value)
							{
							hit=true;
							nHits++;
							}
						}
					
					//Down	
					hit=false;					
					for(dv=v; dv<=v1 &&!hit; dv++)
						{
						m=mask->get_voxel_by_dir(u,dv,w,direction);				
						if(m==object_value)
							{
							hit=true;
							nHits++;
							}
						}
					
					if(nHits==3)
						{
						//Diagonals
						double step_u;
						double step_v;
						double u2;
						double v2;
						int r;

						for(r=-3; r<=3 && nHits==3; r++)
							{
							if(r==0)
								r=1;
							nHits=0;
							//Upper left
							hit=false;
							step_u=-1.0;
							step_v=-r/3.0;
							u2=u;
							v2=v;
							while(u2>=u0 && v2>=v0 && u2<=u1 && v2<=v1 &&!hit)
								{
								m=mask->get_voxel_by_dir(u,dv,w,direction);				
								if(m==object_value)
									{
									hit=true;
									nHits++;
									}
								else
									{
									u2+=step_u;
									v2+=step_v;
									}
								}
							//Upper right
							hit=false;
							step_u=r/3.0;
							step_v=-1.0;
							u2=u;
							v2=v;
							while(u2>=u0 && v2>=v0 && u2<=u1 && v2<=v1 &&!hit)
								{
								m=mask->get_voxel_by_dir(u,dv,w,direction);				
								if(m==object_value)
									{
									hit=true;
									nHits++;
									}
								else
									{
									u2+=step_u;
									v2+=step_v;
									}
								}
							//Lower left
							hit=false;
							step_u=-r/3.0;
							step_v=1.0;
							u2=u;
							v2=v;
							while(u2>=u0 && v2>=v0 && u2<=u1 && v2<=v1 &&!hit)
								{
								m=mask->get_voxel_by_dir(u,dv,w,direction);				
								if(m==object_value)
									{
									hit=true;
									nHits++;
									}
								else
									{
									u2+=step_u;
									v2+=step_v;
									}
								}
							//Lower right
							hit=false;
							step_u=1.0;
							step_v=r/3.0;
							u2=u;
							v2=v;
							while(u2>=u0 && v2>=v0 && u2<=u1 && v2<=v1 &&!hit)
								{
								m=mask->get_voxel_by_dir(u,dv,w,direction);				
								if(m==object_value)
									{
									hit=true;
									nHits++;
									}
								else
									{
									u2+=step_u;
									v2+=step_v;
									}
								}
							}	
						if(nHits<4)
							tot_sizes[i]=0;		
						}
					else if(nHits<3)
						tot_sizes[i]=0;
					}
				}
			}
		//set all labels !=1 -> obj
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(tot_sizes[lut[label]]>0)
					this->set_voxel_by_dir(u,v,w,object_value,direction);
				else
					this->set_voxel_by_dir(u,v,w,!object_value,direction);
				}
			}
		delete[] changes;
		delete[] sizes;
		delete[] lut;
		delete[] tot_sizes;
		delete[] cog_u;
		delete[] cog_v;
		}
	
	//this->image_has_changed();
	delete label_image;				
	}
		
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::connect_outer_2D(int direction, IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
		
	IMGBINARYTYPE p;//pixel value

	for(w=0; w<max_w; w++)
		{
		int number_of_pixels=0;
		int u_at_min_u=max_u;
		int u_at_max_u=0;
		int v_at_min_u=max_v;
		int v_at_max_u=0;
		int u_at_min_v=max_u;
		int u_at_max_v=0;
		int v_at_min_v=max_v;
		int v_at_max_v=0;
		bool found=false;
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				if(p==object_value)
					{
					found=true;
					if(u<u_at_min_u) {u_at_min_u=u; v_at_min_u=v;}
					if(u>u_at_max_u) {u_at_max_u=u; v_at_max_u=v;}
					if(v<v_at_min_v) {v_at_min_v=v; u_at_min_v=u;}
					if(v>v_at_max_v) {v_at_max_v=v; u_at_max_v=u;}
					}
				}
			}
		if(found)
			{
			bool hit;

			//Start from upper left
			int prev_u=u_at_min_v;
			int prev_v=v_at_min_v;
			for(v=v_at_min_v+1; v<v_at_min_u; v++)
				{
				hit=false;
				for(u=u_at_min_u; u<prev_u && !hit; )
					{
					p=this->get_voxel_by_dir(u,v,w,direction);
					if(p==object_value)
						hit=true;
					else
						u++;
					}
				if(hit)
					{
					this->draw_line_2D(u,v,prev_u,prev_v,w,object_value,direction);
					prev_u=u;
					prev_v=v;
					}
				}
			this->draw_line_2D(u_at_min_u,v_at_min_u,prev_u,prev_v,w,object_value,direction);

			//Start from lower left
			prev_u=u_at_max_v;
			prev_v=v_at_max_v;
			for(v=v_at_max_v-1; v>v_at_min_u; v--)
				{
				hit=false;
				for(u=u_at_min_u; u<prev_u && !hit; )
					{
					p=this->get_voxel_by_dir(u,v,w,direction);
					if(p==object_value)
						hit=true;
					else
						u++;
					}
				if(hit)
					{
					this->draw_line_2D(u,v,prev_u,prev_v,w,object_value,direction);
					prev_u=u;
					prev_v=v;
					}
				}
			this->draw_line_2D(u_at_min_u,v_at_min_u,prev_u,prev_v,w,object_value,direction);

			//Start from upper right
			prev_u=u_at_min_v;
			prev_v=v_at_min_v;
			for(v=v_at_min_v; v<v_at_max_u; v++)
				{
				hit=false;
				for(u=u_at_max_u; u>prev_u && !hit; )
					{
					p=this->get_voxel_by_dir(u,v,w,direction);
					if(p==object_value)
						hit=true;
					else
						u--;
					}
				if(hit)
					{
					this->draw_line_2D(u,v,prev_u,prev_v,w,object_value,direction);
					prev_u=u;
					prev_v=v;
					}
				}
			this->draw_line_2D(u_at_max_u,v_at_max_u,prev_u,prev_v,w,object_value,direction);

			//Start from lower right
			prev_u=u_at_max_v;
			prev_v=v_at_max_v;
			for(v=v_at_max_v; v>v_at_max_u; v--)
				{
				hit=false;
				for(u=u_at_max_u; u>prev_u && !hit; )
					{
					p=this->get_voxel_by_dir(u,v,w,direction);
					if(p==object_value)
						hit=true;
					else
						u--;
					}
				if(hit)
					{
					this->draw_line_2D(u,v,prev_u,prev_v,w,object_value,direction);
					prev_u=u;
					prev_v=v;
					}
				}
			this->draw_line_2D(u_at_max_u,v_at_max_u,prev_u,prev_v,w,object_value,direction);
			}
		}
	//this->image_has_changed();				
	}

template <int IMAGEDIM>
image_scalar<float, IMAGEDIM>* image_binary<IMAGEDIM>::distance_2D(bool edge_is_object, int direction, IMGBINARYTYPE object_value)
{
	image_scalar<float, IMAGEDIM>* output = new image_scalar<float,IMAGEDIM> (this,false);
	float u_dist, v_dist, uv_dist;
	if (direction==0) {
		u_dist = output->get_physical_distance_between_voxels(0,0,0,0,1,0);
		v_dist = output->get_physical_distance_between_voxels(0,0,0,0,0,1);
		uv_dist = output->get_physical_distance_between_voxels(0,0,0,0,1,1);
	} else 	if (direction==1) {
		u_dist = output->get_physical_distance_between_voxels(0,0,0,0,0,1);
		v_dist = output->get_physical_distance_between_voxels(0,0,0,1,0,0);
		uv_dist = output->get_physical_distance_between_voxels(0,0,0,1,0,1);
	} else {
		u_dist = output->get_physical_distance_between_voxels(0,0,0,1,0,0);
		v_dist = output->get_physical_distance_between_voxels(0,0,0,0,1,0);
		uv_dist = output->get_physical_distance_between_voxels(0,0,0,1,1,0);
	}
	int u,v,w;
	int max_u=this->get_size_by_dim_and_dir(0,direction);
	int max_v=this->get_size_by_dim_and_dir(1,direction);
	int max_w=this->get_size_by_dim_and_dir(2,direction);

	float veryhigh = (std::max(max_u,max_v)*uv_dist); // veryhigh must be hihgher than any final calculated distance value in image
	float initvalue = (edge_is_object)?veryhigh:0;
	IMGBINARYTYPE p;//pixel value
	float mm,ul,um,ur,ml,mr,ll,lm,lr;//neighbour labels: First letter: upper/middle/lower (v-direction). Second letter: left/middle/right (u-direction).
	
	//Forward pass
	for(w=0; w<max_w; w++){
		for(v=0; v<max_v; v++){
			u=0;
			ml=initvalue;
			ul=initvalue;
			um=(v>0)? output->get_voxel_by_dir(u,v-1,w,direction) : initvalue;
			for(u=0; u<max_u; u++){
				p=this->get_voxel_by_dir(u,v,w,direction);
				ur=(u<max_u-1 && v>0)? output->get_voxel_by_dir(u+1,v-1,w,direction) : initvalue;
				mm=(p==object_value)?std::min(std::min(uv_dist+ul, v_dist+um), std::min(uv_dist+ur, u_dist+ml)):0;
				output->set_voxel_by_dir(u,v,w,mm,direction);
				ml=mm; //traverse the information minimie "get_voxel" calls...
				ul=um;
				um=ur;
			}
		}
			
		//Backward pass
		for(v=max_v-1; v>=0; v--){
			u=max_u-1;
			mr=initvalue;
			lr=initvalue;
			lm=(v<max_v-1)? output->get_voxel_by_dir(u,v+1,w,direction) : initvalue;
			for(u=max_u-1; u>=0; u--){
				mm=output->get_voxel_by_dir(u,v,w,direction);
				ll=(u>0 && v<max_v-1)? output->get_voxel_by_dir(u-1,v+1,w,direction) : initvalue;
				mm=std::min(mm,std::min(std::min(uv_dist+ll,v_dist+lm),std::min(uv_dist+lr,u_dist+mr)));
				output->set_voxel_by_dir(u,v,w,mm,direction);
				mr=mm;
				lr=lm;
				lm=ll;
			}
		}
	}
	
	output->data_has_changed();
	return output;
}

template <int IMAGEDIM>
image_integer<short, IMAGEDIM> *  image_binary<IMAGEDIM>::distance_34_2D(bool edge_is_object, int direction, IMGBINARYTYPE object_value)
	{
	image_integer<short, IMAGEDIM>* output = new image_integer<short,IMAGEDIM> (this,false);
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim_and_dir(0,direction);
	max_v=this->get_size_by_dim_and_dir(1,direction);
	max_w=this->get_size_by_dim_and_dir(2,direction);
	int veryhigh = std::max(max_u,max_v)*4;//std::numeric_limits<int>::max()-9;
	int initvalue=(edge_is_object)?veryhigh:0;
	IMGBINARYTYPE p;//pixel value
	int d,ul,um,ur,ml,mr,ll,lm,lr;//neighbour labels 
	for(w=0; w<max_w; w++)
		{
		//Forward pass
		for(v=0; v<max_v; v++)
			{
			u=0;
			ml=initvalue;
			ul=initvalue;
			um=(v>0)? output->get_voxel_by_dir(u,v-1,w,direction) : initvalue;
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel_by_dir(u,v,w,direction);
				ur=(v>0 && u<max_u-1)? output->get_voxel_by_dir(u+1,v-1,w,direction) : initvalue;
				d=(p==object_value)?std::min(std::min(4+ul,3+um),std::min(4+ur,3+ml)):0;
				output->set_voxel_by_dir(u,v,w,d,direction);
				ml=d;
				ul=um;
				um=ur;
				}
			}
		//Backward pass
		for(v=max_v-1; v>=0; v--)
			{
			u=max_u-1;
			mr=initvalue;
			lr=initvalue;
			lm=(v<max_v-1)? output->get_voxel_by_dir(u,v+1,w,direction) : initvalue;
			for(u=max_u-1; u>=0; u--)
				{
				d=output->get_voxel_by_dir(u,v,w,direction);
				ll=(v<max_v-1 && u>0)? output->get_voxel_by_dir(u-1,v+1,w,direction) : initvalue;
				d=std::min(d,std::min(std::min(4+ll,3+lm),std::min(4+lr,3+mr)));
				output->set_voxel_by_dir(u,v,w,d,direction);
				mr=d;
				lr=lm;
				lm=ll;
				}
			}
		}
	//output->min_max_refresh();
	return output;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::get_num_neighbours_distribution_in_slice_2D_4Nbh(vector<int> &num_vox, int slice, int dir, IMGBINARYTYPE object_value)
{
	int usize = this->get_size_by_dim_and_dir(0,dir);
	int vsize = this->get_size_by_dim_and_dir(1,dir);
	int tmp;
	num_vox.clear();
	for(int i=0; i<=4; i++){
		num_vox.push_back(0);
	}
	
	for(int v=0; v<vsize; v++){
		for(int u=0; u<usize; u++){
			if(this->get_voxel_by_dir(u,v,slice,dir)==object_value){
				tmp=0;

				if(v>0 && this->get_voxel_by_dir(u,v-1,slice,dir)==object_value)
					tmp++;	
				if(u>0 && this->get_voxel_by_dir(u-1,v,slice,dir)==object_value)
					tmp++;	
				if((u+1)<usize && this->get_voxel_by_dir(u+1,v,slice,dir)==object_value)
					tmp++;	
				if((v+1)<vsize && this->get_voxel_by_dir(u,v+1,slice,dir)==object_value)
					tmp++;
				
				num_vox[tmp]++;
			}

		}
	}

}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::get_num_neighbours_distribution_in_slice_2D_8Nbh(vector<int> &num_vox, int slice, int dir, IMGBINARYTYPE object_value)
{
	int usize = this->get_size_by_dim_and_dir(0,dir);
	int vsize = this->get_size_by_dim_and_dir(1,dir);
	int tmp;
	num_vox.clear();
	for(int i=0; i<=8; i++){
		num_vox.push_back(0);
	}
	
	for(int v=0; v<vsize; v++){
		for(int u=0; u<usize; u++){
			if(this->get_voxel_by_dir(u,v,slice,dir)==object_value){
				tmp=0;

				for(int s=std::max(0,v-1); s<=std::min(v+1,vsize-1); s++){
					for(int r=std::max(0,u-1); r<=std::min(u+1,usize-1); r++){
						if(this->get_voxel_by_dir(r,s,slice,dir)==object_value)
							tmp++;	
					}	
				}

				num_vox[tmp]++;

			}

		}
	}

}

//3D functions	
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::fill_holes_3D(IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	IMGBINARYTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	int number_of_objects=1;
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel(u,v,w);
				if(p!=object_value)//Note: we want to label bkg-objects in order to remove holes
					{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		}
	int* changes=new int[number_of_objects];
	int i;
	for(i=0; i<number_of_objects; i++)
		changes[i]=i;
	//merge labels
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(above>0)
						changes[above]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;						
					}
				}
			}
		}
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	new_label=1;
	for(i=1; i<number_of_objects; i++)
		{
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i)
			{
			lut[i]=new_label;
			new_label++;
			}
		else
			lut[i]=lut[label];
		}
	//set all labels !=1 -> obj
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(lut[label]==1)//First label is background
					this->set_voxel(u,v,w,!object_value);
				else //all other labels are object
					this->set_voxel(u,v,w,object_value);
				}
			}
		}
	
	//this->image_has_changed();
	delete[] changes;
	delete[] lut;
	delete label_image;				
	}	

template <int IMAGEDIM>
image_scalar<float, IMAGEDIM>* image_binary<IMAGEDIM>::distance_3D(bool edge_is_object, IMGBINARYTYPE object_value)
{
	image_scalar<float, IMAGEDIM>* output = new image_scalar<float,IMAGEDIM> (this,false);
	float u_dist = output->get_physical_distance_between_voxels(0,0,0,1,0,0);
	float v_dist = output->get_physical_distance_between_voxels(0,0,0,0,1,0);
	float w_dist = output->get_physical_distance_between_voxels(0,0,0,0,0,1);
	float uv_dist = output->get_physical_distance_between_voxels(0,0,0,1,1,0);
	float uw_dist = output->get_physical_distance_between_voxels(0,0,0,1,0,1);
	float vw_dist = output->get_physical_distance_between_voxels(0,0,0,0,1,1);
	float uvw_dist = output->get_physical_distance_between_voxels(0,0,0,1,1,1);

	int u,v,w;
	int max_u=this->get_size_by_dim(0);
	int max_v=this->get_size_by_dim(1);
	int max_w=this->get_size_by_dim(2);
	float veryhigh = (std::max(max_w,std::max(max_u,max_v))*uvw_dist); // veryhigh must be hihgher than any final calculated distance value in image
	float initvalue = (edge_is_object)?veryhigh:0;
	IMGBINARYTYPE p;//pixel value
	float mmm,mul,mum,mur,mml,mmr,mll,mlm,mlr,aul,aum,aur,aml,amm,amr,all,alm,alr,bul,bum,bur,bml,bmm,bmr,bll,blm,blr;//neighbour labels: First letter: above/middle/below (w-direction). Second letter: upper/middle/lower (v-direction). Third letter: left/middle/right (u-direction).
	
	//Forward pass
	for(w=0; w<max_w; w++){
		for(v=0; v<max_v; v++){
			u=0;
			mml=initvalue;
			mul=initvalue;
			aml=initvalue;
			aul=initvalue;
			all=initvalue;
			mum=(v>0)? output->get_voxel(u,v-1,w) : initvalue;
			aum=(v>0 && w>0)? output->get_voxel(u,v-1,w-1) : initvalue;
			amm=(w>0)? output->get_voxel(u,v,w-1) : initvalue;
			alm=(v<max_v-1 && w>0)? output->get_voxel(u,v+1,w-1) : initvalue;
			for(u=0; u<max_u; u++){
				p=this->get_voxel(u,v,w);
				mur=(u<max_u-1 && v>0)? output->get_voxel(u+1,v-1,w) : initvalue;
				aur=(u<max_u-1 && v>0 && w>0)? output->get_voxel(u+1,v-1,w-1) : initvalue;
				amr=(u<max_u-1 && w>0)? output->get_voxel(u+1,v,w-1) : initvalue;
				alr=(u<max_u-1 && v<max_v-1 && w>0)? output->get_voxel(u+1,v+1,w-1) : initvalue;
				mmm=(p==object_value)?std::min(
											std::min(
												std::min( std::min(uvw_dist+aul,vw_dist+aum),std::min(uvw_dist+aur,uw_dist+aml) ),
                                                std::min( std::min(w_dist+amm,uw_dist+amr),std::min(uvw_dist+all,vw_dist+alm) )
												),
											std::min( std::min( std::min(uvw_dist+alr,uv_dist+mul),std::min(v_dist+mum,uv_dist+mur) ), u_dist+mml )
											):0;
				output->set_voxel(u,v,w,mmm);
				mml=mmm; //traverse the information minimie "get_voxel" calls...
				mul=mum;
				mum=mur;
				aul=aum;
				aum=aur;
				aml=amm;
				amm=amr;
				all=alm;
				alm=alr;
				}
			}
		}

	//Backward pass
	for(w=max_w-1; w>=0; w--){
		for(v=max_v-1; v>=0; v--){
			u=max_u-1;
			mmr=initvalue;
			mlr=initvalue;
			bmr=initvalue;
			bur=initvalue;
			blr=initvalue;
			mlm=(v<max_v-1)? output->get_voxel(u,v+1,w) : initvalue;
			bum=(v>0 && w<max_w-1)? output->get_voxel(u,v-1,w+1) : initvalue;
			bmm=(w<max_w-1)? output->get_voxel(u,v,w+1) : initvalue;
			blm=(v<max_v-1 && w<max_w-1)? output->get_voxel(u,v+1,w+1) : initvalue;

			for(u=max_u-1; u>=0; u--){
				p=this->get_voxel(u,v,w);
				mmm=output->get_voxel(u,v,w);
				mll=(u>0 && v<max_v-1)? output->get_voxel(u-1,v+1,w) : initvalue;
				bul=(u>0 && v>0 && w<max_w-1)? output->get_voxel(u-1,v-1,w+1) : initvalue;
				bml=(u>0 && w<max_w-1)? output->get_voxel(u-1,v,w+1) : initvalue;
				bll=(u>0 && v<max_v-1 && w<max_w-1)? output->get_voxel(u-1,v+1,w+1) : initvalue;
				mmm=(p==object_value)?std::min(
                                            std::min(
												std::min( std::min(uvw_dist+bul,vw_dist+bum),std::min(uvw_dist+bur,uw_dist+bml) ),
                                                std::min( std::min(w_dist+bmm,uw_dist+bmr),std::min(uvw_dist+bll,vw_dist+blm) )
												),
											std::min( 
												std::min(std::min(uvw_dist+blr,uv_dist+mll),std::min(v_dist+mlm,uv_dist+mlr) ),
                                                std::min(mmm,u_dist+mmr)
												)
											):0;
				output->set_voxel(u,v,w,mmm);
				mmr=mmm;
				mlr=mlm;
				mlm=mll;
				bur=bum;
				bum=bul;
				bmr=bmm;
				bmm=bml;
				blr=blm;
				blm=bll;
				}
			}
		}

	output->data_has_changed();
	return output;
	}

template <int IMAGEDIM>
image_integer<short, IMAGEDIM> *  image_binary<IMAGEDIM>::distance_345_3D(bool edge_is_object, IMGBINARYTYPE object_value)
	{
	image_integer<short, IMAGEDIM>* output = new image_integer<short,IMAGEDIM> (this,false);
	int u,v,w;
	int max_u=this->get_size_by_dim(0);
	int max_v=this->get_size_by_dim(1);
	int max_w=this->get_size_by_dim(2);
	int veryhigh = (std::max(max_w,std::max(max_u,max_v))*5); //5 times the largest voxel direction... //std::numeric_limits<int>::max()-11;
	int initvalue = (edge_is_object)?veryhigh:0;
	IMGBINARYTYPE p;//pixel value
	int mmm,mul,mum,mur,mml,mmr,mll,mlm,mlr,aul,aum,aur,aml,amm,amr,all,alm,alr,bul,bum,bur,bml,bmm,bmr,bll,blm,blr;//neighbour labels: First letter: above/middle/below (w-direction). Second letter: upper/middle/lower (v-direction). Third letter: left/middle/right (u-direction).
	
	//Forward pass
	for(w=0; w<max_w; w++){
		for(v=0; v<max_v; v++){
			u=0;
			mml=initvalue;
			mul=initvalue;
			aml=initvalue;
			aul=initvalue;
			all=initvalue;
			mum=(v>0)? output->get_voxel(u,v-1,w) : initvalue;
			aum=(v>0 && w>0)? output->get_voxel(u,v-1,w-1) : initvalue;
			amm=(w>0)? output->get_voxel(u,v,w-1) : initvalue;
			alm=(v<max_v-1 && w>0)? output->get_voxel(u,v+1,w-1) : initvalue;
			for(u=0; u<max_u; u++){
				p=this->get_voxel(u,v,w);
				mur=(u<max_u-1 && v>0)? output->get_voxel(u+1,v-1,w) : initvalue;
				aur=(u<max_u-1 && v>0 && w>0)? output->get_voxel(u+1,v-1,w-1) : initvalue;
				amr=(u<max_u-1 && w>0)? output->get_voxel(u+1,v,w-1) : initvalue;
				alr=(u<max_u-1 && v<max_v-1 && w>0)? output->get_voxel(u+1,v+1,w-1) : initvalue;
				mmm=(p==object_value)?std::min(
											std::min(
												std::min( std::min(5+aul,4+aum),std::min(5+aur,4+aml) ),
                                                std::min( std::min(3+amm,4+amr),std::min(5+all,4+alm) )
												),
											std::min( std::min( std::min(5+alr,4+mul),std::min(3+mum,4+mur) ), 3+mml )
											):0;
				output->set_voxel(u,v,w,mmm);
				mml=mmm; //traverse the information minimie "get_voxel" calls...
				mul=mum;
				mum=mur;
				aul=aum;
				aum=aur;
				aml=amm;
				amm=amr;
				all=alm;
				alm=alr;
				}
			}
		}

	//Backward pass
	for(w=max_w-1; w>=0; w--){
		for(v=max_v-1; v>=0; v--){
			u=max_u-1;
			mmr=initvalue;
//			mur=initvalue; //bug --> mur-->mlr		//another bug was fixed by setting all "w-1" --> "w+1"
			mlr=initvalue;
			bmr=initvalue;
			bur=initvalue;
			blr=initvalue;
			mlm=(v<max_v-1)? output->get_voxel(u,v+1,w) : initvalue;
			bum=(v>0 && w<max_w-1)? output->get_voxel(u,v-1,w+1) : initvalue;
			bmm=(w<max_w-1)? output->get_voxel(u,v,w+1) : initvalue;
			blm=(v<max_v-1 && w<max_w-1)? output->get_voxel(u,v+1,w+1) : initvalue;

			for(u=max_u-1; u>=0; u--){
				p=this->get_voxel(u,v,w);
				mmm=output->get_voxel(u,v,w);
				mll=(u>0 && v<max_v-1)? output->get_voxel(u-1,v+1,w) : initvalue;
				bul=(u>0 && v>0 && w<max_w-1)? output->get_voxel(u-1,v-1,w+1) : initvalue;
				bml=(u>0 && w<max_w-1)? output->get_voxel(u-1,v,w+1) : initvalue;
				bll=(u>0 && v<max_v-1 && w<max_w-1)? output->get_voxel(u-1,v+1,w+1) : initvalue;
				mmm=(p==object_value)?std::min(
                                            std::min(
												std::min( std::min(5+bul,4+bum),std::min(5+bur,4+bml) ),
                                                std::min( std::min(3+bmm,4+bmr),std::min(5+bll,4+blm) )
												),
											std::min( 
												std::min(std::min(5+blr,4+mll),std::min(3+mlm,4+mlr) ),
                                                std::min(mmm,3+mmr)
												)
											):0;
				output->set_voxel(u,v,w,mmm);
				mmr=mmm;
				mlr=mlm;
				mlm=mll;
				bur=bum;
				bum=bul;
				bmr=bmm;
				bmm=bml;
				blr=blm;
				blm=bll;
				}
			}
		}

	//output->min_max_refresh();
	return output;
	}

template <int IMAGEDIM>
image_integer<short, IMAGEDIM>* image_binary<IMAGEDIM>::distance_chessboard_3D(bool edge_is_object, IMGBINARYTYPE object_value)
{
	image_integer<short, IMAGEDIM>* output = new image_integer<short,IMAGEDIM> (this,false);
	int u,v,w;
	int max_u=this->get_size_by_dim(0);
	int max_v=this->get_size_by_dim(1);
	int max_w=this->get_size_by_dim(2);
	int veryhigh = (std::max(max_w,std::max(max_u,max_v))); // veryhigh must be higher than any final calculated distance value in image
	int initvalue = (edge_is_object)?veryhigh:0;
	IMGBINARYTYPE p;//pixel value
	int mmm,mul,mum,mur,mml,mmr,mll,mlm,mlr,aul,aum,aur,aml,amm,amr,all,alm,alr,bul,bum,bur,bml,bmm,bmr,bll,blm,blr;//neighbour labels: First letter: above/middle/below (w-direction). Second letter: upper/middle/lower (v-direction). Third letter: left/middle/right (u-direction).
	
	//Forward pass
	for(w=0; w<max_w; w++){
		for(v=0; v<max_v; v++){
			u=0;
			mml=initvalue;
			mul=initvalue;
			aml=initvalue;
			aul=initvalue;
			all=initvalue;
			mum=(v>0)? output->get_voxel(u,v-1,w) : initvalue;
			aum=(v>0 && w>0)? output->get_voxel(u,v-1,w-1) : initvalue;
			amm=(w>0)? output->get_voxel(u,v,w-1) : initvalue;
			alm=(v<max_v-1 && w>0)? output->get_voxel(u,v+1,w-1) : initvalue;
			for(u=0; u<max_u; u++){
				p=this->get_voxel(u,v,w);
				mur=(u<max_u-1 && v>0)? output->get_voxel(u+1,v-1,w) : initvalue;
				aur=(u<max_u-1 && v>0 && w>0)? output->get_voxel(u+1,v-1,w-1) : initvalue;
				amr=(u<max_u-1 && w>0)? output->get_voxel(u+1,v,w-1) : initvalue;
				alr=(u<max_u-1 && v<max_v-1 && w>0)? output->get_voxel(u+1,v+1,w-1) : initvalue;
				mmm=(p==object_value)?std::min(
											   std::min(
														std::min( std::min(aul,aum),std::min(aur,aml) ),
														std::min( std::min(amm,amr),std::min(all,alm) )
														),
											   std::min( std::min( std::min(alr,mul),std::min(mum,mur) ), mml )
											   )+1:0;
				output->set_voxel(u,v,w,mmm);
				mml=mmm; //traverse the information minimie "get_voxel" calls...
				mul=mum;
				mum=mur;
				aul=aum;
				aum=aur;
				aml=amm;
				amm=amr;
				all=alm;
				alm=alr;
			}
		}
	}
	
	//Backward pass
	for(w=max_w-1; w>=0; w--){
		for(v=max_v-1; v>=0; v--){
			u=max_u-1;
			mmr=initvalue;
			mlr=initvalue;
			bmr=initvalue;
			bur=initvalue;
			blr=initvalue;
			mlm=(v<max_v-1)? output->get_voxel(u,v+1,w) : initvalue;
			bum=(v>0 && w<max_w-1)? output->get_voxel(u,v-1,w+1) : initvalue;
			bmm=(w<max_w-1)? output->get_voxel(u,v,w+1) : initvalue;
			blm=(v<max_v-1 && w<max_w-1)? output->get_voxel(u,v+1,w+1) : initvalue;
			
			for(u=max_u-1; u>=0; u--){
				p=this->get_voxel(u,v,w);
				mmm=output->get_voxel(u,v,w);
				mll=(u>0 && v<max_v-1)? output->get_voxel(u-1,v+1,w) : initvalue;
				bul=(u>0 && v>0 && w<max_w-1)? output->get_voxel(u-1,v-1,w+1) : initvalue;
				bml=(u>0 && w<max_w-1)? output->get_voxel(u-1,v,w+1) : initvalue;
				bll=(u>0 && v<max_v-1 && w<max_w-1)? output->get_voxel(u-1,v+1,w+1) : initvalue;
				mmm=(p==object_value)?std::min(
											   std::min(
														std::min( std::min(bul,bum),std::min(bur,bml) ),
														std::min( std::min(bmm,bmr),std::min(bll,blm) )
														),
											   std::min( 
														std::min(std::min(blr,mll),std::min(mlm,mlr) ),
														std::min(mmm-1,mmr)
														)
											   )+1:0;
				output->set_voxel(u,v,w,mmm);
				mmr=mmm;
				mlr=mlm;
				mlm=mll;
				bur=bum;
				bum=bul;
				bmr=bmm;
				bmm=bml;
				blr=blm;
				blm=bll;
			}
		}
	}
	
	output->data_has_changed();
	return output;
}

template <int IMAGEDIM>
vector<image_integer<short, IMAGEDIM>* > image_binary<IMAGEDIM>::distance_path_to_border_3D(image_binary<IMAGEDIM>* rim, image_integer<short, IMAGEDIM>* weights){
	image_integer<short, IMAGEDIM>* output = new image_integer<short,IMAGEDIM> (this,false);
	vector<Vector3D> points;
	vector<Vector3D> temp;
	vector<image_integer<short,IMAGEDIM>* > ret;

	//Create vector 3 in array;
	int max_x=this->get_size_by_dim(0);
	int max_y=this->get_size_by_dim(1);
	int max_z=this->get_size_by_dim(2);
	for(int x = 0; x < max_x; x++){
		for(int y = 0; y < max_y; y++){
			for(int z = 0; z < max_z; z++){
				if(rim->get_voxel(x,y,z) == 1){
					output->set_voxel(x,y,z,3);
					points.push_back(create_Vector3D(x,y,z));
				}else if(this->get_voxel(x,y,z) == 1){
					output->set_voxel(x,y,z,std::numeric_limits<short>::max());
					temp.push_back(create_Vector3D(x,y,z));
				}else{
					output->set_voxel(x,y,z,-1);
				}
			}
		}
	}
	points.insert(points.begin(),temp.begin(),temp.end());
	//It is now set up for dijkstra.
	
	ret.push_back(output);
	ret.push_back(dijkstra_image_version(output,points, weights));

	return ret;
}

template <int IMAGEDIM>
int image_binary<IMAGEDIM>::partition_quicksort(image_integer<short, IMAGEDIM>* dist, vector<Vector3D> &Q, int top, int bottom){
int x = dist->get_voxel(Q[top]);
int i = top - 1;
int j = bottom + 1;
Vector3D temp;
	do{
		do{
			j--;
		}while(x >dist->get_voxel(Q[j]));

		do{
			i++;
		}while(x <dist->get_voxel(Q[i]));

		if (i < j){ 
			temp = Q[i];   
			Q[i] = Q[j];
			Q[j] = temp;
		}
	}while (i < j);    
	return j;           // returns middle subscript  
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::sort_queue(image_integer<short, IMAGEDIM>* dist, vector<Vector3D> &Q, int top, int bottom){
	//Quicksort
     int middle;
     if (top < bottom)
    {
          middle = partition_quicksort(dist, Q, top, bottom);
          sort_queue(dist, Q, top, middle);
          sort_queue(dist, Q, middle+1, bottom);
     }
	return;
}

template <int IMAGEDIM>
bool image_binary<IMAGEDIM>::dijkstra_update(image_integer<short, IMAGEDIM>* dist, int x, int y, int z, short alt){
	if(alt < dist->get_voxel(x,y,z)){
		dist->set_voxel(x,y,z,alt);
		return true;
	}
	return false;
}

template <int IMAGEDIM>
int* image_binary<IMAGEDIM>::dijkstra_n26_weight(image_integer<short, IMAGEDIM>* weight, Vector3D u, int x_change[], int y_change[], int z_change[]){
	
	int vals[26];
	int res[26];
	for(int i = 0; i <26; i++){
		vals[i] = weight->get_voxel(u[0]+x_change[i],u[1]+y_change[i],u[2]+z_change[i]);
	}
	for(int i = 0; i <26; i++){
		res[i] = 0;
		for(int j = 0; j <26; j++){
			if(vals[i] < vals[j]){//Want to be up on the ridges
				res[i]++;
			}
		}
	}
	return res;
}

//Start with a vector with smallest distance last in array.
template <int IMAGEDIM>
image_integer<short,IMAGEDIM>* image_binary<IMAGEDIM>::dijkstra_image_version(image_integer<short, IMAGEDIM>* dist, vector<Vector3D> Q,image_integer<short, IMAGEDIM>* weight){
	vector<Vector3D> v;
	short dist_u;
	image_integer<short,IMAGEDIM>* parent_map = new image_integer<short,IMAGEDIM>(dist,false);
	parent_map->fill(-1);
	int current_m = 3;
	int start_size = Q.size();

	int x_change[26] = {1,-1, 0, 0,0, 0,   -1,-1,1, 1,   1,-1, 0, 0,   1,-1,0, 0,  -1,-1, 1, 1,  -1,-1,1, 1};
	int y_change[26] = {0, 0, 1,-1,0, 0,   -1, 1,1,-1,   0, 0, 1,-1,   0, 0,1,-1,  -1, 1, 1,-1,  -1, 1,1,-1};
	int z_change[26] = {0, 0, 0, 0,1,-1,    0, 0,0, 0,  -1,-1,-1,-1,   1, 1,1, 1,  -1,-1,-1,-1,   1, 1,1, 1};
	int alt_c[26] =    {3, 3, 3, 3,3, 3,	4, 4,4, 4,	 4, 4, 4, 4,   4, 4,4, 4,	5, 5, 5, 5,   5, 5,5, 5};
	NEIGHBOUR_DIR par[26]={N_PCC,N_NCC,N_CPC,N_CNC,N_CCP,N_CCN
								,N_NNC,N_NPC,N_PPC,N_PNC
								,N_PCN,N_NCN,N_CPN,N_CNP
								,N_PCP,N_NCP,N_CPP,N_CNP
								,N_NNN,N_NPN,N_PPN,N_PNN
								,N_NNP,N_NPP,N_PPP,N_PNP};

	while(!Q.empty()){
		
		Vector3D u = Q.back();
		if(dist->get_voxel(u) != current_m){
			cout << "Sorting list" << endl;
			sort_queue(dist, Q, 0 , Q.size()-1);
			cout << "List sorted" << endl;
			u = Q.back();
			current_m = dist->get_voxel(u);

			cout << "Process: " << start_size-Q.size() << " / "<< start_size <<endl;
			cout << "Dist: " << current_m << endl;
		}
		Q.pop_back();
		dist_u = current_m;

		if(dist_u == std::numeric_limits<short>::max()){//No reachable voxels left
			return parent_map;
		}
		//int* w = dijkstra_n26_weight(weight, u, x_change, y_change, z_change);
		for(int i = 0; i <6; i++){
			int alt = std::max(1,weight->get_voxel(u[0]+x_change[i],u[1]+y_change[i],u[2]+z_change[i])-weight->get_voxel(u));
			//int alt = weight->get_voxel(u[0]+x_change[i],u[1]+y_change[i],u[2]+z_change[i]);
			//int alt = 1 + abs(weight->get_voxel(u)-weight->get_voxel(u[0]+x_change[i],u[1]+y_change[i],u[2]+z_change[i]));
			//int alt = w[i]+1;//alt_c[i];//alt_c[i] + w[i];
			if(dijkstra_update(dist, u[0]+x_change[i],u[1]+y_change[i],u[2]+z_change[i],dist_u+alt))
				parent_map->set_voxel(u[0]+x_change[i],u[1]+y_change[i],u[2]+z_change[i],par[i]);
		}
	}
	return parent_map;
}


template <int IMAGEDIM>
image_label<3>* image_binary<IMAGEDIM>::label_connected_objects_3D(IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	IMGBINARYTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;
	changes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel(u,v,w);
				if(p==object_value)	{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(above>new_label)
						changes[above]=new_label;
					if(up>new_label)
						changes[up]=new_label;
					if(left>new_label)
						changes[left]=new_label;
					if(new_label==number_of_objects) {
						changes.push_back(number_of_objects);
						number_of_objects++;	
					}
					label_image->set_voxel(u,v,w,new_label);
				}
				else
					label_image->set_voxel(u,v,w,0);
			}
		}
	}
	
	// calculate look-up-table
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
		}
		else
			lut[i]=lut[label];
	}
	
    // set new labels from look-up table
	image_label<IMAGEDIM> * label_image_label = new image_label<IMAGEDIM> (label_image);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel(u,v,w);
				if (label>0)
					label_image_label->set_voxel(u,v,w,lut[label]);
			}
		}
	}
	delete[] lut;
	delete label_image;
	return label_image_label;
}

template <int IMAGEDIM>
image_integer<unsigned long,3>* image_binary<IMAGEDIM>::label_connected_objects_with_volume_3D(IMGBINARYTYPE object_value)
{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	IMGBINARYTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<unsigned long,IMAGEDIM> * label_image = new image_integer<unsigned long,IMAGEDIM> (this);
	int new_label;
	//init labels
	int number_of_objects=1;
	vector<int> changes;
	changes.push_back(0);
	vector<int> sizes;
	sizes.push_back(0);
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				p=this->get_voxel(u,v,w);
				if(p==object_value)	{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(above>new_label)
						changes[above]=new_label;
					if(up>new_label)
						changes[up]=new_label;
					if(left>new_label)
						changes[left]=new_label;
					if(new_label==number_of_objects) {
						changes.push_back(number_of_objects);
						sizes.push_back(0);
						number_of_objects++;	
					}
					label_image->set_voxel(u,v,w,new_label);
					sizes[new_label]++;
				}
				else
					label_image->set_voxel(u,v,w,0);
			}
		}
	}
	
	// calculate look-up-table and sizes
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	vector<int> tot_sizes;
	tot_sizes.push_back(0);
	new_label=1;
	for(int i=1; i<number_of_objects; i++) {
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i) {
			lut[i]=new_label;
			new_label++;
			tot_sizes.push_back(0);
		}
		else
			lut[i]=lut[label];
		tot_sizes[lut[i]]+=sizes[i];
	}
	
    // set new labels from look-up table and sizes
	for(w=0; w<max_w; w++) {
		for(v=0; v<max_v; v++) {
			for(u=0; u<max_u; u++) {
				label=label_image->get_voxel(u,v,w);
				if (label>0) {
					label_image->set_voxel(u,v,w,tot_sizes[lut[label]]);
				}
			}
		}
	}
	delete[] lut;
	return label_image;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_object_3D(IMGBINARYTYPE object_value)
	{
	int u,v,w;
	int max_u, max_v, max_w;
	max_u=this->get_size_by_dim(0);
	max_v=this->get_size_by_dim(1);
	max_w=this->get_size_by_dim(2);
		
	IMGBINARYTYPE p;//pixel value
	int label,above,up,left;//neighbour labels 
    image_integer<int,IMAGEDIM> * label_image = new image_integer<int, IMAGEDIM> (max_u, max_v, max_w);
	int new_label;
	//init labels
	int number_of_objects=1;
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				p=this->get_voxel(u,v,w);
				if(p==object_value)
					{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					new_label=number_of_objects;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					if(new_label==number_of_objects)
						number_of_objects++;
					label_image->set_voxel(u,v,w,new_label);
					}
				else
					label_image->set_voxel(u,v,w,0);
				}
			}
		}
	int* changes=new int[number_of_objects];
	int i;
	int* sizes=new int[number_of_objects];
	memset(sizes, 0, sizeof(int)*number_of_objects);
	for(i=0; i<number_of_objects; i++)
		changes[i]=i;
	//merge labels
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(label>0)
					{
					above=(w>0)? label_image->get_voxel(u,v,w-1) : 0;
					up=(v>0)? label_image->get_voxel(u,v-1,w) : 0;
					left=(u>0)? label_image->get_voxel(u-1,v,w) : 0;
					while(label!=changes[label])
						label=changes[label];
					while(above!=changes[above])
						above=changes[above];
					while(up!=changes[up])
						up=changes[up];
					while(left!=changes[left])
						left=changes[left];
					new_label=label;
					if(above>0 && above<new_label)
						new_label=above;
					if(up>0 && up<new_label)
						new_label=up;
					if(left>0 && left<new_label)
						new_label=left;
					changes[label]=new_label;
					if(above>0)
						changes[above]=new_label;
					if(up>0)
						changes[up]=new_label;
					if(left>0)
						changes[left]=new_label;
					sizes[new_label]++;									
					}
				}
			}
		}
	int* lut=new int[number_of_objects];
	memset(lut, 0, sizeof(int)*number_of_objects);
	int* tot_sizes=new int[number_of_objects];
	memset(tot_sizes, 0, sizeof(int)*number_of_objects);
	new_label=1;
	for(i=1; i<number_of_objects; i++)
		{
		label=i;
		while(label!=changes[label])
			label=changes[label];
		if(label==i)
			{
			lut[i]=new_label;
			new_label++;
			}
		else
			lut[i]=lut[label];
		tot_sizes[lut[i]]+=sizes[i];
		}
	int max_size=0;
	int max_label=1;
	for(i=1; i<new_label; i++)
		{
		if(tot_sizes[i]>max_size)
			{
			max_size=tot_sizes[i];
			max_label=i;
			}
		}
	//set all labels !=1 -> obj
	for(w=0; w<max_w; w++)
		{
		for(v=0; v<max_v; v++)
			{
			for(u=0; u<max_u; u++)
				{
				label=label_image->get_voxel(u,v,w);
				if(lut[label]==max_label)//First label is background
					this->set_voxel(u,v,w,object_value);
				else //all other labels are object
					this->set_voxel(u,v,w,!object_value);
				}
			}
		}
	
	//this->image_has_changed();
	delete[] changes;
	delete[] sizes;
	delete[] lut;
	delete[] tot_sizes;
	delete label_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::largest_objects_3D(int num_objects, IMGBINARYTYPE object_value)
{
	image_binary<3> *res = new image_binary<3>(this,0);
	res->fill(!object_value);

	image_binary<3> *tmp;
	for(int i=0;i<num_objects;i++){
		cout<<"i="<<i<<endl;
		tmp = new image_binary<3>(this);
		tmp->mask_out(res,!object_value);	//remove already selected objects...
		tmp->largest_object_3D();
		res->combine(tmp,COMB_MAX);
		cout<<"res_vol="<<res->get_number_of_voxels_with_value(object_value)<<endl;
		delete tmp;
	}

	copy_data(res,this);
	delete res;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_2D(int thickness, int direction, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(thickness+1, std::numeric_limits<short>::max(), object_value);
	delete distance_image; 
	copy_data(threshold_image,this);
	delete threshold_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_euclidean_2D(float thickness, int direction, IMGBINARYTYPE object_value)
{		
	bool edge_is_object=false;
    image_scalar<float, IMAGEDIM> * distance_image = distance_2D(edge_is_object, direction, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(thickness+std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::max(), object_value);
	delete distance_image; 
	copy_data(threshold_image,this);
	delete threshold_image;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_2D(int thickness, int direction, IMGBINARYTYPE object_value)
{		
	bool edge_is_object=true;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, !object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(0, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_euclidean_2D(float thickness, int direction, IMGBINARYTYPE object_value)
{		
	bool edge_is_object=true;
    image_scalar<float, IMAGEDIM> * distance_image = distance_2D(edge_is_object, direction, !object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(0, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::outline_2D(int thickness, int direction, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_34_2D(edge_is_object, direction, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(3, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}
	
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_3D(int thickness, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(thickness+1, std::numeric_limits<short>::max(), object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_euclidean_3D(float thickness, IMGBINARYTYPE object_value)
{		
	bool edge_is_object=false;
    image_scalar<float, IMAGEDIM> * distance_image = distance_3D(edge_is_object, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(thickness+std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::max(), object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_3D(int thickness, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=true;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, !object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(0, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_euclidean_3D(float thickness, IMGBINARYTYPE object_value)
{		
	bool edge_is_object=true;
    image_scalar<float, IMAGEDIM> * distance_image = distance_3D(edge_is_object, !object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(0, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::outline_3D(int thickness, IMGBINARYTYPE object_value)
	{		
	bool edge_is_object=false;
    image_integer<short, IMAGEDIM> * distance_image = distance_345_3D(edge_is_object, object_value);
	image_binary <IMAGEDIM> * threshold_image = distance_image->threshold(3, thickness, object_value);
	delete distance_image;
	copy_data(threshold_image,this);
	delete threshold_image;
	}



template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_3D_26Nbh(IMGBINARYTYPE object_value)
{
	dilate_3D_26Nbh(!object_value);
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::erode_3D_26Nbh(int num_iter, IMGBINARYTYPE object_value)
{
	for(int i=0;i<num_iter;i++){
		this->erode_3D_26Nbh(object_value);
	}
}


template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_3D_26Nbh(IMGBINARYTYPE object_value)
{
	int x,y,z,r,s,t;
    image_binary<IMAGEDIM> * res = new image_binary<IMAGEDIM>(this,0); //resulting image...
	res->set_parameters(this); //copy rotation and size infor to tmp image first...

//	cout<<"object_value="<<object_value<<endl;
//	cout<<"!object_value="<<!object_value<<endl;
	res->fill(!object_value);

	int max_x=this->get_size_by_dim(0);
	int max_y=this->get_size_by_dim(1);
	int max_z=this->get_size_by_dim(2);
		
	int radius = 1;

	//x,y,z
	for(z=0; z<max_z; z++){
		for(y=0; y<max_y; y++){
			for(x=0; x<max_x; x++){
				//if you find an object voxel... 
				//fill the whole neighbourhood in the resulting image...
				
				if(this->get_voxel(x,y,z)==object_value){
					//r,s,t
					for(t=std::max(0,z-radius); t<=std::min(max_z-1,z+radius); t++){
						for(s=std::max(0,y-radius); s<=std::min(max_y-1,y+radius); s++){
							for(r=std::max(0,x-radius); r<=std::min(max_x-1,x+radius); r++){
								res->set_voxel(r,s,t,object_value);
							}
						}
					}
				}

			}
		}
	}

	copy_data(res,this);
	delete res;
}

template <int IMAGEDIM>
void image_binary<IMAGEDIM>::dilate_3D_26Nbh(int num_iter, IMGBINARYTYPE object_value)
{
	for(int i=0;i<num_iter;i++){
		this->dilate_3D_26Nbh(object_value);
	}
}

template <int IMAGEDIM>
int image_binary<IMAGEDIM>::find_voxel_index_percent_object_content(int dir, int object_content_percent, IMGBINARYTYPE object_value)
{
	float total = this->get_number_of_voxels_with_value(object_value);
	int p = object_content_percent;
	if(p<0 || p>100){
		pt_error::error("find_voxel_index_percent_object_content - Strange percent value...",pt_error::debug);
	}

	float limit = float(p)/100.0*total;
	float current_number=0;
	int backup_return_value=0;

	switch(dir){
	case 0:
		for(int x=0; x<this->datasize[0]; x++){
			for(int z=0; z<this->datasize[2]; z++){		
				for(int y=0; y<this->datasize[1]; y++){
					if(this->get_voxel(x,y,z) == object_value){
						current_number++;
					}
				}
			}
			if(current_number>=limit){
				return x;
			}
			backup_return_value=x;
		}
		break;
	case 1: 
		for(int y=0; y<this->datasize[1]; y++){
			for(int z=0; z<this->datasize[2]; z++){		
				for(int x=0; x<this->datasize[0]; x++){
					if(this->get_voxel(x,y,z) == object_value){
						current_number++;
					}
				}
			}
			if(current_number>=limit){
				return y;
			}
			backup_return_value=y;
		}
		break;
	case 2: 
		for(int z=0; z<this->datasize[2]; z++){		
			for(int y=0; y<this->datasize[1]; y++){
				for(int x=0; x<this->datasize[0]; x++){
					if(this->get_voxel(x,y,z) == object_value){
						current_number++;
					}
				}
			}
			if(current_number>=limit){
				return z;
			}
			backup_return_value=z;
		}
		break;
	default:
		pt_error::error("find_voxel_index_percent_object_content, erroneous direction...",pt_error::debug);
		break;
	}

	return backup_return_value;
}

/*
template <int IMAGEDIM>
void image_binary<IMAGEDIM>::convex_hull_line_filling_3D(int dir, IMGBINARYTYPE object_value)
{
	int first;
	int last;
	int sx = this->datasize[0];
	int sy = this->datasize[1];
	int sz = this->datasize[2];

	switch(dir){
	case 0:
		for(int z=0; z<sz; z++){		
			for(int y=0; y<sy; y++){
				first=-1;
				last=-1;
				for(int x=0; x<sx; x++){
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = x;
						}else{
							last = x;
						}
					}
				}
				if(last>-1){
					this->draw_line_2D(first,y,last,y,z,object_value);
				}
			}
		}
		break;
	case 1:
		for(int z=0; z<sz; z++){		
			for(int x=0; x<sx; x++){
				first=-1;
				last=-1;
				for(int y=0; y<sy; y++){
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = y;
						}else{
							last = y;
						}
					}
				}
				if(last>-1){
					this->draw_line_2D(x,first,x,last,z,object_value);
				}
			}
		}
		break;
	case 2:
		for(int y=0; y<sy; y++){
			for(int x=0; x<sx; x++){
				first=-1;
				last=-1;
				for(int z=0; z<sz; z++){		
					if(this->get_voxel(x,y,z) == object_value){
						if(first==-1){
							first = z;
						}else{
							last = z;
						}
					}
				}
				if(last>-1){
					for(int z=first; z<=last; z++){	
						this->set_voxel(x,y,z,object_value);
					}
				}
			}
		}
		break;
	default:
		pt_error::error("convex_hull_line_filling_3D, erroneous direction...",pt_error::debug);
		break;
	}

}
*/



template <int IMAGEDIM>
void image_binary<IMAGEDIM>::get_num_neighbours_distribution_3D_26Nbh(vector<int> &num_nb, vector<int> &num_vox, IMGBINARYTYPE object_value)
{
	int nx = this->nx();
	int ny = this->ny();
	int nz = this->nz();
	int tmp;
	
	num_nb.clear();
	num_vox.clear();
	for(int i=0; i<=26; i++){
		num_nb.push_back(i);
		num_vox.push_back(0);
	}

	for(int z=0; z<nz; z++){		
		for(int y=0; y<ny; y++){
			for(int x=0; x<nx; x++){
				if(this->get_voxel(x,y,z)==object_value){
					tmp=0;

					for(int t=std::max(0,z-1); t<=std::min(z+1,nz-1); t++){
						for(int s=std::max(0,y-1); s<=std::min(y+1,ny-1); s++){
							for(int r=std::max(0,x-1); r<=std::min(x+1,nx-1); r++){
								if(this->get_voxel(r,s,t)==object_value){
									tmp++;
								}
							}
						}
					}

					num_vox[tmp]++;

				}

			}
		}
	}

	int inside_limit = 17;
	float border=0;
	float sum=0;

	for(int i=0; i<=26; i++){
		sum += num_vox[i];
		if(i<=inside_limit){
			border += num_vox[i];
		}
	}

	cout<<"num_nb\tnum_vox\tpercent"<<endl;
	for(int i=0; i<=26; i++){
		cout<<num_nb[i]<<"\t"<<num_vox[i]<<"\t"<<float(num_vox[i])/sum<<endl;
	}

	cout<<"border_percent="<<border/sum<<endl;
}


template <int IMAGEDIM>
float image_binary<IMAGEDIM>::get_border_volume_ratio_3D_26Nbh(int num_nb_inside_limit, IMGBINARYTYPE object_value)
{
	vector<int> num_nb;
	vector<int> num_vox;
	this->get_num_neighbours_distribution_3D_26Nbh(num_nb, num_vox, object_value);

	float border=0;
	float sum=0;

	for(int i=0; i<=26; i++){
		sum += num_vox[i];
		if(i<=num_nb_inside_limit){
			border += num_vox[i];
		}
	}
	return border/sum;
}

template <int IMAGEDIM>
float image_binary<IMAGEDIM>::mutual_overlap_3D(image_binary<IMAGEDIM>* second_image)
{
	pt_error::error_if_false(this->same_size(second_image), "images must be of same size in image_binary<IMAGEDIM>::mutual_overlap", pt_error::debug);
		
	int mutual_voxels=0;
	int num_voxels_this=0;
	int num_voxels_second=0;
	for (int z=0; z<this->get_size_by_dim(2); z++)
		for (int y=0; y<this->get_size_by_dim(1); y++)
			for (int x=0; x<this->get_size_by_dim(0); x++) {
				if (this->get_voxel(x,y,z)) {
					num_voxels_this++;
					if (second_image->get_voxel(x,y,z)) {
						mutual_voxels++;
						num_voxels_second++;
					}
				}
				else if (second_image->get_voxel(x,y,z))
					num_voxels_second++;
			}
	if ((num_voxels_this + num_voxels_second)!=0)
		return float(2*mutual_voxels)/float(num_voxels_this + num_voxels_second);
	else
		return 0;
}

template <int IMAGEDIM>
vector<Vector3D> image_binary<IMAGEDIM>::get_center_of_gravities_for_objects_3D(SPACE_TYPE type)
{
	image_label<3> *labels = this->label_connected_objects_3D();
	labels->data_has_changed();
	int max = labels->get_max();
	vector<Vector3D> sums = vector<Vector3D>(max,create_Vector3D(0,0,0));
	vector<int> nums = vector<int>(max,0);

	IMGLABELTYPE val;

	for(int z=0; z<this->nz(); z++){
		for(int y=0; y<this->ny(); y++){
			for(int x=0; x<this->nx(); x++){
				val = labels->get_voxel(x,y,z);
				if(val>0){
					sums[val-1] += create_Vector3D(x,y,z);
					nums[val-1]++;
				}
			}
		}
	}

	for(int i=0; i<max; i++){
		sums[i]/=nums[i];
	}
	
	delete labels;
	
	if(type == VOXEL_SPACE){
		return sums;
	}
		//else PHYSICAL_SPACE

	for(int i=0; i<max; i++){
		sums[i] = this->get_physical_pos_for_voxel(sums[i][0],sums[i][1],sums[i][2]);
	}
	return sums;
}

template <int IMAGEDIM>
image_binary<3>* image_binary<IMAGEDIM>::region_grow_3D_if_lower_intensity_using_dist_thresholding(bool use_dist_345, int dist_thresh, bool use_largest_object)
{
	image_scalar<float,3> *dist;
	image_binary<3> *seeds;

	if(use_dist_345){
		image_integer<short,3> *dist2 = this->distance_345_3D();
		dist = new image_scalar<float,3>(dist2);
		delete dist2;
		seeds = dist->threshold(dist_thresh);
		datamanagement.add(dist,"dist",true);
	}else{
		dist = this->distance_3D(true);
	}
	dist->name("dist");
	//dist->save_to_file(base+"__c01_lung_2_dist.vtk");
	seeds = dist->threshold(dist_thresh);

	if(use_largest_object){
		seeds->largest_object_3D();
	}

	image_binary<3> *res = dist->region_grow_3D_if_lower_intensity(seeds);

//	delete seeds;
	datamanagement.add(dist,"dist",true);
	datamanagement.add(seeds,"seeds",true);
	return res;
}


template <int IMAGEDIM>
void image_binary<IMAGEDIM>::appl_crude_abdominal_artifact_removal()
{
	cout<<"appl_crude_abdominal_artifact_removal..."<<endl;
	image_integer<short,3> *dist = this->distance_345_3D();
	dist->data_has_changed();
	dist->save_to_file("D:/Joel/TMP/res_dist.vtk"); //JK2

	image_binary<3>* seed = dist->threshold_slice_wise_with_slice_max_offsets(1,-1,1);
	seed->data_has_changed();
	seed->name("seed");

	image_binary<3> *res = new image_binary<3>(this,0);
	res->fill(false);

	for(int y=0; y<res->ny(); y++){
		dist->region_grow_declining_in_slice_3D(res,seed,0,y,1);
	}

	res->save_to_file("D:/Joel/TMP/res_grown.vtk"); //JK2
	res->dilate_3D();
	res->erode_3D();
	res->erode_3D();
	res->largest_object_3D();
	res->dilate_3D();
	res->dilate_3D();
	res->dilate_3D();
	res->convex_hull_line_filling_3D(2);
	res->convex_hull_line_filling_3D(0);

	res->name("res");
	res->data_has_changed();

	datamanagement.add(dist,"dist");
//	datamanagement.add(seed,"seed");
	datamanagement.add(res,"res");

	copy_data(res,this);
//	delete dist;
	delete seed;
//	delete res;
}


template <int IMAGEDIM>
plane3D image_binary<IMAGEDIM>::get_plane3D_with_best_fit_to_binary_data(SPACE_TYPE st)
{
	vector<Vector3D> v = this->get_positions_of_voxels_with_value_between(1,255,st);

	Matrix3D ma = cov(v,v);
	cout<<"ma="<<ma<<endl;

	vnl_symmetric_eigensystem<float> vse = vnl_symmetric_eigensystem<float>(ma.GetVnlMatrix());
	for(int i=0;i<3;i++){
		cout<<"eigen = "<<vse.get_eigenvalue(i)<<" --> "<<vse.get_eigenvector(i)<<endl;
	}

	plane3D p(this->get_center_of_gravity_binary_from_range(1,1,st), create_Vector3D(vse.get_eigenvector(2)) );
	return p;
}
