//////////////////////////////////////////////////////////////////////////
//
//  Image_binary $Revision$
//
//  Image type with binary voxel/pixel values
//  Implemented like image_label, only that processing expects 0/1 only
//
//  $LastChangedBy$
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

#ifndef __image_binary__
#define __image_binary__

#include "image_label.h"

#include <vnl/algo/vnl_powell.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>


template<int IMAGEDIM = 3>
class image_binary : public image_label <IMAGEDIM>
    {
public:
	image_binary(int w, int h, int d, IMGBINARYTYPE *ptr = NULL):image_label<IMAGEDIM>(w, h, d, ptr)
	{}

	template<class SOURCETYPE>
    image_binary(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true) : image_label<IMAGEDIM>(old_image, copyData)//!copy constructor
	{}

	image_binary(std::vector<std::string> files, long width, long height, bool bigEndian = false, long headerSize = 0, Vector3D voxelSize = Vector3D (1,1,4), unsigned int startFile = 1,unsigned int increment = 1): image_label<IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment) 
	{}

	image_binary<IMAGEDIM>(itk::SmartPointer< itk::OrientedImage<IMGBINARYTYPE, IMAGEDIM > > &i);
	image_binary(const string filepath, const string name="");
//	image_binary<IMAGEDIM>(itk::SmartPointer< itk::OrientedImage<IMGBINARYTYPE, IMAGEDIM > > &i):image_label<IMAGEDIM>(i){}
//	image_binary(const string filepath, const string name=""):image_label<IMAGEDIM>(filepath, name){}

	string resolve_tooltip();				//combines tooltip data of this class with data from other classes
	string resolve_tooltip_image_binary(); //resolves tooltip data typical for this class

	image_binary<IMAGEDIM>* get_subvolume_from_slice_3D(int slice, int dir=2);

	image_binary<IMAGEDIM>* get_subvolume_from_region_3D(image_binary<3> *mask, IMGBINARYTYPE object_value=TRUE);
	image_binary<IMAGEDIM>* get_subvolume_from_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size);
	image_binary<IMAGEDIM>* get_subvolume_from_region_3D(int x1, int y1, int z1, int x2, int y2, int z2);

    // *** operations ***

    image_binary<IMAGEDIM> * logical_or(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A OR B operation
    image_binary<IMAGEDIM> * logical_and(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A AND B operation
    image_binary<IMAGEDIM> * logical_xor(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A XOR B operation
    image_binary<IMAGEDIM> * logical_or_not(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A OR (NOT B) operation
    image_binary<IMAGEDIM> * logical_and_not(image_binary<IMAGEDIM> *input, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A AND (NOT B) operation
    image_binary<IMAGEDIM> * logical_or_prev_and_next(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A OR (PrevSlice AND NextSlice) operation
    image_binary<IMAGEDIM> * logical_and_prev_or_next(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Perform a voxelwise logical A OR (PrevSlice AND NextSlice) operation
    void invert(); ///Perform a voxelwise inversion
	void get_smallest_box_containing_entire_object_3D(int& x_0, int& y_0, int& z_0, int& x_1, int& y_1, int& z_1, IMGBINARYTYPE object_value=true); //Get coordinates constituting the smallest possible box contatining all object voxels

	image_binary<IMAGEDIM>* expand_borders(unsigned int dx, unsigned int dy, unsigned int dz, IMGBINARYTYPE value=0);	
	image_binary<IMAGEDIM>* expand_borders2D_by_dir(int dir, unsigned int dr=1, IMGBINARYTYPE value=0);	
//	virtual image_base* expand_borders(unsigned int dx, unsigned int dy, unsigned int dz, IMGBINARYTYPE value=0);	
//	virtual image_base* expand_borders2D_by_dir(int dir, unsigned int dr=1, IMGBINARYTYPE value=0);	
	image_binary<IMAGEDIM>* contract_borders(unsigned int dx, unsigned int dy, unsigned int dz);
	image_binary<IMAGEDIM>* contract_borders2D_by_dir(int dir, unsigned int dr=1);
//	virtual image_base* contract_borders(unsigned int dx, unsigned int dy, unsigned int dz);	
//	virtual image_base* contract_borders2D_by_dir(int dir, unsigned int dr=1);

    // *** applications ***
	
	void fit_ellipsoid3D_to_this_image3D(ellipsoid3D *e, vector<double> min_constraints, vector<double> max_constraints, double non_object_cost=3, double object_cost=-1); // Will only converge with initial parameters of ellipsoid_3D close to optimal

    // ----------- 2D operations in image_binaryprocess ---------------

	image_binary<3>* convex_hull_2D(int dir=2); // Get convex hull in 2D-planes orthogonal to the axis given by direction.
	image_binary<3>* convex_hull_objectwise_2D(int dir=2); // Get convex hull of each 4-connected object in 2D-planes orthogonal to the axis given by direction.
	void convex_hull_in_slice_2D(image_binary<3>* image, int dir=2, int slice=0); // Get convex hull in specified 2D-plane orthogonal to the axis given by direction.
	void convex_hull_objectwise_in_slice_2D(image_label<3>* image, int dir=2, int slice=0); // Get convex hull of each 4-connected object in specified 2D-plane orthogonal to the axis given by direction.
    void fill_holes_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Fill holes in objects defined by object_value in 2D-planes orthogonal to the axis given by direction.
    image_label<3>* label_connected_objects_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Get image_label from connected objects in 2D-planes orthogonal to the axis given by direction.
	image_integer<unsigned long, 3>* label_connected_objects_with_area_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Get image_integer from connected objects in 2D-planes orthogonal to the axis given by direction with labels corresponding to object volume (4-nbh).
	image_integer<unsigned long, 3>* label_connected_objects_with_perimeter_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Get image_integer from connected objects in 2D-planes orthogonal to the axis given by direction with labels corresponding to object perimeter (4-nbh).
	image_scalar<float, 3>* label_connected_objects_with_p2a_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); //Uses "perimeter_2D" and "area_2D"
	void largest_object_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Keep the largest object (defined by object_value) for each plane in 2D-planes orthogonal to the axis given by direction.
    void threshold_size_2D(int min_size, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Keep all objects (defined by object_value) > min_size voxels for each plane in 2D-planes orthogonal to the axis given by direction.
    void cog_inside_2D(image_binary<IMAGEDIM>* mask, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Keep all objects (defined by object_value) having their cog within mask in 2D-planes orthogonal to the axis given by direction.
	void erode_2D(int thickness=3, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Morphological erode up to distance value=thickness for each plane in 2D-planes orthogonal to the axis given by direction, using the 34 chamfer distance map.
	void erode_euclidean_2D(float thickness, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Morphological erode up to distance value=thickness for each plane in 2D-planes orthogonal to the axis given by direction, using the additative euclidean distance map.	
	void dilate_2D(int thickness=3, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Morphological dilate up to distance value=thickness for each plane in 2D-planes orthogonal to the axis given by direction, using the 34 chamfer distance map.
	void dilate_euclidean_2D(float thickness, int direction=2, IMGBINARYTYPE object_value=TRUE)	; ///Morphological dilate up to distance value=thickness for each plane in 2D-planes orthogonal to the axis given by direction, using the additative euclidean distance map.
	void outline_2D(int thickness=3, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Morphological outline up to distance value=thickness for each plane in 2D-planes orthogonal to the axis given by direction.
    void connect_outer_2D(int direction=2, IMGBINARYTYPE object_value=TRUE); ///Connect outer contour of objects (defined by object_value) for each plane in 2D-planes orthogonal to the axis given by direction.
	image_integer<short, IMAGEDIM> * distance_34_2D(bool edge_is_object=false, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Compute 34 chamfer distance map for each plane in 2D-planes orthogonal to the axis given by direction. If edge_is_object=true then everything outside the image is regarded to be object voxels.
	image_scalar<float, IMAGEDIM>* distance_2D(bool edge_is_object=false, int direction=2, IMGBINARYTYPE object_value=TRUE); ///Compute distance map where distances are additively computed as physical euclidean distance from voxels in the 26-neighbourhood. If edge_is_object=true then everything outside the image is regarded to be object voxels.

	void get_num_neighbours_distribution_in_slice_2D_4Nbh(vector<int> &num_vox, int slice, int dir=2, IMGBINARYTYPE object_value=TRUE);
	void get_num_neighbours_distribution_in_slice_2D_8Nbh(vector<int> &num_vox, int slice, int dir=2, IMGBINARYTYPE object_value=TRUE);

    // --------------- 3D operations in image_binaryprocess ---------------

    void fill_holes_3D(IMGBINARYTYPE object_value=TRUE); ///Fill holes in objects defined by object_value.
	image_label<3>* label_connected_objects_3D(IMGBINARYTYPE object_value=TRUE); ///Get image_label from connected objects in binary.
	image_integer<unsigned long,3>* label_connected_objects_with_volume_3D(IMGBINARYTYPE object_value=TRUE); ///Get image_integer from connected objects in binary with labels corresponding to object volume.
	void largest_object_3D(IMGBINARYTYPE object_value=TRUE); ///Keep the largest object (defined by object_value).
	void largest_objects_3D(int num_objects=2, IMGBINARYTYPE object_value=TRUE); ///Keep the x largest objects (defined by object_value).
	void erode_3D(int thickness=3, IMGBINARYTYPE object_value=TRUE);///Morphological erode up to distance value=thickness.
	void erode_euclidean_3D(float thickness, IMGBINARYTYPE object_value=TRUE); // Morphological erode up to physical euclidean distance value=thickness in mm.
	//JK - there is a bug in dilate3D... An in-slice line (with 2 segments...) is sometimes seen....
	void dilate_3D(int thickness=3, IMGBINARYTYPE object_value=TRUE); ///Morphological dilate up to distance value=thickness.
	void dilate_euclidean_3D(float thickness, IMGBINARYTYPE object_value=TRUE); // Morphological dilate up to physical euclidean distance value=thickness in mm.
	void outline_3D(int thickness=3, IMGBINARYTYPE object_value=TRUE); ///Morphological outline up to distance value=thickness.
    image_scalar<float, IMAGEDIM>* distance_3D(bool edge_is_object=false, IMGBINARYTYPE object_value=TRUE); ///Compute distance map where distances are additively computed as physical euclidean distance from voxels in the 26-neighbourhood. If edge_is_object=true then everything outside the image is regarded to be object voxels.
	image_integer<short, IMAGEDIM> * distance_345_3D(bool edge_is_object=false, IMGBINARYTYPE object_value=TRUE); ///Compute 345 chamfer distance map. If edge_is_object=true then everything outside the image is regarded to be object voxels.
	image_integer<short, IMAGEDIM>* distance_chessboard_3D(bool edge_is_object=false, IMGBINARYTYPE object_value=TRUE); ///Compute chessboard distance map. If edge_is_object=true then everything outside the image is regarded to be object voxels.
	//TODO: create distancetransform where weights are given as argument (3,4,5)... can then be used to weight spatial resolution....
	vector<image_integer<short, IMAGEDIM>* > distance_path_to_border_3D(image_binary<IMAGEDIM>* rim, bool weighted); //rim is the outer border of the object which are the goal points (distance = 0)
	int partition_quicksort(image_integer<short, IMAGEDIM>* dist, vector<Vector3D> &Q, int top, int bottom);
	void sort_queue(image_integer<short, IMAGEDIM>* dist,vector<Vector3D> &Q, int top, int bottom);
	bool dijkstra_update(image_integer<short, IMAGEDIM>* dist, int x, int y, int  z, short alt);
	image_integer<short,IMAGEDIM>* dijkstra_image_version(image_integer<short, IMAGEDIM>* dist, vector<Vector3D> Q,image_integer<short, IMAGEDIM>* weight);


	void erode_3D_26Nbh(IMGBINARYTYPE object_value=TRUE); ///Morphological erode (26 Neighbourhood)
	void erode_3D_26Nbh(int num_iter, IMGBINARYTYPE object_value); ///Morphological dilate (26 Neighbourhood)
	void dilate_3D_26Nbh(IMGBINARYTYPE object_value=TRUE); ///Morphological dilate (26 Neighbourhood)
	void dilate_3D_26Nbh(int num_iter, IMGBINARYTYPE object_value); ///Morphological dilate (26 Neighbourhood)
	int find_voxel_index_percent_object_content(int dir, int object_content_percent, IMGBINARYTYPE object_value=TRUE); 
//	void convex_hull_line_filling_3D(int dir, IMGBINARYTYPE object_value=TRUE); 
//	void convex_hull_line_filling_in_two_dirs_3D(int dir1, int dir2, IMGBINARYTYPE object_value=TRUE); 
//	void convex_hull_line_filling_inplane_3D(int dir, IMGBINARYTYPE object_value=TRUE); 

	void get_num_neighbours_distribution_3D_26Nbh(vector<int> &num_nb, vector<int> &num_vox, IMGBINARYTYPE object_value=TRUE); //Returns statistics on the number of object neighbours object voxels have.
	float get_border_volume_ratio_3D_26Nbh(int num_nb_inside_limit=17, IMGBINARYTYPE object_value=TRUE); //Returns statistics on the number of object neighbours object voxels have.
	float mutual_overlap_3D(image_binary<IMAGEDIM>* second_image);
	vector<Vector3D> get_center_of_gravities_for_objects_3D(SPACE_TYPE type = VOXEL_SPACE); //oooo

	image_binary<3>* region_grow_3D_if_lower_intensity_using_dist_thresholding(int dist_thresh=10);

	void appl_crude_abdominal_artifact_removal();

	plane3D get_plane3D_with_best_fit_to_binary_data(SPACE_TYPE st=VOXEL_SPACE);

    };

template <int DIM>
image_binary<DIM>* binary_copycast (image_base* input); //! Converts might-be binary types into image_binary

template<int IMAGEDIM = 3>
class fit_ellipsoid3D_cost_function:public vnl_cost_function
{
private:
		image_binary<IMAGEDIM> *the_image;
		ellipsoid3D *the_ellipsoid;
		int xsize, ysize, zsize;
		vector<double> _min_constraints;
		vector<double> _max_constraints;
		double _non_object_cost;
		double _object_cost;
	public:
		fit_ellipsoid3D_cost_function(image_binary<IMAGEDIM> *im, ellipsoid3D *e, vector<double> min_constraints, vector<double> max_constraints, double non_object_cost=3, double object_cost=-1); 
		double f(vnl_vector<double> const &x);
};

#endif