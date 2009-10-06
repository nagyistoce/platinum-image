//////////////////////////////////////////////////////////////////////////
//
//   fcm - Fuzzy c-means $Revision: $
///
///  Class for FCM segmentation of multispectral data
///  Planned extensions are sFCM (where spatial information of the voxel neighbourhoods are included)
///  and inuFCM (Intensity Non Uniformity) where a smoothly varying model compensates for INU.
///
//   $LastChangedBy: joel.kullberg $

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

#ifndef __fcm__
#define __fcm__

#include "string.h"
#include <vector>
#include "global.h"
#include "image_binary.hxx"
//#include "image_integer.hxx"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

enum SFCM_NBH_TYPE {SFCM_4NBH, SFCM_6NBH};

typedef vector<image_scalar<float,3>*> fcm_image_vector_type;

//#include <sstream>
using namespace std;


class fcm
{

protected:
	fcm_image_vector_type images;
	float m;						// FCM "fuzzyness" (the larger the more focused cluster weighting)
	float u_maxdiff_limit;			// Stop criterion for iterating solution (when u show less variation...)
	image_binary<3> *image_mask;	// A binary mask for limiting inclusion of multispectral data

	vnl_matrix<float> V;			//cluster center		V(n_clust, n_bands)

	// data structures for image-based FCM (simplifies inclusion of spatial info)
	fcm_image_vector_type u_images;	//degree of membership	(n_clust)
	fcm_image_vector_type int_dist_images;	//distance in feature space... / int_dist(n_clust)... for calc speedup
											


	int nx();			//image size in x-direction
	int ny();			//image size in y-direction
	int nz();			//image size in z-direction
	int n_pix_masked;
	int n_pix();		//total number of pixels (=nx*ny*nz) alt. n_pix_masked...
	int n_bands();		//number bands (image contrasts) (e.g. T1w, T2w)
	int n_clust();		//number of clusters sought (e.g. background, grey matter, white matter)

	bool is_pixel_included(int i, int j, int k); //if image mask == NULL och pixel inside mask

	// ------ Vector-based version of help functions... --------
	// Following functions implement subrutines of the fcm iteration...
	// they are called with arguments (though not needed) to clarify involved data structures.
//	int get_num_pixels_in_mask(image_binary<3> *image_mask);
//	void fill_X(vnl_matrix<float> &X, image_binary<3> *image_mask=NULL);
//	void calc_int_dist_matrix_euclidean(vnl_matrix<float> &int_dist, const vnl_matrix<float> &X, const vnl_matrix<float> &V);
//	void calc_memberships(vnl_matrix<float> &u, const vnl_matrix<float> &int_dist, const float m);
//	void calc_cluster_centers(vnl_matrix<float> &V, const vnl_matrix<float> &u, const vnl_matrix<float> &X, float m);

	// ------ Image-based version of help functions... --------
	void calc_int_dist_images_euclidean(const vnl_matrix<float> &V);
	void calc_memberships(fcm_image_vector_type u_images, const fcm_image_vector_type &int_dist_images, const float m);
	void copy_memberships_from(fcm_image_vector_type u_images2);
	void calc_cluster_centers(vnl_matrix<float> &V, const fcm_image_vector_type u_images, float m);

public:
//	fcm(fcm_image_vector_type vec, vnl_matrix<float> V_init_clusters, float m_fuzzyness=2, float u_maxdiff_limit=0.05, image_binary<3> *mask=NULL);
	fcm(fcm_image_vector_type vec, vnl_matrix<float> V_init_clusters, float m_fuzzyness=2, float u_maxdiff_limit=0.05, image_binary<3> *mask=NULL);
	~fcm();

//	void Update_vectorfcm(); //executes fcm algorithm ( in sweet ITK style... ;-)  )...
	void Update_imagefcm(float scale_percentile=0.95); //executes fcm algorithm ( in sweet ITK style... ;-)  )...
	void save_membership_images_to_dcm(string file_path_base, float scale_factor=1000); 
	void save_membership_images_to_vtk(string file_path_base); 
	void save_membership_image_collage_to_vtk(string file_path_base); 
	void save_int_dist_images(string file_path_base);

	fcm_image_vector_type get_membership_images();
	
	template <class ELEMTYPE>
		vector<image_scalar<ELEMTYPE,3>*> get_membership_images(float scale_factor=1000.0){
	
			vector<image_scalar<float,3>*> v = get_membership_images();
			vector<image_scalar<ELEMTYPE,3>*> v2;
			for(int i=0;i<v.size();i++){
				v[i]->scale_by_factor(scale_factor);
				v2.push_back( new image_scalar<ELEMTYPE,3>(v[i]) );
				v2[i]->data_has_changed();
				v[i]->scale_by_factor(1/scale_factor);
			}

			return v2;
		}

//	fcm_image_vector_type get_image_vector_from_u_vector(); //note that geometrical info is not reconstructed...
	static void load_vnl_matrix_from_file(vnl_matrix<float> &V, std::string file_path);
	static void save_vnl_matrix_to_file(vnl_matrix<float> &V, std::string file_path);
};



// -------------------------------------------------------------
// ----------------------- SFCM --------------------------------
// -------------------------------------------------------------


// Class for inclusion of spatial naighbourhood information into the fcm algorith. Adapted from
// Liew2003 - Liew AW, Yan H. An adaptive spatial fuzzy clustering algorithm for 3-D MR image segmentation.
// IEEE Trans Med Imaging. 2003 Sep;22(9):1063-75.

class sfcm : public fcm
{
protected:
	fcm_image_vector_type	dissim_images;			//dissimilarity	(n_clust)
	image_scalar<float,3>*	mean_nbh_dist_image;	//denoted delta_av(x) in Liew2003
	float					average_nbh_dist_mean;	//;-) denoted my in Liew2003
	float					sigma;					//;-) denoted sigma in Liew2003, regulates the dregee of influence of the  
	SFCM_NBH_TYPE			the_sfcm_nbh_type;

	float get_squared_pixel_int_dist(int i, int j, int k, int i2, int j2, int k2);
	float get_pixel_int_dist(int i, int j, int k, int i2, int j2, int k2); //denoted delta in Liew2003

	void calc_dissimilarity_images(const vnl_matrix<float> &V);

public:
	sfcm(fcm_image_vector_type vec, vnl_matrix<float> V_init_clusters, float m_fuzzyness=2, float u_maxdiff_limit=0.05, SFCM_NBH_TYPE=SFCM_6NBH, image_binary<3> *mask=NULL);
	~sfcm();

	float calc_lamda(float nbh_dist); //denoted lamda(delta) in Liew2003 (uses: average_nbh_dist_mean / sigma)
	void calc_sigma();
	void calc_mean_nbh_dist_image_6NBH();
	void calc_mean_nbh_dist_image_4NBH();
	float calc_dissimilarity_6NBH(int c, int i, int j, int k); //denoted dissimilarity index (D_kx) in Liew2003
	float calc_dissimilarity_4NBH(int c, int i, int j, int k); //denoted dissimilarity index (D_kx) in Liew2003

	void Update_imagesfcm(float scale_percentile=0.99); //executes sfcm algorithm ( in sweet ITK style... ;-)  )...

	void save_mean_nbh_dist_image(string file_path);
	void save_dissimilarity_images(string file_path_base);

};




#endif __fcm__