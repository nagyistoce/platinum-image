// $Id$

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

#define TFUNCTIONTEST transfer_default

#ifndef __image_storage_hxx__
#define __image_storage_hxx__

#include "image_storage.h"

#include "transfer.hxx"
#include "histogram.hxx"
#include "transfer_interpolated.hxx"
#include "image_storage_iterator.hxx"

template <class fromType, class toType>
void copy_data(image_storage<fromType > * const in, image_storage<toType > * out) //!General data copying
    {
	//MSVC 2005 no like, See following discussion...	//http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=259&rl=1
    //std::copy (in->begin(),in->end(),out->begin());
    typename image_storage<fromType >::iterator i = in->begin();
    typename image_storage<toType >::iterator   o = out->begin();
    while (i != in->end() && o != out->end()){
	    *o = *i;
		++i; ++o;
    }

    pt_error::error_if_false(i == in->end() && o == out->end(),"Image sizes didn't match when copying data",pt_error::serious);
    }

template <class T, class toType>
void copy_data(image_storage<complex<T> > * const in, image_storage<toType > * out) //!General data copying
    {
	//MSVC 2005 no like, See following discussion...	//http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=259&rl=1
    //std::copy (in->begin(),in->end(),out->begin());
    typename image_storage<complex<T> >::iterator i = in->begin();
    typename image_storage<toType >::iterator   o = out->begin();
    while (i != in->end() && o != out->end()){
	    *o = abs(*i); //from complex<float> --> "toType"
		++i; ++o;
    }

    pt_error::error_if_false(i == in->end() && o == out->end(),"Image sizes didn't match when copying data",pt_error::serious);
    }

//template <class fromType>
//void copy_data( image_storage<fromType > * in,image_storage<IMGBINARYTYPE > * out) //!Data copy specialized for copying *to* boolean
//    {
//    typename image_storage<fromType >::iterator i = in->begin();
//    typename image_storage<IMGBINARYTYPE >::iterator   o = out->begin();
//
//    while (i != in->end() && o != out->end())
//        {
//        *o = (*i == true);
//
//        ++i; ++o;
//        }
//
//    /*if (!(i == in->end() && o == out->end()) )
//    {
//    throw ("Image sizes didn't match when copying boolean data");
//    }*/
//    }

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::set_parameters()
    {
    dataptr = NULL;
    //stats = NULL;
    //tfunction = NULL; 

	//set_stats_histogram (new histogram_1D<ELEMTYPE >(this));  //hist1D constructor calls resize()... and calculate()
	//transfer_function();  //set default transfer function
	
	//stats->min(std::numeric_limits<ELEMTYPE>::min());
	//stats->max(std::numeric_limits<ELEMTYPE>::max());
    }

	/*
template <class ELEMTYPE >
void image_storage<ELEMTYPE >::transfer_function(transfer_scalar_base * t)
{
	//tfunction = t;
	int a=0;
}
*/
/*
template <class ELEMTYPE >
void image_storage<ELEMTYPE >::transfer_function(std::string functionName)
    {
//		transfer_scalar_base<ELEMTYPE > *t = transfer_manufactured::factory.Create<ELEMTYPE> (functionName,this);
//		this->transfer_function(t);
		int a=0;
    }
*/

/*
template <class ELEMTYPE >
void image_storage<ELEMTYPE >::set_stats_histogram(histogram_1D<ELEMTYPE > * h)
    {
    if (stats != NULL)
        {
        delete stats;
        }

    stats = h;
    }
*/

template <class ELEMTYPE >
ELEMTYPE * image_storage<ELEMTYPE >::imagepointer()
{
//pt_error::error_if_null (dataptr,"Attempting to access datapointer while it is NULL", pt_error::fatal );
return (dataptr); 
}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::set_imagepointer(ELEMTYPE * new_value)
{
	dataptr = new_value;
}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::deallocate ()
{
	delete dataptr;
}

template <class ELEMTYPE >
image_storage<ELEMTYPE >::image_storage() : image_base ()
    {
    set_parameters();
    }

template <class ELEMTYPE >
template<class SOURCETYPE>
image_storage<ELEMTYPE >::image_storage(image_storage<SOURCETYPE> * const s):image_base (s)
    {
    set_parameters();
    }
/*
template <class ELEMTYPE >
image_storage<ELEMTYPE >::image_storage(const string filepath):image_base(filepath)
{}
*/

template <class ELEMTYPE >
image_storage<ELEMTYPE >::~image_storage()
    {
//    delete tfunction; //öööö

//    if (stats != NULL)
//        { delete stats; }

    if (imagepointer() != NULL)
        { deallocate(); }

    //minvalue=std::numeric_limits<ELEMTYPE>::min();
    //maxvalue=std::numeric_limits<ELEMTYPE>::max();
    }

/*
template <class ELEMTYPE >
ELEMTYPE image_storage<ELEMTYPE >::get_min() const
    {
    return stats->min();
    }
*/
/*
template <> // image_complex testing
float image_storage<std::complex<float> >::get_max_float()
    {
    return abs(maxvalue);
    }
*/

//JK - öööö - I have not managed to specialize this function for "complex<ELEMTYPE>" - I think the whole class needs to be rewritten for "complex<ELEMTYPE>"

template <class ELEMTYPE >
float image_storage<ELEMTYPE >::get_max_float() const
{
	return 0; //JK4 - Function is virtual and result will be delivered by sub-classes
}

template <class ELEMTYPE >
float image_storage<ELEMTYPE >::get_min_float() const
{
	return 0; //JK4 - Function is virtual and result will be delivered by sub-classes
}

/*
template <class ELEMTYPE >
ELEMTYPE image_storage<ELEMTYPE >::get_max() const
    {
    return stats->max();
    }
*/
template <class ELEMTYPE >
float image_storage<ELEMTYPE >::get_mean(image_storage<IMGBINARYTYPE>* mask)
{
	double sum=0;
	int num=0;
	typename image_storage<ELEMTYPE >::iterator itr = this->begin();
	if (mask!=NULL) {
		typename image_storage<IMGBINARYTYPE >::iterator mask_itr = mask->begin();
		while(itr != this->end()) {
			if (*mask_itr) {
				sum+=(double) *itr;
				++num;
			}
			++itr;
			++mask_itr;
		}
		pt_error::error_if_false(itr == this->end() && mask_itr == mask->end(),"Binary mask size didn't match image size when calculating mean",pt_error::serious);
	}else {
		while(itr != this->end()) {
			sum += (double) *itr;
			++num;
			++itr;
		}
	}
	return (float) sum/num;
}

template <class ELEMTYPE >
float image_storage<ELEMTYPE >::get_standard_deviation(image_storage<IMGBINARYTYPE>* mask)
{
	float mean=get_mean(mask);
	double sum=0;
	int num=0;
	typename image_storage<ELEMTYPE >::iterator itr = this->begin();
	if (mask!=NULL) {
		typename image_storage<IMGBINARYTYPE >::iterator mask_itr = mask->begin();
		while(itr != this->end()) {
			if (*mask_itr) {
				sum+=pow((double)*itr-mean, 2);
				++num;
			}
			++itr;
			++mask_itr;
		}
		pt_error::error_if_false(itr == this->end() && mask_itr == mask->end(),"Binary mask size didn't match image size when calculating standard deviation",pt_error::serious);
	}else {
		while(itr != this->end()) {
			sum+=pow((double)*itr-mean, 2);
			++num;
			++itr;
		}
	} 
	return (float) sqrt(sum/num);
}
/*
template <class ELEMTYPE >
ELEMTYPE image_storage<ELEMTYPE >::get_num_values()
{ 
	return stats->num_values(); 
}
*/
/*
template <class ELEMTYPE >
unsigned long image_storage<ELEMTYPE >::get_num_elements()
{
	return num_elements;
}
*/
/*
template <class ELEMTYPE >
histogram_1D<ELEMTYPE> * image_storage<ELEMTYPE >::get_histogram()
{
	pt_error::error_if_null(stats,"Trying to get_histogram() which is NULL");
	return stats;
}
*/
/*
template <class ELEMTYPE >
histogram_1D<ELEMTYPE> *image_storage<ELEMTYPE >::get_histogram_new_with_same_num_buckets_as_intensities()
{
	ELEMTYPE minimum;
	ELEMTYPE maximum;
	this->get_min_max_values(minimum, maximum);
	return new histogram_1D<ELEMTYPE>(this,int(maximum-minimum));
}
*/


template <class ELEMTYPE >
void image_storage<ELEMTYPE >::erase()
{
    memset(imagepointer(), 0, sizeof(ELEMTYPE) * num_elements);
}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::fill(ELEMTYPE value)
{
	//JK3 - Try and see how much time this saves...
//	memset(imagepointer(), value, sizeof(ELEMTYPE) * num_elements);		//was found to occationally give strange values

	typename image_storage<ELEMTYPE>::iterator i = this->begin();
	while (i != this->end())
		{
		*i = value;
		++i;
		}
}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::invert()
	{
	ELEMTYPE max = this->get_max();
	ELEMTYPE min = this->get_min();
	typename image_storage<ELEMTYPE>::iterator i = this->begin();
	while (i != this->end())
		{
		*i = min + max - *i;
		++i;
		}
	}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::add_value_to_all_voxels(ELEMTYPE value, image_storage<IMGBINARYTYPE>* mask)
{
	typename image_storage<ELEMTYPE>::iterator itr = this->begin();
	if (mask!=NULL) {
		typename image_storage<IMGBINARYTYPE>::iterator mask_itr = mask->begin();
		while (itr != this->end()) {
			if (*mask_itr) {*itr += value;}
			++itr;
			++mask_itr;
		}
		pt_error::error_if_false(itr == this->end() && mask_itr == mask->end(),"Binary mask size didn't match image size when adding value to voxels",pt_error::serious);
	}else {
		while (itr != this->end()) {
			*itr += value;
			++itr;
		}
	}
}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::scale(ELEMTYPE new_min, ELEMTYPE new_max)
	{
	this->min_max_refresh();

	if(get_min()==get_max())
		{
		fill(0);
		}
	else
		{
		typename image_storage<ELEMTYPE>::iterator i = this->begin();
		while (i != this->end())
			{
			*i = new_min + (ELEMTYPE) (((*i)-get_min()) * ((new_max-new_min)/(double)(get_max()-get_min())));
			++i;
			}
		}
	}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::scale_by_factor(float factor, ELEMTYPE old_center, ELEMTYPE new_center, image_storage<IMGBINARYTYPE>* mask) {
	typename image_storage<ELEMTYPE>::iterator itr = this->begin();
	if (mask!=NULL) {
		typename image_storage<IMGBINARYTYPE>::iterator mask_itr = mask->begin();
		while (itr != this->end()) {
			if (*mask_itr) {*itr = new_center + ELEMTYPE(float((*itr)-old_center)*factor);}
			++itr;
			++mask_itr;
		}
		pt_error::error_if_false(itr == this->end() && mask_itr == mask->end(),"Binary mask size didn't match image size when scaling by factor",pt_error::serious);
	}else {
		while (itr != this->end()) {
		*itr = new_center + ELEMTYPE(float((*itr)-old_center)*factor);
		++itr;
		}
	}
}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::scale_intervall(ELEMTYPE from_min_val, ELEMTYPE from_max_val, ELEMTYPE to_min_val, ELEMTYPE to_max_val)
{
	typename image_storage<ELEMTYPE>::iterator itr = this->begin();
	ELEMTYPE from_span = from_max_val-from_min_val; 
	ELEMTYPE to_span = to_max_val-to_min_val; 

	while(itr != this->end()) {

		if(*itr < from_min_val){
			*itr = to_min_val;

		}else if(*itr >from_max_val){
			*itr = to_max_val;
		
		}else{
			*itr = to_min_val + ELEMTYPE( ((*itr)-from_min_val)/(from_span)*(to_span) );
		}

		++itr;
	}
}


template <class ELEMTYPE >
void image_storage<ELEMTYPE >::map_values(ELEMTYPE map_from, ELEMTYPE map_to, ELEMTYPE result_value)
	{
	typename image_storage<ELEMTYPE>::iterator i = this->begin();
	while (i != this->end())
		{
		if(*i>=map_from && *i<=map_to){
			*i = result_value;
		}
		++i;
		}
	}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::map_negative_values(ELEMTYPE to_value)
	{
	map_values(std::numeric_limits<ELEMTYPE>::min(), 0, to_value);
	}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::map_values_using_gaussian(gaussian* g)
	{
	typename image_storage<ELEMTYPE>::iterator i = this->begin();
	while (i != this->end())
		{
		*i = g->evaluate_at(*i);
		++i;
		}
	}


template <class ELEMTYPE >
int image_storage<ELEMTYPE >::get_number_of_voxels_with_value(ELEMTYPE val)
	{
		iterator i = this->begin();
		int num=0;
		while(i != this->end())
		{
			if(*i == val){++num;}
			++i;
		}
		return num;
	}

template <class ELEMTYPE >
int image_storage<ELEMTYPE >::get_number_of_voxels_with_value_greater_than(ELEMTYPE val)
	{
		iterator i = this->begin();
		int num=0;
		while(i != this->end())
		{
			if(*i > val){++num;}
			++i;
		}
		return num;
	}


template <class ELEMTYPE >
bool image_storage<ELEMTYPE >::same_size(image_storage<ELEMTYPE> *const image2)
{
	return (this->num_elements == image2->num_elements)?true:false;
}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::combine(image_storage<ELEMTYPE> *const image2, COMBINE_MODE mode)
{
//	cout<<"Combine...";
	if(this->same_size(image2)){
	    typename image_storage<ELEMTYPE>::iterator i = this->begin();
	    typename image_storage<ELEMTYPE>::iterator i2 = image2->begin();

		switch(mode)
        {
        case COMB_ADD:
//			cout<<"...ADD";
			while(i != this->end())
			{
				*i = *i + *i2;
				++i;
				++i2;
			}
            break;

        case COMB_SUB:
//			cout<<"...SUB";
			while(i != this->end())
			{
				*i = *i - *i2;
				++i;
				++i2;
			}
            break;

        case COMB_MULT:
//			cout<<"...MULT";
			while(i != this->end())
			{
				*i = (*i)*(*i2);
				++i;
				++i2;
			}
            break;

        case COMB_DIV:
//			cout<<"...DIV";
			while(i != this->end())
			{
				if(*i2==0){
					*i = 0;
				}else{
					*i = *i / *i2;
				}
				++i;
				++i2;
			}
            break;

        case COMB_MAX:
//			cout<<"...MAX";
			while(i != this->end())
			{
				*i = std::max(*i,*i2);
				++i;
				++i2;
			}
            break;

		case COMB_MIN:
//			cout<<"...MIN";
			while(i != this->end())
			{
				*i = std::min(*i,*i2);
				++i;
				++i2;
			}
            break;

		case COMB_MEAN_NON_ZERO:
//			cout<<"...COMB_MEAN_NON_ZERO";
			while(i != this->end())
			{
				if(*i==0){
					*i=*i2;
				}else if(*i!=0 && *i2!=0){
					*i = 0.5*float(*i + *i2);
				} //if *i2==0 --> Nothing is done...
				++i;
				++i2;
			}
            break;
		case COMB_MAGN:
//			cout<<"...COMB_MAGN";
			while(i != this->end())
			{
				*i = sqrt( float((*i)*(*i) + (*i2)*(*i2)) );
				++i;
				++i2;
			}
            break;
		

		default:
			pt_error::error("image_storage<ELEMTYPE >::combine --> COMBINE_TYPE not recognized",pt_error::debug);
			break;
		}
//		cout<<"...DONE"<<endl;
	}else{
		pt_error::error("image_storage<ELEMTYPE >::combine --> Not same size",pt_error::debug);
	}
}

template <class ELEMTYPE >
string image_storage<ELEMTYPE >::resolve_tooltip()
{
	return resolve_tooltip_image_storage();
}

template <class ELEMTYPE >
string image_storage<ELEMTYPE >::resolve_tooltip_image_storage()
{
//	return resolve_tooltip_image_base() + "num_elem="+int2str(num_elements)+" (size=resolve_size())\n";
//	return resolve_tooltip_image_base() + "num_elem="+int2str(num_elements)+" datatype=" + string(typeid(ELEMTYPE).name()) + " (size=resolve_size())\n";
	string s = resolve_tooltip_image_base() + "datatype=" + string(typeid(ELEMTYPE).name()) + "\n" ;
	s += "min=" + float2str(get_min()) + " max=" + float2str(get_max()) + "\n";
	s += "size=" + resolve_datasize_in_kb()+"kb num_elem=" + int2str(num_elements) + "\n";
	return s;
}

template <class ELEMTYPE >
string image_storage<ELEMTYPE >::resolve_datasize_in_kb()
{
	float f = float(num_elements)*float(numeric_limits<ELEMTYPE>::digits) * 0.001/8;
	return float2str(f);
}
/*
template <>
string image_storage<complex<float> >::resolve_datasize_in_kb()
{
	float f = float(num_elements)*2*float(numeric_limits<float>::digits) * 0.001/8;
	return float2str(f);
}
*/

/*
template <class ELEMTYPE >
void image_storage<ELEMTYPE >::stats_refresh(bool min_max_refresh)
    {
	if(min_max_refresh){
		this->min_max_refresh();
	}
    stats->calculate_from_image_data(); //when called without argument (=0) the histogram "resolution" is kept..


    //TODO:
    //store #distinct values locally 
   
    //don't change if values don't make sense - 
    //that would be an empty/zero image

//    if (stats->min() < stats->max())   {
//        this->maxvalue=stats->max();
//        this->minvalue=stats->min();
//        }


    }
*/
/*
template <class ELEMTYPE >
void image_storage<ELEMTYPE >::min_max_refresh()
{
    ELEMTYPE minimum, maximum;
	get_min_max_values(minimum, maximum);
   
    //don't change if values don't make sense - 
    //that would be an empty/zero image
    if (minimum < maximum)
        {
        this->stats->min(minimum);
        this->stats->max(maximum);
        }
}
*/
template <class ELEMTYPE >
void image_storage<ELEMTYPE >::get_min_max_values(ELEMTYPE &minimum, ELEMTYPE &maximum)
{
//	if(num_elements>0){ //prevents code from being executed on none initiated/allocated images...
		minimum=std::numeric_limits<ELEMTYPE>::max();
		maximum=std::numeric_limits<ELEMTYPE>::min();

		ELEMTYPE val;
		typename image_storage<ELEMTYPE>::iterator itr = this->begin();
		while (itr != this->end())
		{
			val=*itr;		//****If you crash here... the image was likely not loaded correctly...
			minimum = std::min (val, minimum);
			maximum = std::max (val, maximum);
			++itr;
		}
//	}
}



template <class ELEMTYPE >
double image_storage<ELEMTYPE >::get_sum_of_voxels(ELEMTYPE lower_limit, bool calc_scalar_abs_value, image_storage<IMGBINARYTYPE>* mask) 
{
	double sum=0;
	typename image_storage<ELEMTYPE >::iterator itr = this->begin();
	if (mask!=NULL) {
		typename image_storage<IMGBINARYTYPE >::iterator mask_itr = mask->begin();
		if (calc_scalar_abs_value) {
			while(itr != this->end()) {
				if (*mask_itr && *itr>=lower_limit)
					{sum+=abs((double)*itr);}
                ++itr;
				++mask_itr;
			}
		}else {
			while(itr != this->end()) {
				if (*mask_itr && *itr>=lower_limit)
					{sum+=(double)*itr;}
				++itr;
				++mask_itr;
			}
		}
		pt_error::error_if_false(itr == this->end() && mask_itr == mask->end(),"Binary mask size didn't match image size when calculating sum of voxels",pt_error::serious);

	}else {
		if (calc_scalar_abs_value) {
			while(itr != this->end()) {
				if(*itr>lower_limit)
					{sum+=abs((double)*itr);}
				++itr;
			}
		}else {
			while(itr != this->end()) {
				if(*itr>lower_limit)
					{sum+=(double)*itr;}
				++itr;
			}
		}
	}
	return sum;
}

template <class ELEMTYPE >
void image_storage<ELEMTYPE >::set_sum_of_voxels_to_value(double value)
{
	double sum = this->get_sum_of_voxels();
	float factor;
	(sum==0) ? factor=1 : factor= (float) value/sum;
	this->scale_by_factor(factor);
}


#endif
