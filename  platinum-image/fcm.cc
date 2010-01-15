// $Id: fcm.cc  $

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

#include "fcm.h"

fcm::fcm(fcm_image_vector_type vec, vnl_matrix<float> V_init_clusters, float m_fuzzyness, float u_diff_limit, image_binary<3> *mask)
{
	this->images = vec;
	this->V = V_init_clusters;
	this->m = m_fuzzyness;
	this->u_maxdiff_limit = u_diff_limit;
	this->image_mask = mask;
	
	this->n_pix_masked=0;
	if(image_mask !=NULL){
		this->n_pix_masked = image_mask->get_number_of_voxels_with_value(1); //Note that this is assumed...
		this->image_mask->name("fcm-image_mask");
	}

	//initialize the size of the rest of the objects...
	for(int c=0;c<n_clust();c++){
		this->int_dist_images.push_back(new image_scalar<float>(this->images[0],false));//distance in feature space... (n_clust)
		this->u_images.push_back(new image_scalar<float>(this->images[0],false));		//degree of membership	(n_clust)

		this->int_dist_images[c]->name("fcm-int_dist_image_"+int2str(c));
		this->u_images[c]->name("fcm-u_image_"+int2str(c));
	}
}

fcm::~fcm()
{
	cout<<"fcm::~fcm()"<<endl;

	//"images" are not deleted since they might further be used outside class...

	if(image_mask !=NULL){
		delete image_mask;
	}
	for(int c=0;c<n_clust();c++){
		delete this->int_dist_images[c];
		delete this->u_images[c];
	}
}


int fcm::nx()
{return images[0]->get_size_by_dim(0);}

int fcm::ny()
{return images[0]->get_size_by_dim(1);}

int fcm::nz()
{return images[0]->get_size_by_dim(2);}

int fcm::n_pix()
{
	if(image_mask==NULL){
		return nx()*ny()*nz(); 
	}
	return n_pix_masked;
}

int fcm::n_bands()
{return images.size();}

int fcm::n_clust()
{return V.rows();}


bool fcm::is_pixel_included(int i, int j, int k)
{
	if(image_mask==NULL){
		return true;
	}
	else if(image_mask->get_voxel(i,j,k)>0){
		return true;
	}
	return false;
}


void fcm::calc_int_dist_images_euclidean(const vnl_matrix<float> &V)
{
	cout<<"calc_int_dist_images_euclidean... c=";
	float tmp=0;

	for(int c=0;c<n_clust();c++){
		cout<<" "<<c;
		this->int_dist_images[c]->fill(0);	//distance in feature space... (n_clust)

		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){
					if(this->is_pixel_included(i,j,k)){
						//calc euclidean dist in feature from tis pixel position to cluster center
						// = sqrt( dist(band1)^2 + dist(band2)^2 + ... )
						for(int b=0;b<n_bands();b++){
							tmp = pow( float(images[b]->get_voxel(i,j,k) - V(c,b)), float(2.0) );
							this->int_dist_images[c]->set_voxel(i,j,k, this->int_dist_images[c]->get_voxel(i,j,k)+tmp );
						}
						this->int_dist_images[c]->set_voxel(i,j,k, sqrt(this->int_dist_images[c]->get_voxel(i,j,k)));
					}
				}
			}
		}
	}
	cout<<endl;
}

void fcm::calc_memberships(fcm_image_vector_type u_images2, const fcm_image_vector_type &int_dist_images2, const float m)
{
	cout<<"calc_memberships... c=";

	float denom=0;
	float factor = 2.0/(m-1);
	float dist1 = 0;
	float dist2 = 0;
//	cout<<"n_clust()="<<n_clust()<<endl;
//	cout<<"n_pix()="<<n_pix()<<endl;

	for(int c=0;c<n_clust();c++){
		cout<<" "<<c;
		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){

					if(this->is_pixel_included(i,j,k)){
						denom=0;
						dist1 = int_dist_images2[c]->get_voxel(i,j,k);
						for(int cc=0;cc<n_clust();cc++){
							dist2 = int_dist_images2[cc]->get_voxel(i,j,k);
							if(dist2>0){ 
								denom += pow( dist1/dist2, factor );
							}
							//if dist==0 it is simply no included...
						}
						u_images2[c]->set_voxel(i,j,k,1.0/denom);

					}else{
						u_images2[c]->set_voxel(i,j,k,0);
					}

				}
			}
		}
	}
		cout<<endl;
}


void fcm::copy_memberships_from(fcm_image_vector_type u_images2)
{
	for(int c=0;c<n_clust();c++){
		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){
					u_images[c]->set_voxel(i,j,k, u_images2[c]->get_voxel(i,j,k) );
				}
			}
		}
	}

}

void fcm::calc_cluster_centers(vnl_matrix<float> &V, const fcm_image_vector_type u_images2, float m)
{
	cout<<"calc_cluster_centers... c=";
	//matrix(rows, columns)
	//	vnl_matrix<float> u(n_clust, n_pix);		//degree of membership	u(clust,pixel)
	//	vnl_matrix<float> X(n_bands, n_pix);		//pixel intensities		X(band,pixel)
	//	vnl_matrix<float> V(n_clust, n_bands);		//cluster center		V(clust, band)

	V.fill(0);
	float denom;
	float u_tmp;

	for(int c=0;c<n_clust();c++){	//cluster
		cout<<" "<<c;
		denom=0;
		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){
					if(this->is_pixel_included(i,j,k)){
						u_tmp = pow(u_images2[c]->get_voxel(i,j,k),m);
						for(int b=0;b<n_bands();b++){	
							V(c,b) += u_tmp*images[b]->get_voxel(i,j,k);
						}
						denom += u_tmp;
					}
				}
			}
		}
		for(int b=0;b<n_bands();b++){
			V(c,b) = V(c,b)/denom;
		}
	}
	cout<<endl;
}



void fcm::Update_imagefcm(float scale_percentile)
{
	cout<<"fcm::Update_imagefcm()..."<<endl;

	//-----------------------
	// Normalize image intensities... 
	// (one might do more intelligent trimming of the image top intensity values)
	//-----------------------
	cout<<"scale..."<<endl;
	int perc;
	for(int b=0;b<n_bands();b++){
		if(image_mask!=NULL){
			perc = images[b]->get_histogram_from_masked_region_3D(image_mask)->get_intensity_at_histogram_lower_percentile(scale_percentile, false);
		}else{
			perc = images[b]->get_histogram()->get_intensity_at_histogram_lower_percentile(scale_percentile, false);
		}
		cout<<"band="<<b<<" max="<<images[b]->get_max()<<" perc="<<perc<<endl;
		images[b]->map_values(perc,10000000,perc);
		images[b]->scale(0,1);
		images[b]->data_has_changed();
		cout<<"band="<<b<<" max="<<images[b]->get_max()<<endl;
	}
	//-----------------------
	// FCM-algorithm Loop...
	//-----------------------
	float u_change_max=1;
	int iter=0;
	float u_diff;

	fcm_image_vector_type u_images2;	//TEMP. degree of membership2	(n_bands)
	for(int c=0;c<n_clust();c++){
		u_images2.push_back(new image_scalar<float>(this->images[0],false));	//degree of membership	(n_bands)
	}

	//do a first roud outside to allow calculation of "u_change_max"...
	calc_int_dist_images_euclidean(V);
	calc_memberships(u_images2, int_dist_images, m);
	calc_cluster_centers(V,u_images2,m);
	copy_memberships_from(u_images2);


	cout<<"loop..."<<endl;

	while(u_change_max > u_maxdiff_limit)
	{ 
		iter++;
		cout<<"fcm iteration = "<<iter<<endl;

		//-----------------------
		// Calc dist_functions... int_dist(n_clust, n_pix) 
		// i.e. distance from each pixel_intensity to each cluster... (for example euclidean...)
		//-----------------------
		calc_int_dist_images_euclidean(V);
		//	cout<<endl<<"int_dist="<<int_dist<<endl;

		//-----------------------
		// Update membership values...
		//-----------------------
		calc_memberships(u_images2, int_dist_images, m);

		//-----------------------
		// Update cluster centers values...
		//-----------------------
		calc_cluster_centers(V,u_images2,m);
		cout<<"V="<<endl<<V<<endl;

		u_change_max=0;
		for(int c=0;c<n_clust();c++){
			for(int k=0;k<nz();k++){
				for(int j=0;j<ny();j++){
					for(int i=0;i<nx();i++){
						u_diff = abs(u_images2[c]->get_voxel(i,j,k) - u_images[c]->get_voxel(i,j,k));
						if(u_diff > u_change_max){
							u_change_max = u_diff;
						}
					}
				}
			}
		}
		cout<<"u_change_max="<<u_change_max<<endl;

//		u = u2;
		copy_memberships_from(u_images2);

	}
	cout<<"FCM limit reached..."<<endl;

	//free memory.....
	for(int c=0;c<n_clust();c++){
		delete u_images2[c];
	}
}





void fcm::save_membership_images_to_dcm(string file_path_base, float scale_factor)
{
	//itkgdcmImageIO cannot save images with float data, therefore scale and save as ushort
	image_scalar<unsigned short,3>* tmp;
	for(int c=0;c<n_clust();c++){
		u_images[c]->scale_by_factor(scale_factor);
		u_images[c]->data_has_changed();
		tmp = new image_scalar<unsigned short,3>(u_images[c]); 
		tmp->save_to_DCM_file(file_path_base +"_"+ int2str(c)+".dcm");
		delete tmp;
	}
}

void fcm::save_membership_images_to_vtk(string file_path_base)
{
	for(int c=0;c<n_clust();c++){
		u_images[c]->save_to_VTK_file(file_path_base +"_"+ int2str(c)+".vtk");
	}
}

void fcm::save_membership_image_collage_to_vtk(string file_path)
{
	image_scalar<float,3> *tmp = new image_scalar<float,3>(u_images[0]);
	for(int c=1;c<n_clust();c++){
		tmp->add_volume_3D(u_images[c],0);
	}
	tmp->save_to_VTK_file(file_path);
	delete tmp;
}

void fcm::save_int_dist_images(string file_path_base)
{
	for(int c=0;c<n_clust();c++){
		int_dist_images[c]->save_to_file(file_path_base+"_"+int2str(c)+".vtk");
	}
}


fcm_image_vector_type fcm::get_membership_images()
{
	return u_images;
}


void fcm::load_vnl_matrix_from_file(vnl_matrix<float> &V, std::string file_path)
{
	if(file_exists(file_path)){
		std::ifstream myfile;
		myfile.open(file_path.c_str());
		for(int r=0;r<V.rows();r++){
			for(int c=0;c<V.cols();c++){
//				myfile.
	//			V(r,c,) = cin
			}
		}

	//	myfile<<message<<std::endl;
	//	myfile.close();
	}
}

void fcm::save_vnl_matrix_to_file(vnl_matrix<float> &V, std::string file_path)
{
	std::ofstream myfile;
	myfile.open(file_path.c_str());
	for(int r=0;r<V.rows();r++){
		for(int c=0;c<V.cols();c++){
			myfile.write(float2str(V(r,c)).c_str(), sizeof(float));
		}
		myfile.write(string("\n").c_str(),sizeof(char));
	}

//	myfile<<message<<std::endl;
	myfile.close();
}




// -------------------------------------------------------------
// ----------------------- SFCM --------------------------------
// -------------------------------------------------------------

sfcm::sfcm(fcm_image_vector_type vec, vnl_matrix<float> V_init_clusters, float m_fuzzyness, float u_diff_limit, SFCM_NBH_TYPE nbh_type, image_binary<3> *mask) : fcm(vec, V_init_clusters, m_fuzzyness, u_diff_limit, mask)
{

	mean_nbh_dist_image = new image_scalar<float,3>(this->images[0],false);
	mean_nbh_dist_image->fill(0);

	average_nbh_dist_mean = 0;

	for(int c=0;c<n_clust();c++){
		this->dissim_images.push_back(new image_scalar<float>(this->images[0],false));
	}
	this->the_sfcm_nbh_type = nbh_type;
}

sfcm::~sfcm()
{
	cout<<"sfcm::~sfcm()"<<endl;
	delete mean_nbh_dist_image;

	for(int c=0;c<n_clust();c++){
		delete this->dissim_images[c];
	}

	//~fcm() is called after this....
}

float sfcm::get_squared_pixel_int_dist(int i, int j, int k, int i2, int j2, int k2)
{
	float dist=0;
	for(int b=0;b<n_bands();b++){
		dist += pow(images[b]->get_voxel(i,j,k) - images[b]->get_voxel(i2,j2,k2),2);
	}
	return dist;
}

float sfcm::get_pixel_int_dist(int i, int j, int k, int i2, int j2, int k2)
{
	return sqrt(get_squared_pixel_int_dist(i,j,k,i2,j2,k2));
}

float sfcm::calc_lamda(float nbh_dist)
{
	return 1.0/(1.0+exp(-(nbh_dist-average_nbh_dist_mean)/sigma));
}

void sfcm::calc_sigma()
{
	cout<<"calc_sigma()..."<<endl;
	//first determine 95 percentile of delta_av(x)... (i. e. the "mean_nbh_dist_image")
	//then clac sigma by solving eq(8) in Liew2003 for lamda(delta_t)=0.8...
	float delta_t=0;
	if(image_mask==NULL){
		delta_t = mean_nbh_dist_image->get_intensity_at_lower_percentile(0.95, false);
	}else{
		delta_t = mean_nbh_dist_image->get_histogram_from_masked_region_3D(image_mask)->get_intensity_at_histogram_lower_percentile(0.95, false);
	}
	
	cout<<"delta_t="<<delta_t<<endl;

	this->sigma = -(delta_t - average_nbh_dist_mean) / log(1/0.8-1);
	cout<<"sigma="<<sigma<<endl;

}

void sfcm::calc_mean_nbh_dist_image_6NBH()
{
	float d[6];
	float mean;

	for(int k=1;k<nz()-1;k++){
		for(int j=1;j<ny()-1;j++){
			for(int i=1;i<nx()-1;i++){
				if(this->is_pixel_included(i,j,k)){			//check mask...
					d[0] = get_pixel_int_dist(i,j,k,i,j,k-1);
					d[1] = get_pixel_int_dist(i,j,k,i,j,k+1);
					d[2] = get_pixel_int_dist(i,j,k,i,j-1,k);
					d[3] = get_pixel_int_dist(i,j,k,i,j+1,k);
					d[4] = get_pixel_int_dist(i,j,k,i-1,j,k);
					d[5] = get_pixel_int_dist(i,j,k,i+1,j,k);
					mean = 0;
					for(int n=0;n<6;n++){
						mean += d[n];
					}
					mean /= 6.0;
					this->mean_nbh_dist_image->set_voxel(i,j,k,mean);
					this->average_nbh_dist_mean += mean;
				}
			}//x
		}//y
	}//z
	this->average_nbh_dist_mean /= n_pix();
	this->mean_nbh_dist_image->data_has_changed();

	cout<<"average_nbh_dist_mean (6NBH) ="<<this->average_nbh_dist_mean<<endl;
}

void sfcm::calc_mean_nbh_dist_image_4NBH()
{
	float d[4];
	float mean;

	for(int k=0;k<nz();k++){
		for(int j=1;j<ny()-1;j++){
			for(int i=1;i<nx()-1;i++){
				if(this->is_pixel_included(i,j,k)){			//check mask...
					d[0] = get_pixel_int_dist(i,j,k,i,j-1,k);
					d[1] = get_pixel_int_dist(i,j,k,i,j+1,k);
					d[2] = get_pixel_int_dist(i,j,k,i-1,j,k);
					d[3] = get_pixel_int_dist(i,j,k,i+1,j,k);
					mean = 0;
					for(int n=0;n<4;n++){
						mean += d[n];
					}
					mean /= 4.0;
					this->mean_nbh_dist_image->set_voxel(i,j,k,mean);
					this->average_nbh_dist_mean += mean;
				}
			}//x
		}//y
//		for(int j=0;j<ny();j=j+ny()-1){
//		}
	}//z
	this->average_nbh_dist_mean /= n_pix();
	this->mean_nbh_dist_image->data_has_changed();

	cout<<"average_nbh_dist_mean (4NBH) ="<<this->average_nbh_dist_mean<<endl;
}

float sfcm::calc_dissimilarity_6NBH(int c, int i, int j, int k)
{
	float dkx2 = pow(int_dist_images[c]->get_voxel(i,j,k),2);

	float dky2[6];
	dky2[0] = pow(int_dist_images[c]->get_voxel(i,j,k-1),2);
	dky2[1] = pow(int_dist_images[c]->get_voxel(i,j,k+1),2);
	dky2[2] = pow(int_dist_images[c]->get_voxel(i,j-1,k),2);
	dky2[3] = pow(int_dist_images[c]->get_voxel(i,j+1,k),2);
	dky2[4] = pow(int_dist_images[c]->get_voxel(i-1,j,k),2);
	dky2[5] = pow(int_dist_images[c]->get_voxel(i+1,j,k),2);

	float dxy[6];
	dxy[0] = get_pixel_int_dist(i,j,k,i,j,k-1);
	dxy[1] = get_pixel_int_dist(i,j,k,i,j,k+1);
	dxy[2] = get_pixel_int_dist(i,j,k,i,j-1,k);
	dxy[3] = get_pixel_int_dist(i,j,k,i,j+1,k);
	dxy[4] = get_pixel_int_dist(i,j,k,i-1,j,k);
	dxy[5] = get_pixel_int_dist(i,j,k,i+1,j,k);

	float lxy[6];
	for(int i=0;i<6;i++){
		lxy[i] = this->calc_lamda(dxy[i]);
	}

	float dissim=0;
	for(int i=0;i<6;i++){
		dissim += dkx2*lxy[i] + dky2[i]*(1.0-lxy[i]);
	}
	dissim = dissim/6.0;

	return dissim;
}

float sfcm::calc_dissimilarity_4NBH(int c, int i, int j, int k)
{
	float dkx2 = pow(int_dist_images[c]->get_voxel(i,j,k),2);

	float dky2[4];
	dky2[0] = pow(int_dist_images[c]->get_voxel(i,j-1,k),2);
	dky2[1] = pow(int_dist_images[c]->get_voxel(i,j+1,k),2);
	dky2[2] = pow(int_dist_images[c]->get_voxel(i-1,j,k),2);
	dky2[3] = pow(int_dist_images[c]->get_voxel(i+1,j,k),2);

	float dxy[4];
	dxy[0] = get_pixel_int_dist(i,j,k,i,j-1,k);
	dxy[1] = get_pixel_int_dist(i,j,k,i,j+1,k);
	dxy[2] = get_pixel_int_dist(i,j,k,i-1,j,k);
	dxy[3] = get_pixel_int_dist(i,j,k,i+1,j,k);

	float lxy[4];
	for(int i=0;i<4;i++){
		lxy[i] = this->calc_lamda(dxy[i]);
	}

	float dissim=0;
	for(int i=0;i<4;i++){
		dissim += dkx2*lxy[i] + dky2[i]*(1.0-lxy[i]);
	}
	dissim = dissim/4.0;

	return dissim;
}



void sfcm::calc_dissimilarity_images(const vnl_matrix<float> &V)
{
	cout<<"calc_dissimilarity_images... c= ";
	float tmp=0;

	for(int c=0;c<n_clust();c++){
		cout<<" "<<c;
		if(image_mask!=NULL){
			this->dissim_images[c]->fill(0);	//JK-time consuming but needed when a mask is used...
		}else{
			this->dissim_images[c]->fill_image_border_3D(0);
		}

		if(the_sfcm_nbh_type == SFCM_4NBH){
			for(int k=1;k<nz()-1;k++){
				for(int j=1;j<ny()-1;j++){
					for(int i=1;i<nx()-1;i++){
						if(this->is_pixel_included(i,j,k)){
							this->dissim_images[c]->set_voxel(i,j,k,this->calc_dissimilarity_4NBH(c,i,j,k));
						}
					}
				}
			}
		}else if(the_sfcm_nbh_type == SFCM_6NBH){
			for(int k=1;k<nz()-1;k++){
				for(int j=1;j<ny()-1;j++){
					for(int i=1;i<nx()-1;i++){
						if(this->is_pixel_included(i,j,k)){
							this->dissim_images[c]->set_voxel(i,j,k,this->calc_dissimilarity_6NBH(c,i,j,k));
						}
					}
				}
			}

		}
	}
	cout<<endl;

}

void sfcm::Update_imagesfcm(float scale_percentile)
{
	cout<<"sfcm::Update_imagesfcm()..."<<endl;
	string path = "D:/Joel/TMP/";
	//-----------------------
	// Normalize image intensities... (row-wise...) (and scale image intensities from 0...max --> 0...1 
	// (one might do more intelligent trimming of the image top intensity values)
	//-----------------------
	float intensity_at_percentile;
	cout<<"scale..."<<endl;

	for(int b=0;b<n_bands();b++){
		if(image_mask!=NULL){
			intensity_at_percentile = images[b]->get_histogram_from_masked_region_3D(image_mask)->get_intensity_at_histogram_lower_percentile(scale_percentile, false);
		}else{
			intensity_at_percentile = images[b]->get_histogram()->get_intensity_at_histogram_lower_percentile(scale_percentile, false);
		}
		cout<<"band="<<b<<" max="<<images[b]->get_max()<<" intensity_at_percentile="<<intensity_at_percentile<<endl;
		images[b]->map_values(intensity_at_percentile,10000000,intensity_at_percentile);
		images[b]->scale(0,1);
		images[b]->data_has_changed();
		cout<<"band="<<b<<" max="<<images[b]->get_max()<<endl;
	}
	//-----------------------
	// FCM-algorithm Loop...
	//-----------------------
	float u_change_max=1;
	int iter=0;
	float u_diff;

	fcm_image_vector_type u_images2;	//TEMP. degree of membership2	(n_bands)
	for(int c=0;c<n_clust();c++){
		u_images2.push_back(new image_scalar<float>(this->images[0],false));	//degree of membership	(n_bands)
	}



	//do a first roud outside to allow calculation of "u_change_max"...
	calc_int_dist_images_euclidean(V);
//	save_int_dist_images(path+"sfcm0_int_dist");

	//calc sfcm objects....
	if(the_sfcm_nbh_type == SFCM_6NBH){
		calc_mean_nbh_dist_image_6NBH();	//only needed once...

	}else if(the_sfcm_nbh_type == SFCM_4NBH){
		calc_mean_nbh_dist_image_4NBH();	//only needed once...
	}

	save_mean_nbh_dist_image("sfcm1_mean_dist.vtk");
	calc_sigma();				//only needed once...

	calc_dissimilarity_images(V);	//sfcm
//	save_dissimilarity_images(path+"sfcm2_dissim_images");

	calc_memberships(u_images2, dissim_images, m);
	for(int c=0;c<n_clust();c++){
		u_images2[c]->fill_image_border_3D(0); //have to clear outer border....
	}

	for(int c=0;c<n_clust();c++){
		u_images2[c]->save_to_VTK_file(path +"sfcm3_u_images2_"+ int2str(c)+".vtk");
	}

	calc_cluster_centers(V,u_images2,m);
	cout<<"V="<<V<<endl;


//	u = u2;
	copy_memberships_from(u_images2);

	cout<<"loop..."<<endl;

	while(u_change_max > u_maxdiff_limit)
	{ 
		iter++;
		cout<<"sfcm iteration = "<<iter<<endl;

		//-----------------------
		// Calc dist_functions... int_dist(n_clust, n_pix) 
		// i.e. distance from each pixel_intensity to each cluster... (for example euclidean...)
		//-----------------------
		calc_int_dist_images_euclidean(V);
		calc_dissimilarity_images(V);
//		save_dissimilarity_images(path+"sfcm5_dissim_images_iter_"+int2str(iter));
		//	cout<<endl<<"int_dist="<<int_dist<<endl;

		//-----------------------
		// Update membership values...
		//-----------------------
//		calc_memberships(u_images2, int_dist_images, m);
		calc_memberships(u_images2, dissim_images, m);

		for(int c=0;c<n_clust();c++){
			u_images2[c]->fill_image_border_3D(0); //have to clear outer border....
		}

//		for(int c=0;c<n_clust();c++){
//			u_images2[c]->save_to_VTK_file(path +"sfcm6_u_images2_"+int2str(iter)+"_"+int2str(c)+".vtk");
//		}

		//-----------------------
		// Update cluster centers values...
		//-----------------------
		calc_cluster_centers(V,u_images2,m);
		cout<<"V="<<endl<<V<<endl;

		u_change_max=0;
		for(int c=0;c<n_clust();c++){
			for(int k=0;k<nz();k++){
				for(int j=0;j<ny();j++){
					for(int i=0;i<nx();i++){
						u_diff = abs(u_images2[c]->get_voxel(i,j,k) - u_images[c]->get_voxel(i,j,k));
						if(u_diff > u_change_max){
							u_change_max = u_diff;
						}
					}
				}
			}
		}
		cout<<"u_change_max="<<u_change_max<<endl;

		copy_memberships_from(u_images2);

	}


	cout<<"sFCM limit reached..."<<endl;

	//free memory.....
	for(int c=0;c<n_clust();c++){
		delete u_images2[c];
	}

}


void sfcm::save_mean_nbh_dist_image(string file_path)
{
	mean_nbh_dist_image->save_to_file(file_path);
}

void sfcm::save_dissimilarity_images(string file_path_base)
{
	for(int c=0;c<n_clust();c++){
		dissim_images[c]->save_to_file(file_path_base+"_"+int2str(c)+".vtk");
	}
}
