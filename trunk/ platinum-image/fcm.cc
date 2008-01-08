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
	this->u = vnl_matrix<float>(n_clust(), n_pix());
	this->X = vnl_matrix<float>(n_bands(), n_pix());
	this->int_dist = vnl_matrix<float>(n_clust(), n_pix());
}

fcm::fcm(fcm_image_vector_type vec, float m_fuzzyness, vnl_matrix<float> V_init_clusters, float u_diff_limit, image_binary<3> *mask)
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
//	for(int b=0;b<n_bands();b++){
//		this->u_images.push_back(new image_scalar<float>(this->images[0],false));	//degree of membership	(n_bands)
//	}
	for(int c=0;c<n_clust();c++){
		this->int_dist_images.push_back(new image_scalar<float>(this->images[0],false));	//distance in feature space... (n_clust)
		this->u_images.push_back(new image_scalar<float>(this->images[0],false));	//degree of membership	(n_bands)

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

/*
int fcm::get_num_pixels_in_mask(image_binary<3> *image_mask)
{
	image_mask->get_n
	int tmp=0;
	for(int k=0;k<nz();k++){
		for(int j=0;j<ny();j++){
			for(int i=0;i<nx();i++){
				if(image_mask->->get_voxel(i,j,k)>0){
					tmp++;
				}
			}
		}
	}
	return tmp;
}
*/

void fcm::fill_X(vnl_matrix<float> &X, image_binary<3> *image_mask_local)
{
	cout<<"fill_X..."<<endl;
	cout<<"X.rows()="<<X.rows()<<endl;
	cout<<"X.cols()="<<X.cols()<<endl;
	cout<<"n_pix()="<<n_pix()<<endl;

	int tmp=-1;
	if(image_mask_local == NULL){
		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){
					tmp = k*ny()*nx() + j*nx() + i;
					//cout<<tmp<<" ";
					for(int band=0;band<n_bands();band++){
						X(band, tmp) = images[band]->get_voxel(i,j,k);
					}
				}
			}
		}
	}else{
		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){
					if(image_mask_local->get_voxel(i,j,k)>0){
						tmp++;
	//					cout<<tmp<<" ";
						for(int band=0;band<n_bands();band++){
							X(band, tmp) = images[band]->get_voxel(i,j,k);
						}
					}
				}
			}
		}
	}
}



void fcm::calc_int_dist_matrix_euclidean(vnl_matrix<float> &int_dist, const vnl_matrix<float> &X, const vnl_matrix<float> &V)
{
	cout<<"calc_int_dist_matrix_euclidean..."<<endl;

	//int_dist(n_clust, n_pix) --> (i,j) 
	//vnl_matrix<float> X(n_bands, n_pix);		//pixel intensities		X(band,pixel)
	//vnl_matrix<float> V(n_clust, n_bands);		//cluster center		V(clust, band)

	int_dist.fill(0);
	for(int i=0;i<n_clust();i++){
		for(int j=0;j<n_pix();j++){
			for(int band=0;band<n_bands();band++){
				int_dist(i,j) += pow( float(X(band,j)-V(i,band)), float(2.0) );
			}
			int_dist(i,j) = sqrt(int_dist(i,j));
		}
	}
}


void fcm::calc_memberships(vnl_matrix<float> &u, const vnl_matrix<float> &int_dist, const float m)
{
	cout<<"calc_memberships..."<<endl;
	//matrix(rows, columns)
	//vnl_matrix<float> u(n_clust, n_pix);		//degree of membership	u(clust,pixel)
	//vnl_matrix<float> int_dist(n_clust, n_pix);//distance in feature space... for calc speedup
	//float m = 2.0;			//FCM "fuzzyness" (the larger the more focused cluster weighting)

	u.fill(0);
	float denom=0;
	float factor = 2.0/(m-1);
	cout<<"n_clust()="<<n_clust()<<endl;
	cout<<"n_pix()="<<n_pix()<<endl;
	cout<<"u.rows()="<<u.rows()<<endl;
	cout<<"u.cols()="<<u.cols()<<endl;
	cout<<"int_dist.rows()="<<int_dist.rows()<<endl;
	cout<<"int_dist.cols()="<<int_dist.cols()<<endl;

	for(int i=0;i<n_clust();i++){
		cout<<"i="<<i<<endl;
		for(int j=0;j<n_pix();j++){
//			cout<<" "<<j;
			denom=0;
			for(int ii=0;ii<n_clust();ii++){
				if(int_dist(ii,j)>0){ 
					denom += pow( float(int_dist(i,j)/int_dist(ii,j)), factor );
				}
				//if dist==0 it is simply no included...
			}
			u(i,j) = 1.0/denom;
		}
	}
}


void fcm::calc_cluster_centers(vnl_matrix<float> &V, const vnl_matrix<float> &u, const vnl_matrix<float> &X, float m)
{
	cout<<"calc_cluster_centers..."<<endl;
	//matrix(rows, columns)
	//	vnl_matrix<float> u(n_clust, n_pix);		//degree of membership	u(clust,pixel)
	//	vnl_matrix<float> X(n_bands, n_pix);		//pixel intensities		X(band,pixel)
	//	vnl_matrix<float> V(n_clust, n_bands);		//cluster center		V(clust, band)

	V.fill(0);
	float denom;
	float u_tmp;

	for(int i=0;i<n_clust();i++){	//cluster
		denom=0;
		for(int j=0;j<n_pix();j++){	//pix
			u_tmp = pow(u(i,j),m);
			for(int band=0;band<n_bands();band++){	//band
				V(i,band) += u_tmp*X(band,j);
			}
			denom += u_tmp;
		}
		for(int band=0;band<n_bands();band++){	//band
			V(i,band) = V(i,band)/denom;
		}
	}
}


void fcm::calc_int_dist_images_euclidean(const vnl_matrix<float> &V)
{
	cout<<"calc_int_dist_images_euclidean... c="<<endl;
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
	cout<<"calc_memberships... c="<<endl;

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

void fcm::calc_cluster_centers(vnl_matrix<float> &V, const fcm_image_vector_type u_images2, float m)
{
	cout<<"calc_cluster_centers... c="<<endl;
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


fcm_image_vector_type fcm::get_image_vector_from_u_vector()
{
	// vnl_matrix<float> u(n_clust, n_pix);		//degree of membership	u(clust,pixel)
	fcm_image_vector_type vec;
	image_scalar<float,3>* im;
	int tmp=-1;

	for(int c=0;c<n_clust();c++){
		tmp=-1;
		im = new image_scalar<float,3>(nx(),ny(),nz());

		if(image_mask==NULL){
			for(int i=0;i<nx();i++){
				for(int j=0;j<ny();j++){
					for(int k=0;k<nz();k++){
						tmp = i + j*nx() + k*nx()*ny();
						im->set_voxel(i,j,k,u(c,tmp));
					}
				}
			}
		}else{
			for(int k=0;k<nz();k++){
				for(int j=0;j<ny();j++){
					for(int i=0;i<nx();i++){
						if(image_mask->get_voxel(i,j,k)>0){
							tmp++;
							//cout<<tmp<<" ";
							im->set_voxel(i,j,k,u(c,tmp));
						}else{
							im->set_voxel(i,j,k,0);
						}
					}
				}
			}
		}

		im->name("FCM_u" + int2str(c));
		vec.push_back(im);
	}

	return vec;
}





void fcm::Update_vectorfcm()
{
	cout<<"fcm::Update_vectorfcm()..."<<endl;
	//-----------------------
	// Fill X-matrix... One might keep the images and iterate over the... needed when spatial is used...
	//-----------------------
	cout<<"fill_X..."<<endl;
	fill_X(X, image_mask);  //from the "images" vector

	//-----------------------
	// Normalize X-matrix intensities... (row-wise...) (and scale image intensities from 0...max --> 0...1 
	// (one might do more intelligent trimming of the image top intensity values)
	//	vnl_matrix<float> X; //pixel intensities		X(band,pixel)
	//-----------------------
	cout<<"scale..."<<endl;
	for(int band=0;band<n_bands();band++){
		X.scale_row(band,1.0/X.get_row(band).max_value());
	}

	//-----------------------
	// FCM-algorithm Loop...
	//-----------------------
	float u_change_max=1;
	int iter=0;
	float u_diff;
	vnl_matrix<float> u2(n_clust(), n_pix());	//Temp. degree of membership2	u2(clust,pixel)

	//do a first roud outside to allow calculation of "u_change_max"...
	calc_int_dist_matrix_euclidean(int_dist, X, V);
	calc_memberships(u2, int_dist, m);
	calc_cluster_centers(V,u2,X,m);
	u = u2;

	cout<<"loop..."<<endl;

	while(u_change_max > u_maxdiff_limit)
	{ 
		iter++;
		cout<<"fcm iteration = "<<iter<<endl;

		//-----------------------
		// Calc dist_functions... int_dist(n_clust, n_pix) 
		// i.e. distance from each pixel_intensity to each cluster... (for example euclidean...)
		//-----------------------
		calc_int_dist_matrix_euclidean(int_dist, X, V);
		//	cout<<endl<<"int_dist="<<int_dist<<endl;

		//-----------------------
		// Update membership values...
		//-----------------------
		calc_memberships(u2, int_dist, m);

		//-----------------------
		// Update cluster centers values...
		//-----------------------
		calc_cluster_centers(V,u2,X,m);
		cout<<"V="<<endl<<V<<endl;

		u_change_max=0;
		for(int i=0;i<u.rows();i++){
			for(int j=0;j<u.cols();j++){
				u_diff = abs(u2(i,j)-u(i,j));
				if(u_diff > u_change_max){
					u_change_max = u_diff;
				}
			}
		}
		cout<<"u_change_max="<<u_change_max<<endl;

		u = u2;
	}
	cout<<"FCM limit reached..."<<endl;
}



void fcm::Update_imagefcm()
{
	cout<<"fcm::Update_imagefcm()..."<<endl;

	//-----------------------
	// Normalize X-matrix intensities... (row-wise...) (and scale image intensities from 0...max --> 0...1 
	// (one might do more intelligent trimming of the image top intensity values)
	//	vnl_matrix<float> X; //pixel intensities		X(band,pixel)
	//-----------------------
	cout<<"scale..."<<endl;
	for(int b=0;b<n_bands();b++){
		cout<<"band="<<b<<" max="<<images[b]->get_max()<<endl;
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

//	vnl_matrix<float> u2(n_clust(), n_pix());	//Temp. degree of membership2	u2(clust,pixel)
	fcm_image_vector_type u_images2;	//TEMP. degree of membership2	(n_bands)
	for(int c=0;c<n_clust();c++){
		u_images2.push_back(new image_scalar<float>(this->images[0],false));	//degree of membership	(n_bands)
	}

	//do a first roud outside to allow calculation of "u_change_max"...
	calc_int_dist_images_euclidean(V);
	calc_memberships(u_images2, int_dist_images, m);
	calc_cluster_centers(V,u_images2,m);

//	u = u2;

	for(int c=0;c<n_clust();c++){
		for(int k=0;k<nz();k++){
			for(int j=0;j<ny();j++){
				for(int i=0;i<nx();i++){
					u_images[c]->set_voxel(i,j,k, u_images2[c]->get_voxel(i,j,k) );
				}
			}
		}
	}

	cout<<"loop..."<<endl;

	while(u_change_max > u_maxdiff_limit)
	{ 
		iter++;
		cout<<"fcm iteration = "<<iter<<endl;

		//-----------------------
		// Calc dist_functions... int_dist(n_clust, n_pix) 
		// i.e. distance from each pixel_intensity to each cluster... (for example euclidean...)
		//-----------------------
//		calc_int_dist_matrix_euclidean(int_dist, X, V);
		calc_int_dist_images_euclidean(V);
		//	cout<<endl<<"int_dist="<<int_dist<<endl;

		//-----------------------
		// Update membership values...
		//-----------------------
//		calc_memberships(u2, int_dist, m);
		calc_memberships(u_images2, int_dist_images, m);

		//-----------------------
		// Update cluster centers values...
		//-----------------------
//		calc_cluster_centers(V,u2,X,m);
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
	cout<<"FCM limit reached..."<<endl;
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

fcm_image_vector_type fcm::get_membership_images()
{
	return u_images;
}