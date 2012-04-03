//$Id$

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


// *** histogram_typed ***
//#include "histogram.h"
#include "pt_vector.h"

template <class ELEMTYPE>
histogram_typed<ELEMTYPE>::histogram_typed()
{
    i_start  = NULL;
    i_end    = NULL;
    for (int i = 0; i < THRESHOLDMAXCHANNELS; i++)
        { images[i] = NULL; }
    
    max_value = std::numeric_limits<ELEMTYPE>::max();
    min_value = std::numeric_limits<ELEMTYPE>::min();
}

template <class ELEMTYPE>
void histogram_typed<ELEMTYPE>::fill(unsigned long val)
{
	for(unsigned short i=0; i<this->num_buckets; i++)
	{
		//this->buckets[i] = val;
		this->bucket_vector->at(i) = val;
	}
}

template <class ELEMTYPE>
void histogram_typed<ELEMTYPE>::calc_bucket_max(bool ignore_zero_and_one)
{
	this->bucket_max=0;

	if(ignore_zero_and_one){
		for(unsigned short i=0; i<this->num_buckets; i++)
		{
			if(this->bucket_vector->at(i) != 0 && this->bucket_vector->at(i) != 1){ //! feature: ignore 0 and true to get more sensible display scaling
				this->bucket_max = std::max(this->bucket_vector->at(i), this->bucket_max);
			}
		}
	}else{
		/*for(unsigned short i=0; i<this->num_buckets; i++)
		{
			this->bucket_max = std::max(this->bucket_vector->at(i), this->bucket_max);
			
		}*/
		int dummy;
		this->bucket_max = this->bucket_vector->get_maximum_in_range(0,this->bucket_vector->size()-1, dummy);
	}
}


template <class ELEMTYPE>
void histogram_typed<ELEMTYPE>::calc_bucket_mean()
{
	this->bucket_mean=this->bucket_vector->get_mean_in_range(0, this->num_buckets);
	/*this->bucket_mean = 0;
	for(unsigned short i=0; i<this->num_buckets; i++)
	{
		this->bucket_mean += this->bucket_vector->at(i)/(this->num_buckets);
	}*/
}

template <class ELEMTYPE>
void histogram_typed<ELEMTYPE>::calc_num_distinct_values()
{
	this->num_distinct_values=0;
	for(unsigned short i=0; i<this->num_buckets; i++)
	{
		if(this->bucket_vector->at(i) != 0)
		{
			this->num_distinct_values++;
		}
	}
}

template <class ELEMTYPE>
void histogram_typed<ELEMTYPE>::calc_num_elements_in_hist()
{
	this->num_elements_in_hist=0;
	for(unsigned short i=0; i<this->num_buckets; i++)
	{
		this->num_elements_in_hist += this->bucket_vector->at(i);
	}
}



template <class ELEMTYPE>
float histogram_typed<ELEMTYPE>::get_scalefactor(){
	return float(this->max()-this->min())*1.000001/float(this->num_buckets);
	//return float(this->max()-this->min())/float(this->num_buckets-1);
}



template <class ELEMTYPE>
void histogram_typed<ELEMTYPE>::data_has_changed()
{
	calc_bucket_max();
	calc_bucket_mean();
	calc_num_distinct_values();
	calc_num_elements_in_hist();
}




// *** histogram_1D ***

/*template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::image (int vol)
{
    this->threshold.id[0]=vol;
    
    calculate_from_image_data();
}*/

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::resize (unsigned long newNum)
{
//	cout<<"void histogram_1D<ELEMTYPE>::resize("<<newNum<<")"<<endl;

    this->num_buckets = newNum;

	if(this->bucket_vector != NULL){	//JK - Prevent memory loss....
//		cout<<"...delete buckets...."<<endl;
		delete this->bucket_vector;
		this->bucket_vector = NULL;
	}
   // this->buckets = new unsigned long [this->num_buckets];
	this->bucket_vector = new pts_vector<unsigned long>(this->num_buckets);

	//resize() is called from the constructor
    calculate_from_image_data(this->num_buckets);	 //the new number of buckets needs to be sent as argument

	this->bucket_vector->config_x_axis(this->get_scalefactor(), this->min());

}


template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::resize_to_fit_data()
{
	cout<<"histogram_1D<ELEMTYPE>::resize_to_fit_data()"<<endl;

}

/*
template <>
void histogram_1Dfloat<float>::resize_to_fit_data()
{
	cout<<"histogram_1Dfloat<float>::resize_to_fit_data()"<<endl;
}
*/

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (image_storage<ELEMTYPE> * i, int num_buckets):histogram_typed<ELEMTYPE>()
{
    this->images[0] = i;

	if(num_buckets>0){
		resize(num_buckets);
	}else{
		resize(1001);//Default # of buckets... i->max() cannot be called since it uses histogram_1D
	}
}

template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (ELEMTYPE * start,ELEMTYPE * end ):histogram_typed<ELEMTYPE>()
{
    this->i_start = start;
    this->i_end = end;
    
    //these histograms are typically used for stats
    resize (256);
}

//TODO - JK - make this work...
template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D (image_storage<ELEMTYPE> *image_data, image_storage<unsigned char> *image_bin_mask, int num_buckets):histogram_typed<ELEMTYPE>()
{
//	cout<<"histogram_1D...masked..."<<endl;
             
	this->reallocate_buckets_if_necessary(num_buckets);
/*	if(num_buckets >=0){
		this->num_buckets = num_buckets;
		if(this->buckets!=NULL){
			delete this->buckets;
		}
		this->buckets=new unsigned long [this->num_buckets];
	}else{
		pt_error::error("histogram_1D - constructor(masked) - num_buckets=strange...",pt_error::debug);
	}
*/
	//get pointer to source data
	if (this->i_start == NULL)
	{
		this->i_start = image_data->begin().pointer();
		this->i_end = image_data->end().pointer();
	}

	this->readytorender=(this->i_start != NULL);

	if (this->readytorender)
	{
		//reset buckets

		this->fill(0);

		ELEMTYPE *v; //voxel
		ELEMTYPE v_min = std::numeric_limits<ELEMTYPE>::max();
		ELEMTYPE v_max = std::numeric_limits<ELEMTYPE>::min();
		unsigned char *v_bin;

		for( v = this->i_start, v_bin = image_bin_mask->begin().pointer(); (v != this->i_end) && (v_bin != image_bin_mask->end().pointer()); ++v, ++v_bin){
			if(*v_bin>0){
				if(*v > v_max){
					v_max = *v;
				}
				if(*v < v_min){
					v_min = *v;
				}
			}
		}
		this->min(v_min);
		this->max(v_max);

		int bucketpos;

		for( v = this->i_start, v_bin = image_bin_mask->begin().pointer(); (v != this->i_end) && (v_bin != image_bin_mask->end().pointer()); ++v, ++v_bin){
			if(*v_bin>0){
				bucketpos = intensity_to_bucketpos(*v);
				
				//NOT VERY good to write outside allocated memory
				if(bucketpos>=0 && bucketpos<this->num_buckets){	
					//this->buckets[bucketpos]++;
					this->bucket_vector->at(bucketpos)++;
				}else{
					pt_error::error("histogram_1D<ELEMTYPE >::calculate - bucketpos out of range",pt_error::debug);
					cout<<" histogram_1D<ELEMTYPE >::calculate - bucketpos out of range... bucketpos="<<bucketpos<<endl;
				}
			}
		}

		this->data_has_changed();
	}
	
	else
	{
		//no calculation was done, set sensible values
		this->max(std::numeric_limits<ELEMTYPE>::max()); 
		this->min(std::numeric_limits<ELEMTYPE>::min());
	}
}
 
template <class ELEMTYPE>
histogram_1D<ELEMTYPE>::histogram_1D(string hist_text_file_path, std::string separator):histogram_typed<ELEMTYPE>()
{
	cout<<"histogram_1D("<<hist_text_file_path<<")"<<endl;
    ifstream myfile;

	if( file_exists(hist_text_file_path) ){
		myfile.open(hist_text_file_path.c_str());
		char buffer[10000];
		//----------------------------------
		myfile.getline(buffer,10000);	this->num_buckets = atof( get_csv_item(string(buffer),1,separator).c_str() );
		myfile.getline(buffer,10000);	this->min( atof( get_csv_item(string(buffer),1,separator).c_str() ) );
		myfile.getline(buffer,10000);	this->max( atof( get_csv_item(string(buffer),1,separator).c_str() ) );
//		myfile.getline(buffer,10000);	this->num_distinct_values = atof( get_csv_item(string(buffer),1,separator).c_str() );
//		myfile.getline(buffer,10000);	this->num_elements_in_hist =atof( get_csv_item(string(buffer),1,separator).c_str() );
//		myfile.getline(buffer,10000);	this->bucket_max = atof( get_csv_item(string(buffer),1,separator).c_str() );
//		myfile.getline(buffer,10000);	this->bucket_mean = atof( get_csv_item(string(buffer),1,separator).c_str() );
//		myfile.getline(buffer,10000);	this->readytorender = atof( get_csv_item(string(buffer),1,separator).c_str() );
		this->readytorender = true;

		//delete []this->buckets;
		//this->buckets = NULL;
		this->bucket_vector->clear();
		this->bucket_vector = NULL;
		this->reallocate_buckets_if_necessary(this->num_buckets);
/*		if(this->num_buckets >=0){
			if(this->buckets!=NULL){
				delete this->buckets;
			}
			this->buckets=new unsigned long [this->num_buckets];
		}else{
			pt_error::error("histogram_1D - constructor - num_buckets=strange...",pt_error::debug);
		}
*/
		//Jump to the histogram data part.....
		
		string header = "bucket" + separator + "intensity" + separator + "bucketvalue";
		while( !myfile.eof() && (string(buffer)!=header) ){
			myfile.getline(buffer,10000);
//			cout<<"("<<string(buffer)<<")"<<endl;
		}

		//Load the histogram data...

		int i=0;
		while( !myfile.eof() ){
			myfile.getline(buffer,10000);
			this->bucket_vector->at(i) = atof( get_csv_item(string(buffer),2,separator).c_str() );
//			cout<<this->buckets[i]<<" "<<endl;
			i++;
		}

		myfile.close();
	}else
	{
		this->readytorender = true;
		pt_error::error("histogram_1D - constructor - file does not exist...",pt_error::debug);
	}
	this->bucket_vector->config_x_axis(this->get_scalefactor(), this->min());
}




template <class ELEMTYPE>
histogram_1D<ELEMTYPE >::~histogram_1D ()
{}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::calculate_from_image_data(int new_num_buckets)
{
	//cout<<"histogram_1D<ELEMTYPE >::calculate_from_image_data("<<new_num_buckets<<")"<<endl;
	this->reallocate_buckets_if_necessary(new_num_buckets);

    //get pointer to source data
	//if (this->i_start == NULL) //Always re-read the pointers... they might be lost after some ITK-process for example
	//retrieve pointers to image data, iterating pointers are generally a bad idea
    //but this way histograms can be made straight from data pointers when
    //there is not yet an image, such as during load of raw files

	this->readytorender=(this->images[0]->begin().pointer() != NULL);

    if(this->readytorender){
		//cout<<"this->readytorender=1"<<endl;
		this->recalc_min_max_data();
		this->refill_bucket_data();
		this->data_has_changed();
	}else{
		//cout<<"this->readytorender=0"<<endl;
        //no calculation was done, set sensible values
        this->max(std::numeric_limits<ELEMTYPE>::max()); 
        this->min(std::numeric_limits<ELEMTYPE>::min());
	}
}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE >::print_histogram_info()
{
	cout<<"min="<<this->min()<<endl;
	cout<<"get_scalefactor()="<<this->get_scalefactor()<<endl;
	this->bucket_vector->print_pts_vector_info();
}



template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::recalc_min_max_data()
{
    this->i_start = this->images[0]->begin().pointer();
    this->i_end = this->images[0]->end().pointer();
    this->readytorender=(this->i_start != NULL);

    if(this->readytorender){
	    this->max_value = std::numeric_limits<ELEMTYPE>::min(); //!set initial values to opposite, simplifies the algorithm
		this->min_value = std::numeric_limits<ELEMTYPE>::max();

        ELEMTYPE *voxel;
		for(voxel = this->i_start; voxel != this->i_end; ++voxel) //Currently, this needs do be done prior to the filling of the buckets...
		{
			this->min_value = std::min(this->min_value,*voxel);
			this->max_value = std::max(this->max_value,*voxel);
		}

	}else{
        //no calculation was done, set sensible values
        this->max_value = std::numeric_limits<ELEMTYPE>::max(); 
        this->min_value = std::numeric_limits<ELEMTYPE>::min();
    }
	(this->bucket_vector)->config_x_axis(this->get_scalefactor(), this->min()); //XXX testing
}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::refill_bucket_data()
{
    this->i_start = this->images[0]->begin().pointer();
    this->i_end = this->images[0]->end().pointer();
    this->readytorender=(this->i_start != NULL);

    if(this->readytorender){
        this->fill(0);//reset buckets
        unsigned short bucketpos;
        ELEMTYPE *voxel;
		for(voxel = this->i_start; voxel != this->i_end; ++voxel)
		{
			bucketpos = intensity_to_bucketpos(*voxel);
			if(bucketpos<this->num_buckets){				//NOT VERY good to write outside allocated memory
				//this->buckets[bucketpos]++;
				this->bucket_vector->at(bucketpos)++;
			}else{
				pt_error::error("histogram_1D<ELEMTYPE >::calculate - bucketpos out of range",pt_error::debug);
				cout<<" histogram_1D<ELEMTYPE >::calculate - bucketpos out of range... bucketpos="<<bucketpos<<endl;
			}
		}

	}else{
        //no calculation was done, set sensible values
        this->max_value = std::numeric_limits<ELEMTYPE>::max(); 
        this->min_value = std::numeric_limits<ELEMTYPE>::min();
    }
	this->bucket_vector->config_x_axis(this->get_scalefactor(), this->min());
}

/*
template <>
void histogram_1D< complex<float> >::recalc_min_max_data()
{
	cout<<"hej2"<<endl;
}
*/

template <class ELEMTYPE>
template <class E, int D>
void histogram_1D<ELEMTYPE>::calculate_from_image_complex(image_complex<E,D> *im)
{
	this->reallocate_buckets_if_necessary();

	this->readytorender=(this->images[0]->begin().pointer() != NULL);

    if(this->readytorender){
		//this->recalc_min_max_data();
	    this->max_value = std::numeric_limits<ELEMTYPE>::min(); //!set initial values to opposite, simplifies the algorithm
		this->min_value = std::numeric_limits<ELEMTYPE>::max();

		E val;
		for(int z=0;z<im->nz();z++){
			for(int y=0;y<im->ny();y++){
				for(int x=0;x<im->nx();x++){
					val = im->get_voxel_magn(x,y,z);
					if(val>this->max_value){
						this->max_value = val;
					}
					if(val<this->min_value){
						this->min_value = val;
					}
				}
			}
		}

//		this->refill_bucket_data();
//		this->data_has_changed();
	}else{
        //no calculation was done, set sensible values
        this->max_value = std::numeric_limits<ELEMTYPE>::max(); 
        this->min_value = std::numeric_limits<ELEMTYPE>::min();
	}
}


template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::render(unsigned char * image, unsigned int width, unsigned int height)
{
    //about FLTK pixel types:  // http://www.fltk.org/articles.php?L466
	typedef IMGELEMCOMPTYPE RGBpixel[RGB_pixmap_bpp];
    RGBpixel * pixels = reinterpret_cast<RGBpixel *>(image);
    
    if (this->bucket_vector != NULL && this->bucket_max > 0){
        for (unsigned int x = 0; x < width; x ++){
            unsigned int b = height-(this->bucket_vector->at((x*this->num_buckets)/width) * height)/this->bucket_max;
            unsigned int y;
            for (y = 0; y < b; y ++){                //background
                pixels [x+y*width][RADDR] = pixels [x+y*width][GADDR] = pixels [x+y*width][BADDR] = 0;
                }
            for (y = b; y < height; y ++){            //white
                pixels [x+y*width][RADDR] = pixels [x+y*width][GADDR] = pixels [x+y*width][BADDR] = 255;
                }
		}
	}else{ //empty histogram
        for (unsigned short p = 0; p < width * height; p ++){
            pixels [p][RADDR] = pixels [p][GADDR] = pixels [p][BADDR] = 0;
		}
	}
}

template <class ELEMTYPE>
image_storage<ELEMTYPE> * histogram_1D<ELEMTYPE>::image ()
    {
    return (this->images[0]); //will be NULL if historam uses data pointers
    }

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::save_histogram_to_txt_file(std::string filepath, gaussian *g, bool reload_hist_from_image, std::string separator, bool only_nonzero_values){
	vector<gaussian> v;
	if(g!=NULL){
		v.push_back(*g);
	}
	save_histogram_to_txt_file(filepath, v, reload_hist_from_image, separator, only_nonzero_values);
	}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::save_histogram_to_txt_file(std::string filepath, vector<gaussian> v, bool reload_hist_from_image, std::string separator, bool only_nonzero_values){
//		cout<<"save_histogram_to_txt_file(std::string file...)"<<endl;

		if(reload_hist_from_image){
			this->images[0]->min_max_refresh(); //make sure to update image min and max before calling calculate() 
			calculate_from_image_data(); //update histogram statistics...
		}

		ofstream myfile;
		myfile.open(filepath.c_str());
		myfile<<"num_buckets"<<separator<<this->num_buckets<<"\n";
		myfile<<"min_value"<<separator<<this->min()<<"\n";
		myfile<<"max_value"<<separator<<this->max()<<"\n";
		myfile<<"num_distinct_values"<<separator<<this->num_distinct_values<<"\n";
		myfile<<"num_elements_in_hist"<<separator<<this->num_elements_in_hist<<"\n";
		myfile<<"bucket_max"<<separator<<this->bucket_max<<"\n";
		myfile<<"bucket_mean"<<separator<<this->bucket_mean<<"\n";
		myfile<<"readytorender"<<separator<<this->readytorender<<"\n";
		myfile<<"typeMin"<<separator<<std::numeric_limits<ELEMTYPE>::min()<<"\n";
		myfile<<"typeMax"<<separator<<std::numeric_limits<ELEMTYPE>::max()<<"\n";
		myfile<<"p_log_p cost"<<separator<<get_norm_p_log_p_cost()<<"\n";
		myfile<<"\n";

		for(int i=0;i<v.size();i++){
			myfile<<"v[i].amplitude="<<separator<<v[i].amplitude<<"\n";
			myfile<<"v[i].center="<<separator<<v[i].center<<"\n";
			myfile<<"v[i].sigma="<<separator<<v[i].sigma<<"\n";
		}

		myfile<<"bucket"<<separator<<"intensity"<<separator<<"bucketvalue\n";
		bool print_line = true;
		for(unsigned short i=0; i<this->num_buckets; i++){
			print_line = true;
			if(only_nonzero_values && this->bucket_vector->at(i)==0){
				print_line = false;
			}
			if(print_line){
				myfile<<i<<separator<<bucketpos_to_intensity(i)<<separator<<this->bucket_vector->at(i);
				//myfile<<separator<<get_norm_frequency_in_bucket(i)<<separator<<get_norm_p_log_p_frequency_in_bucket(i);
				//myfile<<separator<<get_norm_p_log_p_gradient(i);
				for(int g=0;g<v.size();g++){
					myfile<<separator<<v[g].evaluate_at(bucketpos_to_intensity(i));
				}
				myfile<<"\n";
			}
		}

		myfile.close();
	}


template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::save_histogram_to_tif_file(std::string filepath_base, int ny, gaussian *g)
{
	int nx = this->get_num_buckets();
//	int ny=500;
	float y_scale = float(ny) / (this->get_bucket_max()+1); //"rendered pixels per histogram height"

	image_scalar<unsigned char,3> *im = new image_scalar<unsigned char,3>(nx,ny,1);
	im->fill(255);
	
	float this_y;

	for(int i=0;i<nx;i++){
		this_y = this->bucket_vector->at(i);
		im->set_voxel(i, (im->ny()-1) - this_y*y_scale, 0, 0);
		
		if(g != NULL){
			this_y = g->evaluate_at( this->bucketpos_to_intensity(i) );
			im->set_voxel(i, (im->ny()-1) - this_y*y_scale, 0, 100);
		}
		
	}
	
	im->save_to_TIF_file_series_3D(filepath_base, 2, 0, 0);
	delete im;
}


template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::bucketpos_to_intensity(int bucketpos){
//	return this->min() + float(bucketpos)*this->get_scalefactor();
	float mi = this->min();
	float sf = this->get_scalefactor();
	return mi + float(bucketpos)*sf;
}

template <class ELEMTYPE>
int histogram_1D<ELEMTYPE>::intensity_to_bucketpos(ELEMTYPE intensity){
	if( intensity <= this->min() ){
		return 0;
	}
	else if( intensity > this->max() ){
		return this->num_buckets-1;
	}

	return (intensity - this->min())/this->get_scalefactor();	//jk-complex
}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::add_histogram_data(histogram_1D<ELEMTYPE> *hist2){
	if(this->num_buckets == hist2->num_buckets){
		for(int i=0;i<this->num_buckets;i++){
//			this->bucket_vector->at(i) += hist2->buckets[i];
			this->bucket_vector->at(i) += hist2->bucket_vector->at(i);
		}
	}else{
		pt_error::error("histogram_1D - add_histogram_data - not same size...",pt_error::debug);
	}
	this->bucket_vector->config_x_axis(this->get_scalefactor(), this->min());
}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::clear_zero_intentisty_bucket(){
	int zero_bucket_pos = this->intensity_to_bucketpos(0);
	if(zero_bucket_pos>=0 && zero_bucket_pos<this->num_buckets){
		//this->buckets[zero_bucket_pos] = 0;
		this->bucket_vector->at(zero_bucket_pos) = 0;
	}
	this->bucket_vector->config_x_axis(this->get_scalefactor(), this->min());
}


template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::set_sum_of_bucket_contents_to_value(double value){ //often requires the ELEMTYPEs float or double.
	double scale_factor = value / double(this->num_elements_in_hist);
	for(int i=0;i<this->num_buckets;i++){
		//this->buckets[i] *= scale_factor;
		this->bucket_vector->at(i) *= scale_factor;
	}
	this->bucket_vector->config_x_axis(this->get_scalefactor(), this->min());
}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::logarithm(int zero_handling)
{
	// zero_handling=0: non-positive voxels are left unchanged
	// zero_handling=1: non-positive voxels are set to zero
	// zero_handling=2: non-positive voxels are set to std::numeric_limits<ELEMTYPE>::min()
	if (zero_handling<0 || zero_handling>2)
	{zero_handling=0;}

	for(int i=0;i<this->num_buckets;i++){
		if (this->bucket_vector->at(i)>0)
			{this->bucket_vector->at(i) = log(double(this->bucket_vector->at(i)));}
		else if (zero_handling==1) 
			{this->bucket_vector->at(i) = 0;}
		else if (zero_handling==2) 
			{this->bucket_vector->at(i) = std::numeric_limits<ELEMTYPE>::min();}
	}
	this->bucket_vector->config_x_axis(this->get_scalefactor(), this->min());
}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::smooth_mean(int nr_of_neighbours, int nr_of_times){
	smooth_mean(nr_of_neighbours, nr_of_times, 0, this->num_buckets-1);
}

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::smooth_mean(int nr_of_neighbours, int nr_of_times, int from_bucket, int to_bucket){
//	cout<<"smooth....("<<nr_of_neighbours<<","<<nr_of_times<<")...";

	to_bucket = std::min((unsigned short)to_bucket, this->num_buckets);

	//this variable is filled in the smoothing loop to not affect the smooth input values...
	//the "res-results" are then copied to the buckets[] variable for each iteration...	

	unsigned long * res = new unsigned long[this->num_buckets];
	for(int i=0;i<this->num_buckets;i++){
		res[i] = this->bucket_vector->at(i);
	}

	if(nr_of_neighbours>=3 && to_bucket > (from_bucket + 3)){
		ELEMTYPE tmp=0;
		int half = int(float(nr_of_neighbours)/2.0);
		int start_filt	= from_bucket + half;
		int end_filt	= to_bucket - (nr_of_neighbours-half);
		int start_sum	= - half;
		int end_sum		= nr_of_neighbours - half;
/*		cout<<" half="<<half;
		cout<<" start_filt="<<start_filt;
		cout<<" end_filt="<<end_filt;
		cout<<" start_sum="<<start_sum;
		cout<<" end_sum="<<end_sum<<endl;
		cout<<"this->num_buckets="<<this->num_buckets<<endl;
*/
		for(int nr=1;nr<=nr_of_times;nr++){
			for(int i=start_filt;i<end_filt;i++){
				tmp=0;
				for(int j=start_sum;j<end_sum;j++){
					tmp += this->bucket_vector->at(i+j);
				}
//				cout<<"i="<<i<<" res1="<<res[i];
				res[i] = float(tmp)/float(nr_of_neighbours);
//				cout<<" res2="<<res[i]<<endl;
//				this->buckets[i] = int(float(tmp)/float(nr_of_neighbours));
			}
			for(int i=0;i<this->num_buckets;i++){
//				cout<<"buckets[i]="<<buckets[i]<<" res[i]="<<res[i]<<endl;
				this->bucket_vector->at(i) = res[i];
			}
		}
		//the start and the end have to be smoothed with a smaller filter....

		if(nr_of_neighbours>5){
//			smooth(5, nr_of_times, from, start_filt+5);
//			smooth(5, nr_of_times, end_filt, to);
		}
	}
	this->bucket_vector->config_x_axis(this->get_scalefactor(), this->min());
	delete res;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_norm_frequency_in_bucket(int bucket){
	return float(this->bucket_vector->at(bucket))/float(this->num_elements_in_hist);
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_norm_frequency_for_intensity(ELEMTYPE intensity){
	return float(this->bucket_vector->at(this->intensity_to_bucketpos(intensity)))/float(this->num_elements_in_hist);
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_norm_p_log_p_frequency_in_bucket(int bucket, ZERO_HANDLING_TYPE zht){
	float p = get_norm_frequency_in_bucket(bucket);
	return p*pt_log<ELEMTYPE>(p,zht);
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_norm_p_log_p_frequency_for_intensity(ELEMTYPE intensity, ZERO_HANDLING_TYPE zht){
	float p = get_norm_frequency_for_intensity(intensity);
	return p*pt_log<ELEMTYPE>(p,zht);
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_norm_p_log_p_cost(ZERO_HANDLING_TYPE zht){
	float cost=0;
	for(unsigned short i=0; i<this->num_buckets; i++)
	{
		cost += get_norm_p_log_p_frequency_in_bucket(i,zht);
	}
	return cost;
}


template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_norm_p_log_p_gradient(int bucket, ZERO_HANDLING_TYPE zht){
	//corresponds to 1D sobel filter  (-0.5*a + 0*b +0.5*c)
	if(bucket>0 && bucket<this->num_buckets-1)
	{
		return 0.5*get_norm_p_log_p_frequency_in_bucket(bucket+1,zht) - 0.5*get_norm_p_log_p_frequency_in_bucket(bucket-1,zht) ;
	}
	return 0;
}


template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_norm_p_log_p_gradient_for_intensity(ELEMTYPE intensity, ZERO_HANDLING_TYPE zht){
	return get_norm_p_log_p_gradient(this->intensity_to_bucketpos(intensity),zht);
}




template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_bucket_at_histogram_lower_percentile(float percentile, bool ignore_zero_intensity)
{
//    cout<<"get_bucket_at_histogram_lower_percentile("<<percentile<<")"<<endl;
	//if histogram comes from masked region... following line wont work....
	//float num_elem_limit = float(this->images[0]->get_num_elements())*percentile;

	int the_zero_bucket = this->intensity_to_bucketpos(0);

	float num_elem_limit=0;
	if( ignore_zero_intensity && (the_zero_bucket>=0) && (the_zero_bucket<this->num_buckets) ){
		num_elem_limit = float(this->num_elements_in_hist - this->bucket_vector->at(the_zero_bucket) )*percentile;
	}else{
		num_elem_limit = float(this->num_elements_in_hist)*percentile;
	}

	float sum_elements=0;
	if(ignore_zero_intensity){
		for(unsigned short i=0; i<this->num_buckets; i++){
			if(i!=the_zero_bucket){
				sum_elements += this->bucket_vector->at(i);
				if(sum_elements>=num_elem_limit){
					return i;
				}
			}
		}
	}else{
		for(unsigned short i=0; i<this->num_buckets; i++){
			sum_elements += this->bucket_vector->at(i);
			if(sum_elements>=num_elem_limit){
				return i;
			}
		}
	}

	return this->num_buckets-1;
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_intensity_at_histogram_lower_percentile(float percentile, bool ignore_zero_intensity)
{
//    cout<<"get_intensity_at_histogram_lower_percentile("<<percentile<<")"<<endl;
	return bucketpos_to_intensity( get_bucket_at_histogram_lower_percentile(percentile,ignore_zero_intensity) );
}


template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_bucket_at_histogram_higher_percentile(float percentile, bool ignore_zero_intensity)
{
//    cout<<"get_bucket_at_histogram_higher_percentile("<<percentile<<")"<<endl;
	//if histogram comes from masked region... following line wont work....
	//float num_elem_limit = float(this->images[0]->get_num_elements())*percentile;

	int the_zero_bucket = this->intensity_to_bucketpos(0);

	float num_elem_limit=0;
	if( ignore_zero_intensity && (the_zero_bucket>=0) && (the_zero_bucket<this->num_buckets) ){//the "zero_bucket" might be missing (the histogram might for example be created from a masked region...)
		num_elem_limit = float(this->num_elements_in_hist - this->bucket_vector->at(the_zero_bucket))*percentile;
	}else{
		num_elem_limit = float(this->num_elements_in_hist)*percentile;
	}

	float sum_elements=0;

	if(ignore_zero_intensity){
		for(short i = this->num_buckets-1; i>=0; i--){
			if(i!=the_zero_bucket) {
//				sum_elements += this->buckets[i];
				sum_elements += this->bucket_vector->at(i);
				if(sum_elements>=num_elem_limit){
					return i;
				}
			}
		}
	}else{
		for(short i = this->num_buckets-1; i>=0; i--){
			sum_elements += this->bucket_vector->at(i);
			if(sum_elements>=num_elem_limit){
				return i;
			}
		}
	}

	return 0;
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_intensity_at_histogram_higher_percentile(float percentile, bool ignore_zero_intensity)
{
//    cout<<"get_intensity_at_histogram_higher_percentile("<<percentile<<")"<<endl;
	return bucketpos_to_intensity( get_bucket_at_histogram_higher_percentile(percentile,ignore_zero_intensity) );
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_intensity_at_included_num_pix_from_lower_int(ELEMTYPE lower_int, float num_pix)
{
//    cout<<"get_intensity_at_included_num_pix_from_lower_int("<<lower_int<<","<<num_pix<<")"<<endl;

	unsigned short start_bucket = this->intensity_to_bucketpos(lower_int);
    cout<<"start_bucket="<<start_bucket<<endl;
	float sum_elements=0;

	for (unsigned short i = start_bucket; i < this->num_buckets; i++){
		sum_elements += this->bucket_vector->at(i);
		if(sum_elements>=num_pix){
			return bucketpos_to_intensity(i);
		}
	}

	return bucketpos_to_intensity(this->num_buckets-1);
}

template <class ELEMTYPE>
bool histogram_1D<ELEMTYPE>::is_central_histogram_bimodal(int min_mode_sep, double valley_factor, ELEMTYPE peak_min, int speedup) //looks in the central 50% of intensities
{
	int start = get_bucket_at_histogram_lower_percentile(0.25, true);
	int end = get_bucket_at_histogram_lower_percentile(0.75, true);

	return is_histogram_bimodal(start, end, min_mode_sep, valley_factor, peak_min, speedup);
}
		 
template <class ELEMTYPE>
bool histogram_1D<ELEMTYPE>::is_histogram_bimodal(int start_bucket, int end_bucket, int min_mode_sep, double valley_factor, ELEMTYPE peak_min, int speedup){
	cout<<"isHistogramBimodal..."<<endl;
	this->smooth_mean(5,1,start_bucket,end_bucket);

	bool ret = false;
	ELEMTYPE peak_1 = 0;
	ELEMTYPE peak_2 = 0;
	ELEMTYPE min_peak = 0;
	ELEMTYPE valley = 0;
	for(int i=start_bucket+1; i<=end_bucket-min_mode_sep; i = i+speedup){
		for(int j=i+min_mode_sep; j<end_bucket; j = j+speedup){
			peak_1 = this->get_max_value_in_bucket_range(start_bucket,i);
			peak_2 = this->get_max_value_in_bucket_range(j,end_bucket);
			valley = this->get_min_value_in_bucket_range(i,j);

			min_peak = std::min(peak_1,peak_2);

			if(min_peak > peak_min && valley < valley_factor*double(min_peak)){
				cout<<"start_bucket\ti\tsep\tj\tend_bucket\tpeak_1\tpeak_2\tvalley"<<endl;
				cout<<start_bucket<<"\t"<<i<<"\t"<<min_mode_sep<<"\t"<<j<<"\t";
				cout<<end_bucket<<"\t"<<peak_1<<"\t"<<peak_2<<"\t"<<valley<<endl;
				cout<<"***TRUE***"<<endl;
				return true;
			}

		}
	}
	cout<<"***FALSE***"<<endl;
	return false;
}



template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::fit_gaussian_to_intensity_range(float &amp, float &center, float &sigma, ELEMTYPE from_int, ELEMTYPE to_int, bool print_info)
{
	this->bucket_vector->fit_gaussian_to_intensity_range(amp, center, sigma, from_int, to_int, print_info);
}



template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::find_better_amplitude(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	return this->bucket_vector->find_better_amplitude(g,from_bucket, to_bucket, factor1, factor2, nr_steps);
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::find_better_center(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	return this->bucket_vector->find_better_center(g, from_bucket, to_bucket, factor1, factor2, nr_steps);
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::find_better_sigma(gaussian g, int from_bucket, int to_bucket, float factor1, float factor2, int nr_steps)
{
	return this->bucket_vector->find_better_sigma(g,from_bucket, to_bucket, factor1, factor2, nr_steps);
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_sum_square_diff_between_buckets(vector<gaussian> v, int from_bucket, int to_bucket, bool ignore_zeros, bool weight_with_hist_freq){
	
	return this->bucket_vector->get_sum_square_diff_between_buckets(v, from_bucket, to_bucket, ignore_zeros, weight_with_hist_freq);
}


template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_sum_square_diff_between_buckets(gaussian g, int from_bucket, int to_bucket, bool ignore_zeros){
	vector<gaussian> v;
	v.push_back(g);
	return get_sum_square_diff_between_buckets(v,from_bucket,to_bucket);
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_sum_square_diff(vector<gaussian> v, bool ignore_zeros, bool weight_with_hist_freq){
	return get_sum_square_diff_between_buckets(v,0,this->num_buckets-1, ignore_zeros, weight_with_hist_freq);
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_sum_square_diff(gaussian g, bool ignore_zeros){
	return this->bucket_vector->get_sum_square_diff(g,ignore_zeros);
}


template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_gaussian_area(gaussian g, int from_bucket, int to_bucket)
{
	return this->bucket_vector->get_gaussian_area(g,from_bucket, to_bucket);
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_gaussian_area(gaussian g)
{
	return get_gaussian_area(g,0,this->num_buckets-1);
}

template <class ELEMTYPE>
vector<double> histogram_1D<ELEMTYPE>::get_gaussian_areas(vector<gaussian> v)
{
	return this->bucket_vector->get_gaussian_areas(v);
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_sum_square_gaussian_overlap(vector<gaussian> v, int from_bucket, int to_bucket)
{
	return this->bucket_vector->get_sum_square_gaussian_overlap(v, from_bucket, to_bucket);
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_sum_square_gaussian_overlap(vector<gaussian> v)
{
	return get_sum_square_gaussian_overlap(v,0,this->num_buckets-1);
}

template <class ELEMTYPE>
vector<double> histogram_1D<ELEMTYPE>::get_overlaps_in_percent(vector<gaussian> v)
{
	return this->bucket_vector->get_overlaps_in_percent(v);
}




template <class ELEMTYPE>
vnl_vector<double> histogram_1D<ELEMTYPE>::get_vnl_vector_with_start_guess_of_num_gaussians(int num_gaussians){
	
	return this->bucket_vector->get_vnl_vector_with_start_guess_of_num_gaussians(num_gaussians);
}


template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::fit_two_gaussians_to_histogram_and_return_threshold(float punish_overlap, float punish_area_differences, float punish_variance_differences, bool weight_with_hist_freq, string save_histogram_file_path)
{
	this->print_histogram_info();

	vnl_vector<double> x = this->bucket_vector->get_vnl_vector_with_start_guess_of_num_gaussians(2);

	//the values in x are now in index
	this->bucket_vector->fit_two_gaussians_to_histogram(x, punish_overlap, punish_area_differences, punish_variance_differences, weight_with_hist_freq);


	if(save_histogram_file_path != ""){
		vector<gaussian> v;
		for(int i=0; i<2; i++) {
			v.push_back( gaussian(x[i*3+0],x[i*3+1],x[i*3+2]) );
		}
		this->save_histogram_to_txt_file(save_histogram_file_path,v);
	}

//	return x[1] + 3*x[2]; //pos + 2*SD
	gaussian g = gaussian(x[0],x[1],x[2]);
	gaussian g2 = gaussian(x[3],x[4],x[5]);
//	int index = g.get_value_at_intersection_between_centers(g2);
//	return this->bucketpos_to_intensity(index);
	return g.get_value_at_intersection_between_centers(g2);
}

template <class ELEMTYPE>
vnl_vector<double> histogram_1D<ELEMTYPE>::fit_n_gaussians_to_histogram(int n, string save_histogram_file_path)
{
	unsigned short t = this->get_bucket_at_histogram_higher_percentile(0.03,true);

	for( int i =t; i<  this->bucket_vector->size(); ++i)
	//	this->bucket_vector->pop_back();
		this->bucket_vector->at(i) = 0;

//	this->resize_to_fit_data();
	vnl_vector<double> x = this->bucket_vector->fit_n_gaussians_to_histogram(n, save_histogram_file_path);


	if(save_histogram_file_path != ""){

		vector<gaussian> v;
		for(int i=0; i<n; i++) {
			v.push_back( gaussian(this->bucketpos_to_intensity(x[i*3+0]),this->bucketpos_to_intensity(x[i*3+1]),this->bucketpos_to_intensity(x[i*3+2])) );
		}

		this->save_histogram_to_txt_file(save_histogram_file_path,v);
//		this->save_histogram_to_txt_file(


	

	}

	return x;
}
	
//-----------------------------

template <class ELEMTYPE>
void histogram_1D<ELEMTYPE>::fit_rayleigh_distr_to_intensity_range(float &amp, float &sigma, ELEMTYPE from_int, ELEMTYPE to_int, bool print_info)
{
	this->bucket_vector->fit_rayleigh_distr_to_intensity_range(amp, sigma,from_int, to_int, print_info);
}

		
template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_sum_square_diff(rayleighian r, bool ignore_zeros){
	return get_sum_square_diff_from_buckets(r, 0, this->num_buckets, ignore_zeros);
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_sum_square_diff_from_buckets(rayleighian r, int from_bucket, int to_bucket, bool ignore_zeros){
	return this->bucket_vector->get_sum_square_diff_from_buckets(r, from_bucket, to_bucket, ignore_zeros);
}



template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_min_value_in_bucket_range(int from, int to)
{
	int pos=0;
	return get_min_value_in_bucket_range(from, to, pos);
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_min_value_in_bucket_range(int from, int to, int &min_val_bucket_pos)
{
	return this->bucket_vector->get_minimum_in_range(from, to,min_val_bucket_pos);
}



template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_max_value_in_bucket_range(int from, int to)
{
	int pos=0;
	return get_max_value_in_bucket_range(from, to, pos);
}

template <class ELEMTYPE>
ELEMTYPE histogram_1D<ELEMTYPE>::get_max_value_in_bucket_range(int from, int to, int &max_val_bucket_pos)
{
	return this->bucket_vector->get_maximum_in_range(from, to, max_val_bucket_pos);
}

template <class ELEMTYPE>
double histogram_1D<ELEMTYPE>::get_max_value_in_bucket_range_using_averaging(int from, int to, int mean_nbh, int &max_val_bucket_pos)
{
	return this->bucket_vector->get_maximum_value_in_range_using_averaging(from,to,mean_nbh,max_val_bucket_pos);
}


template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_mean_intensity_in_bucket_range(int from, int to, bool exclude_zero_int_bucket)
{
//	return this->bucket_vector->get_mean_in_range(from, to);
	double sum = 0;
	if(exclude_zero_int_bucket){
		int zero_bucket_pos = this->intensity_to_bucketpos(0);
		for(int i=from; i<=to; i++){
			if(i != zero_bucket_pos){
				sum += this->bucket_vector->at(i) * this->bucketpos_to_intensity(i);
			}
		}
		
		sum = sum/float( this->num_elements_in_hist - this->bucket_vector->at(zero_bucket_pos) );

	}else{
		float at;
		float inten;

		for(int i=from; i<=to; i++){
//			sum += this->bucket_vector->at(i) * this->bucketpos_to_intensity(i);
			at = this->bucket_vector->at(i);
			inten = this->bucketpos_to_intensity(i);
			sum += at*inten;
		}

		sum = sum/float(this->num_elements_in_hist);
	}

	return sum;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_mean_intensity(bool exclude_zero)
{	
	return this->get_mean_intensity_in_bucket_range(0,this->bucket_vector->size()-1,exclude_zero);
//	return this->bucket_vector->get_weighted_mean_intensity();
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_hist_mean() {
	float mean = 0;
	int total = 0;
	for(int i = 0; i<this->num_buckets;i++) 	
		total += this->bucket_vector->at(i);
	for(int i = 0; i<this->num_buckets;i++) 
		mean  += float(bucketpos_to_intensity(i))*float(this->bucket_vector->at(i))/float(total);
	return mean;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_hist_variance() {
	float mean = 0;
	int total = 0;
	float variance = 0;
	for(int i = 0; i<this->num_buckets;i++) 	
		total += this->bucket_vector->at(i);
	for(int i = 0; i<this->num_buckets;i++) 
		mean  += float(bucketpos_to_intensity(i))*float(this->bucket_vector->at(i))/float(total);
	for(int i = 0; i<this->num_buckets;i++) 
		if(bucketpos_to_intensity(i)!=0)
			variance += pow(float(bucketpos_to_intensity(i)) - mean, 2) *float(this->bucket_vector->at(i))/float(total);
	
	return variance;
}

template <class ELEMTYPE>
float histogram_1D<ELEMTYPE>::get_variance_in_intensity_range(ELEMTYPE from, ELEMTYPE to)
{
	return this->bucket_vector->get_variance_in_range(intensity_to_bucketpos(from),intensity_to_bucketpos(to));
}

template <class ELEMTYPE>
int histogram_1D<ELEMTYPE>::get_bucket_pos_with_largest_value_in_bucket_range(int from, int to)
{
	int pos=0;
	(this->bucket_vector)->get_maximum_value_in_range(from, to, pos);
	return pos;
}


template <class ELEMTYPE>
int histogram_1D<ELEMTYPE>::get_bucket_pos_with_largest_value_in_intensity_range(ELEMTYPE from, ELEMTYPE to)
{
	int pos=0;
	(this->bucket_vector)->get_maximum_in_range(intensity_to_bucketpos(from), intensity_to_bucketpos(to), pos);
	return pos;
}

/*
template<class ELEMTYPE, int IMAGEDIM>
histogram_2Dimage<ELEMTYPE,IMAGEDIM>::histogram_2Dimage<ELEMTYPE,IMAGEDIM>(image_scalar<ELEMTYPE,IMAGEDIM>* i1, image_scalar<ELEMTYPE,IMAGEDIM>* i2)
{
	this->im1 = new image_scalar<ELEMTYPE,IMAGEDIM>(i1,1);
	this->im2 = new image_scalar<ELEMTYPE,IMAGEDIM>(i2,1);
}
*/