//////////////////////////////////////////////////////////////////////////
//
//  Classes doing the processing of histograms in 1-3D $Revision$
//  (separated from user interface)
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

#ifndef __histogram__
#define __histogram__

#include "ptmath.h"
#include "pt_vector.h"
#include "curve.hxx"
#include "threshold.h"
#include "global.h"
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_amoeba.h>

template <class ELEMTYPE>
    class image_storage;

//template <class IMAGEDIM>
//    class image_binary;
template <class E, int D>
    class image_complex;


struct regionofinterest
    {
    Vector3D start, size;

    ROImode type;   //determines how these (voxel) coordinates should be interpreted
    };

class histogram_base
    {
    protected:
       // unsigned long *buckets;         //stores histogram "frequency"
		pts_vector<unsigned long> *bucket_vector;
        unsigned long num_distinct_values;
        
        unsigned short num_buckets;     //buckets per dimension, ie. actual #buckets = num_buckets^2 for 2D histogram
        unsigned long bucket_max;		//max value... (frequency)
        unsigned long bucket_mean;		//mean value...
        unsigned long num_elements_in_hist;	//stores total number of elements in histogram, (e.g. histograms from masked regions)

        bool readytorender;					//check that images have the same size --> 2D_hist for example
        
        void clear_pixmap(unsigned char * image, unsigned int w,unsigned int h);
        virtual void render_(unsigned char * image, unsigned int width,unsigned int height) = 0;
        thresholdparvalue threshold;

		void reallocate_buckets_if_necessary(int new_num_buckets=0); 	//if new_num_buckets == 0 --> keep the current resolution...

        histogram_base();

	public:
		
        virtual ~histogram_base();
        void render(unsigned char * image, unsigned int width,unsigned int height)   //use calculated data to render the histogram image
            {
            clear_pixmap (image, width, height);
            render_(image, width, height);
            }
          
		//do variance, max, choose number of buckets and the like... omitting the num_buckets parameter uses current stored resolution 
		virtual void calculate_from_image_data(int number_of_buckets=0) {}      
		virtual void data_has_changed(){}; //Updates statistics from the *buckets data
		curve_scalar<unsigned long>* get_curve_for_rendering();

		virtual void render_threshold (unsigned char * image, unsigned int w,unsigned int h) {}; 

		virtual thresholdparvalue get_threshold ()
			{return threshold;}
		virtual bool ready ();                                  //whether histogram has two valid (and compatible)
																	//images, and can render
	       
		int image_ID (int axis);      //return current image ID, axis 0=h, 1= v and so forth

		unsigned long num_values ()
			{return num_distinct_values;}

		pts_vector<unsigned long>* get_bucket_vector();

    };


template <class ELEMTYPE>
class histogram_typed : public histogram_base //!features common to histograms of different type pertaining to data type
    {
    protected:
        image_storage<ELEMTYPE> * images [THRESHOLDMAXCHANNELS];
        ELEMTYPE *i_start, *i_end;		//needs modification when > 1D histograms are handled
        ELEMTYPE max_value, min_value;
    public:
        histogram_typed();

        ELEMTYPE min()							//returns histogram (and also image) "intensity min"
            {return min_value;}
//        float min_float()
  //          {return min_value;}
	//	template <complex<ELEMTYPE> >
      //  float min_float()
        //    {return abs(min_value);}
        void min (ELEMTYPE new_min)			
            {min_value=new_min;}
        ELEMTYPE max()							//returns histogram (and also image) "intensity max"
            {return max_value;}
        void max (ELEMTYPE new_max)			
            {max_value=new_max;}

		void fill(unsigned long val);
		void calc_bucket_max(bool ignore_zero_and_one=true);
		void calc_bucket_mean();
		void calc_num_distinct_values();
		void calc_num_elements_in_hist();

		virtual void data_has_changed(); //Updates statistics from the *buckets data
    };

template <class ELEMTYPE>
class histogram_1D : public histogram_typed<ELEMTYPE> //horizontal 1D graph histogram
    {
    unsigned long render_max;

    protected:
        void render_(unsigned char * image, unsigned int w, unsigned int h)
            {}
    public:
        histogram_1D(image_storage<ELEMTYPE> * i, int num_buckets=-1);
        histogram_1D(ELEMTYPE * start, ELEMTYPE * end );
        histogram_1D(image_storage<ELEMTYPE> *image_data, image_storage<unsigned char> *image_bin_mask, int num_buckets);
        histogram_1D(string hist_text_file_path, std::string separator="\t");

        ~histogram_1D();
        
        void resize(unsigned long); //JK - Should maybe be protected...
		virtual void resize_to_fit_data(); //Make sure alla integer values have a bucket, JK - Should maybe be protected...

        void render(unsigned char * image, unsigned int width, unsigned int height);

	    //void image (int vol);

		void calculate_from_image_data(int number_of_buckets=0);
		void print_histogram_info();
//		void print_histogram_content();
		//thresholdparvalue get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);

//		void recalc_min_max_data(ELEMTYPE test);//{cout<<"hej1";};
//		void recalc_min_max_data(complex<float> test){cout<<"hej2_complex";}
		void recalc_min_max_data();
		void refill_bucket_data();

		template <class E, int D>
			void calculate_from_image_complex(image_complex<E,D> *im);


		virtual bool ready ()
			{return this->readytorender;}   
		image_storage<ELEMTYPE> * image ();

		void save_histogram_to_txt_file(std::string filepath, gaussian *g=NULL, bool reload_hist_from_image=false, std::string separator="\t");
		void save_histogram_to_txt_file(std::string filepath, vector<gaussian> v, bool reload_hist_from_image=false, std::string separator="\t");
		
		float get_scalefactor();
		ELEMTYPE bucketpos_to_intensity(int bucketpos);
		int intensity_to_bucketpos(ELEMTYPE intensity);
		
		void add_histogram_data(histogram_1D<ELEMTYPE> *hist2);
		void clear_zero_intentisty_bucket();


		//------------ scaling filtering -------------------------
		void set_sum_of_bucket_contents_to_value(double value=1); //often requires the ELEMTYPEs float or double.
		void logarithm(int zero_handling);
		void smooth_mean(int nr_of_neighbours, int nr_of_times);
		void smooth_mean(int nr_of_neighbours, int nr_of_times, int from_bucket, int to_bucket);
//		float get_histogram_gradient_in_bucket(int bucket);
		float get_norm_frequency_in_bucket(int bucket);
		float get_norm_frequency_for_intensity(ELEMTYPE intensity);
		float get_norm_p_log_p_frequency_in_bucket(int bucket, ZERO_HANDLING_TYPE zht=ZERO_HANDLING_SET_ZERO);
		float get_norm_p_log_p_frequency_for_intensity(ELEMTYPE intensity, ZERO_HANDLING_TYPE zht=ZERO_HANDLING_SET_ZERO);
		float get_norm_p_log_p_cost(ZERO_HANDLING_TYPE zht=ZERO_HANDLING_SET_ZERO);
		float get_norm_p_log_p_gradient(int bucket, ZERO_HANDLING_TYPE zht=ZERO_HANDLING_SET_ZERO);
		float get_norm_p_log_p_gradient_for_intensity(ELEMTYPE intensity, ZERO_HANDLING_TYPE zht=ZERO_HANDLING_SET_ZERO);


		ELEMTYPE get_bucket_at_histogram_lower_percentile(float percentile, bool ignore_zero_intensity);
		ELEMTYPE get_intensity_at_histogram_lower_percentile(float percentile, bool ignore_zero_intensity);
		ELEMTYPE get_bucket_at_histogram_higher_percentile(float percentile, bool ignore_zero_intensity);
		ELEMTYPE get_intensity_at_histogram_higher_percentile(float percentile, bool ignore_zero_intensity);
		ELEMTYPE get_intensity_at_included_num_pix_from_lower_int(ELEMTYPE lower_int, float num_pix);

		bool is_central_histogram_bimodal(int min_mode_sep=150, double valley_factor=0.7, ELEMTYPE peak_min=200, int speedup=1); //looks in the central 50% of intensities
		bool is_histogram_bimodal(int start, int end, int min_mode_sep=150, double valley_factor=0.7, ELEMTYPE peak_min=200, int speedup=1);

		//------ Fitting of gaussian functions ------
		void fit_gaussian_to_intensity_range(float &amp, float &center, float &sigma, ELEMTYPE from, ELEMTYPE to, bool print_info=false);

		float find_better_amplitude(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
		float find_better_center(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
		float find_better_sigma(gaussian g, int from_bucket, int to_bucket, float factor1=0.8, float factor2=1.2, int nr_steps=10);
		double get_sum_square_diff_between_buckets(vector<gaussian> v, int from_bucket, int to_bucket, bool ignore_zeros=true, bool weight_with_hist_freq=false);
		double get_sum_square_diff_between_buckets(gaussian g, int from_bucket, int to_bucket, bool ignore_zeros=true);
		double get_sum_square_diff(vector<gaussian> v, bool ignore_zeros=true, bool weight_with_hist_freq=false);
		double get_sum_square_diff(gaussian g, bool ignore_zeros=true);
		double get_gaussian_area(gaussian g, int from_bucket, int to_bucket);
		double get_gaussian_area(gaussian g);
		vector<double> get_gaussian_areas(vector<gaussian> v);
		double get_sum_square_gaussian_overlap(vector<gaussian> v, int from_bucket, int to_bucket);
		double get_sum_square_gaussian_overlap(vector<gaussian> v);
		vector<double> get_overlaps_in_percent(vector<gaussian> v);
		vnl_vector<double> get_vnl_vector_with_start_guess_of_num_gaussians(int num_gaussians);
		ELEMTYPE fit_two_gaussians_to_histogram_and_return_threshold(float punish_overlap=0, float punish_area_differences=0, float punish_variance_differences=0, bool weight_with_hist_freq=false, string save_histogram_file_path = "");
		vnl_vector<double> fit_n_gaussians_to_histogram(int n = 2,string save_histogram_file_path = "");

		//------ Fitting of rayleigh functions ------
		void fit_rayleigh_distr_to_intensity_range(float &amp, float &M, ELEMTYPE from_int, ELEMTYPE to_int, bool print_info=false);
		double get_sum_square_diff(rayleighian r, bool ignore_zeros=true);
		double get_sum_square_diff_from_buckets(rayleighian r, int from_bucket, int to_bucket, bool ignore_zeros=true);

		//------------ min max variance -------------------------
		ELEMTYPE get_min_value_in_bucket_range(int from, int to);
		ELEMTYPE get_min_value_in_bucket_range(int from, int to, int &min_val_bucket_pos);
		ELEMTYPE get_max_value_in_bucket_range(int from, int to);
		ELEMTYPE get_max_value_in_bucket_range(int from, int to, int &max_val_bucket_pos);
		float get_mean_intensity_in_bucket_range(int from, int to, bool exclude_zero_int_bucket=false);
		float get_mean_intensity(bool exclude_zero_int_bucket=false);
		float get_hist_mean();
		float get_hist_variance();
		float get_variance_in_bucket_range(int from, int to);
		float get_variance_in_intensity_range(ELEMTYPE from, ELEMTYPE to);
		int get_bucket_pos_with_largest_value_in_bucket_range(int from, int to);
		int get_bucket_pos_with_largest_value_in_intensity_range(ELEMTYPE from, ELEMTYPE to);
    };


/*template<class ELEMTYPE>
class fit_gaussians_to_curve_cost_function : public vnl_cost_function
{
	histogram_1D<ELEMTYPE> *the_hist;
	int num_gaussians;
	bool punish_overlap;
	bool punish_large_area_differences;

public:
	fit_gaussians_to_curve_cost_function(histogram_1D<ELEMTYPE> *h, int num, bool punish_overlap=false, bool punish_large_area_differences=false);
	double f(vnl_vector<double> const &x);
};

template<class ELEMTYPE>
class fit_rayleighian_to_histogram_1D_cost_function : public vnl_cost_function
{
	histogram_1D<ELEMTYPE> *the_hist;

public:
	fit_rayleighian_to_histogram_1D_cost_function(histogram_1D<ELEMTYPE> *h);
	double f(vnl_vector<double> const &x);
};*/


// This class extends histogram_1D<class ELEMTYPE> just to implement one function
// if this function should be specialized in the "_1D" class the whole class would need 
// to be specialized for float...
/*
template <class ELEMTYPE>
class histogram_1Dfloat : public histogram_1D<float>
{
	void resize_to_fit_data(); //Make sure alla integer values have a bucket, JK - Should maybe be protected...

};
*/

class histogram_2D_plot : public histogram_base  //TEST: histographic plot
    {
    bool imagesdifferinsize;
    unsigned short vol_size[VOLUMEMAXDIMENSION];

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h);
    public:
        void images (int image_hor,int image_vert);
        void calculate_from_image_data(int number_of_buckets=0);

        bool ready ();
    };

class histogram_2D : public histogram_base
    {
    unsigned long render_max;

    bool * highlight_data;      //highlighting of region of interest

    int vol_h_ID,vol_v_ID;      //IDs of the two images used

    protected:
        void render_(unsigned char * image, unsigned int w,unsigned int h);
    public:
        histogram_2D ();
        ~histogram_2D ();
        void images (int image_hor,int image_vert);
        void calculate_from_image_data(int number_of_buckets=0);
        thresholdparvalue get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
        void highlight (regionofinterest * region); //highlight histogram values for supplied
                                                    //region of interest
        bool ready ();
    };

/*
template<class ELEMTYPE, int IMAGEDIM>
class histogram_2Dimage : public histogram_base
    {
//   unsigned long render_max;
//    bool * highlight_data;      //highlighting of region of interest
//    int vol_h_ID,vol_v_ID;      //IDs of the two images used

	private:
		image_scalar<ELEMTYPE,IMAGEDIM>* im1;   //images are copied and stored in histogram2Dobject (original images can then be changed/deleted...)
		image_scalar<ELEMTYPE,IMAGEDIM>* im2;

		image_scalar<float,2>* hist;

    protected:

		void render_ (unsigned char * image, unsigned int width,unsigned int height);
    public:
        histogram_2Dimage(image_scalar<ELEMTYPE,IMAGEDIM>* im1, image_scalar<ELEMTYPE,IMAGEDIM>* im2);
        ~histogram_2Dimage();
//        void images (int image_hor,int image_vert);
//        void calculate_from_image_data(int number_of_buckets=0);
//        thresholdparvalue histogram_2D::get_threshold (float h_min,float h_max, float v_min, float v_max, int mode = THRESHOLD_2D_MODE_RECT);
//        void highlight (regionofinterest * region); //highlight histogram values for supplied
                                                    //region of interest
//        bool ready ();
//    };
*/
#endif