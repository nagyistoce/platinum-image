//////////////////////////////////////////////////////////////////////////
//
//   Image_scalar $Revision$
///
///  Image type with scalar voxel/pixel values
///  Mostly the same as image_general
///
//   $LastChangedBy$
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

#ifndef __image_scalar__
#define __image_scalar__

#include <stack>
#include <queue>
#include "image_general.h"
//#include "image_general.hxx"
#include "bias_field.h"
#include "filters.h"
#include "voxel_tools.h"
#include "Utilities/vxl/contrib/mil3d_trilin_interp_3d.h"
//#include "Utilities/tricubic1.0.0/libtricubic/tricubic.h" (//http://www.lekien.com/~francois/software/tricubic/)
//#include "fcm.h"

//--------------------------------
#include "itkCommand.h"
#include "itkImage.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "vtkImageData.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkImageActor.h"		//JK remove
#include "vtkRenderer.h"	
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h" 

#include "vtkStructuredPointsReader.h"
#include "vtkImageCast.h" //SO
//--------------------------------
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_least_squares_function.h>

#define TRUE 1
#define FALSE 0



template<class ELEMTYPE, int IMAGEDIM = 3>
class image_scalar : public image_general <ELEMTYPE, IMAGEDIM>
{
	friend class image_storage<ELEMTYPE>;

private:
	//void filter_3d_border_voxel(filter_base* filter, image_scalar<float,3>* copy, int x, int y, int z, int borderflag, image_binary<IMAGEDIM>* mask, int maskflag);
	void filter_3d_border_voxel(filter_base* filter, image_scalar<ELEMTYPE,IMAGEDIM>* copy, int x, int y, int z, int borderflag, image_binary<IMAGEDIM>* mask, int maskflag);
//	virtual histogram_1D<ELEMTYPE>* get_stats(){return stats;} //called by image_storage
	virtual void set_max(float m){stats->max(m);}
	virtual void set_min(float m){stats->min(m);}


protected:
	void set_scalar_parameters();
	histogram_1D<ELEMTYPE> *stats;
	void set_stats_histogram(histogram_1D<ELEMTYPE > * h);

	transfer_scalar_base<ELEMTYPE> *tfunction;
	void transfer_function(transfer_scalar_base<ELEMTYPE> * t=NULL);
    virtual void transfer_function(std::string functionName); //! replace transfer function using string identifier

	void set_parameters(itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i);   //set parameters from ITK metadata


public:
	virtual ~image_scalar();

    image_scalar(): image_general<ELEMTYPE, IMAGEDIM>(){set_scalar_parameters();};
    image_scalar(int w, int h, int d, ELEMTYPE *ptr = NULL):image_general<ELEMTYPE, IMAGEDIM>(w, h, d, ptr) {set_scalar_parameters();};
    image_scalar(itk::SmartPointer< itk::OrientedImage<ELEMTYPE, IMAGEDIM > > &i):image_general<ELEMTYPE, IMAGEDIM>(i) {set_scalar_parameters();}
    //copy constructor
    template<class SOURCETYPE>
        image_scalar(image_general<SOURCETYPE, IMAGEDIM> * old_image, bool copyData = true): image_general<ELEMTYPE, IMAGEDIM>(old_image, copyData)
            {set_scalar_parameters();};

	image_scalar(ELEMTYPE * inData, unsigned long inDataNumElems, long width, long height, Vector3D voxelSize) : image_general<ELEMTYPE, IMAGEDIM>(inData,inDataNumElems, width, height, voxelSize) {set_scalar_parameters();}
    //raw constructor
    image_scalar(std::vector<std::string> files, long width, long height, bool bigEndian, long headerSize, Vector3D voxelSize, unsigned int startFile = 1,unsigned int increment = 1) : image_general<ELEMTYPE, IMAGEDIM> (files, width, height, bigEndian, headerSize, voxelSize, startFile,increment){set_scalar_parameters();};
    image_scalar(const string filepath, const string name=""):image_general<ELEMTYPE, IMAGEDIM>(filepath, name){set_scalar_parameters();}

	template <class sourceType>
		void set_parameters(image_general<sourceType, IMAGEDIM> *from_image);         //clone parameters from another image

    //ELEMTYPE get_number_voxel(itk::Vector<int,IMAGEDIM>);
    ELEMTYPE get_num_values();
	ELEMTYPE get_max() const;
	ELEMTYPE get_min() const;
	float get_mean_intensity();
	histogram_1D<ELEMTYPE>* get_histogram();
	histogram_1D<ELEMTYPE>* get_histogram_new_with_same_num_buckets_as_intensities();
	virtual void stats_refresh(bool min_max_refresh=false);
	void min_max_refresh();     //! lighter function that _only_ recalculates max/min values, for use inside processing functions

    float get_number_voxel(int x, int y, int z) const;  //the use of virtual makes for example "complex<>" class work...
	float get_max_float() const;
	float get_min_float() const;
	float get_max_float_safe() const;
	float get_min_float_safe() const;
	virtual float get_display_min_float() const;
	virtual float get_display_max_float() const;
    virtual void get_display_voxel(RGBvalue &val,int x, int y, int z=0) const;

	string resolve_tooltip();		//combines tooltip data of this class with data from other classes
	string resolve_tooltip_image_scalar(); //resolves tooltip data typical for this class

	//	void getVTKImagePointer();
	vtkImageData* getVTKImagePointer();
	void writeVTKImagePointer(vtkImageData *im);
	vtkAlgorithmOutput* getvtkStructuredPoints();



	//------------------------- Interpolations -------------------------
	//Implementation "inspired" by "ResampleImageFilter7" example in the ITK documentation
	void interpolate_spline_ITK_3D(image_scalar<ELEMTYPE, IMAGEDIM > *ref_im, int spline_order=3); 


//	void interpolate_bilinear_2D(float phys_x, float phys_y, int vox_z);
//	void interpolate_trilinear_3D_ITK(float phys_x, float phys_y, float phys_z); //no boundary checks in "itkLinearInterpolateImageFunction.h" 
	void interpolate_trilinear_3D_vxl(image_scalar<ELEMTYPE, IMAGEDIM > *src_im); //Implementation using the vxl package, alpha-tested

	void resample_with_spline_interpolation_3D(int newxsize, int newysize, int newzsize, int spline_order=3);

	// Tricubic interpolation using method described in:
	// F. Lekien, J.E. Marsden
	// Tricubic Interpolation in Three Dimensions
	// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005
	void interpolate_tricubic_3D(image_scalar<ELEMTYPE, IMAGEDIM > *src_im); //Implementation from "libtricubic" package

	//Speed the "interpolate_tricubic_3D" function up by precalculationg the numericla diffs for the whole image.
	image_scalar<double,3>* get_num_diff_image_1storder_central_diff_3D(int direction);	//voxel based (i.e. no real/physical dimensions included),  alpha-tested
	image_scalar<double,3>* get_num_diff_image_2ndorder_central_diff_3D(image_scalar<double,3>*df_dir1, int direction2);	//voxel based (i.e. no real/physical dimensions included) , alpha-tested

	void set_a_coeff2(double a[64], double f[8], double dfdx[8], double dfdy[8], double dfdz[8], double d2fdxdy[8], double d2fdxdz[8], double d2fdydz[8], double d3fdxdydz[8]);
//	void set_a_coeff(double a[64], double f[8], double dfdx[8], double dfdy[8], double dfdz[8], double d2fdxdy[8], double d2fdxdz[8], double d2fdydz[8], double d3fdxdydz[8]);
//	void set_a_coeff_stacked(double a[64], double x[64]);
	double tricubic_eval(double a[64], double x, double y, double z);



	//------------------------- Various Operations -------------------------
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_slice_3D(int slice, int dir=2);
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_slice_rotated_3D(int slice, int dir=2);
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size);	
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_region_3D(int x1, int y1, int z1, int x2, int y2, int z2);	
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_region_3D(image_binary<3> *mask, IMGBINARYTYPE object_value=TRUE);
	image_scalar<ELEMTYPE, IMAGEDIM>* get_subvolume_from_thresholded_region_3D(ELEMTYPE from_val, ELEMTYPE to_val=std::numeric_limits<ELEMTYPE>::max());

	//--------------------
	image_scalar<ELEMTYPE, IMAGEDIM>* expand_borders(unsigned int dx, unsigned int dy, unsigned int dz, IMGBINARYTYPE value=0);	
	image_scalar<ELEMTYPE, IMAGEDIM>* expand_borders2D_by_dir(int dir, unsigned int dr=1, IMGBINARYTYPE value=0);	
	image_scalar<ELEMTYPE, IMAGEDIM>* contract_borders(unsigned int dx, unsigned int dy, unsigned int dz);
	image_scalar<ELEMTYPE, IMAGEDIM>* contract_borders2D_by_dir(int dir, unsigned int dr=1);

	//--------------------
	void crop_3D(image_binary<3> *mask);
	image_scalar<ELEMTYPE, IMAGEDIM>* crop_and_return_3D(image_binary<3> *mask);
	//... get_sub_region(...)

	ELEMTYPE get_max_in_slice3D(int slice, int dir=2);

    image_binary<IMAGEDIM> *threshold(ELEMTYPE low, ELEMTYPE high=std::numeric_limits<ELEMTYPE>::max(), IMGBINARYTYPE true_inside_threshold=true); ///Return a image_binary where all voxels with values between low and high gets the value true_inside_threshold.
    image_binary<IMAGEDIM> *threshold_slice_wise_with_slice_max_offsets(ELEMTYPE thres_min, ELEMTYPE max_offset=-1, int dir=2, IMGBINARYTYPE true_inside_threshold=true, ELEMTYPE high=std::numeric_limits<ELEMTYPE>::max()); ///Return a image_binary where slice-voxels with values between "max(low ,val+offset)" and high gets the value true_inside_threshold.
	void draw_line_2D(int x0, int y0, int x1, int y1, int z, ELEMTYPE value, int direction=2); ///Draw a line between (x0,y0) and (x1,y1) in plane z using color described by value. The coordinates are given on the plane orthogonal to the axis given by direction.
	void draw_line_3D(Vector3Dint from_vox, Vector3Dint to_vox, ELEMTYPE value); 
	void draw_line_3D(line3D line, ELEMTYPE value); 
	bool row_sum_threshold(int* res, ELEMTYPE low_thr, ELEMTYPE high_thr, int row_direction=0, int z_direction=2, int first_slice=-1, int last_slice=-1); ///Compute optimal split level for each slice
    void mask_out(image_binary<IMAGEDIM> *mask, IMGBINARYTYPE object_value=TRUE, ELEMTYPE blank=0); ///All voxels in the current image where the corresponding mask voxels != object_value are set to blank.
    void mask_out(int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, ELEMTYPE blank=0); ///All voxels within the given sub-volume are set to blank.
    void mask_out_from_planes_3D(vector<plane3D> planes, ELEMTYPE blank=0, bool outside_all_planes_needed=false, SPACE_TYPE st=VOXEL_SPACE);
	void mask_out_from_planes_3D(plane3D plane1, plane3D plane2=plane3D(), plane3D plane3=plane3D(), plane3D plane4=plane3D(), plane3D plane5=plane3D(), ELEMTYPE blank=0, SPACE_TYPE st=VOXEL_SPACE);
	std::vector<double> get_slice_sum(int direction=2);

	void flip_voxel_data_3D(int direction);

	ELEMTYPE get_intensity_at_lower_percentile(float percentile, bool ignore_zero_intensity);
    void save_histogram_to_txt_file(const std::string filename, const std::string separator=";");
    void save_histogram_to_txt_file(const std::string filename, gaussian *g=NULL, bool reload_hist_from_image=false, const std::string separator=";");

    image_scalar<unsigned short, 3>* create2Dhistogram_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, bool remove_zero_intensity=false, int num_buckets_a=-1, int num_buckets_b=-1, image_binary<IMAGEDIM>* mask=NULL); 

	//the resulting histogram volume will have the intensities of first/second in the x/y directions.
	//The z direction will gives the different 2D-histograms in the specified direction "hist_slc_dir"
	image_scalar<ELEMTYPE, IMAGEDIM>* create_slicewise_2Dhistograms_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, int hist_slc_dir=2, bool remove_zero_intensity=false, int scale_a=-1, int scale_b=-1); 
    image_scalar<unsigned short, 3>* create2Dhistogram_from_slices(int dir, int slice1, int slice2, bool remove_zero_intensity=false, int num_buckets_a=-1, int num_buckets_b=-1); 

//	image_scalar<ELEMTYPE, IMAGEDIM>* create_slicewise_2Dhistograms_3D(image_scalar<ELEMTYPE, IMAGEDIM> *second_image, int hist_slc_dir=2, bool remove_zero_intensity=false, int scale_a=-1, int scale_b=-1); 


	void translate_slices_to_align_coordinates_3D(vector<Vector3D> coords, int dir, ELEMTYPE empty_value=0, bool unalign=false);

	void smooth_ITK(Vector3D radius); 
	void smooth_3D(Vector3D r); 
	void smooth_using_filter3D(int std_x, int std_y, int std_z);

	image_binary<IMAGEDIM>* region_grow_corners_3D(ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D(queue<Vector3D> seed_queue, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<IMAGEDIM>* region_grow_3D_if_equal_or_lower_intensity(queue<Vector3D> seed_queue, ELEMTYPE min_intensity=1);
	image_binary<IMAGEDIM>* region_grow_3D_if_lower_intensity(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity=1);
	image_binary<IMAGEDIM>* region_grow_3D_if_lower_intensity(queue<Vector3D> seed_queue, ELEMTYPE min_intensity=1);
	image_binary<3>* region_grow_3D_using_object_labeling(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<3>* region_grow_3D_using_object_labeling(image_binary<IMAGEDIM> *seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());
	image_binary<3>* region_grow_3D_using_object_labeling(queue<Vector3D> seed_queue, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max());

	//image_binary<IMAGEDIM>* region_grow_robust_3D(Vector3D seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=26, int radius=1);
	void region_grow_robust_in_slice_3D(image_binary<3>* result, Vector2Dint seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=8, int radius=1, int slice=0, int dir=2);
	void region_grow_robust_in_slice_3D(image_binary<3>* result, image_binary<3>* seed_image, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=8, int radius=1, int slice=0, int dir=2);
	void region_grow_robust_in_slice_3D(image_binary<3>* result, stack<Vector2Dint> seeds, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=8, int radius=1, int slice=0, int dir=2);
	image_binary<IMAGEDIM>* region_grow_robust_3D(stack<Vector3Dint> seeds, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=26, int radius=1);
	image_binary<IMAGEDIM>* region_grow_robust_3D(Vector3Dint seed, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=26, int radius=1);
	image_binary<IMAGEDIM>* region_grow_robust_3D(image_binary<3>* seed_im, ELEMTYPE min_intensity, ELEMTYPE max_intensity=std::numeric_limits<ELEMTYPE>::max(), int nr_accepted_neighbours=26, int radius=1);

	//JK2
	void region_grow_declining_in_slice_3D(image_binary<3>* result, image_binary<3>* seed_image, ELEMTYPE min_intensity, int slice, int dir=2);
	void region_grow_declining_in_slice_3D(image_binary<3>* result, stack<Vector2Dint> seeds, ELEMTYPE min_intensity, int slice, int dir=2);

	float get_moment(int k, int l, int m, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(),  SPACE_TYPE type = VOXEL_SPACE);
	float get_moment_central(int k, int l, int m, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(),  SPACE_TYPE type = VOXEL_SPACE);
	Vector3D get_center_of_gravity(SPACE_TYPE type = VOXEL_SPACE);
	Vector3D get_center_of_gravity_from_range(ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(),  SPACE_TYPE type = VOXEL_SPACE);
	Vector3D get_center_of_gravity_binary_from_range(ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(),  SPACE_TYPE type = VOXEL_SPACE);
	Vector3D get_in_slice_center_of_gravity_in_dir(int dir, int slice, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(),  SPACE_TYPE type = VOXEL_SPACE);
	vector<Vector3D> get_in_slice_center_of_gravities_in_dir(int dir, ELEMTYPE lower_int_limit, ELEMTYPE upper_int_limit=std::numeric_limits<ELEMTYPE>::max(), SPACE_TYPE type = VOXEL_SPACE);
	vector<Vector3D> get_positions_of_voxels_with_value_between(ELEMTYPE from_val=1, ELEMTYPE to_val=1, SPACE_TYPE st = VOXEL_SPACE);

	Vector3D get_pos_of_highest_value_in_region(int x1, int y1, int z1, int x2, int y2, int z2, ELEMTYPE upper_limit=std::numeric_limits<ELEMTYPE>::max());
	Vector3D get_pos_of_highest_value_in_region(Vector3D from_pos, Vector3D to_pos, ELEMTYPE upper_limit=std::numeric_limits<ELEMTYPE>::max());
	Vector3D get_pos_of_highest_value(ELEMTYPE upper_limit=std::numeric_limits<ELEMTYPE>::max());
	Vector3D get_pos_of_lowest_value_in_region(int x1, int y1, int z1, int x2, int y2, int z2, ELEMTYPE lower_limit=std::numeric_limits<ELEMTYPE>::min());
	Vector3D get_pos_of_lowest_value_in_region(Vector3D from_pos, Vector3D to_pos, ELEMTYPE lower_limit=std::numeric_limits<ELEMTYPE>::min());
	Vector3D get_pos_of_lowest_value(ELEMTYPE lower_limit=std::numeric_limits<ELEMTYPE>::min());

	vector<Vector3Dint> get_voxel_positions_from_values_greater_than_3D(ELEMTYPE val=0);
	stack<Vector3Dint> get_voxel_positions_from_values_greater_than_3D_as_stack(ELEMTYPE val=0);
	vector<Vector3Dint> get_voxel_positions_in_slice_from_values_greater_than_3D(int slice, int dir=2, ELEMTYPE val=0);

	image_scalar<ELEMTYPE, IMAGEDIM>* correct_inclined_object_slicewise_after_cg_line(int dir, line3D cg_line, SPACE_TYPE type = VOXEL_SPACE);

	void fit_gaussian_2D_to_this_image_2D(gaussian_2d* g);

	//------------------- Filter functions ----------------------
	image_scalar<ELEMTYPE, IMAGEDIM>* get_gradmagn_filter3D_image_3D(Vector3D from, Vector3D to, GRAD_MAG_TYPE type = XY);

//	void medianFilter2D();
//	void meanFilter();
//	void discreteGaussFilter(double gaussianVariance,int maxKernelWidth);
//	void gradientFilter();
//	void gradientFilter2D();
//	itk::MeanImageFilter<theImageType,theImageType>::Pointer filter = itk::MeanImageFilter<theImageType,theImageType>::New();

	//TODO: to implement different PROJ_MODES and to return a 2D Image...
	image_scalar<ELEMTYPE, 3>* create_projection_3D(int dir, PROJECTION_MODE PROJ=PROJ_MAX); //enum PROJECTION_MODE {PROJ_MEAN, PROJ_MAX}; 
	

	//Calculates TruePositive, FalsePositive values and "Udupa-index" for voxels larger than zero...
	void calculate_TP_FP_Udupa_3D(float &tp, float &fp, float &udupa, image_scalar<ELEMTYPE, IMAGEDIM>* ground_truth, ELEMTYPE gt_obj_val=1, ELEMTYPE this_obj_val=1 ); 
	void calculate_FP_FN_images(image_scalar<ELEMTYPE, IMAGEDIM>* ground_truth, image_binary<IMAGEDIM>* fp_im, image_binary<IMAGEDIM>* fn_im, ELEMTYPE gt_obj_val=1, ELEMTYPE this_obj_val=1 ); 

	// return the voxel position of POINT_TYPE (i.e. max gradient magnitude, max value, ...) using voxel position and voxel radius
	Vector3D get_pos_of_type_in_region_voxel ( Vector3D center, Vector3D radius, POINT_TYPE point_type );
	
	Vector3D get_pos_of_max_grad_mag_in_region_voxel ( Vector3D center, Vector3D radius, GRAD_MAG_TYPE type );

	void scale_slice_by_factor_3d(int dir, float factor, int slice);
	float scale_slice_average_to_3d(int dir, ELEMTYPE new_average, int slice); //returns slice_scale_factor

	float get_mean_from_slice_3d(int dir, int slice, image_binary<IMAGEDIM>* mask=NULL);
//	ELEMTYPE get_mean_from_5nbh(int x, int y, int z, int dir=2);

	void logarithm_3d(int zero_handling=0);
	void abs_3d();

	double calculate_entropy_2d();

	void filter_3D(filter_base* filter, int borderflag=0, image_binary<IMAGEDIM>* mask=NULL, int maskflag=0);

// -------------- bias field operations ---------------
	
	void fill_image_with_bias_field_data3D(bias_poly<3> b);
	void fill_image_with_gaussian_values_centered_2D(int dir, gaussian g);
	void fill_image_with_gaussian_values_centered_2D(int dir, float ampl, float sigma_in_voxels);
	void fill_image_with_gaussian_values_2D(int dir, float ampl, float sigma_in_voxels, int center_u, int center_v);


// -------------- Scalar Load/Save functions ---------------
	void load_dataset_from_VTK_file(std::string file_path);
    void save_to_TIF_file_series_3D(const std::string file_path_base, int dir=2, int from_slice=-1, int to_slice=-1);
	void save_uchar2D_to_TIF_file(const std::string file_path_base, const std::string slice="0");



	// --------- image_scalarprocess.hxx ------- (file for application specific implementations) -----

	//------------------- Functions using voxel_set class ----------------------
	voxel_set<ELEMTYPE> get_voxel_set_from_image_data_3D();
	voxel_set<ELEMTYPE> get_voxel_set_from_image_data_3D(ELEMTYPE exclude_value);
	ELEMTYPE get_median_voxel_value_3D(ELEMTYPE exclude_value);

	voxel_set<ELEMTYPE> set_val_to_voxel_that_has_no_neighbour_with_val_in_vox_radius(int radius, Vector3Dint pos, ELEMTYPE from_val, ELEMTYPE to_val, ELEMTYPE nb_val);
	voxel_set<ELEMTYPE> set_val_to_voxel_that_has_some_neighbour_with_val_in_vox_radius(int radius, Vector3Dint pos, ELEMTYPE from_val, ELEMTYPE to_val, ELEMTYPE nb_val);


	// See description in: Kullberg2006 - J Magn Reson Imaging. 2006 Aug;24(2):394-401.
	// Whole-body T1 mapping improves the definition of adipose tissue: consequences for automated image analysis.
	// Usage: large_flip->calc...(small_flip) 
	// Meta data required: TR, Flip_angle
	// Note that function can be moved to image_storage...
    image_scalar<ELEMTYPE, IMAGEDIM>* calculate_T1Map_from_two_flip_angle_MRvolumes_3D(image_scalar<ELEMTYPE, IMAGEDIM > *small_flip, float body_thres=0, float t1_min=0, float t1_max=2000); 
    image_scalar<ELEMTYPE, IMAGEDIM>* calculate_T1Map_3D(vector<image_scalar<ELEMTYPE, IMAGEDIM > *> v, float body_thres=0, float t1_min=0, float t1_max=2000); 

	// return the gradient magnitude using voxel position
	float grad_mag_voxel ( Vector3D point, GRAD_MAG_TYPE type );
	float grad_mag_voxel ( int x, int y, int z, GRAD_MAG_TYPE type );
	
	float weight_of_type( Vector3D center, Vector3D current, WEIGHT_TYPE type );

	// slice reorganization function that sorts slices from many dynamic scans
	// first used for slice sorting from DICOM export from "COMBI-acquisition" on Philips 1.5T MRI. 
	vector< image_scalar<ELEMTYPE, IMAGEDIM>* > slice_reorganization_multicontrast(int no_dynamics, int no_contrasts);


	//-----------------------------------------------------------------
	//Functions for segmentation of bodies and lungs form whole-body MRI scans
	//Assumes the feet direction is in increasing voxel-y direction... (nose is in neg Z-direction) 

	//Function for localizing main axis of objects (via 2D center of gravities)... pixels are not weighted by their intensities...
	float appl_wb_correct_inclination(image_scalar<ELEMTYPE, IMAGEDIM>*fat, image_scalar<ELEMTYPE, IMAGEDIM>*water);
	image_binary<3>* appl_wb_segment_body_from_sum_image(int initial_thres=30);
	image_binary<3>* appl_wb_segment_rough_lung_from_sum_image(image_binary<3> *mask, float lung_volume_in_litres=2.5);
	image_binary<3>* appl_wb_segment_one_lung_from_sum_image(image_binary<3> *thorax_body_mask, float lung_volume_in_litres=2.5, int high_lung_fitting_thres=50, string base="");
	image_binary<3>* appl_wb_segment_both_lungs_from_sum_image(image_binary<3> *body_mask, float lung_volume_in_litres=2.5, int high_lung_fitting_thres=50, string base="");
//	image_binary<3>* appl_wb_segment_lungs_from_sum_image(image_binary<3> *body_mask, float lung_volume_in_litres=5);
	void appl_wb_segment_find_crotch_pos_from_wp_smooth_image(int &pos_x, int &pos_y, int mip_thres=950, string base="", int y_start=180);
//	void appl_wb_segment_find_crotch_pos_from_water_percent_image(int &pos_x, int &pos_y, int mip_thres=950, string base="", int y_start=180);
	int appl_find_femur_y_level_from_wp_image(image_scalar<unsigned short, 3>* model_l, Vector3Dint guess_center_l, Vector3Dint d_xyz, image_scalar<unsigned short,3> *cost_image);
	int appl_find_femur_y_level_from_body_masked_fp_image(int from_y, int to_y, Vector3Dint &femur_l, Vector3Dint &femur_r, float p2a_c=12.8,float p2a_sd=1.5, float area_c=230, float area_sd=50, int res_thresh=2500, string base="");

	image_binary<3>* appl_wb_segment_VAT_mask_from_this_water_percent_abd_subvolume(image_binary<3> *bin_body, string base="", int dir=1);
	void appl_wb_normalize_features_slicewise_by_global_mean_on_this_float (image_scalar<float, 3>* second_feature, image_scalar<float, 3>* sum=NULL, image_binary<3>* body_lung_mask=NULL);
	void appl_wb_SIM_bias_correction_on_this_float(image_scalar<float, 3>* second_feature, int num_iterations=1, float iteration_strength=0.02, float map_x_smoothing_std_dev=60, float map_y_smoothing_std_dev=15, float map_z_smoothing_std_dev=60, float feat1_smoothing_std_dev=30, float feat2_smoothing_std_dev=30, image_binary<3>* body_lung_mask=NULL, int num_buckets_feat1=200, int num_buckets_feat2=200, bool save_corrected_images_each_iteration=false, bool save_histogram_each_iteration=false, bool save_field_each_iteration=false);

	void appl_1D_SIM_bias_correction(image_binary<3>* mask, int num_iterations=1, float iteration_strength=0.02, float map_x_smoothing_std_dev=60, float map_y_smoothing_std_dev=60, float map_z_smoothing_std_dev=6, float feat1_smoothing_std_dev=30, int num_buckets_feat1=200, bool save_corrected_images_each_iteration=false, bool save_histogram_each_iteration=false, bool save_field_each_iteration=false);

	//JK move to private later...
	float get_mean_squared_difference_to_template_3D(Vector3D pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template);
	float get_mean_squared_difference_to_template_centered_3D(Vector3D pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template);
	image_scalar<float, IMAGEDIM>* get_mean_squared_difference_image_3D(Vector3D from_center_pos, Vector3D to_center_pos, image_scalar<ELEMTYPE, IMAGEDIM> *small_template); //least square fit small template to region (center voxel template coordinates...)

	//ITK vesselness functions
	void vesselness_test(double hessian_sigma=5, double vessel_alpha1=0.5, double vessel_alpha2=0.5);

	void appl_scale_outer_slices_using_mean(int dir, int no_outer_slices=1);
	void appl_scale_outer_slices_using_mean(int dir, int no_outer_slices, image_scalar<ELEMTYPE, IMAGEDIM> *im1, image_scalar<ELEMTYPE, IMAGEDIM> *im2);

	image_binary<3>* appl_abd_create_crude_grad_mask( image_binary<3>* small_abd_mask, int std_x, int std_y, int std_z, int erode_dist, string save_base="" );

};

template<class ELEMTYPE, int IMAGEDIM = 3>
class fit_gaussian_2d_cost_function:public vnl_least_squares_function
{
private:
		image_scalar<ELEMTYPE, IMAGEDIM> *the_image;
		gaussian_2d *the_gaussian;
		int xsize, ysize;
	public:
		fit_gaussian_2d_cost_function(image_scalar<ELEMTYPE, IMAGEDIM> *im, gaussian_2d *g);
		void f(vnl_vector<double> const &x, vnl_vector<double> &fx);
};

#endif